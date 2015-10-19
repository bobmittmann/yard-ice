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

#include "trace.h"
#include "profclk.h"
#include "sdu.h"

#define SDU_SYNC 0x1b
#define SDU_PKT_LEN_MAX (255 + 3)

#define SDU_SUP  (0x20 >> 5)
#define SDU_CMD  (0x40 >> 5)
#define SDU_ACK  (0x60 >> 5)
#define SDU_RSY  (0xc0 >> 5)
#define SDU_NAK  (0xe0 >> 5)

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

const char type_nm[8][4] = {
	"x00",
	"SUP",
	"CMD",
	"ACK",
	"x80",
	"xa0",
	"RSY",
	"NAK" };

void sdu_decode(uint32_t ts, uint8_t * buf, unsigned int buf_len)
{
	struct sdu_link * dev = (struct sdu_link *)&protocol_buf;
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

	DCC_LOG1(LOG_INFO, "0. len=%d", buf_len);

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
		
			if (trace_opt & SHOW_PKT) {
				int n;

				n = MIN(len, 16);
				xxd(xs, 52, msg, n);
				tracef(ts, "%2d %s %3d %c %3d:%s", addr, type_nm[type], 
					   seq,  (retry) ? 'R' : '.', len, xs);

				len -= n;
				msg += n;
#if 1
				while (len) {
					n = MIN(len, 16);
					xxd(xs, 52, msg, n);
					trace_printf("                            :%s\r\n", xs);
					len -= n;
					msg += n;
				}
#endif
			} else {
				xxd(xs, 52, msg, len);
				tracef(ts, "%2d %s %3d %c %3d:%s", addr, type_nm[type], 
					   seq,  (retry) ? 'R' : '.', len, xs);
			}

		} else {
			tracef(ts, "%2d %s %3d %c   0", addr, type_nm[type], 
				   seq,  (retry) ? 'R' : '.');
		}

		DCC_LOG3(LOG_INFO, "%2d %s %3d", addr, type_nm[type], seq);
	}

	DCC_LOG(LOG_INFO, "4. DONE.");
}

void sdu_trace_show_supv(bool en)
{
	uint32_t opt = trace_opt & ~SHOW_SUPV;
	
	trace_opt = opt | (en ? SHOW_SUPV : 0);
}

void sdu_trace_time_abs(bool en)
{
	uint32_t opt = trace_opt & ~TIME_ABS;
	
	trace_opt = opt | (en ? TIME_ABS : 0);
}

void sdu_trace_show_pkt(bool en)
{
	uint32_t opt = trace_opt & ~SHOW_PKT;
	
	trace_opt = opt | (en ? SHOW_PKT : 0);
}

void sdu_trace_init(void)
{
	struct sdu_link * lnk = (struct sdu_link *)&protocol_buf;
	lnk->rx.timeout = false;
	lnk->rx.stuff = 0;
	lnk->rx.pos = 0;
	lnk->rx.tot_len = 0;
	lnk->rx.addr = 0;
	tracef(profclk_get(), "--- SDU trace ---------"); 
}

void trace_sdu_pkt(struct packet * pkt)
{
	sdu_decode(pkt->clk, pkt->data, pkt->cnt);
}

