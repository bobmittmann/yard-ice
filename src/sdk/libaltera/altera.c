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

static int altera_io_init(void)
{
	gpio_io_t io;

	stm32f_gpio_clock_en(STM32F_GPIOB);
	stm32f_gpio_clock_en(STM32F_GPIOC);
	stm32f_gpio_clock_en(STM32F_GPIOE);

	io = n_config;
	stm32f_gpio_clock_en(STM32F_GPIO(io.port));
	stm32f_gpio_mode(STM32F_GPIO(io.port), io.pin, OUTPUT, SPEED_MED);

	io = conf_done;
	stm32f_gpio_clock_en(STM32F_GPIO(io.port));
	stm32f_gpio_mode(STM32F_GPIO(io.port), io.pin, INPUT, SPEED_MED);

	io = n_status;
	stm32f_gpio_clock_en(STM32F_GPIO(io.port));
	stm32f_gpio_mode(STM32F_GPIO(io.port), io.pin, INPUT, SPEED_MED);

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

	if ((c = stm32f_spi_getc(spi)) >= 0) {
//		if (c != 0xff)
//			printf("%02x", c);
	}
}

int altera_configure(const uint8_t * buf, unsigned int max)
{
	int n = 0;
	int ret;

	altera_io_init();
	
	stm32f_spi_init(STM32F_SPI3, &spi3_io, 2000000);

	DCC_LOG2(LOG_TRACE, "rbf=%08x max=%d", buf, max);

	while ((ret = conf_start()) < 0) {
		DCC_LOG(LOG_ERROR, "conf_start() failed!");
		return ret;
	}

	while (!gpio_status(conf_done)) {
		if (!gpio_status(n_status)) {
			return -5;
		};

		conf_wr(buf[n]);
		n++;
		if (n > max) {
			DCC_LOG2(LOG_ERROR, "n(%d) > max(%d)!", n, max);
			return -6;
		}
	}

	return 0;
}

