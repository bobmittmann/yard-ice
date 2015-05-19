/* 
 * File:	 driver/io.c
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
#include <thinkos.h>
#include <stdlib.h>
#include <string.h>

#include "board.h"
#include "fzc_io.h"

#define IO_POLL_PERIOD_MS 50
#define IO_LED_COUNT 5

#define IO_RATE_SLOT_COUNT (1000 / (IO_POLL_PERIOD_MS))
#define IO_RATE_RESIDUAL (1000 % (IO_POLL_PERIOD_MS))

struct {
	uint32_t time;
	uint32_t rate_clk;
	uint8_t rate_slot;
	uint8_t sw_state;
	uint8_t sw_flag;
	uint8_t rate_io[IO_RATE_SLOT_COUNT];
} io_drv;

const struct {
	struct stm32_gpio * gpio;
	unsigned int pin;
} led_io[IO_LED_COUNT] = {
	{ IO_LED_TRBL },
	{ IO_LED_ALRM },
	{ IO_LED_AC_PWR },
	{ IO_LED_SPR },
	{ IO_LED_SLNC }
};

void led_on(unsigned int led)
{
	int i;

	for (i = 0; i < IO_RATE_SLOT_COUNT; ++i)
			io_drv.rate_io[i] |= (1 << led);
}

void led_off(unsigned int led)
{
	int i;

	for (i = 0; i < IO_RATE_SLOT_COUNT; ++i)
		io_drv.rate_io[i] &= ~(1 << led);
}

void led_set_rate(unsigned int led, unsigned int rate)
{
	int cnt;
	int n;
	int k;
	int j;
	int i;

	switch (rate) {
	case RATE_OFF:
	default:
		for (i = 0; i < IO_RATE_SLOT_COUNT; ++i)
			io_drv.rate_io[i] &= ~(1 << led);
		return;
	case RATE_STEADY:
		for (i = 0; i < IO_RATE_SLOT_COUNT; ++i)
			io_drv.rate_io[i] |= (1 << led);
		return;
	case RATE_1BPS:
		cnt = 1; 
		break;
	case RATE_2BPS:
		cnt = 2; 
		break;
	case RATE_3BPS:
		cnt = 3; 
		break;
	case RATE_4BPS:
		cnt = 4; 
		break;
	}

	n = 0;
	i = 0;
	k = IO_RATE_SLOT_COUNT / cnt;
	for (j = 0; j < cnt; ++j) {
		n += k;
		for (; i < n - k / 2; ++i)
			io_drv.rate_io[i] &= ~(1 << led);
		for (; i < n; ++i)
			io_drv.rate_io[i] |= (1 << led);
	}
}

int keypad_read(void)
{
	int sw;

	thinkos_flag_take(io_drv.sw_flag);
	if ((sw = ffs(io_drv.sw_state)) != 0) {
		io_drv.sw_state &= ~(1 << (sw - 1));
	}
	return sw;
}

int32_t clk_time = 0;

void __attribute__((noreturn)) io_task(void * arg)
{
	unsigned int sw0 = 0;
	unsigned int sw1 = 0;
	uint32_t clk = thinkos_clock();
	uint32_t clk_sec = clk + 1000;

	for (;;) {
		unsigned int sw;
		unsigned int d;
		int i;
		int j;

		clk += IO_POLL_PERIOD_MS;
		thinkos_alarm(clk);

		/* update clock time */
		if ((int32_t)(clk - clk_sec) >= 1000) {
			clk_time++;
			clk_sec += 1000;
		}

		/* process led rates */
		j = io_drv.rate_slot;
		for (i = 0; i < IO_LED_COUNT; ++i) {
			if (io_drv.rate_io[j] & (1 << i))
				__led_on(led_io[i].gpio, led_io[i].pin);
			else
				__led_off(led_io[i].gpio, led_io[i].pin);
		}
		if (++io_drv.rate_slot == IO_RATE_SLOT_COUNT)
			io_drv.rate_slot = 0;

		/* Front panel switches */
		sw = 0;
		sw |= stm32_gpio_stat(IO_SW_NAV0) ? 0 : (1 << 0);
		sw |= stm32_gpio_stat(IO_SW_NAV1) ? 0 : (1 << 1);
		sw |= stm32_gpio_stat(IO_SW_NAV2) ? 0 : (1 << 2);
		sw |= stm32_gpio_stat(IO_SW_NAV3) ? 0 : (1 << 3);
		sw |= stm32_gpio_stat(IO_SW_NAV4) ? 0 : (1 << 4);
		if (sw != sw0) {
			/* Debouncing */
			sw0 = sw;
		} if ((d = sw ^ sw1) != 0) {
			/* State change */
			sw1 = sw;
			if (sw != 0) {
				io_drv.sw_state = sw;
				thinkos_flag_give(io_drv.sw_flag);
			}
		}
	}
}

void io_init(void)
{
	static uint32_t io_stack[64];

	static const struct thinkos_thread_inf io_inf = {
		.stack_ptr = io_stack, 
		.stack_size = sizeof(io_stack), 
		.priority = 8,
		.thread_id = 30,
		.paused = 0,
		.tag = "IO"
	};

	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOA);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOB);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOC);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOD);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOE);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOF);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOG);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOI);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOJ);

	stm32_gpio_mode(IO_LED_TRBL, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32_gpio_mode(IO_LED_ALRM, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32_gpio_mode(IO_LED_AC_PWR, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32_gpio_mode(IO_LED_SPR, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32_gpio_mode(IO_LED_SLNC, OUTPUT, PUSH_PULL | SPEED_LOW);

	stm32_gpio_set(IO_LED_TRBL);
	stm32_gpio_set(IO_LED_ALRM);
	stm32_gpio_set(IO_LED_AC_PWR);
	stm32_gpio_set(IO_LED_SPR);
	stm32_gpio_set(IO_LED_SLNC);

	stm32_gpio_mode(IO_SW_NAV0, INPUT, PULL_UP);
	stm32_gpio_mode(IO_SW_NAV1, INPUT, PULL_UP);
	stm32_gpio_mode(IO_SW_NAV2, INPUT, PULL_UP);
	stm32_gpio_mode(IO_SW_NAV3, INPUT, PULL_UP);
	stm32_gpio_mode(IO_SW_NAV4, INPUT, PULL_UP);

	io_drv.sw_flag = thinkos_flag_alloc();

	thinkos_thread_create_inf((void *)io_task, NULL, &io_inf);
}

