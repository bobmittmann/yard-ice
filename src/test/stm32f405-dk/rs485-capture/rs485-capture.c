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

#include <sys/usb-cdc.h>
#include <sys/tty.h>

#include <thinkos.h>
#define __THINKOS_IRQ__
#include <thinkos_irq.h>

#include "io.h"

/*****************************************************************************
 * Console 
 * ----------------------------------------------------------------------
 *****************************************************************************/

#define USART1_TX STM32F_GPIOB, 6
#define USART1_RX STM32F_GPIOB, 7

struct file stm32f_uart1_file = {
	.data = STM32F_USART1, 
	.op = &stm32f_usart_fops 
};

void stdio_init(void)
{
	struct stm32f_usart * uart = STM32F_USART1;

	/* USART1_TX */
	stm32f_gpio_mode(USART1_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);

	stm32f_gpio_mode(USART1_RX, ALT_FUNC, PULL_UP);
	stm32f_gpio_af(USART1_RX, GPIO_AF7);
	stm32f_gpio_af(USART1_TX, GPIO_AF7);

	stm32f_usart_init(uart);
	stm32f_usart_baudrate_set(uart, 115200);
	stm32f_usart_mode_set(uart, SERIAL_8N1);
	stm32f_usart_enable(uart);

	stderr = &stm32f_uart1_file;
	stdin = stderr;
	stdout = stdin;
}

/*****************************************************************************
  Timer
 *****************************************************************************/

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

/*****************************************************************************
  Fifo
 *****************************************************************************/

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
	*ts = fifo->time[tail & (FIFO_LEN - 1)];

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

/*****************************************************************************
  RS485
 *****************************************************************************/

#define USART2_TX STM32F_GPIOA, 2
#define USART2_RX STM32F_GPIOA, 3

#define LINK_TXEN STM32F_GPIOA, 1
#define LINK_LOOP STM32F_GPIOA, 0 

struct {
	struct fifo rx_fifo;
	int rx_flag;
} rs485;

void stm32f_usart2_isr(void)
{
	struct stm32f_usart * uart = STM32F_USART2;
	uint32_t sr;
	uint32_t ts = 0;
	int c;
	
	sr = uart->sr & uart->cr1;

	if (sr & USART_RXNE) {
		ts = timer_ts();
		c = uart->dr;
		if (!fifo_is_full(&rs485.rx_fifo)) { 
			fifo_put(&rs485.rx_fifo, c, ts);
		}
		__thinkos_flag_signal(rs485.rx_flag);
	}	
}

void rs485_init(unsigned int speed)
{
	struct stm32f_usart * uart = STM32F_USART2;
	unsigned int real_speed;
	int err;
	int c;

	rs485.rx_flag = thinkos_flag_alloc(); 

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

	real_speed = stm32f_usart_baudrate_get(uart);
	err = 1000 - ((speed * 1000) / real_speed);

	printf("%s: speed: set=%d get=%d err=%d.%d%%.\n", 
		   __func__, speed, real_speed, err / 10, err % 10);

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

void rs485_putc(int c)
{
	struct stm32f_usart * uart = STM32F_USART2;
	
	uart->dr = c;
}

/*****************************************************************************
  USB
 *****************************************************************************/

int input_task(FILE * f)
{
	unsigned int n;

	for (n = 0; ; ++n) {
		thinkos_sleep(500);
	};

	return 0;
}

int ui_task(FILE * f)
{
	int event;
	int c = 0;

	while (1) {

		/* wait for a push buton event */
		event = btn_event_wait();

		switch (event) {
		case EVENT_CLICK:
			led_flash(LED_I2S, 100);
			rs485_putc((c & 0x7f) | 0x80);
			thinkos_sleep(10);
			rs485_putc(c & 0x7f);
			c++;
			break;

		case EVENT_DBL_CLICK:
			fprintf(f, "------------------------------------\n");
			break;

		case EVENT_HOLD1:
			break;

		case EVENT_CLICK_N_HOLD:
			break;

		case EVENT_HOLD2:
			break;
		}
	}
}

int monitor_task(void * arg)
{
	unsigned int sec = 0;
	unsigned int min = 0;
	unsigned int hour = 0;

	for (;;) {
		thinkos_sleep(1000);
		led_flash(LED_I2S, 100);
		if (++sec == 60) {
			sec = 0;
			if (++min == 60) {
				min = 0;
				hour++;
			}
			printf("-- %2d:%02d --------------\n", hour, min);
		}
	};

	return 0;
}

#define STACK_SIZE 512
uint32_t input_stack[STACK_SIZE / 4];
uint32_t monitor_stack[STACK_SIZE / 4];
uint32_t ui_stack[STACK_SIZE / 4];

int main(int argc, char ** argv)
{
	usb_cdc_class_t * cdc;
	struct tty_dev * tty;
	FILE * f_raw;
	FILE * f;
	int i;
	int c[2];
	uint32_t ts[2];
	int32_t dt;

	cm3_udelay_calibrate();

	thinkos_init(THINKOS_OPT_PRIORITY(8) | THINKOS_OPT_ID(7));

	io_init();
	led_on(LED_NET);

	stdio_init();

	led_on(LED_I2S);

	timer_init();

	printf("1. usb_cdc_init()...\n");
	cdc = usb_cdc_init(&stm32f_otg_fs_dev, *((uint64_t *)STM32F_UID));

	led_on(LED_S2);

	printf("2. usb_cdc_fopen()...\n");
	f_raw = usb_cdc_fopen(cdc);

	led_on(LED_S1);

	tty = tty_attach(f_raw);

	printf("3. tty_fopen()...\n");
	f = tty_fopen(tty);

	thinkos_thread_create((void *)input_task, (void *)f, 
						  input_stack, STACK_SIZE, 1);

	thinkos_thread_create((void *)monitor_task, (void *)NULL, 
						  monitor_stack, STACK_SIZE, 2);

	printf("4. rs485_init()...\n");
	rs485_init(110000000 / (13 * 16));

	thinkos_thread_create((void *)ui_task, (void *)f, 
						  ui_stack, STACK_SIZE, 3);

	thinkos_sleep(100);
	led_off(LED_NET);
	led_off(LED_I2S);
	led_off(LED_S1);
	led_off(LED_S2);

	ts[0] = timer_ts();
	c[0] = 0;
	fprintf(f, "------------------------------------\n");
	fprintf(f, "RS485 capture");
	fprintf(f, "------------------------------------\n");
	for (i = 0; ;i++) {
		while (fifo_is_empty(&rs485.rx_fifo)) {
			thinkos_flag_wait(rs485.rx_flag);
			__thinkos_flag_clr(rs485.rx_flag);
		}
		led_flash(LED_NET, 100);

		ts[1] = ts[0];
		c[1] = c[0];
		c[0] = fifo_get(&rs485.rx_fifo, ts);
		dt = (int32_t)ts[0] - ts[1];
		if (!(c[0] & 0x80)) {
			if (c[1] & 0x80) {
				if ((c[0] & 0x7f) == (c[1] & 0x7f)) {
//					printf("%4d %8d %02x\n", i, dt, c[0]);
					fprintf(f, "%4d %8d %02x\n", i, dt, c[0]);
					led_flash(LED_S2, 100);
				} else {
					/* Numbers do not match!!! */
					led_flash(LED_S1, 100);
				}
			} else {
				/* previous was not master ??? */
				led_flash(LED_S1, 100);
			}
		} else {
			if (c[1] & 0x80) {
				/* master repeated */
				led_flash(LED_S1, 50);
			}
		}
	}

	return 0;
}

