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
#include <sys/time.h>

enum trace_level {
	TRACE_LVL_NONE = 0,
	TRACE_LVL_ERR = 1,
	TRACE_LVL_WARN = 2,
	TRACE_LVL_INF = 3,
	TRACE_LVL_DBG = 4
};

struct trace_ref {
	unsigned char lvl;
	unsigned char opt;
	unsigned short line;
	const char * func;
	const char * fmt;
};

struct trace_entry {
	const struct trace_ref * ref;
	uint32_t dt;
	uint64_t tm;
	uint32_t idx;
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

#ifndef TRACE_LEVEL
 #define TRACE_LEVEL TRACE_LVL_DBG
#else
 #ifndef ENABLE_TRACE
  #define ENABLE_TRACE
 #endif
#endif

/* Trace optons */
#define TRACE_FLUSH     (1 << 0)
#define TRACE_COUNT     (1 << 1)
#define TRACE_ABSTIME   (1 << 2)
#define TRACE_ALL       (1 << 3)
#define TRACE_UNSAFE    (1 << 4)
#define TRACE_FUNC      (1 << 5)

#ifdef ENABLE_TRACE

#define DBG(__FMT, ...) if (1) { if (TRACE_LEVEL >= TRACE_LVL_DBG)  { \
		static const struct trace_ref __ref = \
		{ TRACE_LVL_DBG, 0, __LINE__, __FUNCTION__, __FMT}; \
		tracef(&__ref, ## __VA_ARGS__); \
		}} else (void) 0


#define INF(__FMT, ...) if (1) { if (TRACE_LEVEL >= TRACE_LVL_INF)  { \
		static const struct trace_ref __ref = \
		{ TRACE_LVL_INF, 0, __LINE__, __FUNCTION__, __FMT}; \
		tracef(&__ref, ## __VA_ARGS__); \
		}} else (void) 0


#define WARN(__FMT, ...) if (1) { if (TRACE_LEVEL >= TRACE_LVL_WARN)  { \
		static const struct trace_ref __ref = \
		{ TRACE_LVL_WARN, 0, __LINE__, __FUNCTION__, __FMT}; \
		tracef(&__ref, ## __VA_ARGS__); \
		}} else (void) 0

#define ERR(__FMT, ...) if (1) { if (TRACE_LEVEL >= TRACE_LVL_ERR)  { \
		static const struct trace_ref __ref = \
		{ TRACE_LVL_ERR, 0, __LINE__, __FUNCTION__, __FMT}; \
		tracef(&__ref, ## __VA_ARGS__); \
		}} else (void) 0


#define DBG_S(__STR) if (1) { if (TRACE_LEVEL >= TRACE_LVL_DBG)  { \
		static const struct trace_ref __ref = \
		{ TRACE_LVL_DBG, 0, __LINE__, __FUNCTION__, __STR}; \
		trace(&__ref); \
		}} else (void) 0

#define INF_S(__STR) if (1) { if (TRACE_LEVEL >= TRACE_LVL_INF)  { \
		static const struct trace_ref __ref = \
		{ TRACE_LVL_INF, 0, __LINE__, __FUNCTION__, __STR}; \
		trace(&__ref); \
		}} else (void) 0

#define WARN_S(__STR) if (1) { if (TRACE_LEVEL >= TRACE_LVL_WARN)  { \
		static const struct trace_ref __ref = \
		{ TRACE_LVL_WARN, 0, __LINE__, __FUNCTION__, __STR}; \
		trace(&__ref); \
		}} else (void) 0

#define ERR_S(__STR) if (1) { if (TRACE_LEVEL >= TRACE_LVL_ERR)  { \
		static const struct trace_ref __ref = \
		{ TRACE_LVL_ERR, 0, __LINE__, __FUNCTION__, __STR}; \
		trace(&__ref); \
		}} else (void) 0


#else

#define DBG(__FMT, ...)
#define INF(__FMT, ...)
#define WARN(__FMT, ...)
#define ERR(__FMT, ...)

#define DBG_S(__STR)
#define INF_S(__STR)
#define WARN_S(__STR)
#define ERR_S(__STR)

#endif

extern const char * const trace_lvl_nm[];

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

//__attribute__((format (__printf__, 1, 2)))

void tracef(const struct trace_ref * ref, ... );

void trace(const struct trace_ref * ref);

int trace_tm2timeval(struct timeval * tv, uint64_t tm);

int trace_ts2timeval(struct timeval * tv, uint32_t ts);

/* ----------------------------------------------------------------------
 * Trace decode
 * ----------------------------------------------------------------------
 */

int trace_fprint(FILE * f, unsigned int opt);

int trace_getfirst(struct trace_entry * entry, char * s, int max);

int trace_getnext(struct trace_entry * entry, char * s, int max);

void trace_flush(struct trace_entry * entry);

int trace_tail(struct trace_entry * entry);

#ifdef __cplusplus
}
#endif	

#endif /* __TRACE_H__ */
