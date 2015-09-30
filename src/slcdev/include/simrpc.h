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
#include "stats.h"

enum simrpc_err {
	SIMRPC_EBADMSG   = -0x7ffffff4,
	SIMRPC_EBADF     = -0x7ffffff5,
	SIMRPC_EBUSY     = -0x7ffffff6,
	SIMRPC_ENOENT    = -0x7ffffff7,
	SIMRPC_ENOSYS    = -0x7ffffff8,
	SIMRPC_EINVAL    = -0x7ffffff9,
	SIMRPC_EROUTE    = -0x7ffffffa,
	SIMRPC_EDRIVER   = -0x7ffffffb,
	SIMRPC_ESYSTEM   = -0x7ffffffc,
	SIMRPC_ELINK     = -0x7ffffffd,
	SIMRPC_EPROTO    = -0x7ffffffe,
	SIMRPC_ETIMEDOUT = -0x7fffffff
};

#define SIMRPC_DATA_MAX  (512)

struct simrpc_kernelinfo {
	uint32_t ticks;
	struct {
		uint8_t major;
		uint8_t minor;
		uint16_t build;
		uint32_t timestamp;
	} version;
};

struct simrpc_appinfo {
	uint32_t uptime;
	struct {
		uint8_t major;
		uint8_t minor;
		uint16_t build;
		uint32_t timestamp;
	} version;
};

#define CFGINFO_TAG_MAX 15
#define CFGINFO_AUTHOR_MAX 15
#define CFGINFO_DESC_MAX 91

struct simrpc_cfginfo {
	char tag[CFGINFO_TAG_MAX + 1];
	char author[CFGINFO_AUTHOR_MAX + 1];
	char desc[CFGINFO_DESC_MAX + 1];
	struct {
		uint8_t major;
		uint8_t minor;
		uint16_t build;
	} version;
};

#define DBINFO_DESC_MAX 59

struct simrpc_dbinfo {
	char desc[DBINFO_DESC_MAX + 1];
	struct {
		uint8_t major;
		uint8_t minor;
		uint16_t build;
	} version;
};

struct simrpc_attr_req {
	uint8_t dev_type;
	uint8_t attr_offs;
	uint8_t base_addr;
	uint8_t count;
};

struct simrpc_stats {
	uint32_t uptime;
	bool db_ok;
	bool cfg_ok;
    struct slcdev_stats dev;
};

#define SIMRPC_EXEC_KEY(A, B, C, D) (A | (B << 8) | (C << 16)  | (D << 24))

struct simrpc_pcb;

#define SIMRPC_MK_ADDR(HUB, PORT) (((HUB) << 3) | (PORT))

#define SIMRPC_ADDR_RCAST(HUB) SIMRPC_MK_ADDR((HUB), 7)
#define SIMRPC_ADDR_BCAST      SIMRPC_MK_ADDR(0x1f, 7)
#define SIMRPC_ADDR_LCAST      SIMRPC_MK_ADDR(0x10, 7)

#define SIMRPC_ADDR_RHUB(HUB)  SIMRPC_MK_ADDR((HUB), 0)
#define SIMRPC_ADDR_LHUB       SIMRPC_MK_ADDR(0x10, 0)
#define SIMRPC_ADDR_ANY        SIMRPC_MK_ADDR(0x1f, 0)

#ifdef __cplusplus
extern "C" {
#endif

void simrpc_init(void);

struct simrpc_pcb * simrpc_open(unsigned int daddr);

int simrpc_set_timeout(struct simrpc_pcb * sp, unsigned int tmo_ms);

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

int simrpc_stats_get(struct simrpc_pcb * sp, struct simrpc_stats * stats);

int simrpc_kernelinfo_get(struct simrpc_pcb * sp, 
						  struct simrpc_kernelinfo * inf);

int simrpc_appinfo_get(struct simrpc_pcb * sp, struct simrpc_appinfo * inf);

int simrpc_cfginfo_get(struct simrpc_pcb * sp, struct simrpc_cfginfo * inf);

int simrpc_cfg_compile(struct simrpc_pcb * sp, char * resp, unsigned int max);

int simrpc_dbinfo_get(struct simrpc_pcb * sp, struct simrpc_dbinfo * inf);

int simrpc_db_compile(struct simrpc_pcb * sp, char * resp, unsigned int max);

int simrpc_shellexec(struct simrpc_pcb * sp, const char * cmd,
		char * resp, unsigned int max);

int simrpc_jsexec(struct simrpc_pcb * sp, const char * cmd,
		char * resp, unsigned int max);

int simrpc_file_open(struct simrpc_pcb * sp, const char * path);

int simrpc_file_create(struct simrpc_pcb * sp, const char * path);

int simrpc_file_unlink(struct simrpc_pcb * sp, const char * path);

int simrpc_file_close(struct simrpc_pcb * sp);

int simrpc_file_read(struct simrpc_pcb * sp, void * data, unsigned int cnt);

int simrpc_file_write(struct simrpc_pcb * sp,
					 const void * data, unsigned int cnt);

int simrpc_file_seek(struct simrpc_pcb * sp, uint32_t offs);

int simrpc_file_crc32(struct simrpc_pcb * sp, const char * path, 
					  uint32_t * crc);

int simrpc_file_crc16(struct simrpc_pcb * sp, const char * path, 
					  uint32_t * crc);

int simrpc_mod_opt_get(struct simrpc_pcb * sp, unsigned int addr,
		unsigned int count, uint32_t opt[]);

int simrpc_sens_opt_get(struct simrpc_pcb * sp, unsigned int addr,
		unsigned int count, uint32_t opt[]);

int simrpc_attr_get(struct simrpc_pcb * sp,
		struct simrpc_attr_req * req, uint32_t val[]);

struct simlnk * simrpc_route(unsigned int daddr);

int simrpc_stdout_flush(struct simrpc_pcb * sp);

#ifdef __cplusplus
}
#endif

#endif /* __SIMRPC_H__ */


