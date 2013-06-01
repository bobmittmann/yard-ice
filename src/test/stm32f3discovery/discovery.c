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

#include "lis302dl.h"

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

	printf("%s(): thread %d started.\n", __func__, thinkos_thread_self());

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

		thinkos_sleep(10);
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

uint32_t leds_stack[128];

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

	printf("%s(): leds_mutex=%d.\n", __func__, leds_mutex);

	thinkos_thread_create((void *)__leds_task, (void *)NULL,
						  leds_stack, sizeof(leds_stack), 
						  THINKOS_OPT_PRIORITY(2));

	thinkos_sleep(100);
}

/* ----------------------------------------------------------------------
 * Push button
 * ----------------------------------------------------------------------
 */

/* Button events */
enum {
	BTN_NONE = 0,
	BTN_PRESSED,
	BTN_RELEASED
};


int btn_st;
int btn_mutex;
volatile int btn_event;

#define PUSH_BTN STM32F_GPIOA, 0

static int __btn_task(void)
{
	int st;

	printf("%s(): thread %d started.\n", __func__, thinkos_thread_self());

	stm32f_gpio_clock_en(STM32F_GPIOA);
	stm32f_gpio_mode(PUSH_BTN, INPUT, 0);

	btn_st = stm32f_gpio_stat(PUSH_BTN) ? 1 : 0;

	for (;;) {

		thinkos_sleep(50);

		/* process push button */
		st = stm32f_gpio_stat(PUSH_BTN) ? 1 : 0;
		if (btn_st != st) {
			btn_st = st;
			thinkos_mutex_lock(btn_mutex);
			btn_event = st ? BTN_PRESSED : BTN_RELEASED;
			thinkos_mutex_unlock(btn_mutex);
		}
	}

	return 0;
}

uint32_t btn_stack[128];

static void btn_init(void)
{
	btn_mutex = thinkos_mutex_alloc();

	printf("%s(): btn_mutex=%d.\n", __func__, btn_mutex);

	thinkos_thread_create((void *)__btn_task, (void *)NULL,
						  btn_stack, sizeof(btn_stack), 
						  THINKOS_OPT_PRIORITY(2));
	thinkos_sleep(100);
}

int btn_event_get(void)
{
	int event;

	thinkos_mutex_lock(btn_mutex);
	event = btn_event;
	btn_event = BTN_NONE;
	thinkos_mutex_unlock(btn_mutex);

	return event;
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


#define AVG_N 16

struct acc_info {
	volatile int y;
	volatile int x;
	volatile int z;
	volatile bool cal_req;
	int sem;
};

static int accelerometer_task(struct acc_info * acc)
{
	struct {
		int8_t x;
		uint8_t res1;
		int8_t y;
		uint8_t res2;
		int8_t z;
	} data;
	uint8_t cfg[4];
	uint8_t st;
	int x = 0;
	int y = 0;
	int z = 0;
	int x_off = 0;
	int y_off = 0;
	int z_off = 0;

	printf("%s(): thread %d started.\n", __func__, thinkos_thread_self());

	if (lis302_init() < 0) {
		return -1;
	}

	cfg[0] = CTRL_PD | CTRL_ZEN | CTRL_YEN | CTRL_XEN;
	cfg[1] = 0;
	cfg[3] = 0;
	lis302_wr(LIS302_CTRL_REG1, cfg, 3);

	for (; ;) {
		thinkos_sleep(1);
		/* poll the sensor */
		lis302_rd(LIS302_STATUS_REG, &st, 1);

		if (st & STAT_ZYXDA) {
			/* get the forces data */
			lis302_rd(LIS302_OUT_X, &data, 5);

			/* Filter */
			x = (x * (AVG_N - 1) / AVG_N) + data.x;
			y = (y * (AVG_N - 1) / AVG_N) + data.y;
			z = (z * (AVG_N - 1) / AVG_N) + data.z;

			if (acc->cal_req) {
				x_off = -x;
				y_off = -y;
				z_off = -z;
				acc->cal_req = false;
			}

			acc->x = x_off + x;
			acc->y = y_off + y;
			acc->z = z_off + z;

			thinkos_sem_post(acc->sem);
		} 
	}
}

void stdio_init(void);

uint32_t accelerometer_stack[128];

int main(int argc, char ** argv)
{
	struct acc_info acc;
	uint32_t cnt = 0;
	int x;
	int y;
	int i;

	/* calibrate usecond delay loop */
	cm3_udelay_calibrate();

	/* Initialize the stdin, stdout and stderr */
	stdio_init();

	/* Print a useful information message */
	printf("\n");
	printf("---------------------------------------------------------\n");
	printf(" STM32F4 Discovery example\n");
	printf("---------------------------------------------------------\n");
	printf("\n");

	thinkos_init(THINKOS_OPT_PRIORITY(4) | THINKOS_OPT_ID(4));

	leds_init();
	btn_init();

	acc.sem = thinkos_sem_alloc(0);
	printf("%s(): acc.sem=%d.\n", __func__, acc.sem);
	thinkos_thread_create((void *)accelerometer_task, (void *)&acc,
						  accelerometer_stack, sizeof(accelerometer_stack), 
						  THINKOS_OPT_PRIORITY(1));

	for (i = 0; ; ++i) {
		thinkos_sem_wait(acc.sem);

		if (btn_event_get() == BTN_PRESSED) {
			/* request calibration */
			acc.cal_req = true;
		}
	
		/* Scale */
		x = acc.x * 64 / 512;
		y = acc.y * 64 / 512;

		if ((++cnt & 0x03) == 0) {
			printf("%5d,%5d\r", x, y);
		}

		if (x == 0) {
			led_on(1);
			led_on(3);
		} else if (x < 0) {
			led_blink(3, -x);
			led_off(1);
		} else {
			led_blink(1, x);
			led_off(3);
		}

		if (y == 0) {
			led_on(0);
			led_on(2);
		} else if (y < 0) {
			led_off(0);
			led_blink(2, -y);
		} else {
			led_off(2);
			led_blink(0, y);
		}
	}

	return 0;
}

