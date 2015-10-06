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

#ifndef __SIMRPC_SVC_H__
#define __SIMRPC_SVC_H__

#include <stdlib.h> 
#include <stdbool.h> 
#include <stdint.h> 

#include <simrpc.h>

enum simrpc_opc {
	SIMRPC_LINK       = 0,
	SIMRPC_SUSPEND    = 1,
	SIMRPC_RESUME     = 2,
	SIMRPC_EXEC       = 3,
	SIMRPC_REBOOT     = 4,
	SIMRPC_KERNELINFO = 5,
	SIMRPC_MEM_LOCK   = 6,
	SIMRPC_MEM_UNLOCK = 7,
	SIMRPC_MEM_ERASE  = 8,
	SIMRPC_MEM_READ   = 9,
	SIMRPC_MEM_WRITE  = 10,
	SIMRPC_MEM_SEEK   = 11,
	SIMRPC_MEM_CRC32  = 12,

	SIMRPC_TRACE       = 32,
	SIMRPC_FILE_OPEN   = 33,
	SIMRPC_FILE_CREATE = 34,
	SIMRPC_FILE_CLOSE  = 35,
	SIMRPC_FILE_READ   = 36,
	SIMRPC_FILE_WRITE  = 37,
	SIMRPC_FILE_SEEK   = 38,
	SIMRPC_FILE_UNLINK = 39,
	SIMRPC_FILE_CRC32  = 40,
	SIMRPC_FILE_CRC16  = 41,
	SIMRPC_FILE_MD5SUM = 42,

	SIMRPC_DIR_OPEN    = 43,
	SIMRPC_DIR_CLOSE   = 44,
	SIMRPC_DIR_READ    = 45,

	SIMRPC_APPINFO     = 46,
	SIMRPC_DBCOMPILE   = 47,
	SIMRPC_DBINFO      = 48,
	SIMRPC_CFGCOMPILE  = 49,
	SIMRPC_CFGINFO     = 50,
	SIMRPC_SHELLEXEC   = 51,
	SIMRPC_JSEXEC      = 52,

	SIMRPC_ATTR_GET     = 53,
	SIMRPC_STATS        = 54,

	SIMRPC_STDOUT_DATA  = 128,
	SIMRPC_STDOUT_FLUSH = 129,

	SIMRPC_FAULT        = 248,
	SIMRPC_SIGNAL       = 249,
	SIMRPC_BUSY         = 250,
	SIMRPC_PEND         = 251,
	SIMRPC_FALSE        = 252,
	SIMRPC_TRUE         = 253,
	SIMRPC_OK           = 254,
	SIMRPC_ERR          = 255
};

struct simrpc_link {
	uint8_t id;
	uint8_t pdu[3];
};

struct simrpc_hdr {
	uint8_t dst;
	uint8_t src;
	uint8_t seq;
	uint8_t insn;
	uint32_t data[];
};

#define SIMRPC_PDU_MAX   (4 + SIMRPC_DATA_MAX)

#define SIMRPC_DST(OPC) ((OPC) & 0xff)
#define SIMRPC_SRC(OPC) (((OPC) >> 8) & 0xff)
#define SIMRPC_SEQ(OPC) (((OPC) >> 16) & 0xff)
#define SIMRPC_INSN(OPC) (((OPC) >> 24) & 0xff)

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


#ifdef __cplusplus
extern "C" {
#endif

/* services */
void simrpc_mem_lock_svc(uint32_t hdr, uint32_t * data, unsigned int cnt);
void simrpc_mem_unlock_svc(uint32_t hdr, uint32_t * data, unsigned int cnt);
void simrpc_mem_erase_svc(uint32_t hdr, uint32_t * data, unsigned int cnt);
void simrpc_mem_read_svc(uint32_t hdr, uint32_t * data, unsigned int cnt);
void simrpc_mem_write_svc(uint32_t hdr, uint32_t * data, unsigned int cnt);
void simrpc_mem_seek_svc(uint32_t hdr, uint32_t * data, unsigned int cnt);
void simrpc_mem_crc32_svc(uint32_t hdr, uint32_t * data, unsigned int cnt);

void simrpc_suspend_svc(uint32_t hdr, uint32_t * data, unsigned int cnt);
void simrpc_resume_svc(uint32_t hdr, uint32_t * data, unsigned int cnt);
void simrpc_reboot_svc(uint32_t hdr, uint32_t * data, unsigned int cnt);
void simrpc_exec_svc(uint32_t hdr, uint32_t * data, unsigned int cnt);
void simrpc_kernelinfo_svc(uint32_t hdr, uint32_t * data, unsigned int cnt);

void simrpc_file_open_svc(uint32_t hdr, uint32_t * data, unsigned int cnt);
void simrpc_file_create_svc(uint32_t hdr, uint32_t * data, unsigned int cnt);
void simrpc_file_unlink_svc(uint32_t hdr, uint32_t * data, unsigned int cnt);
void simrpc_file_read_svc(uint32_t hdr, uint32_t * data, unsigned int cnt);
void simrpc_file_write_svc(uint32_t hdr, uint32_t * data, unsigned int cnt);
void simrpc_file_seek_svc(uint32_t hdr, uint32_t * data, unsigned int cnt);
void simrpc_file_close_svc(uint32_t hdr, uint32_t * data, unsigned int cnt);
void simrpc_file_crc16_svc(uint32_t hdr, uint32_t * data, unsigned int cnt);

void simrpc_appinfo_svc(uint32_t hdr, uint32_t * data, unsigned int cnt);
void simrpc_cfginfo_svc(uint32_t hdr, uint32_t * data, unsigned int cnt);
void simrpc_dbinfo_svc(uint32_t hdr, uint32_t * data, unsigned int cnt);
void simrpc_cfgcompile_svc(uint32_t hdr, uint32_t * data, unsigned int cnt);
void simrpc_dbcompile_svc(uint32_t hdr, uint32_t * data, unsigned int cnt);
void simrpc_shellexec_svc(uint32_t hdr, uint32_t * data, unsigned int cnt);
void simrpc_jsexec_svc(uint32_t hdr, uint32_t * data, unsigned int cnt);

void simrpc_svc_init(void);

int simrpc_send(uint32_t opc, void * data, unsigned int cnt);
int simrpc_send_int(uint32_t opc, int val);
int simrpc_send_opc(uint32_t opc);

void simrpc_attr_get_svc(uint32_t hdr, uint32_t * data, unsigned int cnt);

void simrpc_stdout_flush_svc(uint32_t hdr, uint32_t * data, unsigned int cnt);

void simrpc_stats_get_svc(uint32_t hdr, uint32_t * data, unsigned int cnt);

#ifdef __cplusplus
}
#endif

#endif /* __SIMRPC_SVC_H__ */


