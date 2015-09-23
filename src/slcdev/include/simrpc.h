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

enum simrpc_err {
	SIMRPC_EBADF     = -0x7ffffff5,
	SIMRPC_EBUSY     = -0x7ffffff6,
	SIMRPC_ENOENT    = -0x7ffffff7,
	SIMRPC_ENOSYS    = -0x7ffffff8,
	SIMRPC_EINVAL    = -0x7ffffff9,
	SIMRPC_EROUTE    = -0x7ffffffa,
	SIMRPC_EDRIVER   = -0x7ffffffb,
	SIMRPC_ESYSTEM   = -0x7ffffffc,
	SIMRPC_ELINK     = -0x7ffffffd,
	SIMRPC_EPROTOCOL = -0x7ffffffe,
	SIMRPC_ETIMEDOUT = -0x7fffffff
};

struct simrpc_pcb;

#define SIMRPC_BCAST 0xff

#ifdef __cplusplus
extern "C" {
#endif

void simrpc_init(void);

struct simrpc_pcb * simrpc_open(unsigned int daddr);

int simrpc_close(struct simrpc_pcb * sp);

int simrpc_mem_lock(struct simrpc_pcb * sp, uint32_t base, 
					unsigned int size);
int simrpc_mem_unlock(struct simrpc_pcb * sp, uint32_t base, 
					  unsigned int size);

int simrpc_mem_erase(struct simrpc_pcb * sp, 
					 uint32_t offs, unsigned int size);

int simrpc_mem_read(struct simrpc_pcb * sp, 
					void * data, unsigned int cnt);

int simrpc_mem_write(struct simrpc_pcb * sp, 
					 const void * data, unsigned int cnt);

int simrpc_mem_seek(struct simrpc_pcb * sp, 
					uint32_t offs);

int simrpc_mem_crc32(struct simrpc_pcb * sp, uint32_t offs, 
					 unsigned int size, uint32_t * crc);

int simrpc_suspend(struct simrpc_pcb * sp);
int simrpc_resume(struct simrpc_pcb * sp);
int simrpc_reboot(struct simrpc_pcb * sp);
int simrpc_exec(struct simrpc_pcb * sp, uint32_t key);

int simrpc_file_open(struct simrpc_pcb * sp, const char * path);

int simrpc_file_create(struct simrpc_pcb * sp, const char * path);

int simrpc_file_unlink(struct simrpc_pcb * sp, const char * path);

int simrpc_file_close(struct simrpc_pcb * sp);

int simrpc_file_read(struct simrpc_pcb * sp, void * data, unsigned int cnt);

int simrpc_file_write(struct simrpc_pcb * sp,
					 const void * data, unsigned int cnt);

int simrpc_file_seek(struct simrpc_pcb * sp, uint32_t offs);

int simrpc_file_crc32(struct simrpc_pcb * sp, const char * path, uint32_t * crc);

int simrpc_file_crc16(struct simrpc_pcb * sp, const char * path, uint32_t * crc);

struct simlnk * simrpc_route(unsigned int daddr);

#ifdef __cplusplus
}
#endif

#endif /* __SIMRPC_H__ */


