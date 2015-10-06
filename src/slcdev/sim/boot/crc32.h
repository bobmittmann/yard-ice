/* 
 * Copyright(c) 2004-2012 BORESTE (www.boreste.com). All Rights Reserved.
 *
 * This file is part of the libcrc.
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
 * @file crc32.h
 * @brief YARD-ICE libcrc
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <stdint.h>

#ifndef __CRC32_H__
#define __CRC32_H__

#ifdef __cplusplus
extern "C" {
#endif

uint32_t crc32(void  * buf, unsigned int len);

uint32_t crc32_align(void  * buf, unsigned int len);

#ifdef __cplusplus
}
#endif	

#endif /* __CRC32_H__ */

