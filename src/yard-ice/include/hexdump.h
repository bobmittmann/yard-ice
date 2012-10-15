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
 * @file .h
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#ifndef __HEXDUMP_H__
#define __HEXDUMP_H__

#include <stdint.h> 
#include <stdio.h> 

#ifdef __cplusplus
extern "C" {
#endif

void show_line_char(FILE * f, uint32_t  addr, const uint8_t * data, int count);

void show_line_hex32(FILE * f, uint32_t  addr, const uint8_t * data, int count);

int show_hex32(FILE * f, uint32_t  addr, const void * buf, int len);

void show_line_hex16(FILE * f, uint32_t  addr, const uint8_t * data, int count);

int show_hex16(FILE * f, uint32_t  addr, const void * buf, int len);

void show_line_hex8(FILE * f, uint32_t  addr, const uint8_t * data, int count);

int show_hex8(FILE * f, uint32_t  addr, const void * buf, int len);

#ifdef __cplusplus
}
#endif

#endif /* __HEXDUMP_H__ */


