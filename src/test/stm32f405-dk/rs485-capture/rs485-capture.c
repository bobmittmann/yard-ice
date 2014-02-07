/* 
 * File:	 usb-test.c
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
#include <arch/cortex-m3.h>
#include <sys/delay.h>
#include <sys/serial.h>
#include <sys/param.h>
#include <sys/file.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <sys/dcclog.h>
#include <sys/usb-cdc.h>
#include <sys/tty.h>

#include <thinkos.h>
#define __THINKOS_IRQ__
#include <thinkos_irq.h>

/****************************************************************************************************
  Timer
 ***************************************************************************************************/

static inline uint32_t timer_ts(void)
{
	struct stm32f_tim * tim = STM32F_TIM5;

	return tim->cnt;
}

#define TIMESTAMP_FREQ 1000000

static void timer_init(void)
{
	struct stm32f_rcc * rcc = STM32F_RCC;
	struct stm32f_tim * tim = STM32F_TIM5;
	uint32_t div;

	/* get the total divisior */
	div = (stm32f_tim1_hz + (TIMESTAMP_FREQ / 2)) / TIMESTAMP_FREQ;

	/* Timer clock enable */
	rcc->apb1enr |= RCC_TIM5EN;
	
	/* Timer configuration */
	tim->psc = div - 1;
	tim->arr = 0xffffffff;
	tim->cnt = 0;
	tim->egr = 0;
	tim->dier = 0;
	tim->cr2 = 0;
	tim->cr1 = TIM_CEN; /* Enable counter */

	tim->egr = TIM_UG; /* Update registers */
}

/****************************************************************************************************
  Fifo
 ***************************************************************************************************/

#define FIFO_LEN 256

struct fifo {
	volatile uint32_t head;
	volatile uint32_t tail;
	uint8_t data[FIFO_LEN];
	uint32_t time[FIFO_LEN];
};

static inline void fifo_init(struct fifo * fifo)
{
	fifo->head = 0;
	fifo->tail = 0;
}

static inline int fifo_get(struct fifo * fifo, uint32_t * ts)
{
	uint32_t tail = fifo->tail;
	int c;

	c = fifo->data[tail & (FIFO_LEN - 1)];
	*ts = fifo->data[tail & (FIFO_LEN - 1)];

	fifo->tail = tail + 1;
	return c;
}

static inline void fifo_put(struct fifo * fifo, int c, uint32_t ts)
{
	uint32_t head = fifo->head;

	fifo->data[head & (FIFO_LEN - 1)] = c;
	fifo->time[head & (FIFO_LEN - 1)] = ts;

	fifo->head = head + 1;
}

static inline bool fifo_is_empty(struct fifo * fifo)
{
	return (fifo->tail == fifo->head) ? true : false;
}

static inline bool fifo_is_full(struct fifo * fifo)
{
	return ((fifo->head - fifo->tail) == FIFO_LEN) ? true : false;
}

/****************************************************************************************************
  RS485
 ***************************************************************************************************/

#define USART2_TX STM32F_GPIOA, 2
#define USART2_RX STM32F_GPIOA, 3

#define LINK_TXEN STM32F_GPIOA, 1
#define LINK_LOOP STM32F_GPIOA, 0 

struct {
	struct fifo rx_fifo;
	int rx_flag;
	int rx_sem;
} rs485;

void stm32f_usart2_isr(void)
{
	struct stm32f_usart * uart = STM32F_USART2;
	uint32_t sr;
	uint32_t ts = 0;
	int c;
	
	sr = uart->sr & uart->cr1;

	if (sr & USART_RXNE) {
		DCC_LOG(LOG_INFO, "RXNE");
		ts = timer_ts();
		c = uart->dr;
		(void)c;
		if (!fifo_is_full(&rs485.rx_fifo)) { 
			fifo_put(&rs485.rx_fifo, c, ts);
		} else {
			DCC_LOG(LOG_WARNING, "RX fifo full!");
		}
		__thinkos_flag_signal(rs485.rx_flag);
//		__thinkos_sem_signal(rs485.rx_sem);
	}	
}

void rs485_init(unsigned int speed)
{
	struct stm32f_usart * uart = STM32F_USART2;
	int c;

	DCC_LOG(LOG_INFO, "...");
	rs485.rx_flag = thinkos_flag_alloc(); 
	rs485.rx_sem = thinkos_sem_alloc(0); 

	/* IO init */
	stm32f_gpio_mode(USART2_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	stm32f_gpio_mode(USART2_RX, ALT_FUNC, PULL_UP);
	stm32f_gpio_af(USART2_RX, GPIO_AF7);
	stm32f_gpio_af(USART2_TX, GPIO_AF7);

	stm32f_gpio_mode(LINK_TXEN, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32f_gpio_mode(LINK_LOOP, OUTPUT, PUSH_PULL | SPEED_LOW);

	stm32f_gpio_set(LINK_LOOP);
	stm32f_gpio_set(LINK_TXEN);

	cm3_irq_pri_set(STM32F_IRQ_USART2, IRQ_PRIORITY_HIGH);
	cm3_irq_enable(STM32F_IRQ_USART2);

	stm32f_usart_init(uart);
	stm32f_usart_baudrate_set(uart, speed);
	stm32f_usart_mode_set(uart, SERIAL_8N1);

	/* enable RX interrupt */
	uart->cr1 |= USART_RXNEIE;

	/* clear pending data */
	c = uart->dr;
	(void)c;
	c = uart->dr;
	(void)c;

	fifo_init(&rs485.rx_fifo);

	stm32f_usart_enable(uart);
	cm3_irq_enable(STM32F_IRQ_USART2);
}

/****************************************************************************************************
  USB
 ***************************************************************************************************/

#define BUF_SIZE 256
usb_cdc_class_t * cdc;

int input_task(FILE * f)
{
	char buf[BUF_SIZE];
	unsigned int n;

	while (1) {
		DCC_LOG(LOG_TRACE, "usb_cdc_read()");
		n = usb_cdc_read(cdc, buf, 128, 0);
		(void)n;
		DCC_LOG1(LOG_TRACE, "n=%d", n);
	};

	return 0;
}

#define STACK_SIZE 512
uint32_t input_stack[STACK_SIZE / 4];

int main(int argc, char ** argv)
{
	struct tty_dev * tty;
	FILE * f_raw;
	FILE * f;
	int i;
	int c;
	uint32_t ts[2];
	int32_t dt;

	DCC_LOG_INIT();
	DCC_LOG_CONNECT();

	DCC_LOG(LOG_TRACE, "1. cm3_udelay_calibrate()");
	cm3_udelay_calibrate();

	timer_init();

	DCC_LOG(LOG_TRACE, "2. thinkos_init()");
	thinkos_init(THINKOS_OPT_PRIORITY(8) | THINKOS_OPT_ID(7));

	DCC_LOG(LOG_TRACE, "usb_cdc_init()");
	cdc = usb_cdc_init(&stm32f_otg_fs_dev, *((uint64_t *)STM32F_UID));

	DCC_LOG(LOG_TRACE, "usb_cdc_fopen()");
	f_raw = usb_cdc_fopen(cdc);

	tty = tty_attach(f_raw);
	f = tty_fopen(tty);

	/* create some printer threads */
	thinkos_thread_create((void *)input_task, (void *)f, 
						  input_stack, STACK_SIZE, 0);

	rs485_init(110000000 / (16 * 15));


	ts[0] = timer_ts();
	for (i = 0; ;i++) {
		while (fifo_is_empty(&rs485.rx_fifo)) {
			DCC_LOG(LOG_INFO, "wait...");
			thinkos_flag_wait(rs485.rx_flag);
			__thinkos_flag_clr(rs485.rx_flag);
			DCC_LOG(LOG_INFO, "wakeup.");
		}

		ts[1] = ts[0];
		c = fifo_get(&rs485.rx_fifo, ts);
		dt = ts[0] - ts[1];

		fprintf(f, "%4d %8d %02x\n", i, dt, c);
	}

	return 0;
}

