/*
 * File:	debug.h
 * Module:
 * Project:	
 * Author:	Robinson Mittmann bobmittmann@gmail.com)
 * Target:
 * Comment:
 *
 */

#ifndef __DEBUG_H__
#define __DEBUG_H__

enum {
	DBG_NONE    = 0,
	DBG_PANIC   = 1,
	DBG_EXCEPT  = 2,
	DBG_ERROR   = 3,
	DBG_WARNING = 4,
	DBG_TRACE   = 5,
	DBG_INFO    = 6,
	DBG_MSG     = 7
};

#ifdef DEBUG_LEVEL
  #ifndef DEBUG
    #define DEBUG
  #endif
#endif

#ifdef DEBUG 
  #ifndef DEBUG_LEVEL
    #if DEBUG > 1
      #define DEBUG_LEVEL DEBUG
    #else
      #define DEBUG_LEVEL DBG_TRACE
    #endif
  #endif
#endif

#ifdef DEBUG
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#ifdef  _WIN32
#include <windows.h>
static inline uint32_t __dbg_timestamp(void)
{
	return GetTickCount();
}
#elif __APPLE__
#include <mach/clock.h>
#include <mach/mach.h>
static inline uint32_t __dbg_timestamp(void)
{
	clock_serv_t clk;
	mach_timespec_t ts;

	host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &clk);
	clock_get_time(clk, &ts);
	mach_port_deallocate(mach_task_self(), clk);
	return (uint64_t)ts.tv_sec * 1000LL + (uint64_t)ts.tv_nsec / 1000000LL;
}
#else
#include <sys/time.h>
#include <time.h>
static inline uint32_t __dbg_timestamp(void)
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (uint64_t)ts.tv_sec * 1000LL + (uint64_t)ts.tv_nsec / 1000000LL;
}
#endif

#define DBG(LEVEL, __FMT, ...) do { if (LEVEL <= DEBUG_LEVEL)  { \
		uint32_t ts = __dbg_timestamp(); \
		fprintf(stderr, "%02d.%03d: ", ts / 1000, ts % 1000); \
		if (LEVEL == DBG_PANIC)   fprintf(stderr, "PANIC: "); \
		if (LEVEL == DBG_ERROR)   fprintf(stderr, "ERR  : "); \
		if (LEVEL == DBG_WARNING) fprintf(stderr, "WARN : "); \
		if (LEVEL == DBG_TRACE)   fprintf(stderr, "TRACE: "); \
		if (LEVEL == DBG_INFO)    fprintf(stderr, "INFO : "); \
		if (LEVEL == DBG_MSG)     fprintf(stderr, "MSG  : "); \
		fprintf(stderr, "%s: ", __FUNCTION__); \
		fprintf(stderr, __FMT, ## __VA_ARGS__); \
		fprintf(stderr, "\n"); \
		fflush(stderr); \
		if (LEVEL == DBG_PANIC) abort(); \
		}} while (0)

#define DBG_DUMP(LEVEL, P, N) if (1) { if (LEVEL <= DEBUG_LEVEL)  { \
		fprintf(stderr, "%s: ", __FUNCTION__); \
		if (LEVEL == DBG_PANIC) fprintf(stderr, "PANIC:"); \
		if (LEVEL == DBG_ERROR) fprintf(stderr, "ERROR:"); \
		if (LEVEL == DBG_WARNING) fprintf(stderr, "WARNING:"); \
		if (LEVEL == DBG_TRACE) fprintf(stderr, "TRACE:"); \
		if (LEVEL == DBG_INFO) fprintf(stderr, "INFO:"); \
		if (LEVEL == DBG_MSG) fprintf(stderr, "MSG: "); \
		fflush(stderr); \
		{ int __i; for (__i = 0; __i < N; __i++) \
			fprintf(stderr, " %02x", ((unsigned char *)P)[__i]); } \
		fprintf(stderr, "\n"); }} else (void) 0

#define DBG_PUTC(LEVEL, C) if (1) { if (LEVEL <= DEBUG_LEVEL)  { \
		fputc((C), stderr); }} else (void) 0

#define DBG_PUTS(LEVEL, S) if (1) { if (LEVEL <= DEBUG_LEVEL)  { \
		fputs((S), stderr); }} else (void) 0

#define DBG_PRINTF(LEVEL, __FMT, ...) if (1) { if (LEVEL <= DEBUG_LEVEL)  { \
	fprintf(stderr, __FMT, ## __VA_ARGS__); }} else (void) 0

#else /* not DEBUG */
#define DBG(LEVEL, __FMT, ...)
#define DBG_PRINTF(LEVEL, __FMT, ...)
#define DBG_DUMP(LEVEL, P, N)
#define DBG_PUTC(LEVEL, C)
#define DBG_PUTS(LEVEL, S)
#endif /* not DEBUG */

#endif /* not __DEBUG_H__ */

