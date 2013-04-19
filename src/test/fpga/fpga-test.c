/* 
 * File:	 serial-test.c
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(C) 2013 Bob Mittmann. All Rights Reserved.
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
#include <sys/delay.h>
#include <sys/serial.h>
#include <sys/param.h>
#include <sys/file.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <thinkos.h>
#include <sys/dcclog.h>

#include <yard-ice/drv.h>

const struct file stm32f_uart_file = {
	.data = STM32F_UART5, 
	.op = &stm32f_usart_fops 
};

void stdio_init(void)
{
	stderr = (struct file *)&stm32f_uart_file;
	stdout = uart_console_fopen(uart_console_init(115200, SERIAL_8N1));
	stdin = stdout;
}

int main(int argc, char ** argv)
{
	int i = 0;

	DCC_LOG_INIT();
	DCC_LOG_CONNECT();

	cm3_udelay_calibrate();
	thinkos_init(THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(32));
	stdio_init();

	printf("\n\n");
	printf("-----------------------------------------\n");
	printf(" JTAGTOOL3 FPGA test\n");
	printf("-----------------------------------------\n");
	printf("\n");

	for (i = 0; ; i++) {
		thinkos_sleep(500);
	}

	return 0;
}

