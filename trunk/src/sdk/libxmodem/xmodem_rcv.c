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
#include <errno.h>
#include <thinkos.h>

#include <xmodem.h>
#include <crc.h>

#define SOH  0x01
#define STX  0x02
#define EOT  0x04
#define ACK  0x06
#define NAK  0x15
#define CAN  0x18

#include <sys/dcclog.h>

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

	DCC_LOG1(LOG_TRACE, "mode=%s", 
			 (rx->mode == XMODEM_RCV_CRC) ? "CRC" : "CKS");

	return 0;
}

int xmodem_rcv_cancel(struct xmodem_rcv * rx)
{
	unsigned char * pkt = rx->pkt.hdr;

	DCC_LOG(LOG_TRACE, "ABT!");

	pkt[0] = CAN;
	pkt[1] = CAN;
	pkt[2] = CAN;

	rx->comm->op.send(rx->comm->arg, pkt, 3);

	return 0;
}

int xmodem_rcv_pkt(struct xmodem_rcv * rx)
{
	unsigned char * pkt = rx->pkt.hdr;
	unsigned char * cp;
	int ret = 0;
	int cnt = 0;
	int nseq;
	int seq;
	int rem;

	DCC_LOG(LOG_INFO, "....");

	for (;;) {
		DCC_LOG1(LOG_TRACE, "SYN: %02x", rx->sync);

		if ((ret = rx->comm->op.send(rx->comm->arg, &rx->sync, 1)) < 0) {
			return ret;
		}

		for (;;) {
			int c;

			ret = rx->comm->op.recv(rx->comm->arg, pkt, 1, 2000);

			if (ret == THINKOS_ETIMEDOUT)
				goto timeout;

			if (ret < 0)
				return ret;

			c = pkt[0];

			if (c == STX) {
				DCC_LOG(LOG_TRACE, "<- STX");
				cnt = 1024;
				break;
			}

			if (c == SOH) {
				DCC_LOG(LOG_TRACE, "<- SOH");
				cnt = 128;
				break;
			}

			if (c == EOT) {
				DCC_LOG(LOG_TRACE, "<- EOT");
				/* end of transmission */
				pkt[0] = ACK;
				if ((ret = rx->comm->op.send(rx->comm->arg, pkt, 1)) < 0)
					return ret;
				return 0;
			}
		}

		rem = cnt + ((rx->mode) ? 4 : 3);
		cp = pkt + 1;

		DCC_LOG2(LOG_TRACE, "cnt=%d rem=%d", cnt, rem);

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
			DCC_LOG2(LOG_WARNING, "SEQ: %d != NSEQ: %d", seq, nseq);
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
				DCC_LOG2(LOG_WARNING, "crc: %04x != %04x", cmp, crc);
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
			DCC_LOG(LOG_TRACE, "RET");
			rx->sync = ACK;
			continue;
		}

		if (seq != rx->pktno) {
			DCC_LOG2(LOG_TRACE, "SEQ: %d != %d", seq, rx->pktno);
			goto error;
		}

		rx->pktno = (rx->pktno + 1) & 0xff;
		rx->retry = 10;
		rx->sync = ACK;
		rx->data_len = cnt;
		rx->data_pos = 0;
		DCC_LOG(LOG_INFO, "ACK");

		return cnt;

error:
		/* flush */
		while (rx->comm->op.recv(rx->comm->arg, pkt, 1024, 200) > 0);
		rx->sync = NAK;
		DCC_LOG(LOG_TRACE, "NACK");

timeout:
		DCC_LOG(LOG_TRACE, "TMO");

		if ((--rx->retry) == 0) {
			/* too many errors */
			ret = -1;
			break;
		}
	}

	DCC_LOG(LOG_TRACE, "ABT!");

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

	//	printf("%s: len=%d\n", __func__, len);

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

			DCC_LOG1(LOG_INFO, "n=%d", n);

			return n;
		}

		ret = xmodem_rcv_pkt(rx);

	} while (ret > 0);

	return ret;
}


