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

#ifndef __SIMRPC_H__
#define __SIMRPC_H__

#include <stdlib.h> 
#include <stdbool.h> 
#include <stdint.h> 

enum simrpc_opc {
	SIMRPC_SIGNAL = 0,
	SIMRPC_SUSPEND   = 1,
	SIMRPC_RESUME    = 2,
	SIMRPC_REBOOT    = 3,

	SIMRPC_MEM_LOCK   = 4,
	SIMRPC_MEM_UNLOCK = 5,
	SIMRPC_MEM_ERASE  = 6,
	SIMRPC_MEM_READ   = 7,
	SIMRPC_MEM_WRITE  = 8,
	SIMRPC_MEM_SEEK   = 9,
	
	SIMRPC_TRACE,
	SIMRPC_RUN_TEST,
	SIMRPC_RUN_APP,

	SIMRPC_FILE_OPEN,
	SIMRPC_FILE_CREATE,
	SIMRPC_FILE_CLOSE,
	SIMRPC_FILE_READ,
	SIMRPC_FILE_WRITE,
	SIMRPC_FILE_SEEK,

	SIMRPC_DIR_OPEN,
	SIMRPC_DIR_CLOSE,
	SIMRPC_DIR_READ,
};

struct simrpc_hdr {
	uint8_t dst;
	uint8_t src;
	uint8_t seq;
	uint8_t opc;
	uint32_t data[];
};

#define SIMRPC_BCAST 0xff

#define SIMRPC_DST(INSN) ((INSN) & 0xff)
#define SIMRPC_SDC(INSN) (((INSN) >> 8) & 0xff)
#define SIMRPC_SEQ(INSN) (((INSN) >> 16) & 0xff)
#define SIMRPC_OPC(INSN) (((INSN) >> 24) & 0xff)

#define SIMRPC_RET_BIT (1 << 31)
#define SIMRPC_RESPONSE_BIT (1 << 31)

#define SIMRPC_PDU_MAX 500

#define SIMRPC_ADDR_SWAP(OPC) (((OPC) & 0xffff0000) | \
							   (((OPC) & 0x0000ff00) >> 8) | \
							   (((OPC) & 0x000000ff) << 8))

#ifdef __cplusplus
extern "C" {
#endif

/* services */
void simrpc_mem_lock_svc(uint32_t opc, uint32_t * data, unsigned int cnt);
void simrpc_mem_unlock_svc(uint32_t opc, uint32_t * data, unsigned int cnt);
void simrpc_mem_erase_svc(uint32_t opc, uint32_t * data, unsigned int cnt);
void simrpc_mem_read_svc(uint32_t opc, uint32_t * data, unsigned int cnt);
void simrpc_mem_write_svc(uint32_t opc, uint32_t * data, unsigned int cnt);
void simrpc_mem_seek_svc(uint32_t opc, uint32_t * data, unsigned int cnt);

void simrpc_init(void);

int simrpc_mem_lock(unsigned int daddr, uint32_t base, unsigned int size);
int simrpc_mem_unlock(unsigned int daddr, uint32_t base, unsigned int size);
int simrpc_mem_erase(unsigned int daddr, uint32_t base, unsigned int size);

int simrpc_mem_read(unsigned int daddr, uint32_t * data, unsigned int cnt);
int simrpc_mem_write(unsigned int daddr, uint32_t * data, unsigned int cnt);

int simrpc_mem_seek(unsigned int daddr, uint32_t base);

#ifdef __cplusplus
}
#endif

#endif /* __SIMRPC_H__ */


