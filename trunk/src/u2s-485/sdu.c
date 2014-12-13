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
#include <sys/dcclog.h>
#include <thinkos.h>

#include "profclk.h"

#define SDU_SYNC 0x1b
#define SDU_PKT_LEN_MAX (255 + 3)

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

static struct sdu_link sdu_in;
static struct sdu_link sdu_out;

void sdu_rx(uint8_t * buf, unsigned int buf_len)
{
	struct sdu_link * dev = &sdu_in;
	int c;
	int i;

	for (i = 0; i < buf_len; ++i) {
		c = buf[i];

		/* first char */
		if (dev->rx.pos == 0) {
			if (c != SDU_SYNC) {
				DCC_LOG(LOG_INFO, "sync expected!");
				continue;
			}
		} else if (dev->rx.pos == 1) {
			if (c == SDU_SYNC) {
				DCC_LOG(LOG_ERROR, "unexpected sync!");
				dev->rx.pos = 0;
				continue;
			}
		} else {
			/* Byte destuffing (remove any syncs from the stream) */
			if ((c == SDU_SYNC) && (!dev->rx.stuff)) {
				dev->rx.stuff = true;
				continue;
			}
		}

		dev->rx.stuff = false;

		if (dev->rx.pos == 3) {
			/* Get the total packet lenght */
			dev->rx.tot_len = dev->rx.pos + c + 3;

			if (dev->rx.tot_len > SDU_PKT_LEN_MAX) {
				/* Packet is too large */
				DCC_LOG(LOG_ERROR, "too long!");
				dev->rx.pos = 0;
				continue;
			}
		}

		dev->rx.buf[dev->rx.pos++] = c;

		/*    trace_printf("%s() Rx: 0x%02x", __func__, c); */
		if (dev->rx.pos == dev->rx.tot_len) {
			uint8_t * cp;
			uint8_t * msg;
			uint8_t sum;
			uint8_t route;
			uint8_t ctrl;
			int len;
			int addr;
			int j;

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
				DCC_LOG(LOG_ERROR, "checksum!");
				continue;
			}

			addr = route & 0x1f;
			(void)addr;
			(void)msg;

			if (addr > 0) {
				if (len == 0)
					DCC_LOG1(LOG_INFO, "[%2d]", addr);
				else if (len == 1)
					DCC_LOG2(LOG_TRACE, "[%2d]: %02x", addr, msg[0]);
				else if (len == 2)
					DCC_LOG3(LOG_TRACE, "[%2d]: %02x %02x", addr,
							 msg[0], msg[1]);
				else if (len == 3)
					DCC_LOG4(LOG_TRACE, "[%2d]: %02x %02x %02x", addr,
							 msg[0], msg[1], msg[2]);
				else if (len == 4)
					DCC_LOG5(LOG_TRACE, "[%2d]: %02x %02x %02x %02x", addr,
							 msg[0], msg[1], msg[2], msg[3]);
				else if (len == 5)
					DCC_LOG6(LOG_TRACE, "[%2d]: %02x %02x %02x %02x %02x", 
							 addr, msg[0], msg[1], msg[2], msg[3], msg[4]);
				else if (len == 6)
					DCC_LOG7(LOG_TRACE, "[%2d]: %02x %02x %02x %02x %02x %02x", 
							 addr, msg[0], msg[1], msg[2], msg[3], msg[4], msg[5]);
				else if (len == 7)
					DCC_LOG8(LOG_TRACE, "[%2d]: %02x %02x %02x %02x %02x %02x %02x",
							 addr, msg[0], msg[1], msg[2], msg[3], 
							 msg[4], msg[5], msg[6]);
				else
					DCC_LOG9(LOG_TRACE, "[%2d]: %02x %02x %02x %02x %02x %02x "
							 "%02x %02x ...", addr, msg[0], msg[1], msg[2], msg[3], 
							 msg[4], msg[5], msg[6], msg[7]);

			}
		}
	}
}

void sdu_tx(uint8_t * buf, unsigned int buf_len)
{
	struct sdu_link * dev = &sdu_out;
	int c;
	int i;

	for (i = 0; i < buf_len; ++i) {
		c = buf[i];

		/* first char */
		if (dev->rx.pos == 0) {
			if (c != SDU_SYNC) {
				DCC_LOG(LOG_INFO, "sync expected!");
				continue;
			}
		} else if (dev->rx.pos == 1) {
			if (c == SDU_SYNC) {
				DCC_LOG(LOG_ERROR, "unexpected sync!");
				dev->rx.pos = 0;
				continue;
			}
		} else {
			/* Byte destuffing (remove any syncs from the stream) */
			if ((c == SDU_SYNC) && (!dev->rx.stuff)) {
				dev->rx.stuff = true;
				continue;
			}
		}

		dev->rx.stuff = false;

		if (dev->rx.pos == 3) {
			/* Get the total packet lenght */
			dev->rx.tot_len = dev->rx.pos + c + 3;
			if (dev->rx.tot_len > SDU_PKT_LEN_MAX) {
				/* Packet is too large */
				DCC_LOG(LOG_ERROR, "too long!");
				dev->rx.pos = 0;
				continue;
			}
		}

		dev->rx.buf[dev->rx.pos++] = c;

		/*    trace_printf("%s() Rx: 0x%02x", __func__, c); */
		if (dev->rx.pos == dev->rx.tot_len) {
			uint8_t * cp;
			uint8_t * msg;
			uint8_t sum;
			uint8_t route;
			uint8_t ctrl;
			int len;
			int addr;
			int j;

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
				DCC_LOG(LOG_ERROR, "checksum!");
				continue;
			}

			addr = route & 0x1f;
			(void)addr;
			(void)msg;

			if (len == 0)
				DCC_LOG1(LOG_TRACE, "[%2d]", addr);
			else if (len == 1)
				DCC_LOG2(LOG_TRACE, "[%2d]: %02x", addr, msg[0]);
			else if (len == 2)
				DCC_LOG3(LOG_TRACE, "[%2d]: %02x %02x", addr,
						 msg[0], msg[1]);
			else if (len == 3)
				DCC_LOG4(LOG_TRACE, "[%2d]: %02x %02x %02x", addr,
						 msg[0], msg[1], msg[2]);
			else if (len == 4)
				DCC_LOG5(LOG_TRACE, "[%2d]: %02x %02x %02x %02x", addr,
						 msg[0], msg[1], msg[2], msg[3]);
			else if (len == 5)
				DCC_LOG6(LOG_TRACE, "[%2d]: %02x %02x %02x %02x %02x", 
						 addr, msg[0], msg[1], msg[2], msg[3], msg[4]);
			else if (len == 6)
				DCC_LOG7(LOG_TRACE, "[%2d]: %02x %02x %02x %02x %02x %02x", 
						 addr, msg[0], msg[1], msg[2], msg[3], msg[4], msg[5]);
			else if (len == 7)
				DCC_LOG8(LOG_TRACE, "[%2d]: %02x %02x %02x %02x %02x %02x %02x",
						 addr, msg[0], msg[1], msg[2], msg[3], 
						 msg[4], msg[5], msg[6]);
			else
				DCC_LOG9(LOG_TRACE, "[%2d]: %02x %02x %02x %02x %02x %02x "
						 "%02x %02x ...", addr, msg[0], msg[1], msg[2], msg[3], 
						 msg[4], msg[5], msg[6], msg[7]);

		}
	}
}

