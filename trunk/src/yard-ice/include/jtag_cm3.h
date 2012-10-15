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
 * @file .h
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */



#ifndef __JTAG_CM3_H__
#define __JTAG_CM3_H__

#include "jtag.h"

/*
 * ARM ICE Watchpoint struct
 */
typedef struct {
	uint32_t addr_value;
	uint32_t addr_mask;
	uint32_t data_value;
	uint32_t data_mask;
	uint16_t ctrl_value;
	uint16_t ctrl_mask;
} jtag_arm_wp_t;

#ifndef JTAG_ARM_DCC_BUF_LEN 
#define JTAG_ARM_DCC_BUF_LEN 512
#endif

typedef struct {
	int sem; /* semaphore */
	volatile uint32_t tail; /* fifo tail */
	volatile uint32_t head; /* fifo head */
	uint32_t buf[JTAG_ARM_DCC_BUF_LEN]; /* fifo buffer */
} jtag_arm_dcc_t;

typedef struct {
	volatile int status;
	volatile int enabled;
	uint32_t count;
} jtag_arm_poll_t;

extern jtag_arm_dcc_t arm_dcc;

#ifdef __cplusplus
extern "C" {
#endif


/***************************************************************************
 *
 * Cortex M3 JTAG operations
 *
 ***************************************************************************/

int jtag_cm3_apacc_rd(jtag_tap_t * tap);

int jtag_cm3_apacc_wr(jtag_tap_t * tap);

int jtag_cm3_dpacc_rd(jtag_tap_t * tap);

int jtag_cm3_dpacc_wr(jtag_tap_t * tap);

int jtag_cm3_abort_rd(jtag_tap_t * tap);


#ifdef __cplusplus
	}
#endif

#endif /* __JTAG_CM3_H__ */

