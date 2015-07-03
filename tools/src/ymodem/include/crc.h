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
 * @file crc.h
 * @brief YARD-ICE libcrc
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __CRC_H__
#define __CRC_H__

static inline unsigned int __crc16ccitt(unsigned int crc, int c) {
	crc = (crc >> 8) | ((crc & 0xff) << 8);
	crc ^= c;
	crc ^= (crc & 0xff) >> 4;
	crc ^= (crc & 0x0f) << 12;
	crc ^= (crc & 0xff) << 5;
	return crc;
}

#define CRC16CCITT(CRC, C) __crc16ccitt(CRC, C)

extern const unsigned long _crc32_[256];
#define CRC32(CRC, C) (_crc32_[((CRC) ^ (unsigned int)(C)) & 0xff] ^ \
					   ((CRC) >> 8))

#ifdef __cplusplus
extern "C" {
#endif

unsigned int crc16ccitt(unsigned int __crc, const void * __buf, int __len);

unsigned long crc32(unsigned long __crc, const void * __buf, int __len);

#ifdef __cplusplus
}
#endif	

#endif /* __CRC_H__ */

