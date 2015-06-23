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

enum trace_level {
	TRACE_LVL_NONE = 0,
	TRACE_LVL_ERR = 1,
	TRACE_LVL_WARN = 2,
	TRACE_LVL_INF = 3,
	TRACE_LVL_DBG = 4
};

#ifdef DEBUG
 #define ENABLE_TRACE
 #ifdef DEBUG_LEVEL
  #define TRACE_LEVEL DEBUG_LEVEL
 #elif DEBUG == 2
  #define TRACE_LEVEL 2
 #elif DEBUG == 3
  #define TRACE_LEVEL 3
 #elif DEBUG == 4
  #define TRACE_LEVEL 4
 #elif DEBUG == 5
  #define TRACE_LEVEL 5
 #elif DEBUG == 6
  #define TRACE_LEVEL 6
 #elif DEBUG == 7
  #define TRACE_LEVEL 7
 #endif
#endif

/* Trace optons */
#define TRACE_FLUSH     (1 << 0)
#define TRACE_COUNT     (1 << 1)
#define TRACE_ABSTIME   (1 << 2)
#define TRACE_ALL       (1 << 3)
#define TRACE_UNSAFE    (1 << 4)

#ifdef ENABLE_TRACE

#define DBG(__FMT, ...) if (1) { if (TRACE_LEVEL >= TRACE_LVL_DBG)  { \
		tracef(__FMT, ## __VA_ARGS__); \
		}} else (void) 0


#define INF(__FMT, ...) if (1) { if (TRACE_LEVEL >= TRACE_LVL_INF)  { \
		tracef(__FMT, ## __VA_ARGS__); \
		}} else (void) 0


#define WARN(__FMT, ...) if (1) { if (TRACE_LEVEL >= TRACE_LVL_WARN)  { \
		tracef(__FMT, ## __VA_ARGS__); \
		}} else (void) 0


#define ERR(__FMT, ...) if (1) { if (TRACE_LEVEL >= TRACE_LVL_ERR)  { \
		tracef(__FMT, ## __VA_ARGS__); \
		}} else (void) 0


#else

#define DBG(__FMT, ...)
#define INF(__FMT, ...)
#define WARN(__FMT, ...)
#define ERR(__FMT, ...)


#endif

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

__attribute__((format (__printf__, 1, 2))) void tracef(const char *fmt, ... );

void trace(const char * msg);

int trace_fprint(FILE * f, unsigned int opt);

void trace_flush(void);

#ifdef __cplusplus
}
#endif	

#endif /* __TRACE_H__ */
