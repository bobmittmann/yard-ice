/* 
 * File:	 io.c
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(C) 2013 Bob Mittmann. All Rights Reserved.
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
#include <stdint.h>

#include "io.h"

#define POLL_PERIOD_MS 32

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
	{ STM32F_GPIOC, 1 },
	{ STM32F_GPIOC, 14 },
	{ STM32F_GPIOC, 7 },
	{ STM32F_GPIOC, 8 }
};

#define UNLOCKED -1

struct {
	uint8_t tmr[sizeof(led_io) / sizeof(struct stm32f_io)];
} led_drv;

void led_flash(int id, int ms)
{
	led_drv.tmr[id] = ms / POLL_PERIOD_MS;
	stm32f_gpio_set(led_io[id].gpio, led_io[id].pin);
}

void leds_all_off(void)
{
	int i;

	for (i = 0; i < sizeof(led_io) / sizeof(struct stm32f_io); ++i)
		stm32f_gpio_clr(led_io[i].gpio, led_io[i].pin);
}

void leds_all_on(void)
{
	int i;

	for (i = 0; i < sizeof(led_io) / sizeof(struct stm32f_io); ++i)
		stm32f_gpio_set(led_io[i].gpio, led_io[i].pin);
}

void leds_all_flash(int ms)
{
	int i;

	for (i = 0; i < sizeof(led_io) / sizeof(struct stm32f_io); ++i) {
		led_drv.tmr[i] = ms / POLL_PERIOD_MS;
		stm32f_gpio_set(led_io[i].gpio, led_io[i].pin);
	}
}

static void leds_init(void)
{
	int i;

	for (i = 0; i < sizeof(led_io) / sizeof(struct stm32f_io); ++i) {
		stm32f_gpio_mode(led_io[i].gpio, led_io[i].pin,
						 OUTPUT, PUSH_PULL | SPEED_LOW);

		stm32f_gpio_clr(led_io[i].gpio, led_io[i].pin);
	}
}



/* ----------------------------------------------------------------------
 * I/O 
 * ----------------------------------------------------------------------
 */

void stm32f_tim2_isr(void)
{
	struct stm32f_tim * tim = STM32F_TIM2;
	int i;

	/* Clear interrupt flags */
	tim->sr = 0;

	/* process led timers */
	for (i = 0; i < sizeof(led_io) / sizeof(struct stm32f_io); ++i) {
		if (led_drv.tmr[i] == 0)
			continue;
		if (--led_drv.tmr[i] == 0) 
			stm32f_gpio_clr(led_io[i].gpio, led_io[i].pin);
	}
}


static void io_timer_init(uint32_t freq)
{
	struct stm32f_rcc * rcc = STM32F_RCC;
	struct stm32f_tim * tim = STM32F_TIM2;
	uint32_t div;
	uint32_t pre;
	uint32_t n;

	/* get the total divisior */
	div = ((2 * stm32f_apb1_hz) + (freq / 2)) / freq;
	/* get the minimum pre scaler */
	pre = (div / 65536) + 1;
	/* get the reload register value */
	n = (div * 2 + pre) / (2 * pre);

	/* Timer clock enable */
	rcc->apb1enr |= RCC_TIM2EN;
	
	/* Timer configuration */
	tim->psc = pre - 1;
	tim->arr = n - 1;
	tim->cnt = 0;
	tim->egr = 0;
	tim->dier = TIM_UIE; /* Update interrupt enable */
	tim->ccmr1 = TIM_OC1M_PWM_MODE1;
	tim->ccr1 = tim->arr / 2;

	cm3_irq_pri_set(STM32F_IRQ_TIM2, (4 << 5));
	/* Enable interrupt */
	cm3_irq_enable(STM32F_IRQ_TIM2);

	tim->cr1 = TIM_URS | TIM_CEN; /* Enable counter */
}

void io_init(void)
{
	/* Enable IO clocks */
	stm32f_gpio_clock_en(STM32F_GPIOA);
	stm32f_gpio_clock_en(STM32F_GPIOB);
	stm32f_gpio_clock_en(STM32F_GPIOC);

	stm32f_gpio_mode(STM32F_GPIOB, 10, INPUT, 0);
	stm32f_gpio_mode(STM32F_GPIOB, 11, INPUT, 0);

	leds_init();

	io_timer_init(1000 / POLL_PERIOD_MS);
}


