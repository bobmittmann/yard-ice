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
 * @file xmodem_send.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#include <sys/param.h>
#include <stdlib.h>
#include <stdio.h>

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

#define XMODEM_SEND_TMOUT_MS 2000

#ifndef MIN
#define MIN(a,b)    (((a)<(b))?(a):(b))
#endif

enum {
	XMODEM_SEND_IDLE = 0,
	XMODEM_SEND_CRC = 1,
	XMODEM_SEND_CKS = 2
};

static int xmodem_send_pkt(struct xmodem_send * sx, int data_len)
{
	unsigned char * pkt = sx->pkt.hdr; 
	unsigned char * cp;
	int retry = 0;
	int pkt_len;
	int ret;
	int c;


	if (sx->state == XMODEM_SEND_IDLE) {

		for (;;) {

			// Wait for NAK or 'C'
			if ((ret = serial_recv(sx->dev, pkt, 
								   1, XMODEM_SEND_TMOUT_MS)) <= 0) {
				if (ret == 0) {
					DBG(DBG_TRACE, "serial_recv() timed out!");
					if (++retry < 20) 
						continue;
				}
				DBG(DBG_WARNING, "serial_recv() failed 1!");
				return ret;
			}
			c = *pkt;

			if (c == CAN) {
				DBG(DBG_WARNING, "<-- CAN");
				return -1;
			}

			if (c == 'C') {
				DBG(DBG_TRACE, "<-- 'C' (CRC mode)");
				sx->state = XMODEM_SEND_CRC;
				break;
			}

			if (c == NAK) {
				DBG(DBG_TRACE, "<-- NAK (Checksum mode)");
				sx->state = XMODEM_SEND_CKS;
				break;
			}

		}
	}


	if (data_len) {
		unsigned char * fcs;

		if (data_len == 1024)
			pkt[0] = STX;
		else if (data_len == 128)
			pkt[0] = SOH;
		else
			return -1;

		pkt[1] = sx->seq;
		pkt[2] = ~sx->seq;
		cp = &pkt[3];
		fcs = &pkt[3 + data_len];

		if (sx->state == XMODEM_SEND_CRC) {
			unsigned short crc = 0;
			int i;

			for (i = 0; i < data_len; ++i)
				crc = CRC16CCITT(crc, cp[i]);

			fcs[0] = crc >> 8;
			fcs[1] = crc & 0xff;
			pkt_len = 3 + data_len + 2;
		} else {
			unsigned char cks = 0;
			int i;

			for (i = 0; i < data_len; ++i)
				cks += cp[i];

			fcs[0] = cks;
			pkt_len = 3 + data_len + 1;
		}
	} else {
		pkt[0] = EOT;
		pkt_len = 1;
	}

	for (;;) {
		unsigned char buf[1];

//		DBG_DUMP(DBG_TRACE, pkt, data_len + 3);

		if (pkt[0] == STX) {
			DBG(DBG_TRACE, "--> STX %3d", pkt[1]);
		} else if (pkt[0] == SOH) {
			DBG(DBG_TRACE, "--> SOH %3d", pkt[1]);
		} else if (pkt[0] == EOT) {
			DBG(DBG_TRACE, "--> EOT");
		} 

		// Send packet
		if ((ret = serial_send(sx->dev, pkt, pkt_len)) < 0) {
			DBG(DBG_WARNING, "serial_send() failed!");
			return ret;
		}
		
		// Wait for ACK
		if ((ret = serial_recv(sx->dev, buf, 1, 500)) <= 0) {
			if (ret == 0) {
				DBG(DBG_TRACE, "serial_recv() timed out 2!");
				if (++retry == 10) {
					DBG(DBG_WARNING, "too many retries");
					continue;
				}
			} else
				DBG(DBG_WARNING, "serial_recv() failed 2!");

			ret = -1;
			goto error;
		}

		c = buf[0];

		if (c == ACK) {
			DBG(DBG_TRACE, "<-- ACK");
			break;
		}

		if (c == CAN) {
			DBG(DBG_WARNING, "<-- CAN");
			ret = -2;
			goto error;
		}

		if (c != NAK) {
			DBG(DBG_WARNING, "invalid response: 0x%02x '%c'", c, c);
			ret = -3;
			goto error;
		}

		DBG(DBG_TRACE, "<-- NAK");

		if (++retry == 10) {
			DBG(DBG_WARNING, "too many retries");
			ret = -4;
			goto error;
		}
	}

	sx->seq++;
	
	return 0;

error:
	/* flush */
	while (serial_recv(sx->dev, sx->pkt.data, 1024, 200) > 0); 

	return ret;
}

int xmodem_send_open(struct xmodem_send * sx, 
					 const struct serial_dev * dev, unsigned int mode)
{
	if ((sx == NULL) || (dev == NULL) || (mode > MODE_YMODEM))
		return -EINVAL;

	sx->dev = (struct serial_dev *)dev;
	sx->mode = mode;
	sx->data_max = (sx->mode != MODE_XMODEM) ? 1024 : 128;
	sx->data_len = 0;
	sx->seq = (sx->mode != MODE_YMODEM) ? 0 : 1;
	sx->state = XMODEM_SEND_IDLE;

	serial_drain(sx->dev);
	
	return 0;
}

int xmodem_send_start(struct xmodem_send * sx, const char * fname, 
					  unsigned int fsize)
{
	unsigned int timestamp = 0;
	unsigned char * data; 
	int max;
	int ret;
	int i;
	int len;


	if (sx->mode == MODE_YMODEM) {
		data = sx->pkt.data;
		data += sprintf((char *)data, "%s", fname);
		data++;
		data += sprintf((char *)data, "%d", fsize);
		data++;
		data += sprintf((char *)data, "%d", timestamp);
		data++;
		len = data - sx->pkt.data;
#if 1
		max = 1024;
#else
		max =  (sx->data_len < 128) ? 128 : sx->data_max;
#endif
		DBG(DBG_TRACE, "fname='%s' fsize=%d", fname, fsize);

		/* padding */
		for (i = 0; i < (max - len); ++i)
			data[i] = '\0';

		sx->seq = 0;
		if ((ret = xmodem_send_pkt(sx, max)) < 0)
			return ret;

		sx->state = XMODEM_SEND_IDLE;
		sx->data_len = 0;
	}

	return 0;
}

int xmodem_send_loop(struct xmodem_send * sx, const void * data, int len)
{
	unsigned char * src = (unsigned char *)data;

	if ((src == NULL) || (len < 0))
		return -EINVAL;

	DBG(DBG_INFO, "len=%d!", len);

	do {
		unsigned char * dst;
		int ret;
		int rem;
		int n;
		int i;

		dst = &sx->pkt.data[sx->data_len];
		rem = sx->data_max - sx->data_len;
		n = MIN(len, rem);

		for (i = 0; i < n; ++i)
			dst[i] = src[i];

		sx->data_len += n;

		if (sx->data_len == sx->data_max) {

			if ((ret = xmodem_send_pkt(sx, sx->data_len)) < 0) {
				DBG(DBG_WARNING, "xmodem_send_pkt() failed!");
				return ret;
			}

			sx->data_len = 0;
		}

		src += n;
		len -= n;
	} while (len);

	return 0;
}

int xmodem_send_eot(struct xmodem_send * sx)
{
	unsigned char * data;
	int data_len;
	int data_max;
	int ret;

	if ((data_len = sx->data_len) <= (1024 - 128))
		data_max = 128;
	else
		data_max = sx->data_max;
#if 1
	if (sx->mode == MODE_YMODEM)
		data_max = sx->data_max;
#endif

	data = sx->pkt.data;

	while (data_len > 0) {
		int len;
		int i;

		len = MIN(data_len, data_max);

		/* padding */
		for (i = len; i < data_max; ++i)
			data[i] = '\0';


		if ((ret = xmodem_send_pkt(sx, data_max)) < 0) {
			return ret;
		}

		data_len -= len;
		data += len;
	}

	/* Send EOT */
	ret = xmodem_send_pkt(sx, 0);

	sx->data_max = (sx->mode != MODE_XMODEM) ? 1024 : 128;
	sx->data_len = 0;
	sx->seq = (sx->mode != MODE_YMODEM) ? 0 : 1;
	sx->state = XMODEM_SEND_IDLE;

	return ret;
}

int xmodem_send_close(struct xmodem_send * sx)
{
	int ret = 0;
	int i;


	if (sx->mode == MODE_YMODEM) {
		int data_max;
#if 1
		data_max = 1024;
#else
		data_max = 128;
#endif
		/* NULL packet */
		for (i = 0; i < data_max; ++i)
			sx->pkt.data[i] = '\0';

		sx->seq = 0;
		ret = xmodem_send_pkt(sx, data_max);

		sx->data_max = 1024;
		sx->data_len = 0;
		sx->seq = 0;
		sx->state = XMODEM_SEND_IDLE;
	}

	return ret;
}

int xmodem_send_cancel(struct xmodem_send * sx)
{
	unsigned char buf[4];
	int ret;


	buf[0] = CAN;

	serial_send(sx->dev, buf, 1);
	ret = serial_send(sx->dev, buf, 1);

	sx->data_max = (sx->mode != MODE_XMODEM) ? 1024 : 128;
	sx->data_len = 0;
	sx->seq = 0;
	sx->state = XMODEM_SEND_IDLE;

	return ret;
}

struct xmodem_send * xmodem_send_alloc(void)
{
	struct xmodem_send * sx;
	
	sx = (struct xmodem_send *)malloc(sizeof(struct xmodem_send));

	return sx;
}

void xmodem_send_free(struct xmodem_send * sx)
{
	free(sx);
}


