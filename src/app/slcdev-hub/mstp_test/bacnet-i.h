/*
 * Copyright(c) 2004-2014 BORESTE (www.boreste.com). All Rights Reserved.
 *
 * This file is part of the libhttpd.
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
 * @file bacnet-i.h
 * @brief BACnet library private header
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __BACNET_I_H__
#define __BACNET_I_H__

#ifdef CONFIG_H
#include "config.h"
#endif

#include <stdint.h>

const uint8_t bacnet_crc8_lut[256];

static inline unsigned int __bacnet_crc8(unsigned int crc, int c) {
	return bacnet_crc8_lut[crc ^ c];
}

#ifdef __cplusplus
extern "C" {
#endif

unsigned int bacnet_crc8(unsigned int crc, const void * buf, int len);

unsigned int bacnet_crc16(unsigned int crc, const void * buf, int len);

#ifdef __cplusplus
}
#endif

#endif /* __BACNET_I_H__ */

