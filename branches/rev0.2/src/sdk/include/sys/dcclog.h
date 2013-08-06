/* 
 * Copyright(c) 2009-2012 BORESTE (www.boreste.com). All Rights Reserved.
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
 * @file sys/dcclog.h
 * @brief libice-comm
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __ICE_TRACE_H__
#define __ICE_TRACE_H__

#ifdef DEBUG
 #define ENABLE_LOG 
 #ifdef DEBUG_LEVEL
  #define LOG_LEVEL DEBUG_LEVEL
 #elif DEBUG == 2
  #define LOG_LEVEL 2
 #elif DEBUG == 3
  #define LOG_LEVEL 3
 #elif DEBUG == 4
  #define LOG_LEVEL 4
 #elif DEBUG == 5
  #define LOG_LEVEL 5
 #elif DEBUG == 6
  #define LOG_LEVEL 6
 #elif DEBUG == 7
  #define LOG_LEVEL 7
 #endif
#endif

struct trace_entry {
	const char * file;
	unsigned short line;
	unsigned char level;
	unsigned char opt;
	const char * function;
	const char * msg;
};

#define LOG_PANIC   1
#define LOG_EXCEPT  2
#define LOG_ERROR   3
#define LOG_WARNING 4
#define LOG_TRACE   5
#define LOG_INFO    6
#define LOG_MSG     7

#ifdef LOG_LEVEL
#ifndef ENABLE_LOG
#define ENABLE_LOG
#endif
#endif

#ifdef ENABLE_LOG 
#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_TRACE
#endif
#endif

#ifdef ENABLE_LOG

#define DCC_LOG_INIT() ice_trace_init()
#define DCC_LOG_CONNECT() ice_comm_connect()

#define DCC_LOG(__LVL, __MSG) \
	{ if (__LVL <= LOG_LEVEL)  { ice_trace0( ({ \
	static const char _f[] __attribute__ ((section(".dccdata"))) = __FILE__;\
	static const char _m[] __attribute__ ((section(".dccdata"))) = (__MSG);\
	static const struct trace_entry __attribute__((section(".dcclog"))) \
	log_entry = { _f, __LINE__, __LVL, 0, __FUNCTION__, _m}; \
	(struct trace_entry *)&log_entry; })); }}

#define DCC_LOG1(__LVL, __FMT, __A) \
	{ if (__LVL <= LOG_LEVEL)  { ice_trace1( ({ \
	static const char _f[] __attribute__ ((section(".dccdata"))) = __FILE__;\
	static const char _m[] __attribute__ ((section(".dccdata"))) = (__FMT);\
	static const struct trace_entry __attribute__((section(".dcclog"))) \
	log_entry = { _f, __LINE__, __LVL, 0, __FUNCTION__, _m}; \
	(struct trace_entry *)&log_entry; }), (int)(__A)); }}

#define DCC_LOG2(__LVL, __FMT, __A, __B) \
	{ if (__LVL <= LOG_LEVEL)  { ice_trace2( ({ \
	static const char _f[] __attribute__ ((section(".dccdata"))) = __FILE__;\
	static const char _m[] __attribute__ ((section(".dccdata"))) = (__FMT);\
	static const struct trace_entry __attribute__((section(".dcclog"))) \
	log_entry = { _f, __LINE__, __LVL, 0, __FUNCTION__, _m}; \
	(struct trace_entry *)&log_entry; }), (int)(__A), \
	(int)(__B)); }}

#define DCC_LOG3(__LVL, __FMT, __A, __B, __C) \
	{ if (__LVL <= LOG_LEVEL)  { ice_trace3( ({ \
	static const char _f[] __attribute__ ((section(".dccdata"))) = __FILE__;\
	static const char _m[] __attribute__ ((section(".dccdata"))) = (__FMT);\
	static const struct trace_entry __attribute__((section(".dcclog"))) \
	log_entry = { _f, __LINE__, __LVL, 0, __FUNCTION__, _m}; \
	(struct trace_entry *)&log_entry; }), (int)(__A), \
		(int)(__B), (int)(__C)); }}

#define DCC_LOG4(__LVL, __FMT, __A, __B, __C, __D) \
	{ if (__LVL <= LOG_LEVEL)  { ice_trace4( ({ \
	static const char _f[] __attribute__ ((section(".dccdata"))) = __FILE__;\
	static const char _m[] __attribute__ ((section(".dccdata"))) = (__FMT);\
	static const struct trace_entry __attribute__((section(".dcclog"))) \
	log_entry = { _f, __LINE__, __LVL, 0, __FUNCTION__, _m}; \
	(struct trace_entry *)&log_entry; }), (int)(__A), (int)(__B), \
		(int)(__C), (int)(__D)); }}

#define DCC_LOG5(__LVL, __FMT, __A, __B, __C, __D, __E) \
	{ if (__LVL <= LOG_LEVEL)  { ice_trace5( ({ \
	static const char _f[] __attribute__ ((section(".dccdata"))) = __FILE__;\
	static const char _m[] __attribute__ ((section(".dccdata"))) = (__FMT);\
	static const struct trace_entry __attribute__((section(".dcclog"))) \
	log_entry = { _f, __LINE__, __LVL, 0, __FUNCTION__, _m}; \
	(struct trace_entry *)&log_entry; }), (int)(__A), (int)(__B), \
		(int)(__C), (int)(__D), (int)(__E)); }}

#define DCC_LOG6(__LVL, __FMT, __A, __B, __C, __D, __E, __F) \
	{ if (__LVL <= LOG_LEVEL)  { ice_trace6( ({ \
	static const char _f[] __attribute__ ((section(".dccdata"))) = __FILE__;\
	static const char _m[] __attribute__ ((section(".dccdata"))) = (__FMT);\
	static const struct trace_entry __attribute__((section(".dcclog"))) \
	log_entry = { _f, __LINE__, __LVL, 0, __FUNCTION__, _m}; \
	(struct trace_entry *)&log_entry; }), (int)(__A), (int)(__B), \
		(int)(__C), (int)(__D), (int)(__E), (int)(__F)); }}

#define DCC_LOG7(__LVL, __FMT, __A, __B, __C, __D, __E, __F, __G) \
	{ if (__LVL <= LOG_LEVEL)  { ice_trace7( ({ \
	static const char _f[] __attribute__ ((section(".dccdata"))) = __FILE__;\
	static const char _m[] __attribute__ ((section(".dccdata"))) = (__FMT);\
	static const struct trace_entry __attribute__((section(".dcclog"))) \
	log_entry = { _f, __LINE__, __LVL, 0, __FUNCTION__, _m}; \
	(struct trace_entry *)&log_entry; }), (int)(__A), (int)(__B), \
		(int)(__C), (int)(__D), (int)(__E), (int)(__F), (int)(__G)); }}

#define DCC_LOG8(__LVL, __FMT, __A, __B, __C, __D, __E, __F, __G, __H) \
	{ if (__LVL <= LOG_LEVEL)  { ice_trace8( ({ \
	static const char _f[] __attribute__ ((section(".dccdata"))) = __FILE__;\
	static const char _m[] __attribute__ ((section(".dccdata"))) = (__FMT);\
	static const struct trace_entry __attribute__((section(".dcclog"))) \
	log_entry = { _f, __LINE__, __LVL, 0, __FUNCTION__, _m}; \
	(struct trace_entry *)&log_entry; }), (int)(__A), (int)(__B), \
		(int)(__C), (int)(__D), (int)(__E), (int)(__F), (int)(__G), \
		(int)(__H)); }}

#define DCC_LOG9(__LVL, __FMT, __A, __B, __C, __D, __E, __F, __G, __H, __I) \
	{ if (__LVL <= LOG_LEVEL)  { ice_trace9( ({ \
	static const char _f[] __attribute__ ((section(".dccdata"))) = __FILE__;\
	static const char _m[] __attribute__ ((section(".dccdata"))) = (__FMT);\
	static const struct trace_entry __attribute__((section(".dcclog"))) \
	log_entry = { _f, __LINE__, __LVL, 0, __FUNCTION__, _m}; \
	(struct trace_entry *)&log_entry; }), (int)(__A), (int)(__B), \
		(int)(__C), (int)(__D), (int)(__E), (int)(__F), (int)(__G), \
		(int)(__H), (int)(__I)); }}

#define DCC_LOG10(__LVL, __FMT, __A, __B, __C, __D, __E, __F, __G, __H, \
				  __I, __J) \
	{ if (__LVL <= LOG_LEVEL)  { ice_trace10( ({ \
	static const char _f[] __attribute__ ((section(".dccdata"))) = __FILE__;\
	static const char _m[] __attribute__ ((section(".dccdata"))) = (__FMT);\
	static const struct trace_entry __attribute__((section(".dcclog"))) \
	log_entry = { _f, __LINE__, __LVL, 0, __FUNCTION__, _m}; \
	(struct trace_entry *)&log_entry; }), (int)(__A), (int)(__B), \
		(int)(__C), (int)(__D), (int)(__E), (int)(__F), (int)(__G), \
		(int)(__H), (int)(__I), (int)(__J)); }}

#else

#define DCC_LOG_INIT()
#define DCC_LOG_CONNECT()
#define DCC_LOG(__LVL, __MSG)
#define DCC_LOG1(__LVL, __FMT, __A)
#define DCC_LOG2(__LVL, __FMT, __A, __B)
#define DCC_LOG3(__LVL, __FMT, __A, __B, __C)
#define DCC_LOG4(__LVL, __FMT, __A, __B, __C, __D)
#define DCC_LOG5(__LVL, __FMT, __A, __B, __C, __D, __E)
#define DCC_LOG6(__LVL, __FMT, __A, __B, __C, __D, __E, __F)
#define DCC_LOG7(__LVL, __FMT, __A, __B, __C, __D, __E, __F, __G)
#define DCC_LOG8(__LVL, __FMT, __A, __B, __C, __D, __E, __F, __G, __H)
#define DCC_LOG9(__LVL, __FMT, __A, __B, __C, __D, __E, __F, __G, __H, __I)
#define DCC_LOG10(__LVL, __FMT, __A, __B, __C, __D, __E, __F, __G, __H, __I, __J)

#endif


#define LOG0(__LVL, __MSG) DCC_LOG(__LVL, __MSG)

#define LOG1(__LVL, __FMT, __A) DCC_LOG1(__LVL, __FMT, __A)

#define LOG2(__LVL, __FMT, __A, __B) DCC_LOG2(__LVL, __FMT, __A, __B)

#define LOG3(__LVL, __FMT, __A, __B, __C) DCC_LOG3(__LVL, __FMT, __A, __B, __C)

#define LOG4(__LVL, __FMT, __A, __B, __C, __D) \
	DCC_LOG4(__LVL, __FMT, __A, __B, __C, __D)

#define LOG5(__LVL, __FMT, __A, __B, __C, __D, __E) \
	DCC_LOG5(__LVL, __FMT, __A, __B, __C, __D, __E)

#define LOG6(__LVL, __FMT, __A, __B, __C, __D, __E, __F) \
	DCC_LOG6(__LVL, __FMT, __A, __B, __C, __D, __E, __F)

#define LOG7(__LVL, __FMT, __A, __B, __C, __D, __E, __F, __G) \
	DCC_LOG7(__LVL, __FMT, __A, __B, __C, __D, __E, __F, __G)

#define LOG8(__LVL, __FMT, __A, __B, __C, __D, __E, __F, __G, __H) \
	DCC_LOG8(__LVL, __FMT, __A, __B, __C, __D, __E, __F, __G, __H)

#ifdef __cplusplus
extern "C" {
#endif

void ice_trace_init(void);

void ice_comm_connect();

void ice_trace0(const struct trace_entry * __entry);

void ice_trace1(const struct trace_entry * __entry, int __a);

void ice_trace2(const struct trace_entry * __entry, int __a, int __b);

void ice_trace3(const struct trace_entry * __entry, int __a, int __b, int __c);

void ice_trace4(const struct trace_entry * __entry, int __a, int __b, 
			 int __c, int __d);

void ice_trace5(const struct trace_entry * __entry, int __a, int __b, 
			 int __c, int __d, int e);

void ice_trace6(const struct trace_entry * __entry, int __a, int __b, 
			 int __c, int __d, int __e, int __f);

void ice_trace7(const struct trace_entry * __entry, int __a, int __b, 
			 int __c, int __d, int __e, int __f, int __g);

void ice_trace8(const struct trace_entry * __entry, int __a, int __b, 
			 int __c, int __d, int __e, int __f, int __g, int __h);

void ice_trace9(const struct trace_entry * __entry, int __a, int __b, 
			 int __c, int __d, int __e, int __f, int __g, int __h,
			 int __i);

void ice_trace10(const struct trace_entry * __entry, int __a, int __b, 
			 int __c, int __d, int __e, int __f, int __g, int __h,
			 int __i, int __j);

#ifdef __cplusplus
}
#endif

#endif /* __ICE_TRACE_H__ */

