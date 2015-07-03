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
 * @file xmodem_recv.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#include <sys/param.h>
#include <stdlib.h>
#include <errno.h>

#include <xmodem.h>
#include <crc.h>

#define SOH  0x01
#define STX  0x02
#define EOT  0x04
#define ACK  0x06
#define NAK  0x15
#define CAN  0x18

#define XMODEM_RCV_TMOUT_MS 2000

#ifndef MIN
#define MIN(a,b)    (((a)<(b))?(a):(b))
#endif


static int xmodem_recv_pkt(struct xmodem_recv * rx)
{
	unsigned char * pkt = rx->pkt.hdr;
	unsigned char * cp;
	int ret = 0;
	int cnt = 0;
	int nseq;
	int seq;
	int rem;

	for (;;) {

		if ((ret = serial_send(rx->dev, &rx->sync, 1)) < 0) {
			return ret;
		}

		for (;;) {
			int c;

			ret = serial_recv(rx->dev, pkt, 
									1, XMODEM_RCV_TMOUT_MS);

			if (ret == 0)
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
				if ((ret = serial_send(rx->dev, pkt, 1)) < 0)
					return ret;

				return 0;
			}
		}

		rem = cnt + ((rx->fcs_mode == FCS_CRC) ? 4 : 3);
		cp = pkt + 1;


		/* receive the packet */
		while (rem) {

			ret = serial_recv(rx->dev, cp, rem, 500);
			if (ret == 0)
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

		if (rx->fcs_mode == FCS_CRC) {
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
		while (serial_recv(rx->dev, pkt, 1024, 200) > 0);
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

	serial_send(rx->dev, pkt, 3);

	return ret;
}

int xmodem_recv_loop(struct xmodem_recv * rx, void * data, int len)
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

		ret = xmodem_recv_pkt(rx);

	} while (ret > 0);

	return ret;
}



int xmodem_recv_init(struct xmodem_recv * rx, const struct serial_dev * dev, 
					int fcs_mode, int xfr_mode)
{
	if ((rx == NULL) || (dev == NULL) || (xfr_mode > MODE_YMODEM))
		return -EINVAL;

	rx->dev = (struct serial_dev *)dev;

	rx->fcs_mode = fcs_mode;
	rx->xfr_mode = xfr_mode;
	rx->pktno = (xfr_mode == MODE_YMODEM) ? 0 : 1;
	rx->sync = (rx->fcs_mode == FCS_CRC) ? 'C' : NAK;
	rx->retry = 30;
	rx->data_len = 0;
	rx->data_pos = 0;
	rx->fsize = 64 * 1024 * 1024;

	return 0;
}

int xmodem_recv_cancel(struct xmodem_recv * rx)
{
	unsigned char * pkt = rx->pkt.hdr;

	pkt[0] = CAN;
	pkt[1] = CAN;
	pkt[2] = CAN;

	serial_send(rx->dev, pkt, 3);

	return 0;
}


struct xmodem_recv * xmodem_recv_alloc(void)
{
	struct xmodem_recv * rx;
	
	rx = (struct xmodem_recv *)malloc(sizeof(struct xmodem_recv));

	return rx;
}

void xmodem_recv_free(struct xmodem_recv * rx)
{
	free(rx);
}


