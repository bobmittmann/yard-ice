/* 
 * Copyright(C) 2013 Robinson Mittmann. All Rights Reserved.
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
 * @file config.h
 * @brief USB to Serial Converter configuration
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#ifndef __CONFIG_H__
#define __CONFIG_H__

#define TTY_INBUF_LEN 8
#define TTY_DEV_MAX 1

#define FILE_DEV_MAX 2

#define MICROJS_DEBUG_ENABLED 0
#define MICROJS_TRACE_ENABLED 0
#define MICROJS_STRINGS_ENABLED 1
#define MICROJS_STRINGBUF_ENABLED 1
#define MICROJS_VERBOSE_ENABLED 1
#define MICROJS_STDLIB_ENABLED 0
#define MICROJS_OPTIMIZATION_ENABLED 1

#endif /* __CONFIG_H__ */

