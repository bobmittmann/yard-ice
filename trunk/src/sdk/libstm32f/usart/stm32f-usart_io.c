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

#include <sys/stm32f.h>
#include <sys/serial.h>
#include <sys/file.h>
#include <sys/delay.h>
#include <stdlib.h>

int stm32f_usart_lookup(struct stm32f_usart * us);

int stm32f_usart_putc(struct stm32f_usart * usart, int c)
{
	while (!(usart->sr & USART_TXE));

	usart->dr = c;

	return 0;
}

int stm32f_usart_getc(struct stm32f_usart * usart, unsigned int msec)
{
	int tm;

	tm = msec * 20;

	for (;;) {		
		if (usart->sr & USART_RXNE) {
			return usart->dr;
		}
		if (tm == 0) {
			return -1;
		}
		udelay(50);
		tm--;
	}
}

int stm32f_usart_read(struct stm32f_usart * usart, char * buf, 
					  unsigned int len, unsigned int msec)
{
	char * cp = (char *)buf;
	int c;

	c = stm32f_usart_getc(usart, msec);

	if (c < 0)
		return 0;

	*cp = c;
		
	return 1;
}

int stm32f_usart_flush(struct stm32f_usart * usart)
{
	int c;

	while (!(usart->sr & USART_TC));

	/* clear buffered chars */
	c = usart->dr;
	(void)c;

	return 0;
}

int stm32f_usart_write(struct stm32f_usart * usart, const void * buf, 
					   unsigned int len)
{
	char * cp = (char *)buf;
	int c;
	int n;

	for (n = 0; n < len; n++) {
		c = cp[n];
		stm32f_usart_putc(usart, c);
	}

	return n;
}

int stm32f_usart_canon_write(struct stm32f_usart * usart, const void * buf,
							 unsigned int len)
{
	char * cp = (char *)buf;
	int c;
	int n;

	for (n = 0; n < len; n++) {
		c = cp[n];
		if (c == '\n')
			stm32f_usart_putc(usart, '\r');
		stm32f_usart_putc(usart, c);
	}

	return n;
}

#if 0
void burst(void)
{
	struct stm32f_gpio * gpio = STM32F_GPIOC;
	int i;

	stm32f_gpio_clock_en(STM32F_GPIOC_ID);
	stm32f_gpio_mode(gpio, 12, OUTPUT, PUSH_PULL | SPEED_LOW);

	for (i = 0; i < 32; i++) {
		udelay(1000000 / (115200 * 2));
		gpio->bsrrl = 1 << 12;
		udelay(1000000 / (115200 * 2));
		gpio->bsrrh = 1 << 12;
	}
}
#endif

static void io_rxd_cfg(struct stm32f_gpio * gpio, int port, int af)
{
	stm32f_gpio_clock_en(gpio);
	stm32f_gpio_mode(gpio, port, ALT_FUNC, PULL_UP);
#ifdef STM32F2X
	stm32f_gpio_af(gpio, port, af);
#endif
}

static void io_txd_cfg(struct stm32f_gpio * gpio, int port, int af)
{
	stm32f_gpio_clock_en(gpio);
	stm32f_gpio_mode(gpio, port, ALT_FUNC, PUSH_PULL | SPEED_LOW);
#ifdef STM32F2X
	stm32f_gpio_af(gpio, port, af);
#endif
}

static const struct {
	gpio_io_t rx; /* IO port/pin */
	gpio_io_t tx; /* IO port/pin */
	uint8_t	af; /* Alternate function */
	uint8_t	ckbit : 5;  
	uint8_t	apb2 : 1;  
} __attribute__((__packed__)) cfg[6] = {
	{ .rx = GPIO(PA, 10), .tx = GPIO(PA, 9), .af = GPIO_AF7, 
		.ckbit = 4, .apb2 = 1},
	{ .rx = GPIO(PA, 3), .tx = GPIO(PA, 3), .af = GPIO_AF7, 
		.ckbit = 17, .apb2 = 0},
	{ .rx = GPIO(PB, 11), .tx = GPIO(PB, 10), .af = GPIO_AF7,
		.ckbit = 18, .apb2 = 0},
	{ .rx = GPIO(PC, 12), .tx = GPIO(PC, 10), .af = GPIO_AF8,
		.ckbit = 19, .apb2 = 0},
	{ .rx = GPIO(PD, 2), .tx = GPIO(PC, 12), .af = GPIO_AF8,
		.ckbit = 20, .apb2 = 0},
	{ .rx = GPIO(PC, 7), .tx = GPIO(PC, 6), .af = GPIO_AF8,
		.ckbit = 5, .apb2 = 1}
};

int stm32f_usart_release(struct stm32f_usart * us)
{
	int n;

	if ((n = stm32f_usart_lookup(us)) < 0) {
		/* invalid UART ??? */
		return n;
	}

	/* disable all interrupts */
	us->cr1 = 0;

	/* Disable peripheral clock */
	if (cfg[n].apb2)
		STM32F_RCC->apb2enr &= ~(1 << cfg[n].ckbit);
	else
		STM32F_RCC->apb1enr &= ~(1 << cfg[n].ckbit);

	return 0;
}

const struct fileop stm32f_usart_ops = {
	.write = (void *)stm32f_usart_canon_write,
	.read = (void *)stm32f_usart_read,
	.flush = (void *)stm32f_usart_flush,
	.close = (void *)stm32f_usart_release
};

const struct file stm32f_uart_file[] = {
	{ .data = STM32F_USART1, .op = &stm32f_usart_ops },
	{ .data = STM32F_USART2, .op = &stm32f_usart_ops },
	{ .data = STM32F_USART3, .op = &stm32f_usart_ops },
	{ .data = STM32F_UART4, .op = &stm32f_usart_ops },
	{ .data = STM32F_UART5, .op = &stm32f_usart_ops },
#ifdef STM32F_USART6
	{ .data = STM32F_USART6, .op = &stm32f_usart_ops }
#endif
};

struct file * stm32f_usart_open(struct stm32f_usart * us,
								unsigned int baudrate, unsigned int flags)
{
	int id;

	if ((id = stm32f_usart_init(us)) < 0) {
		return NULL;
	}

	io_rxd_cfg(STM32F_GPIO(cfg[id].rx.port), cfg[id].rx.pin, cfg[id].af);
	io_txd_cfg(STM32F_GPIO(cfg[id].tx.port), cfg[id].tx.pin, cfg[id].af);

	stm32f_usart_baudrate_set(us, baudrate);
	stm32f_usart_mode_set(us, flags);
	stm32f_usart_enable(us);

	return (struct file *)&stm32f_uart_file[id];
}

