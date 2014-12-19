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

#include <sys/dcclog.h>
#include <string.h>

const struct stm32f_spi * const stm32f_spi_lut[] = {
	STM32F_SPI1,
	STM32F_SPI2,
	STM32F_SPI3,
#ifdef STM32F_SPI4
	STM32F_SPI4
#endif
};

int stm32f_spi_lookup(struct stm32f_spi * spi)
{
	int id = sizeof(stm32f_spi_lut) / sizeof(struct stm32f_spi *);

	while ((--id >= 0) && (spi != stm32f_spi_lut[id]));

	return id;
}

/* SPI - alternate function and clock configuration table */
static const struct {
	uint8_t	af; /* Alternate function */
	uint8_t	ckbit : 5; /* clock bit */  
	uint8_t	apb2 : 1; /* APB1/APB2 */ 
} __attribute__((__packed__)) spi_cfg[] = {
	{ .af = GPIO_AF5, .ckbit = 12, .apb2 = 1},
	{ .af = GPIO_AF5, .ckbit = 14, .apb2 = 0},
	{ .af = GPIO_AF6, .ckbit = 15, .apb2 = 0},
#ifdef STM32F_SPI4
	/* FIXME: check the alternate function bit */
	{ .af = GPIO_AF5, .ckbit = 13, .apb2 = 1}
#endif
};

int stm32f_spi_init(struct stm32f_spi * spi, 
					const struct stm32f_spi_io * spi_io, 
					unsigned int freq, unsigned int opt)
{
	struct stm32_rcc * rcc = STM32_RCC;
	gpio_io_t io;
	uint32_t div;
	int br;
	int id;

	if ((id = stm32f_spi_lookup(spi)) < 0) {
		/* invalid SPI ??? */
		return id;
	}

	/* Configure IO pins */
	io = spi_io->miso;
	stm32_gpio_clock_en(STM32_GPIO(io.port));
	stm32_gpio_mode(STM32_GPIO(io.port), io.pin, ALT_FUNC, 
					 PULL_UP | SPEED_MED);
	stm32_gpio_af(STM32_GPIO(io.port), io.pin, spi_cfg[id].af);

	io = spi_io->mosi;
	stm32_gpio_clock_en(STM32_GPIO(io.port));
	stm32_gpio_mode(STM32_GPIO(io.port), io.pin, ALT_FUNC, 
					 PUSH_PULL | SPEED_MED);
	stm32_gpio_af(STM32_GPIO(io.port), io.pin, spi_cfg[id].af);

	io = spi_io->sck;
	stm32_gpio_clock_en(STM32_GPIO(io.port));
	stm32_gpio_mode(STM32_GPIO(io.port), io.pin, ALT_FUNC, 
					 PUSH_PULL | SPEED_MED);
	stm32_gpio_af(STM32_GPIO(io.port), io.pin, spi_cfg[id].af);

	/* Enable peripheral clock */
	if (spi_cfg[id].apb2) {
		rcc->apb2enr |= (1 << spi_cfg[id].ckbit);
		div = stm32f_apb2_hz / freq / 2;
	} else {
		rcc->apb1enr |= (1 << spi_cfg[id].ckbit);
		div = stm32f_apb1_hz / freq / 2;
	}

	br = 31 - __clz(div);
	if (div > (1 << br)) {
		br++;
	}
    DCC_LOG3(LOG_TRACE, "SPI id=%d div=%d br=%d", id, div, br);

	spi->cr1 = 0;
	spi->cr2 = 0;
	spi->i2scfgr = 0;
	spi->i2spr = 0;

	spi->cr1 = SPI_SPE | SPI_BR_SET(br) | opt | SPI_SSM | SPI_SSI;

#if 0
	spi->cr1 = SPI_SPE | SPI_MSTR | SPI_SSM | SPI_SSI | \
			   SPI_BR_SET(br) | SPI_LSBFIRST;
#endif

	return id;
}

int stm32f_spi_putc(struct stm32f_spi * spi, int c)
{
	unsigned int sr;

	while (!((sr = spi->sr) & SPI_TXE)) {
		if (sr & SPI_MODF) {
			return -1;
		}
	}

	spi->dr = c;

	return 0;
}

int stm32f_spi_getc(struct stm32f_spi * spi)
{
	if (spi->sr & SPI_RXNE) {
		return spi->dr;
	}

	return -1;
}

