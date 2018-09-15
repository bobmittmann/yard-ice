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
 * @file ice_comm.h
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */



#ifndef __ICE_COMM_H__
#define __ICE_COMM_H__

#include <stdint.h>

#ifndef ICE_COMM_RX_BUF_LEN 
#define ICE_COMM_RX_BUF_LEN 512
#endif

#ifndef ICE_COMM_TX_BUF_LEN 
#define ICE_COMM_TX_BUF_LEN 16
#endif

typedef struct {
	volatile uint8_t open; /* status */
	int8_t reserved; /* unused */
	int8_t rx_sem; /* semaphore */
	int8_t tx_sem; /* semaphore */
	/* RX queue (from device) */
	struct {
		volatile uint32_t head; /* fifo head */
		volatile uint32_t tail; /* fifo tail */
		uint32_t buf[ICE_COMM_RX_BUF_LEN]; /* fifo buffer */
	} rx;
	/* TX queue (to device) */
	struct {
		volatile uint32_t tail; /* fifo tail */
		volatile uint32_t head; /* fifo head */
		uint32_t buf[ICE_COMM_TX_BUF_LEN]; /* fifo buffer */
	} tx;
} ice_comm_t;

#ifdef __cplusplus
extern "C" {
#endif

int ice_comm_open(ice_comm_t * comm);

int ice_comm_close(ice_comm_t * comm);

int ice_comm_read(ice_comm_t * comm, void * buf, 
					  int len, int tmo);

int ice_comm_write(ice_comm_t * comm, const void * buf, 
					   int len, int tmo);

int ice_comm_flush(ice_comm_t * comm);

int ice_comm_init(ice_comm_t * comm);

int ice_comm_done(ice_comm_t * comm);


int comm_tcp_start(ice_comm_t * comm);

#ifdef __cplusplus
	}
#endif

#endif /* __ICE_COMM_H__ */

