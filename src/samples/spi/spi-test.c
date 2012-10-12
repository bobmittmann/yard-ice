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
#include <arch/cortex-m3.h>
#include <sys/delay.h>
#include <sys/serial.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

static void gpio_io_config(gpio_io_t io, int mode, int opt)
{
	struct stm32f_gpio * gpio = STM32F_GPIO(io.port);

	stm32f_gpio_clock_en(gpio);
	stm32f_gpio_mode(gpio, io.pin, mode, opt);
}

static void io_miso_cfg(struct stm32f_gpio * gpio, int port, int af)
{
	stm32f_gpio_clock_en(gpio);
	stm32f_gpio_mode(gpio, port, ALT_FUNC, PULL_UP);
	stm32f_gpio_af(gpio, port, af);
}

static void io_mosi_cfg(struct stm32f_gpio * gpio, int port, int af)
{
	stm32f_gpio_clock_en(gpio);
	stm32f_gpio_mode(gpio, port, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	stm32f_gpio_af(gpio, port, af);
}

static void io_sck_cfg(struct stm32f_gpio * gpio, int port, int af)
{
	stm32f_gpio_clock_en(gpio);
	stm32f_gpio_mode(gpio, port, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	stm32f_gpio_af(gpio, port, af);
}

int spi_init(void)
{
	struct stm32f_rcc * rcc = STM32F_RCC;
	struct stm32f_spi * spi = STM32F_SPI3;

	printf(" - Configuring SPI IO pins...\n");
	io_miso_cfg(STM32F_GPIOC, 11, GPIO_AF6);
	io_mosi_cfg(STM32F_GPIOB, 5, GPIO_AF6);
	io_sck_cfg(STM32F_GPIOC, 10, GPIO_AF6);

	/* Enable peripheral clock */
	printf(" - Enabling SPI clock...\n");
	rcc->apb1enr |= RCC_SPI3EN;

	spi->cr2 = 0;
	spi->i2scfgr = 0;
	spi->i2spr = 0;
	spi->cr1 = SPI_SPE | SPI_MSTR | SPI_SSM | SPI_SSI | \
			   SPI_BR_SET(5) | SPI_LSBFIRST;

	return 0;
}

static int inline spi_putc(struct stm32f_spi * spi, int c)
{
	unsigned int sr;
	while (!((sr = spi->sr) & SPI_TXE)) {
		if (sr & SPI_MODF) {
			printf("MODE FAULT\n");
			return -1;
		}
	}

	spi->dr = c;

	return 0;
}

static int inline spi_getc(struct stm32f_spi * spi)
{
	if (spi->sr & SPI_RXNE) {
		return spi->dr;
	}

	return -1;
}

gpio_io_t n_config = GPIO(PE, 0);
gpio_io_t conf_done = GPIO(PE, 1);
gpio_io_t n_status = GPIO(PC, 10);

int altera_io_init(void)
{
	gpio_io_config(n_config, OUTPUT, 0);
	gpio_io_config(conf_done, INPUT, 0);
	gpio_set(n_config);

	return 0;
}

static inline int conf_start(void)
{
//	struct stm32f_spi * spi = STM32F_SPI3;

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
#if 0
	/* 40 uSec */
	if (gpio_status(n_status)) {
		return -3;
	}
#endif
	if (gpio_status(conf_done))
		return -4;

	return 0;
}

void conf_wr(int c)
{
	struct stm32f_spi * spi = STM32F_SPI3;

	spi_putc(spi, c);

	if ((c = spi_getc(spi)) >= 0) {
		if (c != 0xff)
			printf("%02x", c);
	}
}

int altera_configure(const uint8_t * buf, int len)
{
	int n = 0;
	int ret;

	while ((ret = conf_start()) < 0) {
		return ret;
	}

	while (!gpio_status(conf_done)) {

/*		while (!gpio_status(n_status)) {
			return -5;
		} */

		conf_wr(buf[n]);
		n++;

		if (n > len) {
			return -6;
		}
	}

	return 0;
}

int altera_nstatus(void)
{
	return 0;
}

int altera_conf_done(void)
{
	return 0;
}

FILE * stdout = (FILE *)&stm32f_usart5_file;

int main(int argc, char ** argv)
{
	struct stm32f_usart * us = STM32F_USART5;
	struct stm32f_spi * spi = STM32F_SPI3;
	uint8_t * rbf = (uint8_t *)0x10000;
	int c;
	int ret;

	stm32f_usart_open(us, 115200, SERIAL_8N1);
	cm3_udelay_calibrate();

	printf("\n");
	printf("--------------------------------------------\n");
	printf("SPI test\n");
	printf("--------------------------------------------\n");
	printf("\n");

	altera_io_init();
	spi_init();

	if ((ret = altera_configure(rbf, 40000)) < 0) {
		printf(" # altera_configure() failed: %d!\n", ret);
	};

	printf("- FPGA configuration done.\n");

	for (;;) {
		udelay(50000);

		spi_putc(spi, 0x55);
		if ((c = spi_getc(spi)) >= 0) {
			printf("%02x", c);
		}
	}

	return 0;
}

