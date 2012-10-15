/* 
 * File:	stm32f-spi.c
 * Author:	Robinson Mittmann (bobmittmann@gmail.com)
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

static int inline putc(struct stm32f_spi * spi, int c)
{
	while (!(spi->sr & USART_TXE));

	spi->dr = c;

	return 0;
}

static int inline getc(struct stm32f_spi * spi, unsigned int msec)
{
	int tm;

	tm = msec * 32;

	for (;;) {		
		if (spi->sr & USART_RXNE) {
			return spi->dr;
		}
		if (tm == 0) {
			return -1;
		}
//		udelay(31);
		tm--;
	}
}


static void set_baudrate(struct stm32f_spi * spi, unsigned int baudrate)
{
	uint32_t div;
	uint32_t f;
	uint32_t m;

	if ((spi == STM32F_USART1) || (spi == STM32F_USART6)) {
		div = STM32F_APB2_HZ / baudrate;
	} else {
		div = STM32F_APB1_HZ / baudrate;
	}

	m = div >> 4;
	f = div & 0x0f;		
	if (spi->cr1 & USART_OVER8) {
		f >>= 1;
	}

	/* Write to USART BRR register */
	spi->brr = (m << 4) | f;
}


int stm32f_spi_putc(struct stm32f_spi * spi, int c)
{
	return putc(spi, c);
}

int stm32f_spi_getc(struct stm32f_spi * spi, unsigned int msec)
{
	return getc(spi, msec);
}

int stm32f_spi_read(struct stm32f_spi * spi, char * buf, 
					  unsigned int len, unsigned int msec)
{
	char * cp = (char *)buf;
	int c;

	c = getc(spi, msec);

	if (c < 0)
		return 0;

	*cp = c;
		
	return 1;
}

int stm32f_spi_write(struct stm32f_spi * spi, const void * buf, 
					   unsigned int len)
{
	char * cp = (char *)buf;
	int n;

	for (n = 0; n < len; n++) {
		putc(spi, cp[n]);
	}

	return n;
}

int stm32f_spi_flush(struct stm32f_spi * spi)
{
	int c;

	while (!(spi->sr & USART_TC));

	/* clear buffered chars */
	c = spi->dr;
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


int stm32f_spi_open(struct stm32f_spi * us,
					  unsigned int baudrate, unsigned int flags)
{
	uint32_t cr1;
	uint32_t cr2;
	int bits;

	/* disable all interrupts */
	us->cr1 = 0;

#ifdef STM32F207
	if (us == STM32F_USART5) {
		io_rxd_cfg(STM32F_GPIOD, 2, GPIO_AF8);
		io_txd_cfg(STM32F_GPIOC, 12, GPIO_AF8);
		/* Enable peripheral clock */
		STM32F_RCC->apb1enr |= RCC_UART5EN;
	} else {
		return -1;
	}
#else

#error "Unknown MCU"

#endif

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

	return 0;
}

int stm32f_spi_close(struct stm32f_spi * spi)
{
#ifdef STM32F207
	if (spi == STM32F_USART5) {
		/* Disable peripheral clock */
		STM32F_RCC->apb1enr &= ~RCC_UART5EN;
	} else {
		return -1;
	}
#else

