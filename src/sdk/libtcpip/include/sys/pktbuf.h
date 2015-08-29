/* 
 * Copyright(C) 2015 Bob Mittmann. All Rights Reserved.
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
 * @file pktbuf.c
 * @brief
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __SYS_PKTBUF_H__
#define __SYS_PKTBUF_H__

extern const uint16_t pktbuf_len;

#ifdef __cplusplus
extern "C" {
#endif

void * pktbuf_alloc(void);

void pktbuf_free(void * ptr);

void pktbuf_pool_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __PKTBUF_H__ */

