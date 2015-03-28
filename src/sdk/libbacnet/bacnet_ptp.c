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
#include <errno.h>

#include <sys/dcclog.h>

#include <bacnet/bacnet-ptp.h>
#include <bacnet/bacnet-dl.h>

#include "bacnet-i.h"

/* -------------------------------------------------------------------------
 * 10 DATA LINK/PHYSICAL LAYERS: POINT-TO-POINT (PTP)
 * ------------------------------------------------------------------------- */

/* PTP Frame  types */
enum {
	PTP_FRM_HEARTBEAT_XOFF  = 0x00,
	PTP_FRM_HEARTBEAT_XON   = 0x01,
	PTP_FRM_DATA_0          = 0x02,
	PTP_FRM_DATA_1          = 0x03,
	PTP_FRM_DATA_ACK_0_XOFF = 0x04,
	PTP_FRM_DATA_ACK_1_XOFF = 0x05,
	PTP_FRM_DATA_ACK_0_XON  = 0x06,
	PTP_FRM_DATA_ACK_1_XON  = 0x07,
	PTP_FRM_DATA_NAK_0_XOFF = 0x08,
	PTP_FRM_DATA_NAK_1_XOFF = 0x09,
	PTP_FRM_DATA_NAK_0_XON  = 0x0a,
	PTP_FRM_DATA_NAK_1_XON  = 0x0b,
	PTP_FRM_CONNECT_REQ     = 0x0c,
	PTP_FRM_CONNECT_RESP    = 0x0d,
	PTP_FRM_DISCONNECT_REQ  = 0x0e,
	PTP_FRM_DISCONNECT_RESP = 0x0f,
	PTP_FRM_TEST_REQ        = 0x14,
	PTP_FRM_TEST_RESP       = 0x15
};

struct bacnet_ptp_hdr {
	uint16_t preamble;
	uint8_t type;
	uint16_t len;
	uint8_t crc;
};

struct bacnet_ptp_frm {
	struct bacnet_ptp_hdr hdr;
	uint8_t data[];
};


/* PTP Connection State Machine states */
enum {
	BACNET_PTP_DISCONNECTED = 0,
	BACNET_PTP_DISCONNECTING,
	BACNET_PTP_INBOUND,
	BACNET_PTP_OUTBOUND,
	BACNET_PTP_CONNECTED
};

#define DLE  0x10
#define XON  0x11
#define XOFF 0x13

#define TX_IDLE_HBEAT_TMO_MS 15000
#define RX_IDLE_DISC_TMO_MS 30000
#define CONN_RXMT_TIME_MS  5000
#define DISC_RXMT_TIME_MS  5000
#define NPDU_RXMT_TIME_MS  1000

#define PTP_LINK_RXMT_MAX 3

static void bacnet_ptp_lnk_rx_rst(struct bacnet_ptp_lnk * lnk)
{
	lnk->rx.off = 0;
	lnk->rx.cnt = 0;
	lnk->rx.dle = false;
}

#if 0
int bacnet_ptp_frame_decode(uint8_t * frm, int frm_len)
{
	static uint8_t frm_buf[512];
	uint8_t * buf = frm_buf;
	bool dle = false;
	int off = 0;
	int cnt = 0;
	uint8_t * cp;
	int len;
	int pdu_len;
	
	for (;;) {
		uint8_t crc;

		/* move remaining octets to the beginning of the buffer */
		if (off) {
			DCC_LOG2(LOG_INFO, "shift off=%d cnt=%d", off, cnt);
			cnt = cnt - off;
			memcpy(buf, &buf[off], cnt);
		} else
			cnt = 0;

		off = 0;
		cp = &buf[cnt];

		/* receive a packet header */
		while (cnt < 5) {
			int i;
			int j;
			int n;
			int rem = BACNET_PTP_MTU - cnt;

			memcpy(buf, frm, frm_len);
			n = frm_len;
			if (n <= 0) {
				DCC_LOG1(LOG_INFO, "serdrv_recv=%d", n);
				return n;
			}

			for (i = 0, j = 0; i < n; ++i) {
				int c = cp[i];

				/* byte destuff */
				if (dle) {
					DCC_LOG1(LOG_INFO, "DLE %02x", c);
					c &= 0x7f;
					dle = false;
				} else if (c == DLE) {
					dle = true;
					continue;
				} 

				cp[j++] = c;
				DCC_LOG1(LOG_INFO, "%02x", c);
				dle = false;
			}

			cp += j;
			cnt += j;
		}

		if (buf[0] != 0x55) {
			DCC_LOG(LOG_WARNING, "frame error 1");
			off = 1;
			continue;
		}
		if (buf[1] != 0xff) {
			DCC_LOG(LOG_WARNING, "frame error 2");
			off = 2;
			continue;
		}

		crc = ~bacnet_crc8(0xff, &buf[2], 3);

		if (buf[5] != crc) {
			DCC_LOG2(LOG_WARNING, "CRC error: %02x != %02x", buf[5], crc);
			off = 3;
			continue;
		}

		break;
	}

	len = (buf[3] << 8) + buf[4];
	DCC_LOG2(LOG_TRACE, "cnt=%d len=%d", cnt, len);

	if (len > 0) {
		uint16_t crc;
		uint16_t chk;
		int i;

		chk = (buf[len + 7] << 8) + buf[len + 6];
		crc = ~bacnet_crc16(0xffff, &buf[6], len);
		if (crc != chk) {
			DCC_LOG2(LOG_WARNING, "Data CRC error %04x != %04x", crc, chk);
			return -1;
		}
#if 0
		{
			int i;
			printf("RX:");
			for (i = 0; i < len + 8; ++i)
				printf(" %02x", buf[i]);
			printf("\n");
		}
#endif
		pdu_len = len;
		off = len + 8;
	} else {
#if 0
		{
			int i;
			printf("RX:");
			for (i = 0; i < 6; ++i)
				printf(" %02x", buf[i]);
			printf("\n");
		}
#endif
		pdu_len = 0;
		off = 6;
	}


	switch (buf[2]) {
	case PTP_FRM_HEARTBEAT_XOFF:
		DCC_LOG(LOG_TRACE, "HEARTBEAT_XOFF");
		break;
	case PTP_FRM_HEARTBEAT_XON:
		DCC_LOG(LOG_TRACE, "HEARTBEAT_XON");
		break;
	case PTP_FRM_DATA_0:
		DCC_LOG1(LOG_TRACE, "DATA_0(%d)", pdu_len);
		break;
	case PTP_FRM_DATA_1:
		DCC_LOG1(LOG_TRACE, "DATA_1(%d)", pdu_len);
		break;
	case PTP_FRM_DATA_ACK_0_XOFF:
		DCC_LOG(LOG_TRACE, "DATA_ACK_0_XOFF");
		break;
	case PTP_FRM_DATA_ACK_1_XOFF:
		DCC_LOG(LOG_TRACE, "DATA_ACK_1_XOFF");
		break;
	case PTP_FRM_DATA_ACK_0_XON:
		DCC_LOG(LOG_TRACE, "DATA_ACK_0_XON");
		break;
	case PTP_FRM_DATA_ACK_1_XON:
		DCC_LOG(LOG_TRACE, "DATA_ACK_1_XON");
		break;
	case PTP_FRM_DATA_NAK_0_XOFF:
		DCC_LOG(LOG_TRACE, "DATA_NAK_0_XOFF");
		break;
	case PTP_FRM_DATA_NAK_1_XOFF:
		DCC_LOG(LOG_TRACE, "DATA_NAK_1_XOFF");
		break;
	case PTP_FRM_DATA_NAK_0_XON:
		DCC_LOG(LOG_TRACE, "DATA_NAK_0_XON");
		break;
	case PTP_FRM_DATA_NAK_1_XON:
		DCC_LOG(LOG_TRACE, "DATA_NAK_1_XON");
		break;
	case PTP_FRM_CONNECT_REQ:
		DCC_LOG(LOG_TRACE, "CONNECT_REQ");
		break;
	case PTP_FRM_CONNECT_RESP:
		DCC_LOG(LOG_TRACE, "CONNECT_RESP");
		break;
	case PTP_FRM_DISCONNECT_REQ:
		DCC_LOG(LOG_TRACE, "DISCONNECT_REQ");
		break;
	case PTP_FRM_DISCONNECT_RESP:
		DCC_LOG(LOG_TRACE, "DISCONNECT_RESP");
		break;
	case PTP_FRM_TEST_REQ:
		DCC_LOG(LOG_TRACE, "TEST_REQ");
		break;
	case PTP_FRM_TEST_RESP:
		DCC_LOG(LOG_TRACE, "TEST_RESP");
		break;
	}

	return buf[2]; /* Frame Type */
}

#endif


int bacnet_ptp_frame_recv(struct bacnet_ptp_lnk * lnk, unsigned int tmo)
{
	bool dle = lnk->rx.dle;
	int off = lnk->rx.off;
	int cnt = lnk->rx.cnt;
	uint8_t * buf = lnk->rx.buf;
	uint8_t * cp;
	int len;
	
	for (;;) {
		uint8_t crc;

		/* move remaining octets to the beginning of the buffer */
		if (off) {
			DCC_LOG2(LOG_INFO, "shift off=%d cnt=%d", off, cnt);
			cnt = cnt - off;
			memcpy(buf, &buf[off], cnt);
		} else
			cnt = 0;

		off = 0;
		cp = &buf[cnt];

		/* receive a packet header */
		while (cnt < 6) {
			int i;
			int j;
			int n;
			int rem = BACNET_PTP_MTU - cnt;

			n = serial_recv(lnk->dev, cp, rem, tmo);
			if (n <= 0) {
				DCC_LOG1(LOG_INFO, "serdrv_recv=%d", n);
				bacnet_ptp_lnk_rx_rst(lnk);
				return n;
			}

			for (i = 0, j = 0; i < n; ++i) {
				int c = cp[i];

				/* byte destuff */
				if (dle) {
					DCC_LOG1(LOG_INFO, "DLE %02x", c);
					c &= 0x7f;
					dle = false;
				} else if (c == DLE) {
					dle = true;
					continue;
				} 

				cp[j++] = c;
				DCC_LOG1(LOG_INFO, "%02x", c);
				dle = false;
			}

			cp += j;
			cnt += j;
		}

		if (buf[0] != 0x55) {
			DCC_LOG(LOG_WARNING, "frame error 1");
			off = 1;
			continue;
		}
		if (buf[1] != 0xff) {
			DCC_LOG(LOG_WARNING, "frame error 2");
			off = 2;
			continue;
		}

		crc = ~bacnet_crc8(0xff, &buf[2], 3);

		if (buf[5] != crc) {
			DCC_LOG2(LOG_WARNING, "CRC error: %02x != %02x", buf[5], crc);
			off = 2;
			continue;
		}

		break;
	}

	len = (buf[3] << 8) + buf[4];
	DCC_LOG2(LOG_INFO, "cnt=%d len=%d", cnt, len);

	if (len > 0) {
		uint16_t crc;
		uint16_t chk;
		int i;

		/* receive the frame data */
		while (cnt < len + 8) {
			int j;
			int n;
			int rem = BACNET_PTP_MTU - cnt;

			n = serial_recv(lnk->dev, cp, rem, tmo);
			if (n <= 0) {
				DCC_LOG1(LOG_WARNING, "serdrv_recv=%d", n);
				bacnet_ptp_lnk_rx_rst(lnk);
				return n;
			}

			for (i = 0, j = 0; i < n; ++i) {
				int c = cp[i];

				/* byte destuff */
				if (dle) {
					DCC_LOG1(LOG_INFO, "DLE %02x", c);
					c &= 0x7f;
					dle = false;
				} else if (c == DLE) {
					dle = true;
					continue;
				} 

				cp[j++] = c;
				DCC_LOG1(LOG_INFO, "%02x", c);
			}
			cp += j;
			cnt += j;
		}

		chk = (buf[len + 7] << 8) + buf[len + 6];
		crc = ~bacnet_crc16(0xffff, &buf[6], len);
		if (crc != chk) {
			DCC_LOG2(LOG_WARNING, "Data CRC error %04x != %04x", crc, chk);
			bacnet_ptp_lnk_rx_rst(lnk);
			return -1;
		}

#if 0
		{
			int i;
			printf("RX:");
			for (i = 0; i < len + 8; ++i)
				printf(" %02x", buf[i]);
			printf("\n");
		}
#endif
		lnk->rx.pdu_len = len;
		off = len + 8;
	} else {
#if 0
		{
			int i;
			printf("RX:");
			for (i = 0; i < 6; ++i)
				printf(" %02x", buf[i]);
			printf("\n");
		}
#endif
		lnk->rx.pdu_len = 0;
		off = 6;
	}

	lnk->rx.dle = dle;
	lnk->rx.off = off;
	lnk->rx.cnt = cnt;

	switch (buf[2]) {
	case PTP_FRM_HEARTBEAT_XOFF:
		DCC_LOG(LOG_INFO, "HEARTBEAT_XOFF");
		break;
	case PTP_FRM_HEARTBEAT_XON:
		DCC_LOG(LOG_INFO, "HEARTBEAT_XON");
		break;
	case PTP_FRM_DATA_0:
		DCC_LOG1(LOG_INFO, "DATA_0(%d)", len);
		break;
	case PTP_FRM_DATA_1:
		DCC_LOG1(LOG_INFO, "DATA_1(%d)", len);
		break;
	case PTP_FRM_DATA_ACK_0_XOFF:
		DCC_LOG(LOG_INFO, "DATA_ACK_0_XOFF");
		break;
	case PTP_FRM_DATA_ACK_1_XOFF:
		DCC_LOG(LOG_INFO, "DATA_ACK_1_XOFF");
		break;
	case PTP_FRM_DATA_ACK_0_XON:
		DCC_LOG(LOG_INFO, "DATA_ACK_0_XON");
		break;
	case PTP_FRM_DATA_ACK_1_XON:
		DCC_LOG(LOG_INFO, "DATA_ACK_1_XON");
		break;
	case PTP_FRM_DATA_NAK_0_XOFF:
		DCC_LOG(LOG_INFO, "DATA_NAK_0_XOFF");
		break;
	case PTP_FRM_DATA_NAK_1_XOFF:
		DCC_LOG(LOG_INFO, "DATA_NAK_1_XOFF");
		break;
	case PTP_FRM_DATA_NAK_0_XON:
		DCC_LOG(LOG_INFO, "DATA_NAK_0_XON");
		break;
	case PTP_FRM_DATA_NAK_1_XON:
		DCC_LOG(LOG_INFO, "DATA_NAK_1_XON");
		break;
	case PTP_FRM_CONNECT_REQ:
		DCC_LOG(LOG_INFO, "CONNECT_REQ");
		break;
	case PTP_FRM_CONNECT_RESP:
		DCC_LOG(LOG_INFO, "CONNECT_RESP");
		break;
	case PTP_FRM_DISCONNECT_REQ:
		DCC_LOG(LOG_INFO, "DISCONNECT_REQ");
		break;
	case PTP_FRM_DISCONNECT_RESP:
		DCC_LOG(LOG_INFO, "DISCONNECT_RESP");
		break;
	case PTP_FRM_TEST_REQ:
		DCC_LOG(LOG_INFO, "TEST_REQ");
		break;
	case PTP_FRM_TEST_RESP:
		DCC_LOG(LOG_INFO, "TEST_RESP");
		break;
	}

	return buf[2]; /* Frame Type */
}

int bacnet_ptp_frame_send(struct bacnet_ptp_lnk * lnk, int typ, 
						uint8_t * pdu, unsigned int len)
{
	uint8_t hdr[6];
	uint8_t buf[9 + 2 * len];
	int cnt;
	int i;
	int j;
	int c;

	/* reset TX idle timer */
	lnk->tx.idle_tmr = lnk->clk + TX_IDLE_HBEAT_TMO_MS; 

	hdr[0] = 0x55;
	hdr[1] = 0xff;
	hdr[2] = typ;
	hdr[3] = (len >> 8) & 0xff;
	hdr[4] = len & 0xff;
	hdr[5] = ~bacnet_crc8(0xff, &hdr[2], 3);

	/* encode header */
	for (i = 0, j = 0; i < 6; ++i) {
		c = hdr[i];
		if ((c == DLE) || (c == XON) || (c == XOFF)) {
			buf[j++] = DLE;
			c |= 0x80;
		}
		buf[j++] = c;
	}
	cnt = j;

	if (len > 0) {
		uint16_t crc;

		crc = ~bacnet_crc16(0xffff, pdu, len);
		for (i = 0, j = cnt; i < len; ++i) {
			c = pdu[i];
			if ((c == DLE) || (c == XON) || (c == XOFF)) {
				buf[j++] = DLE;
				c |= 0x80;
			}
			buf[j++] = c;
		}
		c = (crc & 0xff);
		if ((c == DLE) || (c == XON) || (c == XOFF)) {
			buf[j++] = DLE;
			c |= 0x80;
		}
		buf[j++] = c;

		c = (crc >> 8) & 0xff;
		if ((c == DLE) || (c == XON) || (c == XOFF)) {
			buf[j++] = DLE;
			c |= 0x80;
		}
		buf[j++] = c;

		cnt = j;
	} 

#if 0
	printf("TX:");
	for (i = 0; i < cnt; ++i) {
		c = buf[i];
		printf(" %02x", c);
	}
	printf("\n");
#endif

	switch (typ) {
	case PTP_FRM_HEARTBEAT_XOFF:
		DCC_LOG(LOG_INFO, "HEARTBEAT_XOFF");
		break;
	case PTP_FRM_HEARTBEAT_XON:
		DCC_LOG(LOG_INFO, "HEARTBEAT_XON");
		break;
	case PTP_FRM_DATA_0:
		DCC_LOG1(LOG_INFO, "DATA_0(%d)", len);
		break;
	case PTP_FRM_DATA_1:
		DCC_LOG1(LOG_INFO, "DATA_1(%d)", len);
		break;
	case PTP_FRM_DATA_ACK_0_XOFF:
		DCC_LOG(LOG_INFO, "DATA_ACK_0_XOFF");
		break;
	case PTP_FRM_DATA_ACK_1_XOFF:
		DCC_LOG(LOG_INFO, "DATA_ACK_1_XOFF");
		break;
	case PTP_FRM_DATA_ACK_0_XON:
		DCC_LOG(LOG_INFO, "DATA_ACK_0_XON");
		break;
	case PTP_FRM_DATA_ACK_1_XON:
		DCC_LOG(LOG_INFO, "DATA_ACK_1_XON");
		break;
	case PTP_FRM_DATA_NAK_0_XOFF:
		DCC_LOG(LOG_INFO, "DATA_NAK_0_XOFF");
		break;
	case PTP_FRM_DATA_NAK_1_XOFF:
		DCC_LOG(LOG_INFO, "DATA_NAK_1_XOFF");
		break;
	case PTP_FRM_DATA_NAK_0_XON:
		DCC_LOG(LOG_INFO, "DATA_NAK_0_XON");
		break;
	case PTP_FRM_DATA_NAK_1_XON:
		DCC_LOG(LOG_INFO, "DATA_NAK_1_XON");
		break;
	case PTP_FRM_CONNECT_REQ:
		DCC_LOG(LOG_INFO, "CONNECT_REQ");
		break;
	case PTP_FRM_CONNECT_RESP:
		DCC_LOG(LOG_INFO, "CONNECT_RESP");
		break;
	case PTP_FRM_DISCONNECT_REQ:
		DCC_LOG(LOG_INFO, "DISCONNECT_REQ");
		break;
	case PTP_FRM_DISCONNECT_RESP:
		DCC_LOG(LOG_INFO, "DISCONNECT_RESP");
		break;
	case PTP_FRM_TEST_REQ:
		DCC_LOG(LOG_INFO, "TEST_REQ");
		break;
	case PTP_FRM_TEST_RESP:
		DCC_LOG(LOG_INFO, "TEST_RESP");
		break;
	}

	return serial_send(lnk->dev, buf, cnt);
}

int bacnet_ptp_loop(struct bacnet_ptp_lnk * lnk)
{
	uint8_t * pdu;
	int pdu_len;
	uint32_t clk;
	int typ;
	int len;

	lnk->tx.xon = false;
	lnk->tx.seq = 1;
	lnk->tx.len = 0;
	thinkos_flag_give(lnk->rx.flag);

	lnk->rx.xon = false;
	lnk->rx.seq = 1;
	lnk->rx.pdu_len = 0;
	thinkos_flag_give(lnk->tx.flag);

	while (lnk->state == BACNET_PTP_DISCONNECTED) {
		/* In this state, the device waits for the network layer to 
		   initiate a PTP data link connection or for the physical 
		   layer to indicate the occurrence of a physical 
		   layer connection. */
		thinkos_sleep(100);
		clk = thinkos_clock();
		lnk->clk = clk;
	}

	lnk->rx.idle_tmr = lnk->clk + RX_IDLE_DISC_TMO_MS; 

again:
	typ = bacnet_ptp_frame_recv(lnk, 100);
	clk = thinkos_clock();
	lnk->clk = clk;

	if (typ > 0) {
#if 0
		int len = *pdu_len;
		if (len) {
			uint8_t * cp = *pdu;
			int i;
			printf("RX PDU: ");
			for (i = 0; i < len; ++i)
				printf(" %02x", cp[i]);
			printf("\n");
		}
#endif
		/* Reset IDLE timer */
		lnk->rx.idle_tmr = clk + RX_IDLE_DISC_TMO_MS; 
	} 
	
	switch (lnk->state) {
	case BACNET_PTP_DISCONNECTED:
		DCC_LOG(LOG_TRACE, "[DISCONNECTED]");
		return 0;

	case BACNET_PTP_INBOUND:
		/* In this state, the Connection State Machine has 
		   recognized that the calling device wishes to establish a 
		   BACnet connection, and the local device is waiting for a 
		   Connect Response frame from the calling device. */
		if (typ == PTP_FRM_CONNECT_RESP) {
			lnk->state = BACNET_PTP_CONNECTED;
			DCC_LOG(LOG_TRACE, "[CONNECTED]");
			bacnet_ptp_frame_send(lnk, PTP_FRM_HEARTBEAT_XON,
								NULL, 0);
			/* start the cyclic 1 second timer for DCC */
//			timer_interval_start_seconds(&DCC_Timer, DCC_CYCLE_SECONDS);
			break;
		} 

		if (typ == PTP_FRM_DISCONNECT_REQ) {
			bacnet_ptp_frame_send(lnk, PTP_FRM_DISCONNECT_RESP,
								NULL, 0);
			lnk->state = BACNET_PTP_DISCONNECTED;
			DCC_LOG(LOG_TRACE, "[DISCONNECTED]");
			return 0;
		}

		if (typ == THINKOS_ETIMEDOUT) {
			if (((int32_t)(lnk->tx.rxmt_tmr - clk)) < 0) {
				if (++lnk->tx.rxmt_cnt < PTP_LINK_RXMT_MAX) {
					lnk->tx.rxmt_tmr = clk + CONN_RXMT_TIME_MS; 
					bacnet_ptp_frame_send(lnk, PTP_FRM_CONNECT_REQ, 
										  NULL, 0);
					DCC_LOG(LOG_TRACE, "[INBOUND]");
				} else {
					lnk->state = BACNET_PTP_DISCONNECTED;
					DCC_LOG(LOG_TRACE, "[DISCONNECTED]");
					return 0;
				} 
			}
		}
		break;

	case BACNET_PTP_OUTBOUND:
		/* FIXME: not implemented ... */
		break;

	case BACNET_PTP_CONNECTED:
		switch (typ) {
		case PTP_FRM_HEARTBEAT_XOFF:
			if (lnk->tx.xon) {
				DCC_LOG(LOG_TRACE, "[XOFF]");
				lnk->tx.xon = false;
			}
			break;

		case PTP_FRM_HEARTBEAT_XON:
			if (!lnk->tx.xon) {
				DCC_LOG(LOG_TRACE, "[XON]");
				lnk->tx.xon = true;
			}
			break;

		case PTP_FRM_DATA_0:
			/* FIXME: check for correct sequence number */
			bacnet_ptp_frame_send(lnk, PTP_FRM_DATA_ACK_0_XON, NULL, 0);
			thinkos_flag_give(lnk->rx.flag);
			bacnet_dl_pdu_recv_notify(lnk->dln);
			break;

		case PTP_FRM_DATA_1:
			/* FIXME: check for correct sequence number */
			bacnet_ptp_frame_send(lnk, PTP_FRM_DATA_ACK_1_XON, NULL, 0);
			thinkos_flag_give(lnk->rx.flag);
			bacnet_dl_pdu_recv_notify(lnk->dln);
			break;

		case PTP_FRM_DATA_ACK_0_XOFF:
			break;
		case PTP_FRM_DATA_ACK_1_XOFF:
			break;

		case PTP_FRM_DATA_ACK_0_XON:
			if ((lnk->tx.seq & 1) == 0) {
				lnk->tx.seq++;
				lnk->tx.len = 0;
				thinkos_flag_give(lnk->tx.flag);
			}
			break;

		case PTP_FRM_DATA_ACK_1_XON:
			if ((lnk->tx.seq & 1) == 1) {
				lnk->tx.seq++;
				lnk->tx.len = 0;
				thinkos_flag_give(lnk->tx.flag);
			}	
			break;

		case PTP_FRM_DATA_NAK_0_XOFF:
			break;

		case PTP_FRM_DATA_NAK_1_XOFF:
			break;

		case PTP_FRM_DATA_NAK_0_XON:
			if ((lnk->tx.seq & 1) == 0)
				bacnet_ptp_frame_send(lnk, PTP_FRM_DATA_0,
									  lnk->tx.buf, lnk->tx.len);
			break;

		case PTP_FRM_DATA_NAK_1_XON:
			if ((lnk->tx.seq & 1) == 1)
				bacnet_ptp_frame_send(lnk, PTP_FRM_DATA_1,
									  lnk->tx.buf, lnk->tx.len);
			break;

		case PTP_FRM_CONNECT_REQ:
			bacnet_ptp_frame_send(lnk, PTP_FRM_CONNECT_RESP, NULL, 0);
			break;

		case PTP_FRM_CONNECT_RESP:
			DCC_LOG(LOG_TRACE, "???");
			break;

		case PTP_FRM_DISCONNECT_REQ:
			bacnet_ptp_frame_send(lnk, PTP_FRM_DISCONNECT_RESP,
								NULL, 0);
			lnk->state = BACNET_PTP_DISCONNECTED;
			DCC_LOG(LOG_TRACE, "[DISCONNECTED]");
			return 0;

		case PTP_FRM_DISCONNECT_RESP:
			break;

		case PTP_FRM_TEST_REQ:
			break;

		case PTP_FRM_TEST_RESP:
			break;

		case THINKOS_ETIMEDOUT:
			if (lnk->tx.len) {
				/* data pending on the TX buffer */
				if (((int32_t)(lnk->tx.rxmt_tmr - clk)) < 0) {
					if (++lnk->tx.rxmt_cnt < PTP_LINK_RXMT_MAX) {
						/* restart retransmission timer */
						lnk->tx.rxmt_tmr = clk + NPDU_RXMT_TIME_MS; 
						if (lnk->tx.xon) {
							DCC_LOG(LOG_TRACE, "retransmitting...");
							bacnet_ptp_frame_send(lnk, (lnk->tx.seq & 1) ? 
												  PTP_FRM_DATA_1 :
												  PTP_FRM_DATA_0,
												  lnk->tx.buf, lnk->tx.len);
						}
					} else {
						DCC_LOG(LOG_TRACE, "too many retries giving up...");
						lnk->tx.len = 0;
						thinkos_flag_give(lnk->tx.flag);
					}
				} 
			}
			if (((int32_t)(lnk->rx.idle_tmr - clk)) < 0) {
				DCC_LOG(LOG_TRACE, "RX IDLE timeout");
				uint8_t code = 0x01;
				/* IDLE timeout */
				lnk->tx.rxmt_tmr = clk + DISC_RXMT_TIME_MS; 
				lnk->tx.rxmt_cnt = 0;
				bacnet_ptp_frame_send(lnk, PTP_FRM_DISCONNECT_REQ,
									  &code, 1);

				lnk->state = BACNET_PTP_DISCONNECTING;
				DCC_LOG(LOG_TRACE, "[DISCONNECTING]");
			}
			if (((int32_t)(lnk->tx.idle_tmr - clk)) < 0) {
				DCC_LOG(LOG_INFO, "TX IDLE timeout");
				bacnet_ptp_frame_send(lnk, PTP_FRM_HEARTBEAT_XON, 
									  NULL, 0);
			}
			break;
		}
		break;

	case BACNET_PTP_DISCONNECTING:

		if (typ == PTP_FRM_DISCONNECT_RESP) {
			lnk->state = BACNET_PTP_DISCONNECTED;
			DCC_LOG(LOG_TRACE, "[DISCONNECTED]");
			return 0;
		}

		if (typ == THINKOS_ETIMEDOUT) {
			if (((int32_t)(lnk->tx.rxmt_tmr - clk)) < 0) {
				if (++lnk->tx.rxmt_cnt < PTP_LINK_RXMT_MAX) {
					uint8_t code = 0x02;
					bacnet_ptp_frame_send(lnk, PTP_FRM_DISCONNECT_REQ,
										  &code, 1);
					DCC_LOG(LOG_TRACE, "[DISCONNECTING]");
					lnk->tx.rxmt_tmr = clk + DISC_RXMT_TIME_MS; 
				} else {
					lnk->state = BACNET_PTP_DISCONNECTED;
					DCC_LOG(LOG_TRACE, "[DISCONNECTED]");
					return 0;
				} 
			}
		}

		break;
	}

	goto again;
}

int bacnet_ptp_recv(struct bacnet_ptp_lnk * lnk, uint8_t pdu[], 
					unsigned int max)
{
	int pdu_len;

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

int bacnet_ptp_send(struct bacnet_ptp_lnk * lnk, const uint8_t pdu[], 
					unsigned int len)
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
							  PTP_FRM_DATA_1 :
							  PTP_FRM_DATA_0,
							  lnk->tx.buf, lnk->tx.len);
	}

	return len;
}

const struct bacnetdl_op bacnet_ptp_op = {
	.recv = (void *)bacnet_ptp_recv,
	.send = (void *)bacnet_ptp_send
};

int bacnet_ptp_init(struct bacnet_ptp_lnk * lnk, 
					const char * name, 
					struct serial_dev * dev)
{
	uint32_t clk;

	memset(lnk, 0, sizeof(struct bacnet_ptp_lnk));

	lnk->dev = dev;
	lnk->state = BACNET_PTP_DISCONNECTED;
	lnk->rx.seq = 0;
	lnk->tx.seq = 0;
	lnk->tx.xon = false;
	lnk->rx.xon = false;

	DCC_LOG(LOG_TRACE, "[DISCONNECTED]");
	bacnet_ptp_lnk_rx_rst(lnk);
	lnk->rx.flag = thinkos_flag_alloc();
	lnk->tx.flag = thinkos_flag_alloc();

	DCC_LOG2(LOG_TRACE, "tx.flag=%d rx.flag=%d", lnk->rx.flag, lnk->tx.flag);

	clk = thinkos_clock();
	lnk->clk = clk;
	lnk->rx.idle_tmr = clk + RX_IDLE_DISC_TMO_MS; 

	lnk->dln = bacnet_dl_register(name, lnk, &bacnet_ptp_op);

	return 0;
}

int bacnet_ptp_outbound(struct bacnet_ptp_lnk * lnk)
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
	bacnet_ptp_frame_send(lnk, PTP_FRM_CONNECT_REQ, NULL, 0);

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

