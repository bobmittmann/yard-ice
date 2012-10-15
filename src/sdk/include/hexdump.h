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
 * @file hexdump.h
 * @brief YARD-ICE libhexdump
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __HEXDUMP_H__
#define __HEXDUMP_H__

#include <stdint.h> 
#include <stdio.h> 

#ifdef __cplusplus
extern "C" {
#endif

/* Show a line of 16 ASCII characters */
void show_line_char(FILE * f, uint32_t  addr, const uint8_t * data, int count);

/* Show a line of 4 32bits hexadecimal values */
void show_line_hex32(FILE * f, uint32_t  addr, const uint8_t * data, int count);

/* Show a line of 8 16bits hexadecimal values */
void show_line_hex16(FILE * f, uint32_t  addr, const uint8_t * data, int count);

/* Show a line of 16 8bits hexadecimal values */
void show_line_hex8(FILE * f, uint32_t  addr, const uint8_t * data, int count);

/* Show buffer as 32bits hexadecimal values followed by ASCII chars*/
int show_hex32(FILE * f, uint32_t  addr, const void * buf, int len);

/* Show buffer as 16bits hexadecimal values followed by ASCII chars*/
int show_hex16(FILE * f, uint32_t  addr, const void * buf, int len);

/* Show buffer as 8bits hexadecimal values followed by ASCII chars*/
int show_hex8(FILE * f, uint32_t  addr, const void * buf, int len);

#ifdef __cplusplus
}
#endif

#endif /* __HEXDUMP_H__ */

