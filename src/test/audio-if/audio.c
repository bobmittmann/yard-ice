/* 
 * File:	 i2s-slave.c
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(C) 2011 Bob Mittmann. All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */


#include <sys/stm32f.h>
#include <sys/delay.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <hexdump.h>

#include "trace.h"
#include "spectrum.h"
#include "vt100.h"
#include "fixpt.h"
#include "tlv320.h"
#include "sndbuf.h"
#include "tonegen.h"
#include "i2s.h"
#include "i2c.h"
#include "io.h"
#include "telctl.h"
#include "jitbuf.h"
#include "net.h"
#include "audio.h"

#include <sys/dcclog.h>

#include <thinkos.h>
#define __THINKOS_IRQ__
#include <thinkos_irq.h>

#ifndef SAMPLE_RATE 
#define SAMPLE_RATE 8000
#endif

#ifndef ENABLE_G711
#define ENABLE_G711 0
#endif

#ifndef DISABLE_JITBUF
#define DISABLE_JITBUF 0
#endif

//
// 2nd order IIR filter
//
typedef struct FXA_IIR2_TAG
  {
  int16_t A[3];
  int16_t B[3];
  int16_t y[3];
  int16_t x[3];
  } FXA_IIR2;

static inline int16_t FxaIirApply(FXA_IIR2 * pF, int16_t Val) {
  int32_t y;
  // Shift the old samples
  pF->x[2] = pF->x[1];
  pF->x[1] = pF->x[0];
  pF->y[2] = pF->y[1];
  pF->y[1] = pF->y[0];
  // Calculate the new output
  pF->x[0] = Val;
  y = pF->A[0] * pF->x[0];
  y += pF->A[1] * pF->x[1] - pF->B[1] * pF->y[1];
  y += pF->A[2] * pF->x[2] - pF->B[2] * pF->y[2];
  // Scale down
  y *= pF->B[0];
  y /= 32768;
  pF->y[0] = y;
  return y;
  }

FXA_IIR2 iir_hp_240hz =
{
  .A = { 13985, -27971, 13985 },
  .B = { 2, -27693, 11808 },
  .y = { 0, 0, 0 },
  .x = { 0, 0, 0 }
};

FXA_IIR2 iir_hp_120hz =
{
  .A = { 15123, -30246, 15123 },
  .B = { 2, -30164, 13914 },
  .y = { 0, 0, 0 },
  .x = { 0, 0, 0 }
};


int32_t codec_addr = 64;
struct spectrum audio_tx_sa;
struct spectrum audio_rx_sa;
struct tonegen tonegen;

struct {
	bool enabled;
	int io_thread;
	int net_thread;
	int tone_mode;
	int dac_gain;
	volatile bool stream_enabled;
	jitbuf_t jitbuf;
} audio_drv;

int tlv320_wr(unsigned int reg, unsigned int val)
{
	uint8_t buf[1];
	int ret;
	
	buf[0] = val;

	if ((ret = i2c_write(codec_addr, reg, buf, 1)) < 0) {
		tracef("%s(): i2c_write() failed!", __func__);
	}

	return ret;
}

void tlv320_reset(void)
{
	uint8_t tlv[4];

	tracef("%s()...", __func__);

	if (i2c_read(codec_addr, 0, tlv, 4) < 0) {
		tracef("%s(): i2c_read() failed!", __func__);
		return;
	}

	/* reset the device */
	tlv320_wr(3, CR3_PWDN_NO | CR3_SWRS);
	/* wait at least 132MCLK ~ 12us (11.2896 MHz) */

	thinkos_sleep(1);
}	

void tlv320_status(void)
{
	uint8_t tlv[8];

	tracef("%s()...", __func__);

	if (i2c_read(codec_addr, 0, tlv, 8) < 0) {
		tracef("%s(): i2c_read() failed!", __func__);
		return;
	}

	tracef("%s(): 0x%02x 0x%02x 0x%02x 0x%02x", __func__,
		   tlv[0], tlv[1], tlv[2], tlv[3]);
	tracef("%s(): 0x%02x 0x%02x 0x%02x 0x%02x", __func__,
		   tlv[4], tlv[5], tlv[6], tlv[7]);
}	

/* Divider values of M, N, and P to be used in junction with the 
   FSDIV bit for calculation of FS frequency according to the 
   formula FS = MCLK / (16 x M x N x P) */
/* MCLK = 22579200 / 2 = 11289600 */ 
/* FS = 8000 */ 
void tlv320_init(void)
{
	uint8_t tlv[4];

	if (i2c_read(codec_addr, 0, tlv, 4) < 0) {
		tracef("%s(): i2c_read() failed!", __func__);
		return;
	}

	tracef("%s(): 0x%02x 0x%02x 0x%02x 0x%02x", __func__,
		   tlv[0], tlv[1], tlv[2], tlv[3]);

//	tlv320_wr(2, CR2_DIFBP | CR2_I2CX_SET(4) | CR2_HPC_I2C);
//	tlv320_wr(2, CR2_I2CX_SET(4) | CR2_HPC_I2C);

	tlv320_wr(3, CR3_PWDN_NO | CR3_OSR_512 | CR3_ASRF_1);
	/* 8000 sps */
	tlv320_wr(4, CR4_M_SET(44));
	/* 11025 sps */
//	tlv320_wr(4, CR4_M_SET(32));
	tlv320_wr(4, CR4_NP_SET(1, 2));
	tlv320_wr(5, CR5A_ADGAIN_DB(0));

	tlv320_wr(5, CR5B_DAGAIN_DB(0));
	tlv320_wr(5, CR5C_DGSTG_MUTE | CR5C_INBG_6DB);
	tlv320_wr(6, CR3_AINSEL_INP_M1);
	tlv320_wr(1, CR1_CX | CR1_IIR | CR1_BIASV_LO | CR1_DAC16);
};

/* Set the DAC gain in dB (-42 to 20) */
void audio_dac_gain_set(int gain)
{
	if (gain < -42)
		gain = -42;
	else if (gain > 20)
		gain = 20;

	if (audio_drv.dac_gain == gain)
		return;

	if (tlv320_wr(5, CR5B_DAGAIN_DB(gain)) > 0) {
		audio_drv.dac_gain = gain;
		tracef("%s(): gain=%ddB", __func__, gain);
	}
}

void audio_tx_analyze(void) 
{
	spectrum_reset(&audio_tx_sa);

	spectrum_run(&audio_tx_sa, 0);

//	spectrum_normalize(&audio_tx_sa);

//	spectrum_pwr_show(&audio_tx_sa);

	spectrum_mag_show(&audio_tx_sa);
}

void audio_rx_analyze(void) 
{
	spectrum_reset(&audio_rx_sa);

	spectrum_run(&audio_rx_sa, 0);

//	spectrum_normalize(&audio_rx_sa);

//	spectrum_pwr_show(&audio_rx_sa);

	spectrum_mag_show(&audio_rx_sa);
}


int audio_tone_set(int tone, int gain)
{
	return tonegen_init(&tonegen, q15_db2amp(gain), tone);
}

int audio_tone_mode_set(int mode)
{
	if (mode < TONE_OFF)
		mode = TONE_OFF;
	else if (mode > TONE_ADC)
		mode = TONE_ADC;

	return audio_drv.tone_mode = mode;
}

#if DISABLE_JITBUF
sndbuf_t * xfr_buf;
#endif

void audio_io_task(void)
{
	sndbuf_t * out_buf;
	sndbuf_t * in_buf;
	uint32_t ts = 0;
	int i;

	tracef("%s(): <%d> started...", __func__, thinkos_thread_self());

	tonegen_init(&tonegen, 0, 0);
	spectrum_init(&audio_tx_sa, SAMPLE_RATE);
	spectrum_init(&audio_rx_sa, SAMPLE_RATE);

	for (;;) {
#if DISABLE_JITBUF
		out_buf = xfr_buf;
#else
		out_buf = jitbuf_dequeue(&audio_drv.jitbuf);
#endif

		if (audio_drv.tone_mode == TONE_DAC) {
			if (out_buf == NULL) {
				if ((out_buf = sndbuf_alloc()) != NULL)
					tonegen_apply(&tonegen, out_buf);
				else
					out_buf = (sndbuf_t *)&sndbuf_zero;
			}
		} else {
			if (out_buf == NULL) {
#if 0
			tracef("%s(): out_buf == NULL!", __func__);
#endif
				out_buf = (sndbuf_t *)&sndbuf_zero;
			}
		}

		spectrum_rec(&audio_tx_sa, out_buf);

		in_buf = i2s_io(out_buf);

  		for (i = 0; i < SNDBUF_LEN; i++)
		    in_buf->data[i] = FxaIirApply(&iir_hp_120hz, in_buf->data[i]);
//		    in_buf->data[i] = FxaIirApply(&iir_hp_240hz, in_buf->data[i]);

		led_flash(LED_I2S, 100);

		if (in_buf != &sndbuf_null) {
			if (audio_drv.tone_mode == TONE_ADC)
				tonegen_apply(&tonegen, in_buf);

			spectrum_rec(&audio_rx_sa, in_buf);
		}

		if (audio_drv.stream_enabled) {
#if ENABLE_G711
			if (g711_alaw_send(0, in_buf, ts) < 0) {
				tracef("%s(): net_send() failed!", __func__);
			}
#else
			if (audio_send(0, in_buf, ts) < 0) {
				tracef("%s(): net_send() failed!", __func__);
			}
#endif
			led_flash(LED_NET, 100);
		}

		ts += SNDBUF_LEN;

		sndbuf_free(in_buf);
		sndbuf_free(out_buf);
	}
}

void net_rcv_task(void)
{
	sndbuf_t * buf;
	uint32_t ts;
	int n;

	tracef("%s(): <%d> started...", __func__, thinkos_thread_self());

	for (;;) {
		while ((buf = sndbuf_alloc()) == NULL) {
			tracef("%s(): sndbuf_alloc() failed!", __func__);
			thinkos_sleep(1000);
		}

#if ENABLE_G711
		n = g711_alaw_recv(0, buf, &ts);
#else
		n = audio_recv(0, buf, &ts);
#endif

		if (n != sndbuf_len) {
			tracef("%s(): (n=%d != sndbuf_len)!", __func__, n);
		} else {
			if (audio_drv.stream_enabled) {
#if !DISABLE_JITBUF
				jitbuf_enqueue(&audio_drv.jitbuf, buf, ts);
#endif
			}
		}

#if DISABLE_JITBUF
		xfr_buf = buf;
#else
		sndbuf_free(buf);
#endif
	}
}

void audio_stream_enable(void)
{
	if (!audio_drv.enabled)
		return;

	if (audio_drv.stream_enabled)
		return;

	audio_drv.stream_enabled = true;
	tracef("%s(): audio stream enabled.", __func__);
}

void audio_stream_disable(void)
{
	if (!audio_drv.enabled)
		return;

	if (!audio_drv.stream_enabled)
		return;

	audio_drv.stream_enabled = false;
	tracef("%s(): audio stream disabled.", __func__);
}

void audio_enable(void)
{
	if (!audio_drv.enabled) {
		audio_drv.enabled = true;
		i2s_enable();
	}
}

void audio_disable(void)
{
	if (audio_drv.enabled) {
		audio_drv.stream_enabled = false;
		i2s_disable();
		audio_drv.enabled = false;
	}
}

void audio_reset(void)
{
	bool enable_stream;

	tracef("%s(): audio system reset ...", __func__);

	enable_stream = audio_drv.stream_enabled;

	audio_drv.stream_enabled = false;

	codec_hw_reset();
	
	tlv320_reset();

	if (audio_drv.enabled)
		i2s_disable();

	tlv320_init();

	if (audio_drv.enabled)
		i2s_enable();

	audio_drv.stream_enabled = enable_stream;
}

uint32_t audio_io_stack[256];
uint32_t net_rcv_stack[256];

void audio_init(void)
{
	int th;

	DCC_LOG(LOG_TRACE, "...");

	audio_drv.enabled = false;

	/* 50 ms jitter buffer */
	jitbuf_init(&audio_drv.jitbuf, SAMPLE_RATE,
				SAMPLE_RATE, 25);

	codec_hw_reset();

	tracef("%s(): initializing I2S...", __func__);
	i2s_slave_init();

	tracef("%s(): initializing TLV320...", __func__);
	tlv320_reset();

	tlv320_init();

	th = thinkos_thread_create((void *)net_rcv_task, (void *)NULL,
							   net_rcv_stack, 
							   sizeof(net_rcv_stack), 
							   THINKOS_OPT_PRIORITY(1) | 
							   THINKOS_OPT_ID(1));
	audio_drv.net_thread = th;

	th = thinkos_thread_create((void *)audio_io_task, (void *)NULL,
							   audio_io_stack, 
							   sizeof(audio_io_stack), 
							   THINKOS_OPT_PRIORITY(0) | 
							   THINKOS_OPT_ID(0));
	audio_drv.io_thread = th;

}

