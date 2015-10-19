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
 * @file usb-serial.c
 * @brief
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/usb-cdc.h>
#include <sys/param.h>

#include <thinkos.h>

#include <sys/dcclog.h>

#include "profclk.h"
#include "trace.h"
#include "capture.h"
#include "damp.h"
#include "sdu.h"

uint32_t trace_ts;
uint8_t trace_opt;
struct usb_cdc_class * usb_cdc;

#define TIME_ABS  1
#define DUMP_PKT  2
#define SHOW_SUPV 4
#define SHOW_PKT  8

uint32_t protocol_buf[(20 + 512) / sizeof(uint32_t)];

#define TRACE_MAX 92
char trace_buf[TRACE_MAX + 1];

int tracef(uint32_t ts, const char *fmt, ... )
{
	char * s = trace_buf;
	char * cp = s;
	int32_t dt;
	int32_t sec;
	uint32_t ms;
	uint32_t us;
	va_list ap;
	int rem;
	int n;

	if (trace_opt & TIME_ABS)
		dt = profclk_us(ts);
	else
		dt = profclk_us((int32_t)(ts - trace_ts));

	sec = dt / 1000000;
	dt -= (sec * 1000000);
	ms = dt / 1000;
	us = dt - (ms * 1000);
	trace_ts = ts;

	rem = 126;

	n = sprintf(s, "%2d.%03d.%03d: ", sec, ms, us);
	cp += n;
	rem -= n;

	va_start(ap, fmt);
	n = vsnprintf(cp, rem + 1, fmt, ap);
	n = MIN(n, rem);
	cp += n;
	va_end(ap);

	*cp++ = '\r';
	*cp++ = '\n';

	return usb_cdc_write(usb_cdc, s, cp - s);
}

int xxd(char * s, int max, uint8_t * buf, int len)
{
	char * cp = s;
	int rem;
	int cnt;
	int n;
	int i;

	rem = max - 2;
	cnt = MIN(len, rem / 3);

	DCC_LOG2(LOG_INFO, "len=%d cnt=%d", len, cnt);

	if (cnt < len) 
		cnt--; /* make room for elipses */

	for (i = 0; i < cnt; ++i) {
		snprintf(cp, rem, " %02x", buf[i]);
		if (rem < 3) {
			DCC_LOG1(LOG_MSG, "rem=%d", rem);
			break;
		}
		cp += 3;
		rem -= 3;
	}

	if (cnt < len) {
		*cp++ = ' ';
		*cp++ = '.';
		*cp++ = '.';
		*cp++ = '.';
	}

	*cp++ = '\0';
	n = cp - s;

	DCC_LOG1(LOG_MSG, "n=%d", n);

	return n;
}

int trace_printf(const char *fmt, ... )
{
	char * s = trace_buf;
	va_list ap;
	int n;

	va_start(ap, fmt);
	n = vsnprintf(s, TRACE_MAX + 1, fmt, ap);
	va_end(ap);

	n = MIN(n, TRACE_MAX);

	return usb_cdc_write(usb_cdc, s, n);
}

int usb_printf(const char *fmt, ... )
{
	char * s = trace_buf;
	va_list ap;
	int n;

	va_start(ap, fmt);
	n = vsnprintf(s, TRACE_MAX + 1, fmt, ap);
	va_end(ap);

	n = MIN(n, TRACE_MAX);

	return  usb_cdc_write(usb_cdc, s, n);
}

void trace_raw_pkt(struct packet * pkt)
{
	uint32_t ts = pkt->clk;
	uint8_t * data = pkt->data;
	unsigned int len = pkt->cnt;
	char * s = trace_buf;
	char * cp = s;
	int32_t dt;
	int32_t sec;
	uint32_t ms;
	uint32_t us;
	int rem;
	int cnt;
	int n;
	int i;

	if (trace_opt & TIME_ABS )
		dt = profclk_us(ts);
	else
		dt = profclk_us((int32_t)(ts - trace_ts));

	sec = dt / 1000000;
	dt -= (sec * 1000000);
	ms = dt / 1000;
	us = dt - (ms * 1000);
	trace_ts = ts;

	rem = 80 - 4;
	n = sprintf(s, "%2d.%03d.%03d:", sec, ms, us);
	cp += n;
	rem -= n;

	cnt = MIN(len, rem / 3);

	for (i = 0; i < cnt; ++i) {
		n = sprintf(cp, " %02x", data[i]);
		cp += n;
		rem -= n;
		if (rem == 0)
			break;
	}

	if (i < cnt) {
		if ((i - cnt) == 1)
			n = sprintf(cp, " %02x", data[i]);
		else
			n = sprintf(cp, " ...");
		cp += n;
		rem -= n;
	}

	*cp++ = '\r';
	*cp++ = '\n';

	usb_cdc_write(usb_cdc, s, cp - s);
}

void trace_time_abs(bool en)
{
	uint32_t opt = trace_opt & ~TIME_ABS;
	
	trace_opt = opt | (en ? TIME_ABS : 0);
}

