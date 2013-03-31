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

#include <sys/dcclog.h>

const struct stm32f_usart * stm32f_usart_lut[6] = {
	STM32F_USART1,
	STM32F_USART2,
	STM32F_USART3,
	STM32F_UART4,
	STM32F_UART5,
#ifdef STM32F_USART6
	STM32F_USART6
#endif
};

/* APB clock */

#define APB1 (1 << 5)
#define APB2 (1 << 6)
#define CLK(APB, BIT) (APB | BIT)
#define CLK_BIT 0x1f

#ifdef STM32F2X
static const uint8_t us_clk_lut[] = {  
	CLK(APB2, 4), 
	CLK(APB1, 17), 
	CLK(APB1, 18), 
	CLK(APB1, 19), 
	CLK(APB1, 20), 
	CLK(APB2,  5)
};
#endif

#ifdef STM32F10X
static const uint8_t us_clk_lut[] = {  
	CLK(APB2, 14), 
	CLK(APB1, 17), 
	CLK(APB1, 18), 
	CLK(APB1, 19), 
	CLK(APB1, 20)
};
#endif


int stm32f_usart_lookup(struct stm32f_usart * us)
{
	int id = sizeof(stm32f_usart_lut) / sizeof(struct stm32f_usart *);

	while ((--id >= 0) && (us != stm32f_usart_lut[id]));

	return id;
}

#if 0
static const struct {
	gpio_io_t rx; /* IO port/pin */
	gpio_io_t tx; /* IO port/pin */
	uint8_t	af; /* Alternate function */
	uint8_t	ckbit : 5; /* APB clock bit */
	uint8_t	apb2 : 1;  
} __attribute__((__packed__)) cfg[6] = {
	{ .rx = GPIO(PA, 10), .tx = GPIO(PA, 9), .af = GPIO_AF7, 
		.ckbit = 4, .apb2 = 1},
	{ .rx = GPIO(PA, 3), .tx = GPIO(PA, 2), .af = GPIO_AF7, 
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

int stm32f_usart_io_cfg(struct stm32f_usart * us)
{
	int id;

	if ((id = stm32f_usart_lookup(us)) < 0) {
		/* invalid UART ??? */
		return id;
	}

	/* configure IO pins */
	io_rxd_cfg(STM32F_GPIO(cfg[id].rx.port), cfg[id].rx.pin, cfg[id].af);
	io_txd_cfg(STM32F_GPIO(cfg[id].tx.port), cfg[id].tx.pin, cfg[id].af);

	return 0;
}
#endif

int stm32f_usart_init(struct stm32f_usart * us)
{
	int id;
	int clk;

	if ((id = stm32f_usart_lookup(us)) < 0) {
		/* invalid UART ??? */
		return id;
	}

	DCC_LOG2(LOG_TRACE, "USART %d -> 0x%08x.", id + 1, us);

	/* disable all interrupts */
	us->cr1 = 0;

	/* Enable peripheral clock */
	clk = us_clk_lut[id];


	DCC_LOG2(LOG_TRACE, "APB%d |= 1 << %d.", 
			 (clk & APB2) ? 2 : 1, (clk & CLK_BIT));

	if (clk & APB2)
		STM32F_RCC->apb2enr |= (1 << (clk & CLK_BIT));
	else
		STM32F_RCC->apb1enr |= (1 << (clk & CLK_BIT));

	/* output drain */
//	while (!(us->sr & USART_TXE));

	us->cr1 = 0;
	us->cr2 = 0;
	us->cr3 = 0;
	us->gtpr = 0;

	DCC_LOG(LOG_TRACE, "done.");

	return id;
}

int stm32f_usart_baudrate_set(struct stm32f_usart * us, unsigned int baudrate)
{
	uint32_t div;
	uint32_t f;
	uint32_t m;
	uint32_t cr1;
	uint32_t f_pclk;
	int id;

	if ((id = stm32f_usart_lookup(us)) < 0) {
		/* invalid UART ??? */
		return id;
	}

	/* disable TX and RX */
	cr1 = us->cr1;
	us->cr1 = cr1 & ~(USART_UE | USART_TE | USART_RE);

	if (us_clk_lut[id] & APB2)
		f_pclk = stm32f_apb2_hz;
	else 
		f_pclk = stm32f_apb1_hz;

	div = f_pclk / baudrate;

	DCC_LOG3(LOG_TRACE, "baudrate=%d p_clk=%d div=%d", baudrate, f_pclk, div);

	m = div >> 4;
	f = div & 0x0f;
	us->brr = (m << 4) | f;

	/* restore cr1 */
	us->cr1 = cr1;

	return 0;
}

int stm32f_usart_mode_set(struct stm32f_usart * us, unsigned int flags)
{
	uint32_t cr1;
	uint32_t cr2;
	uint32_t cr_en;
	int bits;
	int id;

	if ((id = stm32f_usart_lookup(us)) < 0) {
		/* invalid UART ??? */
		return id;
	}

	/* save enable bits */
	cr_en = us->cr1 & (USART_UE | USART_TE | USART_RE);
	if (cr_en & USART_TE) {
		/* drain output buffer */
		while (!(us->sr & USART_TXE));
	}

	/* disable TX, RX and all interrupts */
	us->cr1 = 0;

	cr1 = 0;
	cr2 = 0;

	bits = flags & SERIAL_DATABITS_MASK;
	/* parity and data bits */
	switch (flags & SERIAL_PARITY_MASK) {
	case SERIAL_PARITY_NONE:
		DCC_LOG(LOG_TRACE, "parity NONE");
		cr1 |= (bits == SERIAL_DATABITS_9) ? USART_M9 : USART_M8;
		break;
	case SERIAL_PARITY_EVEN:
		DCC_LOG(LOG_TRACE, "parity EVEN");
		cr1 |= USART_PCE | USART_PS_EVEN;
		cr1 |= (bits == SERIAL_DATABITS_8) ? USART_M9 : USART_M8;
		break;
	case SERIAL_PARITY_ODD:
		DCC_LOG(LOG_TRACE, "parity ODD");
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

	us->cr1 = cr1 | cr_en;
	us->cr2 = cr2;

	return 0;
}

void stm32f_usart_enable(struct stm32f_usart * us)
{
	/* enable TX and RX */
	us->cr1 |= USART_UE | USART_TE | USART_RE;
}

void stm32f_usart_disable(struct stm32f_usart * us)
{
	/* disable TX, RX and all interrupts */
	us->cr1 = 0;
}

int stm32f_usart_power_off(struct stm32f_usart * us)
{
	int id;
	int clk;

	if ((id = stm32f_usart_lookup(us)) < 0) {
		/* invalid UART ??? */
		return id;
	}

	/* disable TX, RX and all interrupts */
	us->cr1 = 0;

	/* Disable peripheral clock */
	clk = us_clk_lut[id];
	if (clk & APB2)
		STM32F_RCC->apb2enr &= ~(1 << (clk & CLK_BIT));
	else
		STM32F_RCC->apb1enr &= ~(1 << (clk & CLK_BIT));

	return 0;
}

