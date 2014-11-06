/* 
 * File:	 serial-test.c
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

#include <hexdump.h>

#define __THINKOS_IRQ__
#include <thinkos_irq.h>
#include <thinkos.h>

#include <sys/dcclog.h>

#include "pktbuf.h"
#include "rs485lnk.h"

/* GPIO pin description */ 
struct stm32f_io {
	struct stm32_gpio * gpio;
	uint8_t pin;
};

/* ----------------------------------------------------------------------
 * LEDs 
 * ----------------------------------------------------------------------
 */

const struct stm32f_io led_io[] = {
	{ STM32_GPIOC, 1 },
	{ STM32_GPIOC, 14 },
	{ STM32_GPIOC, 7 },
	{ STM32_GPIOC, 8 }
};

void led_on(int id)
{
	stm32_gpio_set(led_io[id].gpio, led_io[id].pin);
}

void led_off(int id)
{
	stm32_gpio_clr(led_io[id].gpio, led_io[id].pin);
}

void leds_init(void)
{
	int i;

	for (i = 0; i < sizeof(led_io) / sizeof(struct stm32f_io); ++i) {
		stm32_gpio_mode(led_io[i].gpio, led_io[i].pin,
						 OUTPUT, PUSH_PULL | SPEED_LOW);

		stm32_gpio_clr(led_io[i].gpio, led_io[i].pin);
	}
}

struct rs485_link link;

#define USART2_TX STM32_GPIOA, 2
#define USART2_RX STM32_GPIOA, 3

#define LINK_TXEN STM32_GPIOA, 1
#define LINK_LOOP STM32_GPIOA, 0 

#define USART2_DMA_STRM_RX 5
#define USART2_DMA_CHAN_RX 4
#define USART2_DMA_IRQ_RX STM32F_IRQ_DMA1_STREAM5

#define USART2_DMA_STRM_TX 6
#define USART2_DMA_CHAN_TX 4
#define USART2_DMA_IRQ_TX STM32F_IRQ_DMA1_STREAM6

void stm32f_usart2_isr(void)
{
	rs485_link_isr(&link);
}

void net_init(void)
{
	printf("%s():...\n", __func__);

	/* IO init */
	stm32_gpio_mode(USART2_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	stm32_gpio_mode(USART2_RX, ALT_FUNC, PULL_UP);
	stm32_gpio_af(USART2_RX, GPIO_AF7);
	stm32_gpio_af(USART2_TX, GPIO_AF7);

	stm32_gpio_mode(LINK_TXEN, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32_gpio_mode(LINK_LOOP, OUTPUT, PUSH_PULL | SPEED_LOW);

	stm32_gpio_set(LINK_LOOP);
	stm32_gpio_set(LINK_TXEN);

	/* initialize the packet buffer pool */
	pktbuf_pool_init();

	/* DMA configuration for USART2 
	 * TX: DMA1, Stream6, Channel 4
	 * RX: DMA1, Stream5, Channel 4 
	 */

	/* Link init */
	rs485_init(&link, STM32_USART2, 1000, STM32F_DMA1, 
			   USART2_DMA_STRM_RX, USART2_DMA_CHAN_RX,
			   USART2_DMA_STRM_TX, USART2_DMA_CHAN_TX);

	cm3_irq_enable(STM32_IRQ_USART2);
}

int net_send(const void * buf, int len)
{
	void * pkt;

	pkt = pktbuf_alloc();
	if (pkt == NULL) {
		DCC_LOG(LOG_ERROR, "pktbuf_alloc() failed!");
		return -1;
	}

	memcpy(pkt, buf, len);

	DCC_LOG2(LOG_TRACE, "pkt=%p len=%d", pkt, len);

	len = MIN(len, pktbuf_len);

	pkt = rs485_pkt_enqueue(&link, pkt, len);

	if (pkt != NULL)
		pktbuf_free(pkt);

	return 0;
}

int net_recv(void * buf, int len)
{
	void * pkt;

	pkt = pktbuf_alloc();
	if (pkt == NULL) {
		DCC_LOG(LOG_ERROR, "pktbuf_alloc() failed!");
		printf("%s(): pktbuf_alloc() failed!\n", __func__);
		return -1;
	}

	len = rs485_pkt_receive(&link, &pkt, len);

//	printf("%s(): len=%d\n", __func__, len);

	DCC_LOG1(LOG_TRACE, "%d", len);

	DCC_LOG2(LOG_TRACE, "pkt=%p len=%d", pkt, len);

	if (pkt != NULL) {
		memcpy(buf, pkt, len);
		pktbuf_free(pkt);
	}

	return len;
}

/* ----------------------------------------------------------------------
 * Console 
 * ----------------------------------------------------------------------
 */

#define USART1_TX STM32_GPIOB, 6
#define USART1_RX STM32_GPIOB, 7

struct file stm32_uart1_file = {
	.data = STM32_USART1, 
	.op = &stm32_usart_fops 
};

void stdio_init(void)
{
	struct stm32_usart * uart = STM32_USART1;
#if defined(STM32F1X)
	struct stm32f_afio * afio = STM32F_AFIO;
#endif

	DCC_LOG(LOG_TRACE, "...");

	/* USART1_TX */
	stm32_gpio_mode(USART1_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);

#if defined(STM32F1X)
	/* USART1_RX */
	stm32f_gpio_mode(USART1_RX, INPUT, PULL_UP);
	/* Use alternate pins for USART1 */
	afio->mapr |= AFIO_USART1_REMAP;
#elif defined(STM32F4X)
	stm32_gpio_mode(USART1_RX, ALT_FUNC, PULL_UP);
	stm32_gpio_af(USART1_RX, GPIO_AF7);
	stm32_gpio_af(USART1_TX, GPIO_AF7);
#endif

	stm32_usart_init(uart);
	stm32_usart_baudrate_set(uart, 115200);
	stm32_usart_mode_set(uart, SERIAL_8N1);
	stm32_usart_enable(uart);

	stdin = &stm32_uart1_file;
	stdout = &stm32_uart1_file;
	stderr = &stm32_uart1_file;
}

/* ----------------------------------------------------------------------
 * I/O
 * ----------------------------------------------------------------------
 */
void io_init(void)
{
	DCC_LOG(LOG_MSG, "Configuring GPIO ports...");

	stm32_gpio_clock_en(STM32_GPIOA);
	stm32_gpio_clock_en(STM32_GPIOB);
	stm32_gpio_clock_en(STM32_GPIOC);
}

/* ----------------------------------------------------------------------
 * Supervisory task
 * ----------------------------------------------------------------------
 */
int supervisor_task(void)
{
	for (;;) {
		thinkos_sleep(200);
		led_on(0);
		led_on(1);
		thinkos_sleep(100);
		led_off(0);
		led_off(1);
		thinkos_sleep(400);
		led_on(2);
		led_on(3);
		thinkos_sleep(100);
		led_off(2);
		led_off(3);
		thinkos_sleep(400);
	}
}

/* ----------------------------------------------------------------------
 * Receive
 * ----------------------------------------------------------------------
 */
int rx_task(void)
{
	uint8_t buf[256];
	int n;
	int i;


	for (;;) {
		n = net_recv(buf, 256);
		for (i = 0; i < n; ++i) {
			printf("%c", buf[i]);
		}
	}
}

/* ----------------------------------------------------------------------
 * Transmit
 * ----------------------------------------------------------------------
 */
int tx_task(void)
{
	char msg[256];
	int n;
	int i;

	for (i = 0; ; ++i) {
		n = sprintf(msg, "%d - The quick brown fox jumps over the lazy dog...\n", i);
		net_send(msg, n);

//		thinkos_sleep(1000);
	}

	return 0;
}

uint32_t rx_stack[512];
uint32_t tx_stack[512];
uint32_t supervisor_stack[512];

int main(int argc, char ** argv)
{
	uint32_t * uid = STM32F_UID;
	int i = 0;

	DCC_LOG_INIT();
	DCC_LOG_CONNECT();

	/* calibrate usecond delay loop */
	cm3_udelay_calibrate();

	DCC_LOG(LOG_TRACE, "1. io_init()");
	io_init();

	DCC_LOG(LOG_TRACE, "2. leds_init()");
	leds_init();

	DCC_LOG(LOG_TRACE, "3. stdio_init()");
	stdio_init();

	printf("\n\n");
	printf("-----------------------------------------\n");
	printf(" RS485 token ring network test\n");
	printf("-----------------------------------------\n");
	printf("\n");

	DCC_LOG(LOG_TRACE, "4. thinkos_init()");
	thinkos_init(THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(32));

	DCC_LOG(LOG_TRACE, "5. net_init()");
	net_init();

	thinkos_thread_create((void *)supervisor_task, (void *)NULL,
						  supervisor_stack, sizeof(supervisor_stack), 
						  THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(0));

	thinkos_thread_create((void *)tx_task, (void *)NULL,
						  tx_stack, sizeof(tx_stack), 
						  THINKOS_OPT_PRIORITY(1) | THINKOS_OPT_ID(1));

	thinkos_thread_create((void *)rx_task, (void *)NULL,
						  rx_stack, sizeof(rx_stack), 
						  THINKOS_OPT_PRIORITY(2) | THINKOS_OPT_ID(2));


	printf("UID=%08x:%08x:%08x\n", uid[2], uid[1], uid[0]);
	DCC_LOG3(LOG_TRACE, "UID=%08x:%08x:%08x", uid[2], uid[1], uid[0]);

	for (i = 0; ; ++i) {
		thinkos_sleep(1000);
	}

	return 0;
}

