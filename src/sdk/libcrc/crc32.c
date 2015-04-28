/* 
 * Copyright(c) 2004-2012 BORESTE (www.boreste.com). All Rights Reserved.
 *
 * This file is part of the YARD-ICE
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
 * @file crc32.c
 * @brief YARD-ICE libcrc
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <crc.h>

unsigned long crc32(unsigned long __crc, const void * __buf, int __len)
{
	for(; __len; __len--)
	 	__crc = CRC32(__crc, *(unsigned char *)__buf++);
	 
	return __crc ^ 0xffffffffL;
}

