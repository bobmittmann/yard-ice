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
 * @file trace.h
 * @brief
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __TRACE_H__
#define __TRACE_H__

#include <stdlib.h>
#include <stdint.h>
#include <sys/usb-cdc.h>

#define TIME_ABS  1
#define DUMP_PKT  2
#define SHOW_SUPV 4
#define SHOW_PKT  8

extern uint32_t trace_opt;
extern uint32_t trace_ts;
extern struct usb_cdc_class * usb_cdc;
extern uint32_t protocol_buf[];

#ifdef __cplusplus
extern "C" {
#endif

int tracef(uint32_t ts, const char *fmt, ... );

int trace_printf(const char *fmt, ... );

int xxd(char * s, int max, uint8_t * buf, int len);

int xx_dump(uint32_t ts, uint8_t * buf, int len);

void usb_trace_init(struct usb_cdc_class * cdc);

int usb_printf(struct usb_cdc_class * cdc, const char *fmt, ... );

void raw_trace(uint32_t ts, uint8_t * rx_buf, unsigned int rx_len);

void raw_trace_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __TRACE_H__ */

