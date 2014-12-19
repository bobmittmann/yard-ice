/* 
 * File:	stm32f-usart.c
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

#include "usart-priv.h"

static void io_rxd_cfg(struct stm32_gpio * gpio, int port, int af)
{
	stm32_gpio_clock_en(gpio);
	stm32_gpio_mode(gpio, port, ALT_FUNC, PULL_UP);
#ifdef STM32F2X
	stm32_gpio_af(gpio, port, af);
#endif
}

static void io_txd_cfg(struct stm32_gpio * gpio, int port, int af)
{
	stm32_gpio_clock_en(gpio);
	stm32_gpio_mode(gpio, port, ALT_FUNC, PUSH_PULL | SPEED_LOW);
#ifdef STM32F2X
	stm32_gpio_af(gpio, port, af);
#endif
}

static const struct {
	gpio_io_t rx; /* IO port/pin */
	gpio_io_t tx; /* IO port/pin */
	uint8_t	af; /* Alternate function */
	uint8_t	ckbit : 5;  
	uint8_t	apb2 : 1;  
} __attribute__((__packed__)) cfg[6] = {
	{ .rx = GPIO(GPIOA, 10), .tx = GPIO(GPIOA, 9), .af = GPIO_AF7, 
		.ckbit = 4, .apb2 = 1},
	{ .rx = GPIO(GPIOA, 3), .tx = GPIO(GPIOA, 3), .af = GPIO_AF7, 
		.ckbit = 17, .apb2 = 0},
	{ .rx = GPIO(GPIOB, 11), .tx = GPIO(GPIOB, 10), .af = GPIO_AF7,
		.ckbit = 18, .apb2 = 0},
	{ .rx = GPIO(GPIOC, 12), .tx = GPIO(GPIOC, 10), .af = GPIO_AF8,
		.ckbit = 19, .apb2 = 0},
	{ .rx = GPIO(GPIOD, 2), .tx = GPIO(GPIOC, 12), .af = GPIO_AF8,
		.ckbit = 20, .apb2 = 0},
	{ .rx = GPIO(GPIOC, 7), .tx = GPIO(GPIOC, 6), .af = GPIO_AF8,
		.ckbit = 5, .apb2 = 1}
};

const struct file stm32f_uart_file[] = {
	{ .data = STM32_USART1, .op = &stm32_usart_fops },
	{ .data = STM32_USART2, .op = &stm32_usart_fops },
	{ .data = STM32_USART3, .op = &stm32_usart_fops },
	{ .data = STM32_UART4, .op = &stm32_usart_fops },
	{ .data = STM32_UART5, .op = &stm32_usart_fops },
#ifdef STM32F_USART6
	{ .data = STM32F_USART6, .op = &stm32_usart_fops }
#endif
};

struct file * stm32_usart_open(struct stm32_usart * us,
								unsigned int baudrate, unsigned int flags)
{
	int id;

	if ((id = stm32_usart_init(us)) < 0) {
		return NULL;
	}

	io_rxd_cfg(STM32_GPIO(cfg[id].rx.port), cfg[id].rx.pin, cfg[id].af);
	io_txd_cfg(STM32_GPIO(cfg[id].tx.port), cfg[id].tx.pin, cfg[id].af);

	stm32_usart_baudrate_set(us, baudrate);
	stm32_usart_mode_set(us, flags);
	stm32_usart_enable(us);

	return (struct file *)&stm32f_uart_file[id];
}

