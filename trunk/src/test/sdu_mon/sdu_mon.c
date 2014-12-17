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

#include <sys/stm32f.h>
#include <arch/cortex-m3.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/serial.h>
#include <sys/usb-cdc.h>
#include <sys/param.h>

#include <thinkos.h>

#include <sys/dcclog.h>

#include "profclk.h"
#include "board.h"
#include "led.h"
#include "sdu.h"

#ifndef SDU_INFO
#ifdef DEBUG
#define SDU_INFO 1
#else
#define SDU_INFO 0
#endif
#endif

#ifndef RAW_INFO
#define RAW_INFO 0
#endif

#define SDU_SYNC 0x1b
#define SDU_PKT_LEN_MAX (255 + 3)

#define SDU_SUP  (0x20 >> 5)
#define SDU_CMD  (0x40 >> 5)
#define SDU_ACK  (0x60 >> 5)
#define SDU_RSY  (0xc0 >> 5)
#define SDU_NAK  (0xe0 >> 5)

struct serial_dev * serial2_open(void);
void serial_rx_disable(struct serial_dev * dev);
void serial_rx_enable(struct serial_dev * dev);

struct sdu_link {
	struct {
		bool timeout;
		bool stuff;
		uint32_t pos;
		uint32_t tot_len;
		uint32_t addr;
		uint8_t buf[SDU_PKT_LEN_MAX];
	} rx;
};

uint32_t trace_ts;
uint32_t trace_opt;
struct usb_cdc_class * cdc;

#define TIME_ABS 1
#define DUMP_PKT 2
#define SHOW_SUPV 4

int tracef(uint32_t ts, const char *fmt, ... )
{
	char s[80];
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

	rem = 80 - 2;

	n = sprintf(s, "%2d.%03d.%03d: ", sec, ms, us);
	cp += n;
	rem -= n;

	va_start(ap, fmt);
	n = vsnprintf(cp, rem, fmt, ap);
	DCC_LOG1(LOG_TRACE, "n=%d", n);
	cp += n;
	va_end(ap);

	*cp++ = '\r';
	*cp++ = '\n';

	return usb_cdc_write(cdc, s, cp - s);
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
		n = snprintf(cp, rem, " %02x", buf[i]);
		cp += n;
		rem -= n;
		if (rem <= 0) {
			DCC_LOG1(LOG_TRACE, "rem=%d", rem);
			break;
		}
	}

	if (cnt < len) {
		*cp++ = ' ';
		*cp++ = '.';
		*cp++ = '.';
		*cp++ = '.';
	}

	*cp++ = '\0';
	n = cp - s;

	DCC_LOG1(LOG_TRACE, "n=%d", n);

	return n;
}


int xx_dump(uint32_t ts, uint8_t * buf, int len)
{
	char s[80];
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

	rem = 80 - 3;
	n = sprintf(s, "%2d.%03d.%03d:", sec, ms, us);
	cp += n;
	rem -= n;

	cnt = MIN(len, rem / 3);

	for (i = 0; i < cnt; ++i) {
		n = snprintf(cp, rem, " %02x", buf[i]);
		cp += n;
		rem -= n;
		if (rem == 0)
			break;
	}

	*cp++ = '\r';
	*cp++ = '\n';

	return usb_cdc_write(cdc, s, cp - s);
}

int __tracef(uint32_t ts, const char *fmt, ... )
{
	return 0;
}

const char type_nm[8][4] = {
	"x00",
	"SUP",
	"CMD",
	"ACK",
	"x80",
	"xa0",
	"RSY",
	"NAK" };

void sdu_decode(struct sdu_link * dev, uint8_t * buf, unsigned int buf_len)
{
	uint32_t ts;
	uint8_t * cp;
	uint8_t * msg;
	uint8_t sum;
	uint8_t route;
	uint8_t ctrl;
	int seq;
	bool retry;
	int len;
	int addr;
	int type;
	int j;
	int c;
	int i;

	ts = profclk_get();

	DCC_LOG1(LOG_INFO, "0. len=%d", buf_len);
//	xx_dump(ts, buf, buf_len);

	for (i = 0; i < buf_len; ++i) {
	
		c = buf[i];

		DCC_LOG2(LOG_INFO, "1. i=%d  pos=%d", i, dev->rx.pos);

		/* first char */
		if (dev->rx.pos == 0) {
			if (c != SDU_SYNC) {
				tracef(ts, "sync expected, got: %02x!", c);
				continue;
			}
		} else if (dev->rx.pos == 1) {
			if (c == SDU_SYNC) {
				tracef(ts, "unexpected sync!");
				dev->rx.pos = 0;
				continue;
			}
		} else {
			/* Byte destuffing (remove any syncs from the stream) */
			if ((c == SDU_SYNC) && (!dev->rx.stuff)) {
				dev->rx.stuff = true;
				DCC_LOG(LOG_INFO, "byte stuffing");
				continue;
			}
		}

		dev->rx.stuff = false;

		if (dev->rx.pos == 3) {
			/* Get the total packet lenght */
			dev->rx.tot_len = dev->rx.pos + c + 3;

			if (dev->rx.tot_len > SDU_PKT_LEN_MAX) {
				/* Packet is too large */
				tracef(ts, "too long!");
				dev->rx.pos = 0;
				continue;
			}
		}

		dev->rx.buf[dev->rx.pos++] = c;

		if (dev->rx.pos < 4) 
			continue;

		if (dev->rx.pos < dev->rx.tot_len)
			continue;

		/* restart the position index */
		dev->rx.pos = 0;

		route = dev->rx.buf[1];
		sum = route;
		ctrl = dev->rx.buf[2];
		sum += ctrl;
		len = dev->rx.buf[3];
		sum += len;
		cp = &dev->rx.buf[4];

		msg = cp; 
		for (j = 0; j < len; j++) {
			sum += *cp++;
		}

		sum += *cp;

		if (sum != 0) {
			tracef(ts, "checksum!");
			continue;
		}

		DCC_LOG(LOG_INFO, "2. PKT.");
		led1_flash(1);

		addr = route & 0x1f;
		(void)addr;
		type = (route & 0xe0) >> 5;
		(void)route;
		(void)type;
		(void)msg;

		seq = ctrl & 0x7f;
		(void)seq;
		retry = (ctrl & 0x80) ? true : false;
		(void)retry;

		(void)msg;

		if ((type == SDU_SUP) && !(trace_opt & SHOW_SUPV))
			continue;

		if (len > 0) {
			char xs[64];
			xxd(xs, 52, msg, len);
			tracef(ts, "%2d %s %3d %c %3d:%s", addr, type_nm[type], 
				   seq,  (retry) ? 'R' : '.', len, xs);
		} else {
			tracef(ts, "%2d %s %3d %c   0", addr, type_nm[type], 
				   seq,  (retry) ? 'R' : '.');
		}

		DCC_LOG3(LOG_INFO, "%2d %s %3d", addr, type_nm[type], seq);
	}

	DCC_LOG(LOG_INFO, "4. DONE.");
}

//const char msg[] = "The quick brown fox jumps over the lazy dog!\r\n";

void __attribute__((noreturn)) serial_recv_task(struct serial_dev * serial)
{
	uint8_t buf[32];
	struct sdu_link sdu;
	int len;
	int cnt = 0;

	(void)cnt;

//	for (;;) {
//		usb_cdc_write(cdc, msg, sizeof(msg));
//	}

	memset(&sdu, 0, sizeof(sdu));

	serial_enable(serial);
	serial_rx_disable(serial);

	for (;;) {
//		DCC_LOG1(LOG_INFO, "%d", cnt++);

		len = serial_read(serial, buf, 32, 5000);
		if (len > 0) {
			DCC_LOG1(LOG_INFO, "len=%d", len);
			sdu_decode(&sdu, buf, len);
		} else {
			tracef(profclk_get(), "idle");
		}	
	}
}

void usb_cdc_dump_stat(usb_cdc_class_t * cl);

struct serial_dev * serial;

int usb_printf(const char *fmt, ... )
{
	char s[64];
	char * cp = s;
	va_list ap;

	va_start(ap, fmt);
	cp += vsnprintf(cp, 64 - 1, fmt, ap);
	va_end(ap);

	return usb_cdc_write(cdc, s, cp - s);
}

void show_menu(void)
{
	usb_printf("--- Option:\r\n");
	usb_printf(" [e] enable capture\r\n");
	usb_printf(" [d] disable capture\r\n");
	usb_printf(" [p] packed mode\r\n");
	usb_printf(" [n] normal mode\r\n");
	usb_printf(" [s] show supervisory\r\n");
};

void __attribute__((noreturn)) usb_recv_task(usb_cdc_class_t * cdc)
{
	uint8_t buf[64];
	int len;
	int c;
	int i;

	for (;;) {
		len = usb_cdc_read(cdc, buf, 64, 15000);

		if (len < 0) {
			DCC_LOG(LOG_INFO, "timeout!");
			continue;
		}	

		if (len == 0) {
			DCC_LOG(LOG_TRACE, "len == 0!");
			continue;
		}	

//		usb_cdc_dump_stat(cdc);

		led2_flash(1);

		for (i = 0; i < len; ++i) {
			c = buf[i];
			(void)c;

			DCC_LOG1(LOG_TRACE, "c=%c", c);

			switch (c) {
			case 'e':
				usb_printf("--- SDU capture start ----------------\r\n");
				serial_rx_enable(serial);
				break;
			case 'd':
				serial_rx_disable(serial);
				usb_printf("-------------------\r\n");
				break;
			case 's':
				trace_opt |=  SHOW_SUPV;
				usb_printf("-- show supervisory\r\n");
				break;
			case 'n':
				trace_opt = 0;
				usb_printf("-- normal mode\r\n");
				break;
			default:
				show_menu();
			}
		}
	}
}

void led_task(void);

uint32_t __attribute__((aligned(8))) led_stack[32];
uint32_t __attribute__((aligned(8))) serial_recv_stack[1024];

int main(int argc, char ** argv)
{
	uint64_t esn;

	DCC_LOG_INIT();
	DCC_LOG_CONNECT();

	/* calibrate usecond delay loop */
	cm3_udelay_calibrate();

	DCC_LOG(LOG_INFO, "1. io_init()");
	io_init();

	DCC_LOG(LOG_INFO, "2. thinkos_init()");
	thinkos_init(THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(0));

	leds_init();

	thinkos_thread_create((void *)led_task, (void *)NULL,
						  led_stack, sizeof(led_stack) |
						  THINKOS_OPT_PRIORITY(1) | THINKOS_OPT_ID(1));

	led_flash_all(8);

	serial = serial2_open();

	esn = *((uint64_t *)STM32F_UID);
	DCC_LOG2(LOG_INFO, "ESN=0x%08x%08x", esn >> 32, esn);
	cdc = usb_cdc_init(&stm32f_usb_fs_dev, esn);

	usb_vbus(true);

	DCC_LOG(LOG_TRACE, "1. ...");

	usb_cdc_acm_lc_wait(cdc);

	DCC_LOG(LOG_TRACE, "2. ...");
	profclk_init();

	led_flash_all(2);

	DCC_LOG(LOG_TRACE, "3. ...");

	thinkos_thread_create((void *)serial_recv_task, 
						  (void *)serial,
						  serial_recv_stack, sizeof(serial_recv_stack) |
						  THINKOS_OPT_PRIORITY(2) | THINKOS_OPT_ID(2));

	DCC_LOG(LOG_TRACE, "4. ...");

	for (;;) {
		usb_recv_task(cdc);
//		thinkos_sleep(10);
	}
}

