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
 * @file mstp.c
 * @brief
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/param.h>
#include <sys/dcclog.h>

#include "damp.h"
#include "trace.h"
#include "profclk.h"

#include "damp.h"

#define DAMP_SYNC 0x1b
#define DAMP_PKT_LEN_MAX (32 + 3)

#define DAMP_SUP  (0x20 >> 5)
#define DAMP_CMD  (0x40 >> 5)
#define DAMP_ACK  (0x60 >> 5)
#define DAMP_RSY  (0xc0 >> 5)
#define DAMP_NAK  (0xe0 >> 5)

struct damp_link {
	struct {
		bool timeout;
		bool stuff;
		uint32_t pos;
		uint32_t tot_len;
		uint32_t addr;
		uint8_t buf[DAMP_PKT_LEN_MAX];
	} rx;
};

static const char type_nm[8][4] = {
	"x00",
	"SUP",
	"CMD",
	"ACK",
	"x80",
	"xa0",
	"RSY",
	"NAK" };

struct damp_link  * damp_buf;


#define CMD_FLASH_ON                       0x10
#define CMD_FLASH_OFF                      0x11
#define CMD_PAGE_CONTROL                   0x12

#define CMD_GET_VER                        0x84
#define CMD_LED_RATE                       0x80
#define CMD_LAMP_TEST                      0x90

#define CMD_HIBERNATE_AMPLIFIER            0x88
#define CMD_ACTIVATE_AMPLIFIER_SUPPLY      0x8c
#define CMD_RETURN_LINE_SUPERVISION        0x94
#define CMD_SET_SUPERVISION_COMPARATOR     0x98
#define CMD_START_SIGNAL_TRANSITION_COUNT  0x9c
#define CMD_RETURN_SIGNAL_TRANSITION_COUNT 0xa0
#define CMD_RETURN_EMERGENCY_SIGNAL_STATUS 0xa4
#define CMD_RETURN_AMPLIFIER_STATE         0xa8
#define CMD_SPEAKER_OUTPUT_CONNECTION      0xac
#define CMD_AMPLIFIER_TRANSFER             0Xb0
#define CMD_SELECT_AMPLIFIER_SIGNAL        0xb8
#define CMD_RETURN_SIGNAL_SELECTED         0xbc
#define CMD_AMPLIFIER_GAIN_CONTROL         0xc0
#define CMD_SHORT_CIRCUIT_CONTROL          0xc4
#define CMD_VOLTAGE_SELECT                 0xc8
#define CMD_SET_INTERNAL_SUPERVISION_LVL   0xd0
#define CMD_RETURN_SIGNAL_PEAK_COUNT       0xd8

#define CMD_CHRG_CTRL                       0x03

#define CHARGER_OFF_NO_LOAD 0x01
#define CHARGER_OFF_LOADED  0x02
#define CHARGER_ON_NO_LOAD  0x03
#define CHARGER_ON_LOADED   0x04

#define CMD_XFER_TO_BAT_CTRL                0x05
#define CMD_SET_AUTO_XFER                   0x07
#define CMD_SET_CHRG_CURRENT                0x08
#define CMD_RETURN_MEASUREMENT              0x09
#define CMD_SET_CHRG_SUPERVISION_LEVEL      0x0a
#define CMD_RESET_CHRG_SUPERVISION_COUNTERS 0x0b
#define CMD_SET_CHRG_VOLTAGE                0x0

void damp_decode(uint32_t ts, uint8_t * buf, unsigned int buf_len)
{
	struct damp_link * dev = damp_buf; 
	char xs[64];
	uint8_t * cp;
	uint8_t * msg;
	uint8_t sum;
	uint8_t addr;
	int len;
	int cmd;
	int j;
	int c;
	int i;

	DCC_LOG1(LOG_INFO, "0. len=%d", buf_len);

	for (i = 0; i < buf_len; ++i) {
	
		c = buf[i];

		DCC_LOG2(LOG_INFO, "1. i=%d  pos=%d", i, dev->rx.pos);

		/* first char */
		if (dev->rx.pos == 0) {
			if (c != DAMP_SYNC) {
				tracef(ts, "sync expected, got: %02x!", c);
				continue;
			}
		} else if (dev->rx.pos == 1) {
			if (c == DAMP_SYNC) {
				tracef(ts, "unexpected sync!");
				dev->rx.pos = 0;
				continue;
			}
		} else {
			/* Byte destuffing (remove any syncs from the stream) */
			if ((c == DAMP_SYNC) && (!dev->rx.stuff)) {
				dev->rx.stuff = true;
				DCC_LOG(LOG_INFO, "byte stuffing");
				continue;
			}
		}

		dev->rx.stuff = false;

		if (dev->rx.pos == 2) {
			/* Get the total packet lenght */
			dev->rx.tot_len = dev->rx.pos + c + 2;

			if (dev->rx.tot_len > DAMP_PKT_LEN_MAX) {
				/* Packet is too large */
				tracef(ts, "too long!");
				dev->rx.pos = 0;
				continue;
			}
		}

		dev->rx.buf[dev->rx.pos++] = c;

		if (dev->rx.pos < 3) 
			continue;

		if (dev->rx.pos < dev->rx.tot_len)
			continue;

		/* restart the position index */
		dev->rx.pos = 0;

		addr = dev->rx.buf[1];
		sum = addr;
		len = dev->rx.buf[2];
		sum += len;
		cp = &dev->rx.buf[3];

		msg = cp; 
		for (j = 0; j < len; j++)
			sum += *cp++;

		sum += *cp;
		if (sum != 0) {
			tracef(ts, "checksum!");
			continue;
		}

		if (addr == 0xfd) {
			xxd(xs, 32, msg, len);
			tracef(ts, "< AMP(%d): %s", len, xs);
			continue;
		}

		if (addr == 0xfe) {
			xxd(xs, 32, msg, len);
			tracef(ts, "< CHG(%d): %s", len, xs);
			continue;
		}

		cmd = msg[0];

		switch (cmd) {
		case CMD_FLASH_ON:
			tracef(ts, "> %02x LED ON", addr);
			break;
		case CMD_FLASH_OFF:
			tracef(ts, "> %02x LED OFF", addr);
			break;
		case CMD_PAGE_CONTROL:
			tracef(ts, "> %02x PAG CTL", addr);
			break;

		case CMD_GET_VER:
			tracef(ts, "> %02x GET VER", addr);
			break;
		case CMD_LED_RATE:
			tracef(ts, "> %02x LED RATE", addr);
			break;
		case CMD_LAMP_TEST:
			tracef(ts, "> %02x LAMP TST", addr);
			break;

		case CMD_HIBERNATE_AMPLIFIER:
			tracef(ts, "> %02x HIBERN", addr);
			break;
		case CMD_ACTIVATE_AMPLIFIER_SUPPLY:
			tracef(ts, "> %02x PWR", addr);
			break;
		case CMD_RETURN_LINE_SUPERVISION:
			tracef(ts, "> %02x LIN SPV", addr);
			break;
		case CMD_SET_SUPERVISION_COMPARATOR:
			tracef(ts, "> %02x SET SPV CMP", addr);
			break;
		case CMD_START_SIGNAL_TRANSITION_COUNT:
			tracef(ts, "> %02x CNT START", addr);
			break;
		case CMD_RETURN_SIGNAL_TRANSITION_COUNT:
			tracef(ts, "> %02x CNT GET", addr);
			break;
		case CMD_RETURN_EMERGENCY_SIGNAL_STATUS:
			tracef(ts, "> %02x EMGCY", addr);
			break;
		case CMD_RETURN_AMPLIFIER_STATE:
			tracef(ts, "> %02x AMP STATE", addr);
			break;
		case CMD_SPEAKER_OUTPUT_CONNECTION:
			tracef(ts, "> %02x SPK CON", addr);
			break;
		case CMD_AMPLIFIER_TRANSFER:
			tracef(ts, "> %02x AMP XFER", addr);
			break;
		case CMD_SELECT_AMPLIFIER_SIGNAL:
			tracef(ts, "> %02x AMP SIG SEL", addr);
			break;
		case CMD_RETURN_SIGNAL_SELECTED:
			tracef(ts, "> %02x AMP SIG GET", addr);
			break;
		case CMD_AMPLIFIER_GAIN_CONTROL:
			tracef(ts, "> %02x AMP GAIN", addr);
			break;
		case CMD_SHORT_CIRCUIT_CONTROL:
			tracef(ts, "> %02x SHORT CTL", addr);
			break;
		case CMD_VOLTAGE_SELECT:
			tracef(ts, "> %02x VOUT ", addr);
			break;
		case CMD_SET_INTERNAL_SUPERVISION_LVL:
			tracef(ts, "> %02x SPV LVL", addr);
			break;
		case CMD_RETURN_SIGNAL_PEAK_COUNT:
			tracef(ts, "> %02x PEAK CNT", addr);
			break;

		case CMD_CHRG_CTRL:
			tracef(ts, "> %02x CHRG CTRL", addr);
			break;
		case CMD_XFER_TO_BAT_CTRL:
			tracef(ts, "> %02x CHRG BAT XFER", addr);
			break;
		case CMD_SET_AUTO_XFER:
			tracef(ts, "> %02x CHRG AUTO XFER", addr);
			break;
		case CMD_SET_CHRG_CURRENT:
			tracef(ts, "> %02x CHRG I SET", addr);
			break;
		case CMD_RETURN_MEASUREMENT:
			tracef(ts, "> %02x CHRG VAL", addr);
			break;
		case CMD_SET_CHRG_SUPERVISION_LEVEL:
			tracef(ts, "> %02x CHRG SPV LVL", addr);
			break;
		case CMD_RESET_CHRG_SUPERVISION_COUNTERS:
			tracef(ts, "> %02x CHRG SPV RST", addr);
			break;
		case CMD_SET_CHRG_VOLTAGE:
			tracef(ts, "> %02x CHRG V", addr);
			break;

		default:
			xxd(xs, 64, msg, len);
			tracef(ts, "> %02x ???: %s", addr, xs);
		}
	}
}

void damp_trace_show_supv(bool en)
{
	uint32_t opt = trace_opt & ~SHOW_SUPV;
	
	trace_opt = opt | (en ? SHOW_SUPV : 0);
}

void damp_trace_show_pkt(bool en)
{
	uint32_t opt = trace_opt & ~SHOW_PKT;
	
	trace_opt = opt | (en ? SHOW_PKT : 0);
}

void damp_trace_init(void * buf)
{
	struct damp_link * lnk = buf;

	damp_buf = buf; 
	lnk->rx.timeout = false;
	lnk->rx.stuff = 0;
	lnk->rx.pos = 0;
	lnk->rx.tot_len = 0;
	lnk->rx.addr = 0;
	tracef(profclk_get(), "--- DAMP trace ---------"); 
}

