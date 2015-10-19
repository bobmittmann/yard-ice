/* 
 * Copyright(C) 2013 Robinson Mittmann. All Rights Reserved.
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
 * @file stm32f-usb_fs_dev.c
 * @brief STM32F USB Full Speed Device Driver
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <arch/cortex-m3.h>
#include <stm32f/stm32f-usb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/param.h>

#define STM32F_BASE_USB     0x40005c00
#define STM32F_USB_PKTBUF_ADDR 0x40006000
#define STM32F_USB_PKTBUF_SIZE 512
#define STM32F_USB_PKTBUF ((struct stm32f_usb_pktbuf *)STM32F_USB_PKTBUF_ADDR)
#define STM32F_USB ((struct stm32f_usb *)STM32F_BASE_USB)

/* -------------------------------------------------------------------------
 * End point packet buffer helpers
 * ------------------------------------------------------------------------- */

static void __copy_from_pktbuf(void * ptr,
							   struct stm32f_usb_rx_pktbuf * rx,
							   unsigned int cnt)
{
	uint32_t * src;
	uint8_t * dst = (uint8_t *)ptr;
	uint32_t data;
	int i;

	/* copy data to destination buffer */
	src = (uint32_t *)STM32F_USB_PKTBUF_ADDR + (rx->addr / 2);
	for (i = 0; i < (cnt + 1) / 2; i++) {
		data = *src++;
		*dst++ = data;
		*dst++ = data >> 8;
	}
}

static void __copy_to_pktbuf(struct stm32f_usb_tx_pktbuf * tx,
							 uint8_t * src, int len)
{
	uint32_t * dst;
	int i;

	/* copy data to destination buffer */
	dst = (uint32_t *)STM32F_USB_PKTBUF_ADDR + (tx->addr / 2);
	for (i = 0; i < ((len + 1) / 2); i++) {
		*dst++ = src[0] | (src[1] << 8);
		src += 2;
	}

	tx->count = len;
}

/* ------------------------------------------------------------------------- */

int usb_recv(int ep_id, void * buf, unsigned int len, unsigned int msec)
{
	struct cm3_systick * systick = CM3_SYSTICK;
	struct stm32f_usb_pktbuf * pktbuf = STM32F_USB_PKTBUF;
	struct stm32f_usb * usb = STM32F_USB;
	struct stm32f_usb_rx_pktbuf * rx_pktbuf;
	uint32_t epr;
	int cnt;

	epr = usb->epr[ep_id];
again:
	if (epr & USB_EP_DBL_BUF) {
		/* select the descriptor according to the data toggle bit */
		rx_pktbuf = &pktbuf[ep_id].dbrx[(epr & USB_SWBUF_RX) ? 1 : 0];
	} else {
		/* single buffer */
		rx_pktbuf = &pktbuf[ep_id].rx;
	}
	if (rx_pktbuf->count == 0) {
		while (!(epr & USB_CTR_RX)) {
			if (systick->csr & SYSTICK_CSR_COUNTFLAG) {
				if (msec == 0)
					return -1;
				msec--;
			}
			epr = usb->epr[ep_id];
		}
		/* OUT */
		__clr_ep_flag(usb, ep_id, USB_CTR_RX);
		goto again;
	}

	cnt = MIN(rx_pktbuf->count, len);

	/* Data received */
	__copy_from_pktbuf(buf, rx_pktbuf, cnt);
	rx_pktbuf->count = 0;

	if (epr & USB_EP_DBL_BUF) {
		/* release the buffer to the USB controller */
		__toggle_ep_flag(usb, ep_id, USB_SWBUF_RX);
	} else {
		/* free the out(rx) packet buffer */
		__set_ep_rxstat(usb, ep_id, USB_RX_VALID);
	}

	return cnt;
}

/* start sending */
int usb_send(int ep_id, void * buf, unsigned int len)
{
	struct stm32f_usb_pktbuf * pktbuf = STM32F_USB_PKTBUF;
	struct stm32f_usb_tx_pktbuf * tx_pktbuf;
	struct stm32f_usb * usb = STM32F_USB;
	uint32_t epr;
	unsigned int mxpktsz = 64; /* Maximum packet size for this EP */
	unsigned int rem; /* Bytes pendig in the transfer buffer */
	uint8_t * ptr; /* Pointer to the next transfer */
	unsigned int pktsz = 0;

	

	epr = usb->epr[ep_id];
	ptr = (uint8_t *)buf;
	rem = len;

	while (rem > 0) {
		if (epr & USB_EP_DBL_BUF) {
			/* select the descriptor according to the data toggle bit */
			tx_pktbuf = &pktbuf[ep_id].dbtx[(epr & USB_SWBUF_TX) ? 1: 0];
		} else {
			tx_pktbuf = &pktbuf[ep_id].tx;
		}
		pktsz = MIN(rem, mxpktsz);
		__copy_to_pktbuf(tx_pktbuf, ptr, pktsz);
		rem -= pktsz;
		ptr += pktsz;

		if (epr & USB_EP_DBL_BUF) {
			__toggle_ep_flag(usb, ep_id, USB_SWBUF_TX);
		} else {
			__set_ep_txstat(usb, ep_id, USB_TX_VALID);
		}

		while (!((epr = usb->epr[ep_id]) & USB_CTR_TX)) {
		}

		/* IN */
		__clr_ep_flag(usb, ep_id, USB_CTR_TX);
	}

	if (pktsz == mxpktsz) {
		if (epr & USB_EP_DBL_BUF) {
			/* select the descriptor according to the data toggle bit */
			tx_pktbuf = &pktbuf[ep_id].dbtx[(epr & USB_SWBUF_TX) ? 1: 0];
			tx_pktbuf->count = 0;
			__toggle_ep_flag(usb, ep_id, USB_SWBUF_TX);
		} else {
			__set_ep_txstat(usb, ep_id, USB_TX_NAK);
		}
	}	

	if (!(epr & USB_EP_DBL_BUF)) {
		__set_ep_txstat(usb, ep_id, USB_TX_NAK);
	}

	return len;
}

void usb_drain(int ep_id)
{
	struct stm32f_usb_pktbuf * pktbuf = STM32F_USB_PKTBUF;
	struct stm32f_usb_tx_pktbuf * tx_pktbuf;
	struct stm32f_usb * usb = STM32F_USB;
	uint32_t epr;

	epr = usb->epr[ep_id];

	if (epr & USB_EP_DBL_BUF) {
		/* select the descriptor according to the data toggle bit */
		tx_pktbuf = &pktbuf[ep_id].dbtx[(epr & USB_SWBUF_TX) ? 1: 0];
		tx_pktbuf->count = 0;
		__toggle_ep_flag(usb, ep_id, USB_SWBUF_TX);
	} else {
		tx_pktbuf = &pktbuf[ep_id].tx;
		tx_pktbuf->count = 0;
		__set_ep_txstat(usb, ep_id, USB_TX_VALID);
	}

	while (!((epr = usb->epr[ep_id]) & USB_CTR_TX)) {
	}

	if (!(epr & USB_EP_DBL_BUF)) {
		__set_ep_txstat(usb, ep_id, USB_TX_NAK);
	}
}

#if 0
int uint2hex(char * s, unsigned int val)
{
	int n;
	int c;
	int i;

	/* value is zero ? */
	if (!val) {
		*s++ = '0';
		*s = '\0';
		return 1;
	}

	n = 0;
	for (i = 0; i < (sizeof(unsigned int) * 2); i++) {
		c = val >> ((sizeof(unsigned int) * 8) - 4);
		val <<= 4;
		if ((c != 0) || (n != 0)) {
			s[n++] = c < 10 ? c + '0' : c + ('a' - 10);
		}
	}

	s[n] = '\0';

	return n;
}

void usb_send_hex(int ep_id, unsigned int val)
{
	char buf[16];
	char * cp = buf;;
	int n;

	*cp++ = ' ';
	*cp++ = '0';
	*cp++ = 'x';
	n = uint2hex(cp, val);
	cp += n;
	*cp++ = '\r';
	*cp++ = '\n';
	n += 5;

	usb_send(ep_id, buf, n);
}

#endif

