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
#define SDU_SUP  0x20
#define SDU_CMD  0x40
#define SDU_ACK  0x60
#define SDU_RSY  0xc0
#define SDU_NAK  0xe0

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

void TX(uint8_t * buf, unsigned int buf_len)
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

		if (dev->rx.pos < 4)
			continue;

		/*    trace_printf("%s() Rx: 0x%02x", __func__, c); */
		if (dev->rx.pos == dev->rx.tot_len) {
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
			type = route & 0xe0;
			(void)route;
			(void)type;
			(void)msg;

			seq = ctrl & 0x7f;
			(void)seq;
			retry = (ctrl & 0x80) ? true : false;
			(void)retry;

			(void)msg;
			
			if ((len == 0) && (type = SDU_SUP)) {
				continue;
			}

			if (retry) {
				if (len == 0) {
					switch (type) {
					case 0x20:
						DCC_LOG2(LOG_WARNING, "SUP %2d R %3d", addr, seq);
						break;
					case 0x40:
						DCC_LOG2(LOG_WARNING, "CMD %2d R %3d", addr, seq);
						break;
					case 0x60:
						DCC_LOG2(LOG_WARNING, "ACK %2d R %3d", addr, seq);
						break;
					case 0xC0:
						DCC_LOG2(LOG_WARNING, "SYN %2d R %3d", addr, seq);
						break;
					case 0xe0:
						DCC_LOG2(LOG_WARNING, "NAK %2d R %3d", addr, seq);
						break;
					} 
					continue;
				} 

				if (len == 1) {
					switch (type) {
					case 0x20:
						DCC_LOG3(LOG_WARNING, "SUP %2d R %3d: %02x", addr, seq, msg[0]);
						break;
					case 0x40:
						DCC_LOG3(LOG_WARNING, "CMD %2d R %3d: %02x", addr, seq, msg[0]);
						break;
					case 0x60:
						DCC_LOG3(LOG_WARNING, "ACK %2d R %3d: %02x", addr, seq, msg[0]);
						break;
					case 0xC0:
						DCC_LOG3(LOG_WARNING, "SYN %2d R %3d: %02x", addr, seq, msg[0]);
						break;
					case 0xe0:
						DCC_LOG3(LOG_WARNING, "NAK %2d R %3d: %02x", addr, seq, msg[0]);
						break;
					}

					continue;
				}

				if (len == 2) {
					switch (type) {
					case 0x20:
						DCC_LOG4(LOG_WARNING, "SUP %2d R %3d: %02x %02x", 
								 addr, seq, msg[0], msg[1]);
						break;
					case 0x40:
						DCC_LOG4(LOG_WARNING, "CMD %2d R %3d: %02x %02x", 
								 addr, seq, msg[0], msg[1]);
						break;
					case 0x60:
						DCC_LOG4(LOG_WARNING, "ACK %2d R %3d: %02x %02x", 
								 addr, seq, msg[0], msg[1]);
						break;
					case 0xC0:
						DCC_LOG4(LOG_WARNING, "SYN %2d R %3d: %02x %02x", 
								 addr, seq, msg[0], msg[1]);
						break;
					case 0xe0:
						DCC_LOG4(LOG_WARNING, "NAK %2d R %3d: %02x %02x", 
								 addr, seq, msg[0], msg[1]);
						break;
					}

					continue;
				}

				if (len == 3) {
					switch (type) {
					case 0x20:
						DCC_LOG5(LOG_WARNING, "SUP %2d R %3d: %02x %02x %02x", 
								 addr, seq, msg[0], msg[1], msg[2]);
						break;
					case 0x40:
						DCC_LOG5(LOG_WARNING, "CMD %2d R %3d: %02x %02x %02x", 
								 addr, seq, msg[0], msg[1], msg[2]);
						break;
					case 0x60:
						DCC_LOG5(LOG_WARNING, "ACK %2d R %3d: %02x %02x %02x", 
								 addr, seq, msg[0], msg[1], msg[2]);
						break;
					case 0xC0:
						DCC_LOG5(LOG_WARNING, "SYN %2d R %3d: %02x %02x %02x", 
								 addr, seq, msg[0], msg[1], msg[2]);
						break;
					case 0xe0:
						DCC_LOG5(LOG_WARNING, "NAK %2d R %3d: %02x %02x %02x", 
								 addr, seq, msg[0], msg[1], msg[2]);
						break;
					}

					continue;
				}

				if (len == 4) {
					switch (type) {
					case 0x20:
						DCC_LOG6(LOG_WARNING, "SUP %2d R %3d: %02x %02x %02x %02x", 
								 addr, seq, msg[0], msg[1], msg[2], msg[3]);
						break;
					case 0x40:
						DCC_LOG6(LOG_WARNING, "CMD %2d R %3d: %02x %02x %02x %02x", 
								 addr, seq, msg[0], msg[1], msg[2], msg[3]);
						break;
					case 0x60:
						DCC_LOG6(LOG_WARNING, "ACK %2d R %3d: %02x %02x %02x %02x", 
								 addr, seq, msg[0], msg[1], msg[2], msg[3]);
						break;
					case 0xC0:
						DCC_LOG6(LOG_WARNING, "SYN %2d R %3d: %02x %02x %02x %02x", 
								 addr, seq, msg[0], msg[1], msg[2], msg[3]);
						break;
					case 0xe0:
						DCC_LOG6(LOG_WARNING, "NAK %2d R %3d: %02x %02x %02x %02x", 
								 addr, seq, msg[0], msg[1], msg[2], msg[3]);
						break;
					}

					continue;
				}

				if (len == 5) {
					switch (type) {
					case 0x20:
						DCC_LOG7(LOG_WARNING, "SUP %2d R %3d: "
								 "%02x %02x %02x %02x %02x", 
								 addr, seq, msg[0], msg[1], msg[2], msg[3], msg[4]);
						break;
					case 0x40:
						DCC_LOG7(LOG_WARNING, "CMD %2d R %3d: "
								 "%02x %02x %02x %02x %02x", 
								 addr, seq, msg[0], msg[1], msg[2], msg[3], msg[4]);
						break;
					case 0x60:
						DCC_LOG7(LOG_WARNING, "ACK %2d R %3d: "
								 "%02x %02x %02x %02x %02x", 
								 addr, seq, msg[0], msg[1], msg[2], msg[3], msg[4]);
						break;
					case 0xC0:
						DCC_LOG7(LOG_WARNING, "SYN %2d R %3d: "
								 "%02x %02x %02x %02x %02x", 
								 addr, seq, msg[0], msg[1], msg[2], msg[3], msg[4]);
						break;
					case 0xe0:
						DCC_LOG7(LOG_WARNING, "NAK %2d R %3d: "
								 "%02x %02x %02x %02x %02x", 
								 addr, seq, msg[0], msg[1], msg[2], msg[3], msg[4]);
						break;
					}

					continue;
				}

				switch (type) {
				case 0x20:
					DCC_LOG8(LOG_WARNING, "SUP %2d R %3d: "
							 "%02x %02x %02x %02x %02x %02x ...", 
							 addr, seq, msg[0], msg[1], msg[2], msg[3], msg[4], msg[5]);
					break;
				case 0x40:
					DCC_LOG8(LOG_WARNING, "CMD %2d R %3d: "
							 "%02x %02x %02x %02x %02x %02x ...", 
							 addr, seq, msg[0], msg[1], msg[2], msg[3], msg[4], msg[5]);
					break;
				case 0x60:
					DCC_LOG8(LOG_WARNING, "ACK %2d R %3d: "
							 "%02x %02x %02x %02x %02x %02x ...", 
							 addr, seq, msg[0], msg[1], msg[2], msg[3], msg[4], msg[5]);
					break;
				case 0xC0:
					DCC_LOG8(LOG_WARNING, "SYN %2d R %3d: "
							 "%02x %02x %02x %02x %02x %02x ...", 
							 addr, seq, msg[0], msg[1], msg[2], msg[3], msg[4], msg[5]);
					break;
				case 0xe0:
					DCC_LOG8(LOG_WARNING, "NAK %2d R %3d: "
							 "%02x %02x %02x %02x %02x %02x ...", 
							 addr, seq, msg[0], msg[1], msg[2], msg[3], msg[4], msg[5]);
					break;
				}

			} else {
				if (len == 0) {
					switch (type) {
					case 0x20:
						DCC_LOG2(LOG_TRACE, "SUP %2d . %3d", addr, seq);
						break;
					case 0x40:
						DCC_LOG2(LOG_TRACE, "CMD %2d . %3d", addr, seq);
						break;
					case 0x60:
						DCC_LOG2(LOG_TRACE, "ACK %2d . %3d", addr, seq);
						break;
					case 0xC0:
						DCC_LOG2(LOG_TRACE, "SYN %2d . %3d", addr, seq);
						break;
					case 0xe0:
						DCC_LOG2(LOG_TRACE, "NAK %2d . %3d", addr, seq);
						break;
					} 
					continue;
				} 

				if (len == 1) {
					switch (type) {
					case 0x20:
						DCC_LOG3(LOG_TRACE, "SUP %2d . %3d: %02x", addr, seq, msg[0]);
						break;
					case 0x40:
						DCC_LOG3(LOG_TRACE, "CMD %2d . %3d: %02x", addr, seq, msg[0]);
						break;
					case 0x60:
						DCC_LOG3(LOG_TRACE, "ACK %2d . %3d: %02x", addr, seq, msg[0]);
						break;
					case 0xC0:
						DCC_LOG3(LOG_TRACE, "SYN %2d . %3d: %02x", addr, seq, msg[0]);
						break;
					case 0xe0:
						DCC_LOG3(LOG_TRACE, "NAK %2d . %3d: %02x", addr, seq, msg[0]);
						break;
					}

					continue;
				}

				if (len == 2) {
					switch (type) {
					case 0x20:
						DCC_LOG4(LOG_TRACE, "SUP %2d . %3d: %02x %02x", 
								 addr, seq, msg[0], msg[1]);
						break;
					case 0x40:
						DCC_LOG4(LOG_TRACE, "CMD %2d . %3d: %02x %02x", 
								 addr, seq, msg[0], msg[1]);
						break;
					case 0x60:
						DCC_LOG4(LOG_TRACE, "ACK %2d . %3d: %02x %02x", 
								 addr, seq, msg[0], msg[1]);
						break;
					case 0xC0:
						DCC_LOG4(LOG_TRACE, "SYN %2d . %3d: %02x %02x", 
								 addr, seq, msg[0], msg[1]);
						break;
					case 0xe0:
						DCC_LOG4(LOG_TRACE, "NAK %2d . %3d: %02x %02x", 
								 addr, seq, msg[0], msg[1]);
						break;
					}

					continue;
				}

				if (len == 3) {
					switch (type) {
					case 0x20:
						DCC_LOG5(LOG_TRACE, "SUP %2d . %3d: %02x %02x %02x", 
								 addr, seq, msg[0], msg[1], msg[2]);
						break;
					case 0x40:
						DCC_LOG5(LOG_TRACE, "CMD %2d . %3d: %02x %02x %02x", 
								 addr, seq, msg[0], msg[1], msg[2]);
						break;
					case 0x60:
						DCC_LOG5(LOG_TRACE, "ACK %2d . %3d: %02x %02x %02x", 
								 addr, seq, msg[0], msg[1], msg[2]);
						break;
					case 0xC0:
						DCC_LOG5(LOG_TRACE, "SYN %2d . %3d: %02x %02x %02x", 
								 addr, seq, msg[0], msg[1], msg[2]);
						break;
					case 0xe0:
						DCC_LOG5(LOG_TRACE, "NAK %2d . %3d: %02x %02x %02x", 
								 addr, seq, msg[0], msg[1], msg[2]);
						break;
					}

					continue;
				}

				if (len == 4) {
					switch (type) {
					case 0x20:
						DCC_LOG6(LOG_TRACE, "SUP %2d . %3d: %02x %02x %02x %02x", 
								 addr, seq, msg[0], msg[1], msg[2], msg[3]);
						break;
					case 0x40:
						DCC_LOG6(LOG_TRACE, "CMD %2d . %3d: %02x %02x %02x %02x", 
								 addr, seq, msg[0], msg[1], msg[2], msg[3]);
						break;
					case 0x60:
						DCC_LOG6(LOG_TRACE, "ACK %2d . %3d: %02x %02x %02x %02x", 
								 addr, seq, msg[0], msg[1], msg[2], msg[3]);
						break;
					case 0xC0:
						DCC_LOG6(LOG_TRACE, "SYN %2d . %3d: %02x %02x %02x %02x", 
								 addr, seq, msg[0], msg[1], msg[2], msg[3]);
						break;
					case 0xe0:
						DCC_LOG6(LOG_TRACE, "NAK %2d . %3d: %02x %02x %02x %02x", 
								 addr, seq, msg[0], msg[1], msg[2], msg[3]);
						break;
					}

					continue;
				}

				if (len == 5) {
					switch (type) {
					case 0x20:
						DCC_LOG7(LOG_TRACE, "SUP %2d . %3d: "
								 "%02x %02x %02x %02x %02x", 
								 addr, seq, msg[0], msg[1], msg[2], msg[3], msg[4]);
						break;
					case 0x40:
						DCC_LOG7(LOG_TRACE, "CMD %2d . %3d: "
								 "%02x %02x %02x %02x %02x", 
								 addr, seq, msg[0], msg[1], msg[2], msg[3], msg[4]);
						break;
					case 0x60:
						DCC_LOG7(LOG_TRACE, "ACK %2d . %3d: "
								 "%02x %02x %02x %02x %02x", 
								 addr, seq, msg[0], msg[1], msg[2], msg[3], msg[4]);
						break;
					case 0xC0:
						DCC_LOG7(LOG_TRACE, "SYN %2d . %3d: "
								 "%02x %02x %02x %02x %02x", 
								 addr, seq, msg[0], msg[1], msg[2], msg[3], msg[4]);
						break;
					case 0xe0:
						DCC_LOG7(LOG_TRACE, "NAK %2d . %3d: "
								 "%02x %02x %02x %02x %02x", 
								 addr, seq, msg[0], msg[1], msg[2], msg[3], msg[4]);
						break;
					}

					continue;
				}

				switch (type) {
				case 0x20:
					DCC_LOG8(LOG_TRACE, "SUP %2d . %3d: "
							 "%02x %02x %02x %02x %02x %02x ...", 
							 addr, seq, msg[0], msg[1], msg[2], msg[3], msg[4], msg[5]);
					break;
				case 0x40:
					DCC_LOG8(LOG_TRACE, "CMD %2d . %3d: "
							 "%02x %02x %02x %02x %02x %02x ...", 
							 addr, seq, msg[0], msg[1], msg[2], msg[3], msg[4], msg[5]);
					break;
				case 0x60:
					DCC_LOG8(LOG_TRACE, "ACK %2d . %3d: "
							 "%02x %02x %02x %02x %02x %02x ...", 
							 addr, seq, msg[0], msg[1], msg[2], msg[3], msg[4], msg[5]);
					break;
				case 0xC0:
					DCC_LOG8(LOG_TRACE, "SYN %2d . %3d: "
							 "%02x %02x %02x %02x %02x %02x ...", 
							 addr, seq, msg[0], msg[1], msg[2], msg[3], msg[4], msg[5]);
					break;
				case 0xe0:
					DCC_LOG8(LOG_TRACE, "NAK %2d . %3d: "
							 "%02x %02x %02x %02x %02x %02x ...", 
							 addr, seq, msg[0], msg[1], msg[2], msg[3], msg[4], msg[5]);
					break;
				}

			}
		}
	}
}

void RX(uint8_t * buf, unsigned int buf_len)
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

		if (dev->rx.pos < 4)
			continue;

		/*    trace_printf("%s() Rx: 0x%02x", __func__, c); */
		if (dev->rx.pos == dev->rx.tot_len) {
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
			type = route & 0xe0;
			(void)route;
			(void)type;
			(void)msg;

			seq = ctrl & 0x7f;
			(void)seq;
			retry = (ctrl & 0x80) ? true : false;
			(void)retry;

			(void)msg;
			
			if ((len == 0) && (type = SDU_SUP)) {
				continue;
			}

			if (retry) {
				if (len == 0) {
					switch (type) {
					case 0x20:
						DCC_LOG2(LOG_WARNING, "SUP %2d R %3d", addr, seq);
						break;
					case 0x40:
						DCC_LOG2(LOG_WARNING, "CMD %2d R %3d", addr, seq);
						break;
					case 0x60:
						DCC_LOG2(LOG_WARNING, "ACK %2d R %3d", addr, seq);
						break;
					case 0xC0:
						DCC_LOG2(LOG_WARNING, "SYN %2d R %3d", addr, seq);
						break;
					case 0xe0:
						DCC_LOG2(LOG_WARNING, "NAK %2d R %3d", addr, seq);
						break;
					} 
					continue;
				} 

				if (len == 1) {
					switch (type) {
					case 0x20:
						DCC_LOG3(LOG_WARNING, "SUP %2d R %3d: %02x", addr, seq, msg[0]);
						break;
					case 0x40:
						DCC_LOG3(LOG_WARNING, "CMD %2d R %3d: %02x", addr, seq, msg[0]);
						break;
					case 0x60:
						DCC_LOG3(LOG_WARNING, "ACK %2d R %3d: %02x", addr, seq, msg[0]);
						break;
					case 0xC0:
						DCC_LOG3(LOG_WARNING, "SYN %2d R %3d: %02x", addr, seq, msg[0]);
						break;
					case 0xe0:
						DCC_LOG3(LOG_WARNING, "NAK %2d R %3d: %02x", addr, seq, msg[0]);
						break;
					}

					continue;
				}

				if (len == 2) {
					switch (type) {
					case 0x20:
						DCC_LOG4(LOG_WARNING, "SUP %2d R %3d: %02x %02x", 
								 addr, seq, msg[0], msg[1]);
						break;
					case 0x40:
						DCC_LOG4(LOG_WARNING, "CMD %2d R %3d: %02x %02x", 
								 addr, seq, msg[0], msg[1]);
						break;
					case 0x60:
						DCC_LOG4(LOG_WARNING, "ACK %2d R %3d: %02x %02x", 
								 addr, seq, msg[0], msg[1]);
						break;
					case 0xC0:
						DCC_LOG4(LOG_WARNING, "SYN %2d R %3d: %02x %02x", 
								 addr, seq, msg[0], msg[1]);
						break;
					case 0xe0:
						DCC_LOG4(LOG_WARNING, "NAK %2d R %3d: %02x %02x", 
								 addr, seq, msg[0], msg[1]);
						break;
					}

					continue;
				}

				if (len == 3) {
					switch (type) {
					case 0x20:
						DCC_LOG5(LOG_WARNING, "SUP %2d R %3d: %02x %02x %02x", 
								 addr, seq, msg[0], msg[1], msg[2]);
						break;
					case 0x40:
						DCC_LOG5(LOG_WARNING, "CMD %2d R %3d: %02x %02x %02x", 
								 addr, seq, msg[0], msg[1], msg[2]);
						break;
					case 0x60:
						DCC_LOG5(LOG_WARNING, "ACK %2d R %3d: %02x %02x %02x", 
								 addr, seq, msg[0], msg[1], msg[2]);
						break;
					case 0xC0:
						DCC_LOG5(LOG_WARNING, "SYN %2d R %3d: %02x %02x %02x", 
								 addr, seq, msg[0], msg[1], msg[2]);
						break;
					case 0xe0:
						DCC_LOG5(LOG_WARNING, "NAK %2d R %3d: %02x %02x %02x", 
								 addr, seq, msg[0], msg[1], msg[2]);
						break;
					}

					continue;
				}

				if (len == 4) {
					switch (type) {
					case 0x20:
						DCC_LOG6(LOG_WARNING, "SUP %2d R %3d: %02x %02x %02x %02x", 
								 addr, seq, msg[0], msg[1], msg[2], msg[3]);
						break;
					case 0x40:
						DCC_LOG6(LOG_WARNING, "CMD %2d R %3d: %02x %02x %02x %02x", 
								 addr, seq, msg[0], msg[1], msg[2], msg[3]);
						break;
					case 0x60:
						DCC_LOG6(LOG_WARNING, "ACK %2d R %3d: %02x %02x %02x %02x", 
								 addr, seq, msg[0], msg[1], msg[2], msg[3]);
						break;
					case 0xC0:
						DCC_LOG6(LOG_WARNING, "SYN %2d R %3d: %02x %02x %02x %02x", 
								 addr, seq, msg[0], msg[1], msg[2], msg[3]);
						break;
					case 0xe0:
						DCC_LOG6(LOG_WARNING, "NAK %2d R %3d: %02x %02x %02x %02x", 
								 addr, seq, msg[0], msg[1], msg[2], msg[3]);
						break;
					}

					continue;
				}

				if (len == 5) {
					switch (type) {
					case 0x20:
						DCC_LOG7(LOG_WARNING, "SUP %2d R %3d: "
								 "%02x %02x %02x %02x %02x", 
								 addr, seq, msg[0], msg[1], msg[2], msg[3], msg[4]);
						break;
					case 0x40:
						DCC_LOG7(LOG_WARNING, "CMD %2d R %3d: "
								 "%02x %02x %02x %02x %02x", 
								 addr, seq, msg[0], msg[1], msg[2], msg[3], msg[4]);
						break;
					case 0x60:
						DCC_LOG7(LOG_WARNING, "ACK %2d R %3d: "
								 "%02x %02x %02x %02x %02x", 
								 addr, seq, msg[0], msg[1], msg[2], msg[3], msg[4]);
						break;
					case 0xC0:
						DCC_LOG7(LOG_WARNING, "SYN %2d R %3d: "
								 "%02x %02x %02x %02x %02x", 
								 addr, seq, msg[0], msg[1], msg[2], msg[3], msg[4]);
						break;
					case 0xe0:
						DCC_LOG7(LOG_WARNING, "NAK %2d R %3d: "
								 "%02x %02x %02x %02x %02x", 
								 addr, seq, msg[0], msg[1], msg[2], msg[3], msg[4]);
						break;
					}

					continue;
				}

				switch (type) {
				case 0x20:
					DCC_LOG8(LOG_WARNING, "SUP %2d R %3d: "
							 "%02x %02x %02x %02x %02x %02x ...", 
							 addr, seq, msg[0], msg[1], msg[2], msg[3], msg[4], msg[5]);
					break;
				case 0x40:
					DCC_LOG8(LOG_WARNING, "CMD %2d R %3d: "
							 "%02x %02x %02x %02x %02x %02x ...", 
							 addr, seq, msg[0], msg[1], msg[2], msg[3], msg[4], msg[5]);
					break;
				case 0x60:
					DCC_LOG8(LOG_WARNING, "ACK %2d R %3d: "
							 "%02x %02x %02x %02x %02x %02x ...", 
							 addr, seq, msg[0], msg[1], msg[2], msg[3], msg[4], msg[5]);
					break;
				case 0xC0:
					DCC_LOG8(LOG_WARNING, "SYN %2d R %3d: "
							 "%02x %02x %02x %02x %02x %02x ...", 
							 addr, seq, msg[0], msg[1], msg[2], msg[3], msg[4], msg[5]);
					break;
				case 0xe0:
					DCC_LOG8(LOG_WARNING, "NAK %2d R %3d: "
							 "%02x %02x %02x %02x %02x %02x ...", 
							 addr, seq, msg[0], msg[1], msg[2], msg[3], msg[4], msg[5]);
					break;
				}

			} else {
				if (len == 0) {
					switch (type) {
					case 0x20:
						DCC_LOG2(LOG_TRACE, "SUP %2d . %3d", addr, seq);
						break;
					case 0x40:
						DCC_LOG2(LOG_TRACE, "CMD %2d . %3d", addr, seq);
						break;
					case 0x60:
						DCC_LOG2(LOG_TRACE, "ACK %2d . %3d", addr, seq);
						break;
					case 0xC0:
						DCC_LOG2(LOG_TRACE, "SYN %2d . %3d", addr, seq);
						break;
					case 0xe0:
						DCC_LOG2(LOG_TRACE, "NAK %2d . %3d", addr, seq);
						break;
					} 
					continue;
				} 

				if (len == 1) {
					switch (type) {
					case 0x20:
						DCC_LOG3(LOG_TRACE, "SUP %2d . %3d: %02x", addr, seq, msg[0]);
						break;
					case 0x40:
						DCC_LOG3(LOG_TRACE, "CMD %2d . %3d: %02x", addr, seq, msg[0]);
						break;
					case 0x60:
						DCC_LOG3(LOG_TRACE, "ACK %2d . %3d: %02x", addr, seq, msg[0]);
						break;
					case 0xC0:
						DCC_LOG3(LOG_TRACE, "SYN %2d . %3d: %02x", addr, seq, msg[0]);
						break;
					case 0xe0:
						DCC_LOG3(LOG_TRACE, "NAK %2d . %3d: %02x", addr, seq, msg[0]);
						break;
					}

					continue;
				}

				if (len == 2) {
					switch (type) {
					case 0x20:
						DCC_LOG4(LOG_TRACE, "SUP %2d . %3d: %02x %02x", 
								 addr, seq, msg[0], msg[1]);
						break;
					case 0x40:
						DCC_LOG4(LOG_TRACE, "CMD %2d . %3d: %02x %02x", 
								 addr, seq, msg[0], msg[1]);
						break;
					case 0x60:
						DCC_LOG4(LOG_TRACE, "ACK %2d . %3d: %02x %02x", 
								 addr, seq, msg[0], msg[1]);
						break;
					case 0xC0:
						DCC_LOG4(LOG_TRACE, "SYN %2d . %3d: %02x %02x", 
								 addr, seq, msg[0], msg[1]);
						break;
					case 0xe0:
						DCC_LOG4(LOG_TRACE, "NAK %2d . %3d: %02x %02x", 
								 addr, seq, msg[0], msg[1]);
						break;
					}

					continue;
				}

				if (len == 3) {
					switch (type) {
					case 0x20:
						DCC_LOG5(LOG_TRACE, "SUP %2d . %3d: %02x %02x %02x", 
								 addr, seq, msg[0], msg[1], msg[2]);
						break;
					case 0x40:
						DCC_LOG5(LOG_TRACE, "CMD %2d . %3d: %02x %02x %02x", 
								 addr, seq, msg[0], msg[1], msg[2]);
						break;
					case 0x60:
						DCC_LOG5(LOG_TRACE, "ACK %2d . %3d: %02x %02x %02x", 
								 addr, seq, msg[0], msg[1], msg[2]);
						break;
					case 0xC0:
						DCC_LOG5(LOG_TRACE, "SYN %2d . %3d: %02x %02x %02x", 
								 addr, seq, msg[0], msg[1], msg[2]);
						break;
					case 0xe0:
						DCC_LOG5(LOG_TRACE, "NAK %2d . %3d: %02x %02x %02x", 
								 addr, seq, msg[0], msg[1], msg[2]);
						break;
					}

					continue;
				}

				if (len == 4) {
					switch (type) {
					case 0x20:
						DCC_LOG6(LOG_TRACE, "SUP %2d . %3d: %02x %02x %02x %02x", 
								 addr, seq, msg[0], msg[1], msg[2], msg[3]);
						break;
					case 0x40:
						DCC_LOG6(LOG_TRACE, "CMD %2d . %3d: %02x %02x %02x %02x", 
								 addr, seq, msg[0], msg[1], msg[2], msg[3]);
						break;
					case 0x60:
						DCC_LOG6(LOG_TRACE, "ACK %2d . %3d: %02x %02x %02x %02x", 
								 addr, seq, msg[0], msg[1], msg[2], msg[3]);
						break;
					case 0xC0:
						DCC_LOG6(LOG_TRACE, "SYN %2d . %3d: %02x %02x %02x %02x", 
								 addr, seq, msg[0], msg[1], msg[2], msg[3]);
						break;
					case 0xe0:
						DCC_LOG6(LOG_TRACE, "NAK %2d . %3d: %02x %02x %02x %02x", 
								 addr, seq, msg[0], msg[1], msg[2], msg[3]);
						break;
					}

					continue;
				}

				if (len == 5) {
					switch (type) {
					case 0x20:
						DCC_LOG7(LOG_TRACE, "SUP %2d . %3d: "
								 "%02x %02x %02x %02x %02x", 
								 addr, seq, msg[0], msg[1], msg[2], msg[3], msg[4]);
						break;
					case 0x40:
						DCC_LOG7(LOG_TRACE, "CMD %2d . %3d: "
								 "%02x %02x %02x %02x %02x", 
								 addr, seq, msg[0], msg[1], msg[2], msg[3], msg[4]);
						break;
					case 0x60:
						DCC_LOG7(LOG_TRACE, "ACK %2d . %3d: "
								 "%02x %02x %02x %02x %02x", 
								 addr, seq, msg[0], msg[1], msg[2], msg[3], msg[4]);
						break;
					case 0xC0:
						DCC_LOG7(LOG_TRACE, "SYN %2d . %3d: "
								 "%02x %02x %02x %02x %02x", 
								 addr, seq, msg[0], msg[1], msg[2], msg[3], msg[4]);
						break;
					case 0xe0:
						DCC_LOG7(LOG_TRACE, "NAK %2d . %3d: "
								 "%02x %02x %02x %02x %02x", 
								 addr, seq, msg[0], msg[1], msg[2], msg[3], msg[4]);
						break;
					}

					continue;
				}

				switch (type) {
				case 0x20:
					DCC_LOG8(LOG_TRACE, "SUP %2d . %3d: "
							 "%02x %02x %02x %02x %02x %02x ...", 
							 addr, seq, msg[0], msg[1], msg[2], msg[3], msg[4], msg[5]);
					break;
				case 0x40:
					DCC_LOG8(LOG_TRACE, "CMD %2d . %3d: "
							 "%02x %02x %02x %02x %02x %02x ...", 
							 addr, seq, msg[0], msg[1], msg[2], msg[3], msg[4], msg[5]);
					break;
				case 0x60:
					DCC_LOG8(LOG_TRACE, "ACK %2d . %3d: "
							 "%02x %02x %02x %02x %02x %02x ...", 
							 addr, seq, msg[0], msg[1], msg[2], msg[3], msg[4], msg[5]);
					break;
				case 0xC0:
					DCC_LOG8(LOG_TRACE, "SYN %2d . %3d: "
							 "%02x %02x %02x %02x %02x %02x ...", 
							 addr, seq, msg[0], msg[1], msg[2], msg[3], msg[4], msg[5]);
					break;
				case 0xe0:
					DCC_LOG8(LOG_TRACE, "NAK %2d . %3d: "
							 "%02x %02x %02x %02x %02x %02x ...", 
							 addr, seq, msg[0], msg[1], msg[2], msg[3], msg[4], msg[5]);
					break;
				}

			}
		}
	}
}

