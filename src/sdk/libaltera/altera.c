/* 
 * File:	 spi-test.c
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
#include <sys/delay.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include <sys/dcclog.h>

/*--------------------------------------------------------------------------
 * Altera 
 ---------------------------------------------------------------------------*/

static const gpio_io_t n_config = GPIO(GPIOE, 0);
static const gpio_io_t conf_done = GPIO(GPIOE, 1);
static const gpio_io_t n_status = GPIO(GPIOC, 11);

static const struct stm32f_spi_io spi3_io = {
	.miso = GPIO(GPIOC, 11), /* MISO */
	.mosi = GPIO(GPIOB, 5), /* MOSI */
	.sck = GPIO(GPIOC, 10)  /* SCK */
};

int altera_io_init(void)
{
	gpio_io_t io;

	stm32_gpio_clock_en(STM32_GPIOB);
	stm32_gpio_clock_en(STM32_GPIOC);
	stm32_gpio_clock_en(STM32_GPIOE);

	io = n_config;
	stm32_gpio_clock_en(STM32_GPIO(io.port));
	stm32_gpio_mode(STM32_GPIO(io.port), io.pin, OUTPUT, SPEED_MED);

	io = conf_done;
	stm32_gpio_clock_en(STM32_GPIO(io.port));
	stm32_gpio_mode(STM32_GPIO(io.port), io.pin, INPUT, SPEED_MED);

	io = n_status;
	stm32_gpio_clock_en(STM32_GPIO(io.port));
	stm32_gpio_mode(STM32_GPIO(io.port), io.pin, INPUT, SPEED_MED);

	gpio_set(n_config);
	udelay(40);

	return 0;
}

static inline int conf_start(void)
{
	int tmo;

	gpio_clr(n_config);
	udelay(1);

	if (gpio_status(n_status))
		return -1;

	if (gpio_status(conf_done))
		return -2;

	/* 40 uSec */
	udelay(40);

	gpio_set(n_config);

	udelay(40);

	if (gpio_status(conf_done))
		return -3;

	tmo = 500; /* 50 ms */
	while (!gpio_status(n_status)) {
		udelay(100);
		if (--tmo ==0)
			return -4;
	}

	return 0;
}

static void conf_wr(int c)
{
	struct stm32f_spi * spi = STM32F_SPI3;

	stm32f_spi_putc(spi, c);

	c = stm32f_spi_getc(spi);
	(void)c;
}

int altera_configure(const uint8_t * buf, unsigned int max)
{
	struct stm32f_spi * spi = STM32F_SPI3;
	unsigned int div;
	gpio_io_t io;
	int freq = 1000000;
	int n = 0;
	int ret;
	int br;
	int i;

	DCC_LOG2(LOG_TRACE, "rbf=%08x max=%d", buf, max);
	
	io = n_config;
	stm32_gpio_mode(STM32_GPIO(io.port), io.pin, OUTPUT, SPEED_MED);
	gpio_set(n_config);

	io = conf_done;
	stm32_gpio_mode(STM32_GPIO(io.port), io.pin, INPUT, PULL_UP | SPEED_MED);

	io = n_status;
	stm32_gpio_mode(STM32_GPIO(io.port), io.pin, INPUT, PULL_UP | SPEED_MED);

	io = spi3_io.sck;
	stm32_gpio_mode(STM32_GPIO(io.port), io.pin, 
					ALT_FUNC, PUSH_PULL | SPEED_MED);
	stm32_gpio_af(STM32_GPIO(io.port), io.pin, GPIO_AF6);

	io = spi3_io.mosi;
	stm32_gpio_mode(STM32_GPIO(io.port), io.pin, 
					ALT_FUNC, PUSH_PULL | SPEED_MED);
	stm32_gpio_af(STM32_GPIO(io.port), io.pin, GPIO_AF6);

	/* Enable peripheral clock */;
	stm32_clk_enable(STM32_RCC, STM32_CLK_SPI3);

	/* Configure SPI */
	div = stm32_clk_hz(STM32_CLK_SPI3) / freq / 2;
	br = 31 - __clz(div);
	if (div > (1 << br))
		br++;

    DCC_LOG3(LOG_TRACE, "SPI freq=%d div=%d br=%d", freq, div, br);

	spi->cr1 = 0;
	spi->cr2 = 0;
	spi->i2scfgr = 0;
	spi->i2spr = 0;

	/* Master mode, LSB first */
	spi->cr1 = SPI_SPE | SPI_BR_SET(br) | SPI_MSTR |
		SPI_SSM | SPI_SSI | SPI_LSBFIRST;

//#define SPI_CPOL (1 << 1)
//#define SPI_CPHA (1 << 0)
//	for (i = 0; i < 128; ++i) {
//		conf_wr(0xff);
//	}

	udelay(40);

	while ((ret = conf_start()) < 0) {
		DCC_LOG(LOG_ERROR, "conf_start() failed!");
		return ret;
	}

	while (!gpio_status(conf_done)) {
#if 0
		if ((n & 0x7) == 0) {
			DCC_LOG8(LOG_TRACE, "%02x %02x %02x %02x %02x %02x %02x %02x", 
				buf[n], buf[n + 1], buf[n + 2], buf[n + 3],
				buf[n + 4], buf[n + 5], buf[n + 6], buf[n + 7]);
		}
#endif
		if (!gpio_status(n_status)) {
			DCC_LOG1(LOG_ERROR, "nSTATUS low after %d bytes!", n);
			return -5;
		};

		conf_wr(buf[n]);
		n++;
		if (n > max) {
			for (i = 0; i < 5000000; ++i) {
//				udelay(100);
				conf_wr(0xff);
				if (gpio_status(conf_done)) {
					return 0;
				};
			}

			DCC_LOG2(LOG_ERROR, "n(%d) > max(%d)!", n, max);
			return -6;
		}
	}

	return n;
}

