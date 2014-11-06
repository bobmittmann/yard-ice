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
#include <stdio.h>
#include <stdlib.h>

/* ----------------------------------------------------------------------
 * Stdio init 
 * ----------------------------------------------------------------------
 */

#if defined(STM32F2X)

#define UART_TX STM32F_GPIOC, 12
#define UART_RX STM32F_GPIOD, 2

struct file stm32f_uart_file = {
	.data = STM32F_UART5, 
	.op = &stm32f_usart_fops 
};
#else

#define UART_TX STM32F_GPIOB, 6
#define UART_RX STM32F_GPIOB, 7

struct file stm32f_uart_file = {
	.data = STM32F_USART1, 
	.op = &stm32f_usart_fops 
};
#endif

void stdio_init(void)
{
#if defined(STM32F2X)
	struct stm32f_usart * uart = STM32F_UART5;
#else
	struct stm32f_usart * uart = STM32F_USART1;
#endif
#if defined(STM32F1X)
	struct stm32f_afio * afio = STM32F_AFIO;
#endif

	/* Enable GPIO */
#if defined(STM32F2X)
	stm32f_gpio_clock_en(STM32F_GPIOC);
	stm32f_gpio_clock_en(STM32F_GPIOD);
#else
	stm32f_gpio_clock_en(STM32F_GPIOB);
#endif

	/* USART1_TX */
	stm32f_gpio_mode(UART_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);

#if defined(STM32F1X)
	/* USART1_RX */
	stm32f_gpio_mode(UART_RX, INPUT, PULL_UP);
	/* Use alternate pins for USART1 */
	afio->mapr |= AFIO_USART1_REMAP;
#elif defined(STM32F4X)
	stm32f_gpio_mode(UART_RX, ALT_FUNC, PULL_UP);
	stm32f_gpio_af(UART_RX, GPIO_AF7);
	stm32f_gpio_af(UART_TX, GPIO_AF7);
#elif defined(STM32F2X)
	stm32f_gpio_mode(UART_RX, ALT_FUNC, PULL_UP);
	stm32f_gpio_af(UART_RX, GPIO_AF8);
	stm32f_gpio_af(UART_TX, GPIO_AF8);
#endif

	stm32f_usart_init(uart);
	stm32f_usart_baudrate_set(uart, 115200);
	stm32f_usart_mode_set(uart, SERIAL_8N1);
	stm32f_usart_enable(uart);

	stderr = &stm32f_uart_file;
	stdin = stderr;
	stdout = stdin;
}

