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
 * @file ice-comm.h
 * @brief YARD-ICE libice-comm
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __ICE_COMM_H__
#define __ICE_COMM_H__

#include <stdint.h>
#include <sys/file.h>

#define DEV_SYNC 0x43
#define DEV_CONNECTED 0x65

#define DBG_CONNECTED 1
#define DBG_SYNC 2

struct ice_comm_blk {
	/* device write only */
	volatile uint16_t tx_head;
	volatile uint8_t rx_tail;
	volatile uint8_t dev;
	/* host write only */
	volatile uint16_t tx_tail;
	volatile uint8_t rx_head;
	volatile uint8_t dbg; /* debugger attached flag */
	/* device's transmitt buffer */
	union {
		uint8_t u8[64];
		uint32_t u32[16];
	} tx_buf;
	/* device's receive buffer */
	union {
		uint8_t u8[8];
		uint32_t u32[2];
	} rx_buf;
};

extern struct ice_comm_blk ice_comm_blk;

extern const struct file ice_comm_file;

#define ICE_COMM_BLK (*(struct ice_comm_blk **)(0x08000000 + 4 * 8))

#ifdef __cplusplus
extern "C" {
#endif

void ice_comm_sync(void);

void ice_comm_connect(void);

void ice_comm_w32(uint32_t data);

struct file * ice_comm_open(void);

#ifdef __cplusplus
}
#endif

#endif /* __ICE_COMM_H__ */

