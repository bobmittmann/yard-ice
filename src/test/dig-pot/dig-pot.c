/* 
 * File:	 dig-pot.c
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
#include <arch/cortex-m3.h>
#include <sys/delay.h>
#include <sys/serial.h>
#include <sys/param.h>
#include <sys/file.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define __THINKOS_IRQ__
#include <thinkos_irq.h>
#include <thinkos.h>

#include <sys/dcclog.h>
#include <sys/usb-cdc.h>

/* GPIO pin description */ 
struct stm32f_io {
	struct stm32f_gpio * gpio;
	uint8_t pin;
};


/* 
 * MCP402X digital potentiometer protocol 
 * 
 * Increment:
 *
 *  __  ______                              _________
 *  CS        |___________________________|
 *            :
 *            :--LCUR---:--THI--:-TLO-:
 *                      :       :     :  
 *   _     _______       _______       _______
 * U/D  __|       |_____|       |_____|       |_______
 *                      :             :
 *                      :             :
 *                      :              ______________
 *   W                   _____________|
 *      ________________|
 *
 * Decrement:
 *
 *  __  ______                            ___________
 *  CS        |__________________________|
 *            :
 *            :-LCUR-:--THI--:--TLO--:
 *                   :       :       :  
 *   _                _______         _________    
 * U/D  _____________|       |_______|         |________
 *                   :               :
 *                   :               :
 *      _____________                :
 *   W               |_______________
 *                                   |__________________
 *
 * T_LCUR(MIN) = 3uS
 * T_LO(MIN) = 500nS
 * T_HI(MI) = 500nS
 * 
 */


#define DGPOT_STEPS 64
#define DGPOT_CHIPS 2

struct dgpot_drv {
	int8_t ev;
	int8_t mutex;
	uint8_t pos[DGPOT_CHIPS];
} dgpot;


#define DGPOT_NCS0 STM32F_GPIOB, 11
#define DGPOT_NCS1 STM32F_GPIOB, 12
#define DGPOT_UPDWN STM32F_GPIOB, 13

struct stm32f_io dgpot_cs[] = {
	{ DGPOT_NCS0 },
	{ DGPOT_NCS1 }
};

void stm32f_tim1_up_tim16_isr(void)
{
	struct stm32f_tim * tim = STM32F_TIM1;

	/* Clear interrupt flags */
	tim->sr = 0;

	stm32f_gpio_set(DGPOT_NCS0);
	stm32f_gpio_set(DGPOT_NCS1);

	DCC_LOG(LOG_MSG, "tick.");
}

#if 0
void stm32f_tim1_up_tim16_isr(void) 
	__attribute__ ((alias ("stm32f_tim1_up_isr")));
#endif

#define TIMER_CLK_FREQ 1800000

uint32_t dgpot_lcur;

static void dgpot_timer_init(void)
{
	struct stm32f_rcc * rcc = STM32F_RCC;
	struct stm32f_tim * tim = STM32F_TIM1;
	uint32_t div;
	uint32_t pre;
	uint32_t n;

	DCC_LOG(LOG_TRACE, "..");

	/* get the total divisior */
	div = ((2 * stm32f_apb2_hz) + (TIMER_CLK_FREQ / 2)) / TIMER_CLK_FREQ;
	/* get the minimum pre scaler */
	pre = (div / 65536) + 1;
	/* get the reload register value */
	n = (div + pre / 2) / pre;

	/* Timer clock enable */
	rcc->apb2enr |= RCC_TIM1EN;
	
	/* Timer configuration */
	tim->sr= 0; /* Clear interrupts */
	tim->psc = pre - 1;
	tim->arr = n - 1;

	dgpot_lcur = 6 * n - 1;

	tim->cnt = 0;
	tim->egr = 0;
	tim->rcr = 4;
	/* */
	DCC_LOG1(LOG_TRACE, "ARR=%d", tim->arr);
	tim->ccr1 = tim->arr / 2;
	tim->ccmr1 = TIM_OC1M_TOGGLE | TIM_OC1PE;
	tim->ccer = TIM_CC1NE;
	tim->bdtr = TIM_MOE | TIM_OSSR;
	/* Edge mode, down count, one pulse, enable counter */
	tim->cr2 = 0;
	tim->cr1 = TIM_CMS_EDGE | TIM_DIR_DOWN | TIM_OPM | TIM_URS | TIM_CEN; 
	tim->dier = TIM_UIE; /* Update interrupt enable */

	cm3_irq_enable(STM32F_IRQ_TIM1_UP);
}

void dgpot_set(unsigned int cs, unsigned int pos)
{
	struct stm32f_tim * tim = STM32F_TIM1;
	int diff;

	/* limit the range */
	if (pos > (DGPOT_STEPS - 1))
		pos = (DGPOT_STEPS - 1);

//	thinkos_mutex_lock(dgpot.mutex);

	diff = pos - dgpot.pos[cs];

	if (diff < 0) {
		/* decrement */
		diff = -diff;
		/* set the repetition counter register */
		tim->rcr = (diff * 2) - 2;
		/* force initial UP/DOWN level to LOW */
		tim->ccmr1 = TIM_OC1M_FORCE_INACTIVE;
	} else if (diff > 0) {
		/* increment */
		/* set the repetition counter register */
		tim->rcr = (diff * 2) - 1;
		/* force initial UP/DOWN level to HIGH */
		tim->ccmr1 = TIM_OC1M_FORCE_ACTIVE;
	}

	tim->ccmr1 = TIM_OC1M_TOGGLE | TIM_OC1PE;

	/* Generate an update event to start */
	tim->egr = TIM_UG;

	tim->cnt = dgpot_lcur;

	stm32f_gpio_clr(dgpot_cs[cs].gpio, dgpot_cs[cs].pin);

	tim->cr1 = TIM_CMS_EDGE | TIM_DIR_DOWN | TIM_OPM | TIM_URS | TIM_CEN; 

	/* wait for the end of adjustment ... */
//	__thinkos_critical_enter();
//	DCC_LOG(LOG_TRACE, "wait ...");
//	__thinkos_ev_wait(dev->ev);
//	DCC_LOG(LOG_TRACE, "wakeup");
//	__thinkos_critical_exit();

	dgpot.pos[cs] = pos;
//	thinkos_mutex_unlock(dgpot.mutex);
}

void dgpot_init(void)
{
	int i;

	stm32f_gpio_mode(DGPOT_NCS0, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32f_gpio_mode(DGPOT_NCS1, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32f_gpio_mode(DGPOT_UPDWN, ALT_FUNC, PUSH_PULL | SPEED_HIGH);

	stm32f_gpio_set(DGPOT_NCS0);
	stm32f_gpio_set(DGPOT_NCS1);

	dgpot_timer_init();

//	dgpot.mutex = thinkos_mutex_alloc();
//	dgpot.ev = thinkos_event_alloc();

	for (i = 0; i < DGPOT_CHIPS; ++i) {
		/* artificially set the potentiometer to the 
		   maximum level to allow zeroing */
		dgpot.pos[i] = DGPOT_STEPS - 1;
		dgpot_set(i, 0);
	}
}

#define USART1_TX STM32F_GPIOB, 6
#define USART1_RX STM32F_GPIOB, 7

const struct stm32f_io led_io[] = {
	{ STM32F_GPIOB, 14 },
	{ STM32F_GPIOB, 15 },
	{ STM32F_GPIOC, 13 },
	{ STM32F_GPIOC, 14 },
	{ STM32F_GPIOC, 15 }
};

void led_on(int id)
{
	stm32f_gpio_set(led_io[id].gpio, led_io[id].pin);
}

void led_off(int id)
{
	stm32f_gpio_clr(led_io[id].gpio, led_io[id].pin);
}

void leds_init(void)
{
	int i;

	for (i = 0; i < 5; ++i) {
		stm32f_gpio_mode(led_io[i].gpio, led_io[i].pin,
						 OUTPUT, PUSH_PULL | SPEED_LOW);

		stm32f_gpio_clr(led_io[i].gpio, led_io[i].pin);
	}
}

const struct stm32f_io relay_io[] = {
	{ STM32F_GPIOA, 9 },
	{ STM32F_GPIOA, 10 },
	{ STM32F_GPIOA, 11 },
	{ STM32F_GPIOA, 12 },
	{ STM32F_GPIOB, 2 }
};

void relay_on(int id)
{
	stm32f_gpio_set(relay_io[id].gpio, relay_io[id].pin);
}

void relay_off(int id)
{
	stm32f_gpio_clr(relay_io[id].gpio, relay_io[id].pin);
}

void relays_init(void)
{
	int i;

	for (i = 0; i < 5; ++i) {
		stm32f_gpio_mode(relay_io[i].gpio, relay_io[i].pin,
						 OUTPUT, PUSH_PULL | SPEED_LOW);

		stm32f_gpio_clr(relay_io[i].gpio, relay_io[i].pin);
	}
}

void io_init(void)
{
	struct stm32f_rcc * rcc = STM32F_RCC;
	struct stm32f_afio * afio = STM32F_AFIO;

	DCC_LOG(LOG_MSG, "Configuring GPIO pins...");

	stm32f_gpio_clock_en(STM32F_GPIOA);
	stm32f_gpio_clock_en(STM32F_GPIOB);
	stm32f_gpio_clock_en(STM32F_GPIOC);

	/* Enable Alternate Functions IO clock */
	rcc->apb2enr |= RCC_AFIOEN;

	/* USART1_TX */
	stm32f_gpio_mode(USART1_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	/* USART1_RX */
	stm32f_gpio_mode(USART1_RX, INPUT, PULL_UP);
	/* Use alternate pins for USART1 */
	afio->mapr |= AFIO_USART1_REMAP;
}

void self_test(void)
{
	int i;

	for (i = 0; i < 5; ++i) {
		relay_on(i);
		led_on(i);
		thinkos_sleep(100);
		relay_off(i);
		led_off(i);
		thinkos_sleep(100);
	}
}

struct file stm32f_uart1_file = {
	.data = STM32F_USART1, 
	.op = &stm32f_usart_fops 
};

void stdio_init(void)
{
	struct stm32f_usart * us = STM32F_USART1;

	stm32f_usart_init(us);
	stm32f_usart_baudrate_set(us, 115200);
	stm32f_usart_mode_set(us, SERIAL_8N1);
	stm32f_usart_enable(us);

	stdin = &stm32f_uart1_file;
	stdout = &stm32f_uart1_file;
	stderr = &stm32f_uart1_file;
}

int main(int argc, char ** argv)
{
	int i = 0;

	DCC_LOG_INIT();
	DCC_LOG_CONNECT();

	/* calibrate usecond delay loop */
	cm3_udelay_calibrate();

	DCC_LOG(LOG_TRACE, "1. io_init()");
	io_init();

	DCC_LOG(LOG_TRACE, "2. leds_init()");
	leds_init();

	DCC_LOG(LOG_TRACE, "3. relays_init()");
	relays_init();

	DCC_LOG(LOG_TRACE, "4. thinkos_init()");
	thinkos_init(THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(32));

	DCC_LOG(LOG_TRACE, "5. stdio_init()");
	stdio_init();

	DCC_LOG(LOG_TRACE, "6. dgpot_init()");
	dgpot_init();


	printf("Hello world!\n");
	printf("Hello world!\n");

	for (i = 0; ; i++) {
		printf("- %d\n", i);
		self_test();
	}

	for (i = 0; ; i++) {
		DCC_LOG1(LOG_TRACE, "%d", i);
		thinkos_sleep(1000);
		relay_on(1);
		led_on(1);
		dgpot_set(0, 0);

		thinkos_sleep(1000);
		relay_off(1);
		led_off(1);

		dgpot_set(0, 31);

		thinkos_sleep(1000);
		dgpot_set(0, 63);
		
	}

	return 0;
}

