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

#define STM32F_HSI_HZ 16000000
#define STM32F_HSE_HZ 24000000
#define STM32F_PLL_HZ 120000000
#define STM32F_APB1_HZ 30000000
#define STM32F_APB2_HZ 60000000

#define STM32F_AHB_HZ 120000000

static int inline putc(struct stm32f_usart * usart, int c)
{
	while (!(usart->sr & USART_TXE));

	usart->dr = c;

	return 0;
}

static int inline getc(struct stm32f_usart * usart, unsigned int msec)
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


static void set_baudrate(struct stm32f_usart * usart, unsigned int baudrate)
{
	uint32_t div;
	uint32_t f;
	uint32_t m;

	if ((usart == STM32F_USART1) || (usart == STM32F_USART6)) {
		div = STM32F_APB2_HZ / baudrate;
	} else {
		div = STM32F_APB1_HZ / baudrate;
	}

	m = div >> 4;
	f = div & 0x0f;		
	if (usart->cr1 & USART_OVER8) {
		f >>= 1;
	}

	/* Write to USART BRR register */
	usart->brr = (m << 4) | f;
}


int stm32f_usart_putc(struct stm32f_usart * usart, int c)
{
	return putc(usart, c);
}

int stm32f_usart_getc(struct stm32f_usart * usart, unsigned int msec)
{
	return getc(usart, msec);
}

int stm32f_usart_read(struct stm32f_usart * usart, char * buf, 
					  unsigned int len, unsigned int msec)
{
	char * cp = (char *)buf;
	int c;

	c = getc(usart, msec);

	if (c < 0)
		return 0;

	*cp = c;
		
	return 1;
}

int stm32f_usart_write(struct stm32f_usart * usart, const void * buf, 
					   unsigned int len)
{
	char * cp = (char *)buf;
	int c;
	int n;

	for (n = 0; n < len; n++) {
		c = cp[n];
		putc(usart, c);
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
			putc(usart, '\r');
		putc(usart, c);
	}

	return n;
}

int stm32f_usart_flush(struct stm32f_usart * usart)
{
	int c;

	while (!(usart->sr & USART_TC));

	/* clear buffered chars */
	c = usart->dr;
	c = c;

	return 0;
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
	stm32f_gpio_af(gpio, port, af);
}

static void io_txd_cfg(struct stm32f_gpio * gpio, int port, int af)
{
	stm32f_gpio_clock_en(gpio);
	stm32f_gpio_mode(gpio, port, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	stm32f_gpio_af(gpio, port, af);
}

const struct stm32f_usart * stm32f_usart_lut[6] = {
	STM32F_USART1,
	STM32F_USART2,
	STM32F_USART3,
	STM32F_UART4,
	STM32F_UART5,
	STM32F_USART6
};

int stm32f_usart_lookup(struct stm32f_usart * us)
{
	int id = sizeof(stm32f_usart_lut) / sizeof(struct stm32f_usart *);

	while ((--id >= 0) && (us != stm32f_usart_lut[id]));

	return id;
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

int stm32f_usart_init(struct stm32f_usart * us,
					  unsigned int baudrate, unsigned int flags)
{
	uint32_t cr1;
	uint32_t cr2;
	int bits;
	int id;

	if ((id = stm32f_usart_lookup(us)) < 0) {
		/* invalid UART ??? */
		return id;
	}

	/* disable all interrupts */
	us->cr1 = 0;

	/* Enable peripheral clock */
	if (cfg[id].apb2)
		STM32F_RCC->apb2enr |= (1 << cfg[id].ckbit);
	else
		STM32F_RCC->apb1enr |= (1 << cfg[id].ckbit);

	io_rxd_cfg(STM32F_GPIO(cfg[id].rx.port), cfg[id].rx.pin, cfg[id].af);
	io_txd_cfg(STM32F_GPIO(cfg[id].tx.port), cfg[id].tx.pin, cfg[id].af);

	/* set character length */
	/* 8N1 */
	us->cr1 = USART_UE | USART_TE | USART_RE;

	/* output drain */
	while (!(us->sr & USART_TXE));

	/* enable TX and RX */
	cr1 = USART_UE | USART_TE | USART_RE;
	cr2 = 0;

	bits = flags & SERIAL_DATABITS_MASK;
	/* parity and data bits */
	switch (flags & SERIAL_PARITY_MASK) {
	case SERIAL_PARITY_NONE:
		cr1 |= (bits == SERIAL_DATABITS_9) ? USART_M9 : USART_M8;
		break;
	case SERIAL_PARITY_EVEN:
		cr1 |= USART_PCE | USART_PS_EVEN;
		cr1 |= (bits == SERIAL_DATABITS_8) ? USART_M9 : USART_M8;
		break;
	case SERIAL_PARITY_ODD:
		cr1 |= USART_PCE | USART_PS_ODD;
		cr1 |= (bits == SERIAL_DATABITS_8) ? USART_M9 : USART_M8;
		break;
	}

	/* stop bits */
	switch (flags & SERIAL_STOPBITS_MASK) {
	case SERIAL_STOPBITS_2:
		cr2 |= USART_STOP_2;
		break;
	case SERIAL_STOPBITS_1:
		cr2 |= USART_STOP_1;
		break;
	}

	us->cr1 = cr1;
	us->cr2 = cr2;
	us->cr3 = 0;
	us->gtpr = 0;

	set_baudrate(us, baudrate);

	return id;
}

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
	{ .data = STM32F_USART6, .op = &stm32f_usart_ops }
};

struct file * stm32f_usart_open(struct stm32f_usart * us,
								unsigned int baudrate, unsigned int flags)
{
	int id;

	if ((id = stm32f_usart_init(us, baudrate, flags)) < 0) {
		return NULL;
	}

	return (struct file *)&stm32f_uart_file[id];
}

