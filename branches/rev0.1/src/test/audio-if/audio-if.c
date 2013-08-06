/* 
 * File:	 audio-if.c
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(C) 2013 Bob Mittmann. All Rights Reserved.
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
#include <arch/cortex-m3.h>
#include <sys/delay.h>
#include <sys/serial.h>
#include <sys/param.h>
#include <sys/file.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define __THINKOS_IRQ__
#include <thinkos_irq.h>
#include <thinkos.h>

#include <sys/dcclog.h>

#include "i2c.h"
#include "console.h"
#include "io.h"
#include "audio.h"
#include "trace.h"
#include "fixpt.h"
#include "sndbuf.h"
#include "vt100.h"
#include "telctl.h"
#include "tonegen.h"
#include "net.h"
#include "g711.h"

int tone_gain = 0;
int tone_wave = 0;
int tone_freq = 0;
int tone_mode = TONE_OFF;

bool autobalance_enabled = true;
bool autogain_enabled = true;

/* ----------------------------------------------------------------------
 * Supervisory task
 * ----------------------------------------------------------------------
 */

const char sup_nm_lut[][4] = {
	"UN",
	"OP",
	"ST",
	"ID",
	"1P",
	"2P",
	"3P",
	"4P",
	"5P"
};

const char tone_nm_lut[][4] = {
	"OFF",
	"DAC",
	"ADC"
};

void print_status(void)
{
	char s[64];
	char * cp = s;
	int i;
	int g;
	int z;
	int cnt;

	g = telctl.hybrid.g;
	z = telctl.hybrid.z;

	cp += sprintf(cp, VT100_CURSOR_SAVE VT100_GOTO, 1, 36); 
	cp += sprintf(cp, "| %c %c | ", 
				  autobalance_enabled  ? 'B' : '-',
				  autogain_enabled  ? 'G' : '-');

	cp += sprintf(cp, "%3s %-4d | ", tone_nm_lut[tone_mode], tone_freq);

	cp += sprintf(cp, "%4d %d.%02d | ", z, g / 100, g % 100);

	cnt = 0;
	for (i = 0; i < 5; ++i) {
		cp += sprintf(cp, "%s ", sup_nm_lut[telctl.line[i].sup_st]);
		if ((telctl.line[i].sup_st >= LINE_PHONE1) && 
			(!telctl.line[i].connected)) {
			cnt++;
		}
	}

	if (cnt)
		led_flash(LED_S2, 100);

	cp += sprintf(cp, VT100_CURSOR_UNSAVE); 

	printf(s);
}

void thinkos_exception_dsr(struct thinkos_context * ctx)
{
	trace_fprint(stderr, 0);
	fprintf(stderr, "---------\n");
}

int supervisor_task(void)
{
	tracef("%s(): <%d> started...", __func__, thinkos_thread_self());

	for (;;) {
		trace_fprint(stdout, 1);

		print_status();

		if (autobalance_enabled && autogain_enabled) {
			led_on(LED_S1);
		} else if (autobalance_enabled || autogain_enabled) {
			led_flash(LED_S1, 100);
		} else {
			led_off(LED_S1);
		}

		thinkos_sleep(250);
	}
}

void system_reset(void)
{
	DCC_LOG(LOG_TRACE, "...");

	thinkos_sleep(10);
    CM3_SCB->aircr =  SCB_AIRCR_VECTKEY | SCB_AIRCR_SYSRESETREQ;
	for(;;);
}


int acq_task(void)
{
	int z;
	int vr;
	int gain;

	DCC_LOG(LOG_TRACE, "started...");
	tracef("%s(): <%d> started...", __func__, thinkos_thread_self());

	for (;;) {
		DCC_LOG(LOG_INFO, "Poll...");
		thinkos_sleep(250);
		if (telctl_adc_scan() >= 0) { 
			switch (telctl.load.cnt) {
			case 1:
				z = 1293;
				gain = 0;
				break;
			case 2:
				z = 2325;
				gain = 3;
				break;
			case 3:
				z = 3992;
				gain = 5;
				break;
			case 4:
				z = 4500;
				gain = 7;
				break;
			case 5:
				z = 5500;
				gain = 9;
				break;
			case 0:
			default:
				z = 0;
				gain = 0;
				break;
			}
//			z -= 500;
			z = (1300 * telctl.load.cnt) - 500;
			vr = ((z * 63) + 2500) / 5000;
			if (autobalance_enabled)
				hybrid_impedance_set(vr);
			if (autogain_enabled)
				audio_dac_gain_set(gain);
		};

		if (line_disconnect_on_hook() == 0) {
			audio_stream_disable();
		}
	}
}

void tone_gain_step(int d)
{
	int gain;

	gain = tone_gain + d;

	if (gain > 0) {
		gain = 0;
	} else if (gain < q15_db2amp_min) {
		gain = tone_gain;
	}

	audio_tone_set(tone_wave, gain);
	tone_gain = gain;
}

void tone_mode_cycle(void)
{
	tone_mode = (tone_mode == TONE_ADC) ? TONE_OFF : tone_mode + 1;
	tone_mode = audio_tone_mode_set(tone_mode);
}

void tone_freq_cycle(void)
{
	tone_wave = (tone_wave == wave_max) ? 0 : tone_wave + 1;
	tone_freq = audio_tone_set(tone_wave, tone_gain);
}

void show_menu(void)
{
	printf("\n");
	printf(" Options:\n");
	printf(" --------\n");
	printf("   1..5 - Toggle line \n");
	printf("   9 0  - Hybrid impedance\n");
	printf("   - =  - Hybrid gain\n");
	printf("   r    - ADC (RX) analyzer\n");
	printf("   t    - DAC (TX) analyzer\n");
	printf("   y    - Halt analyzer\n");
	printf("   g    - Auto gain enable/disable\n");
	printf("   b    - Auto balance enable/disable\n");
	printf("   m    - Tone mode select\n");
	printf("   f    - Tone frequency select\n");
	printf("   [ ]  - Tone generator gain\n");
	printf("   s    - system reset\n");

	printf("\n");
}

struct {
	volatile int chan;
	volatile int count;
	volatile bool enabled;
	int flag;
} spectrum_analyzer;

void spectrum_analyzer_task(void)
{
	while (1) {
		thinkos_flag_wait(spectrum_analyzer.flag);
		thinkos_flag_clr(spectrum_analyzer.flag);

		printf(VT100_CLRSCR);

		while (spectrum_analyzer.enabled) {

			if (spectrum_analyzer.chan == 1)
				audio_rx_analyze();
			else
				audio_tx_analyze();

			spectrum_analyzer.count--;
			if (spectrum_analyzer.count == 0)
				spectrum_analyzer.enabled = false;

//			thinkos_sleep(200);
		};
	}
}

void shell_task(void)
{
	int c;

	printf("-------------\n");
	printf(" Audio Shell \n");
	printf("-------------\n");
	printf("\n");

	for(;;) {
		c = getchar();
		switch (c) {

		case '\n':
			show_menu();
			break;

		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
			line_toggle(c - '1');
			break;

		case 'r':
			spectrum_analyzer.chan = 1;
			spectrum_analyzer.count = 10000;
			spectrum_analyzer.enabled = true;
			thinkos_flag_set(spectrum_analyzer.flag);
			break;
		case 't':
			spectrum_analyzer.chan = 0;
			spectrum_analyzer.count = 10000;
			spectrum_analyzer.enabled = true;
			thinkos_flag_set(spectrum_analyzer.flag);
			break;
		case 'y':
			spectrum_analyzer.enabled = false;
			break;

		case 'e':
			audio_enable();
			break;
		case 'o':
			audio_disable();
			break;
		case 's':
			system_reset();
			break;
		case '[':
			tone_gain_step(-6);
			break;
		case ']':
			tone_gain_step(6);
			break;

		case 'm':
			tone_mode_cycle();
			break;
		case 'f':
			tone_freq_cycle();
			break;

		case 'g':
			autogain_enabled = (autogain_enabled) ? false : true;
			break;
		case 'b':
			autobalance_enabled = (autobalance_enabled) ? false : true;
			break;

		case '9':
			hybrid_impedance_step(-1);
			break;
		case '0':
			hybrid_impedance_step(1);
			break;

		case '-':
			hybrid_gain_step(-1);
			break;
		case '=':
			hybrid_gain_step(1);
			break;
		}
	}
}

uint32_t spectrum_analyzer_stack[1024];

void spectrum_analyzer_init(void)
{
	DCC_LOG(LOG_TRACE, "thinkos_thread_create()");

	spectrum_analyzer.flag = thinkos_flag_alloc();

	thinkos_thread_create((void *)spectrum_analyzer_task, (void *)NULL,
						  spectrum_analyzer_stack, 
						  sizeof(spectrum_analyzer_stack), 
						  THINKOS_OPT_PRIORITY(7) | THINKOS_OPT_ID(7));

	thinkos_sleep(10);
}

uint32_t shell_stack[128];

void shell_init(void)
{
	DCC_LOG(LOG_TRACE, "thinkos_thread_create()");
	thinkos_thread_create((void *)shell_task, (void *)NULL,
						  shell_stack, sizeof(shell_stack), 
						  THINKOS_OPT_PRIORITY(4) | THINKOS_OPT_ID(4));

	thinkos_sleep(10);
}


/* ----------------------------------------------------------------------
 * User interface task
 * ----------------------------------------------------------------------
 */
int ui_task(void)
{
	int event;
	int i;

	tracef("%s(): <%d> started...", __func__, thinkos_thread_self());


	DCC_LOG1(LOG_TRACE, "[%d] started.", thinkos_thread_self());

	while (1) {

		/* wait for a push buton event */
		event = btn_event_wait();

		switch (event) {
		case EVENT_CLICK:
	//		tracef("Click");
			if (line_connect_off_hook() > 0) {
				audio_stream_enable();
			}
			break;

		case EVENT_DBL_CLICK:
	//		tracef("Double click");
			if (autobalance_enabled) {
				autobalance_enabled = false;
				autogain_enabled = false;
//				hybrid_gain_set(16);
				hybrid_impedance_set(0);
				audio_dac_gain_set(0);
			} else {
				autobalance_enabled = true;
				autogain_enabled = true;
			}

			break;

		case EVENT_HOLD1:
	//		tracef("Hold 1");
			audio_stream_disable();
			line_hangup_all();
			break;

		case EVENT_CLICK_N_HOLD:
	//		tracef("click'n hold");
			break;

		case EVENT_HOLD2:
	//		tracef("Hold 2");
			leds_lock();
			leds_all_off();
			for (i = 0; i < 6; ++i) {
				thinkos_sleep(500);
				leds_all_flash(200);
			}
			system_reset();
			break;
		}
	}
}

uint32_t supervisor_stack[256];
uint32_t acq_stack[128];

void test(void)
{
	int i;
	int16_t val;

	for (i = -32768; i <= 32767; i++) {
		val = i;
		printf("0x%4x - > %d %d %d\n", val & 0xffff, 
			   __top_bit(val), 31 - __clz((uint16_t)val), 
			   ffs((uint16_t)val));
	}
}

int main(int argc, char ** argv)
{
	DCC_LOG_INIT();
	DCC_LOG_CONNECT();

	/* calibrate usecond delay loop */
	cm3_udelay_calibrate();

	DCC_LOG(LOG_TRACE, "1. trace_init()");
	trace_init();

	DCC_LOG(LOG_TRACE, "2. thinkos_init()");
	thinkos_init(THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(32));

	DCC_LOG(LOG_TRACE, "3. io_init()");
	io_init();

	DCC_LOG(LOG_TRACE, "4. stdio_init()");
	stdio_init();

	printf(" " VT100_CLRSCR);
	printf("\n\n");
	printf("\n\n");
	printf("-----------------------------------------\n");
	printf(" Audio interface test\n");
	printf("-----------------------------------------\n");
	printf("\n");

//	test();

	thinkos_thread_create((void *)supervisor_task, (void *)NULL,
						  supervisor_stack, sizeof(supervisor_stack), 
						  THINKOS_OPT_PRIORITY(4) | THINKOS_OPT_ID(8));

	DCC_LOG(LOG_TRACE, "6. sndbuf_pool_init()");
	sndbuf_pool_init();

	DCC_LOG(LOG_TRACE, "6. i2c_init()");
	i2c_init();

	net_init();

	/* give a little time for the slave board start-up */
	thinkos_sleep(100);

	telctl_sync();

	telctl_tonegen_set(1, 1);

	hybrid_gain_set(40);

	thinkos_thread_create((void *)acq_task, (void *)NULL,
						  acq_stack, sizeof(acq_stack), 
						  THINKOS_OPT_PRIORITY(2) | THINKOS_OPT_ID(2));

	DCC_LOG(LOG_TRACE, "6. i2c_init()");
	audio_init();

	DCC_LOG(LOG_TRACE, "17. shell_init()");

	spectrum_analyzer_init();

	shell_init();

	audio_enable();

	return ui_task();
}

