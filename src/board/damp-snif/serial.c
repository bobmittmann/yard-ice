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
 * @file console.c
 * @brief YARD-ICE UART console
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include "board.h"
#include "capture.h"
#include "profclk.h"

#include <stdio.h>
#include <string.h>
#include <sys/param.h>

#include <thinkos.h>

#include <sys/dcclog.h>

_Pragma ("GCC optimize (\"Ofast\")")

#ifndef SERIAL_STATS_ENABLE
#define SERIAL_STATS_ENABLE 0
#endif

#define SERIAL_BAUDRATE 38400
//#define SERIAL_BAUDRATE 19200
#define RX_FIFO_LEN 16

struct capture_drv {
#if SERIAL_STATS_ENABLE
	uint32_t err_cnt;
	uint32_t ore_cnt;
	uint32_t fe_cnt;
#endif
	uint8_t idle_bits;
	uint16_t baudrate;
	uint32_t byte_time;
	uint32_t byte_time_max;
	struct {
		uint32_t clk;
		uint32_t seq;
		volatile uint32_t head;
		volatile uint32_t tail;
		struct packet buf[RX_FIFO_LEN];
	} rx_fifo;	
};

#define CTL_FLAG THINKOS_FLAG_DESC(SERDRV_CTL_FLAG_NO)
#define RX_SEM THINKOS_SEM_DESC(SERDRV_RX_SEM_NO)

struct capture_drv uart2_capture_drv;

#define BITS_TO_TICKS(BITS, BITRATE) ((((BITS) * PROFCLK_HZ) + \
									   ((BITRATE) / 2)) / (BITRATE))
void stm32f_usart2_isr(void)
{
	struct capture_drv * drv = &uart2_capture_drv;
	struct stm32_usart * uart = STM32_USART2;
	uint32_t sr;
	int c;
	
	sr = uart->sr;

	if (sr & USART_RXNE) {
		struct packet * pkt;
		uint32_t head;
		uint32_t clk;
		uint32_t diff;
		uint32_t cnt;
		int free;

		clk = profclk_get();
		diff = clk - drv->rx_fifo.clk;
		drv->rx_fifo.clk = clk;
	
		c = uart->dr;

		DCC_LOG1(LOG_TRACE, "%d", profclk_us(diff));

		head = drv->rx_fifo.head;
		pkt = &drv->rx_fifo.buf[head & (RX_FIFO_LEN - 1)];
		cnt = pkt->cnt;
		if ((cnt == PACKET_DATA_MAX) || (diff > drv->byte_time_max)) {
			/* end of packet */
			head++;
			free = RX_FIFO_LEN - (uint8_t)(head - drv->rx_fifo.tail);
			if (free != 0) { 
				drv->rx_fifo.head = head;
				thinkos_sem_post_i(RX_SEM);
				pkt = &drv->rx_fifo.buf[head & (RX_FIFO_LEN - 1)];
			} else {
#if SERIAL_STATS_ENABLE
				drv->err_cnt++;
#endif
			}
			pkt->seq = ++drv->rx_fifo.seq;
			pkt->clk = clk - drv->byte_time; 
			cnt = 0;
		}

		pkt->data[cnt++] = c;
		pkt->cnt = cnt;
		
	} else if (sr & USART_ORE) {
#if SERIAL_STATS_ENABLE
		drv->ore_cnt++;
#endif
	} else if (sr & USART_FE) {
#if SERIAL_STATS_ENABLE
		drv->fe_cnt++;
#endif
	}
}

struct packet * capture_pkt_recv(void)
{
	struct capture_drv * drv = &uart2_capture_drv;
	struct packet * pkt;
	uint32_t tail;
	int ret;
	int cnt;

	if ((ret = thinkos_sem_timedwait(RX_SEM, 500)) < 0) {
		return NULL;
	}

	tail = drv->rx_fifo.tail;
	cnt = (int32_t)(drv->rx_fifo.head - tail);
	if (cnt == 0) {
		DCC_LOG(LOG_ERROR, "RX FIFO empty!");
		return NULL;
	}

	pkt = &drv->rx_fifo.buf[tail++ & (RX_FIFO_LEN - 1)];
	drv->rx_fifo.tail = tail;

	return pkt;
}


void capture_start(void)
{
	struct stm32_usart * uart = STM32_USART2;

	uart->cr1 |= USART_RE;
}

void capture_stop(void)
{
	struct stm32_usart * uart = STM32_USART2;

	uart->cr1 &= ~USART_RE ;
}

void capture_baudrate_set(unsigned int rate)
{
	struct capture_drv * drv = &uart2_capture_drv;
	struct stm32_usart * uart = STM32_USART2;
	unsigned int bits;

	uart->brr = stm32f_apb1_hz / rate;

	bits = drv->idle_bits;
	drv->baudrate = rate;
	drv->byte_time = BITS_TO_TICKS(10, rate);
	if (bits == 0)
		drv->byte_time_max = BITS_TO_TICKS(21, rate * 2);
	else
		drv->byte_time_max = BITS_TO_TICKS(10 + drv->idle_bits, rate);

	DCC_LOG2(LOG_TRACE, "byte_time=%d max=%d...", 
			 profclk_us(drv->byte_time), profclk_us(drv->byte_time_max));
}

void capture_idletime_set(unsigned int bits)
{
	struct capture_drv * drv = &uart2_capture_drv;
	unsigned int rate;

	drv->idle_bits = bits;
	rate = drv->baudrate;
	if (bits == 0)
		drv->byte_time_max = BITS_TO_TICKS(21, rate * 2);
	else
		drv->byte_time_max = BITS_TO_TICKS(10 + drv->idle_bits, rate);

	DCC_LOG2(LOG_TRACE, "byte_time=%d max=%d...", 
			 profclk_us(drv->byte_time), profclk_us(drv->byte_time_max));
}

void capture_init(void)
{
	struct capture_drv * drv = &uart2_capture_drv;
	struct stm32_usart * uart = STM32_USART2;
	uint32_t clk;

	DCC_LOG2(LOG_TRACE, "drv=%p uart=%p...", drv, uart);

	clk = profclk_get();
	drv->rx_fifo.head = drv->rx_fifo.tail = 0;
	drv->rx_fifo.seq = 0;
	drv->rx_fifo.buf[0].clk = clk;
	drv->rx_fifo.buf[0].cnt = 0;
	drv->rx_fifo.buf[0].seq = 0;
	drv->rx_fifo.clk = clk;
#if SERIAL_STATS_ENABLE
	drv->err_cnt = 0;
	drv->ore_cnt = 0;
	drv->fe_cnt = 0;
#endif
	drv->idle_bits = 2;

	/* clock enable */
	stm32_clk_enable(STM32_RCC, STM32_CLK_USART2);

	/* UART --------------------------------------------------------------- */

	/* Disable UART */
	uart->cr1 = 0;
	/* Clear pending TC and BRK interrupts */
	uart->sr = 0;
	uart->gtpr = 1;
	capture_baudrate_set(SERIAL_BAUDRATE);
	/* configure the UART */
	uart->cr3 = USART_ONEBIT;
	/* Configure 8N1 */
	uart->cr2 = USART_STOP_1;
	/* enable UART, RX and IDLE interrupts */
	uart->cr1 = USART_UE | USART_RXNEIE;
	/* Errors interrupt */
	uart->cr3 |= USART_EIE;

	/* configure interrupts */
	cm3_irq_pri_set(STM32_IRQ_USART2, IRQ_PRIORITY_HIGHEST);
	/* enable interrupts */
	cm3_irq_enable(STM32_IRQ_USART2);
}

