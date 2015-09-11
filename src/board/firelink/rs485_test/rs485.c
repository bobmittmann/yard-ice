/* 
 * File:	 usb-test.c
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(C) 2011 Bob Mittmann. All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <sys/stm32f.h>
#include <sys/serial.h>
#include <thinkos.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "board.h"
#include "profclk.h"

struct rs585_dev {
	struct serial_dev * serial;
	bool loopback;
	bool test;
};

#define RS485_PKT_MAX 254

int rc485_pkt_recv(struct rs585_dev * rs485, uint8_t * pkt,
		unsigned int tmo)
{
	int n;

	n = serial_recv(rs485->serial, pkt, RS485_PKT_MAX, 1000000);
	if (n <= 0)
		return n;

	return n;
}

int rc485_pkt_send(struct rs585_dev * rs485, uint8_t * pkt,
		unsigned int len)
{
	return serial_send(rs485->serial, pkt, len);
}

void __attribute__((noreturn)) rs485_task(struct rs585_dev * rs485)
{
	uint8_t pkt[RS485_PKT_MAX + 2];
	char s[80];
	char * cp;
	uint32_t profclk_old;
	uint32_t profclk_new;
	uint32_t dt;
	int us;
	int ms;
	int sec;
	int n;
	int i;

	profclk_init();
	profclk_old = profclk_get();
	for (;;) {
		n = serial_recv(rs485->serial, pkt, RS485_PKT_MAX, 1000);
		profclk_new = profclk_get();
		dt = profclk_us(profclk_new - profclk_old);
		profclk_old = profclk_new;
	    sec = dt / 1000000;
	    dt -= (sec * 1000000);
	    ms = dt / 1000;
	    us = dt - (ms * 1000);

	    cp = (char *)s;
		cp += sprintf(cp, "%2d.%03d.%03d:", sec, ms, us);

		if (n > 0) {
			cp += sprintf(cp, " (%3d)", n);
			if (n > 8) {
				for (i = 0; i < 8; ++i)
					cp += sprintf(cp, " %02x", pkt[i]);
				cp += sprintf(cp, " ...\r\n");
			} else {
				for (i = 0; i < n; ++i)
					cp += sprintf(cp, " %02x", pkt[i]);
				cp += sprintf(cp, "\r\n");
			}

			if (rs485->loopback) {
				serial_send(rs485->serial, pkt, n);
			}
		}

		if (n <= 0) {
			cp += sprintf(cp, " <TMO>\r\n");
			n = sprintf((char *)pkt, "The quick brown fox jumps over the lazy dog!\r\n");
			serial_send(rs485->serial, pkt, n);
		}

		printf(s);
	};
}


extern const uint8_t ice40lp384_bin[];
extern const unsigned int sizeof_ice40lp384_bin;

struct rs585_dev * rs485_init(void)
{
	static struct rs585_dev rs485_dev;
	static uint32_t task_stack[256];
	static const struct thinkos_thread_inf task_inf = {
		.stack_ptr = task_stack,
		.stack_size = sizeof(task_stack),
		.priority = 8,
		.thread_id = 32, 
		.paused = 0,
		.tag = "RS485"
	};

    /* IO init */
    stm32_gpio_mode(IO_RS485_RX, ALT_FUNC, PULL_UP);
    stm32_gpio_af(IO_RS485_RX, GPIO_AF8);

    stm32_gpio_mode(IO_RS485_TX, ALT_FUNC, PUSH_PULL | SPEED_MED);
    stm32_gpio_af(IO_RS485_TX, GPIO_AF8);

    stm32_gpio_mode(IO_RS485_MODE, OUTPUT, PUSH_PULL | SPEED_LOW);
    stm32_gpio_set(IO_RS485_MODE);

    lattice_ice40_configure(ice40lp384_bin, sizeof_ice40lp384_bin);

	rs485_dev.serial = stm32f_uart7_serial_init(500000, SERIAL_8N1);

	thinkos_thread_create_inf((void *)rs485_task, &rs485_dev, &task_inf);

	return &rs485_dev;
}
