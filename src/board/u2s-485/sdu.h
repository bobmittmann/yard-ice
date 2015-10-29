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
 * @file sdu.h
 * @brief
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __SDU_H__
#define __SDU_H__

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void RX(uint8_t * buf, unsigned int len);
void TX(uint8_t * buf, unsigned int len);

void sdu_trace_init(void);
void sdu_decode(uint32_t ts, uint8_t * buf, unsigned int buf_len);

void sdu_trace_show_supv(bool en);
void sdu_trace_time_abs(bool en);
void sdu_trace_show_pkt(bool en);

#ifdef __cplusplus
}
#endif

#endif /* __SDU_H__ */

