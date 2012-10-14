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
int wq_timer; /* wait queue */

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
	tim7->sr = 0;

	if (meter.max.tim7 < latency)
		meter.max.tim7 = latency;

	meter.avg.tim7 = (meter.avg.tim7 * 63) / 64 + latency;
	ticks = meter.ticks.tim7;
	meter.ticks.tim7 = ticks + 1;

	if (req & !ack) {
		thinkos_event_notify(wq_timer);
		ack = true;
		req = false;
	}

	if (ticks & 1)
		stm32f_gpio_set(STM32F_GPIOB, 6);
	else
		stm32f_gpio_clr(STM32F_GPIOB, 6);

	/* Clear update interrupt flag */
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

	ticks = ticks;

	/* This is a low priority interrupt handler, we can 
	   invoque service calls from it. */
	thinkos_sem_post(sem_timer);
}

void tim7_init(void)
{
	struct stm32f_tim * tim7 = STM32F_TIM7;
	struct stm32f_rcc * rcc = STM32F_RCC;

	/* clock enable */
	rcc->apb1enr |= RCC_TIM7EN;

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
	struct stm32f_rcc * rcc = STM32F_RCC;

	/* clock enable */
	rcc->apb2enr |= RCC_TIM9EN;

	tim9->cnt = 0;
	tim9->psc = 4 - 1; /* 2 * APB2_CLK(60MHz) / 4 = 30MHz*/
	tim9->arr = 30000 - 1; /* 30MHz / 3000 = 1KHz */

	tim9->egr = 0;
	tim9->dier = TIM_UIE; /* Update interrupt enable */
	tim9->cr2 = 0;
	tim9->cr1 = TIM_ARPE | TIM_URS | TIM_CEN;

}

int timer_isr_task(void * arg)
{
	int self = thinkos_thread_self();
	struct stm32f_tim * tim6 = STM32F_TIM6;
	struct stm32f_rcc * rcc = STM32F_RCC;
	uint32_t latency;
	uint32_t ticks;
	uint32_t ev;

	printf(" [%d] started.\n", self);
	thinkos_sleep(100);

	/* clock enable */
	rcc->apb1enr |= RCC_TIM6EN;

	tim6->cnt = 0;
	tim6->psc = 2 - 1; /* 2 * APB1_CLK(30MHz) / 2 = 30MHz*/
	tim6->arr = 30000 - 1; /* 30MHz / 30000 = 1K Hz*/

	tim6->egr = 0;
	tim6->dier = TIM_UIE; /* Update interrupt enable */
	tim6->cr2 = 0;
	tim6->cr1 = TIM_URS | TIM_CEN;

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

		if (ticks & 1)
			stm32f_gpio_set(STM32F_GPIOB, 7);
		else
			stm32f_gpio_clr(STM32F_GPIOB, 7);

	}

	return 0;
}

int high_priority_task(void * arg)
{
	struct stm32f_tim * tim7 = STM32F_TIM7;
	int self = thinkos_thread_self();
	uint32_t latency;
	uint32_t ticks;

	req = false;
	ack = false;

	printf(" [%d] started.\n", self);
	thinkos_sleep(100);

	while (1) {
		ack = false;
		req = true;
		thinkos_critical_enter();
		while (!ack) {
			thinkos_event_wait(wq_timer);
		}
		thinkos_critical_exit();

		latency = tim7->cnt;

		if (meter.max.event < latency)
			meter.max.event = latency;

		meter.avg.event = (meter.avg.event * 63) / 64 + latency;
		ticks = meter.ticks.event;
		meter.ticks.event = ticks + 1;
	}

	return 0;
}

int low_priority_task(void * arg)
{
	int self = thinkos_thread_self();
	struct set max;
	struct set avg;
	struct set ticks;
	struct set ticks0;
	struct set dt;
	int i;
	int ms;

	ticks0.tim6 = 0;
	ticks0.tim7 = 0;
	ticks0.tim9 = 0;
	ticks0.event = 0;

	printf(" [%d] started.\n", self);
	for (i = 0; i < 10; i++) {
		for (ms = 0; ms < 1000; ms++) 
			thinkos_sem_wait(sem_timer);

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

		printf("| %4d.%4d:%-4d | %4d.%4d:%-4d | %4d %4d:%-4d | %4d %4d:%-4d |\n", 
			   dt.tim6, avg.tim6, max.tim6, 
			   dt.tim7, avg.tim7, max.tim7, 
			   dt.tim9, avg.tim9, max.tim9,
			   dt.event, avg.event, max.event);
	}
	printf("\n");

	printf(" [%d] end.\n", self);

	return 0;
}


#define STACK_SIZE 512
uint32_t stack[4][STACK_SIZE / 4];

void irq_test(void)
{
	int display_th;
	int timer_th;
	int priority_th;

	/* allocate semaphore */
	printf("1.\n");
	sem_timer = thinkos_sem_alloc(0); 
	printf("2.\n");
	wq_timer = thinkos_wq_alloc(); 

	printf("4.\n");
	priority_th = thinkos_thread_create(high_priority_task, NULL, 
						  stack[1], STACK_SIZE, 
						  THINKOS_OPT_PRIORITY(0) |
						  THINKOS_OPT_ID(0));

	printf("5.\n");
	timer_th = thinkos_thread_create(timer_isr_task, NULL, 
						  stack[2], STACK_SIZE, 
						  THINKOS_OPT_PRIORITY(0) |
						  THINKOS_OPT_ID(0));
	printf("3.\n");
	display_th = thinkos_thread_create(low_priority_task, NULL, 
						  stack[0], STACK_SIZE, 
						  THINKOS_OPT_PRIORITY(8) |
						  THINKOS_OPT_ID(64));

	tim7_init();
	/* set timer 7 to very high priority */
	cm3_irq_pri_set(STM32F_IRQ_TIM7, 0x20);

	tim9_init();
	/* set timer 9 to very low priority */
	cm3_irq_pri_set(STM32F_IRQ_TIM9, 0xff);

	cm3_irq_enable(STM32F_IRQ_TIM7);

	cm3_irq_enable(STM32F_IRQ_TIM9);

	/* wait for the display thread to finish */
	thinkos_join(display_th);

	thinkos_cancel(priority_th, 0);

	thinkos_cancel(timer_th, 0);

	cm3_irq_disable(STM32F_IRQ_TIM7);

	cm3_irq_disable(STM32F_IRQ_TIM9);

	thinkos_wq_free(wq_timer);
	thinkos_sem_free(sem_timer);
}

void io_init(void)
{
	stm32f_gpio_clock_en(STM32F_GPIOB);
	stm32f_gpio_mode(STM32F_GPIOB, 6, OUTPUT, PUSH_PULL | SPEED_MED);
	stm32f_gpio_mode(STM32F_GPIOB, 7, OUTPUT, PUSH_PULL | SPEED_MED);
}

int main(int argc, char ** argv)
{
	cm3_udelay_calibrate();
	stdout = stm32f_usart_open(STM32F_UART5, 115200, SERIAL_8N1);

	printf("\n");
	printf("---------------------------------------------------------\n");
	printf(" ThinkOS IRQ Test\n");
	printf("---------------------------------------------------------\n");
	printf("\n");

	thinkos_init(THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(0));

	io_init();

	irq_test();

	printf("---------------------------------------------------------\n");

	thinkos_sleep(10000);

	return 0;
}

