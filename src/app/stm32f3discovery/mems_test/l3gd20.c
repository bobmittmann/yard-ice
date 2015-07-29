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
#include <sys/param.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/delay.h>

#include <thinkos.h>

#include "l3gd20.h"


/* ----------------------------------------------------------------------
 * Accelerometer
 * ----------------------------------------------------------------------
 */

const struct {
	struct stm32f_spi * spi;
} l3gd20ctl = {
	.spi = STM32F_SPI1
};

static const struct stm32f_spi_io spi1_io = {
	.miso = GPIO(GPIOA, 6), /* MISO */
	.mosi = GPIO(GPIOA, 7), /* MOSI */
	.sck = GPIO(GPIOA, 5)  /* SCK */
};

static const gpio_io_t l3gd20_cs = GPIO(GPIOE, 3);

int l3gd20_wr(struct stm32f_spi * spi, unsigned int reg, 
			  void * buf, unsigned int len)
{
	uint8_t * data = (uint8_t *)buf;
	unsigned int addr;
	unsigned int sr;
	unsigned int dummy;
	int i;

	if (len == 0)
		return 0;

	addr = (reg & 0x3f) | ((len > 1) ? 0x40 : 0x00);

	gpio_clr(l3gd20_cs);

	udelay(1);

	sr = spi->sr;
	
	if (!(sr & SPI_TXE))
		return -1;

	if (sr & SPI_MODF)
		return -2;

	if (sr & SPI_RXNE) {
		/* clear input buffer */
		dummy = spi->dr;
		(void)dummy;
	}

	/* send the address */
	spi->dr = addr;

	for (i = 0; i < len; ++i) {

		while (!((sr = spi->sr) & SPI_TXE)) {
			thinkos_irq_wait(STM32F_IRQ_SPI1);
		} 

		/* send the data */
		spi->dr = data[i];

		/* wait for incomming data */
		while (!((sr = spi->sr) & SPI_RXNE)) {
			thinkos_irq_wait(STM32F_IRQ_SPI1);
		} 

		/* discard */
		dummy = spi->dr;
		(void)dummy;
	}

	while (!((sr = spi->sr) & SPI_RXNE)) {
		thinkos_irq_wait(STM32F_IRQ_SPI1);
	}

	dummy = spi->dr;
	(void)dummy;

	udelay(1);

	gpio_set(l3gd20_cs);

	return len;
}

int l3gd20_rd(struct stm32f_spi * spi, int reg, 
			  void * buf, unsigned int len)
{
	uint8_t * data = (uint8_t *)buf;
	unsigned int addr;
	unsigned int sr;
	unsigned int dummy;
	int i;

	if (len == 0)
		return 0;

	addr = (reg & 0x3f) | 0x80 | ((len > 1) ? 0x40 : 0x00);

	gpio_clr(l3gd20_cs);

	udelay(1);

	sr = spi->sr;
	
	if (!(sr & SPI_TXE))
		return -1;

	if (sr & SPI_MODF)
		return -2;

	if (sr & SPI_RXNE) {
		/* clear input buffer */
		dummy = spi->dr;
		(void)dummy;
	}

	/* send the address */
	spi->dr = addr;

	while (!((sr = spi->sr) & SPI_TXE)) {
		thinkos_irq_wait(STM32F_IRQ_SPI1);
	} 

	/* send first dummy data */
	spi->dr = 0;

	/* wait for incomming data */
	while (!((sr = spi->sr) & SPI_RXNE)) {
		thinkos_irq_wait(STM32F_IRQ_SPI1);
	} 

	/* discard */
	dummy = spi->dr;
	(void)dummy;

	for (i = 0; i < (len - 1); ++i) {
		while (!((sr = spi->sr) & SPI_TXE)) {
			thinkos_irq_wait(STM32F_IRQ_SPI1);
		} 

		/* send dummy data */
		spi->dr = 0;

		/* wait for incomming data */
		while (!((sr = spi->sr) & SPI_RXNE)) {
			thinkos_irq_wait(STM32F_IRQ_SPI1);
		} 

		data[i] = spi->dr;
	}

	while (!((sr = spi->sr) & SPI_RXNE)) {
		thinkos_irq_wait(STM32F_IRQ_SPI1);
	}

	data[i] = spi->dr;

	udelay(1);

	gpio_set(l3gd20_cs);

	return len;
}

int l3gd20_init(void)
{
	struct stm32f_spi * spi = STM32F_SPI1;

	gpio_io_t io;

	io = l3gd20_cs ;
	stm32_gpio_clock_en(STM32_GPIO(io.port));
	stm32_gpio_mode(STM32_GPIO(io.port), io.pin, OUTPUT, SPEED_MED);
	gpio_set(io);

	stm32f_spi_init(spi, &spi1_io, 500000, SPI_MSTR | SPI_CPOL | SPI_CPHA);

	spi->cr2 = SPI_TXEIE | SPI_RXNEIE;

	return 0;
}


