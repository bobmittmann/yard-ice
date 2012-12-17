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


#include <arch/cortex-m3.h>
#include <sys/delay.h>
#include <sys/serial.h>
#include <sys/stm32f.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <thinkos.h>
#include <hexdump.h>

#include <yard-ice/drv.h>
#include <sys/dcclog.h>

/*--------------------------------------------------------------------------
 * SPI
 ---------------------------------------------------------------------------*/

/* SPI */
static void io_miso_cfg(struct stm32f_gpio * gpio, int pin, int af)
{
	stm32f_gpio_clock_en(gpio);
	stm32f_gpio_mode(gpio, pin, ALT_FUNC, PULL_UP);
	stm32f_gpio_af(gpio, pin, af);
}

static void io_mosi_cfg(struct stm32f_gpio * gpio, int pin, int af)
{
	stm32f_gpio_clock_en(gpio);
	stm32f_gpio_mode(gpio, pin, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	stm32f_gpio_af(gpio, pin, af);
}

static void io_sck_cfg(struct stm32f_gpio * gpio, int pin, int af)
{
	stm32f_gpio_clock_en(gpio);
	stm32f_gpio_mode(gpio, pin, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	stm32f_gpio_af(gpio, pin, af);
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


/*--------------------------------------------------------------------------
 * Altera 
 ---------------------------------------------------------------------------*/

static void gpio_io_config(gpio_io_t io, int mode, int opt)
{
	struct stm32f_gpio * gpio = STM32F_GPIO(io.port);

	stm32f_gpio_clock_en(gpio);
	stm32f_gpio_mode(gpio, io.pin, mode, opt);
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
		return -4;

	return 0;
}

void conf_wr(int c)
{
	struct stm32f_spi * spi = STM32F_SPI3;

	spi_putc(spi, c);

	if ((c = spi_getc(spi)) >= 0) {
		if (c != 0xff) {
			printf("%02x", c);
		}
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
		conf_wr(buf[n]);
		n++;
		if (n > len) {
			return -6;
		}
	}

	return 0;
}

static void mco2_cfg(void)
{
	struct stm32f_gpio * gpio = STM32F_GPIOC;
	struct stm32f_syscfg * syscfg = STM32F_SYSCFG;
	int pin = 9;

	stm32f_gpio_clock_en(gpio);
	stm32f_gpio_mode(gpio, pin, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32f_gpio_af(gpio, pin, GPIO_AF0);

    /* enable I/O compensation cell */
	syscfg->cmpcr |= SYSCFG_CMP_PD;
}

gpio_io_t fsmc_io[] = {
	GPIO(PD, 14), /* D0 */
	GPIO(PD, 15), /* D1 */
	GPIO(PD, 0),  /* D2 */
	GPIO(PD, 1),  /* D3 */
	GPIO(PE, 7),  /* D4 */
	GPIO(PE, 8),  /* D5 */
	GPIO(PE, 9),  /* D6 */
	GPIO(PE, 10), /* D7 */
	GPIO(PE, 11), /* D8 */
	GPIO(PE, 12), /* D9 */
	GPIO(PE, 13), /* D10 */
	GPIO(PE, 14), /* D11 */
	GPIO(PE, 15), /* D12 */
	GPIO(PD, 8),  /* D13 */
	GPIO(PD, 9),  /* D14 */
	GPIO(PD, 10), /* D15 */
	GPIO(PD, 3), /* CLK */
	GPIO(PD, 4), /* NOE */
	GPIO(PD, 5), /* NWE */
	GPIO(PD, 7), /* NE1 */
//	GPIO(PD, 6), /* NWAIT */
	GPIO(PB, 7), /* NL */
};

void fsmc_speed(int div)
{
	struct stm32f_fsmc * fsmc = STM32F_FSMC;

	fsmc->btr1 = FSMC_ACCMOD_A | FSMC_DATLAT_SET(0) |
		FSMC_CLKDIV_SET(div) | FSMC_BUSTURN_SET(0) |
		FSMC_DATAST_SET(0) | FSMC_ADDHDL_SET(0) |
		FSMC_ADDSET_SET(0);
}

void fsmc_init(void)
{
	struct stm32f_fsmc * fsmc = STM32F_FSMC;
	struct stm32f_rcc * rcc = STM32F_RCC;
	gpio_io_t io;
	int i;

	mco2_cfg();

	/* Flexible static memory controller module clock enable */
	rcc->ahb3enr |= RCC_FSMCEN;

	/* Configur IO pins */
	stm32f_gpio_clock_en(STM32F_GPIO(PD));
	stm32f_gpio_clock_en(STM32F_GPIO(PE));
	for (i = 0; i < sizeof(fsmc_io) / sizeof(gpio_io_t); i++) {
		io = fsmc_io[i];
		stm32f_gpio_mode(STM32F_GPIO(io.port), io.pin, 
						 ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32f_gpio_af(STM32F_GPIO(io.port), io.pin, GPIO_AF12);
	}

	stm32f_gpio_mode(STM32F_GPIO(PD), 6, INPUT, PUSH_PULL | SPEED_HIGH);

	fsmc->bcr1 =
//		FSMC_EXTMOD |
		FSMC_CBURSTRW |	
		FSMC_WREN | 
		FSMC_BURSTEN | 
		FSMC_MWID_16 | 
		FSMC_MTYP_PSRAM | 
		FSMC_MUXEN | /* Address/Data multiplexed */
		FSMC_MBKEN |
		FSMC_WAITEN |
		FSMC_WAITPOL |
		0;
	
	fsmc->btr1 = FSMC_ACCMOD_A | FSMC_DATLAT_SET(0) |
		FSMC_CLKDIV_SET(3) | FSMC_BUSTURN_SET(0) |
		FSMC_DATAST_SET(0) | FSMC_ADDHDL_SET(0) |
		FSMC_ADDSET_SET(0);

	fsmc->bwtr1 = FSMC_ACCMOD_A | FSMC_DATLAT_SET(0) |
		FSMC_CLKDIV_SET(1) | FSMC_BUSTURN_SET(0) |
		FSMC_DATAST_SET(0) | FSMC_ADDHDL_SET(0) |
		FSMC_ADDSET_SET(0);
//	printf("fsmc->bwtr1=%08x\n", &fsmc->bwtr1);
}


struct fpga_io { 
	union {
		uint16_t h[2048];
		uint32_t w[1024];
		uint64_t d[512];
	};
	uint32_t res1[0x2000 - 1024];
	uint16_t mem[256];
	uint32_t res2[0x4000 - 0x2000 + 128];
	union {
		volatile uint16_t reg[8];
		volatile uint32_t r32[4];
		volatile uint64_t r64[2];
		struct {
			volatile uint16_t src;
			volatile uint16_t dst;
			volatile uint16_t len;
			volatile uint16_t ctl;
			volatile uint16_t cnt;
		};
	};
};

static inline void __strd(void * addr, uint64_t value) {
	asm volatile ("strd %0, %1, [%2]" : : "r" (value), 
				  "r" (value >> 32), "r" (addr));
}

static inline uint64_t __ldrd(void * addr) {
	register uint32_t rl;
	register uint32_t rh;
	asm volatile ("ldrd %0, %1, [%2]" : "=r" (rl), "=r" (rh) : "r" (addr));
	return ((uint64_t)rh << 32) + rl;
}

void fill_up_64(struct fpga_io * fpga, uint64_t * buf, int len)
{
	int i;

	for (i = 0; i < len; i++)
		buf[i] = ((uint64_t)rand() << 32) + 0x4000000000000000LL +
			rand() + 0x40000000;
	for (i = 0; i < len; i++)
		fpga->d[i] = buf[i];
}

void fill_down_64(struct fpga_io * fpga, uint64_t * buf, int len)
{
	int i;

	for (i = 0; i < len; i++)
		buf[i] = ((uint64_t)rand() << 32) + 0x4000000000000000LL +
			rand() + 0x40000000;
	for (i = len - 1; i >= 0; i--)
		fpga->d[i] = buf[i];
}


bool cmp_64(struct fpga_io * fpga, uint64_t * buf, int len)
{
	uint64_t val;
	int i;

	for (i = 0; i < len; i++) {
//		val = __ldrd((void *)&fpga->d[i]);
		val = fpga->d[i];
		if (buf[i] != val)
			return false;
	}
	return true;
}

bool cmp_32(struct fpga_io * fpga, uint64_t * buf, int len)
{
	uint64_t val;
	int i;

	for (i = 0; i < len * 2; i += 2) {
		val = (uint64_t)(fpga->w[i]) + ((uint64_t)(fpga->w[i + 1]) << 32);
		if (buf[i / 2] != val)
			return false;
	}

	return true;
}

bool cmp_16(struct fpga_io * fpga, uint64_t * buf, int len)
{
	uint64_t val;
	int i;

	for (i = 0; i < len  * 4; i += 4) {
		val = (uint64_t)fpga->h[i] + 
			((uint64_t)fpga->h[i + 1] << 16) +
			((uint64_t)fpga->h[i + 2] << 32) +
			((uint64_t)fpga->h[i + 3] << 48);
		if (buf[i / 4] != val)
			return false;
	}

	return true;
}

void registers_test(struct fpga_io * fpga)
{
	uint32_t val;
	uint64_t v64;
	int i;

//	fpga->reg[7] = 88;
//	thinkos_sleep(1);
	v64 = 0x0004000300020001LL;
	fpga->r64[0] = v64;
	thinkos_sleep(1);
	fpga->r64[0] = v64;

/*	for (i = 0; i < 8; i++) {
		fpga->reg[i] = i;
		thinkos_sleep(1);
	}
*/
	for (i = 0; i < 8; i++) {
		val = fpga->reg[i];
		thinkos_sleep(1);
		printf("[%4d]-> %d\n", i, val);
	}
	thinkos_sleep(1000);
}

void io_test(struct fpga_io * fpga)
{
	uint32_t val;
	int i = 0;
	int r;
	int c;
	
	c = getchar();
	while (c != '\033') {
		r = c - '0';

		fpga->reg[r] = i;
		thinkos_sleep(100);
		val = fpga->reg[r];
		printf("%2d - r[%d]-> %d 0x%08x\n", i, r, val, fpga->r32[r >> 1]);
		i++;

		c = getchar();
	}
}

void memcpy_test(struct fpga_io * fpga)
{
	int i = 0;
	uint16_t buf[256];

	for (i = 0; i < 256; i++) {
		fpga->mem[i] = i;
	}

	thinkos_sleep(100);

	for (i = 0; i < 256; i++)
		buf[i] = fpga->mem[i];
	show_hex16(stdout, 0, buf, 512);

	fpga->src = 0;
	fpga->dst = 0;
	fpga->len = 128;
	fpga->ctl = 1;

	thinkos_sleep(100);

	printf("\n");
	for (i = 0; i < 256; i++)
		buf[i] = fpga->mem[i];
	show_hex16(stdout, 0, buf, 512);

	thinkos_sleep(1000);
}

void slow_test(struct fpga_io * fpga)
{
	int i;
	uint64_t v64;
	uint32_t count;

	for (i = 0; i < 64; i += 4) {
		v64 = i + ((i  + 1) << 16) + 
			((uint64_t)(i  + 2) << 32) + ((uint64_t)(i  + 3) << 48);
		fpga->d[i / 4] = v64;
		printf("%016llx\n", v64);
	}

	printf("\n");

	for (i = 0; i < 64; i += 4) {
//		fpga->src = 222;
		fpga->cnt = 100 + i;
		thinkos_sleep(1000);
		count = fpga->cnt;
		v64 = fpga->d[i / 4];
		printf("%016llx %d\n", v64, count);
	}
}

FILE * stdout = (FILE *)&stm32f_usart5_file;

int main(int argc, char ** argv)
{
	struct fpga_io * fpga =  (struct fpga_io *)STM32F_FSMC_NE1;
	uint8_t * rbf = (uint8_t *)0x08010000;
	uint64_t val;
	uint64_t buf[512];
	int ret;
	int i;
	int n = 512;

	DCC_LOG_CONNECT();
	DCC_LOG_INIT();

	cm3_udelay_calibrate();
	thinkos_init(THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(0));

	stdout = uart_console_open(115200, SERIAL_8N1);
	stdin = stdout;

	printf("\n");
	printf("------------------------------------------------------\n");
	printf("- FSMC test\n");
	printf("------------------------------------------------------\n");
	printf("\n");
	printf("\r\n");

	altera_io_init();
	spi_init();
	fsmc_init();

	if ((ret = altera_configure(rbf, 40000)) < 0) {
		printf(" # altera_configure() failed: %d!\n", ret);
		for(;;);
	};

	printf("- FPGA configuration done.\n");

	val = 0;

	fsmc_speed(7);

	memcpy_test(fpga);

	io_test(fpga);

	registers_test(fpga);

	slow_test(fpga);

	for (;;) {
		printf("- Write 64bits ascending\n");
		fill_up_64(fpga, buf, n);
		udelay(50000);

		printf("- Read 64bits... ");
		if (cmp_64(fpga, buf, n) == false) {
			printf("fail!\n");
		} else {
			printf("ok.\n");
		}
		delay(1);

		printf("- Write 64bits descending\n");
		fill_down_64(fpga, buf, n);
		udelay(50000);
		printf("- Read 64bits... ");
		if (cmp_64(fpga, buf, n) == false) {
			printf("fail!\n");
		} else {
			printf("ok.\n");
		}
		delay(1);

		printf("- Write 64bits\n");
		fill_up_64(fpga, buf, n);
		udelay(50000);
		printf("- Read 32bits... ");
		if (cmp_32(fpga, buf, n) == false) {
			printf("fail!\n");
		} else {
			printf("ok.\n");
		}
		delay(1);

		printf("- Write 64bits\n");
		fill_up_64(fpga, buf, n);
		udelay(50000);
		printf("- Read 16bits... ");
		if (cmp_16(fpga, buf, n) == false) {
			printf("fail!\n");
		} else {
			printf("ok.\n");
		}
		delay(1);

		printf("- Write 16bits\n");
		for (i = 0; i < n; i++)
			buf[i] = ((uint64_t)rand() << 32) + rand();
		for (i = 0; i < n * 4; i += 4) {
			val = buf[i / 4];
			fpga->h[i] = val;
			fpga->h[i + 1] = val >> 16;
			fpga->h[i + 2] = val >> 32;
			fpga->h[i + 3] = val >> 48;
		}
		udelay(50000);
		printf("- Read 64bits... ");
		if (cmp_64(fpga, buf, n) == false) {
			printf("fail!\n");
		} else {
			printf("ok.\n");
		}
		delay(1);

		printf("- Write 32bits\n");
		for (i = 0; i < n; i++)
			buf[i] = ((uint64_t)rand() << 32) + rand();
		for (i = 0; i < n * 2; i += 2) {
			val = buf[i / 2];
			fpga->w[i] = val;
			fpga->w[i + 1] = val >> 32;
		}
		udelay(50000);
		printf("- Read 64bits... ");
		if (cmp_64(fpga, buf, n) == false) {
			printf("fail!\n");
		} else {
			printf("ok.\n");
		}
		delay(1);
	}

	for (;;) {
		for (i = 0; i < 16; i++) {
			fpga->w[i] = (1 << i) + (1 << (31 - i));
		}

		for (i = 0; i < 8; i ++) {
			val = fpga->w[i];
			printf(" %08x", (int)val);
		}

		printf("\n");

		for (i = 8; i < 16; i ++) {
			val = fpga->w[i];
			printf(" %08x", (int)val);
		}

		printf("\n");
		udelay(2500000);
	}

	for (;;) {
		delay(2);
		printf("1.8V");
		val = fpga->h[0];
		fpga->h[10] = val;
		fpga->h[11] = val;

		delay(2);
		printf("2.5V");
		val = fpga->h[1];
		fpga->h[11] = val;

		delay(2);
		printf("3.0V");
		val = fpga->h[1];
		fpga->h[12] = val;

		delay(2);
		printf("3.3V");
		val = fpga->h[1];
		fpga->h[13] = val;
	}

	return 0;
}

