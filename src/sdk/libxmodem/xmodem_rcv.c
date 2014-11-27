/* 
 * Copyright(C) 2012-2014 Robinson Mittmann. All Rights Reserved.
 * 
 * This file is part of the YARD-ICE.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You can receive a copy of the GNU Lesser General Public License from 
 * http://www.gnu.org/
 */

/** 
 * @file xmodem_rcv.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#include <sys/param.h>
#include <stdlib.h>
#include <errno.h>
#include <thinkos.h>

#include <xmodem.h>
#include <crc.h>

#include <sys/dcclog.h>

#define SOH  0x01
#define STX  0x02
#define EOT  0x04
#define ACK  0x06
#define NAK  0x15
#define CAN  0x18

#define XMODEM_RCV_TMOUT_MS 2000

int xmodem_rcv_init(struct xmodem_rcv * rx, const struct comm_dev * comm, 
					int mode)
{
	if ((rx == NULL) || (comm == NULL) || (mode > XMODEM_RCV_CRC))
		return -EINVAL;

	rx->comm = comm;

	rx->mode = mode;
	rx->pktno = 1;
	rx->sync = (rx->mode == XMODEM_RCV_CRC) ? 'C' : NAK;
	rx->retry = 30;
	rx->data_len = 0;
	rx->data_pos = 0;

	return 0;
}

int xmodem_rcv_cancel(struct xmodem_rcv * rx)
{
	unsigned char * pkt = rx->pkt.hdr;

	pkt[0] = CAN;
	pkt[1] = CAN;
	pkt[2] = CAN;

	rx->comm->op.send(rx->comm->arg, pkt, 3);

	return 0;
}

static int xmodem_rcv_pkt(struct xmodem_rcv * rx)
{
	unsigned char * pkt = rx->pkt.hdr;
	unsigned char * cp;
	int ret = 0;
	int cnt = 0;
	int nseq;
	int seq;
	int rem;

	for (;;) {

		if ((ret = rx->comm->op.send(rx->comm->arg, &rx->sync, 1)) < 0) {
			return ret;
		}

		for (;;) {
			int c;

			ret = rx->comm->op.recv(rx->comm->arg, pkt, 
									1, XMODEM_RCV_TMOUT_MS);

			if (ret == THINKOS_ETIMEDOUT)
				goto timeout;

			if (ret < 0)
				return ret;

			c = pkt[0];

			if (c == STX) {
				cnt = 1024;
				break;
			}

			if (c == SOH) {
				cnt = 128;
				break;
			}

			if (c == EOT) {
				/* end of transmission */
				pkt[0] = ACK;
				if ((ret = rx->comm->op.send(rx->comm->arg, pkt, 1)) < 0)
					return ret;

				return 0;
			}
		}

		rem = cnt + ((rx->mode) ? 4 : 3);
		cp = pkt + 1;


		/* receive the packet */
		while (rem) {

			ret = rx->comm->op.recv(rx->comm->arg, cp, rem, 500);
			if (ret == THINKOS_ETIMEDOUT)
				goto timeout;
			if (ret < 0)
				return ret;

			rem -= ret;
			cp += ret;
		}

		/* sequence */
		seq = pkt[1];
		/* inverse sequence */
		nseq = pkt[2];

		if (seq != ((~nseq) & 0xff)) {
			goto error;
		}

		cp = &pkt[3];

		if (rx->mode) {
			unsigned short crc = 0;
			unsigned short cmp;
			int i;

			for (i = 0; i < cnt; ++i)
				crc = CRC16CCITT(crc, cp[i]);

			cmp = (unsigned short)cp[i] << 8 | cp[i + 1];

			if (cmp != crc) {
				goto error;
			}

		} else {
			unsigned char cks = 0;
			int i;

			for (i = 0; i < cnt; ++i)
				cks += cp[i];

			if (cp[i] != cks)
				goto error;
		}


		if (seq == ((rx->pktno - 1) & 0xff)) {
			/* retransmission */
			rx->sync = ACK;
			continue;
		}

		if (seq != rx->pktno) {
			goto error;
		}

		rx->pktno = (rx->pktno + 1) & 0xff;
		rx->retry = 10;
		rx->sync = ACK;
		rx->data_len = cnt;
		rx->data_pos = 0;

		return cnt;

error:
		/* flush */
		while (rx->comm->op.recv(rx->comm->arg, pkt, 1024, 200) > 0);
		rx->sync = NAK;

timeout:

		if ((--rx->retry) == 0) {
			/* too many errors */
			ret = -1;
			break;
		}
	}


	pkt[0] = CAN;
	pkt[1] = CAN;
	pkt[2] = CAN;

	rx->comm->op.send(rx->comm->arg, pkt, 3);

	return ret;
}

int xmodem_rcv_loop(struct xmodem_rcv * rx, void * data, int len)
{
	unsigned char * dst = (unsigned char *)data;
	int rem;
	int ret;

	if ((dst == NULL) || (len <= 0)) {
		return -EINVAL;
	}

	do {
		if ((rem = (rx->data_len - rx->data_pos)) > 0) {
			unsigned char * src;
			int n;
			int i;

			n = MIN(rem, len);
			src = &rx->pkt.data[rx->data_pos];

			for (i = 0; i < n; ++i)
				dst[i] = src[i];

			rx->data_pos += n;

			return n;
		}

		ret = xmodem_rcv_pkt(rx);

	} while (ret > 0);

	return ret;
}


