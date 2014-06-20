/* 
 * File:	 net.c
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

#include <hexdump.h>

#define __THINKOS_IRQ__
#include <thinkos_irq.h>
#include <thinkos.h>

#include <sys/dcclog.h>

#include "trace.h"
#include "pktbuf.h"
#include "rs485lnk.h"
#include "g711.h"
#include "crc.h"

#include "board.h"
#include "lattice.h"

struct {
	int8_t initialized;
	struct rs485_link link;
} net = {
	.initialized = false
};

#define USART2_DMA         STM32F_DMA1
#define USART2_DMA_CHAN    4
#define USART2_RX_DMA_STRM 5
#define USART2_TX_DMA_STRM 6


#define USART1_DMA         STM32F_DMA2
#define USART1_DMA_CHAN    4
#define USART1_RX_DMA_STRM 5
#define USART1_TX_DMA_STRM 7


#define RS485_LINK_SPEED 500000

//void stm32f_usart2_isr(void)
//{
//	rs485_link_isr(&net.link);
//}

const uint8_t * ice40lp384_bin = (uint8_t *)0x08060000;

void __attribute__((noreturn)) net_recv_task(void)
{
	void * pkt;
	int len;

	DCC_LOG1(LOG_TRACE, "thread=%d", thinkos_thread_self());

	tracef("%s(): <%d> started...", __func__, thinkos_thread_self());

	for (;;) {
		pkt = pktbuf_alloc();
		if (pkt == NULL) {
			DCC_LOG(LOG_ERROR, "pktbuf_alloc() failed!");
			tracef("%s(): pktbuf_alloc() failed!", __func__);
			thinkos_sleep(5000);
			continue;
		}

		len = rs485_pkt_receive(&net.link, &pkt, pktbuf_len);

		if (pkt != NULL) {
			char * s = (char *)pkt;

			DCC_LOG1(LOG_TRACE, "len=%d", len);
			s[len] = '\0';
			printf("\"%s\"\n", s);

			pktbuf_free(pkt);
		}

	}
}

uint32_t __attribute__((aligned(8))) net_recv_stack[1024 + 256];

void net_recv_init(void)
{
	static int8_t thread = -1;

	if (thread >= 0)
		return;

	thread = thinkos_thread_create((void *)net_recv_task, (void *)NULL,
								   net_recv_stack, sizeof(net_recv_stack), 
								   THINKOS_OPT_PRIORITY(5) | THINKOS_OPT_ID(5));
}

void net_probe_enable(void)
{
#ifdef RS485_MODE
	DCC_LOG(LOG_TRACE, "Probe mode.");
	stm32_gpio_clr(RS485_MODE);
#endif
}

void net_probe_disable(void)
{
#ifdef RS485_MODE
	DCC_LOG(LOG_TRACE, "Normal mode.");
	stm32_gpio_set(RS485_MODE);
#endif
}

int net_init(void)
{
	tracef("%s():...", __func__);

	if (lattice_ice40_configure(ice40lp384_bin, 32768) < 0) {
		trace("lattice_ice40_configure() failed!");
		return -1;
	}

	/* IO init */
	stm32_gpio_mode(RS485_RX, ALT_FUNC, PULL_UP);
	stm32_gpio_af(RS485_RX, GPIO_AF7);

	stm32_gpio_mode(RS485_TX, ALT_FUNC, PUSH_PULL | SPEED_MED);
	stm32_gpio_af(RS485_TX, GPIO_AF7);

#ifdef RS485_CK
	stm32_gpio_mode(RS485_CK, ALT_FUNC, PUSH_PULL | SPEED_MED);
	stm32_gpio_af(RS485_CK, GPIO_AF7);
#endif

#ifdef RS485_TXEN
	stm32_gpio_mode(RS485_TXEN, OUTPUT, PUSH_PULL | SPEED_MED);
	stm32_gpio_set(RS485_TXEN);
#endif

#ifdef RS485_LOOP
	stm32_gpio_mode(RS485_LOOP, OUTPUT, PUSH_PULL | SPEED_MED);
	stm32_gpio_set(RS485_LOOP);
#endif

#ifdef RS485_MODE
	stm32_gpio_mode(RS485_MODE, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32_gpio_set(RS485_MODE);
#endif

	if (!net.initialized) {
		/* Link init */
		rs485_init(&net.link, RS485_USART, RS485_LINK_SPEED, 
				   USART1_DMA, USART1_DMA_CHAN,
				   USART1_RX_DMA_STRM, USART1_TX_DMA_STRM);

		/* initialize the packet buffer pool */
		pktbuf_pool_init();

		net_recv_init();
	
		net.initialized = true;
	}


	return 0;
}

int net_send(const void * buf, int len)
{
	void * pkt;

	pkt = pktbuf_alloc();
	if (pkt == NULL) {
		DCC_LOG(LOG_ERROR, "pktbuf_alloc() failed!");
		return -1;
	}

	DCC_LOG2(LOG_TRACE, "pkt=%p len=%d", pkt, len);

	len = MIN(len, pktbuf_len);

	memcpy(pkt, buf, len);

	pkt = rs485_pkt_enqueue(&net.link, pkt, len);

	if (pkt != NULL)
		pktbuf_free(pkt);

	return 0;
}

int net_recv(void * buf, int len)
{
	void * pkt;

	pkt = pktbuf_alloc();
	if (pkt == NULL) {
		DCC_LOG(LOG_ERROR, "pktbuf_alloc() failed!");
		tracef("%s(): pktbuf_alloc() failed!\n", __func__);
		return -1;
	}

	len = rs485_pkt_receive(&net.link, &pkt, pktbuf_len);

//	tracef("%s(): len=%d\n", __func__, len);

	DCC_LOG1(LOG_TRACE, "%d", len);

	DCC_LOG2(LOG_TRACE, "pkt=%p len=%d", pkt, len);

	if (pkt != NULL) {
		memcpy(buf, pkt, len);
		pktbuf_free(pkt);
	}

	return len;
}


#include "sndbuf.h"

struct audio_pkt {
	uint32_t ts;
	uint16_t crc;
	uint16_t data_len;
	uint32_t data[];
};

int g711_alaw_send(int stream, sndbuf_t * buf, uint32_t ts)
{
	struct audio_pkt * pkt;
	unsigned int data_len;
	uint8_t * dst;
	int i;

	pkt = (struct audio_pkt *)pktbuf_alloc();
	if (pkt == NULL) {
		DCC_LOG(LOG_ERROR, "pktbuf_alloc() failed!");
		tracef("%s(): pktbuf_alloc() failed!\n", __func__);
		return -1;
	}

	data_len = MIN(sndbuf_len, pktbuf_len - sizeof(struct audio_pkt));

	pkt->ts = ts;
	pkt->crc = 0;
	pkt->data_len = data_len;
	dst = (uint8_t *)pkt->data;

	for (i = 0; i < data_len; ++i) {
		dst[i] = linear2alaw(buf->data[i] >> 3);
	}

	pkt->crc = crc16ccitt(0, pkt, data_len + sizeof(struct audio_pkt));

//	tracef("%s(): ts=%d data_len=%d", __func__, ts, data_len);

	pkt = rs485_pkt_enqueue(&net.link, pkt, 
							data_len + sizeof(struct audio_pkt));

	if (pkt != NULL)
		pktbuf_free(pkt);

	return 0;
}

int g711_alaw_recv(int stream, sndbuf_t * buf, uint32_t * ts)
{
	struct audio_pkt * pkt;
	int data_len;
	uint16_t crc;
	uint8_t * src;
	int len;
	int i;

	pkt = (struct audio_pkt *)pktbuf_alloc();
	if (pkt == NULL) {
		DCC_LOG(LOG_ERROR, "pktbuf_alloc() failed!");
		tracef("%s(): pktbuf_alloc() failed!\n", __func__);
		return -1;
	}

	len = rs485_pkt_receive(&net.link, (void **)&pkt, pktbuf_len);

	if (len < 0) {
		tracef("%s(): rs485_pkt_receive() failed!", __func__);
		return -1;
	}

	if (len == 0) {
		tracef("%s(): rs485_pkt_receive() == 0!", __func__);
		return -1;
	}

	if (pkt != NULL) {
		data_len = len - sizeof(struct audio_pkt);
		crc = pkt->crc;
		pkt->crc = 0;
		if (crc16ccitt(0, pkt,
					   data_len + sizeof(struct audio_pkt)) != crc) {
			tracef("%s(): crc error, data_len=%d!", __func__, data_len);
			data_len = -1;
		} else {
			if ((data_len = len - sizeof(struct audio_pkt)) > 0) {
				*ts = pkt->ts;
				src = (uint8_t *)pkt->data;
				for (i = 0; i < data_len; ++i) {
					buf->data[i] = alaw2linear(src[i]) << 3;
				}
			}
		}
		pktbuf_free(pkt);
		return data_len;
	} 

	return -1;
}

int audio_send(int stream, sndbuf_t * buf, uint32_t ts)
{
	struct audio_pkt * pkt;
	unsigned int data_len;

	pkt = (struct audio_pkt *)pktbuf_alloc();
	if (pkt == NULL) {
		DCC_LOG(LOG_ERROR, "pktbuf_alloc() failed!");
		tracef("%s(): pktbuf_alloc() failed!\n", __func__);
		return -1;
	}

	data_len = MIN(sndbuf_len * 2, pktbuf_len - sizeof(struct audio_pkt));

	pkt->ts = ts;
	pkt->crc = 0;
	pkt->data_len = data_len;
	memcpy(pkt->data, buf->data, data_len);
	pkt->crc = crc16ccitt(0, pkt, data_len + sizeof(struct audio_pkt));

//	tracef("%s(): ts=%d data_len=%d", __func__, ts, data_len);

	pkt = rs485_pkt_enqueue(&net.link, pkt, data_len + sizeof(struct audio_pkt));

	if (pkt != NULL)
		pktbuf_free(pkt);

	return 0;
}

int audio_recv(int stream, sndbuf_t * buf, uint32_t * ts)
{
	struct audio_pkt * pkt;
	uint16_t crc;
	int data_len;
	int len;
	int ret = -1;

	pkt = (struct audio_pkt *)pktbuf_alloc();
	if (pkt == NULL) {
		DCC_LOG(LOG_ERROR, "pktbuf_alloc() failed!");
		tracef("%s(): pktbuf_alloc() failed!\n", __func__);
		return -1;
	}

	len = rs485_pkt_receive(&net.link, (void **)&pkt, pktbuf_len);

	if (len < 0) {
		tracef("%s(): rs485_pkt_receive() failed!", __func__);
		return -1;
	}

	if (len == 0) {
		tracef("%s(): rs485_pkt_receive() == 0!", __func__);
		return -1;
	}

	if (pkt != NULL) {
		data_len = len - sizeof(struct audio_pkt);
		crc = pkt->crc;
		pkt->crc = 0;
		if (pkt->data_len != data_len) {
			tracef("%s(): data_len=%d!=%d error!", __func__, 
				   data_len, pkt->data_len);
		} else if (crc16ccitt(0, pkt,
					   data_len + sizeof(struct audio_pkt)) != crc) {
			tracef("%s(): CRC error, data_len=%d!", __func__, data_len);
		} else { 
//			tracef("%s(): ts=%d data_len=%d", __func__, pkt->ts, data_len);
			*ts = pkt->ts;
			memcpy(buf->data, pkt->data, data_len);
			ret =  data_len / 2;
		}
		pktbuf_free(pkt);
	} 

	return ret;
}

