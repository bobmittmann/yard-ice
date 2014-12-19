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

#include "trace.h"
#include "io.h"

#include <thinkos.h>
#define __THINKOS_IRQ__
#include <thinkos_irq.h>

#define POLL_PERIOD_MS 16

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
	int lock;
	uint8_t tmr[sizeof(led_io) / sizeof(struct stm32f_io)];
} led_drv;

void led_on(int id)
{
//	if ((led_drv.lock != UNLOCKED) && (led_drv.lock != thinkos_thread_self()))
//		return;

	stm32f_gpio_set(led_io[id].gpio, led_io[id].pin);
}

void led_off(int id)
{
//	if ((led_drv.lock != UNLOCKED) && (led_drv.lock != thinkos_thread_self()))
//		return;

	stm32f_gpio_clr(led_io[id].gpio, led_io[id].pin);
}

void led_flash(int id, int ms)
{
//	if ((led_drv.lock != UNLOCKED) && (led_drv.lock != thinkos_thread_self()))
//		return;

	led_drv.tmr[id] = (ms * (65536 / POLL_PERIOD_MS) + 32768) / 65536;
	stm32f_gpio_set(led_io[id].gpio, led_io[id].pin);
}

void leds_all_off(void)
{
	int i;

//	if ((led_drv.lock != UNLOCKED) && (led_drv.lock != thinkos_thread_self()))
//		return;

	for (i = 0; i < sizeof(led_io) / sizeof(struct stm32f_io); ++i)
		stm32f_gpio_clr(led_io[i].gpio, led_io[i].pin);
}

void leds_all_on(void)
{
	int i;

	if ((led_drv.lock != UNLOCKED) && (led_drv.lock != thinkos_thread_self()))
		return;

	for (i = 0; i < sizeof(led_io) / sizeof(struct stm32f_io); ++i)
		stm32f_gpio_set(led_io[i].gpio, led_io[i].pin);
}

void leds_all_flash(int ms)
{
	int i;

//	if ((led_drv.lock != UNLOCKED) && (led_drv.lock != thinkos_thread_self()))
//		return;

	for (i = 0; i < sizeof(led_io) / sizeof(struct stm32f_io); ++i) {
		led_drv.tmr[i] = (ms * (65536 / POLL_PERIOD_MS) + 32768) / 65536;
		stm32f_gpio_set(led_io[i].gpio, led_io[i].pin);
	}
}

void leds_lock(void)
{
	if (led_drv.lock != UNLOCKED)
		return;

	led_drv.lock = thinkos_thread_self();
}

void leds_unlock(void)
{
	if ((led_drv.lock != UNLOCKED) && (led_drv.lock != thinkos_thread_self()))
		return;

	led_drv.lock = UNLOCKED;
}


static void leds_init(void)
{
	int i;

	led_drv.lock = UNLOCKED;

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

#define PUSH_BTN STM32F_GPIOC, 9

/* Button internal events */
enum {
	BTN_PRESSED = 1,
	BTN_RELEASED,
	BTN_TIMEOUT
};

/* Button FSM states */
enum {
	BTN_FSM_IDLE = 0,
	BTN_FSM_CLICK1_WAIT,
	BTN_FSM_PRE_WAIT1,
	BTN_FSM_CLICK2_WAIT,
	BTN_FSM_HOLD_TIME_WAIT,
	BTN_FSM_CLICK_N_HOLD_TIME_WAIT,
	BTN_FSM_HOLD_RELEASE_WAIT,
};

struct btn_drv {
	uint8_t st;
	volatile uint16_t tmr;
	uint8_t fsm;
	int8_t event;
	int flag;
} btn_drv;

int btn_event_wait(void)
{
	int event = EVENT_NONE;
	int irq_ev;

//	tracef("%s(): wait...", __func__);
	thinkos_flag_wait(btn_drv.flag);

	/* FIXME: possible race condition on this variable */
	irq_ev = btn_drv.event;
	btn_drv.event = 0;

	thinkos_flag_clr(btn_drv.flag);

	switch (btn_drv.fsm) {
	case BTN_FSM_IDLE:
		if (irq_ev == BTN_PRESSED) {
			/* double click timer */
			btn_drv.tmr = 500 / POLL_PERIOD_MS;
			btn_drv.fsm = BTN_FSM_CLICK1_WAIT;
		}
		break;

	case BTN_FSM_CLICK1_WAIT:
		/* wait for button release after first press */
		if (irq_ev == BTN_RELEASED) {
			event = EVENT_CLICK;
			btn_drv.fsm = BTN_FSM_PRE_WAIT1;
		} else if (irq_ev == BTN_TIMEOUT) {
			/* hold timer */
			btn_drv.tmr = 500 / POLL_PERIOD_MS;;
			btn_drv.fsm = BTN_FSM_HOLD_TIME_WAIT;
		}
		break;

	case BTN_FSM_PRE_WAIT1:
		/* wait for button press after first press and release
		 inside the double click timer */
		if (irq_ev == BTN_PRESSED) {
			btn_drv.fsm = BTN_FSM_CLICK2_WAIT;
		} else if (irq_ev == BTN_TIMEOUT) {
			btn_drv.fsm = BTN_FSM_IDLE;
		}
		break;

	case BTN_FSM_CLICK2_WAIT:
		/* wait for button release after press/release/press
		 inside the double click timer */
		if (irq_ev == BTN_RELEASED) {
			event = EVENT_DBL_CLICK;
			btn_drv.tmr = 0;
			btn_drv.fsm = BTN_FSM_IDLE;
		} else if (irq_ev == BTN_TIMEOUT) {
			btn_drv.fsm = BTN_FSM_IDLE;
			/* hold and click timer */
			btn_drv.tmr = 500 / POLL_PERIOD_MS;
			btn_drv.fsm = BTN_FSM_CLICK_N_HOLD_TIME_WAIT;
		}
		break;


	case BTN_FSM_HOLD_TIME_WAIT:
		/* wait for button release after press
		 inside the hold timer */
		if (irq_ev == BTN_RELEASED) {
			btn_drv.tmr = 0;
			btn_drv.fsm = BTN_FSM_IDLE;
		} else if (irq_ev == BTN_TIMEOUT) {
			event = EVENT_HOLD1;
			/* long hold timer */
			btn_drv.tmr = 3500 / POLL_PERIOD_MS;
			btn_drv.fsm = BTN_FSM_HOLD_RELEASE_WAIT;
		}
		break;

	case BTN_FSM_CLICK_N_HOLD_TIME_WAIT:
		/* wait for button release after press/release/press
		 inside the hold timer */
		if (irq_ev == BTN_RELEASED) {
			btn_drv.tmr = 0;
			btn_drv.fsm = BTN_FSM_IDLE;
		} else if (irq_ev == BTN_TIMEOUT) {
			event = EVENT_CLICK_N_HOLD;
			/* long hold timer */
			btn_drv.tmr = 3500 / POLL_PERIOD_MS;
			btn_drv.fsm = BTN_FSM_HOLD_RELEASE_WAIT;
		}
		break;

	case BTN_FSM_HOLD_RELEASE_WAIT:
		/* wait for button release after the hold timer expired */
		if (irq_ev == BTN_RELEASED) {
			btn_drv.tmr = 0;
			btn_drv.fsm = BTN_FSM_IDLE;
		} else if (irq_ev == BTN_TIMEOUT) {
			event = EVENT_HOLD2;
			btn_drv.fsm = BTN_FSM_IDLE;
		}
		break;
	};

	return event;
}


static void btn_init(void)
{
	btn_drv.flag = thinkos_flag_alloc();
	btn_drv.st = stm32f_gpio_stat(PUSH_BTN) ? 0 : 1;
	btn_drv.fsm = BTN_FSM_IDLE;
}


/* ----------------------------------------------------------------------
 * I/O 
 * ----------------------------------------------------------------------
 */

void stm32f_tim2_isr(void)
{
	struct stm32f_tim * tim = STM32F_TIM2;
	int st;
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

	if ((btn_drv.tmr) && (--btn_drv.tmr == 0)) {
		/* process button timer */
		btn_drv.event = BTN_TIMEOUT;
		__thinkos_flag_signal(btn_drv.flag);
	} else {
		/* process push button */
		st = stm32f_gpio_stat(PUSH_BTN) ? 0 : 1;
		if (btn_drv.st != st) {
			btn_drv.st = st;
			btn_drv.event = st ? BTN_PRESSED : BTN_RELEASED;
			__thinkos_flag_signal(btn_drv.flag);
		}
	}
}

void __attribute__((noreturn)) io_task(void * arg)
{
	for (;;) {
		thinkos_irq_wait(STM32F_IRQ_TIM2);
		stm32f_tim2_isr();
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

	cm3_irq_pri_set(STM32F_IRQ_TIM2, IRQ_PRIORITY_LOW);
	/* Enable interrupt */
//	cm3_irq_enable(STM32F_IRQ_TIM2);

	tim->cr1 = TIM_URS | TIM_CEN; /* Enable counter */
}

uint32_t io_stack[64];

void io_init(void)
{
	/* Enable IO clocks */
	stm32f_gpio_clock_en(STM32F_GPIOA);
	stm32f_gpio_clock_en(STM32F_GPIOB);
	stm32f_gpio_clock_en(STM32F_GPIOC);

	stm32f_gpio_mode(PUSH_BTN, INPUT, PULL_UP);

	stm32f_gpio_mode(STM32F_GPIOB, 10, INPUT, 0);
	stm32f_gpio_mode(STM32F_GPIOB, 11, INPUT, 0);

	btn_init();
	leds_init();

	thinkos_thread_create((void *)io_task, (void *)NULL, 
						  io_stack, sizeof(io_stack), 15);

	io_timer_init(1000 / POLL_PERIOD_MS);
}


