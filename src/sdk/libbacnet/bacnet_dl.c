/* 
 * File:	 usb-test.c
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <sys/param.h>
#include <thinkos.h>
#include <crc.h>
#include <errno.h>

#include <sys/dcclog.h>

#include <bacnet/bacnet-ptp.h>

#include "dcc.h"


/* -------------------------------------------------------------------------
 * 10 DATA LINK/PHYSICAL LAYERS: POINT-TO-POINT (PTP)
 * ------------------------------------------------------------------------- */

struct bacnetdl_op {
	int ( * recv)(void * drv, uint8_t pdu[], unsigned int max);
	int ( * send)(void * drv, const uint8_t pdu[], unsigned int max);
};

struct bacnetdl_dev {
	void * drv;
	const struct bacnetdl_op * op;
};

struct {
	struct bacnetdl_dev dev[12];
	int ev;
} bacnet_io;

int bacnet_dl_recv(uint8_t pdu[], unsigned int max)
{
	int pdu_len;

	int i = 0;
	int ev;
	uint32_t clk;

	printf(" %s(): [%d] started...\n", __func__, thinkos_thread_self());
	thinkos_sleep(100);

	/* set the production enable flag to start production */
	for (;;) {
		ev = thinkos_ev_wait(bacnet_io.ev);
		clk = thinkos_clock();
		printf(" %5d.%03d - %4d - event=%d\n", clk / 1000, clk % 1000, i, ev);
		i++;
	} 

	return i;



	for(;;) {
		if (thinkos_flag_take(lnk->rx.flag) < 0) {
			DCC_LOG(LOG_ERROR, "thinkos_flag_take() failed!");
			abort();
		}

		if (lnk->state == BACNET_PTP_DISCONNECTED) {
			DCC_LOG(LOG_TRACE, "Disconnected. Bailing out...");
			return -EAGAIN;
		}

		/* check for a slot in the xmit queue ... */
		if (lnk->rx.pdu_len) 
			break;
	};

	pdu_len = lnk->rx.pdu_len;
	memcpy(pdu, lnk->rx.pdu, pdu_len);

	lnk->rx.pdu_len = 0;

	return pdu_len;
}

int bacnet_dl_send(const uint8_t pdu[], unsigned int len)
{
	uint32_t clk;

	for(;;) {
		if (thinkos_flag_take(lnk->tx.flag) < 0) {
			DCC_LOG(LOG_ERROR, "thinkos_flag_take() failed!");
			abort();
		}

		if (lnk->state != BACNET_PTP_CONNECTED)
			return -EAGAIN;

		/* check for a slot in the xmit queue ... */
		if (lnk->tx.len == 0) 
			break;
	};

	/* update the link layer event clock */
	clk = thinkos_clock();
	lnk->clk = clk;

	/* restart the retransmission timer */
	lnk->tx.rxmt_tmr = clk + NPDU_RXMT_TIME_MS; 
	lnk->tx.rxmt_cnt = 0;

	/* insert frame in the transmission queue ...  */
	memcpy(lnk->tx.buf, pdu, len);
	lnk->tx.len = len;

	if (lnk->tx.xon) {
		DCC_LOG1(LOG_INFO, "sending now (%d)...", len);
		/* if the remote side is reception is on send the frame now! */
		bacnet_ptp_frame_send(lnk, (lnk->tx.seq & 1) ? 
							  BACNET_PTP_FRM_DATA_1 :
							  BACNET_PTP_FRM_DATA_0,
							  lnk->tx.buf, lnk->tx.len);
	}

	return len;
}


struct bacnet_ptp_lnk * bacnet_ptp_outbound(struct bacnet_ptp_lnk * lnk)
{
	DCC_LOG(LOG_TRACE, "Starting BACnet PtP Data Link");

	/* 10.4.9.1 DISCONNECTED, ConnectOutbound 
	If a DL-CONNECT.request is received,
	then establish a physical connection; transmit the "BACnet<CR>" trigger 
	sequence; set RetryCount to zero; set
	ResponseTimer to zero; and enter the OUTBOUND state.
	 */
	serial_send(lnk->dev, "BACnet\r", 7);

	lnk->state = BACNET_PTP_OUTBOUND;
	DCC_LOG(LOG_TRACE, "[OUTBOUND]");

	return 0;
}

int bacnet_ptp_inbound(struct bacnet_ptp_lnk * lnk)
{

	DCC_LOG(LOG_TRACE, "Starting BACnet PtP Data Link");

	lnk->tx.rxmt_tmr = lnk->clk + CONN_RXMT_TIME_MS; 
	lnk->tx.rxmt_cnt = 0;
	bacnet_ptp_frame_send(lnk, BACNET_PTP_FRM_CONNECT_REQ, NULL, 0);

	/* 10.4.9.1 DISCONNECTED, ConnectInbound */
	lnk->state = BACNET_PTP_INBOUND;
	DCC_LOG(LOG_TRACE, "[INBOUND]");

	return 0;
}

#if 0

uint32_t bacnet_ptp_stack[256];

const struct thinkos_thread_inf bacnet_ptp_inf = {
	.stack_ptr = bacnet_ptp_stack, 
	.stack_size = sizeof(bacnet_ptp_stack), 
	.priority = 32,
	.thread_id = 7, 
	.paused = 0,
	.tag = "BN PTP"
};

void bacnet_ptp_task_init(struct bacnet_ptp_lnk * lnk)
{
	DCC_LOG(LOG_TRACE, "5. BACnet PtP Task...");
	thinkos_thread_create_inf((void *)bacnet_ptp_task, (void *)lnk, 
							  &bacnet_ptp_inf);
}

#endif

