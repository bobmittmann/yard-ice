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

#define DBG_NONE      0
#define DBG_PANIC     1
#define DBG_EXCEPT    2
#define DBG_ERROR     3
#define DBG_WARNING   4
#define DBG_TRACE     5
#define DBG_INFO      6
#define DBG_MSG       7

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

#ifdef  WIN32
#include <windows.h>
static inline uint32_t __timestamp(void)
{
	return GetTickCount();
}
#else
#include <sys/time.h>
static inline uint32_t __timestamp(void)
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (uint64_t)ts.tv_sec * 1000LL + (uint64_t)ts.tv_nsec / 1000000LL;
}
#endif

#define DBG(LEVEL, __FMT, ...) if (1) { if (LEVEL <= DEBUG_LEVEL)  { \
		uint32_t ts = __timestamp(); \
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
		}} else (void) 0

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

