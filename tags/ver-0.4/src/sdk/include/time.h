/* 
 * Copyright(c) 2004-2012 BORESTE (www.boreste.com). All Rights Reserved.
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
 * @file time.h
 * @brief YARD-ICE libc
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __TIME_H__
#define __TIME_H__

#include <sys/types.h>

struct timespec {
	time_t  tv_sec;   /* Seconds */
	long    tv_nsec;  /* Nanoseconds */
};

#ifdef __cplusplus
extern "C" {
#endif
	
/* Return the current time and put it in *TIMER if TIMER is not NULL.  */
time_t time(time_t * __timer);

#ifdef __cplusplus
}
#endif

#endif /* __TIME_H__ */

