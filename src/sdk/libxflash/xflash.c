/* 
 * File:	 xflash.c
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

#include <arch/cortex-m3.h>
#include <sys/param.h>
#include <stdint.h>
#include <stdbool.h>
#include <crc.h>
#include "xflash.h"

void __attribute__((noreturn)) reset(void);
void delay(unsigned int msec);

#define FLASH_WR_BLK_SIZE 128

#ifndef ENABLE_XMODEM_CKS
#define ENABLE_XMODEM_CKS 1
#endif


enum {
	XMODEM_RCV_CKS = 0,
	XMODEM_RCV_CRC = 1,
};

struct xmodem_rcv {
	signed char mode;
	unsigned char sync;
	unsigned char pktno;
	unsigned char retry;

	struct { 
		unsigned char hdr[3];
		unsigned char data[1024];
		unsigned char fcs[2];
	} pkt;
};

enum {
	XMODEM_SND_STD = 0,
	XMODEM_SND_1K = 1
};

#define SOH  0x01
#define STX  0x02
#define EOT  0x04
#define ACK  0x06
#define NAK  0x15
#define CAN  0x18

#define XMODEM_RCV_TMOUT_MS 2000

static int usb_xmodem_rcv_init(struct xmodem_rcv * rx, int mode)
{
	rx->mode = mode;
	rx->pktno = 1;
#ifndef ENABLE_XMODEM_CKS
	rx->sync = (rx->mode == XMODEM_RCV_CRC) ? 'C' : NAK;
#else
	rx->sync = 'C';
#endif
	rx->retry = 15;

	return 0;
}

#define CDC_TX_EP 2
#define CDC_RX_EP 1

void xmodem_rcv_cancel(struct xmodem_rcv * rx)
{
	unsigned char * pkt = rx->pkt.hdr;

	pkt[0] = CAN;
	pkt[1] = CAN;
	pkt[3] = CAN;

	usb_send(CDC_TX_EP, pkt, 3);
	usb_drain(CDC_TX_EP);
}

int usb_xmodem_rcv_pkt(struct xmodem_rcv * rx)
{
	unsigned char * pkt = rx->pkt.hdr;
	unsigned char * cp;
	int ret = 0;
	int cnt = 0;
	int nseq;
	int seq;
	int rem;
	int pos;
	int i;

	for (;;) {

		if ((ret = usb_send(CDC_TX_EP, &rx->sync, 1)) < 0) {
			return ret;
		}

		rem = 0;
		pos = 0;
		for (;;) {
			int c;

			if (rem == 0) {
				ret = usb_recv(CDC_RX_EP, pkt, 128, 2000);

				if (ret <= 0)
					goto timeout;

				pos = 0;
				rem = ret;
			}

			c = pkt[pos];
			pos++;
			rem--;

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
				pkt[1] = ACK;
				if ((ret = usb_send(CDC_TX_EP, pkt, 2)) < 0)
					return ret;
				return 0;
			}
		}

		cp = pkt + 1;
		for (i = 0; i < rem; ++i)
			cp[i] = pkt[pos + i];
		cp += rem;

#ifndef ENABLE_XMODEM_CKS
		rem = cnt + ((rx->mode) ? 4 : 3) - rem;
#else
		rem = cnt + 4 - rem;
#endif

		/* receive the packet */
		while (rem) {

			ret = usb_recv(CDC_RX_EP, cp, rem, 500);
			if (ret <= 0)
				goto timeout;

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

#ifndef ENABLE_XMODEM_CKS
		if (rx->mode) 
#endif
		{
			unsigned short crc = 0;
			unsigned short cmp;
			int i;

			for (i = 0; i < cnt; ++i)
				crc = CRC16CCITT(crc, cp[i]);

			cmp = (unsigned short)cp[i] << 8 | cp[i + 1];

			if (cmp != crc) {
				
				goto error;
			}

		} 
#ifndef ENABLE_XMODEM_CKS
		else {
			unsigned char cks = 0;
			int i;

			for (i = 0; i < cnt; ++i)
				cks += cp[i];

			if (cp[i] != cks)
				goto error;
		}
#endif

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

		return cnt;

error:
		/* flush */
		while (usb_recv(CDC_RX_EP, pkt, 1024, 200) > 0);
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

	usb_send(CDC_TX_EP, pkt, 2);

	return ret;
}

#define MAGIC_REC_MAX 16

static bool magic_match(struct magic * magic, int pos, uint8_t * buf, int len)
{
	int sz = magic->hdr.cnt * sizeof(struct magic_rec);
	int k;
	int j;

	/* check whether the magic record is in this
	   data block */
	if (magic->hdr.pos < pos)
		return false;

	if ((magic->hdr.pos + sz) > (pos + len))
		return false;

	k = magic->hdr.pos - pos;
	for (j = 0; j < magic->hdr.cnt; ++j) {
		uint32_t data;

		data = buf[k] + (buf[k + 1] << 8) + 
			(buf[k + 2] << 16) + (buf[k + 3] << 24);

		if ((data & magic->rec[j].mask) != magic->rec[j].comp) {
			return false;
		}

		k += sizeof(uint32_t);
	}	

	return true;
}

static const char s_xmodem[] = "\r\nXmodem (^D to cancel)... ";
static const char s_invalid[] = "\r\nInvalid file!";

int __attribute__((noreturn)) xflash(uint32_t blk_offs, unsigned int blk_size, 
									 const struct magic * magic)
{
	struct {
		struct magic_hdr hdr;
		struct magic_rec rec[MAGIC_REC_MAX];
	} magic_buf;
	struct xmodem_rcv rx;
	unsigned int cnt;
	uint32_t offs;
	int ret;
	int i;

	if (magic != 0) {
		/* copy magic check block */
		cnt = magic->hdr.cnt > MAGIC_REC_MAX ? MAGIC_REC_MAX : magic->hdr.cnt;
		for (i = 0; i < cnt; ++i) {
//			usb_send(CDC_TX_EP, "\r\nmagic.", 8);
			magic_buf.rec[i] = magic->rec[i];
		}	
		magic_buf.hdr.cnt = cnt;
		magic_buf.hdr.pos = magic->hdr.pos;
	} else {
		magic_buf.hdr.cnt = 0;
		magic_buf.hdr.pos = 0;
	}

	flash_unlock();

	do {
//		usb_send(CDC_TX_EP, "\r\nErasing...", 12);
//		flash_erase(blk_offs, blk_size);

		usb_send(CDC_TX_EP, s_xmodem, sizeof(s_xmodem) - 1);
		usb_xmodem_rcv_init(&rx, XMODEM_RCV_CRC);
		offs = blk_offs;
		cnt = 0;

		ret = usb_xmodem_rcv_pkt(&rx);

		while (ret > 0) {
			uint8_t * buf = rx.pkt.data;
			int rem = ret;

			if (cnt == 0) {
				if (!magic_match((struct magic *)&magic_buf, cnt, buf, rem)) {
					xmodem_rcv_cancel(&rx);
					delay(1000);
					usb_send(CDC_TX_EP, s_invalid, sizeof(s_invalid) - 1);
					ret = -1;
					break;
				}
			}

			/* XXX: STM32F103: wait at least 50ms between erasing 
				   and start writing to the flash!!! */
			flash_erase(offs, rem);
//			delay(64);

			while (rem > 0) {
				int n;

				n = MIN(rem, FLASH_WR_BLK_SIZE);
				n = MIN(n, blk_size - cnt);
				if (n == 0)
					break;

				flash_write(offs, buf, n);

				offs += n;
				buf += n;
				cnt += n;
				rem -= n;
			}

			ret = usb_xmodem_rcv_pkt(&rx);
		} 
	} while ((ret < 0) || (cnt == 0));

	usb_send(CDC_TX_EP, "\r\nDone.\r\n", 9);

	usb_drain(CDC_TX_EP);

	delay(3000);

	reset();
}

