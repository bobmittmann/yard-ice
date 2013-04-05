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

#include <pktbuf.h>

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

	for (i = 0; i < sizeof(led_io) / sizeof(struct stm32f_io); ++i) {
		stm32f_gpio_mode(led_io[i].gpio, led_io[i].pin,
						 OUTPUT, PUSH_PULL | SPEED_LOW);

		stm32f_gpio_clr(led_io[i].gpio, led_io[i].pin);
	}
}

/* ----------------------------------------------------------------------
 * RS485
 * ----------------------------------------------------------------------
 */


/* ----------------------------------------------------------------------
 * RS485 network
 * ----------------------------------------------------------------------
 */

struct rs485_link {
	/* USART */
	struct stm32f_usart * uart;
	/* DMA controller */
	struct stm32f_dma * dma;
	struct {
		/* DMA stream/channel id */
		int dma_id;
		void * pend_pkt;
		/* Bitband pointer to interrupt status flags */
		uint32_t * isr;
		/* Bitband pointer to interrupt clear flags */
		uint32_t * ifcr;
		int dma_irq;
	} tx;

	struct {
		/* DMA stream/channel id */
		int dma_id;
	} rx;
};

const uint8_t dma_isr_base_lut[] = {
	     0,      6,      16,      22,
	32 + 0, 32 + 6, 32 + 16, 32 + 22,
};

const uint8_t stm32f_dma1_irqnum_lut[] = {
	STM32F_IRQ_DMA1_STREAM0,
	STM32F_IRQ_DMA1_STREAM1,
	STM32F_IRQ_DMA1_STREAM2,
	STM32F_IRQ_DMA1_STREAM3,
	STM32F_IRQ_DMA1_STREAM4,
	STM32F_IRQ_DMA1_STREAM5,
	STM32F_IRQ_DMA1_STREAM6,
	STM32F_IRQ_DMA1_STREAM7
};

const uint8_t stm32f_dma2_irqnum_lut[] = {
	STM32F_IRQ_DMA2_STREAM0,
	STM32F_IRQ_DMA2_STREAM1,
	STM32F_IRQ_DMA2_STREAM2,
	STM32F_IRQ_DMA2_STREAM3,
	STM32F_IRQ_DMA2_STREAM4,
	STM32F_IRQ_DMA2_STREAM5,
	STM32F_IRQ_DMA2_STREAM6,
	STM32F_IRQ_DMA2_STREAM7
};

#define FEIF_BIT 0
#define DMEIF_BIT 2
#define TEIF_BIT 3
#define HTIF_BIT 4
#define TCIF_BIT 5

void rs485_init(struct rs485_link * lnk, 
				struct stm32f_usart * uart,
				unsigned int speed,
				struct stm32f_dma * dma,
				int rx_dma_strm_id, int rx_dma_chan_id, 
				int tx_dma_strm_id, int tx_dma_chan_id)
{
	struct stm32f_dma_stream * tx_dma_strm;
	struct stm32f_dma_stream * rx_dma_strm;
	struct stm32f_rcc * rcc = STM32F_RCC;
	int isr_base;

	lnk->uart = uart;
	lnk->dma = dma;
	lnk->tx.pend_pkt = NULL;


	/* DMA clock enable */
	if (dma == STM32F_DMA1) {
		rcc->ahb1enr |= RCC_DMA1EN;
		lnk->tx.dma_irq = stm32f_dma1_irqnum_lut[tx_dma_strm_id];
	} else {
		rcc->ahb1enr |= RCC_DMA2EN;
		lnk->tx.dma_irq = stm32f_dma2_irqnum_lut[tx_dma_strm_id];
	}

	/* TX DMA */
	lnk->tx.dma_id = tx_dma_strm_id;

	isr_base = dma_isr_base_lut[tx_dma_strm_id]; 
	lnk->tx.isr = CM3_BITBAND_DEV(&dma->lisr, isr_base);
	lnk->tx.ifcr = CM3_BITBAND_DEV(&dma->lifcr, isr_base);

	DCC_LOG2(LOG_TRACE, "0x%p 0x%p", lnk->tx.isr, lnk->tx.ifcr);

	tx_dma_strm = &dma->s[tx_dma_strm_id];
	DCC_LOG2(LOG_TRACE, "TX DMA stream[%d]=0x%p", lnk->tx.dma_id, tx_dma_strm);

	/* Disable DMA stream */
	tx_dma_strm->cr = 0;
	while (tx_dma_strm->cr & DMA_EN); /* Wait for the channel to be ready .. */

	/* clear all interrupt flags */
	lnk->tx.ifcr[FEIF_BIT] = 1;
	lnk->tx.ifcr[DMEIF_BIT] = 1;
	lnk->tx.ifcr[TEIF_BIT] = 1;
	lnk->tx.ifcr[HTIF_BIT] = 1;
	lnk->tx.ifcr[TCIF_BIT] = 1; 

	tx_dma_strm->cr = DMA_CHSEL_SET(tx_dma_chan_id) | 
		DMA_MBURST_1 | DMA_PBURST_1 | 
		DMA_MSIZE_8 | DMA_PSIZE_8 | DMA_MINC | DMA_DIR_MTP | 
		DMA_TCIE | DMA_TEIE;


	tx_dma_strm->par = &uart->dr;
	tx_dma_strm->fcr = DMA_DMDIS | DMA_FTH_FULL;

	/* RX DMA */
	lnk->rx.dma_id = rx_dma_strm_id;

	rx_dma_strm = &dma->s[rx_dma_strm_id];
	DCC_LOG2(LOG_TRACE, "RX DMA stream[%d]=0x%p", lnk->rx.dma_id, rx_dma_strm);

	/* Disable DMA stream */
	rx_dma_strm->cr = 0;
	while (rx_dma_strm->cr & DMA_EN); /* Wait for the channel to be ready .. */

	rx_dma_strm->cr = DMA_CHSEL_SET(rx_dma_chan_id) | 
		DMA_MBURST_1 | DMA_PBURST_1 | 
		DMA_MSIZE_8 | DMA_PSIZE_8 | DMA_MINC | DMA_DIR_PTM;

	rx_dma_strm->par = &uart->dr;
	rx_dma_strm->fcr = DMA_DMDIS | DMA_FTH_FULL;

	stm32f_usart_init(uart);
	stm32f_usart_baudrate_set(uart, speed);
	stm32f_usart_mode_set(uart, SERIAL_8N1);

	/* Enable DMA for transmission and reception */
//	uart->cr3 |= USART_DMAT | USART_DMAR;

	uart->cr3 |= USART_DMAT;

	stm32f_usart_enable(uart);
}

void * _rs485_pkt_enqueue(struct rs485_link * lnk, void * pkt, int len)
{
	int i;
	uint8_t * s = (uint8_t *)pkt;

	for (i = 0; i < len; ++i) {
		stm32f_usart_putc(lnk->uart, s[i]);
		thinkos_sleep(10);
	}

	return pkt;
}

void * rs485_pkt_enqueue(struct rs485_link * lnk, void * pkt, int len)
{
	struct stm32f_usart * uart = lnk->uart;
	struct stm32f_dma * dma = lnk->dma;
	struct stm32f_dma_stream * dma_strm;
	uint32_t sr;
	void * pend_pkt = NULL;

	dma_strm = &dma->s[lnk->tx.dma_id];

	DCC_LOG2(LOG_INFO, "DMA stream[%d]=0x%p", lnk->tx.dma_id, dma_strm);

	if (lnk->tx.pend_pkt) {
		DCC_LOG(LOG_TRACE, "pending packet...");

		/* wait for the DMA transfer to complete */
		while (!lnk->tx.isr[TCIF_BIT]) {
			if (lnk->tx.isr[TEIF_BIT]) {
				DCC_LOG(LOG_ERROR, "DMA transfer error!");
				lnk->tx.ifcr[TEIF_BIT] = 1;
			}
			thinkos_irq_wait(lnk->tx.dma_irq);
		} 

		/* clear the the DMA trasfer complete flag */
		lnk->tx.ifcr[TCIF_BIT] = 1;

		/* return a reference to the packet just transmitted */
		pend_pkt = lnk->tx.pend_pkt;
	}
	
	if (dma_strm->cr & DMA_EN) {
		DCC_LOG(LOG_ERROR, "DMA enabled");
	}

	/* set this packet as pending */
	lnk->tx.pend_pkt = pkt;
	/* set DMA memory address */
	dma_strm->m0ar = (void *)pkt;
	/* set DMA number of data items to transfer */
	dma_strm->ndtr = len;

	/* clear the TC bit */
	if ((sr = uart->sr) & USART_TC) {
		DCC_LOG(LOG_INFO, "TC=1");
		uart->sr = 0;
	}

	/* enable DMA */
	dma_strm->cr |= DMA_EN;

	/* return previous pending packet */
	return pend_pkt;
}


struct rs485_link link;

#define USART2_TX STM32F_GPIOA, 2
#define USART2_RX STM32F_GPIOA, 3

#define LINK_TXEN STM32F_GPIOA, 1
#define LINK_LOOP STM32F_GPIOA, 0 

#define USART2_DMA_STRM_RX 5
#define USART2_DMA_CHAN_RX 4
#define USART2_DMA_IRQ_RX STM32F_IRQ_DMA1_STREAM5

#define USART2_DMA_STRM_TX 6
#define USART2_DMA_CHAN_TX 4
#define USART2_DMA_IRQ_TX STM32F_IRQ_DMA1_STREAM6

void net_init(void)
{
	printf("%s():...\n", __func__);

	/* IO init */
	stm32f_gpio_mode(USART2_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	stm32f_gpio_mode(USART2_RX, ALT_FUNC, PULL_UP);
	stm32f_gpio_af(USART2_RX, GPIO_AF7);
	stm32f_gpio_af(USART2_TX, GPIO_AF7);

	stm32f_gpio_mode(LINK_TXEN, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32f_gpio_mode(LINK_LOOP, OUTPUT, PUSH_PULL | SPEED_LOW);

	stm32f_gpio_set(LINK_LOOP);
	stm32f_gpio_set(LINK_TXEN);

	/* initialize the packet buffer pool */
	pktbuf_pool_init();

	/* DMA configuration for USART2 
	 * TX: DMA1, Stream6, Channel 4
	 * RX: DMA1, Stream5, Channel 4 
	 */

	/* Link init */
	rs485_init(&link, STM32F_USART2, 10000, STM32F_DMA1, 
			   USART2_DMA_STRM_RX, USART2_DMA_CHAN_RX,
			   USART2_DMA_STRM_TX, USART2_DMA_CHAN_TX);
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

	pkt = rs485_pkt_enqueue(&link, pkt, len);

	if (pkt != NULL)
		pktbuf_free(pkt);

	return 0;
}

int net_recv(void * buf, int len)
{
	struct stm32f_usart * uart = STM32F_USART2;
	uint8_t * data = (uint8_t *)buf;
	int c;	
	int i;

	for (i = 0; i < len; ++i) {
		while ((c = stm32f_usart_getc(uart, 0)) == -1) {
			continue;
		}
		printf("%c", c);
		data[i] = c;
	}
	DCC_LOG1(LOG_TRACE, "%d", len);

	return 0;
}

/* ----------------------------------------------------------------------
 * Console 
 * ----------------------------------------------------------------------
 */

#define USART1_TX STM32F_GPIOB, 6
#define USART1_RX STM32F_GPIOB, 7

struct file stm32f_uart1_file = {
	.data = STM32F_USART1, 
	.op = &stm32f_usart_fops 
};

void stdio_init(void)
{
	struct stm32f_usart * uart = STM32F_USART1;
#if defined(STM32F1x)
	struct stm32f_afio * afio = STM32F_AFIO;
#endif

	DCC_LOG(LOG_TRACE, "...");

	/* USART1_TX */
	stm32f_gpio_mode(USART1_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);

#if defined(STM32F1X)
	/* USART1_RX */
	stm32f_gpio_mode(USART1_RX, INPUT, PULL_UP);
	/* Use alternate pins for USART1 */
	afio->mapr |= AFIO_USART1_REMAP;
#elif defined(STM32F4X)
	stm32f_gpio_mode(USART1_RX, ALT_FUNC, PULL_UP);
	stm32f_gpio_af(USART1_RX, GPIO_AF7);
	stm32f_gpio_af(USART1_TX, GPIO_AF7);
#endif

	stm32f_usart_init(uart);
	stm32f_usart_baudrate_set(uart, 115200);
	stm32f_usart_mode_set(uart, SERIAL_8N1);
	stm32f_usart_enable(uart);

	stdin = &stm32f_uart1_file;
	stdout = &stm32f_uart1_file;
	stderr = &stm32f_uart1_file;
}

/* ----------------------------------------------------------------------
 * I/O
 * ----------------------------------------------------------------------
 */
void io_init(void)
{
	DCC_LOG(LOG_MSG, "Configuring GPIO ports...");

	stm32f_gpio_clock_en(STM32F_GPIOA);
	stm32f_gpio_clock_en(STM32F_GPIOB);
	stm32f_gpio_clock_en(STM32F_GPIOC);
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
	uint8_t pkt[256];

	for (;;) {
		net_recv(pkt, 256);
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
	}

	return 0;
}

uint32_t rx_stack[256];
uint32_t tx_stack[256];
uint32_t supervisor_stack[256];

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

