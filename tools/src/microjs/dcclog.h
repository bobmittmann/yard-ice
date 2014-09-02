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

#define LOG_PANIC   1
#define LOG_EXCEPT  2
#define LOG_ERROR   3
#define LOG_WARNING 4
#define LOG_TRACE   5
#define LOG_INFO    6
#define LOG_MSG     7

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

#endif /* __ICE_TRACE_H__ */

