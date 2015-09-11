/* 
 * File:	 leds.c
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

#include "board.h"

#include <sys/dcclog.h>

struct {
	uint8_t led_tmr[2];
} io_drv;

#define IO_POLL_PERIOD_MS 10

void led_on(unsigned int id)
{
	struct stm32f_tim * tim = STM32F_TIM3;
	uint32_t arr = tim->arr;

	if (id == LED_AMBER)
		tim->ccr1 = arr / 2;
	else if (id == LED_RED)
		tim->ccr2 = arr / 2;
}

void led_off(unsigned int id)
{
	struct stm32f_tim * tim = STM32F_TIM3;

	if (id == LED_AMBER)
		tim->ccr1 = 0;
	else if (id == LED_RED)
		tim->ccr2 = tim->arr;
}

void led_flash(unsigned int id, unsigned int ms)
{
	io_drv.led_tmr[id] = ms / IO_POLL_PERIOD_MS;
	led_on(id);
}

#ifndef LEDDRV_FLAG_NO
int8_t led_flag;
#endif

#ifdef LEDDRV_FLAG_NO
#define LED_FLAG (THINKOS_FLAG_BASE + LEDDRV_FLAG_NO)
#else
#define LED_FLAG led_flag
#endif

uint32_t clk_time = 0;

void __attribute__((noreturn)) led_task(void)
{
	uint32_t clk = __thinkos_ticks();
	uint32_t clk_sec = clk + 1000;

	DCC_LOG1(LOG_TRACE, "[%d] started.", thinkos_thread_self());

	for (;;) {
		int i;

		clk += IO_POLL_PERIOD_MS;
		thinkos_alarm(clk);

		/* update clock time */
		if ((int32_t)(clk - clk_sec) >= 1000) {
			clk_time++;
			clk_sec += 1000;
		}

		/* process led timers */
		for (i = 0; i < 2; ++i) {
			if (io_drv.led_tmr[i] == 0)
				continue;
			if (--io_drv.led_tmr[i] == 0) 
				led_off(i);
		}

	}
}

#define TIMER_PWM_FREQ 8000

void leds_init(void)
{
	struct stm32_rcc * rcc = STM32_RCC;
	struct stm32f_tim * tim = STM32F_TIM3;
	struct stm32_afio * afio = STM32_AFIO;
	uint32_t div;
	uint32_t pre;
	uint32_t n;

	afio->mapr |= AFIO_SPI1_REMAP;

	stm32_gpio_mode(LED1_IO, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_mode(LED2_IO, ALT_FUNC, PUSH_PULL | SPEED_HIGH);

	/* get the total divisior */
	div = ((2 * stm32f_apb1_hz) + (TIMER_PWM_FREQ / 2)) / TIMER_PWM_FREQ;
	/* get the minimum pre scaler */
	pre = (div / 65536) + 1;
	/* get the reload register value */
	n = (div + pre / 2) / pre;

	/* Timer clock enable */
	rcc->apb1enr |= RCC_TIM3EN;
	
	/* Timer configuration */
	tim->sr= 0; /* Clear interrupts */
	tim->psc = pre - 1;
	tim->arr = n - 1;

	tim->cnt = 0;
	tim->egr = 0;
	tim->rcr = 0;

	/* */
	tim->ccr1 = 0;
	tim->ccr2 = tim->arr;
	tim->ccmr1 = TIM_OC1M_PWM_MODE1 | TIM_OC1PE | \
				 TIM_OC2M_PWM_MODE2 | TIM_OC2PE;
	tim->ccer = TIM_CC1E | TIM_CC2E;
	tim->bdtr = TIM_MOE | TIM_OSSR;

	/* enable counter */
	tim->cr2 = 0;
	tim->cr1 = TIM_URS | TIM_CEN; 
}

