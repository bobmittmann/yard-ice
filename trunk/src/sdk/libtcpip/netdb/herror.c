/* 
 * Copyright(c) 2004-2012 BORESTE (www.boreste.com). All Rights Reserved.
 *
 * This file is part of the libtcpip.
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
 * @file herror.c
 * @brief 
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifdef __CONFIG__
#include "config.h"
#endif

#include <netdb.h>
#include <errno.h>

#ifdef NETDB_DEBUG
#ifndef DEBUG
#define DEBUG
#endif
#endif
#include <debug.h>

int *__h_errno_location(void)
{
	return &errno;
}

const char * hstrerror(int __err_num)
{
	return NULL;
}

void herror(const char *__str)
{

}

