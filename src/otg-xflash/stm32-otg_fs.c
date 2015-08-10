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
#include <stm32f/stm32f-otg_fs.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/param.h>

#define STM32F_BASE_OTG_FS  0x50000000
#define STM32F_OTG_FS ((struct stm32f_otg_fs *)STM32F_BASE_OTG_FS)

void dbg_putstr(const char * s);
void dbg_puthex(unsigned int val);

const uint8_t stm32f_otg_fs_ep0_mpsiz_lut[] = {
	64, 32, 16, 8
};

int usb_send(int ep_id, void * buf, unsigned int len)
{
	struct stm32f_otg_fs * otg_fs = STM32F_OTG_FS;
	unsigned int rem;
	uint8_t * xfrptr;

	rem = len;
	xfrptr = (uint8_t *)buf;

	while (rem > 0) {
		uint32_t deptsiz;
		uint32_t depctl;
		uint32_t mpsiz;
		uint32_t xfrsiz;
		uint32_t pktcnt;
		uint32_t data;
		int i;

		do {
			/* wait for room in the fifo */
			deptsiz = otg_fs->inep[ep_id].dieptsiz;
			xfrsiz = OTG_FS_XFRSIZ_GET(deptsiz);
			pktcnt = OTG_FS_PKTCNT_GET(deptsiz);
		} while ((xfrsiz == 0) && (pktcnt));

		depctl = otg_fs->inep[ep_id].diepctl;
		if (ep_id == 0)
			mpsiz = OTGFS_EP0_MPSIZ_GET(depctl);
		else
			mpsiz = OTG_FS_MPSIZ_GET(depctl);

		if (rem > 0) {
			pktcnt = (rem + (mpsiz - 1)) / mpsiz;
			if (pktcnt > 7) {
				pktcnt = 7;
				xfrsiz = 7 * mpsiz;
			} else {
				xfrsiz = rem;
			}
		} else {
			pktcnt = 1;
			xfrsiz = 0;
		}

		otg_fs->inep[ep_id].dieptsiz = OTG_FS_PKTCNT_SET(pktcnt) | 
			OTG_FS_XFRSIZ_SET(xfrsiz); 

		/* enable end point, clear NACK */
		otg_fs->inep[ep_id].diepctl = depctl | OTG_FS_EPENA | OTG_FS_CNAK; 

		/* push into fifo */
		for (i = 0; i < xfrsiz; i += 4) {
			data = xfrptr[0] + (xfrptr[1] << 8) + 
				(xfrptr[2] << 16) + (xfrptr[3] << 24);
			otg_fs->dfifo[ep_id].push = data;
			xfrptr += 4;
		}	

		/* wait for transfer to complete */
		while (!(otg_fs->inep[ep_id].diepint & OTG_FS_XFRC));
		rem -= xfrsiz;
	}

	return len;
}

#define CDC_TX_EP 2
static void __ep_zlp_send(struct stm32f_otg_fs * otg_fs, int epnum)
{
	otg_fs->inep[epnum].dieptsiz = OTG_FS_PKTCNT_SET(1) | OTG_FS_XFRSIZ_SET(0);
	otg_fs->inep[epnum].diepctl |= OTG_FS_EPENA | OTG_FS_CNAK;
}

static void __pktbuf_copy(void * ptr,
						  volatile uint32_t * pop,
						  unsigned int cnt)
{
	uint8_t * dst = (uint8_t *)ptr;
	uint32_t data;
	int rem;
	int i;

	/* pop data from the fifo and copy to destination buffer */
	for (i = 0; i < (cnt / 4); ++i) {
		data = *pop;
		*dst++ = data;
		*dst++ = data >> 8;
		*dst++ = data >> 16;
		*dst++ = data >> 24;
	}

	if ((rem = cnt % 4) > 0) {
		/* remaining bytes */
		data = *pop;
		dst[0] = data;
		if (rem > 1)
			dst[1] = data >> 8;
		if (rem > 2)
			dst[2] = data >> 16;
	}
}

static void __pktbuf_discard(volatile uint32_t * pop, unsigned int cnt)
{
	uint32_t data;
	int i;

	/* pop data from the fifo and copy to destination buffer */
	for (i = 0; i < (cnt + 3) / 4; ++i) {
		data = *pop;
		(void)data;
	}
}


int usb_recv(int ep_id, void * buf, unsigned int len, unsigned int msec)
{
	struct cm3_systick * systick = CM3_SYSTICK;
	struct stm32f_otg_fs * otg_fs = STM32F_OTG_FS;
	uint32_t rxfsiz;
	uint32_t mxpktsz;
	uint32_t pktcnt;
	uint32_t gintsts;
	uint32_t setup[2];

	/* get EP max packet size */
	mxpktsz	= OTG_FS_MPSIZ_GET(otg_fs->outep[ep_id].doepctl);
	/* get the size of the RX fifio */
	rxfsiz = otg_fs->grxfsiz * 4;
	/* packets in the RX fifio */
	pktcnt = rxfsiz / mxpktsz;

	otg_fs->outep[ep_id].doepctl |= OTG_FS_CNAK;

	for (;;) {

		gintsts = otg_fs->gintsts & otg_fs->gintmsk;
		otg_fs->gintsts = gintsts;

		if (gintsts & OTG_FS_OEPINT) {
			uint32_t ep_intr = (otg_fs->daint & otg_fs->daintmsk);
			if (ep_intr & OTG_FS_OEPINT0) {
				uint32_t doepint = otg_fs->outep[0].doepint & 
					otg_fs->doepmsk;
				if (doepint & OTG_FS_XFRC)
					__ep_zlp_send(otg_fs, 0);
				if (doepint & OTG_FS_STUP)
					__ep_zlp_send(otg_fs, 0);
				otg_fs->outep[0].doepint = doepint;
			}
		}

		if (gintsts & OTG_FS_RXFLVL) {
			uint32_t grxsts;
			int epnum;
			int cnt;

			/* 1. On catching an RXFLVL interrupt (OTG_FS_GINTSTS register),
			   the application must read the Receive status pop
			   register (OTG_FS_GRXSTSP). */
			grxsts = otg_fs->grxstsp;
			cnt = OTG_FS_BCNT_GET(grxsts);
			epnum = OTG_FS_EPNUM_GET(grxsts);

			if (epnum == ep_id) {
				switch (grxsts & OTG_FS_PKTSTS_MSK) {
				case OTG_FS_PKTSTS_OUT_DATA_UPDT:
					__pktbuf_copy(buf, &otg_fs->dfifo[ep_id].pop, cnt);
					return cnt;
				case OTG_FS_PKTSTS_OUT_XFER_COMP:
					/* Prepare to receive more */
					otg_fs->outep[ep_id].doeptsiz = OTG_FS_PKTCNT_SET(pktcnt) | 
						OTG_FS_XFRSIZ_SET(pktcnt * mxpktsz);
					break;
				}
			} else if (epnum == 0) {
				switch (grxsts & OTG_FS_PKTSTS_MSK) {
				case OTG_FS_PKTSTS_OUT_DATA_UPDT:
					/* OUT data packet received */
					/* Number of words in the receive fifo */
					/* discard other EP's data */
					__pktbuf_discard(&otg_fs->dfifo[0].pop, cnt);
					break;
				case OTG_FS_PKTSTS_SETUP_UPDT:
					__pktbuf_copy(setup, &otg_fs->dfifo[0].pop, cnt);
					break;
				}
			} else {
				__pktbuf_discard(&otg_fs->dfifo[0].pop, cnt);
			}
		}

		if (systick->csr & SYSTICK_CSR_COUNTFLAG) {
			if (msec == 0) {
//				otg_fs->outep[ep_id].doepctl |= OTG_FS_CNAK;
				return -1;
			}
			msec--;
		}

	}
}


void usb_drain(int ep_id)
{
	struct stm32f_otg_fs * otg_fs = STM32F_OTG_FS;
	/* wait for transfer to complete */
	while (!(otg_fs->inep[ep_id].diepint & OTG_FS_XFRC));
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

