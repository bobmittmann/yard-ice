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
#define __THINKOS_APP__
#include <thinkos/app.h>
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

#define BOOTLOADER_BLOCK_OFFS 0x00000000
#define BOOTLOADER_BLOCK_SIZE (16 * 1024)
#define BOOTLOADER_START_ADDR (0x08000000 + BOOTLOADER_BLOCK_OFFS)

#define CONFIG_BLOCK_OFFS 0x00008000
#define CONFIG_BLOCK_SIZE (16 * 1024)
#define CONFIG_START_ADDR (0x08000000 + CONFIG_BLOCK_OFFS)

#define RBF_BLOCK_OFFS 0x00010000
#define RBF_BLOCK_SIZE (64 * 1024)
#define RBF_START_ADDR (0x08000000 + RBF_BLOCK_OFFS)

#define APPLICATION_BLOCK_OFFS 0x00020000
#define APPLICATION_BLOCK_SIZE (384 * 1024)
#define APPLICATION_START_ADDR (0x08000000 + APPLICATION_BLOCK_OFFS)

extern const struct magic_blk board_app_magic;

#ifdef __cplusplus
extern "C" {
#endif

void monitor_task(const struct monitor_comm * comm, void * param);

#ifdef __cplusplus
}
#endif
#endif /* __MONITOR_I_H__ */

