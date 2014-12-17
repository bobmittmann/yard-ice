/* 
 * debug.h
 *
 * Copyright(C) 2009 Robinson Mittmann. All Rights Reserved.
 * 
 * This file is part of the ThinkOS library.
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


#ifndef __DEBUG_H__
#define __DEBUG_H__

/* 1 is reserved for default debug level */
#define _ERR_       2
#define _WARN_      3
#define _INF_       4
#define _DBG_       5
#define _DBG1_      6
#define _DBG2_      7
#define _DBG3_      8
#define _DBG4_      9
#define _DBG5_     10

#ifdef DEBUG 
 #if (DEBUG == 1) /* Default debug level */
  #undef DEBUG 
  #define DEBUG _DBG_
 #endif
#endif

#ifdef DEBUG
#include <stdio.h>
#include <stdlib.h>

#define ERR(__FMT, ...) do { if (DEBUG >= _ERR_)  { \
	fprintf(stderr, " ERR: %s: " __FMT "\n", __FUNCTION__, ## __VA_ARGS__); \
	fflush(stderr); }} while (0)

#define WARN(__FMT, ...) do { if (DEBUG >= _WARN_)  { \
	fprintf(stderr, "WARN: %s: " __FMT "\n", __FUNCTION__, ## __VA_ARGS__); \
	fflush(stderr); }} while (0)

#define INF(__FMT, ...) do { if (DEBUG >= _INF_)  { \
	fprintf(stderr, " INF: %s: " __FMT "\n", __FUNCTION__, ## __VA_ARGS__); \
	fflush(stderr); }} while (0)

#define DBG(__FMT, ...) do { if (DEBUG >= _DBG_)  { \
	fprintf(stderr, " DBG: %s: " __FMT "\n", __FUNCTION__, ## __VA_ARGS__); \
	fflush(stderr); }} while (0)

#define DBG1(__FMT, ...) do { if (DEBUG >= _DBG1_)  { \
	fprintf(stderr, " DBG: %s: " __FMT "\n", __FUNCTION__, ## __VA_ARGS__); \
	fflush(stderr); }} while (0)

#define DBG2(__FMT, ...) do { if (DEBUG >= _DBG2_)  { \
	fprintf(stderr, " DBG: %s: " __FMT "\n", __FUNCTION__, ## __VA_ARGS__); \
	fflush(stderr); }} while (0)

#define DBG3(__FMT, ...) do { if (DEBUG >= _DBG3_)  { \
	fprintf(stderr, " DBG: %s: " __FMT "\n", __FUNCTION__, ## __VA_ARGS__); \
	fflush(stderr); }} while (0)

#define DBG4(__FMT, ...) do { if (DEBUG >= _DBG4_)  { \
	fprintf(stderr, " DBG: %s: " __FMT "\n", __FUNCTION__, ## __VA_ARGS__); \
	fflush(stderr); }} while (0)

#define DBG5(__FMT, ...) do { if (DEBUG >= _DBG5_)  { \
	fprintf(stderr, " DBG: %s: " __FMT "\n", __FUNCTION__, ## __VA_ARGS__); \
	fflush(stderr); }} while (0)

#else /* not DEBUG */

#define ERR(__FMT, ...)
#define WARN(__FMT, ...)
#define INF(__FMT, ...)
#define DBG(__FMT, ...)
#define DBG1(__FMT, ...)
#define DBG2(__FMT, ...)
#define DBG3(__FMT, ...)
#define DBG4(__FMT, ...)
#define DBG5(__FMT, ...)

#endif /* not DEBUG */

#endif /* not __DEBUG_H__ */

