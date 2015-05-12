/* 
 * File:	 sem_test.c
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


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include <sys/stm32f.h>
#include <sys/serial.h>
#include <sys/delay.h>

#include <thinkos.h>
#define __THINKOS_IRQ__
#include <thinkos_irq.h>

#include <sys/dcclog.h>


/* ----------------------------------------------------------------------
 * I/O 
 * ----------------------------------------------------------------------
 */
int my_ev_set;

void stm32f_tim2_isr(void)
{
	struct stm32f_tim * tim = STM32F_TIM2;

	/* Clear interrupt flags */
	tim->sr = 0;
	thinkos_ev_raise_i(my_ev_set, 16);

	DCC_LOG(LOG_TRACE, "...");
}

void timer_init(uint32_t freq)
{
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

	DCC_LOG3(LOG_TRACE, "freq=%dHz pre=%d n=%d", freq, pre, n);
	DCC_LOG1(LOG_TRACE, "real freq=%dHz\n", (2 * stm32f_apb1_hz) / pre / n);

	/* Timer clock enable */
	stm32_clk_enable(STM32_RCC, STM32_CLK_TIM2);
	
	/* Timer configuration */
	tim->psc = pre - 1;
	tim->arr = n - 1;
	tim->cnt = 0;
	tim->egr = 0;
	tim->dier = TIM_UIE; /* Update interrupt enable */
	tim->ccmr1 = TIM_OC1M_PWM_MODE1;
	tim->ccr1 = tim->arr / 2;
	tim->cr1 = 0;

	cm3_irq_pri_set(STM32F_IRQ_TIM2, IRQ_PRIORITY_REGULAR);
	/* Enable interrupt */
	cm3_irq_enable(STM32F_IRQ_TIM2);

}

void timer_enable(void)
{
	struct stm32f_tim * tim = STM32F_TIM2;
	tim->cr1 = TIM_URS | TIM_CEN; /* Enable counter */
}

int prod_count;

int producer_task(void * arg)
{
	unsigned int i = 0;

	printf(" %s(): [%d] started...\n", __func__, thinkos_thread_self());
	thinkos_sleep(100);

	for (i = 0; i < prod_count; i++) {
		/* let's spend some time thinking */
		thinkos_sleep(500);

		/* signal a full buffer */
		thinkos_ev_raise(my_ev_set, i % 16);
	}

	return i;
}

int periodic1_task(void * arg)
{
	printf(" %s(): [%d] started...\n", __func__, thinkos_thread_self());
	thinkos_sleep(100);

	for (;;) {
		thinkos_sleep(500);
		thinkos_ev_raise(my_ev_set, 1);
	}

	return 0;
}

int periodic2_task(void * arg)
{
	uint32_t clk = 0;

	printf(" %s(): [%d] started...\n", __func__, thinkos_thread_self());

	for (;;) {
		clk += 1000;
		thinkos_alarm(clk);
		thinkos_ev_raise(my_ev_set, 17);
		thinkos_ev_raise(my_ev_set, 18);
		thinkos_ev_raise(my_ev_set, 19);
	}

	return 0;
}


int consumer_task(void * arg)
{
	int i = 0;
	int ev;
	uint32_t clk;

	printf(" %s(): [%d] started...\n", __func__, thinkos_thread_self());
	thinkos_sleep(100);

	/* set the production enable flag to start production */
	for (;;) {
		printf(" %3d ", i);
		ev = thinkos_ev_wait(my_ev_set);
		clk = thinkos_clock();
		printf(" %5d.%03d - %4d - event=%d\n", clk / 1000, clk % 1000, i, ev);
		i++;
	} 

	return i;
};

uint32_t prod1_stack[128];
uint32_t prod2_stack[128];
uint32_t prod3_stack[128];
uint32_t consumer_stack[128];

void event_test(void)
{
	int producer_th;
	int consumer_th;
	int producer_ret;
	int consumer_ret;

	timer_init(2);

	/* allocate the empty signal semaphores */
	/* initialize the empty as 1 so we can insert an item immediately. */
	my_ev_set = thinkos_ev_alloc(); 

	/* create the producer thread */
	producer_th = thinkos_thread_create(producer_task, NULL, 
			prod1_stack, sizeof(prod1_stack));

	thinkos_thread_create(periodic1_task, NULL, 
			prod2_stack, sizeof(prod2_stack));

	thinkos_thread_create(periodic2_task, NULL, 
			prod3_stack, sizeof(prod3_stack));

	/* create the consuer thread */
	consumer_th = thinkos_thread_create(consumer_task, NULL, 
			consumer_stack, sizeof(consumer_stack));

	printf(" * My event set: %d\n", my_ev_set);
	printf(" * Producer thread: %d\n", producer_th);
	printf(" * Consumer thread: %d\n", consumer_th);
	printf("\n");

	/* number of items to be produced */
	prod_count = 100;

	/* wait for the first second clock time */
	thinkos_alarm(1000);
	timer_enable();


	thinkos_alarm(4000);
	printf(" * masking event %d\n", 16);
	thinkos_ev_mask(my_ev_set, (1 << 16));

	thinkos_alarm(6000);
	printf(" * masking event %d\n", 17);
	thinkos_ev_mask(my_ev_set, (1 << 17));

	thinkos_alarm(8000);
	printf(" * masking event %d\n", 18);
	thinkos_ev_mask(my_ev_set, (1 << 18));

	thinkos_alarm(10000);
	printf(" * masking event %d\n", 19);
	thinkos_ev_mask(my_ev_set, (1 << 19));

	thinkos_alarm(12000);
	printf(" * unmasking all events.\n");
	thinkos_ev_unmask(my_ev_set, 0xffffffff);

	/* wait for the production thread to finish */
	producer_ret = thinkos_join(producer_th);

	/* wait for the consumer thread to finish */
	consumer_ret = thinkos_join(consumer_th);

	printf(" * Production return = %d\n", producer_ret);
	printf(" * Consumer return = %d\n", consumer_ret);

	printf("\n");
};

void stdio_init(void);

int main(int argc, char ** argv)
{
	DCC_LOG_INIT();
	DCC_LOG_CONNECT();

	/* Calibrate the the uDelay loop */
	cm3_udelay_calibrate();

	/* Initialize the stdin, stdout and stderr */
	stdio_init();

	/* Print a useful information message */
	printf("\n");
	printf("---------------------------------------------------------\n");
	printf(" ThinkOS - Event sets example\n");
	printf("---------------------------------------------------------\n");
	printf("\n");

	/* Initialize the ThinkOS kernel */
	thinkos_init(THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(0));

	/* Run the test */
	event_test();

	thinkos_sleep(10000);

	return 0;
}

