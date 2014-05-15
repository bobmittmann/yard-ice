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

static const char rx_state_name[][4] = {
	"IDL",
	"DAT",
	"EOT",
	"SYN",
	"STX",
	"SEQ",
	"NSQ",
	"CKS",
	"CR1",
	"CR2",
	"ABT"
};

int xmodem_rcv_init(struct xmodem_rcv * rx, const struct comm_dev * comm, 
					int mode)
{
	if ((rx == NULL) || (comm == NULL) || (mode > XMODEM_1K))
		return -EINVAL;

	rx->comm = *comm;

	rx->state = XMODEM_RCV_IDLE;
	rx->mode = mode;
	rx->pktno = 1;
	rx->sync = (rx->mode) ? 'C' : NAK;
	rx->retry = 10;
	rx->data_len = 0;
	rx->data_pos = 0;

	return 0;
}

int xmodem_rcv_cancel(struct xmodem_rcv * rx)
{
	rx->again = 3;
	rx->state = XMODEM_RCV_ABORT;
	/* the retry field is used to store the return error code */
	rx->retry = XMODEM_CANCEL;

	return 0;
}

int xmodem_rcv(struct xmodem_rcv * xp, int * cp)
{
	int c = *cp;

	*cp = -1;

	switch (xp->state) {

	case XMODEM_RCV_IDLE:
		xp->retry = 20;
		xp->again = 10;
		xp->tmout = XMODEM_RCV_TMOUT_MS; /* mS */
		xp->cks = 0;
		xp->crc = 0;
		xp->pos = 0;
		xp->state = XMODEM_RCV_SYNC;
		*cp = (xp->mode) ? 'C' : NAK;
		DCC_LOG1(LOG_INFO, "[%s]", rx_state_name[xp->state]);
		return XMODEM_RCV_IDLE;

	case XMODEM_RCV_SYNC:
		if (c == STX) {
#ifdef XMODEM_DEBUG		
			printf("\nSTX ");
#endif
			xp->tmout = XMODEM_RCV_TMOUT_MS;
			xp->count = 1024;
			xp->state = XMODEM_RCV_SEQ;
			DCC_LOG1(LOG_INFO, "[%s]", rx_state_name[xp->state]);
			return XMODEM_RCV_SYNC;
		}

		if (c == SOH) {
#ifdef XMODEM_DEBUG		
			printf("\nSOH ");
#endif
			xp->tmout = XMODEM_RCV_TMOUT_MS;
			xp->count = 128;
			xp->state = XMODEM_RCV_SEQ;
			DCC_LOG1(LOG_INFO, "[%s]", rx_state_name[xp->state]);
			return XMODEM_RCV_SYNC;
		}

		if (c == CAN) {
			xp->state = XMODEM_RCV_IDLE;
			DCC_LOG1(LOG_INFO, "[%s]", rx_state_name[xp->state]);
			*cp = CAN;
			return XMODEM_CANCEL;
		}

		if (c == -1)  {
			if ((xp->tmout -= XMODEM_TMOUT_TICK_MS) > 0)
				return XMODEM_OK;

			if (--xp->retry) {
#ifdef XMODEM_DEBUG		
				printf("SYNC ");
#endif
				xp->tmout = XMODEM_RCV_TMOUT_MS;
				*cp = (xp->mode) ? 'C' : NAK;
				return XMODEM_OK;
			}
		}

		xp->state = XMODEM_RCV_IDLE;
		DCC_LOG1(LOG_INFO, "[%s]", rx_state_name[xp->state]);
		return XMODEM_SYNC_ERR;
	
	case XMODEM_RCV_STX:
		if (c == STX) {
#ifdef XMODEM_DEBUG		
			printf("\nSTX ");
#endif
			xp->tmout = XMODEM_RCV_TMOUT_MS;
			xp->count = 1024;
			xp->cks = 0;
			xp->crc = 0;
			xp->pos = 0;
			xp->state = XMODEM_RCV_SEQ;
			DCC_LOG1(LOG_INFO, "[%s]", rx_state_name[xp->state]);
			return XMODEM_RCV_STX;
		}

		if (c == SOH) {
#ifdef XMODEM_DEBUG		
			printf("\nSOH ");
#endif
			xp->tmout = XMODEM_RCV_TMOUT_MS;
			xp->count = 128;
			xp->cks = 0;
			xp->crc = 0;
			xp->pos = 0;
			xp->state = XMODEM_RCV_SEQ;
			DCC_LOG1(LOG_INFO, "[%s]", rx_state_name[xp->state]);
			return XMODEM_RCV_STX;
		}

		if (c == EOT) {
			xp->state = XMODEM_RCV_EOT;
			DCC_LOG1(LOG_INFO, "[%s]", rx_state_name[xp->state]);
			*cp = ACK;
			return XMODEM_OK;
		}

		if (c == CAN) {
			xp->state = XMODEM_RCV_IDLE;
			DCC_LOG1(LOG_INFO, "[%s]", rx_state_name[xp->state]);
			*cp = CAN;
			return XMODEM_CANCEL;
		}

		if (c == -1)  {
			if ((xp->tmout -= XMODEM_TMOUT_TICK_MS) > 0)
				return XMODEM_OK;

			if (--xp->retry) {
#ifdef XMODEM_DEBUG		
				printf("!");
#endif
				xp->tmout = XMODEM_RCV_TMOUT_MS;
				*cp = NAK;
				return XMODEM_OK;
			}
		}
		xp->again = 3;
		xp->state = XMODEM_RCV_ABORT;
		DCC_LOG1(LOG_INFO, "[%s]", rx_state_name[xp->state]);
		xp->retry = XMODEM_SYNC_ERR;
		*cp = CAN;
		return XMODEM_OK;

	case XMODEM_RCV_ABORT:
		if (--xp->again) {
			*cp = CAN;
			return XMODEM_OK;
		}

#ifdef XMODEM_DEBUG		
		printf(" ABORT\n");
#endif

		xp->state = XMODEM_RCV_IDLE;
		DCC_LOG1(LOG_INFO, "[%s]", rx_state_name[xp->state]);
		return xp->retry;

	case XMODEM_RCV_EOT:
#ifdef XMODEM_DEBUG		
		printf(" EOT\n");
#endif
		xp->state = XMODEM_RCV_IDLE;	
		DCC_LOG1(LOG_INFO, "[%s]", rx_state_name[xp->state]);
		return XMODEM_RCV_EOT;

	case XMODEM_RCV_SEQ:
		if (c == -1)
			break;
		xp->seq = (unsigned char)c;
		xp->state = XMODEM_RCV_NSEQ;
		DCC_LOG1(LOG_INFO, "[%s]", rx_state_name[xp->state]);
#ifdef XMODEM_DEBUG		
		printf("%u", c);
#endif
		return XMODEM_RCV_SEQ;

	case XMODEM_RCV_NSEQ:
		if (c == -1)
			break;
		xp->nseq = ~((unsigned char)c);
		xp->state = XMODEM_RCV_DATA;
		DCC_LOG1(LOG_INFO, "[%s]", rx_state_name[xp->state]);
		return XMODEM_RCV_NSEQ;

	case XMODEM_RCV_DATA:
		if (c == -1)
			break;

		xp->buf[xp->pos++] = (char)c;
		xp->cks += (unsigned char)c;
		xp->crc = CRC16CCITT(xp->crc, (unsigned char)c);

		if (--xp->count)
			return XMODEM_RCV_DATA;

		if (xp->mode)
			xp->state = XMODEM_RCV_CRC1;
		else
			xp->state = XMODEM_RCV_CKS;
		DCC_LOG1(LOG_INFO, "[%s]", rx_state_name[xp->state]);
		return XMODEM_RCV_DATA;

	case XMODEM_RCV_CKS:
		if (c == -1)
			break;

#ifdef XMODEM_DEBUG		
		printf(" CKS: %02x", c);
#endif

		if (!((unsigned char)c - xp->cks) && (xp->nseq == xp->seq) &&
			((xp->seq == xp->pktno) || (xp->seq == xp->pktno - 1))) {
			xp->retry = 10;
			xp->tmout = XMODEM_RCV_TMOUT_MS; /* mS */
			xp->state = XMODEM_RCV_STX;
			DCC_LOG1(LOG_INFO, "[%s]", rx_state_name[xp->state]);
			*cp = ACK;
			if (xp->seq == xp->pktno) {
				xp->pktno++;
				xp->again = 10;
				return xp->pos;
			}
			if (--xp->again)
				return XMODEM_OK;

			*cp = NAK;
			xp->again = 3;
			xp->state = XMODEM_RCV_ABORT;
			DCC_LOG1(LOG_INFO, "[%s]", rx_state_name[xp->state]);
			/* the retry field is used to store the return error code */
			xp->retry = XMODEM_RETRY_ERR;
			return XMODEM_OK;
		}
		xp->tmout = XMODEM_TMOUT_TICK_MS;
		break;

	case XMODEM_RCV_CRC1:
		if (c == -1)
			break;

#ifdef XMODEM_DEBUG		
		printf(" CRC: ");
#endif
			
		xp->cks = (unsigned char)c;
		xp->state = XMODEM_RCV_CRC2;
			DCC_LOG1(LOG_INFO, "[%s]", rx_state_name[xp->state]);
		return XMODEM_OK;

	case XMODEM_RCV_CRC2:
		if (c == -1)
			break;
		c |= xp->cks << 8;
#ifdef XMODEM_DEBUG		
		printf("%04x", c);
#endif
		if ((!(c - xp->crc)) && (xp->nseq == xp->seq) &&
			((xp->seq == xp->pktno) || (xp->seq == xp->pktno -1))) {
			xp->retry = 10;
			xp->tmout = XMODEM_RCV_TMOUT_MS; /* mS */
			xp->state = XMODEM_RCV_STX;
			DCC_LOG1(LOG_INFO, "[%s]", rx_state_name[xp->state]);
			*cp = ACK;
			if (xp->seq == xp->pktno) {
				xp->pktno++;
				xp->again = 10;
				return xp->pos;
			}
			if (--xp->again)
				return XMODEM_OK;

			*cp = NAK;
			xp->again = 3;
			xp->state = XMODEM_RCV_ABORT;
			DCC_LOG1(LOG_INFO, "[%s]", rx_state_name[xp->state]);
			/* the retry field is used to store the return error code */
			xp->retry = XMODEM_RETRY_ERR;
			return XMODEM_OK;
		}
		xp->tmout = XMODEM_TMOUT_TICK_MS;
		break;
		
	default:
		return XMODEM_OK;
	}

	if ((xp->tmout -= XMODEM_TMOUT_TICK_MS) > 0)
		return XMODEM_OK;
		
	if (--xp->again) {
#ifdef XMODEM_DEBUG		
		printf("!");
#endif
		xp->retry = 10;
		xp->tmout = XMODEM_RCV_TMOUT_MS; /* mS */
		xp->state = XMODEM_RCV_STX;
		DCC_LOG1(LOG_INFO, "[%s]", rx_state_name[xp->state]);
		*cp = NAK;
		return XMODEM_OK;	
	}
	xp->again = 3;
	xp->state = XMODEM_RCV_ABORT;
	DCC_LOG1(LOG_INFO, "[%s]", rx_state_name[xp->state]);
	/* the retry field is used to store the return error code */
	xp->retry = XMODEM_RETRY_ERR;

	return XMODEM_OK;
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

	for (;;) {
		DCC_LOG(LOG_INFO,"SYN");

		if ((ret = rx->comm.op.send(rx->comm.arg, &rx->sync, 1)) < 0) {
			return ret;
		}

		for (;;) {
			int c;

			ret = rx->comm.op.recv(rx->comm.arg, pkt, 1, 2000);

			if (ret == -ETIMEDOUT)
				goto timeout;

			if (ret < 0)
				return ret;

			c = pkt[0];

			if (c == STX) {
				DCC_LOG(LOG_INFO,"STX");
				cnt = 1024;
				break;
			}

			if (c == SOH) {
				DCC_LOG(LOG_INFO,"SOH");
				cnt = 128;
				break;
			}

			if (c == EOT) {
				DCC_LOG(LOG_INFO,"EOT");
				/* end of transmission */
				pkt[0] = ACK;
				if ((ret = rx->comm.op.send(rx->comm.arg, pkt, 1)) < 0)
					return ret;
				return 0;
			}
		}

		rem = cnt + (rx->mode) ? 4 : 3;
		cp = pkt + 1;

		/* receive the packet */
		while (rem) {

			ret = rx->comm.op.recv(rx->comm.arg, cp, rem, 500);
			if (ret == 	-ETIMEDOUT)
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

			cmp = cp[i] << 8 | cp[i + 1];

			if (cmp != crc)
				goto error;

		} else {
			unsigned char cks = 0;
			int i;

			for (i = 0; i < rx->data_len; ++i)
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
		while (rx->comm.op.recv(rx->comm.arg, pkt, 1024, 200) > 0);
		rx->sync = NAK;
		DCC_LOG(LOG_TRACE, "NACK");

timeout:
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

	rx->comm.op.send(rx->comm.arg, pkt, 3);

	return ret;
}



int xmodem_rcv_loop(struct xmodem_rcv * rx, void * data, int len)
{
	unsigned char * dst = (unsigned char *)data;
	int rem;
	int ret;

	if ((dst == NULL) || (len <= 0))
		return -EINVAL;

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

			return n;
		}

		ret = xmodem_rcv_pkt(rx);

	} while (ret > 0);

	return ret;
}


