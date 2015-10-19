/* 
 * Copyright(C) 2013 Robinson Mittmann. All Rights Reserved.
 * 
 * This file is part of the usb-serial converter.
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
 * @file packet.h
 * @brief
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __CAPTURE_H__
#define __CAPTURE_H__

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#define PACKET_DATA_MAX 256

struct packet {
	uint32_t clk;
	uint16_t seq;
	uint16_t cnt;
	uint8_t data[PACKET_DATA_MAX];
};

#ifdef __cplusplus
extern "C" {
#endif

struct packet * capture_pkt_recv(void);

void capture_start(void);

void capture_stop(void);

void capture_init(void);

void capture_idletime_set(unsigned int bits);

void capture_baudrate_set(unsigned int rate);

#ifdef __cplusplus
}
#endif

#endif /* __PACKET_H__ */

