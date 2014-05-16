/* 
 * File:	 xmodem_rcv.c
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(c) 2003-2006 BORESTE (www.boreste.com). All Rights Reserved.
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


#include <sys/param.h>
#include <stdlib.h>
#include <stdio.h>

#include <xmodem.h>
#include <crc.h>
#include <errno.h>
#include <thinkos.h>

#include <sys/dcclog.h>

#define SOH  0x01
#define STX  0x02
#define EOT  0x04
#define ACK  0x06
#define NAK  0x15
#define CAN  0x18

#define XMODEM_SND_TMOUT_MS 2000

enum {
	XMODEM_SND_IDLE = 0,
	XMODEM_SND_CRC = 1,
	XMODEM_SND_CKS = 2
};


int xmodem_snd_init(struct xmodem_snd * sx, 
					const struct comm_dev * comm, unsigned int mode)
{
	if ((sx == NULL) || (comm == NULL) || (mode > XMODEM_SND_1K))
		return -EINVAL;

	sx->comm = comm;
	sx->mode = mode;
	sx->data_max = (sx->mode == XMODEM_SND_1K) ? 1024 : 128;
	sx->data_len = 0;
	sx->seq = 1;
	sx->state = XMODEM_SND_IDLE;

	/* flush */
	while (sx->comm->op.recv(sx->comm->arg, sx->pkt.data, 1024, 200) > 0); 

	DCC_LOG(LOG_TRACE, "[IDLE]");

	return 0;
}

int xmodem_snd_cancel(struct xmodem_snd * sx)
{
	unsigned char buf[4];
	int ret;

	DCC_LOG(LOG_TRACE, "[CAN]");

	buf[0] = CAN;
	
	ret = sx->comm->op.send(sx->comm->arg, buf, 1);

	sx->data_max = (sx->mode == XMODEM_SND_1K) ? 1024 : 128;
	sx->data_len = 0;
	sx->seq = 1;
	sx->state = XMODEM_SND_IDLE;

	DCC_LOG(LOG_TRACE, "[IDLE]");

	return ret;
}

static int xmodem_send_pkt(struct xmodem_snd * sx, 
						   unsigned char * data, int data_len)
{
	unsigned char * pkt = data - 3; 
	unsigned char * cp;
	unsigned char fcs[2];
	int retry = 0;
	int fcs_len;
	int ret;
	int c;

	DCC_LOG1(LOG_INFO, "len=%d", data_len);

	if (sx->state == XMODEM_SND_IDLE) {

		for (;;) {
			DCC_LOG(LOG_TRACE, "waiting");

			// Wait for NAK or 'C'
			if ((ret = sx->comm->op.recv(sx->comm->arg, pkt, 
										1, XMODEM_SND_TMOUT_MS)) < 0) {
				if (ret == THINKOS_ETIMEDOUT) {
					DCC_LOG(LOG_WARNING, "TMO");
					if (++retry < 20) 
						continue;
				}
				return ret;
			}
			c = *pkt;

			if (c == CAN) {
				DCC_LOG(LOG_TRACE, "[CAN]");
				return -ECANCELED;
			}

			if (c == 'C') {
				DCC_LOG(LOG_TRACE, "[CRC]");
				sx->state = XMODEM_SND_CRC;
				break;
			}

			if (c == NAK) {
				DCC_LOG(LOG_TRACE, "[NAK]");
				sx->state = XMODEM_SND_CKS;
				break;
			}

			DCC_LOG1(LOG_WARNING, "%02x", c);
		}
	}

	if (data_len == 1024) {
		pkt[0] = STX;
	} else {
		pkt[0] = SOH;
	} 

	pkt[1] = sx->seq;
	pkt[2] = ~sx->seq;
	cp = &pkt[3];

	if (sx->state == XMODEM_SND_CRC) {
		unsigned short crc = 0;
		int i;

		for (i = 0; i < data_len; ++i)
			crc = CRC16CCITT(crc, cp[i]);

		fcs[0] = crc >> 8;
		fcs[1] = crc & 0xff;
		fcs_len = 2;
	} else {
		unsigned char cks = 0;
		int i;

		for (i = 0; i < data_len; ++i)
			cks += cp[i];

		fcs[0] = cks;
		fcs_len = 1;
	}

	for (;;) {

		DCC_LOG2(LOG_INFO, "Xmit seq=%d retry=%d", sx->seq, retry);

		// Send packet less FCS 
		if ((ret = sx->comm->op.send(sx->comm->arg, pkt, data_len + 3)) < 0) {
			DCC_LOG(LOG_WARNING, "comm->op.send() failed!");
			return ret;
		}

		// Send FCS (checksum or CRC)
		if ((ret = sx->comm->op.send(sx->comm->arg, fcs, fcs_len)) < 0) {
			DCC_LOG(LOG_WARNING, "comm->op.send() failed!");
			return ret;
		}

		// Wait for ACK
		if ((ret = sx->comm->op.recv(sx->comm->arg, pkt, 
									1, XMODEM_SND_TMOUT_MS)) <= 0) {
			if (ret == THINKOS_ETIMEDOUT) {
				DCC_LOG(LOG_TRACE, "[TMO]");
				if (++retry < 10)
					continue;
			}
			DCC_LOG(LOG_WARNING, "comm->op.recv() failed!");
			return ret;
		}

		c = *pkt;

		if (c == ACK) {
			DCC_LOG(LOG_TRACE, "[ACK]");
			break;
		}

		if (c == CAN) {
			DCC_LOG(LOG_TRACE, "[CAN]");
			ret = -ECANCELED;
			goto error;
		}

		if (c != NAK) {
			DCC_LOG1(LOG_WARNING, "rx=%02x", c);
			ret = -EBADMSG;
			goto error;
		}

		DCC_LOG(LOG_WARNING, "[NAK]");
		
		if (++retry == 10) {
			ret = -ECANCELED;
			goto error;
		}
	}

	sx->seq++;

	return 0;

error:
	/* flush */
	while (sx->comm->op.recv(sx->comm->arg, sx->pkt.data, 1024, 200) > 0); 

	return ret;
}

int xmodem_snd_loop(struct xmodem_snd * sx, const void * data, int len)
{
	unsigned char * src = (unsigned char *)data;

	if ((src == NULL) || (len < 0))
		return -EINVAL;

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

			if ((ret = xmodem_send_pkt(sx, sx->pkt.data, sx->data_len)) < 0) {
				return ret;
			}
		
			sx->data_len = 0;
		}
	
		src += n;
		len -= n;
	} while (len);

	return 0;
}

int xmodem_snd_eot(struct xmodem_snd * sx)
{
	unsigned char buf[4];
	unsigned char * data;
	int data_len;
	int data_max;
	int ret;


	if ((data_len = sx->data_len) <= (1024 - 128))
		data_max = 128;
	else
		data_max =  sx->data_max;

	data = sx->pkt.data;

	while (data_len > 0) {
		int len;
		int i;

		len = MIN(data_len, data_max);

		/* padding */
		for (i = len; i < data_max; ++i)
			data[i] = '\0';

		DCC_LOG1(LOG_INFO, "len=%d", len);

		if ((ret = xmodem_send_pkt(sx, data, data_max)) < 0) {
			return ret;
		}

		data_len -= len;
		data += len;
	}

	DCC_LOG(LOG_TRACE, "[EOT]");

	buf[0] = EOT;
	ret = sx->comm->op.send(sx->comm->arg, buf, 1);

	sx->data_max = (sx->mode == XMODEM_SND_1K) ? 1024 : 128;
	sx->data_len = 0;
	sx->seq = 1;
	sx->state = XMODEM_SND_IDLE;

	DCC_LOG(LOG_TRACE, "[IDLE]");

	return ret;
}

