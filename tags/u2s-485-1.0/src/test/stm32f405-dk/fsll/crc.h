/* 
 * Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
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
 * @brief CRC Library 
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __CRC_H__
#define __CRC16_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

unsigned int crc5(const void *data, size_t len);

unsigned int crc5tbl8(const void *data, size_t len);

unsigned int crc5tbl4(const void *data, size_t len);

unsigned int crc5bwe(const void *data, size_t len);


unsigned int crc8(const void *data, size_t len);

unsigned int crc8tbl8(const void *data, size_t len);

unsigned int crc8tbl4(const void *data, size_t len);

unsigned int crc8bwe(const void *data, size_t len);


unsigned int crc16(const void *data, size_t len);

unsigned int crc16tbl8(const void *data, size_t len);

unsigned int crc16tbl4(const void *data, size_t len);

unsigned int crc16bwe(const void *data, size_t len);


#ifdef  __cplusplus
}
#endif

#endif /* __CRC16_H__ */

