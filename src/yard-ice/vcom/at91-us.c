/* 
 * Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
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
 * @file .c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uthreads.h>
#include <sys/at91.h>
#include <sys/interrupts.h>
#include <sys/mclk.h>
#include <sys/serial.h>
#include <sys/at91.h>

#include <sys/dcclog.h>

#if defined(AT91SAM7X128) || defined(AT91SAM7X256)
#define __AT91_PMC AT91_PMC
#define ENABLE_AT91_US0 1
#define GPIO_RXD0 {.pio = PIOA, .pin = 0}
#define GPIO_TXD0 {.pio = PIOA, .pin = 1}
#define ENABLE_AT91_US1 1
#define GPIO_RXD1 {.pio = PIOA, .pin = 5}
#define GPIO_TXD1 {.pio = PIOA, .pin = 6}
#endif

#ifdef AT91X40
#define __AT91_PMC AT91_PS
#define ENABLE_AT91_US0 1
#define GPIO_RXD0 {.pio = PIOA, .pin = 15}
#define GPIO_TXD0 {.pio = PIOA, .pin = 14}
#define ENABLE_AT91_US1 1
#define GPIO_RXD1 {.pio = PIOA, .pin = 22}
#define GPIO_TXD1 {.pio = PIOA, .pin = 21}
#endif

struct us_def {
	struct at91_us * us;
	int id;
	at91_gpio_t gpio_rx;
	at91_gpio_t gpio_tx;
};

static const struct us_def us_tab[] = {
#if ENABLE_AT91_US0
	{ .us = AT91_US0, .id = AT91_ID_US0, 
	  .gpio_rx = GPIO_RXD0, .gpio_rx = GPIO_TXD0 },
#endif
#if ENABLE_AT91_US1
	{ .us = AT91_US1, .id = AT91_ID_US1, 
	  .gpio_rx = GPIO_RXD1, .gpio_rx = GPIO_TXD1 },
#endif
#if ENABLE_AT91_US2
	{ .us = AT91_US2, .id = AT91_ID_US2, 
	  .gpio_rx = GPIO_RXD2, .gpio_rx = GPIO_TXD2 },
#endif
#if ENABLE_AT91_US3
	{ .us = AT91_US3, .id = AT91_ID_US3, 
	  .gpio_rx = GPIO_RXD3, .gpio_rx = GPIO_TXD3 },
#endif
#if ENABLE_AT91_US4
	{ .us = AT91_US4, .id = AT91_ID_US4, 
	  .gpio_rx = GPIO_RXD4, .gpio_rx = GPIO_TXD4 },
#endif
#if ENABLE_AT91_US5
	{ .us = AT91_US5, .id = AT91_ID_US5, 
	  .gpio_rx = GPIO_RXD5, .gpio_rx = GPIO_TXD5 },
#endif
};

void __attribute__((noreturn)) at91_us_isr(struct at91_us_dev * dev)
{
	struct at91_us * us = dev->us;
	int irq = dev->id;
	uint32_t status;
	uint32_t bits;

	DCC_LOG1(LOG_MSG, "sp=%p", __sp());

	for (;;) {

		if ((status = (us->csr & us->imr))) {
			uthread_mutex_lock(dev->mutex);

			if ((bits = status & (US_TXRDY | US_ENDTX)) != 0) {
				DCC_LOG1(LOG_MSG, "TX, status=%08x", status);
				us->idr = bits;
				uthread_cond_broadcast(dev->tx_cond);
			}

			if ((bits = status & (US_RXRDY | US_ENDRX | US_TIMEOUT)) != 0) {
				DCC_LOG1(LOG_MSG, "RX, status=%08x", status);
				us->idr = bits;
				uthread_cond_broadcast(dev->rx_cond);
			}

			if (status & US_ENDRX) {
				uthread_cond_broadcast(dev->rx_cond);
			}
				
			if (status & US_TIMEOUT) {
				uthread_cond_broadcast(dev->rx_cond);
			}

			uthread_mutex_unlock(dev->mutex);
		}

		/* wait for interrupt */
		uthread_int_wait(irq);
	}
}

int at91_us_init(struct at91_us_dev * dev, int port)
{
	struct at91_us * us = dev->us;
	int max = sizeof(us_tab) / sizeof(struct us_def);

	DCC_LOG(LOG_MSG, "1.");

	if (dev == NULL) {
		DCC_LOG(LOG_WARNING, "dev is NULL");
		return -EINVAL;
	}
	if (port > max) {
		DCC_LOG2(LOG_WARNING, "port=%d > max=%d", port, max);
		return -ENXIO;
	}

	dev->us = us_tab[port].us;
	dev->id = us_tab[port].id;

	at91_pio_periph_a(us_tab[port].gpio_rx);
	at91_pio_periph_a(us_tab[port].gpio_tx);

	us = dev->us;

	/* Enable peripheral clock */
	__AT91_PMC->pcer = (1 << dev->id);

	DCC_LOG1(LOG_MSG, "us=0x%p", us);

	/* disable all interrupts */
	us->idr = US_RXRDY | US_TXRDY | US_ENDRX | 
		US_ENDTX | US_OVRE | US_FRAME | 
		US_PARE | US_TXEMPTY;

	/* disable TX and RX */ 
	us->cr = US_TXDIS | US_RXDIS;

	dev->tx_cond = uthread_cond_alloc();
	dev->rx_cond = uthread_cond_alloc();
	dev->mutex = uthread_mutex_alloc();

	uthread_create(dev->isr_stack, sizeof(dev->isr_stack), 
				   (uthread_task_t)at91_us_isr, dev, 1, NULL);

	interrupt_config(dev->id, INT_POSITIVE_EDGE, 7);

	return 0;
}

int at91_us_write(struct at91_us_dev * dev, const void * buf, int len)
{
	struct at91_us * us = dev->us;

	DCC_LOG3(LOG_MSG, "buf=%p len=%d sp=%p", buf, len, __sp());

	uthread_mutex_lock(dev->mutex);

	if (len == 1) {
		while (!(us->csr & US_TXRDY)) {
			us->ier = US_TXRDY;
			DCC_LOG(LOG_MSG, "block 1");

			uthread_cond_wait(dev->tx_cond, dev->mutex);
		}
		us->thr = *(uint8_t *)buf;
	} else {
		while (!(us->csr & US_ENDTX)) {
			us->ier = US_ENDTX;
			DCC_LOG(LOG_MSG, "wait ENDTX 1");
			uthread_cond_wait(dev->tx_cond, dev->mutex);
		}


		us->tpr = (void *)buf;
		us->tcr = len;

		do {
			DCC_LOG(LOG_MSG, "wait ENDTX 2");
			us->ier = US_ENDTX;
			uthread_cond_wait(dev->tx_cond, dev->mutex);
		} while (!(us->csr & US_ENDTX));


	}

	uthread_mutex_unlock(dev->mutex);

	return len;
}

int at91_us_read(struct at91_us_dev * dev, void * buf, int len, int msec)
{
	struct at91_us * us = dev->us;
	int csr;

	DCC_LOG2(LOG_MSG, "buf=0x%p len=%d", buf, len);

	DCC_LOG1(LOG_MSG, "msec=%d", msec);

	uthread_mutex_lock(dev->mutex);

	if (len == 1) {
		/* only one position buffer, 
		 get from the receiver holder directly */
		char * cp = (char *)buf;

		while (!(us->csr & US_RXRDY)) {
			us->ier = US_RXRDY;

			if (uthread_cond_timedwait(dev->rx_cond, dev->mutex, msec) < 0) {
				if (errno == ETIMEDOUT) {
					DCC_LOG(LOG_MSG, "TIMEOUT 1");
					uthread_mutex_unlock(dev->mutex);
					return 0;
				}
			}
		}	

		*cp = us->rhr;

		uthread_mutex_unlock(dev->mutex);

		return len;
	}

	while (!(us->csr & US_ENDRX)) {
		DCC_LOG(LOG_MSG, "receiving???");
		us->ier = US_ENDRX | US_TIMEOUT;
		if (uthread_cond_timedwait(dev->rx_cond, dev->mutex, msec) < 0) {
			if (errno == ETIMEDOUT) {
				uthread_mutex_unlock(dev->mutex);
				DCC_LOG(LOG_MSG, "TIMEOUT 2");
				return 0;
			}
		}
	}

	/* configure DMA */
	DCC_LOG(LOG_MSG, "configure DMA");
	us->rpr = buf;
	us->rcr = len;

	us->cr = US_STTTO;

	for (;;) {
		DCC_LOG(LOG_MSG, "enable IRQ");
		us->ier = US_ENDRX | US_TIMEOUT;
		
		if (uthread_cond_timedwait(dev->rx_cond, dev->mutex, msec) < 0) {
			if (errno == ETIMEDOUT) {
				DCC_LOG(LOG_MSG, "TIMEOUT 3");
				us->rcr = 0;
				uthread_mutex_unlock(dev->mutex);
				return 0;
			}
		}

		csr = us->csr;

		if (csr & US_ENDRX) {
			DCC_LOG(LOG_MSG, "DMA complete");
			break;
		}

		if (csr & US_TIMEOUT) {
			int n;
			/* restart timeout timer */
			us->cr = US_STTTO;
			if ((n = len - us->rcr) == 0) {
				DCC_LOG(LOG_MSG, "0 bytes !!!");
				continue;
			}
			/* release the DMA channel */
			us->rcr = 0;
			len = n;
			DCC_LOG(LOG_MSG, "DMA timeout!");
			break;
		}
	}


	uthread_mutex_unlock(dev->mutex);

	return len;
}

int at91_us_config(struct at91_us_dev * dev, int baudrate, 
				   int mode, int tmo_bits)
{
	struct at91_us * us = dev->us;
	uint32_t mr = 0;
	int tmp;

	DCC_LOG(LOG_MSG, "1.");

	uthread_mutex_lock(dev->mutex);

	/* save interrupt configuration */
	tmp = us->imr;
 
	/* disables all interrupts */
	us->idr = US_RXRDY | US_TXRDY | US_RXBRK | US_ENDRX | US_ENDTX | \
	              US_OVRE | US_FRAME | US_PARE | US_TIMEOUT | US_TXEMPTY;
	/* flush */ 
	us->cr = US_TXEN | US_RXDIS;

	while (!(us->csr & US_TXEMPTY));

	/* disable receiver & transmitter */
	us->cr = US_RXDIS | US_TXDIS;

	/* set character length */
	switch (mode & SERIAL_DATABITS_MASK) {
	case SERIAL_DATABITS_5:
		mr |= US_CHRL_5;
		break;
	case SERIAL_DATABITS_6:
		mr |= US_CHRL_6;
		break;
	case SERIAL_DATABITS_7:
		mr |= US_CHRL_7;
		break;
	case SERIAL_DATABITS_8:
		mr |= US_CHRL_8;
		break;
	}

	/* set parity */
	switch (mode & SERIAL_PARITY_MASK) {
	case SERIAL_PARITY_ODD:
		mr |= US_PAR_ODD;
		break;
	case SERIAL_PARITY_EVEN:
		mr |= US_PAR_EVEN;
		break;
	default:
		mr |= US_PAR_NO;
	}

	/* sets stop bits */
	if ((mode & SERIAL_STOPBITS_MASK) == SERIAL_STOPBITS_2)
		mr |= US_NBSTOP_2;
	else
		mr |= US_NBSTOP_1;

//	mr |= US_CHMODE_LOCAL_LOOPBACK;

	/* set mode register */
	us->mr = mr;

	/* set baud rate */
	us->brgr = (usec2mclk((1000000 << 8) / baudrate) + (1 << 11)) >> 12;

	/* set the receiver timeout bit times */
	us->rtor = tmo_bits >> 2;

	DCC_LOG2(LOG_MSG, "brg=%d rtor=%d", us->brgr, us->rtor);

	/* Stops Peripheral Data Transfer dedicated to the transmmiter */
	us->tcr = 0;
#ifndef AT91X40
	us->ptcr = PDC_TXTEN;
#endif

	/* Stops Peripheral Data Transfer dedicated to the receiver */
	us->rcr = 0;
#ifndef AT91X40
	us->ptcr = PDC_RXTEN;
#endif

	/* reset receiver, reset transmitter, reset status */
	us->cr = US_RSTRX | US_RSTTX | US_RSTSTA;

	/* enable transmitter */
	/* enable receiver, start timeout */
	us->cr = US_TXEN | US_RXEN | US_STTTO;

	/* restore interrupt configuration */
	us->ier = tmp;

	uthread_mutex_unlock(dev->mutex);

	return 0;
}

