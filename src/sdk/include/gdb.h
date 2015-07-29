/* 
 * Copyright(c) 2004-2012 BORESTE (www.boreste.com). All Rights Reserved.
 *
 * This file is part of the libmd5.
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
 * @file gdb.h
 * @brief YARD-ICE libgdb
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __GDB_H__
#define __GDB_H__

#include <stdint.h>

#define __THINKOS_DMON__
#include <thinkos_dmon.h>
#include <thinkos.h>

#define SZ_128   7
#define SZ_256   8
#define SZ_1K   10
#define SZ_2K   11
#define SZ_4K   12
#define SZ_8K   13
#define SZ_16K  14
#define SZ_32K  15
#define SZ_64K  16
#define SZ_128K 17
#define SZ_256K 18
#define SZ_512K 19
#define SZ_1M   20
#define SZ_2M   21
#define SZ_4M   22
#define SZ_8M   23
#define SZ_16M  24
#define SZ_32M  25
#define SZ_64M  26
#define SZ_128M 27
#define SZ_256M 28
#define SZ_512M 29
#define SZ_1G   30
#define SZ_2G   31

#define BLK_RW  (0 << 7)
#define BLK_RO  (1 << 7)

struct blk_desc {
	uint32_t ref;
	uint8_t  opt;
	uint8_t  siz;
	uint16_t cnt;
};

struct mem_desc {
	char name[8];
	struct blk_desc blk[];
};

struct gdb_target {
	char name[20];
	struct {
		const struct mem_desc * ram;
		const struct mem_desc * flash;
	} mem;
	struct {
		uint32_t start_addr;
		uint32_t block_size;
	} app;
};

#ifdef __cplusplus
extern "C" {
#endif

void gdb_init(void (* shell)(struct dmon_comm * ), 
			  const struct gdb_target * tgt); 

void __attribute__((noreturn)) gdb_task(struct dmon_comm * comm);

#ifdef __cplusplus
}
#endif	

#endif /* __GDB_H__ */

