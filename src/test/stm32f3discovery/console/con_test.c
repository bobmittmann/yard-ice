/* 
 * File:	 rtos_basic.c
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
#include <sys/delay.h>
#include <sys/serial.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* ----------------------------------------------------------------------
 * Stdio init 
 * ----------------------------------------------------------------------
 */

#define UART_TX STM32F_GPIOA, 9
#define UART_RX STM32F_GPIOA, 10

const struct file stm32f_uart_file = {
	.data = STM32F_USART1, 
	.op = &stm32f_usart_fops 
};

void stdio_init(void)
{
	struct stm32f_usart * uart = STM32F_USART1;

	/* Enable GPIO */
	stm32f_gpio_clock_en(STM32F_GPIOA);

	/* Configure TX pin */
	stm32f_gpio_mode(UART_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	stm32f_gpio_af(UART_TX, GPIO_AF7);

	/* Configure RX pin */
	stm32f_gpio_mode(UART_RX, ALT_FUNC, PULL_UP);
	stm32f_gpio_af(UART_RX, GPIO_AF7);

	stm32f_usart_init(uart);
	stm32f_usart_baudrate_set(uart, 115200);
	stm32f_usart_mode_set(uart, SERIAL_8N1);
	stm32f_usart_enable(uart);

	stm32f_usart_write(uart, "Hello world!", 12);

	stderr = (struct file *)&stm32f_uart_file;
	stdin = stderr;
	stdout = stdin;
}



int main(int argc, char ** argv)
{
	int i;

	/* calibrate usecond delay loop */
	cm3_udelay_calibrate();

	/* stdio initialization */
	stdio_init();

	printf("\n");
	printf("---------------------------------------------------------\n");
	printf(" STM32F3 Discovery Board - Console example\n");
	printf("---------------------------------------------------------\n");
	printf("\n");

	for (i = 0; ; ++i) {
		printf("%d\n", i);
		udelay(500000);
	}

	return 0;
}

