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

#include <stdint.h>
#include <string.h>

#include <arch/cortex-m3.h>

#if 0
#include <sys/param.h>
#include <crc.h>
#include <sys/dcclog.h>

#define FLASH_WR_BLK_SIZE 128

void flash_unlock(void);
int flash_write(uint32_t offs, const void * buf, unsigned int len);
int flash_erase(unsigned int offs, unsigned int len);

void uart_reset(void * uart);
void uart_drain(void * uart);
int uart_send(void * uart, const void * buf, unsigned int len);
int uart_recv(void * uart, void * buf, unsigned int len, unsigned int msec);

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

static int xmodem_rcv_init(struct xmodem_rcv * rx, int mode)
{
	rx->mode = mode;
	rx->pktno = 1;
#ifndef ENABLE_XMODEM_CKS
	rx->sync = (rx->mode == XMODEM_RCV_CRC) ? 'C' : NAK;
#else
	rx->sync = 'C';
#endif
	rx->retry = 10;

	return 0;
}

static int xmodem_rcv_pkt(void * uart, struct xmodem_rcv * rx)
{
	unsigned char * pkt = rx->pkt.hdr;
	unsigned char * cp;
	int ret = 0;
	int cnt = 0;
	int nseq;
	int seq;
	int rem;

	for (;;) {

		DCC_LOG1(LOG_TRACE, "uart_send() -> %c", rx->sync);

		if ((ret = uart_send(uart, &rx->sync, 1)) < 0) {
			return ret;
		}

		for (;;) {
			int c;

			ret = uart_recv(uart, pkt, 1, 2000);

			if (ret <= 0)
				goto timeout;

			c = pkt[0];

			if (c == STX) {
				DCC_LOG(LOG_TRACE, "STX");
				cnt = 1024;
				break;
			}

			if (c == SOH) {
				DCC_LOG(LOG_TRACE, "SOH");
				cnt = 128;
				break;
			}

			if (c == EOT) {
				DCC_LOG(LOG_TRACE, "EOT");
				/* end of transmission */
				pkt[0] = ACK;
				pkt[1] = ACK;
				if ((ret = uart_send(uart, pkt, 2)) < 0)
					return ret;
				return 0;
			}

			DCC_LOG1(LOG_WARNING, "uart_recv() <- %02x", c);
		}

#ifndef ENABLE_XMODEM_CKS
		rem = cnt + ((rx->mode) ? 4 : 3);
#else
		rem = cnt + 4;
#endif
		cp = pkt + 1;

		DCC_LOG1(LOG_TRACE, "rem=%d", rem);

		/* receive the packet */
		while (rem) {

			ret = uart_recv(uart, cp, rem, 500);
			if (ret <= 0) {
				DCC_LOG1(LOG_WARNING, "Timeout: rem=%d", rem);
				goto timeout;
			}

//			DCC_LOG3(LOG_TRACE, "c=%02x ret=%d rem=%d", *cp, ret, rem);

			rem -= ret;
			cp += ret;
		}


		/* sequence */
		seq = pkt[1];
		/* inverse sequence */
		nseq = pkt[2];

		DCC_LOG2(LOG_TRACE, "seq=%d cnt=%d", seq, cnt);

		if (seq != ((~nseq) & 0xff)) {
			DCC_LOG2(LOG_ERROR, "seq=%02x nseq=%02x", seq, nseq);
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
		DCC_LOG(LOG_TRACE, "ERROR");

		/* flush */
		while (uart_recv(uart, pkt, 1024, 200) > 0);
		rx->sync = NAK;

timeout:
		DCC_LOG(LOG_TRACE, "TIMEOUT");

		if ((--rx->retry) == 0) {
			/* too many errors */
			ret = -1;
			break;
		}
	}

	pkt[0] = CAN;
	pkt[1] = CAN;
	pkt[2] = CAN;

	uart_send(uart, pkt, 3);

	return ret;
}

static void reset(void)
{
	DCC_LOG(LOG_TRACE, "...");
    CM3_SCB->aircr =  SCB_AIRCR_VECTKEY | SCB_AIRCR_SYSRESETREQ;
	for(;;);
}

int xflash(void * uart, uint32_t offs, unsigned int size)
{
	struct xmodem_rcv rx;
	unsigned int cnt = 0;
	int ret;

	uart_reset(uart);

	xmodem_rcv_init(&rx, XMODEM_RCV_CRC);

	DCC_LOG2(LOG_TRACE, "offs=%08x size=%d", offs, size);

	ret = xmodem_rcv_pkt(uart, &rx);
	if (ret <= 0)
		return ret;

	DCC_LOG1(LOG_TRACE, "xmodem_rcv_pkt() -> %d", ret);
	flash_erase(offs, size);

	while (ret > 0) {
		unsigned char * src = rx.pkt.data;
		int rem = ret;

		while (rem > 0) {
			int n;

			n = MIN(rem, FLASH_WR_BLK_SIZE);
			n = MIN(n, size - cnt);
			if (n == 0)
				break;

			DCC_LOG1(LOG_TRACE, "flash_write() -> %d", ret);
			flash_write(offs, src, n);

			offs += n;
			cnt += n;
			rem -= n;
		}

		ret = xmodem_rcv_pkt(uart, &rx);
		DCC_LOG1(LOG_TRACE, "xmodem_rcv_pkt() -> %d", ret);
	} 

	uart_drain(uart);

	reset();

	return 0;
}

#else

extern const uint8_t uart_xflash_pic[];
extern const unsigned int sizeof_uart_xflash_pic;
extern uint32_t __data_start[];

void __attribute__((noreturn)) uart_xflash(void * uart, 
										   uint32_t offs, uint32_t len)
{
	uint32_t * xflash_code = __data_start;
	int (* xflash_ram)(void *, uint32_t, uint32_t) = ((void *)xflash_code) + 1;

	cm3_cpsid_i();

	memcpy(xflash_code, uart_xflash_pic, sizeof_uart_xflash_pic);

	xflash_ram(uart, offs, len);

	cm3_sysrst();
}


#endif

