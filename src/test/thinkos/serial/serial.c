/* 
 * File:	 adc_dma.c
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
#include <sys/ice-comm.h>

#include <thinkos.h>

#define CONSOLE_RX_BUF_LEN 16

struct console_ctrl {
	struct stm32_usart * uart;
	struct stm32f_dma * dma;
	struct {
		struct stm32f_dma_stream * s;
	} tx;
	struct {
		struct stm32f_dma_stream * s;
		uint8_t buf[CONSOLE_RX_BUF_LEN];
	} rx;
};

static int inline getc(struct stm32_usart * usart, unsigned int msec)
{
	int tm;

	tm = msec * 20;

	for (;;) {		
		if (usart->sr & USART_RXNE) {
			return usart->dr;
		}
		if (tm == 0) {
			return -1;
		}
		udelay(50);
		tm--;
	}
}

int console_read(struct console_ctrl * ctrl, char * buf, 
				 unsigned int len, unsigned int msec)
{
	uint32_t st;
	uint32_t sr;

	fprintf(stderr, "%s(ctrl=%08x, buf=%08x, len=%d)\n", 
			__func__, (int)ctrl, (int)buf, len);

	if (ctrl->rx.s->cr & DMA_EN) {
		fprintf(stderr, "%s() DMA enabled\n", __func__);
	}

	/* Memory address */
	ctrl->rx.s->m0ar = (void *)buf;
	/* Number of data items to transfer */
	ctrl->rx.s->ndtr = len;

	/* clear the TC bit */
	if ((sr = ctrl->uart->sr) & USART_TC) {
		fprintf(stderr, "%s() TC=1\n", __func__);
	}

	/* enable DMA */
	ctrl->rx.s->cr = DMA_CHSEL_SET(4) | DMA_MBURST_1 | DMA_PBURST_1 | 
		DMA_MSIZE_8 | DMA_PSIZE_8 | DMA_MINC | 
		DMA_DIR_PTM | DMA_TCIE  | DMA_EN;

	/* wait for the DMA transfer to complete */
	while (((st = ctrl->dma->hisr) & DMA_TCIF7) == 0) {
		fprintf(stderr, "%s() %08x ", __func__, st);
		if (st & DMA_HTIF7)
			fprintf(stderr, "HT ");
		if (st & DMA_TEIF7)
			fprintf(stderr, "TE ");
		if (st & DMA_DMEIF7)
			fprintf(stderr, "DME ");
		if (st & DMA_FEIF7)
			fprintf(stderr, "FE ");
		fprintf(stderr, "wait...\n");
//		ctrl->dma->hifcr = st;

		/* wait for the DMA interrupt */
		thinkos_irq_wait(STM32F_IRQ_DMA1_STREAM7);
	} 

	fprintf(stderr, "%s() done.\n", __func__);

	/* clear the the DMA stream trasfer complete flag */
	ctrl->dma->hifcr = DMA_CTCIF7;

	return len;
}

int console_write(struct console_ctrl * ctrl, const void * buf, 
					   unsigned int len)
{
	uint32_t st;
	uint32_t sr;

	fprintf(stderr, "%s(ctrl=%08x, buf=%08x, len=%d)\n", 
			__func__, (int)ctrl, (int)buf, len);

	if (ctrl->tx.s->cr & DMA_EN) {
		fprintf(stderr, "%s() DMA enabled\n", __func__);
	}

	/* Memory address */
	ctrl->tx.s->m0ar = (void *)buf;
	/* Number of data items to transfer */
	ctrl->tx.s->ndtr = len;

	/* clear the TC bit */
	if ((sr = ctrl->uart->sr) & USART_TC) {
		fprintf(stderr, "%s() TC=1\n", __func__);
	}

	/* enable DMA */
	ctrl->tx.s->cr = DMA_CHSEL_SET(4) | DMA_MBURST_1 | DMA_PBURST_1 | 
		DMA_MSIZE_8 | DMA_PSIZE_8 | DMA_MINC | 
		DMA_DIR_MTP | DMA_TCIE  | DMA_EN;

	/* wait for the DMA transfer to complete */
	while (((st = ctrl->dma->lisr) & DMA_TCIF0) == 0) {
		fprintf(stderr, "%s() %08x ", __func__, st);
		if (st & DMA_HTIF0)
			fprintf(stderr, "HT ");
		if (st & DMA_TEIF0)
			fprintf(stderr, "TE ");
		if (st & DMA_DMEIF0)
			fprintf(stderr, "DME ");
		if (st & DMA_FEIF0)
			fprintf(stderr, "FE ");
		fprintf(stderr, "wait...\n");
//		ctrl->dma->lifcr = st;
		thinkos_irq_wait(STM32F_IRQ_DMA1_STREAM0);
	} 

	fprintf(stderr, "%s() done.\n", __func__);

	/* clear the the DMA stream trasfer complete flag */
	ctrl->dma->lifcr = DMA_CTCIF0;

	return len;
}

int console_flush(struct console_ctrl * ctrl)
{
	fprintf(stderr, "%s(ctrl=%08x)\n", __func__, (int)ctrl);
	return 0;
}

const struct fileop console_ops = {
	.write = (void *)console_write,
	.read = (void *)console_read,
	.flush = (void *)console_flush,
	.close = (void *)NULL
};

const struct console_ctrl uart5_ctrl = {
	.uart = STM32_UART5, 
	.dma = STM32F_DMA1,
	.rx.s = &STM32F_DMA1->s[0],
	.tx.s = &STM32F_DMA1->s[7]
};

const struct file console_file = {
	.data = (void *)&uart5_ctrl, 
	.op = &console_ops
};

struct file * console_open(unsigned int baudrate, unsigned int flags)
{
	struct console_ctrl * ctrl = (struct console_ctrl *)console_file.data;

	/* Enable DMA for transmission and reception */
	stm32_usart_init(ctrl->uart);
	stm32_usart_baudrate_set(ctrl->uart, baudrate);
	stm32_usart_mode_set(ctrl->uart, flags);

	/* Enable DMA for transmission and reception */
	ctrl->uart->cr3 |= USART_DMAT | USART_DMAR;

	/* DMA clock enable */
	if (ctrl->dma == STM32F_DMA1) {
		/* DMA clock enable */
		stm32_clk_enable(STM32_RCC, STM32_CLK_DMA1);
	} else {
		/* DMA clock enable */
		stm32_clk_enable(STM32_RCC, STM32_CLK_DMA2);
	}

	/* Disable DMA stream */
	ctrl->tx.s->cr = 0;
	while (ctrl->tx.s->cr & DMA_EN); /* Wait for the channel to be ready .. */
	ctrl->tx.s->par = &ctrl->uart->dr;
//	ctrl->tx.s->fcr = 0;
	ctrl->tx.s->fcr = DMA_DMDIS | DMA_FTH_FULL;

	/* Disable DMA stream */
	ctrl->rx.s->cr = 0;
	while (ctrl->rx.s->cr & DMA_EN); /* Wait for the channel to be ready .. */
	ctrl->rx.s->par = &ctrl->uart->dr;
	/* Memory address */
	ctrl->tx.s->m0ar = (void *)ctrl->rx.buf;
//	ctrl->tx.s->m0ar = (void *)ctrl->rx.buf[0];
//	ctrl->tx.s->m1ar = (void *)ctrl->rx.buf[1];
	/* Number of data items to transfer */
	ctrl->tx.s->ndtr = CONSOLE_RX_BUF_LEN;
	ctrl->rx.s->cr = DMA_CHSEL_SET(4) | DMA_MBURST_1 | DMA_PBURST_1 | 
		DMA_MSIZE_8 | DMA_PSIZE_8 | DMA_MINC | 
		DMA_DIR_PTM;

	return (struct file *)&console_file;
}

int main(int argc, char ** argv)
{
	FILE * f;	
	int i;

	cm3_udelay_calibrate();

	stdout = ice_comm_open();
	stderr = stdout;

	thinkos_init(THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(0));

	printf("\n");
	printf("---------------------------------------------------------\n");
	printf(" ThinkOS ADC-DMA Test\n");
	printf("---------------------------------------------------------\n");
	printf("\n");

	f = console_open(115200, SERIAL_8N1);

	for (i = 0; i < 10; i++) {
		fprintf(f, "\r\nHello world %d!\r\n", i);
		delay(1);
	}

	printf("---------------------------------------------------------\n");
	delay(10);

	return 0;
}

