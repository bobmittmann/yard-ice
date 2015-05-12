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
#include <sys/delay.h>
#include <sys/param.h>

#include <thinkos.h>
#define __THINKOS_IRQ__
#include <thinkos_irq.h>

#include <sys/dcclog.h>


/* ----------------------------------------------------------------------
 * I/O 
 * ----------------------------------------------------------------------
 */
#define QUEUE_LEN 512
#define Q_IDX(X) ((X) % (QUEUE_LEN  - 1))

struct queue {
	int rd_flag;
	volatile uint32_t tail;
	volatile uint32_t head;
	uint8_t buf[QUEUE_LEN];
};

int q_write(struct queue * q, const void * buf, int len)
{
	uint8_t * src = (uint8_t *)buf;
	uint32_t head;
	int rem;
	int n;
	int i;

	head = q->head;
	rem = QUEUE_LEN - (head - q->tail);
	n = MIN(len, rem);

	for (i = 0; i < n; ++i)
		q->buf[Q_IDX(head++)] = src[i];

	q->head = head;

	return n;
}

int q_read(struct queue * q, void * buf, int len)
{
	uint8_t * dst = (uint8_t *)buf;
	uint32_t tail;
	int cnt;
	int n;
	int i;

	tail = q->tail;
	cnt = q->head - tail;
	n = MIN(len, cnt);

	for (i = 0; i < n; ++i)
		dst[i] = q->buf[Q_IDX(tail++)];

	q->tail = tail;

	return n;
}

int q_len(struct queue * q)
{
	return q->head - q->tail;
}

struct queue my_queue;

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

void stm32f_tim2_isr(void)
{
	struct stm32f_tim * tim = STM32F_TIM2;

	/* Clear interrupt flags */
	tim->sr = 0;

	q_write(&my_queue, "IIiiJJjj", 8);
	thinkos_flag_signal_i(my_queue.rd_flag);
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
		q_write(&my_queue, "PPppQQqqRRrr", 12);
		thinkos_flag_signal(my_queue.rd_flag);
	}

	return i;
}

int consumer_task(void * arg)
{
	char buf[16];
	uint32_t clk;
	int n;
	int i = 0;
	int id;

	printf(" %s(): [%d] started...\n", __func__, id = thinkos_thread_self());
	thinkos_sleep(100);

	for (;;) {
		if (thinkos_flag_wait(my_queue.rd_flag) < 0)
			abort();
		n = q_read(&my_queue, buf, 3);
		thinkos_sleep(10);
		clk = thinkos_clock();

		buf[n] = '\0';
		printf("<%d> %5d.%03d - %4d - \"%s\"\n", 
			   id, clk / 1000, clk % 1000, i, buf);
		thinkos_flag_release(my_queue.rd_flag, q_len(&my_queue));
		i++;
	} 

	return i;
};

uint32_t producer1_stack[128];
uint32_t producer2_stack[128];
uint32_t producer3_stack[128];
uint32_t consumer1_stack[128];
uint32_t consumer2_stack[128];

void event_test(void)
{
};

void stdio_init(void);

int main(int argc, char ** argv)
{
	/* Run the test */
	int producer_th;
	int consumer1_th;
	int consumer2_th;

	DCC_LOG_INIT();
	DCC_LOG_CONNECT();

	/* Calibrate the the uDelay loop */
	cm3_udelay_calibrate();

	/* Initialize the stdin, stdout and stderr */
	stdio_init();

	/* Print a useful information message */
	printf("\n");
	printf("---------------------------------------------------------\n");
	printf(" ThinkOS - Flags example\n");
	printf("---------------------------------------------------------\n");
	printf("\n");

	/* Initialize the ThinkOS kernel */
	thinkos_init(THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(0));


	timer_init(2);

	/* allocate the empty signal semaphores */
	/* initialize the empty as 1 so we can insert an item immediately. */
	my_queue.rd_flag = thinkos_flag_alloc(); 

	/* create the producer thread */
	producer_th = thinkos_thread_create(producer_task, NULL, 
			producer1_stack, sizeof(producer1_stack));

	/* create the consuer thread */
	consumer1_th = thinkos_thread_create(consumer_task, NULL, 
			consumer1_stack, sizeof(consumer1_stack));
	consumer2_th = thinkos_thread_create(consumer_task, NULL, 
			consumer2_stack, sizeof(consumer2_stack));

	printf(" * My flag: %d\n", my_queue.rd_flag);
	printf(" * Producer thread: %d\n", producer_th);
	printf(" * Consumer thread 1: %d\n", consumer1_th);
	printf(" * Consumer thread 2: %d\n", consumer2_th);
	printf("\n");

	/* number of items to be produced */
	prod_count = 100;

	/* wait for the first second clock time */
	thinkos_alarm(1000);
	timer_enable();

	/* wait for the production thread to finish */
	thinkos_join(producer_th);
	/* wait for the consumer thread to finish */
	thinkos_join(consumer1_th);
	thinkos_join(consumer2_th);

	printf("\n");

	thinkos_sleep(10000);

	return 0;
}

