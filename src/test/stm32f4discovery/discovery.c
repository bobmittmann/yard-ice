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
#include <arch/cortex-m3.h>
#include <sys/serial.h>
#include <sys/delay.h>
#include <sys/dcclog.h>

#include <thinkos.h>


/* GPIO pin description */ 
struct stm32f_io {
	struct stm32f_gpio * gpio;
	uint8_t pin;
};

/* ----------------------------------------------------------------------
 * LEDs 
 * ----------------------------------------------------------------------
 */

const struct stm32f_io led_io[] = {
	{ STM32F_GPIOD, 12 }, /* LED4 */
	{ STM32F_GPIOD, 13 }, /* LED3 */
	{ STM32F_GPIOD, 14 }, /* LED5 */
	{ STM32F_GPIOD, 15 }, /* LED6 */
};


#define LED_COUNT (sizeof(led_io) / sizeof(struct stm32f_io))

static volatile uint8_t led_state[LED_COUNT];

static volatile unsigned int led_rate[LED_COUNT];

static int leds_mutex;

static inline void __led_on(int id)
{
	stm32f_gpio_set(led_io[id].gpio, led_io[id].pin);
	led_state[id] = 1;
}

static inline void __led_off(int id)
{
	stm32f_gpio_clr(led_io[id].gpio, led_io[id].pin);
	led_state[id] = 0;
}

static int __leds_task(void)
{
	unsigned int tmr[4];
	unsigned int i;
	unsigned int rate;
	unsigned int tm;

	for (i = 0; i < LED_COUNT; ++i)
		tmr[i] = led_rate[i];

	for (;;) {

		thinkos_mutex_lock(leds_mutex); 

		for (i = 0; i < LED_COUNT; ++i) {

			if ((rate = led_rate[i]) == 0) {
				continue;
			}

			tm = MIN(tmr[i], rate);
			
			if (tm == 0) {
				tmr[i] = rate;
				if (led_state[i]) {
					__led_off(i);
				} else {
					__led_on(i);
				}
			} else {
				tmr[i] = tm - 1;
			}
		}

		thinkos_mutex_unlock(leds_mutex); 

		thinkos_sleep(5);
	}

	return 0;
}


void led_on(unsigned int id)
{
	thinkos_mutex_lock(leds_mutex); 

	led_rate[id] = 0;
	__led_on(id);

	thinkos_mutex_unlock(leds_mutex); 
}

void led_off(unsigned int id)
{
	thinkos_mutex_lock(leds_mutex); 

	led_rate[id] = 0;
	__led_off(id);

	thinkos_mutex_unlock(leds_mutex); 
}

void leds_all_off(void)
{
	int i;

	thinkos_mutex_lock(leds_mutex); 

	for (i = 0; i < LED_COUNT; ++i) {
		__led_off(i);
		led_rate[i] = 0;
	}

	thinkos_mutex_unlock(leds_mutex); 
}

void leds_all_on(void)
{
	int i;

	thinkos_mutex_lock(leds_mutex); 

	for (i = 0; i < LED_COUNT; ++i) {
		__led_on(i);
		led_state[i] = 1;
	}

	thinkos_mutex_unlock(leds_mutex); 
}

void led_blink(unsigned int id, unsigned int rate)
{
	thinkos_mutex_lock(leds_mutex); 

	led_rate[id] = rate;

	thinkos_mutex_unlock(leds_mutex); 
}

uint32_t leds_stack[64];

void leds_init(void)
{
	int i;

	stm32f_gpio_clock_en(STM32F_GPIOD);

	for (i = 0; i < LED_COUNT; ++i) {
		stm32f_gpio_mode(led_io[i].gpio, led_io[i].pin,
						 OUTPUT, PUSH_PULL | SPEED_LOW);

		__led_off(i);
		led_rate[i] = 0;
	}

	leds_mutex = thinkos_mutex_alloc();

	thinkos_thread_create((void *)__leds_task, (void *)NULL,
						  leds_stack, sizeof(leds_stack), 
						  THINKOS_OPT_PRIORITY(2));

}


/* ----------------------------------------------------------------------
 * Accelerometer
 * ----------------------------------------------------------------------
 */

static const struct stm32f_spi_io spi1_io = {
	.miso = GPIO(GPIOA, 6), /* MISO */
	.mosi = GPIO(GPIOA, 7), /* MOSI */
	.sck = GPIO(GPIOA, 5)  /* SCK */
};

static const gpio_io_t lis302_cs = GPIO(GPIOE, 3);


int spi_io(struct stm32f_spi * spi, unsigned int dout)
{
	unsigned int sr;
	int din = -1;

	sr = spi->sr;

	while (!(sr & SPI_TXE)) {
		thinkos_irq_wait(STM32F_IRQ_SPI1);
		if (sr & SPI_RXNE)
			din = spi->dr;
	}

	return din;
}

int lis302_wr(unsigned int reg, void * buf, unsigned int len)
{
	struct stm32f_spi * spi = STM32F_SPI1;
	uint8_t * data = (uint8_t *)buf;
	unsigned int addr;
	unsigned int sr;
	unsigned int dummy;
	int i;

	if (len == 0)
		return 0;

	addr = (reg & 0x3f) | ((len > 1) ? 0x40 : 0x00);

	gpio_clr(lis302_cs);

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

	gpio_set(lis302_cs);

	return len;
}

int lis302_rd(int reg, void * buf, unsigned int len)
{
	struct stm32f_spi * spi = STM32F_SPI1;
	uint8_t * data = (uint8_t *)buf;
	unsigned int addr;
	unsigned int sr;
	unsigned int dummy;
	int i;

	if (len == 0)
		return 0;

	addr = (reg & 0x3f) | 0x80 | ((len > 1) ? 0x40 : 0x00);

	gpio_clr(lis302_cs);

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

	gpio_set(lis302_cs);

	return len;
}

int lis302_init(void)
{
	struct stm32f_spi * spi = STM32F_SPI1;

	gpio_io_t io;

	io = lis302_cs ;
	stm32f_gpio_clock_en(STM32F_GPIO(io.port));
	stm32f_gpio_mode(STM32F_GPIO(io.port), io.pin, OUTPUT, SPEED_MED);
	gpio_set(io);

	stm32f_spi_init(spi, &spi1_io, 500000, SPI_MSTR | SPI_CPOL | SPI_CPHA);

	spi->cr2 = SPI_TXEIE | SPI_RXNEIE;

	return 0;
}




#define LIS302_WHO_AM_I         0x0f
#define LIS302_CTRL_REG1        0x20

#define CTRL_DR   (1 << 7)
#define CTRL_PD   (1 << 6)
#define CTRL_FS   (1 << 5)
#define CTRL_STP  (1 << 4)
#define CTRL_STM  (1 << 3)
#define CTRL_ZEN  (1 << 2)
#define CTRL_YEN  (1 << 1)
#define CTRL_XEN  (1 << 0)

#define LIS302_CTRL_REG2        0x21

#define CTRL_SIM  (1 << 7)
#define CTRL_BOOT (1 << 6)
#define CTRL_FDS  (1 << 4)

#define LIS302_CTRL_REG3        0x22
#define LIS302_HP_FILTER_RESET  0x23

#define LIS302_STATUS_REG       0x27

#define STAT_ZYXOR  (1 << 7)
#define STAT_ZOR    (1 << 6)
#define STAT_YOR    (1 << 5)
#define STAT_XOR    (1 << 4)

#define STAT_ZYXDA  (1 << 3)
#define STAT_ZDA    (1 << 2)
#define STAT_YDA    (1 << 1)
#define STAT_XDA    (1 << 0)

#define LIS302_OUT_X            0x29
#define LIS302_OUT_Y            0x2b
#define LIS302_OUT_Z            0x2d

#define LIS302_FF_WU_CFG_1      0x30
#define LIS302_FF_WU_SRC_1      0x31
#define LIS302_FF_WU_THS_1      0x32
#define LIS302_FF_WU_DURATION_1 0x33

#define LIS302_FF_WU_CFG_2      0x34
#define LIS302_FF_WU_SRC_2      0x35
#define LIS302_FF_WU_THS_2      0x36
#define LIS302_FF_WU_DURATION_2 0x37

#define LIS302_CLICK_CFG        0x38
#define LIS302_CLICK_SRC        0x39
#define LIS302_CLICK_THSY_X     0x3b
#define LIS302_CLICK_THSZ       0x3c
#define LIS302_CLICK_TIME_LIMIT 0x3d
#define LIS302_CLICK_LATENCY    0x3e
#define LIS302_CLICK_WINDOW     0x3f

struct lis302_data {
	int8_t x;
	uint8_t res1;
	int8_t y;
	uint8_t res2;
	int8_t z;
};

#define PA1 STM32F_GPIOA, 1
#define PA3 STM32F_GPIOA, 3

void stdio_init(void);

int main(int argc, char ** argv)
{
	struct lis302_data d;
	uint8_t cfg[4];
	uint8_t st;
	int x;
	int y;
	int i;

	/* calibrate usecond delay loop */
	cm3_udelay_calibrate();

	/* Initialize the stdin, stdout and stderr */
	stdio_init();

	stm32f_gpio_clock_en(STM32F_GPIOA);
	stm32f_gpio_mode(PA1, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32f_gpio_mode(PA3, OUTPUT, PUSH_PULL | SPEED_LOW);

	thinkos_init(THINKOS_OPT_PRIORITY(4) | THINKOS_OPT_ID(4));

	/* Print a useful information message */
	printf("\n");
	printf("---------------------------------------------------------\n");
	printf(" STM32F4 Discovery example\n");
	printf("---------------------------------------------------------\n");
	printf("\n");

	leds_init();

	if (lis302_init() < 0) {
		for (;;) {
			led_on(0);
			udelay(100000);
			led_off(0);
			udelay(100000);
		}
	}

	cfg[0] = CTRL_PD | CTRL_ZEN | CTRL_YEN | CTRL_XEN;
	cfg[1] = 0;
	cfg[3] = 0;
	lis302_wr(LIS302_CTRL_REG1, cfg, 3);

	led_blink(0, 100);
	led_blink(1, 100);
	led_blink(2, 100);
	led_blink(3, 100);

	for (i = 0; ; ++i) {
//		thinkos_sleep(1);

		lis302_rd(LIS302_STATUS_REG, &st, 1);

		if (st & STAT_ZYXDA) {

			stm32f_gpio_set(PA1);

			lis302_rd(LIS302_OUT_X, &d, 5);

			x = d.x;
			y = d.y;
			if (x < 0) {
				led_blink(1, 64 + x);
				led_off(3);
			} else {
				led_blink(3, 64 - x);
				led_off(1);
			}

			if (y < 0) {
				led_off(0);
				led_blink(2, 64 + y);
			} else {
				led_off(2);
				led_blink(0, 64 - y);
			}

			stm32f_gpio_clr(PA1);
		} 
	}

	return 0;
}

