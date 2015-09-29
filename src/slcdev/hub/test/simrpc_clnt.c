/* 
 * Copyright(C) 2015 Robinson Mittmann. All Rights Reserved.
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
 * @file simlnk.c
 * @brief MS/TP Link Layer
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */

//#include "simlnk-i.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/param.h>
#include <sys/serial.h>
#include <sys/dcclog.h>

#include <thinkos.h>

#include "io.h"
#include "simlnk.h"
#include "simrpc.h"
#include "simrpc_svc.h"

#undef DEBUG
#undef TRACE_LEVEL
#define TRACE_LEVEL TRACE_LVL_WARN
#include <trace.h>

int simrpc_suspend(struct simrpc_pcb * sp)
{
	return simlnk_rpc(sp, SIMRPC_SUSPEND, NULL, 0, NULL, 0);
}

int simrpc_resume(struct simrpc_pcb * sp)
{
	return simlnk_rpc(sp, SIMRPC_RESUME, NULL, 0, NULL, 0);
}

int simrpc_reboot(struct simrpc_pcb * sp)
{
	return simlnk_rpc(sp, SIMRPC_REBOOT, NULL, 0, NULL, 0);
}

int simrpc_exec(struct simrpc_pcb * sp, uint32_t key)
{
	uint32_t req[1];

	req[0] = key;

	return simlnk_rpc(sp, SIMRPC_EXEC, req, 4, NULL, 0);
}

int simrpc_kernelinfo_get(struct simrpc_pcb * sp,
		struct simrpc_kernelinfo * inf)
{
	return simlnk_rpc(sp, SIMRPC_KERNELINFO, NULL, 0, inf,
			sizeof(struct simrpc_kernelinfo));
}

int simrpc_appinfo_get(struct simrpc_pcb * sp, struct simrpc_appinfo * inf)
{
	return simlnk_rpc(sp, SIMRPC_APPINFO, NULL, 0, inf,
			sizeof(struct simrpc_appinfo));
}

int simrpc_cfginfo_get(struct simrpc_pcb * sp, struct simrpc_cfginfo * inf)
{
	return simlnk_rpc(sp, SIMRPC_CFGINFO, NULL, 0, inf,
			sizeof(struct simrpc_cfginfo));
}

int simrpc_cfg_compile(struct simrpc_pcb * sp, char * resp, unsigned int max)
{
	return simlnk_rpc(sp, SIMRPC_CFGCOMPILE, NULL, 0,  resp, max);
}

int simrpc_mod_opt_get(struct simrpc_pcb * sp, unsigned int addr,
		unsigned int count, uint32_t opt[])
{
	struct simrpc_attr_req req;

	req.dev_type = 1;
	req.attr_offs = 0;
	req.base_addr = addr;
	req.count = count;

	return simlnk_rpc(sp, SIMRPC_ATTR_GET, &req, sizeof(req),
			opt, count * sizeof(uint32_t));
}

int simrpc_sens_opt_get(struct simrpc_pcb * sp, unsigned int addr,
		unsigned int count, uint32_t opt[])
{
	struct simrpc_attr_req req;

	req.dev_type = 0;
	req.attr_offs = 0;
	req.base_addr = addr;
	req.count = count;

	return simlnk_rpc(sp, SIMRPC_ATTR_GET, &req, sizeof(req),
			opt, count * sizeof(uint32_t));
}

int simrpc_attr_get(struct simrpc_pcb * sp,
		struct simrpc_attr_req * req, uint32_t val[])
{
	unsigned int count = req->count;

	return simlnk_rpc(sp, SIMRPC_ATTR_GET, req, sizeof(req),
			val, count * sizeof(uint32_t));
}

int simrpc_dbinfo_get(struct simrpc_pcb * sp, struct simrpc_dbinfo * inf)
{
	return simlnk_rpc(sp, SIMRPC_DBINFO, NULL, 0, inf,
			sizeof(struct simrpc_dbinfo));
}

int simrpc_db_compile(struct simrpc_pcb * sp, char * resp, unsigned int max)
{
	return simlnk_rpc(sp, SIMRPC_DBCOMPILE, NULL, 0,  resp, max);
}

int simrpc_shellexec(struct simrpc_pcb * sp, const char * cmd,
		char * resp, unsigned int max)
{
	return simlnk_rpc(sp, SIMRPC_SHELLEXEC, cmd, strlen(cmd), resp, max);
}

int simrpc_jsexec(struct simrpc_pcb * sp, const char * cmd,
		char * resp, unsigned int max)
{
	return simlnk_rpc(sp, SIMRPC_JSEXEC, cmd, strlen(cmd), resp, max);
}


/* ---------------------------------------------------------------------------
 * Memory
 * ---------------------------------------------------------------------------
 */

int simrpc_mem_lock(struct simrpc_pcb * sp, uint32_t base, unsigned int size)
{
	uint32_t req[2];

	req[0] = base;
	req[1] = size;

	return simlnk_rpc(sp, SIMRPC_MEM_LOCK, req, 8, NULL, 0);
}

int simrpc_mem_unlock(struct simrpc_pcb * sp, uint32_t base, unsigned int size)
{
	uint32_t req[2];

	req[0] = base;
	req[1] = size;

	return simlnk_rpc(sp, SIMRPC_MEM_UNLOCK, req, 8, NULL, 0);
}

int simrpc_mem_erase(struct simrpc_pcb * sp, uint32_t offs, unsigned int size)
{
	uint32_t req[2];

	req[0] = offs;
	req[1] = size;

	return simlnk_rpc(sp, SIMRPC_MEM_ERASE, req, 8, NULL, 0);
}

int simrpc_mem_read(struct simrpc_pcb * sp, void * data, unsigned int cnt)
{
	uint32_t req[1];
	int ret;

	req[0] = cnt;
	ret = simlnk_rpc(sp, SIMRPC_MEM_READ, req, 4, data, cnt);

	/* sanity check */
	if (ret <= cnt)
		return ret;

	return SIMRPC_EBADMSG;

}

int simrpc_mem_write(struct simrpc_pcb * sp, 
					 const void * data, unsigned int cnt)
{
	return simlnk_rpc(sp, SIMRPC_MEM_WRITE, data, cnt, NULL, 0);
}

int simrpc_mem_seek(struct simrpc_pcb * sp, uint32_t offs)
{
	uint32_t req[1];

	req[0] = offs;

	return simlnk_rpc(sp, SIMRPC_MEM_SEEK, req, 4, NULL, 0);
}

int simrpc_mem_crc32(struct simrpc_pcb * sp, uint32_t offs, 
					 unsigned int size, uint32_t * crc)
{
	uint32_t req[2];

	req[0] = offs;
	req[1] = size;

	return simlnk_rpc(sp, SIMRPC_MEM_CRC32, req, 8, crc, 4);
}

/* ---------------------------------------------------------------------------
 * File
 * ---------------------------------------------------------------------------
 */

int simrpc_file_open(struct simrpc_pcb * sp, const char * path)
{
	uint32_t req[64];
	int len;

	len = strlen(path) + 1;
	memcpy(req, path, len);

	return simlnk_rpc(sp, SIMRPC_FILE_OPEN, req, len, NULL, 0);
}

int simrpc_file_create(struct simrpc_pcb * sp, const char * path)
{
	uint32_t req[64];
	int len;

	len = strlen(path) + 1;
	memcpy(req, path, len);

	return simlnk_rpc(sp, SIMRPC_FILE_CREATE, req, len, NULL, 0);
}

int simrpc_file_unlink(struct simrpc_pcb * sp, const char * path)
{
	uint32_t req[64];
	int len;

	len = strlen(path) + 1;
	memcpy(req, path, len);

	return simlnk_rpc(sp, SIMRPC_FILE_UNLINK, req, len, NULL, 0);
}

int simrpc_file_close(struct simrpc_pcb * sp)
{
	return simlnk_rpc(sp, SIMRPC_FILE_CLOSE, NULL, 0, NULL, 0);
}

int simrpc_file_read(struct simrpc_pcb * sp, void * data, unsigned int cnt)
{
	uint32_t req[1];
	int ret;

	req[0] = cnt;
	ret = simlnk_rpc(sp, SIMRPC_FILE_READ, req, 4, data, cnt);

	/* sanity check */
	if (ret <= cnt)
		return ret;

	return SIMRPC_EBADMSG;
}

int simrpc_file_write(struct simrpc_pcb * sp,
					 const void * data, unsigned int cnt)
{
	return simlnk_rpc(sp, SIMRPC_FILE_WRITE, data, cnt, NULL, 0);
}

int simrpc_file_seek(struct simrpc_pcb * sp, uint32_t offs)
{
	uint32_t req[1];

	req[0] = offs;

	return simlnk_rpc(sp, SIMRPC_FILE_SEEK, req, 4, NULL, 0);
}

int simrpc_file_crc32(struct simrpc_pcb * sp, const char * path, uint32_t * crc)
{
	uint32_t req[64];
	int len;

	len = strlen(path) + 1;
	memcpy(req, path, len);

	return simlnk_rpc(sp, SIMRPC_FILE_CRC32, req, len, crc, 4);
}

int simrpc_file_crc16(struct simrpc_pcb * sp, const char * path,
		uint32_t * crc)
{
	uint32_t req[64];
	int len;

	len = strlen(path) + 1;
	memcpy(req, path, len);

	return simlnk_rpc(sp, SIMRPC_FILE_CRC16, req, len, crc, 4);
}

/* ---------------------------------------------------------------------------
 * ---------------------------------------------------------------------------
 */

