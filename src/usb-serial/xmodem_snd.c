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


#include <xmodem.h>
#include <crc.h>
#include <errno.h>
#include <sys/param.h>
#include <stdlib.h>

#define SOH  0x01
#define STX  0x02
#define EOT  0x04
#define ACK  0x06
#define NAK  0x15
#define CAN  0x18

#include <sys/dcclog.h>

#define XMODEM_SND_TMOUT_MS 2000

int xmodem_snd_init(struct xmodem_snd * sx, 
					const struct xmodem_comm * comm, unsigned int mode)
{
	sx->comm = *comm;

	if ((sx == NULL) || (comm == NULL) || (mode > XMODEM_1K))
		return -EINVAL;

	sx->mode = mode;
	sx->data_max = (sx->mode == XMODEM_1K) ? 1024 : 128;
	sx->data_len = 0;
	sx->seq = 1;
	sx->state = XMODEM_SND_IDLE;

	DCC_LOG(LOG_TRACE, "[IDLE]");

	return 0;
}

int xmodem_snd_cancel(struct xmodem_snd * sx)
{
	unsigned char buf[4];
	int ret;

	DCC_LOG(LOG_TRACE, "[CAN]");

	buf[0] = CAN;
	
	ret = sx->comm.op.send(sx->comm.arg, buf, 1);

	sx->data_max = (sx->mode == XMODEM_1K) ? 1024 : 128;
	sx->data_len = 0;
	sx->seq = 1;
	sx->state = XMODEM_SND_IDLE;

	DCC_LOG(LOG_TRACE, "[IDLE]");

	return ret;
}

static int xmodem_send_pkt(struct xmodem_snd * sx)
{
	unsigned char buf[4];
	unsigned char * pkt;
	int retry = 0;
	int len;
	int ret;
	int c;

	if (sx->state == XMODEM_SND_IDLE) {
		for (;;) {
			DCC_LOG(LOG_TRACE, "waiting");

			// Wait for NAK or 'C'
			if ((ret = sx->comm.op.recv(sx->comm.arg, buf, 
										1, XMODEM_SND_TMOUT_MS)) < 0) {
				return ret;
			}
			c = buf[0];

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

	pkt = sx->pkt.hdr;

	if (sx->data_len == 1024) {
		pkt[0] = STX;
	} else {
		pkt[0] = SOH;
	} 

	pkt[1] = sx->seq;
	pkt[2] = ~sx->seq;

	if (sx->state == XMODEM_SND_CRC) {
		unsigned char * cp = sx->pkt.data;
		unsigned short crc = 0;
		int i;

		for (i = 0; i < sx->data_len; ++i)
			crc = CRC16CCITT(crc, cp[i]);
		cp[i] = crc >> 8;
		cp[i + 1] = crc & 0xff;
		len = i + 5;
	} else {
		unsigned char * cp = sx->pkt.data;
		unsigned char cks = 0;
		int i;

		for (i = 0; i < sx->data_len; ++i)
			cks += cp[i];

		cp[i] = cks;
		len = i + 4;
	}

	for (;;) {

		DCC_LOG2(LOG_TRACE, "Xmit seq=%d retry=%d", sx->seq, retry);

		// Send packet 
		if ((ret = sx->comm.op.send(sx->comm.arg, pkt, len)) < 0)
			return ret;

		// Wait for ACK
		if ((ret = sx->comm.op.recv(sx->comm.arg, buf, 
									1, XMODEM_SND_TMOUT_MS)) < 0) {
			return ret;
		}

		c = buf[0];

		if (c == ACK) 
			break;

		if (c == CAN) 
			return -ECANCELED;

		if (c != NAK) 
			return -EBADMSG;

		retry++;
	}

	sx->seq++;
	sx->data_len = 0;

	return 0;
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
			if ((ret = xmodem_send_pkt(sx)) < 0)
				return ret;
		}

		src += n;
		len -= n;
	} while (len);

	return 0;
}

int xmodem_snd_eot(struct xmodem_snd * sx)
{
	unsigned char buf[4];
	int ret;

	if (sx->data_len > 0) {
		int i;

		if ((sx->data_max == 1024) && (sx->data_len < 128))
			sx->data_max = 128;

		for (i = sx->data_len; i < sx->data_max; ++i)
			sx->pkt.data[i] = '\0';

		if ((ret = xmodem_send_pkt(sx)) < 0)
			return ret;
	}

	buf[0] = EOT;
	ret = sx->comm.op.send(sx->comm.arg, buf, 1);

	sx->data_max = (sx->mode == XMODEM_1K) ? 1024 : 128;
	sx->data_len = 0;
	sx->seq = 1;
	sx->state = XMODEM_SND_IDLE;

	DCC_LOG(LOG_TRACE, "[IDLE]");

	return ret;
}

