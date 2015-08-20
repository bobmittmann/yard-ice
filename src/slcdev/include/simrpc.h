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
	SIMRPC_SIGNAL     = 0,
	SIMRPC_SUSPEND    = 1,
	SIMRPC_RESUME     = 2,
	SIMRPC_EXEC       = 3,
	SIMRPC_REBOOT     = 4,
	SIMRPC_MEM_LOCK   = 5,
	SIMRPC_MEM_UNLOCK = 6,
	SIMRPC_MEM_ERASE  = 7,
	SIMRPC_MEM_READ   = 8,
	SIMRPC_MEM_WRITE  = 9,
	SIMRPC_MEM_SEEK   = 10,
	SIMRPC_MEM_CRC32  = 11,

	SIMRPC_TRACE,
	SIMRPC_FILE_OPEN,
	SIMRPC_FILE_CREATE,
	SIMRPC_FILE_CLOSE,
	SIMRPC_FILE_READ,
	SIMRPC_FILE_WRITE,
	SIMRPC_FILE_SEEK,

	SIMRPC_DIR_OPEN,
	SIMRPC_DIR_CLOSE,
	SIMRPC_DIR_READ,

	SIMRPC_BUSY = 251,
	SIMRPC_PEND = 253,
	SIMRPC_OK   = 254,
	SIMRPC_ERR  = 255
};

enum simrpc_err {
	SIMRPC_EROUTE    = -1000000006,
	SIMRPC_EDRIVER   = -1000000005,
	SIMRPC_ESYSTEM   = -1000000004,
	SIMRPC_ELINK     = -1000000003,
	SIMRPC_EPROTOCOL = -1000000002,
	SIMRPC_ETIMEDOUT = -1000000001
};

struct simrpc_hdr {
	uint8_t dst;
	uint8_t src;
	uint8_t seq;
	uint8_t opc;
	uint32_t data[];
};

#define SIMRPC_BCAST 0xff

#define SIMRPC_DST(OPC) ((OPC) & 0xff)
#define SIMRPC_SRC(OPC) (((OPC) >> 8) & 0xff)
#define SIMRPC_SEQ(OPC) (((OPC) >> 16) & 0xff)
#define SIMRPC_OPC(OPC) (((OPC) >> 24) & 0xff)

#define SIMRPC_PDU_MAX 500

#define SIMRPC_ADDR_SWAP(OPC) (((OPC) & 0xffff0000) | \
							   (((OPC) & 0x0000ff00) >> 8) | \
							   (((OPC) & 0x000000ff) << 8))

#define SIMRPC_REPLY_OK(OPC) ((SIMRPC_OK << 24) | \
							  ((OPC) & 0x00ff0000) | \
							  (((OPC) & 0x0000ff00) >> 8) | \
							  (((OPC) & 0x000000ff) << 8))

#define SIMRPC_REPLY_ERR(OPC) ((SIMRPC_ERR << 24) | \
							  ((OPC) & 0x00ff0000) | \
							  (((OPC) & 0x0000ff00) >> 8) | \
							  (((OPC) & 0x000000ff) << 8))

#define SIMRPC_REPLY_BUSY(OPC) ((SIMRPC_ERR << 24) | \
							  ((OPC) & 0x00ff0000) | \
							  (((OPC) & 0x0000ff00) >> 8) | \
							  (((OPC) & 0x000000ff) << 8))


#define SIMRPC_REPLY_PEND(OPC) ((SIMRPC_PEND << 24) | \
							  ((OPC) & 0x00ff0000) | \
							  (((OPC) & 0x0000ff00) >> 8) | \
							  (((OPC) & 0x000000ff) << 8))

#define SIMRPC_DEF_TMO_MS 100

#define SIMRPC_EXEC_KEY(A, B, C, D) (A | (B << 8) | (C << 16)  | (D << 24))

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

void simrpc_suspend_svc(uint32_t opc, uint32_t * data, unsigned int cnt);
void simrpc_resume_svc(uint32_t opc, uint32_t * data, unsigned int cnt);
void simrpc_reboot_svc(uint32_t opc, uint32_t * data, unsigned int cnt);
void simrpc_exec_svc(uint32_t opc, uint32_t * data, unsigned int cnt);

void simrpc_init(void);

int simrpc_mem_lock(unsigned int daddr, uint32_t base, unsigned int size);
int simrpc_mem_unlock(unsigned int daddr, uint32_t base, unsigned int size);

int simrpc_mem_erase(unsigned int daddr, uint32_t offs, unsigned int size);
int simrpc_mem_read(unsigned int daddr, void * data, unsigned int cnt);
int simrpc_mem_write(unsigned int daddr, const void * data, unsigned int cnt);
int simrpc_mem_seek(unsigned int daddr, uint32_t offs);

int simrpc_suspend(unsigned int daddr);
int simrpc_resume(unsigned int daddr);
int simrpc_reboot(unsigned int daddr);
int simrpc_exec(unsigned int daddr, uint32_t key);


struct simlnk * simrpc_route(unsigned int daddr);

int simrpc_send(uint32_t opc, void * data, unsigned int cnt);
int simrpc_send_int(uint32_t opc, int val);
int simrpc_send_opc(uint32_t opc);

#ifdef __cplusplus
}
#endif

#endif /* __SIMRPC_H__ */


