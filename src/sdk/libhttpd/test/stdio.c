/* 
 * File:	 stdio.c
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
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
#include <sys/delay.h>
#include <sys/tty.h>
#include <stdio.h>
#include <stdlib.h>


const struct file stm32_uart_file = {
	.data = STM32_UART5, 
	.op = &stm32_usart_fops 
};

struct uart_console_dev * uart_console_init(unsigned int baudrate, 
											unsigned int flags);

struct file * uart_console_fopen(struct uart_console_dev * dev);

void stdio_init(void)
{
	struct uart_console_dev * dev;
	struct tty_dev * tty;
	FILE * f_tty;
	FILE * f_raw;

	dev = uart_console_init(115200, SERIAL_8N1);
	f_raw = uart_console_fopen(dev);
	tty = tty_attach(f_raw);
	f_tty = tty_fopen(tty);

	stderr = (struct file *)&stm32_uart_file;
	stdout = f_tty;
	stdin = f_tty;
}

int stdio_shell(void);
