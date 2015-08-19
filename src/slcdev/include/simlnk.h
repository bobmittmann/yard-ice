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
	SIMLNK_RPC_RETURN    = 0,
	SIMLNK_RPC_SUSPEND   = 1,
	SIMLNK_RPC_RESUME    = 2,
	SIMLNK_RPC_REBOOT    = 3,

	SIMLNK_RPC_MEM_LOCK   = 4,
	SIMLNK_RPC_MEM_UNLOCK = 5,
	SIMLNK_RPC_MEM_ERASE  = 6,
	SIMLNK_RPC_MEM_READ   = 7,
	SIMLNK_RPC_MEM_WRITE  = 8,
	SIMLNK_RPC_MEM_SEEK   = 9,
	
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

#define SIMRPC_BCAST 0xff


#ifdef __cplusplus
extern "C" {
#endif

struct simlnk * simlnk_alloc(void);

int simlnk_init(struct simlnk * lnk, const char * name, 
				unsigned int addr, struct serial_dev * dev);

int simlnk_send(struct simlnk * lnk, const void * buf, unsigned int cnt);

int simlnk_recv(struct simlnk * lnk, void * buf, unsigned int max);

/* services */
void simrpc_mem_lock_svc(uint32_t opc, uint32_t * data, unsigned int cnt);
void simrpc_mem_unlock_svc(uint32_t opc, uint32_t * data, unsigned int cnt);
void simrpc_mem_erase_svc(uint32_t opc, uint32_t * data, unsigned int cnt);
void simrpc_mem_read_svc(uint32_t opc, uint32_t * data, unsigned int cnt);
void simrpc_mem_write_svc(uint32_t opc, uint32_t * data, unsigned int cnt);
void simrpc_mem_seek_svc(uint32_t opc, uint32_t * data, unsigned int cnt);

void simrpc_init(void);

void simrpc_mem_lock(unsigned int daddr, uint32_t base, unsigned int size);
void simrpc_mem_unlock(unsigned int daddr, uint32_t base, unsigned int size);
void simrpc_mem_erase(unsigned int daddr, uint32_t base, unsigned int size);

void simrpc_mem_read(unsigned int daddr, uint32_t * data, unsigned int cnt);
void simrpc_mem_write(unsigned int daddr, uint32_t * data, unsigned int cnt);

void simrpc_mem_seek(unsigned int daddr, uint32_t base);

#ifdef __cplusplus
}
#endif

#endif /* __SIMLNK_H__ */

