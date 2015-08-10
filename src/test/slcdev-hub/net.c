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

#include "net.h"

struct {
	bool initialized;
	volatile bool probe_mode;
	volatile bool pkt_mode;
	volatile uint32_t probe_seq;
	uint32_t tx_seq;
	uint32_t rx_seq;
	int probe_flag;
	struct rs485_link link;
	struct netstats stat;
} net = {
	.initialized = false,
	.probe_mode = false,
	.pkt_mode = false
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
//const uint8_t * ice40lp384_bin = (uint8_t *)0x08060000;

extern const uint8_t ice40lp384_bin[];
extern const unsigned int sizeof_ice40lp384_bin;

/* -------------------------------------------------------------------------
   - Packets
   ------------------------------------------------------------------------- */

struct net_pkt {
	uint16_t crc;
	uint16_t data_len;
	uint16_t seq;
	uint8_t data[];
};


int net_pkt_send(const void * buf, int len)
{
	struct net_pkt * pkt;
	unsigned int data_len;
	uint8_t * dst;
	uint8_t * src;
	int i;

	pkt = (struct net_pkt *)pktbuf_alloc();
	if (pkt == NULL) {
		DCC_LOG(LOG_ERROR, "pktbuf_alloc() failed!");
		DBG("pktbuf_alloc() failed!\n");
		net.stat.tx.err_cnt++;
		return -1;
	}

	data_len = MIN(len, pktbuf_len - sizeof(struct net_pkt));

	pkt->crc = 0;
	pkt->data_len = data_len;
	pkt->seq = net.tx_seq++;
	dst = (uint8_t *)pkt->data;
	src = (uint8_t *)buf;

	for (i = 0; i < data_len; ++i)
		dst[i] = src[i];

	pkt->crc = crc16ccitt(0, pkt, data_len + sizeof(struct net_pkt));

	pkt = rs485_pkt_enqueue(&net.link, pkt, 
							data_len + sizeof(struct net_pkt));
	net.stat.tx.pkt_cnt++;
	net.stat.tx.octet_cnt += data_len + sizeof(struct net_pkt);
	if (pkt != NULL)
		pktbuf_free(pkt);

	return 0;
}

void net_pkt_recv(struct net_pkt * pkt, int len)
{
	void * buf = NULL;
	int data_len;
	uint16_t crc;

	data_len = len - sizeof(struct net_pkt);
	crc = pkt->crc;
	pkt->crc = 0;
	if (crc16ccitt(0, pkt, data_len + sizeof(struct net_pkt)) != crc) {
		DBG("CRC error!");
		data_len = -1;
		net.stat.rx.err_cnt++;
	} else if (data_len != pkt->data_len) {
		DBG("invalid data_len=%d!", data_len);
		data_len = -1;
		net.stat.rx.err_cnt++;
	} else {
		net.stat.rx.pkt_cnt++;
		net.stat.rx.octet_cnt += data_len + sizeof(struct net_pkt);

		if (pkt->seq != net.rx_seq) {
			int32_t n;
		
			n = (int32_t)(pkt->seq - net.rx_seq);
			if (n > 0) {
				DBG("SEQ error: %d pkts lost!", n);
				net.stat.rx.seq_err_cnt += n;
			} 
			net.rx_seq = pkt->seq;
		}

		net.rx_seq++;

		if (buf != NULL) {
			uint8_t * src;
			uint8_t * dst;
			int i;

			src = (uint8_t *)pkt->data;
			dst = (uint8_t *)buf;
			for (i = 0; i < data_len; ++i)
				dst[i] = src[i];
		}

	}
}

void net_probe_recv(char * s, int len)
{
	if ((len == 12) && (s[0] == 'P') && (s[1] == 'R')
		&& (s[2] == 'B') && (s[3] == '=')) {
		net.probe_seq = strtoul(&s[4], NULL, 16);
		net.stat.rx.pkt_cnt++;
		net.stat.rx.octet_cnt += len;
	} else {
		net.probe_seq = 0;
	}
	thinkos_flag_give(net.probe_flag);
}

void net_recv(char * s, int len)
{
	net.stat.rx.pkt_cnt++;
	net.stat.rx.octet_cnt += len;

	s[len] = '\0';

//	tracef("net rx: len=%d", len);
	DBG("\"%s\"", s);
}

void __attribute__((noreturn)) net_recv_task(void)
{
	void * pkt;
	int len;

	DCC_LOG1(LOG_TRACE, "thread=%d", thinkos_thread_self());

	DBG("<%d> started...", thinkos_thread_self());

	for (;;) {
		pkt = pktbuf_alloc();
		if (pkt == NULL) {
			DCC_LOG(LOG_ERROR, "pktbuf_alloc() failed!");
			DBG("pktbuf_alloc() failed!");
			thinkos_sleep(1000);
			continue;
		}

		len = rs485_pkt_receive(&net.link, &pkt, pktbuf_len);

		if (len < 0) {
			DBG("rs485_pkt_receive() failed!");
			thinkos_sleep(1000);
			continue;
		}

		if (len == 0) {
			DBG("rs485_pkt_receive() == 0!");
			thinkos_sleep(1000);
			continue;
		}


		if (pkt != NULL) {
			if (net.probe_mode)
				net_probe_recv((char *)pkt, len);
			else if (net.pkt_mode)
				net_pkt_recv((struct net_pkt *)pkt, len);
			else
				net_recv((char *)pkt, len);

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
								   net_recv_stack, sizeof(net_recv_stack) |
								   THINKOS_OPT_PRIORITY(5) | THINKOS_OPT_ID(5));
}

void net_pkt_mode(bool en)
{
	net.pkt_mode = en;
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

int net_probe(void)
{
	void * pkt;
	int ret;

	/* drain the transmmit queue */
	pkt = rs485_pkt_drain(&net.link);
	if (pkt != NULL)
		pktbuf_free(pkt);

	/* set the probe pin low */
	net.probe_mode = true;
	stm32_gpio_clr(RS485_MODE);
	DCC_LOG(LOG_TRACE, "Probe mode.");

	if ((pkt = pktbuf_alloc()) != NULL) {
		uint32_t seq;

		while ((seq = rand()) == 0);

		/* send a probe packet */
		sprintf((char *)pkt, "PRB=%08x", seq);
		DCC_LOG1(LOG_TRACE, "seq=0x%08x", seq);

		rs485_pkt_enqueue(&net.link, pkt, 12);
		net.stat.tx.pkt_cnt++;
		net.stat.tx.octet_cnt += 12;
		/* wait for the end of transmission */
		pkt = rs485_pkt_drain(&net.link);
		pktbuf_free(pkt);

		if ((ret = thinkos_flag_timedtake(net.probe_flag, 10)) == 0) {
			if (seq != net.probe_seq) {
				DCC_LOG(LOG_WARNING, "probe sequence mismatch!");
				ret = -1;
			}
		} else if (ret == THINKOS_ETIMEDOUT) {
			DCC_LOG(LOG_WARNING, "probe sequence timedout!");
		}
		DCC_LOG1(LOG_TRACE, "seq=0x%08x", seq);
	} else {	
		DCC_LOG(LOG_ERROR, "pktbuf_alloc() failed!");
		ret  = -1;
	}

	/* set the probe pin high */
	net.probe_mode = false;
	stm32_gpio_set(RS485_MODE);
	DCC_LOG(LOG_TRACE, "Probe mode.");

	return ret;
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
	net.stat.tx.pkt_cnt++;
	net.stat.tx.octet_cnt += len;

	if (pkt != NULL)
		pktbuf_free(pkt);

	return 0;
}

void net_get_stats(struct netstats * stat, bool rst)
{
	memcpy(stat, &net.stat, sizeof(struct netstats));
	if (rst)
		memset(&net.stat, 0, sizeof(struct netstats));
}

#if 0
int net_recv(void * buf, int len)
{
	void * pkt;

	pkt = pktbuf_alloc();
	if (pkt == NULL) {
		DCC_LOG(LOG_ERROR, "pktbuf_alloc() failed!");
		DBG("pktbuf_alloc() failed!\n");
		return -1;
	}

	len = rs485_pkt_receive(&net.link, &pkt, pktbuf_len);

//	DBG("len=%d\n", len);

	DCC_LOG1(LOG_TRACE, "%d", len);

	DCC_LOG2(LOG_TRACE, "pkt=%p len=%d", pkt, len);

	if (pkt != NULL) {
		memcpy(buf, pkt, len);
		pktbuf_free(pkt);
	}

	return len;
}
#endif

int net_init(void)
{
	DBG("RS485 network init...");

	if (lattice_ice40_configure(ice40lp384_bin, 
								sizeof_ice40lp384_bin) < 0) {
		ERR("lattice_ice40_configure() failed!");
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

#ifdef RS485_TRIG
	stm32_gpio_mode(RS485_TRIG, INPUT, PULL_UP);
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

		net.probe_flag = thinkos_flag_alloc();

		net.initialized = true;
	}


	return 0;
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
		DBG("pktbuf_alloc() failed!\n");
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

//	DBG("ts=%d data_len=%d", ts, data_len);

	pkt = rs485_pkt_enqueue(&net.link, pkt, 
							data_len + sizeof(struct audio_pkt));
	net.stat.tx.pkt_cnt++;
	net.stat.tx.octet_cnt += data_len + sizeof(struct audio_pkt);
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
		DBG("pktbuf_alloc() failed!\n");
		return -1;
	}

	len = rs485_pkt_receive(&net.link, (void **)&pkt, pktbuf_len);

	if (len < 0) {
		DBG("rs485_pkt_receive() failed!");
		return -1;
	}

	if (len == 0) {
		DBG("rs485_pkt_receive() == 0!");
		return -1;
	}

	if (pkt != NULL) {
		data_len = len - sizeof(struct audio_pkt);
		crc = pkt->crc;
		pkt->crc = 0;
		if (crc16ccitt(0, pkt,
					   data_len + sizeof(struct audio_pkt)) != crc) {
			DBG("crc error, data_len=%d!", data_len);
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
		DBG("pktbuf_alloc() failed!\n");
		return -1;
	}

	data_len = MIN(sndbuf_len * 2, pktbuf_len - sizeof(struct audio_pkt));

	pkt->ts = ts;
	pkt->crc = 0;
	pkt->data_len = data_len;
	memcpy(pkt->data, buf->data, data_len);
	pkt->crc = crc16ccitt(0, pkt, data_len + sizeof(struct audio_pkt));

//	DBG("ts=%d data_len=%d", ts, data_len);

	pkt = rs485_pkt_enqueue(&net.link, pkt, 
							data_len + sizeof(struct audio_pkt));
	net.stat.tx.pkt_cnt++;
	net.stat.tx.octet_cnt += data_len + sizeof(struct audio_pkt);
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
		DBG("pktbuf_alloc() failed!\n");
		return -1;
	}

	len = rs485_pkt_receive(&net.link, (void **)&pkt, pktbuf_len);

	if (len < 0) {
		DBG("rs485_pkt_receive() failed!");
		return -1;
	}

	if (len == 0) {
		DBG("rs485_pkt_receive() == 0!");
		return -1;
	}

	if (pkt != NULL) {
		data_len = len - sizeof(struct audio_pkt);
		crc = pkt->crc;
		pkt->crc = 0;
		if (pkt->data_len != data_len) {
			DBG("data_len=%d!=%d error!", 
				   data_len, pkt->data_len);
		} else if (crc16ccitt(0, pkt,
					   data_len + sizeof(struct audio_pkt)) != crc) {
			DBG("CRC error, data_len=%d!", data_len);
		} else { 
//			DBG("ts=%d data_len=%d", pkt->ts, data_len);
			*ts = pkt->ts;
			memcpy(buf->data, pkt->data, data_len);
			ret =  data_len / 2;
		}
		pktbuf_free(pkt);
	} 

	return ret;
}

