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
#include <crc.h>
#include "xflash.h"

#define FLASH_WR_BLK_SIZE 128

static void reset(void)
{
	CM3_SCB->aircr =  SCB_AIRCR_VECTKEY | SCB_AIRCR_SYSRESETREQ;
	for(;;);
}

static void delay(unsigned int msec)
{
	while (msec > 0) {
		if (CM3_SYSTICK->csr & SYSTICK_CSR_COUNTFLAG)
			msec--;
	}
}

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
	pkt[2] = CAN;

	usb_send(CDC_TX_EP, pkt, 3);

	return ret;
}

struct magic_rec {
	uint32_t * addr;
	uint32_t mask;
	uint32_t comp;
};

struct magic {
	uint32_t cnt;
	struct magic_rec rec[];
};

#define MAGIC_REC_MAX 8

int __attribute__((section (".init"))) usb_xflash(uint32_t blk_offs, 
												  unsigned int blk_size,
												  struct magic * magic)
{
	struct magic_rec rec[MAGIC_REC_MAX];
	struct xmodem_rcv rx;
	int magic_cnt = 0;
	unsigned int cnt;
	uint32_t offs;
	int ret;
	int i;

	if (magic != 0) {
		magic_cnt = magic->cnt > MAGIC_REC_MAX ? 
			MAGIC_REC_MAX : magic->cnt;
		for (i = 0; i < magic_cnt; ++i)
			rec[i] = magic->rec[i];
	}

	flash_unlock();

	do {
		usb_send(CDC_TX_EP, "\r\nErasing...", 12);

		if ((ret = flash_erase(blk_offs, blk_size)) >= 0) {

			usb_send(CDC_TX_EP, "\r\nXmodem... ", 12);

			usb_xmodem_rcv_init(&rx, XMODEM_RCV_CRC);
			offs = blk_offs;
			cnt = 0;

			ret = usb_xmodem_rcv_pkt(&rx);

			while (ret > 0) {
				unsigned char * src = rx.pkt.data;
				int rem = ret;

				while (rem > 0) {
					int n;

					n = MIN(rem, FLASH_WR_BLK_SIZE);
					n = MIN(n, blk_size - cnt);
					if (n == 0)
						break;

					flash_write(offs, src, n);

					offs += n;
					src += n;
					cnt += n;
					rem -= n;
				}

				ret = usb_xmodem_rcv_pkt(&rx);
			} 
		}

		if (ret >= 0) {
			for (i = 0; i < magic_cnt; ++i) {
				uint32_t data = *rec[i].addr;
				if ((data & rec[i].mask) != rec[i].comp) {
					ret = -1;
					usb_send(CDC_TX_EP, "\r\nInvalid!", 10);
					break;
				}
			}	
		}

	} while ((ret < 0) || (cnt == 0));

	usb_send(CDC_TX_EP, "\r\nDone.\r\n", 9);

	usb_drain(CDC_TX_EP);

	delay(3000);

	reset();

	return 0;
}

