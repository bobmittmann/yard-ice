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
 * @file sys/console.h
 * @brief YARD-ICE 
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 


#ifndef __SYS_CONSOLE_H__
#define __SYS_CONSOLE_H__

#include <stdint.h>
#include <stdbool.h>
#include <sys/file.h>

#ifdef __cplusplus
extern "C" {
#endif

int console_write(void * dev, const void * buf, unsigned int len); 

int console_read(void * dev, void * buf, unsigned int len, unsigned int msec); 

int console_drain(void * dev);

int console_close(void * dev);

struct file * console_fopen(void);

bool is_console_file(struct file * f);

#ifdef __cplusplus
}
#endif

#endif /* __SYS_CONSOLE_H__ */

