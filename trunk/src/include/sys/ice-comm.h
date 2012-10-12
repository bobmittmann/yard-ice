/* 
 * File:    sys/ice_comm.h
 * Author:  Robinson Mittmann (bobmittmann@gmail.com)
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

