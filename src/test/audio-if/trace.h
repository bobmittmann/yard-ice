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
 * @file trace.c
 * @brief Real-time trace
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __TRACE_H__
#define __TRACE_H__

#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ----------------------------------------------------------------------
 * Real-Time Trace
 * ----------------------------------------------------------------------
 */

uint32_t trace_timestamp(void);

uint32_t trace_ts2us(uint32_t tm);

void trace_init(void);

int32_t trace_dt(uint32_t * prev);

int tracef(const char *fmt, ... );

void trace(const char * msg);

void trace_print(FILE * f, int flush);


#ifdef __cplusplus
}
#endif	

#endif /* __TRACE_H__ */
