/* 
 * Copyright(c) 2004-2012 BORESTE (www.boreste.com). All Rights Reserved.
 *
 * This file is part of the libmd5.
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
 * @file md5.h
 * @brief YARD-ICE libmd5
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __MD5_H__
#define __MD5_H__

#include <stdint.h>

struct md5ctx {
	uint32_t lo, hi;
	uint32_t a, b, c, d;
	uint8_t buffer[64];
	uint32_t block[16];
};

extern void md5_init(struct md5ctx *ctx);
extern void md5_update(struct md5ctx *ctx, const void *data, 
					   unsigned long size);
extern void md5_final(uint8_t * result, struct md5ctx *ctx);

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif	

#endif /* __MD5_H__ */

