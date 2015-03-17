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

#ifndef SERIAL_RX_BUF_LEN
#define SERIAL_RX_BUF_LEN 64
#endif

#ifndef SERIAL_ENABLE_DMA
#define SERIAL_ENABLE_DMA 0
#endif

#ifndef SERIAL_ENABLE_TX_MUTEX
#define SERIAL_ENABLE_TX_MUTEX 1
#endif

#ifndef SERIAL_TX_FIFO_LEN
#define SERIAL_TX_FIFO_LEN  512
#endif

#ifndef SERIAL_RX_FIFO_LEN
#define SERIAL_RX_FIFO_LEN  16
#endif

#ifndef SERIAL_IRQ_PRIORITY 
#define SERIAL_IRQ_PRIORITY IRQ_PRIORITY_REGULAR
#endif

#if (THINKOS_FLAG_MAX == 0) || (THINKOS_ENABLE_FLAG_ALLOC == 0)
#error "THINKOS_FLAG_MAX or THINKOS_ENABLE_FLAG_ALLOC not set!"
#endif

#if (SERIAL_ENABLE_TX_MUTEX)
 #if (THINKOS_MUTEX_MAX == 0) && (THINKOS_ENABLE_MUTEX_ALLOC == 0)
 #error "THINKOS_MUTEX_MAX or THINKOS_ENABLE_MUTEX_ALLOC not set!"
 #endif
#endif

struct stm32f_serial_dev {
	struct stm32_usart * uart;
	uint8_t tx_flag;
	uint8_t rx_flag;
#if SERIAL_ENABLE_TX_MUTEX
	uint8_t tx_mutex;
#endif

#if SERIAL_ENABLE_DMA
	struct stm32f_dma * dma;
	struct {
		int irq;
		struct stm32f_dma_stream * s;
		int flag;
	} tx;
	struct {
		int irq;
		struct stm32f_dma_stream * s;
		uint8_t buf[2][SERIAL_RX_BUF_LEN];
		uint32_t idx;
		int flag;
	} rx;
#else
	uint32_t * txie;
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
#endif
};

#ifdef __cplusplus
extern "C" {
#endif

int stm32f_serial_init(struct stm32f_serial_dev * dev, 
					   unsigned int baudrate, unsigned int flags);

int stm32f_serial_read(struct stm32f_serial_dev * dev, void * buf, 
					   unsigned int len, unsigned int tmo);

int stm32f_serial_write(struct stm32f_serial_dev * dev, const void * buf,
						unsigned int len);

int stm32f_serial_flush(struct stm32f_serial_dev * dev);

int stm32f_serial_close(struct stm32f_serial_dev * dev);

#ifdef __cplusplus
}
#endif

#endif /* __STM32_SERIAL_I_H__ */

