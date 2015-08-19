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
 * @file simlnk.h
 * @brief YARD-ICE libhexdump
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __SIMLNK_H__
#define __SIMLNK_H__

#include <stdlib.h> 
#include <stdbool.h> 
#include <stdint.h> 
#include <sys/serial.h> 

enum simlnk_opc {
	SIMLNK_OPC_RETURN    = 0,
	SIMLNK_OPC_SUSPEND   = 1,
	SIMLNK_OPC_RESUME    = 2,
	SIMLNK_OPC_REBOOT    = 3,
	SIMLNK_OPC_MEM_SEEK  = 4,
	SIMLNK_OPC_MEM_ERASE = 5,
	SIMLNK_OPC_MEM_READ  = 6,
	SIMLNK_OPC_MEM_WRITE = 7,
	
	SIMLNK_OPC_TRACE,
	SIMLNK_OPC_RUN_TEST,
	SIMLNK_OPC_RUN_APP,

	SIMLNK_OPC_FILE_OPEN,
	SIMLNK_OPC_FILE_CREATE,
	SIMLNK_OPC_FILE_CLOSE,
	SIMLNK_OPC_FILE_READ,
	SIMLNK_OPC_FILE_WRITE,
	SIMLNK_OPC_FILE_SEEK,

	SIMLNK_OPC_DIR_OPEN,
	SIMLNK_OPC_DIR_CLOSE,
	SIMLNK_OPC_DIR_READ,
};

struct simlnk_op {
	uint8_t dst;
	uint8_t src;
	uint8_t seq;
	uint8_t opc;
	uint32_t data[];
};

#ifdef __cplusplus
extern "C" {
#endif

struct simlnk * simlnk_alloc(void);

int simlnk_init(struct simlnk * lnk, const char * name, 
				unsigned int addr, struct serial_dev * dev);

#ifdef __cplusplus
}
#endif

#endif /* __SIMLNK_H__ */

