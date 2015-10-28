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

#define CDC_TX_EP 2
#define CDC_RX_EP 1

#define FLASH_WR_BLK_SIZE 128

void __attribute__((noreturn)) reset(void);
void delay(unsigned int msec);

#define SOH  0x01
#define STX  0x02
#define EOT  0x04
#define ACK  0x06
#define NAK  0x15
#define CAN  0x18

#define XMODEM_RCV_TMOUT_MS 2000

struct ymodem_rcv {
	unsigned int pktno;
	unsigned int fsize;
	unsigned int count;

	unsigned char crc_mode;
	unsigned char xmodem;
	unsigned char sync;
	unsigned char retry;

	struct { 
		unsigned char hdr[3];
		unsigned char data[1024];
		unsigned char fcs[2];
	} pkt;
};

static int ymodem_rcv_init(struct ymodem_rcv * rx, bool crc_mode, bool xmodem)
{
	rx->pktno = 0;
	rx->crc_mode = crc_mode;
	rx->xmodem = xmodem;
	rx->sync = rx->crc_mode ? 'C' : NAK;
	rx->xmodem = 0;
	rx->retry = 30;
	rx->fsize = 1024 * 1024;
	rx->count = 0;

	return 0;
}

static int usb_ymodem_rcv_cancel(struct ymodem_rcv * rx)
{
	unsigned char * pkt = rx->pkt.hdr;

	pkt[0] = CAN;
	pkt[1] = CAN;

	return usb_send(CDC_TX_EP, pkt, 2);
}

static int usb_ymodem_rcv_pkt(struct ymodem_rcv * rx)
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

			if (c == CAN) {
				return -1;
			}

			if (c == EOT) {
				/* end of transmission */
				rx->sync = rx->crc_mode ? 'C' : NAK;
				rx->pktno = 0;
				pkt[0] = ACK;
				usb_send(CDC_TX_EP, pkt, 1);
				return 0;
			}
		}

		cp = pkt + 1;
		for (i = 0; i < rem; ++i)
			cp[i] = pkt[pos + i];
		cp += rem;

		rem = cnt + ((rx->crc_mode) ? 4 : 3) - rem;

		/* receive the packet */
		while (rem) {
			ret = usb_recv(CDC_RX_EP, cp, rem, 500);
			if (ret < 0)
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

		if (rx->crc_mode) {
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

			if (cp[i] != cks) {
				goto error;
			}
		}

		if (seq == ((rx->pktno - 1) & 0xff)) {
			/* retransmission */
			continue;
		}

		if (seq != (rx->pktno & 0xff)) {
			if ((rx->pktno == 0) && (seq == 1)) {
				rx->pktno++;
				/* Fallback to XMODEM */
				rx->xmodem = 1;
			} else {
				goto error;
			}
		}

		/* YModem first packet ... */
		if (rx->pktno == 0) {
			pkt[0] = ACK;
			usb_send(CDC_TX_EP, pkt, 1);
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
		while (usb_recv(CDC_RX_EP, pkt, 1024, 100) > 0);
		ret = -1;
		break;

timeout:
		if ((--rx->retry) == 0) {
			/* too many errors */
			ret = -1;
			break;
		}
	}


	return usb_ymodem_rcv_cancel(rx);
}

unsigned long dec2int(const char * __s)
{
	unsigned long val = 0;
	char * cp = (char *)__s;
	char c;

	while ((c = *cp) != '\0') {
		if ((c < '0') || (c > '9'))
			break;
		val = val * 10;
		val += c - '0';
		cp++;
	}

	return val;
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

static const char s_xmodem[] = "\r\nYmodem (^X to cancel)... ";
static const char s_invalid[] = "\r\nInvalid file!";
int __attribute__((noreturn)) yflash(uint32_t blk_offs, unsigned int blk_size, 
									 const struct magic * magic)
{
	struct {
		struct magic_hdr hdr;
		struct magic_rec rec[MAGIC_REC_MAX];
	} magic_buf;
	struct ymodem_rcv ry;
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
		usb_send(CDC_TX_EP, s_xmodem, sizeof(s_xmodem) - 1);
		ymodem_rcv_init(&ry, true, false);
		ry.fsize = blk_size;
		offs = blk_offs;
		cnt = 0;

		while ((ret = usb_ymodem_rcv_pkt(&ry)) >= 0) {
			if ((ret == 0) && (ry.xmodem) )
				break;
			int len = ret;
			if (ry.pktno == 1) {
				char * cp;
				int fsize;
				cp = (char *)ry.pkt.data;
				while (*cp != '\0')
					cp++;
				cp++; /* skip null */
				fsize = dec2int(cp);
				if (fsize == 0) {
					ret = 0;
					break;
				}
				ry.fsize = fsize;
			} else {
				if (ry.pktno == 2) {
					if (!magic_match((struct magic *)&magic_buf, 
									 cnt, ry.pkt.data, len)) {
						usb_ymodem_rcv_cancel(&ry);
#if 0
						usb_ymodem_rcv_cancel(&ry);
						delay(1000);
						usb_send(CDC_TX_EP, s_invalid, sizeof(s_invalid) - 1);
#endif
						ret = -1;
						break;
					}
//					flash_erase(offs, ry->fsize);
				}

				flash_erase(offs, len);
				flash_write(offs, ry.pkt.data, len);
				offs += len;
				cnt += len;
			}
		}

	} while ((ret < 0) || (cnt == 0));

	usb_send(CDC_TX_EP, "\r\nDone.\r\n", 9);

	usb_drain(CDC_TX_EP);

	delay(3000);

	reset();
}


