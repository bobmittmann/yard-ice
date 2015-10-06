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
 * @file stm32-serial-i.h
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


/*****************************************************************************
 * libshell internal (private) header file
 *****************************************************************************/

#ifndef __STM32_SERIAL_I_H__
#define __STM32_SERIAL_I_H__

#ifndef __STM32_SERIAL_I__
#error "Never use <stm32-serial-i.h> directly; include <sys/serial.h> instead."
#endif

#ifdef CONFIG_H
#include "config.h"
#endif

#include <sys/stm32f.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/serial.h>
#include <sys/delay.h>

#include <thinkos.h>
#define __THINKOS_IRQ__
#include <thinkos_irq.h>

#include <sys/dcclog.h>

#ifndef SERIAL_ENABLE_TX_MUTEX
#define SERIAL_ENABLE_TX_MUTEX 0
#endif

#ifndef SERIAL_TX_FIFO_LEN
#define SERIAL_TX_FIFO_LEN  64
#endif

#ifndef SERIAL_RX_FIFO_LEN
#define SERIAL_RX_FIFO_LEN  64
#endif

#ifndef SERIAL_RX_TRIG_LVL
#define SERIAL_RX_TRIG_LVL (SERIAL_RX_FIFO_LEN / 2)
#endif

#ifndef SERIAL_IRQ_PRIORITY 
#define SERIAL_IRQ_PRIORITY IRQ_PRIORITY_REGULAR
#endif

#ifndef SERIAL_ENABLE_STATS
#define SERIAL_ENABLE_STATS 1
#endif

struct stm32f_serial_drv {
	struct stm32_usart * uart;
	uint8_t tx_gate;
	uint8_t rx_gate;
#if SERIAL_ENABLE_TX_MUTEX
	uint8_t tx_mutex;
#endif
	uint16_t rx_trig;
//	uint32_t * txie;
	struct {
		volatile uint32_t head;
		volatile uint32_t tail;
		uint8_t buf[SERIAL_TX_FIFO_LEN];
	} tx_fifo;	
	struct {
		volatile uint32_t head;
		volatile uint32_t tail;
		uint8_t buf[SERIAL_RX_FIFO_LEN];
	} rx_fifo;	
#if SERIAL_ENABLE_STATS
	struct {
		uint32_t rx_drop;
		uint32_t err_ovr;
		uint32_t err_frm;
		uint32_t rx_brk;
		uint32_t rx_idle;
		uint32_t rx_cnt;
	} stats;
#endif
};

struct stm32f_serial_dma_drv {
	struct stm32_usart * uart;
	uint8_t rx_idle;
	uint8_t tx_done;
#if SERIAL_ENABLE_TX_MUTEX
	uint8_t tx_mutex;
#endif
	struct {
		struct stm32_dmactl dmactl;
	} tx;
	struct {
		struct stm32_dmactl dmactl;
		void * buf_ptr;
		unsigned int buf_len;
	} rx;
#if SERIAL_ENABLE_STATS
	struct {
		uint32_t err_ovr;
		uint32_t rx_brk;
		uint32_t rx_idle;
		uint32_t rx_cnt;
		uint32_t rx_dmatc;
		uint32_t rx_dmate;
	} stats;
#endif
};

extern const struct serial_op stm32f_uart_serial_op;

extern const struct serial_op stm32f_uart_serial_dma_op;

#ifdef __cplusplus
extern "C" {
#endif

void stm32f_serial_isr(struct stm32f_serial_drv * drv);

int stm32f_serial_init(struct stm32f_serial_drv * drv, 
					   unsigned int baudrate, unsigned int flags);


void stm32f_serial_dma_isr(struct stm32f_serial_dma_drv * drv);

void stm32f_serial_dma_rx_isr(struct stm32f_serial_dma_drv * drv);

void stm32f_serial_dma_tx_isr(struct stm32f_serial_dma_drv * drv);

int stm32f_serial_dma_init(struct stm32f_serial_dma_drv * drv, 
						   unsigned int baudrate, unsigned int flags,
						   int dma_chan_id);

#ifdef __cplusplus
}
#endif

#endif /* __STM32_SERIAL_I_H__ */

