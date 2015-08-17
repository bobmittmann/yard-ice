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

#include "board.h"
#include "lattice.h"
#include <bacnet/mstp_lnk.h>

#include "net.h"

struct {
	bool initialized;
	volatile bool probe_mode;
	volatile bool pkt_mode;
	volatile uint32_t probe_seq;
	uint32_t tx_seq;
	uint32_t rx_seq;
	int probe_flag;
	struct netstats stat;
} net = {
	.initialized = false,
	.probe_mode = false,
	.pkt_mode = false
};


#define RS485_LINK_SPEED 500000

extern const uint8_t ice40lp384_bin[];
extern const unsigned int sizeof_ice40lp384_bin;

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
	int ret = 0;
#if 0
	void * pkt;

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
#endif
	return ret;
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

/* -------------------------------------------------------------------------
 * MS/TP 
 * ------------------------------------------------------------------------- */

int mstp_task(void * arg)
{
	struct mstp_lnk * mstp = (struct mstp_lnk *)arg;

	printf("MS/TP task started...\n");

	mstp_lnk_loop(mstp);

	return 0;
}

struct serial_dev * rs485_init(void)
{
	struct serial_dev * ser;

    /* IO init */
    stm32_gpio_mode(IO_RS485_RX, ALT_FUNC, PULL_UP);
    stm32_gpio_af(IO_RS485_RX, RS485_USART_AF);

    stm32_gpio_mode(IO_RS485_TX, ALT_FUNC, PUSH_PULL | SPEED_MED);
    stm32_gpio_af(IO_RS485_TX, RS485_USART_AF);

    stm32_gpio_mode(IO_RS485_MODE, OUTPUT, PUSH_PULL | SPEED_LOW);
    stm32_gpio_set(IO_RS485_MODE);

//	ser = stm32f_uart1_serial_init(500000, SERIAL_8N1);
	ser = stm32f_uart1_serial_dma_init(500000, SERIAL_8N1);
//	ser = stm32f_uart7_serial_init(500000, SERIAL_8N1);

	return ser;
}

uint32_t mstp_stack[512];

const struct thinkos_thread_inf mstp_inf = {
	.stack_ptr = mstp_stack,
	.stack_size = sizeof(mstp_stack),
	.priority = 8,
	.thread_id = 1,
	.paused = 0,
	.tag = "MS/TP"
};

struct mstp_lnk * mstp_start(int addr)
{
	struct serial_dev * ser;
	struct mstp_lnk * mstp;

	printf("1. rs485_init() ...\n");
	if ((ser = rs485_init()) == NULL) {
		return NULL;
	}

	printf("2. mstp_lnk_alloc() ...\n");
	mstp = mstp_lnk_alloc();

	printf("3. MS/TP link addr: %d ...\n", addr);
	mstp_lnk_init(mstp, "MS/TP1", addr, ser);

	printf("4. thinkos_thread_create_inf()\n");
	thinkos_thread_create_inf(mstp_task, mstp, &mstp_inf);

	thinkos_sleep(100);

	printf("5. mstp_lnk_resume()\n");
	mstp_lnk_resume(mstp);

	return mstp;
}

struct board_cfg {
    uint32_t magic;
    uint32_t mstp_addr;
    uint32_t ip_addr;
};

#define CFG_MAGIC 0x01020304
#define CFG_ADDR  0x0800ff00

int recv_task(void * arg)
{
	struct mstp_lnk * mstp = (struct mstp_lnk *)arg;
	struct mstp_frame_inf inf;
	uint8_t pdu[502];
	int len;
	unsigned int octet_cnt = 0;
	unsigned int bcast_cnt = 0;
	unsigned int unicast_cnt = 0;
	unsigned int seconds = 10;
	uint32_t clk;

	printf("receive task started...\n");

	clk = thinkos_clock() + seconds * 1000;
	for (;;) {
		len = mstp_lnk_recv(mstp, pdu, sizeof(pdu), &inf);
		octet_cnt += len;
		if (inf.daddr == MSTP_ADDR_BCAST)
			bcast_cnt++;
		else
			unicast_cnt++;

//		printf("RCV: %d->%d (%d) \"%s\"\n", inf.saddr, inf.daddr,
//				len, (char*)pdu);
//		DBG("RCV: %d->%d (%d)", inf.saddr, inf.daddr, len);
		/* Log summary each 5 seconds */
		if ((int32_t)(clk - thinkos_clock()) <= 0) {
			unsigned int rate;
			rate = octet_cnt / seconds;

			DBG("Receive: %d bcast, %d unicast, %d octets, %d.%03d KBps.",
					bcast_cnt, unicast_cnt, octet_cnt,
					rate / 1000, rate % 1000);

			bcast_cnt = 0;
			unicast_cnt = 0;
			octet_cnt = 0;
			seconds = 10;
			clk += seconds * 1000;
		}
	}

	return 0;
}

uint32_t recv_stack[512];

const struct thinkos_thread_inf recv_inf = {
	.stack_ptr = recv_stack, 
	.stack_size = sizeof(recv_stack), 
	.priority = 32,
	.thread_id = 8, 
	.paused = 0,
	.tag = "RECV"
};

int net_init(void)
{
	struct board_cfg * cfg = (struct board_cfg *)(CFG_ADDR);
	struct mstp_lnk * mstp;
	struct mstp_frame_inf inf;
	uint8_t mstp_addr = 1;
	uint8_t pdu[502];
	uint32_t clk;
	uint32_t tmo;
	unsigned int octet_cnt = 0;
	unsigned int bcast_cnt = 0;
	unsigned int itval = 5;
	int len;
	int i;

	DBG("RS485 network init...");

	if (lattice_ice40_configure(ice40lp384_bin, 
								sizeof_ice40lp384_bin) < 0) {
		ERR("lattice_ice40_configure() failed!");
		return -1;
	}


	if (cfg->magic == CFG_MAGIC)
		mstp_addr = cfg->mstp_addr;
	else
		mstp_addr = *((uint32_t *)STM32F_UID) & 0x1f;


	INF("Starting MS/TP test");

	if ((mstp = mstp_start(mstp_addr)) == NULL) {
		thinkos_sleep(1000);
		return 1;
	}


	thinkos_thread_create_inf(recv_task, mstp, &recv_inf);

	clk = thinkos_clock();
	tmo = clk + itval * 1000;

	for (i = 0;; ++i) {
		inf.daddr = MSTP_ADDR_BCAST;
		inf.type = FRM_DATA_NO_REPLY;
		len = snprintf((char *)pdu, 501, "%6d"
					  " The quick brown fox jumps over the lazy dog."
					  " The quick brown fox jumps over the lazy dog."
					  " The quick brown fox jumps over the lazy dog."
				  	  " The quick brown fox jumps over the lazy dog."
				  	  " The quick brown fox jumps over the lazy dog."
				      " The quick brown fox jumps over the lazy dog."
					  " The quick brown fox jumps over the lazy dog."
					  " The quick brown fox jumps over the lazy dog."
					  " The quick brown fox jumps over the lazy dog."
				      " The quick brown fox jumps over the lazy dog."
					  " The quick brown fox jumps over the lazy dog.",
					  i);
		if (mstp_lnk_send(mstp, pdu, len, &inf) < 0) {
			ERR("mstp_lnk_send() failed!");
		}
		octet_cnt += len;
		bcast_cnt++;

		/* Log summary each 10 seconds */
		if ((int32_t)(tmo - clk) <= 0) {
			unsigned int rate;

			rate = octet_cnt / itval;

			DBG("Send: %d bcasts, %d octets, %d.%03d KBps.", bcast_cnt, octet_cnt,
					rate / 1000, rate % 1000);

			octet_cnt = 0;
			bcast_cnt = 0;
			itval = 10;
			tmo += itval * 1000;
		}
		clk = thinkos_clock();
	}



	return 0;
}
