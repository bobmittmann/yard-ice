/* 
 * File:	 i2s-slave.c
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
#include <sys/delay.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <hexdump.h>

#include "trace.h"
#include "spectrum.h"
#include "vt100.h"
#include "fixpt.h"
#include "sndbuf.h"
#include "tonegen.h"

#include <sys/dcclog.h>

#include <thinkos.h>
#define __THINKOS_IRQ__
#include <thinkos_irq.h>

#define I2S2_WS     STM32F_GPIOB, 12
#define I2S2_CK     STM32F_GPIOB, 13
#define I2S2EXT_SD  STM32F_GPIOB, 14
#define I2S2_SD     STM32F_GPIOB, 15

#define I2S_DMA_RX_STRM 3
#define I2S_DMA_RX_CHAN 0
#define I2S_DMA_RX_IRQ STM32F_IRQ_DMA1_STREAM3

#define I2S_DMA_TX_STRM 4
#define I2S_DMA_TX_CHAN 2
#define I2S_DMA_TX_IRQ STM32F_IRQ_DMA1_STREAM4

struct {
	struct {
		sndbuf_t * buf[2];
		volatile uint32_t idx;
	} tx;
	struct {
		sndbuf_t * buf[2];
		sndbuf_t * xfr[2];
		volatile uint32_t idx;
	} rx;
	int io_flag;
} i2s; 

void i2s_slave_init(void)
{
	struct stm32f_spi * spi = STM32F_SPI2;
	struct stm32f_spi * i2s_ext = STM32F_I2S2EXT;
	struct stm32f_rcc * rcc = STM32F_RCC;
	struct stm32f_dma * dma = STM32F_DMA1;

	tracef("%s SPI=0x%08x", __func__, (uint32_t)spi);
	tracef("%s I2S_EXT=0X%08x", __func__, (uint32_t)i2s_ext);


	stm32f_gpio_af(I2S2_WS, GPIO_AF5);
	stm32f_gpio_af(I2S2_CK, GPIO_AF5);
	stm32f_gpio_af(I2S2EXT_SD, GPIO_AF6);
	stm32f_gpio_af(I2S2_SD, GPIO_AF5);

	/* DMA clock enable */
	rcc->ahb1enr |= RCC_DMA1EN;

	DCC_LOG(LOG_TRACE, "1.");

	/* Disable DMA channel */
	dma->s[I2S_DMA_RX_STRM].cr = 0;
	while (dma->s[I2S_DMA_RX_STRM].cr & DMA_EN); 

	dma->s[I2S_DMA_TX_STRM].cr = 0;
	while (dma->s[I2S_DMA_TX_STRM].cr & DMA_EN); 

	DCC_LOG(LOG_TRACE, "2.");

	/* Enable SPI clock */
	rcc->apb1enr |= RCC_SPI2EN;

	/* disable peripherals */
	spi->i2scfgr &= ~SPI_I2SE;
	i2s_ext->i2scfgr &= ~SPI_I2SE;

	spi->cr1 = 0;
	spi->cr2 = 0;
	spi->i2scfgr = SPI_I2SMOD | SPI_I2SCFG_SLV_RCV | 
		SPI_PCMSYNC_SHORT | SPI_I2SSTD_PCM | SPI_DATLEN_16 | SPI_CHLEN_16;
	spi->i2spr = 0;
	spi->cr2 = SPI_RXDMAEN;

	DCC_LOG(LOG_TRACE, "3.");

	i2s_ext->cr1 = 0;
	i2s_ext->cr2 = 0;
	i2s_ext->i2scfgr = SPI_I2SMOD | SPI_I2SCFG_SLV_XMT | 
		SPI_PCMSYNC_SHORT | SPI_I2SSTD_PCM | SPI_DATLEN_16 | SPI_CHLEN_16;
	i2s_ext->i2spr = 0;
	i2s_ext->cr2 = SPI_TXDMAEN;

	DCC_LOG(LOG_TRACE, "4.");

	i2s.tx.buf[0] = (sndbuf_t *)&sndbuf_zero;
	i2s.tx.buf[1] = (sndbuf_t *)&sndbuf_zero;
	i2s.tx.idx = 0;

	i2s.rx.buf[0] = &sndbuf_null;
	i2s.rx.buf[1] = &sndbuf_null;
	i2s.rx.idx = 0;

	/* Configure DMA channel */
	dma->s[I2S_DMA_RX_STRM].cr = DMA_CHSEL_SET(I2S_DMA_RX_CHAN) | 
		DMA_MBURST_1 | DMA_PBURST_1 | DMA_MSIZE_16 | DMA_PSIZE_16 | 
		DMA_CT_M0AR | DMA_DBM |  DMA_CIRC | DMA_MINC | DMA_DIR_PTM |
		DMA_TCIE | DMA_TEIE | DMA_DMEIE;
	dma->s[I2S_DMA_RX_STRM].par = &spi->dr;
	dma->s[I2S_DMA_RX_STRM].m0ar = i2s.rx.buf[0]->data;
	i2s.rx.xfr[0] = i2s.rx.buf[0];
	dma->s[I2S_DMA_RX_STRM].m1ar = i2s.rx.buf[1]->data;
	i2s.rx.xfr[1] = i2s.rx.buf[1];
	dma->s[I2S_DMA_RX_STRM].ndtr = SNDBUF_LEN;
	dma->s[I2S_DMA_RX_STRM].fcr = DMA_FEIE | DMA_DMDIS | DMA_FTH_FULL;

	DCC_LOG(LOG_TRACE, "5.");

	dma->s[I2S_DMA_TX_STRM].cr = DMA_CHSEL_SET(I2S_DMA_TX_CHAN) | 
		DMA_MBURST_1 | DMA_PBURST_1 | DMA_MSIZE_16 | DMA_PSIZE_16 | 
		DMA_CT_M0AR | DMA_DBM | DMA_CIRC | DMA_MINC | DMA_DIR_MTP |
		DMA_TCIE | DMA_TEIE | DMA_DMEIE;
	dma->s[I2S_DMA_RX_STRM].par = &spi->dr;
	dma->s[I2S_DMA_TX_STRM].par = &i2s_ext->dr;
	dma->s[I2S_DMA_TX_STRM].m0ar = i2s.tx.buf[0]->data;
	dma->s[I2S_DMA_TX_STRM].m1ar = i2s.tx.buf[1]->data;
	dma->s[I2S_DMA_TX_STRM].ndtr = SNDBUF_LEN;
	dma->s[I2S_DMA_TX_STRM].fcr = DMA_FEIE | DMA_DMDIS | DMA_FTH_FULL;


	DCC_LOG(LOG_TRACE, "6.");

	/* Set DMA to medium priority */
	cm3_irq_pri_set(I2S_DMA_RX_IRQ, 0x10);
	cm3_irq_enable(I2S_DMA_RX_IRQ);

	DCC_LOG(LOG_TRACE, "7.");

	cm3_irq_pri_set(I2S_DMA_TX_IRQ, 0x10);
	cm3_irq_enable(I2S_DMA_TX_IRQ);

	DCC_LOG(LOG_TRACE, "8.");

	i2s.io_flag = thinkos_flag_alloc(); 
	tracef("%s(): flag=%d.", __func__, i2s.io_flag);
}

void i2s_enable(void)
{
	struct stm32f_spi * spi = STM32F_SPI2;
	struct stm32f_spi * i2s_ext = STM32F_I2S2EXT;
	struct stm32f_dma * dma = STM32F_DMA1;

	tracef("%s(): ...", __func__);
	DCC_LOG(LOG_TRACE, "...");

	/* Enable IO pins */
	stm32f_gpio_mode(I2S2_WS, ALT_FUNC, SPEED_MED);
	stm32f_gpio_mode(I2S2_CK, ALT_FUNC, SPEED_MED);
	stm32f_gpio_mode(I2S2EXT_SD, ALT_FUNC, PUSH_PULL | SPEED_MED);
	stm32f_gpio_mode(I2S2_SD, ALT_FUNC, SPEED_MED);

	/* enable DMA */
	dma->s[I2S_DMA_TX_STRM].cr |= DMA_EN;
	dma->s[I2S_DMA_RX_STRM].cr |= DMA_EN;

	/* Enable peripherals */
	spi->i2scfgr |= SPI_I2SE;
	i2s_ext->i2scfgr |= SPI_I2SE;

}

void i2s_disable(void)
{
	struct stm32f_spi * spi = STM32F_SPI2;
	struct stm32f_spi * i2s_ext = STM32F_I2S2EXT;
	struct stm32f_dma * dma = STM32F_DMA1;

	tracef("%s(): ... ", __func__);
	DCC_LOG(LOG_TRACE, "...");

	/* Disable peripherals */
	spi->i2scfgr &= ~SPI_I2SE;
	i2s_ext->i2scfgr &= ~SPI_I2SE;

	/* Disable DMA */
	dma->s[I2S_DMA_TX_STRM].cr &= ~DMA_EN | DMA_TCIE | DMA_TEIE | DMA_DMEIE;
	dma->s[I2S_DMA_RX_STRM].cr &= ~DMA_EN | DMA_TCIE | DMA_TEIE | DMA_DMEIE;	

	/* Disable IO pins */
	stm32f_gpio_mode(I2S2_WS, INPUT, 0);
	stm32f_gpio_mode(I2S2_CK, INPUT, 0);
	stm32f_gpio_mode(I2S2EXT_SD, INPUT, 0);
	stm32f_gpio_mode(I2S2_SD, INPUT, 0);
}



/* TX DMA IRQ */
void stm32f_dma1_stream4_isr(void)
{
	struct stm32f_dma * dma = STM32F_DMA1;

	if (dma->hisr & DMA_FEIF4) {
		trace("DMA_FEIF4");
		DCC_LOG(LOG_TRACE, "DMA_FEIF4");
		dma->hifcr = DMA_CFEIF4;
	}

	if (dma->hisr & DMA_TEIF4) {
		trace("DMA_TEIF4");
		DCC_LOG(LOG_TRACE, "DMA_TEIF4");
		dma->hifcr = DMA_CTEIF4;
	}

	if ((dma->hisr & DMA_TCIF4) == 0) {
	//	trace("!= DMA_TCIF4!!!");
		DCC_LOG(LOG_TRACE, "!=DMA_TCIF4");
		return;
	}

	DCC_LOG(LOG_INFO, "DMA_TCIF4");

	/* clear the DMA transfer complete flag */
	dma->hifcr = DMA_CTCIF4;

	if (dma->s[I2S_DMA_TX_STRM].cr & DMA_CT) {
		dma->s[I2S_DMA_TX_STRM].m0ar = i2s.tx.buf[0]->data;
		i2s.tx.idx = 1;
		DCC_LOG(LOG_INFO, "1.");
	} else {
		dma->s[I2S_DMA_TX_STRM].m1ar = i2s.tx.buf[1]->data;
		i2s.tx.idx = 0;
		DCC_LOG(LOG_INFO, "0.");
	}
}


/* RX DMA IRQ */
void stm32f_dma1_stream3_isr(void)
{
	struct stm32f_dma * dma = STM32F_DMA1;
	sndbuf_t * buf;

	if (dma->lisr & DMA_FEIF3) {
		trace("DMA_FEIF3");
		DCC_LOG(LOG_TRACE, "DMA_FEIF3");
		dma->lifcr = DMA_CFEIF3;
	}

	if (dma->lisr & DMA_TEIF3) {
		trace("DMA_TEIF3");
		DCC_LOG(LOG_TRACE, "DMA_TEIF3");
		dma->lifcr = DMA_CTEIF3;
	}

	if ((dma->lisr & DMA_TCIF3) == 0) {
		trace("!= DMA_TCIF3!!!");
		DCC_LOG(LOG_TRACE, "!=DMA_TCIF3");
		return;
	}


	/* clear the DMA transfer complete flag */
	dma->lifcr = DMA_CTCIF3;

	if (dma->s[I2S_DMA_RX_STRM].cr & DMA_CT) {
		
		buf = i2s.rx.xfr[0];
		dma->s[I2S_DMA_RX_STRM].m0ar = i2s.rx.buf[0]->data;
		i2s.rx.xfr[0] = i2s.rx.buf[0];
		i2s.rx.buf[0] = buf;
		i2s.rx.idx = 0;
//		tracef("%d <-- DMA[0] <-- %d", sndbuf_id(buf), 
//			   sndbuf_id(i2s.rx.xfr[0])); 

	} else {
		buf = i2s.rx.xfr[1];
		dma->s[I2S_DMA_RX_STRM].m1ar = i2s.rx.buf[1]->data;
		i2s.rx.xfr[1] = i2s.rx.buf[1];
		i2s.rx.buf[1] = buf;
		i2s.rx.idx = 1;
//		tracef("%d <-- DMA[1] <-- %d", sndbuf_id(buf), 
//			   sndbuf_id(i2s.rx.xfr[1])); 
	}

	__thinkos_flag_signal(i2s.io_flag);
}


sndbuf_t * i2s_io(sndbuf_t * out_buf)
{
	sndbuf_t * in_buf;
	sndbuf_t * buf;
	int idx;

	if (out_buf == NULL) {
		trace("txbuf = zero");
		out_buf = (sndbuf_t *)&sndbuf_zero;
	} else {
		/* incerement the reference counter */
		sndbuf_use(out_buf);
	}

	/* wait for the end of transfer cycle */
//	tracef("%s(): wait ", __func__);
	thinkos_flag_wait(i2s.io_flag);
	__thinkos_flag_clr(i2s.io_flag);

	idx = i2s.rx.idx;
	/* get the input data buffer */
	in_buf = i2s.rx.buf[idx];
	/* alloc an empty buffer for next cycle */
	buf = sndbuf_alloc();
	
	if (buf == NULL) {
		tracef("%s(): sndbuf_alloc() failed!", __func__);
	}

//	tracef("%d <-- IN[%d] <-- %d", sndbuf_id(in_buf), idx, sndbuf_id(buf)); 
	/* set the input buffer for next cycle */
	i2s.rx.buf[idx] = (buf == NULL) ? &sndbuf_null : buf;

	idx = i2s.tx.idx;
	/* release the previouslly transmitted buffer */
	sndbuf_free(i2s.tx.buf[idx]);
	/* set the output data buffer */
	i2s.tx.buf[idx] = out_buf;

	return in_buf;
}


