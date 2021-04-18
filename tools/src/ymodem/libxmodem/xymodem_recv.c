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

#include "xmodem.h"
#include "crc.h"
#include "debug.h"

#define SOH  0x01
#define STX  0x02
#define EOT  0x04
#define ACK  0x06
#define NAK  0x15
#define CAN  0x18

#define XMODEM_RCV_TMOUT_MS 2000
#define XMODEM_FILE_SIZE_MAX (64 * 1024 * 1024)

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
			DBG(DBG_WARNING, "serial_send() failed!");
			return ret;
		}

		if (rx->sync == NAK)
			DBG(DBG_TRACE, "--> NAK");
		else if (rx->sync == ACK)
			DBG(DBG_TRACE, "--> ACK");
		else if (rx->sync == 'C')
			DBG(DBG_TRACE, "--> 'C'");
		else
			DBG(DBG_WARNING, "--> 0x%02x", rx->sync);

		for (;;) {
			int c;

			ret = serial_recv(rx->dev, pkt, 
									1, XMODEM_RCV_TMOUT_MS);

			if (ret == 0) {
				DBG(DBG_TRACE, "serial_send() timeout!");
				goto timeout;
			}

			if (ret < 0) {
				DBG(DBG_WARNING, "serial_send() failed!");
				return ret;
			}

			c = pkt[0];

			if (c == STX) {
				DBG(DBG_TRACE, "<-- STX");
				cnt = 1024;
				break;
			}

			if (c == SOH) {
				DBG(DBG_TRACE, "<-- SOH");
				cnt = 128;
				break;
			}

			if (c == CAN) {
				DBG(DBG_WARNING, "<-- CAN");
				return -1;
			}

			if (c == EOT) {
				DBG(DBG_TRACE, "<-- EOT");
				/* end of transmission */
				rx->pktno = (rx->xfr_mode == MODE_YMODEM) ? 0 : 1;
				rx->sync = (rx->fcs_mode == FCS_CRC) ? 'C' : NAK;
				pkt[0] = ACK;
				if ((ret = serial_send(rx->dev, pkt, 1)) < 0)
					return ret;

				DBG(DBG_TRACE, "--> ACK");

				return 0;
			}
		}

		rem = cnt + ((rx->fcs_mode == FCS_CRC) ? 4 : 3);
		cp = pkt + 1;

		/* receive the packet */
		while (rem) {
//			ret = serial_recv(rx->dev, cp, rem > 8 ? 8 : rem, 500);
			ret = serial_recv(rx->dev, cp, rem, 500);

			if (ret == 0) {
				DBG(DBG_TRACE, "serial_recv() timeout!");
				goto timeout;
			}

			if (ret < 0) {
				DBG(DBG_WARNING, "serial_recv() failed!");
				return ret;
			}

//			DBG_DUMP(DBG_TRACE, cp, ret);

			rem -= ret;
			cp += ret;
		}

		/* sequence */
		seq = pkt[1];
		/* inverse sequence */
		nseq = (~pkt[2]) & 0xff;

//		DBG_DUMP(DBG_TRACE, pkt, cnt + ((rx->fcs_mode == FCS_CRC) ? 5 : 4));

		if (seq != nseq) {
			DBG(DBG_WARNING, "invalid sequence %02x != %02x!", seq, nseq);
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
				DBG(DBG_WARNING, "CRC error %04x!=%04x!", cmp, crc);
				goto error;
			}

		} else {
			unsigned char cks = 0;
			int i;

			for (i = 0; i < cnt; ++i)
				cks += cp[i];

			if (cp[i] != cks) {
				DBG(DBG_WARNING, "Checksum error!");
				goto error;
			}
		}


		if (seq == ((rx->pktno - 1) & 0xff)) {
			/* retransmission */
			rx->sync = ACK;
			continue;
		}

		if (seq != (rx->pktno & 0xff)) {
			if ((rx->pktno == 0) && (seq == 1)) {
				rx->pktno++;
				/* Fallback to XMODEM */
				rx->xfr_mode = MODE_XMODEM_1K;
				rx->fsize = XMODEM_FILE_SIZE_MAX;
			} else {
				/* wrong sequence */
				DBG(DBG_WARNING, "wrong sequence!");
				goto error;
			}
		}

		/* YModem first packet ... */
		if (rx->pktno == 0) {
			pkt[0] = ACK;
			serial_send(rx->dev, pkt, 1);
		} else {
			rx->retry = 10;
			rx->sync = ACK;
			if ((rx->count + cnt) > rx->fsize)
				cnt = rx->fsize - rx->count;
			rx->count += cnt;
		}


		rx->pktno++;

		return cnt;

error:
		/* flush */
		while (serial_recv(rx->dev, rx->pkt.data, 1024, 200) > 0); 
		ret = -1;
		break;

timeout:

		if ((--rx->retry) == 0) {
			/* too many errors */
			ret = -1;
			break;
		}
	}

	pkt[0] = CAN;
	pkt[1] = CAN;

	serial_send(rx->dev, pkt, 2);

	return ret;
}

int xmodem_recv_loop(struct xmodem_recv * rx, void * data, int len)
{
	int ret;

	if ((data == NULL) || (len <= 0)) {
		return -1;
	}

	DBG(DBG_MSG, "len=%d.", len);

	for (;;) {
		int cnt;
		int rem;

		if ((rem = (rx->data_len - rx->data_pos)) > 0) {
			unsigned char * dst = (unsigned char *)data;
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
		DBG(DBG_INFO, "xmodem_recv_pkt()=%d.", ret);

		if (ret < 0) {
			break;
		}

		if ((ret == 0) && (rx->xfr_mode != MODE_YMODEM))
			break;

		cnt = ret;

		if (rx->pktno == 1) {
			char * src;
			char * dst;
			int fsize;

			DBG(DBG_TRACE, "YModem");

			src = (char *)rx->pkt.data;
			dst = (char *)rx->fname;
			while (*src != '\0')
				*dst++ = *src++;
			*dst = '\0';
			/* skip null */
			src++;
			fsize = atoi(src);
			rx->fsize = fsize;

			DBG(DBG_TRACE, "fname='%s' fsize=%d", rx->fname, rx->fsize);

			rx->count = 0;
			ret = 0;
			break;
		} 
		
		rx->data_len = cnt;
		rx->data_pos = 0;
	} 

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
	rx->pktno = (rx->xfr_mode == MODE_YMODEM) ? 0 : 1;
	rx->sync = (rx->fcs_mode == FCS_CRC) ? 'C' : NAK;
	rx->retry = 30;
	rx->data_len = 0;
	rx->data_pos = 0;
	rx->fsize = (rx->xfr_mode == MODE_YMODEM) ? 0 : XMODEM_FILE_SIZE_MAX;
	rx->fname[0] = '\0';
	rx->count = 0;

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

