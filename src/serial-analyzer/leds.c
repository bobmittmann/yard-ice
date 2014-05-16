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

#include <sys/stm32f.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <thinkos.h>
#include <sys/dcclog.h>

#define LED1_IO STM32F_GPIOA, 6
#define LED2_IO STM32F_GPIOA, 7

int8_t led_flag;
volatile uint8_t led1_flash_head;
volatile uint8_t led1_flash_tail;
volatile uint8_t led2_flash_head;
volatile uint8_t led2_flash_tail;
volatile int8_t led_locked;

void led_lock(void)
{
	led_locked = 1;
}

void led_unlock(void)
{
	led_locked = 0;
}

void led1_flash(unsigned int cnt)
{
	led1_flash_head = led1_flash_tail + cnt;
	DCC_LOG(LOG_MSG, "thinkos_flag_set()");
	thinkos_flag_set(led_flag);
}

void led2_flash(unsigned int cnt)
{
	led2_flash_head = led2_flash_tail + cnt;
	DCC_LOG(LOG_MSG, "thinkos_flag_set()");
	thinkos_flag_set(led_flag);
}

void led_flash_all(unsigned int cnt)
{
	led1_flash_head = led1_flash_tail + cnt;
	led2_flash_head = led2_flash_tail + cnt;
	thinkos_flag_set(led_flag);
}

void led1_on(void)
{
	struct stm32f_tim * tim = STM32F_TIM3;

	tim->ccr1 = tim->arr / 2;
}

void led1_off(void)
{
	struct stm32f_tim * tim = STM32F_TIM3;

	tim->ccr1 = 0;
}

void led2_on(void)
{
	struct stm32f_tim * tim = STM32F_TIM3;
	uint32_t arr = tim->arr;

	tim->ccr2 = arr - (arr / 2);
}

void led2_off(void)
{
	struct stm32f_tim * tim = STM32F_TIM3;

	tim->ccr2 = tim->arr;
}

int led_task(void)
{
	DCC_LOG1(LOG_TRACE, "[%d] started.", thinkos_thread_self());

	while (1) {
		DCC_LOG(LOG_MSG, "thinkos_flag_wait()...");
		thinkos_flag_wait(led_flag);
		if (led1_flash_tail != led1_flash_head) {
			led1_flash_tail++;
			if (!led_locked)
				led1_on();
		}
		if (led2_flash_tail != led2_flash_head) {
			led2_flash_tail++;
			if (!led_locked)
				led2_on();
		}

		if ((led1_flash_tail == led1_flash_head) &&
			(led2_flash_tail == led2_flash_head)) 
			thinkos_flag_clr(led_flag);

		thinkos_sleep(100);
		if (!led_locked) {
			led1_off();
			led2_off();
		}
		thinkos_sleep(100);
	}
}

static uint32_t __attribute__((aligned(8))) led_stack[32];

#define TIMER_PWM_FREQ 8000

void leds_init(void)
{
	struct stm32f_rcc * rcc = STM32F_RCC;
	struct stm32f_tim * tim = STM32F_TIM3;
	struct stm32f_afio * afio = STM32F_AFIO;
	uint32_t div;
	uint32_t pre;
	uint32_t n;

	afio->mapr |= AFIO_SPI1_REMAP;

	stm32f_gpio_mode(LED1_IO, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32f_gpio_mode(LED2_IO, ALT_FUNC, PUSH_PULL | SPEED_HIGH);

	/* get the total divisior */
	div = ((2 * stm32f_apb1_hz) + (TIMER_PWM_FREQ / 2)) / TIMER_PWM_FREQ;
	/* get the minimum pre scaler */
	pre = (div / 65536) + 1;
	/* get the reload register value */
	n = (div + pre / 2) / pre;

	DCC_LOG2(LOG_TRACE, "div=%d pre=%d", div, pre);

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
	DCC_LOG1(LOG_TRACE, "ARR=%d", tim->arr);
	tim->ccr1 = 0;
	tim->ccr2 = tim->arr;
	tim->ccmr1 = TIM_OC1M_PWM_MODE1 | TIM_OC1PE | \
				 TIM_OC2M_PWM_MODE2 | TIM_OC2PE;
	tim->ccer = TIM_CC1E | TIM_CC2E;
	tim->bdtr = TIM_MOE | TIM_OSSR;

	/* enable counter */
	tim->cr2 = 0;
	tim->cr1 = TIM_URS | TIM_CEN; 

	led_flag = thinkos_flag_alloc();

	thinkos_thread_create((void *)led_task, (void *)NULL,
						  led_stack, sizeof(led_stack),
						  THINKOS_OPT_PRIORITY(8) | THINKOS_OPT_ID(6));
}

