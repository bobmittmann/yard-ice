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

#ifdef __cplusplus
extern "C" {
#endif

void gdb_init(void (* shell)(struct dmon_comm * ));

void __attribute__((noreturn)) gdb_task(struct dmon_comm * comm);

#ifdef __cplusplus
}
#endif	

#endif /* __GDB_H__ */

