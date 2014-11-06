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

int console_mutex;

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

//#define CLT_RAND_MAX 0xfffffffffffffLL
#define CLT_RAND_MAX 0x7fffffffLL

/* Random number generator
   Aproximation of Normal Distribution using Center Limit Theorem. */
int32_t clt_rand(uint64_t * seedp, int32_t max) 
{
	uint64_t randseed  = *seedp;
	uint64_t val = 0;
	int i;

	for (i = 0; i < 16; ++i) {
		randseed = (randseed * 6364136223846793005LL) + 1LL;
		val += (randseed >> 16) & CLT_RAND_MAX;
	}

	*seedp = randseed;
	val = (val * (int64_t)max) / (16LL * (CLT_RAND_MAX + 1));

	return val;
}

static struct {
	int flag;
	volatile bool en;
	uint64_t seed;
} loopback;

int loopback_task(FILE * f)
{
	unsigned int seq = 0;
	unsigned int delay = 0;

	for (;;) {
		do {
			thinkos_flag_wait(loopback.flag);
			__thinkos_flag_clr(loopback.flag);
		} while (!loopback.en);

		thinkos_mutex_lock(console_mutex);
		fprintf(f, "Loopback test start.\n");
		thinkos_mutex_unlock(console_mutex);

		while (loopback.en) {
//			delay = (rand() & 0x3f) + 1;
			delay = clt_rand(&loopback.seed, 100) + 1;
			led_flash(LED_I2S, 50);
			rs485_putc((seq & 0x7f) | 0x80);
			thinkos_sleep(delay);
			rs485_putc(seq & 0x7f);
			seq++;
	//		thinkos_sleep(100 - delay);
			thinkos_sleep(1);
		}

		__thinkos_flag_clr(loopback.flag);

		thinkos_mutex_lock(console_mutex);
		fprintf(f, "Loopback test stop.\n");
		thinkos_mutex_unlock(console_mutex);
	};

	return 0;
}


void loopback_start(void)
{
	loopback.en = true;
	thinkos_flag_set(loopback.flag);
}

void loopback_stop(void)
{
	if (loopback.en)  {
	loopback.en = false;
	thinkos_flag_set(loopback.flag);
	}
}

#define BIN_RES 50
#define BIN_CNT 10000

struct {
	int mutex;
	uint32_t error;
	uint32_t match;
	int32_t dt_max;
	int32_t dt_min;
	int32_t dt_avg;
	int64_t dt_sum;
	uint32_t bin[BIN_CNT];
} stat;

void stat_clear(void)
{
	int i;

	thinkos_mutex_lock(stat.mutex);

	stat.error = 0;
	stat.match = 0;
	stat.dt_min = 9000000;
	stat.dt_max = 0;
	stat.dt_avg = 0;
	stat.dt_sum = 0;

	for (i = 0; i < BIN_CNT; ++i) {
		stat.bin[i] = 0;
	}

	thinkos_mutex_unlock(stat.mutex);
}

void stat_init(void)
{
	stat.mutex = thinkos_mutex_alloc();
	stat_clear();
}

void stat_show(FILE * f)
{
	uint32_t peak;
	int32_t dt;
	int i;
	div_t x;
	

	thinkos_mutex_lock(console_mutex);
	fprintf(f, "---- Statistics ----------\n");

	if (stat.match > 0) {
//		stat.dt_avg = (int64_t)(stat.dt_sum / (int64_t)stat.match);
		x = div(stat.dt_sum, stat.match);
		stat.dt_avg = x.quot;
	}

	thinkos_mutex_lock(stat.mutex);
	fprintf(f, "  Error: %6d\n", stat.error);
	fprintf(f, "  Match: %6d\n", stat.match);
	fprintf(f, "    Min: %4d.%03d\n", stat.dt_min / 1000, stat.dt_min % 1000);
	fprintf(f, "    Max: %4d.%03d\n", stat.dt_max / 1000, stat.dt_max % 1000);
	fprintf(f, "    Avg: %4d.%03d\n", stat.dt_avg / 1000, stat.dt_avg % 1000);
	peak = 0;
	dt = 0;
	for (i = 0; i < BIN_CNT; ++i) {
		if (stat.bin[i] > peak) {
			peak = stat.bin[i];
			dt = i * BIN_RES;
		}
	}
	fprintf(f, "   Peak: %4d.%03d (%d)\n", dt / 1000, dt % 1000, peak);
	thinkos_mutex_unlock(stat.mutex);

	thinkos_mutex_unlock(console_mutex);
}

void stat_hist_show(FILE * f)
{
	int l;
	int h;
	int i;
	int j;
	int range;
	int step;
	int n;
	uint32_t sum;
	uint32_t max;
	uint32_t bin[80];
	char s[128];

	thinkos_mutex_lock(console_mutex);
	fprintf(f, "---- Histogram ----------\n");
	thinkos_mutex_unlock(console_mutex);

	thinkos_mutex_lock(stat.mutex);
	for (l = 0; l < BIN_CNT; ++l) {
		if (stat.bin[l] > 0)
			break;
	}

	for (h = BIN_CNT - 2; h >= 0; --h) {
		if (stat.bin[h] > 0)
			break;
	}

	range = h - l + 1;

	if (range < 1) {
		thinkos_mutex_unlock(stat.mutex);
		return;
	}

	n = 50;
	step = (range + n - 1) / n;
	range = n * step;
	/* adjust high limit */
	h = l + range;
//	fprintf(f, "Lo=%d Hi=%d Step=%d Range=%d\n", l, h, step, range);
	if (h >= BIN_CNT) {
		fprintf(f, "FIXME: adjust range!!!!\n");
		thinkos_mutex_unlock(stat.mutex);
		return;
	}

	max = 0;
	for (i = 0; i < n; ++i) {
		int k;
		sum = 0;
		k = l + i * step;
		for (j = 0; j < step; ++j) {
			sum += stat.bin[k + j];
		}
		bin[i] = sum;
		if (sum > max)
			max = sum;
	}
	thinkos_mutex_unlock(stat.mutex);

	thinkos_mutex_lock(console_mutex);
	for (i = 0; i < n; ++i) {
		int32_t dt;
		dt = (l + i * step) * BIN_RES;
		for (j = 0; j < (bin[i] * 64) / max; ++j)
			s[j] = '#';
		s[j] = '\0';

		fprintf(f, "%3d.%03d %s\n", dt / 1000, dt % 1000, s);
	}
	thinkos_mutex_unlock(console_mutex);
}

void stat_dist_show(FILE * f)
{
	int l;
	int h;
	int i;
	int32_t dt;
	uint32_t sum;

	thinkos_mutex_lock(stat.mutex);
	for (l = 0; l < BIN_CNT; ++l) {
		if (stat.bin[l] > 0)
			break;
	}

	for (h = BIN_CNT - 1; h >= 0; --h) {
		if (stat.bin[h] > 0)
			break;
	}

	thinkos_mutex_lock(console_mutex);
	fprintf(f, "---- Distribution ----------\n");
	
	for (i = l; i <= h; ++i) {
		dt = i * BIN_RES;
		sum = stat.bin[i];
		if (sum > 0) {
			fprintf(f, "%4d.%03d, %5d\n", dt / 1000, dt % 1000, sum);
		}
	}
	thinkos_mutex_unlock(console_mutex);
	thinkos_mutex_unlock(stat.mutex);
}

void show_menu(FILE * f)
{
	thinkos_mutex_lock(console_mutex);
	fprintf(f, "\n");
	fprintf(f, " Options:\n");
	fprintf(f, " --------\n");
	fprintf(f, "   c    - Clear statistics\n");
	fprintf(f, "   d    - Show distribution\n");
	fprintf(f, "   h    - Show histogram\n");
	fprintf(f, "   l    - Run loopback test\n");
	fprintf(f, "   s    - Show statistics\n");
	fprintf(f, "\n");
	thinkos_mutex_unlock(console_mutex);
}

int input_task(FILE * f)
{
	FILE * f_raw;
	unsigned int n;
	int c;

	if (isfatty(f)) {
		f_raw = ftty_lowlevel(f);
	} else {
		f_raw = f;
	}

	for (n = 0; ; ++n) {
		thinkos_sleep(100);
		c = fgetc(f_raw);
		loopback_stop();
		switch (c) {
		case '\r':
			show_menu(f);
			break;
		case 'l':
			loopback_start();
			break;
		case 's':
			stat_show(f);
			break;
		case 'd':
			stat_dist_show(f);
			break;
		case 'c':
			thinkos_mutex_lock(console_mutex);
			fprintf(f, "---- Clear Statistics --------\n");
			thinkos_mutex_unlock(console_mutex);
			stat_clear();
			break;
		case 'h':
			stat_hist_show(f);
			break;
		}
	}

	return 0;
}

int ui_task(FILE * f)
{
	int event;
	int c = 0;

	for (;;) {

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

	return 0;
}

int monitor_task(void * arg)
{
	unsigned int sec = 0;
	unsigned int min = 0;
	unsigned int hour = 0;

	for (;;) {
		thinkos_sleep(1000);
		led_flash(LED_I2S, 250);
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
#define INPUT_STACK_SIZE 2048
uint32_t input_stack[INPUT_STACK_SIZE / 4];
uint32_t monitor_stack[STACK_SIZE / 4];
uint32_t ui_stack[STACK_SIZE / 4];
uint32_t loopback_stack[STACK_SIZE / 4];

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

	console_mutex = thinkos_mutex_alloc();

	thinkos_thread_create((void *)input_task, (void *)f, 
						  input_stack, sizeof(input_stack), 1);

	thinkos_thread_create((void *)monitor_task, (void *)NULL, 
						  monitor_stack, STACK_SIZE, 2);

	printf("4. rs485_init()...\n");
	rs485_init(500000);

	thinkos_thread_create((void *)ui_task, (void *)f, 
						  ui_stack, STACK_SIZE, 3);

	loopback.flag = thinkos_flag_alloc();
	loopback.en = false;
	loopback.seed = 1LL;
	thinkos_thread_create((void *)loopback_task, (void *)f, 
						  loopback_stack, STACK_SIZE, 4);

	thinkos_sleep(100);
	led_off(LED_NET);
	led_off(LED_I2S);
	led_off(LED_S1);
	led_off(LED_S2);

	stat_init();

	ts[0] = timer_ts();
	c[0] = 0;
	fprintf(f, "\n------------------------------------\n");
	fprintf(f, "RS485 capture\n");
	fprintf(f, "------------------------------------\n\n");
	for (i = 0; ;i++) {
		while (fifo_is_empty(&rs485.rx_fifo)) {
			thinkos_flag_wait(rs485.rx_flag);
			__thinkos_flag_clr(rs485.rx_flag);
		}
		led_flash(LED_NET, 50);

		ts[1] = ts[0];
		c[1] = c[0];
		c[0] = fifo_get(&rs485.rx_fifo, ts);
		dt = (int32_t)ts[0] - ts[1];
		if (!(c[0] & 0x80)) {
			if (c[1] & 0x80) {
				if ((c[0] & 0x7f) == (c[1] & 0x7f)) {
					int idx;
//					printf("%4d %8d %02x\n", i, dt, c[0]);
					thinkos_mutex_lock(stat.mutex);
					stat.match++;
					idx = dt / BIN_RES;
					if (idx >= BIN_CNT)
						idx = BIN_CNT - 1;
					stat.bin[idx]++;
					stat.dt_sum += dt;
					if (dt > stat.dt_max)
						stat.dt_max = dt;
					if (dt < stat.dt_min)
						stat.dt_min = dt;
					thinkos_mutex_unlock(stat.mutex);

					led_flash(LED_S2, 50);
					thinkos_mutex_lock(console_mutex);
					fprintf(f, "%4d, %3d, %4d.%03d\n", 
							stat.match, c[0] & 0x7f, dt / 1000, dt % 1000);
					thinkos_mutex_unlock(console_mutex);
				} else {
					/* Numbers do not match!!! */
					led_flash(LED_S1, 100);
					stat.error++;
				}
			} else {
				/* previous was not master ??? */
				led_flash(LED_S1, 100);
				stat.error++;
			}
		} else {
			if (c[1] & 0x80) {
				/* master repeated */
				led_flash(LED_S1, 100);
				stat.error++;
			}
		}
	}

	return 0;
}

