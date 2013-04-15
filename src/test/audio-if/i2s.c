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

#define SAMPLE_RATE 8000

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
		uint32_t idx;
	} tx;
	struct {
		sndbuf_t * buf[2];
		uint32_t idx;
	} rx;
} i2s; 

struct spectrum i2s_tx_sa;
struct spectrum i2s_rx_sa;
struct tonegen tonegen;

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
	spi->i2scfgr |= SPI_I2SE;
	i2s_ext->i2scfgr |= SPI_I2SE;

	spi->cr1 = 0;
	spi->cr2 = 0;
	spi->i2scfgr = SPI_I2SMOD | SPI_I2SCFG_SLV_RCV | 
		SPI_PCMSYNC_SHORT | SPI_CKPOL_LO | SPI_I2SSTD_PCM | SPI_DATLEN_16 |
		SPI_CHLEN_16;
	spi->i2spr = 0;
	spi->cr2 = SPI_RXDMAEN;

	DCC_LOG(LOG_TRACE, "3.");

	i2s_ext->cr1 = 0;
	i2s_ext->cr2 = 0;
	i2s_ext->i2scfgr = SPI_I2SMOD | SPI_I2SCFG_SLV_XMT | 
		SPI_PCMSYNC_SHORT | SPI_CKPOL_LO | SPI_I2SSTD_PCM | SPI_DATLEN_16 |
		SPI_CHLEN_16;
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
		DMA_CT_M0AR | DMA_DBM |  DMA_CIRC | DMA_MINC | DMA_DIR_PTM;
//		DMA_TCIE | DMA_TEIE | DMA_DMEIE;
	dma->s[I2S_DMA_RX_STRM].par = &spi->dr;
	dma->s[I2S_DMA_RX_STRM].m0ar = i2s.rx.buf[0];
	dma->s[I2S_DMA_RX_STRM].m1ar = i2s.rx.buf[1];
	dma->s[I2S_DMA_RX_STRM].ndtr = SNDBUF_LEN;
	dma->s[I2S_DMA_RX_STRM].fcr = DMA_FEIE | DMA_DMDIS | DMA_FTH_FULL;

	DCC_LOG(LOG_TRACE, "5.");

	dma->s[I2S_DMA_TX_STRM].cr = DMA_CHSEL_SET(I2S_DMA_TX_CHAN) | 
		DMA_MBURST_1 | DMA_PBURST_1 | DMA_MSIZE_16 | DMA_PSIZE_16 | 
		DMA_CT_M0AR | DMA_DBM | DMA_CIRC | DMA_MINC | DMA_DIR_MTP;
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

//	dma->s[I2S_DMA_TX_STRM].cr |= DMA_EN;	
//	dma->s[I2S_DMA_RX_STRM].cr |= DMA_EN;	

	tonegen_init(&tonegen, 0, 0);

	spectrum_init(&i2s_tx_sa, SAMPLE_RATE);
	spectrum_init(&i2s_rx_sa, SAMPLE_RATE);
}

void i2s_enable(void)
{
	struct stm32f_spi * spi = STM32F_SPI2;
	struct stm32f_spi * i2s_ext = STM32F_I2S2EXT;
	struct stm32f_dma * dma = STM32F_DMA1;

	tracef("%s...", __func__);
	DCC_LOG(LOG_TRACE, "...");

	/* Enable IO pins */
	stm32f_gpio_mode(I2S2_WS, ALT_FUNC, SPEED_MED);
	stm32f_gpio_mode(I2S2_CK, ALT_FUNC, SPEED_MED);
	stm32f_gpio_mode(I2S2EXT_SD, ALT_FUNC, PUSH_PULL | SPEED_MED);
	stm32f_gpio_mode(I2S2_SD, ALT_FUNC, SPEED_MED);

	/* flush TX bufffers */
	if (i2s.tx.buf[0] != &sndbuf_zero) {
		sndbuf_free(i2s.tx.buf[0]);
		i2s.tx.buf[0] = (sndbuf_t *)&sndbuf_zero;
	}

	if (i2s.tx.buf[1] != &sndbuf_zero) {
		sndbuf_free(i2s.tx.buf[1]);
		i2s.tx.buf[1] = (sndbuf_t *)&sndbuf_zero;
	}

	/* flush RX bufffers */
	if (i2s.rx.buf[0] != &sndbuf_null) {
		sndbuf_free(i2s.rx.buf[0]);
		i2s.rx.buf[0] = &sndbuf_null;
	}

	if (i2s.rx.buf[1] != &sndbuf_null) {
		sndbuf_free(i2s.rx.buf[1]);
		i2s.rx.buf[1] = &sndbuf_null;
	}

	/* enable DMA */
	dma->s[I2S_DMA_TX_STRM].cr |= DMA_EN | DMA_TCIE | DMA_TEIE | DMA_DMEIE;
	dma->s[I2S_DMA_RX_STRM].cr |= DMA_EN | DMA_TCIE | DMA_TEIE | DMA_DMEIE;	

	/* Enable peripherals */
	spi->i2scfgr |= SPI_I2SE;
	i2s_ext->i2scfgr |= SPI_I2SE;
}

void i2s_disable(void)
{
	struct stm32f_spi * spi = STM32F_SPI2;
	struct stm32f_spi * i2s_ext = STM32F_I2S2EXT;
	struct stm32f_dma * dma = STM32F_DMA1;

	tracef("%s...", __func__);
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
	sndbuf_t * buf;
	int idx;

	if (dma->hisr & DMA_FEIF4) {
		trace("DMA_FEIF4");
		DCC_LOG(LOG_TRACE, "DMA_FEIF4");
		dma->hifcr = DMA_CFEIF4;
//		dma->s[I2S_DMA_TX_STRM].cr = 0;	
	}

	if (dma->hisr & DMA_DMEIF4) {
		trace("DMA_DMEIF4");
		DCC_LOG(LOG_TRACE, "DMA_DMEIF4");
		dma->hifcr = DMA_CDMEIF4;
	//	dma->s[I2S_DMA_TX_STRM].cr = 0;	
	}

	if (dma->hisr & DMA_TEIF4) {
		trace("DMA_TEIF4");
		DCC_LOG(LOG_TRACE, "DMA_TEIF4");
		dma->hifcr = DMA_CTEIF4;
//		dma->s[I2S_DMA_TX_STRM].cr = 0;	
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
		if (i2s.tx.buf[0] == &sndbuf_zero) {
			trace("txbuf[0] == zero!!");
			DCC_LOG(LOG_TRACE, "TXBUF[0] == zero!!");
		}
		dma->s[I2S_DMA_TX_STRM].m0ar = i2s.tx.buf[0]->data;
		i2s.tx.idx = idx = 1;
		DCC_LOG(LOG_INFO, "1.");
	} else {
		if (i2s.tx.buf[1] == &sndbuf_zero) {
			trace("txbuf[1] == zero!!");
			DCC_LOG(LOG_TRACE, "TXBUF[1] == zero!!");
		}
		dma->s[I2S_DMA_TX_STRM].m1ar = i2s.tx.buf[1]->data;
		i2s.tx.idx = idx = 0;
		DCC_LOG(LOG_INFO, "0.");
	}

	buf = i2s.tx.buf[idx];
	if (buf != &sndbuf_zero) {
		i2s.tx.buf[idx] = (sndbuf_t *)&sndbuf_zero;
		sndbuf_free(buf);
	} else {
		DCC_LOG1(LOG_WARNING, "TXBUF[%d] == zero", idx);
	}
}


/* RX DMA IRQ */
void stm32f_dma1_stream3_isr(void)
{
	struct stm32f_dma * dma = STM32F_DMA1;
	sndbuf_t * buf;
	int idx;

	if (dma->lisr & DMA_FEIF3) {
		trace("DMA_FEIF3");
		DCC_LOG(LOG_TRACE, "DMA_FEIF3");
		dma->lifcr = DMA_CFEIF3;
	}

	if (dma->lisr & DMA_DMEIF3) {
		trace("DMA_DMEIF3");
		DCC_LOG(LOG_TRACE, "DMA_DMEIF3");
		dma->lifcr = DMA_CDMEIF3;
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

//	tracef("RX DMA_TCIF3 %d", i2s_rx_cnt); 

	/* clear the DMA transfer complete flag */
	dma->lifcr = DMA_CTCIF3;

	if ((buf = sndbuf_alloc()) == NULL) {
		trace("rxbuf == null!!!!");
		buf = &sndbuf_null;
	}

	if (dma->s[I2S_DMA_RX_STRM].cr & DMA_CT) {
		if (i2s.rx.buf[0] != &sndbuf_null) {
			trace("rxbuf[0] != null!!");
			DCC_LOG(LOG_TRACE, "RXBUF[0] != null!!");
		}
		i2s.rx.buf[0] = buf;
		dma->s[I2S_DMA_RX_STRM].m0ar = buf->data;
		i2s.rx.idx = idx = 1;
		DCC_LOG(LOG_INFO, "1.");
	} else {
		if (i2s.rx.buf[1] != &sndbuf_null) {
			trace("rxbuf[1] != null!!");
			DCC_LOG(LOG_TRACE, "RXBUF[1] != null!!");
		}
		i2s.rx.buf[1] = buf;
		dma->s[I2S_DMA_RX_STRM].m1ar = buf->data;
		i2s.rx.idx = idx = 0;

		DCC_LOG(LOG_INFO, "0.");
	}

	/* get the input data */
	buf = i2s.rx.buf[idx];
	if (buf != &sndbuf_null) {
		i2s.rx.buf[idx] = &sndbuf_null;
		spectrum_rec(&i2s_rx_sa, buf);
		sndbuf_free(buf);
	}

	if ((buf = sndbuf_alloc()) == NULL) {
		trace("sndbuf_alloc() failed!");
	} else {
		tonegen_apply(&tonegen, buf);
		spectrum_rec(&i2s_tx_sa, buf);

		if (i2s.tx.buf[i2s.tx.idx] != &sndbuf_zero) {
			trace("txbuf != zero");
		}

		i2s.tx.buf[i2s.tx.idx] = buf;
	}
}


void i2s_tx_analyze(void) 
{
//	spectrum_reset(&i2s_tx_sa);

	spectrum_run(&i2s_tx_sa);

	spectrum_normalize(&i2s_tx_sa);

//	spectrum_pwr_show(&i2s_tx_sa);

	spectrum_mag_show(&i2s_tx_sa);
}

void i2s_rx_analyze(void) 
{
//	spectrum_reset(&i2s_rx_sa);

	spectrum_run(&i2s_rx_sa);

	spectrum_normalize(&i2s_rx_sa);

//	spectrum_pwr_show(&i2s_rx_sa);

	spectrum_mag_show(&i2s_rx_sa);
}

void i2s_rx_dump(void)
{
	printf("RX frames: %d\n", i2s.rx.idx);

	show_hex16(stdout, 0, i2s.rx.buf[i2s.rx.idx], SNDBUF_LEN * 2);

	printf("\n");
}

void i2s_tx_dump(void)
{
	printf("TX frames: %d\n", i2s.tx.idx);

	show_hex16(stdout, 0, i2s.tx.buf[i2s.tx.idx], SNDBUF_LEN * 2);

	printf("\n");
}

int i2s_tone_set(int tone, int gain)
{
	return tonegen_init(&tonegen, q15_db2amp(gain), tone);
}

