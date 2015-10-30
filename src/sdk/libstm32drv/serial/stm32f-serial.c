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
 * @file stm32f-serial.c
 * @brief STM32F serial driver
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#define __STM32_SERIAL_I__
#include "stm32-serial-i.h"
#include <sys/param.h>
#include <errno.h>

#include <arch/cortex-m3.h>

void stm32f_serial_isr(struct stm32f_serial_drv * drv)
{
	struct stm32_usart * uart = drv->uart;
	uint32_t cr;
	uint32_t sr;
	int c;
	
	cr = uart->cr1;

	sr = uart->sr & (cr | USART_ORE | USART_LBD);

	/* break detection */
	if (sr & USART_LBD) {
		/* clear the break detection interrupt flag */
		uart->sr = sr & ~(USART_ORE | USART_LBD);

		DCC_LOG(LOG_INFO, "BRK");

		/* remove the break char from the queue */
		drv->rx_fifo.head--;
#if SERIAL_ENABLE_STATS
		drv->stats.rx_brk++;
#endif
		/* signal the waiting thread */
		thinkos_gate_open_i(drv->rx_gate);
		return;
	}

	if (sr & USART_IDLE) { /* idle detection */
		DCC_LOG(LOG_INFO, "IDLE!");
		c = uart->rdr;
		(void)c;
#if SERIAL_ENABLE_STATS
		drv->stats.rx_idle++;
#endif
		thinkos_gate_open_i(drv->rx_gate);
		return;
	}

	if (sr & USART_RXNE) {
		uint32_t head;
		int free;

		c = uart->rdr;
#if SERIAL_ENABLE_STATS
		drv->stats.rx_cnt++;
#endif

		head = drv->rx_fifo.head;
		free = SERIAL_RX_FIFO_LEN - (head - drv->rx_fifo.tail);
		if (free > 0) { 
			drv->rx_fifo.buf[head & (SERIAL_RX_FIFO_LEN - 1)] = c;
			drv->rx_fifo.head = head + 1;
			free--;
			if (free == drv->rx_trig) {
				DCC_LOG(LOG_MSG, "---");
				thinkos_gate_open_i(drv->rx_gate);
			}
		} else {
			DCC_LOG(LOG_INFO, "RX fifo full!");
#if SERIAL_ENABLE_STATS
			drv->stats.rx_drop++;
#endif
		}
	}

#if 1
	if (sr & USART_TXE) {
		uint32_t tail = drv->tx_fifo.tail;
		if (tail == drv->tx_fifo.head) {
			/* FIFO empty, disable TXE interrupts */
	//		*drv->txie = 0; 
			uart->cr1 = cr & ~USART_TXEIE;
			thinkos_gate_open_i(drv->tx_gate);
		} else {
			uart->tdr = drv->tx_fifo.buf[tail++ % SERIAL_TX_FIFO_LEN];
			drv->tx_fifo.tail = tail;
		}
	}
#else
	if (sr & USART_TXE) {
		uint32_t tail = drv->tx_fifo.tail;
		if (tail == drv->tx_fifo.head) {
			/* FIFO empty, disable TXE interrupts, Enable TC interrupt */
			uart->cr1 = (cr & ~USART_TXEIE) | USART_TCIE;
		} else {
			uart->tdr = drv->tx_fifo.buf[tail++ % SERIAL_TX_FIFO_LEN];
			drv->tx_fifo.tail = tail;
		}
	}
#endif

	if (sr & USART_TC) {
		DCC_LOG1(LOG_JABBER, "UART%d TC.", stm32_usart_lookup(uart) + 1);
		/* TC interrupt is cleared by writing 0 back to the SR register */
		uart->sr = sr & ~USART_TC;
		/* disable the transfer complete interrupt */
		cr &= ~USART_TCIE;
		if (cr & USART_IDLEIE) {
			/* Pulse TE to generate an IDLE Frame */
			uart->cr1 = cr & ~USART_TE;
			uart->cr1 = cr | USART_TE;
		} else {
			/* Generate a brake condition */
			uart->cr1 = cr | USART_SBK;
			thinkos_gate_open_i(drv->tx_gate);
		}
	}

	if (sr & USART_ORE) {
		DCC_LOG(LOG_INFO, "OVR!");
#if SERIAL_ENABLE_STATS
		drv->stats.err_ovr++;
#endif
	}
}

int stm32f_serial_init(struct stm32f_serial_drv * drv, 
					   unsigned int baudrate, unsigned int flags)
{
	struct stm32_usart * uart = drv->uart;

	DCC_LOG1(LOG_INFO, "UART=0x%08x", uart);
	DCC_LOG1(LOG_INFO, "SERIAL_RX_FIFO_LEN=%d", SERIAL_RX_FIFO_LEN);
	DCC_LOG1(LOG_INFO, "SERIAL_RX_TRIG_LVL=%d", SERIAL_RX_TRIG_LVL);
	DCC_LOG1(LOG_INFO, "SERIAL_TX_FIFO_LEN=%d", SERIAL_TX_FIFO_LEN);
	DCC_LOG1(LOG_INFO, "SERIAL_ENABLE_TX_MUTEX=%d", SERIAL_ENABLE_TX_MUTEX);

	drv->rx_gate = thinkos_gate_alloc(); 
	drv->tx_gate = thinkos_gate_alloc(); 
#if SERIAL_ENABLE_TX_MUTEX
	drv->tx_mutex = thinkos_mutex_alloc(); 
	DCC_LOG1(LOG_INFO, "tx_mutex=%d", drv->tx_mutex);
#endif

	drv->tx_fifo.head = drv->tx_fifo.tail = 0;
	drv->rx_fifo.head = drv->rx_fifo.tail = 0;
	drv->rx_trig = SERIAL_RX_TRIG_LVL;

//	drv->txie = CM3_BITBAND_DEV(&uart->cr1, 7);
	thinkos_gate_open(drv->tx_gate);

	stm32_usart_init(uart);
	stm32_usart_baudrate_set(uart, baudrate);
	stm32_usart_mode_set(uart, SERIAL_8N1);

	/* enable RX interrupt */
	uart->cr1 |= USART_RXNEIE;

	drv->uart->sr &= ~USART_TC;

	if (flags & SERIAL_EOT_BREAK) {
		/* line break detection */
		drv->uart->cr2 |= USART_LBDIE;
	} else {
		/* enable RX IDLE interrupt */
		drv->uart->cr1 |= USART_IDLEIE;
	}

	/* enable UART */
	drv->uart->cr1 |= USART_UE | USART_TE | USART_RE;


	return 0;
}

int stm32f_serial_recv(struct stm32f_serial_drv * drv, void * buf, 
					   unsigned int len, unsigned int tmo)
{
	uint8_t * cp = (uint8_t *)buf;
	uint32_t tail;
	int ret;
	int cnt;
	int n;
	int i;

	DCC_LOG2(LOG_INFO, "1. len=%d tmo=%d", len, tmo);

//	thinkos_trace("stm32f_serial_recv() 1.");

again:

	thinkos_trace("stm32f_serial_recv() 2.");

	if ((ret = thinkos_gate_timedwait(drv->rx_gate, tmo)) < 0) {
		DCC_LOG1(LOG_INFO, "cnt=%d, timeout!", 
				 drv->rx_fifo.head - drv->rx_fifo.tail);

		thinkos_trace("stm32f_serial_recv() 3.");

		return ret;
	}

	thinkos_trace("stm32f_serial_recv() 4.");

	tail = drv->rx_fifo.tail;
	cnt = drv->rx_fifo.head - tail;
	if (cnt == 0) {
		DCC_LOG(LOG_WARNING, "RX FIFO empty!");
		thinkos_gate_exit(drv->rx_gate, 0);
		goto again;
	}
	n = MIN(len, cnt);

	DCC_LOG3(LOG_INFO, "head=%d tail=%d len=%d", drv->rx_fifo.head, 
			 drv->rx_fifo.tail, n);

	for (i = 0; i < n; ++i)
		cp[i] = drv->rx_fifo.buf[tail++ & (SERIAL_RX_FIFO_LEN - 1)];

	drv->rx_fifo.tail = tail;

	thinkos_gate_exit(drv->rx_gate, cnt > n);

//	DCC_LOG1(LOG_INFO, "len=%d", n);
//	DCC_LOG2(LOG_INFO, "len=%d '%c'...", n, cp[0]);

	return n;
}

int stm32f_serial_send(struct stm32f_serial_drv * drv, const void * buf,
						unsigned int len)
{
	struct stm32_usart * uart = drv->uart;
	uint8_t * cp = (uint8_t *)buf;
	int rem = len;
	uint32_t cr;


	DCC_LOG1(LOG_INFO, "len=%d", len);

#if SERIAL_ENABLE_TX_MUTEX
	 thinkos_mutex_lock(drv->tx_mutex); 
#endif

	while (rem) {
		uint32_t head;
		int free;
		int n;
		int i;

		thinkos_gate_wait(drv->tx_gate);

		head = drv->tx_fifo.head;
		free = SERIAL_TX_FIFO_LEN - (head - drv->tx_fifo.tail);
		n = MIN(rem, free);
		DCC_LOG3(LOG_INFO, "head=%d tail=%d n=%d", head, drv->tx_fifo.tail, n);
		for (i = 0; i < n; ++i) 
			drv->tx_fifo.buf[head++ % SERIAL_TX_FIFO_LEN] = cp[i];
		drv->tx_fifo.head = head;

		/* enable TX empty interrupt */
//		*drv->txie = 1;

		do {
			cr = __ldrex((uint32_t *)&uart->cr1);
			cr |= USART_TXEIE;
		} while (__strex((uint32_t *)&uart->cr1, cr));

		free -= n;
		rem -= n;
		DCC_LOG2(LOG_INFO, "rem=%d free=%d", rem, free);

		thinkos_gate_exit(drv->tx_gate, free);

		cp += n;
	}

#if SERIAL_ENABLE_TX_MUTEX
	thinkos_mutex_unlock(drv->tx_mutex); 
#endif

	return len;
}

int stm32f_serial_drain(struct stm32f_serial_drv * drv)
{
	do {
		thinkos_gate_wait(drv->tx_gate);
	} while ((drv->tx_fifo.head - drv->tx_fifo.tail) > 0);

	stm32_usart_flush(drv->uart);

	thinkos_gate_exit(drv->tx_gate, 0);

	return 0;
}

int stm32f_serial_close(struct stm32f_serial_drv * drv)
{
	return 0;
}

int stm32f_serial_ioctl(struct stm32f_serial_drv * drv, int opt, 
						uintptr_t arg1, uintptr_t arg2)
{
	struct stm32_usart * us = drv->uart;
	unsigned int msk = 0;

	switch (opt) {
	case SERIAL_IOCTL_ENABLE:
		DCC_LOG(LOG_INFO, "SERIAL_IOCTL_ENABLE");
		msk |= (arg1 & SERIAL_RX_EN) ? USART_RE : 0;
		msk |= (arg1 & SERIAL_TX_EN) ? USART_TE : 0;
		us->cr1 |= msk;
		break;

	case SERIAL_IOCTL_DISABLE:
		DCC_LOG(LOG_INFO, "SERIAL_IOCTL_DISABLE");
		msk |= (arg1 & SERIAL_RX_EN) ? USART_RE : 0;
		msk |= (arg1 & SERIAL_TX_EN) ? USART_TE : 0;
		us->cr1 &= ~msk;
		break;

	case SERIAL_IOCTL_DRAIN:
		DCC_LOG(LOG_INFO, "SERIAL_IOCTL_DRAIN");
		stm32f_serial_drain(drv);
		break;

	case SERIAL_IOCTL_RESET:
		DCC_LOG(LOG_INFO, "SERIAL_IOCTL_RESET");
		stm32f_serial_drain(drv);
		drv->tx_fifo.head = 0;
		drv->tx_fifo.tail = 0;
		break;

	case SERIAL_IOCTL_STATS_GET:
		{
			struct serial_stats * stats = (struct serial_stats *)arg1;

			DCC_LOG(LOG_INFO, "SERIAL_IOCTL_STATS_GET");
			stats->rx_cnt = drv->rx_fifo.head;
			stats->tx_cnt = drv->tx_fifo.tail;
			stats->err_cnt = 0;
			break;
		}

	case SERIAL_IOCTL_RX_TRIG_SET:
		DCC_LOG(LOG_INFO, "SERIAL_IOCTL_RX_TRIG_SET");
		drv->rx_trig = (unsigned int)arg1 < SERIAL_RX_FIFO_LEN ? arg1 : 0;
		break;

	case SERIAL_IOCTL_CONF_SET: 
		{
			struct serial_config * cfg = (struct serial_config *)arg1;
			uint32_t flags;
			DCC_LOG(LOG_INFO, "SERIAL_IOCTL_CONF_SET");

			stm32_usart_baudrate_set(us, cfg->baudrate);
			flags = CFG_TO_FLAGS(cfg);
			stm32_usart_mode_set(us, flags);
		}
		break;

	default:
		return -EINVAL;
	}

	return 0;
}

const struct serial_op stm32f_uart_serial_op = {
	.send = (void *)stm32f_serial_send,
	.recv = (void *)stm32f_serial_recv,
	.drain = (void *)stm32f_serial_drain,
	.close = (void *)stm32f_serial_close,
	.ioctl = (void *)stm32f_serial_ioctl
};

