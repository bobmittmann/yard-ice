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
 * @file monitor-i.h
 * @brief Bootloader monitor private header
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */

#ifndef __MONITOR_I_H__
#define __MONITOR_I_H__

#ifdef CONFIG_H
#include "config.h"
#endif

#define __THINKOS_MONITOR__
#include <thinkos/monitor.h>
#define __THINKOS_BOOTLDR__
#include <thinkos/bootldr.h>
#define __THINKOS_CONSOLE__
#include <thinkos/console.h>
#define __THINKOS_DEBUG__
#include <thinkos/debug.h>
#include <thinkos.h>

#include <stdlib.h>
#include <stdbool.h>

#define MONITOR_AUTOBOOT 1
#define MONITOR_SHELL 2


#define CTRL_C  0x03 /* ETX */

struct monitor {
	uint32_t flags;
};


#ifdef __cplusplus
extern "C" {
#endif

void monitor_task(const struct monitor_comm * comm, void * param);

#ifdef __cplusplus
}
#endif
#endif /* __MONITOR_I_H__ */

