/* 
 * File:    sys/ice_trace.h
 * Author:  Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(C) 2012 Bob Mittmann. All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */


#ifndef __ICE_TRACE_H__
#define __ICE_TRACE_H__

#ifdef DEBUG
#define ENABLE_LOG 
#ifdef DEBUG_LEVEL
#define LOG_LEVEL DEBUG_LEVEL
#endif
#undef DEBUG
#undef DEBUG_LEVEL
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

#define DCC_LOG(__LVL, __MSG) \
	if (1) { if (__LVL <= LOG_LEVEL)  { ice_trace0( ({ \
	static const char _f[] __attribute__ ((section(".dccdata"))) = __FILE__;\
	static const char _m[] __attribute__ ((section(".dccdata"))) = (__MSG);\
	static const struct trace_entry __attribute__((section(".dcclog"))) \
	log_entry = { _f, __LINE__, __LVL, 0, __FUNCTION__, _m}; \
	(struct trace_entry *)&log_entry; })); }} else (void) 0

#define DCC_LOG1(__LVL, __FMT, __A) \
	if (1) { if (__LVL <= LOG_LEVEL)  { ice_trace1( ({ \
	static const char _f[] __attribute__ ((section(".dccdata"))) = __FILE__;\
	static const char _m[] __attribute__ ((section(".dccdata"))) = (__FMT);\
	static const struct trace_entry __attribute__((section(".dcclog"))) \
	log_entry = { _f, __LINE__, __LVL, 0, __FUNCTION__, _m}; \
	(struct trace_entry *)&log_entry; }), (int)(__A)); }} else (void) 0

#define DCC_LOG2(__LVL, __FMT, __A, __B) \
	if (1) { if (__LVL <= LOG_LEVEL)  { ice_trace2( ({ \
	static const char _f[] __attribute__ ((section(".dccdata"))) = __FILE__;\
	static const char _m[] __attribute__ ((section(".dccdata"))) = (__FMT);\
	static const struct trace_entry __attribute__((section(".dcclog"))) \
	log_entry = { _f, __LINE__, __LVL, 0, __FUNCTION__, _m}; \
	(struct trace_entry *)&log_entry; }), (int)(__A), \
	(int)(__B)); }} else (void) 0

#define DCC_LOG3(__LVL, __FMT, __A, __B, __C) \
	if (1) { if (__LVL <= LOG_LEVEL)  { ice_trace3( ({ \
	static const char _f[] __attribute__ ((section(".dccdata"))) = __FILE__;\
	static const char _m[] __attribute__ ((section(".dccdata"))) = (__FMT);\
	static const struct trace_entry __attribute__((section(".dcclog"))) \
	log_entry = { _f, __LINE__, __LVL, 0, __FUNCTION__, _m}; \
	(struct trace_entry *)&log_entry; }), (int)(__A), \
	(int)(__B), (int)(__C)); }} else (void) 0

#define DCC_LOG4(__LVL, __FMT, __A, __B, __C, __D) \
	if (1) { if (__LVL <= LOG_LEVEL)  { ice_trace4( ({ \
	static const char _f[] __attribute__ ((section(".dccdata"))) = __FILE__;\
	static const char _m[] __attribute__ ((section(".dccdata"))) = (__FMT);\
	static const struct trace_entry __attribute__((section(".dcclog"))) \
	log_entry = { _f, __LINE__, __LVL, 0, __FUNCTION__, _m}; \
	(struct trace_entry *)&log_entry; }), (int)(__A), (int)(__B), \
	  (int)(__C), (int)(__D)); \
	}} else (void) 0

#define DCC_LOG5(__LVL, __FMT, __A, __B, __C, __D, __E) \
	if (1) { if (__LVL <= LOG_LEVEL)  { ice_trace5( ({ \
	static const char _f[] __attribute__ ((section(".dccdata"))) = __FILE__;\
	static const char _m[] __attribute__ ((section(".dccdata"))) = (__FMT);\
	static const struct trace_entry __attribute__((section(".dcclog"))) \
	log_entry = { _f, __LINE__, __LVL, 0, __FUNCTION__, _m}; \
	(struct trace_entry *)&log_entry; }), (int)(__A), (int)(__B), \
	  (int)(__C), (int)(__D), (int)(__E)); \
	}} else (void) 0

#define DCC_LOG6(__LVL, __FMT, __A, __B, __C, __D, __E, __F) \
	if (1) { if (__LVL <= LOG_LEVEL)  { ice_trace6( ({ \
	static const char _f[] __attribute__ ((section(".dccdata"))) = __FILE__;\
	static const char _m[] __attribute__ ((section(".dccdata"))) = (__FMT);\
	static const struct trace_entry __attribute__((section(".dcclog"))) \
	log_entry = { _f, __LINE__, __LVL, 0, __FUNCTION__, _m}; \
	(struct trace_entry *)&log_entry; }), (int)__A, (int)__B, \
	  (int)__C, (int)__D, (int)__E, (int)__F); \
	}} else (void) 0

#define DCC_LOG7(__LVL, __FMT, __A, __B, __C, __D, __E, __F, __G) \
	if (1) { if (__LVL <= LOG_LEVEL)  { ice_trace7( ({ \
	static const char _f[] __attribute__ ((section(".dccdata"))) = __FILE__;\
	static const char _m[] __attribute__ ((section(".dccdata"))) = (__FMT);\
	static const struct trace_entry __attribute__((section(".dcclog"))) \
	log_entry = { _f, __LINE__, __LVL, 0, __FUNCTION__, _m}; \
	(struct trace_entry *)&log_entry; }), (int)__A, (int)__B, \
	  (int)__C, (int)__D, (int)__E, (int)__F, (int)__G); \
	}} else (void) 0

#define DCC_LOG8(__LVL, __FMT, __A, __B, __C, __D, __E, __F, __G, __H) \
	if (1) { if (__LVL <= LOG_LEVEL)  { ice_trace8( ({ \
	static const char _f[] __attribute__ ((section(".dccdata"))) = __FILE__;\
	static const char _m[] __attribute__ ((section(".dccdata"))) = (__FMT);\
	static const struct trace_entry __attribute__((section(".dcclog"))) \
	log_entry = { _f, __LINE__, __LVL, 0, __FUNCTION__, _m}; \
	(struct trace_entry *)&log_entry; }), (int)__A, (int)__B, \
	  (int)__C, (int)__D, (int)__E, (int)__F, (int)__G, (int)__H); \
	}} else (void) 0

#define DCC_LOG9(__LVL, __FMT, __A, __B, __C, __D, __E, __F, __G, __H, __I) \
	if (1) { if (__LVL <= LOG_LEVEL)  { ice_trace9( ({ \
	static const char _f[] __attribute__ ((section(".dccdata"))) = __FILE__;\
	static const char _m[] __attribute__ ((section(".dccdata"))) = (__FMT);\
	static const struct trace_entry __attribute__((section(".dcclog"))) \
	log_entry = { _f, __LINE__, __LVL, 0, __FUNCTION__, _m}; \
	(struct trace_entry *)&log_entry; }), (int)__A, (int)__B, \
	  (int)__C, (int)__D, (int)__E, (int)__F, (int)__G, (int)__H, (int)__I); \
	}} else (void) 0

#define DCC_LOG10(__LVL, __FMT, __A, __B, __C, __D, __E, __F, __G, __H, __I, __J) \
	if (1) { if (__LVL <= LOG_LEVEL)  { ice_trace10( ({ \
	static const char _f[] __attribute__ ((section(".dccdata"))) = __FILE__;\
	static const char _m[] __attribute__ ((section(".dccdata"))) = (__FMT);\
	static const struct trace_entry __attribute__((section(".dcclog"))) \
	log_entry = { _f, __LINE__, __LVL, 0, __FUNCTION__, _m}; \
	(struct trace_entry *)&log_entry; }), (int)__A, (int)__B, \
	  (int)__C, (int)__D, (int)__E, (int)__F, (int)__G, (int)__H, (int)__I, (int)__J); \
	}} else (void) 0



#else

#define DCC_LOG_INIT()
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

int ice_trace_connect(unsigned int tmo);

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

