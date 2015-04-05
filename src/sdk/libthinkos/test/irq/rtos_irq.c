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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <arch/cortex-m3.h>
#include <sys/serial.h>
#include <sys/delay.h>

#include <thinkos.h>

#define __THINKOS_IRQ__
#include <thinkos_irq.h>

struct set {
	uint32_t tim6;
	uint32_t tim7;
	uint32_t tim9;
	uint32_t event;
};

struct {
	volatile struct set max;
	volatile struct set avg;
	volatile struct set ticks;
} meter;

int sem_timer; /* semaphore to signal a full buffer */
int flag_timer; /* event flag */

volatile bool req;
volatile bool ack;

void stm32f_tim7_isr(void)
{
	struct stm32f_tim * tim7 = STM32F_TIM7;
	uint32_t latency;
	uint32_t ticks;

	latency = tim7->cnt;

	if (tim7->sr == 0)
		return;
	/* Clear update interrupt flag */
	tim7->sr = 0;

	if (meter.max.tim7 < latency)
		meter.max.tim7 = latency;

	meter.avg.tim7 = (meter.avg.tim7 * 63) / 64 + latency;
	ticks = meter.ticks.tim7;
	meter.ticks.tim7 = ticks + 1;

	if (req & !ack) {
		thinkos_flag_give(flag_timer);
		ack = true;
		req = false;
	}
#if 0
	if (ticks & 1)
		stm32f_gpio_set(STM32F_GPIOB, 6);
	else
		stm32f_gpio_clr(STM32F_GPIOB, 6);
#endif
}

void stm32f_tim1_brk_tim9_isr(void)
{
	struct stm32f_tim * tim9 = STM32F_TIM9;
	uint32_t latency;
	uint32_t ticks;

	latency = tim9->cnt;
	if (tim9->sr == 0)
		return;
	/* Clear update interrupt flag */
	tim9->sr = 0;

	if (meter.max.tim9 < latency)
		meter.max.tim9 = latency;

	meter.avg.tim9 = (meter.avg.tim9 * 63) / 64 + latency;
	ticks = meter.ticks.tim9++;
	meter.ticks.tim9 = ticks + 1;

	/* This is a low priority interrupt handler, we can 
	   invoque service calls from it. */
	thinkos_sem_post(sem_timer);
}

void tim7_init(void)
{
	struct stm32f_tim * tim7 = STM32F_TIM7;

	/* clock enable */
	stm32_clk_enable(STM32_RCC, STM32_CLK_TIM7);

	tim7->cnt = 0;
	tim7->psc = 2 - 1; /* 2 * APB1_CLK(30MHz) / 2 = 30MHz*/
	tim7->arr = 30000 - 1; /* 30MHz / 300000 = 1 KHz*/

	tim7->egr = 0; /* Update generation */
	tim7->dier = TIM_UIE; /* Update interrupt enable */
	tim7->cr2 = 0;
	tim7->cr1 = TIM_ARPE | TIM_URS | TIM_CEN;
}

void tim9_init(void)
{
	struct stm32f_tim * tim9 = STM32F_TIM9;

	/* clock enable */
	stm32_clk_enable(STM32_RCC, STM32_CLK_TIM9);

	tim9->cnt = 0;
	tim9->psc = 4 - 1; /* 2 * APB2_CLK(60MHz) / 4 = 30MHz*/
	tim9->arr = 30000 - 1; /* 30MHz / 3000 = 1KHz */

	tim9->egr = 0;
	tim9->dier = TIM_UIE; /* Update interrupt enable */
	tim9->cr2 = 0;
	tim9->cr1 = TIM_ARPE | TIM_URS | TIM_CEN;

}

void timer_init(struct stm32f_tim * tim)
{
	if (tim == STM32F_TIM9) {
		/* clock enable */
		stm32_clk_enable(STM32_RCC, STM32_CLK_TIM9);
		tim->psc = 4 - 1; /* 2 * APB2_CLK(60MHz) / 4 = 30MHz*/
	} else {
		if (tim == STM32F_TIM7) {
			stm32_clk_enable(STM32_RCC, STM32_CLK_TIM7);
		} else if (tim == STM32F_TIM6) {
			/* clock enable */
			stm32_clk_enable(STM32_RCC, STM32_CLK_TIM6);
		}
		tim->psc = 2 - 1; /* 2 * APB1_CLK(30MHz) / 2 = 30MHz*/
	}

	tim->arr = 30000 - 1; /* 30MHz / 3000 = 1KHz */
	tim->cnt = 0;

	tim->egr = 0;
	tim->dier = TIM_UIE; /* Update interrupt enable */
	tim->cr2 = 0;
	tim->cr1 = 0;
}

void timer_start(struct stm32f_tim * tim)
{
	tim->cr1 = TIM_ARPE | TIM_URS | TIM_CEN;
}

int timer_isr_task(void * arg)
{
	int self = thinkos_thread_self();
	struct stm32f_tim * tim6 = STM32F_TIM6;
	uint32_t latency;
	uint32_t ticks;
	uint32_t ev;

	printf(" [%d] started.\n", self);

	while (1) {
		thinkos_irq_wait(STM32F_IRQ_TIM6);
		latency = tim6->cnt;
		ev = tim6->sr;
		if (ev == 0)
			continue;

		/* Clear update interrupt flag */
		tim6->sr = 0;

		if (meter.max.tim6 < latency)
			meter.max.tim6 = latency;

		meter.avg.tim6 = (meter.avg.tim6 * 63) / 64 + latency;
		ticks = meter.ticks.tim6;
		meter.ticks.tim6 = ticks + 1;

#if 0
		if (ticks & 1)
			stm32f_gpio_set(STM32F_GPIOB, 7);
		else
			stm32f_gpio_clr(STM32F_GPIOB, 7);
#endif
	}

	return 0;
}

int event_wait_task(void * arg)
{
	struct stm32f_tim * tim7 = STM32F_TIM7;
	int self = thinkos_thread_self();
	uint32_t latency;
	uint32_t ticks;

	req = false;
	ack = false;

	printf(" [%d] started.\n", self);

	while (1) {
		ack = false;
		req = true;
		while (!ack) {
			thinkos_flag_take(flag_timer);
		}
		latency = tim7->cnt;

		if (meter.max.event < latency)
			meter.max.event = latency;

		meter.avg.event = (meter.avg.event * 63) / 64 + latency;
		ticks = meter.ticks.event;
		meter.ticks.event = ticks + 1;
	}

	return 0;
}


#define STACK_SIZE 512
uint32_t stack[4][STACK_SIZE / 4];

void irq_test(void)
{
	int timer_th;
	int event_th;
	struct set max;
	struct set avg;
	struct set ticks;
	struct set ticks0;
	struct set dt;
	int i;
	int ms;

	/* make sure IRQs are disabled */
	cm3_irq_disable(STM32F_IRQ_TIM6);
	cm3_irq_disable(STM32F_IRQ_TIM7);
	cm3_irq_disable(STM32F_IRQ_TIM9);

	/* allocate semaphore */
	printf("1.\n");
	sem_timer = thinkos_sem_alloc(0); 
	/* allocate flag */
	printf("2.\n");
	flag_timer = thinkos_flag_alloc(); 

	/* initialize timer 6 */
	timer_init(STM32F_TIM6);

	/* initialize timer 7 */
	timer_init(STM32F_TIM7);
	/* set timer 7 to very high priority */
	cm3_irq_pri_set(STM32F_IRQ_TIM7, 0x20);
	cm3_irq_enable(STM32F_IRQ_TIM7);

	/* initialize timer 9 */
	timer_init(STM32F_TIM9);
	/* set timer 9 to very low priority */
	cm3_irq_pri_set(STM32F_IRQ_TIM9, 0xff);
	cm3_irq_enable(STM32F_IRQ_TIM9);

	printf("4.\n");
	event_th = thinkos_thread_create(event_wait_task, NULL, 
									 stack[1], STACK_SIZE);

	printf("5.\n");
	timer_th = thinkos_thread_create(timer_isr_task, NULL, 
									 stack[2], STACK_SIZE);


	thinkos_sleep(100);

//	printf("- All times in microseconds\n");
	printf("| TIM6 IRQ Wait  | TIM7 High Pri  "
		   "| TIM9 Low Pri   | TIM7 > Ev Wait |\n"); 

	printf("|   dt  avg  max |   dt  avg  max "
		   "|   dt  avg  max |   dt  avg  max |\n"); 
		   
	memset(&meter, 0, sizeof(meter));

	timer_start(STM32F_TIM6);
	timer_start(STM32F_TIM7);
	timer_start(STM32F_TIM9);

	ticks0.tim6 = 0;
	ticks0.tim7 = 0;
	ticks0.tim9 = 0;
	ticks0.event = 0;

//	for (i = 0; i < 10; i++) {
	for (i = 0; i < 5; i++) {
		for (ms = 0; ms < 1000; ms++) 
			thinkos_sem_wait(sem_timer);

		/* get data */
		max = meter.max;
		avg = meter.avg;
		ticks = meter.ticks;

		avg.tim6 = (avg.tim6 * 33) / 64;
		max.tim6 *= 33;

		avg.tim7 = (avg.tim7 * 33) / 64;
		max.tim7 *= 33;

		avg.tim9 = (avg.tim9 * 33) / 64;
		max.tim9 *= 33;

		avg.event = (avg.event * 33) / 64;
		max.event *= 33;

		dt.tim6 = ticks.tim6 - ticks0.tim6;
		ticks0.tim6 = ticks.tim6;

		dt.tim7 = ticks.tim7 - ticks0.tim7;
		ticks0.tim7 = ticks.tim7;

		dt.tim9 = ticks.tim9 - ticks0.tim9;
		ticks0.tim9 = ticks.tim9;

		dt.event = ticks.event - ticks0.event;
		ticks0.event = ticks.event;

		printf("| %4d %4d %4d | %4d %4d %4d | %4d %4d %4d | %4d %4d %4d |\n", 
			   dt.tim6, avg.tim6, max.tim6, 
			   dt.tim7, avg.tim7, max.tim7, 
			   dt.tim9, avg.tim9, max.tim9,
			   dt.event, avg.event, max.event);
	}
	printf("\n");

	cm3_irq_disable(STM32F_IRQ_TIM7);
	cm3_irq_disable(STM32F_IRQ_TIM9);

	thinkos_cancel(event_th, 0);
	thinkos_cancel(timer_th, 0);

	thinkos_flag_free(flag_timer);
	thinkos_sem_free(sem_timer);
}

void io_init(void)
{
	stm32_gpio_clock_en(STM32_GPIOB);
	stm32_gpio_mode(STM32_GPIOB, 6, OUTPUT, PUSH_PULL | SPEED_MED);
	stm32_gpio_mode(STM32_GPIOB, 7, OUTPUT, PUSH_PULL | SPEED_MED);
}

void stdio_init(void);

int main(int argc, char ** argv)
{
	cm3_udelay_calibrate();

	cm3_udelay_calibrate();

	/* Initialize the stdin, stdout and stderr */
	stdio_init();

	printf("\n");
	printf("---------------------------------------------------------\n");
	printf(" ThinkOS IRQ Test\n");
	printf("---------------------------------------------------------\n");
	printf("\n");

	thinkos_init(THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(7));

	io_init();

	irq_test();

	printf("---------------------------------------------------------\n");

	thinkos_sleep(5000);

	return 0;
}

