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

#include "wavetab.h"
#include "trace.h"

#include <sys/dcclog.h>

#define AUDIO_FRAME_LEN 128

struct tonegen {
	int16_t * wave;
	uint32_t len;
	uint32_t pos;
	int32_t amp;
};

void tonegen_init(struct tonegen * gen, int amp, int tone)
{
	gen->wave = (int16_t *)wave_lut[tone].buf;
	gen->len = wave_lut[tone].len;
	gen->amp = amp;
	gen->pos = 0;
}

void tonegen_apply(struct tonegen * gen, int16_t frm[])
{
	int i;

	for (i = 0; i < AUDIO_FRAME_LEN; ++i) {
//		frm[i] = (gen->amp * gen->wave[gen->pos]) >> 15;
		frm[i] = gen->wave[gen->pos];
		if (++gen->pos == gen->len)
			gen->pos = 0;
	}

//	DCC_LOG1(LOG_TRACE, "sp=%08x", cm3_sp_get());
}

void blank_apply(int16_t frm[])
{
	int i;

	for (i = 0; i < AUDIO_FRAME_LEN; ++i) {
		frm[i] = 0;
	}
}

void ramp_apply(int16_t frm[])
{
	int i;
	int d = 65536 / AUDIO_FRAME_LEN;
	int v = -32768;

	for (i = 0; i < AUDIO_FRAME_LEN; ++i) {
		frm[i] = v;
		v += d;
	}
}

void pulse_apply(int16_t frm[])
{
	int i;

	for (i = 0; i < AUDIO_FRAME_LEN; ++i) {
		frm[i] = (1 << (i % 16));
	}
}

void pattern_apply(int16_t frm[])
{
	int i;

	for (i = 0; i < AUDIO_FRAME_LEN; i += 8) {
		frm[i] = 0x4000;
		frm[i + 1] = 0x4000;
		frm[i + 2] = 0x4000;
		frm[i + 3] = 0x4000;

		frm[i + 4] = 0x0000;
		frm[i + 5] = 0x0000;
		frm[i + 6] = 0x0000;
		frm[i + 7] = 0x0000;
	}
}


struct tonegen tonegen;

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
//		struct stm32f_dmactl dma;
		int16_t buf[2][AUDIO_FRAME_LEN];
	} tx;
	struct {
//		struct stm32f_dmactl dma;
		int16_t buf[2][AUDIO_FRAME_LEN];
	} rx;
} i2s; 

void i2s_test_init(void)
{
	int i;

	tonegen_init(&tonegen, 16384, WAVE_1K);

	for (i = 0; i < AUDIO_FRAME_LEN; ++i) {
		i2s.tx.buf[0][i] =  wave_1k[i % 8];
//		i2s.tx.buf[1][i] =  wave_1k[i % 8];
		i2s.tx.buf[1][i] =  0;
	}

	for (i = 0; i < AUDIO_FRAME_LEN; ++i) {
		tracef("%d", i2s.tx.buf[0][i]);
	}
}

void i2s_slave_init(void)
{
	struct stm32f_spi * spi = STM32F_SPI2;
	struct stm32f_spi * i2s_ext = STM32F_I2S2EXT;
	struct stm32f_rcc * rcc = STM32F_RCC;
	struct stm32f_dma * dma = STM32F_DMA1;

	tracef("%s SPI=0x%08x", __func__, (uint32_t)spi);
	tracef("%s I2S_EXT=0X%08x", __func__, (uint32_t)i2s_ext);

	stm32f_gpio_mode(I2S2_WS, ALT_FUNC, SPEED_MED);
	stm32f_gpio_mode(I2S2_CK, ALT_FUNC, SPEED_MED);
	stm32f_gpio_mode(I2S2EXT_SD, ALT_FUNC, PUSH_PULL | SPEED_MED);
	stm32f_gpio_mode(I2S2_SD, ALT_FUNC, SPEED_MED);

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
		SPI_PCMSYNC_SHORT | SPI_I2SSTD_PCM | SPI_DATLEN_16 |
		SPI_CHLEN_16;
	spi->i2spr = 0;
	spi->cr2 = SPI_RXDMAEN;

	DCC_LOG(LOG_TRACE, "3.");

	i2s_ext->cr1 = 0;
	i2s_ext->cr2 = 0;
	i2s_ext->i2scfgr = SPI_I2SMOD | SPI_I2SCFG_SLV_XMT | 
		SPI_PCMSYNC_SHORT | SPI_I2SSTD_PCM | SPI_DATLEN_16 |
		SPI_CHLEN_16;
	i2s_ext->i2spr = 0;
	i2s_ext->cr2 = SPI_TXDMAEN;

	DCC_LOG(LOG_TRACE, "4.");

#if 0
	/* configure RX DMA stream */
	stm32f_dmactl_init(&i2s.rx.dma, STM32F_DMA1, I2S_DMA_RX_STRM);
	/* Configuration for double buffer circular, 
	   half-transfer interrupt  */
	i2s.rx.dma.strm->cr = DMA_CHSEL_SET(I2S_DMA_RX_CHAN) | 
		DMA_MBURST_1 | DMA_PBURST_1 | DMA_MSIZE_16 | DMA_PSIZE_16 | 
		DMA_CIRC | DMA_MINC | DMA_DIR_PTM |
		DMA_HTIE | DMA_TCIE | DMA_TEIE;
	i2s.rx.dma.strm->par = &spi->dr;
	i2s.rx.dma.strm->m0ar = i2s.rx.buf;
	i2s.rx.dma.strm->ndtr = 2 * AUDIO_FRAME_LEN;
	i2s.rx.dma.strm->fcr = DMA_DMDIS | DMA_FTH_FULL;

	/* configure TX DMA */
	stm32f_dmactl_init(&i2s.tx.dma, STM32F_DMA1, I2S_DMA_TX_STRM);

	i2s.tx.dma.strm->cr = DMA_CHSEL_SET(I2S_DMA_TX_CHAN) | 
		DMA_MBURST_1 | DMA_PBURST_1 | DMA_MSIZE_16 | DMA_PSIZE_16 | 
		DMA_CIRC | DMA_MINC | DMA_DIR_MTP | 
		DMA_HTIE | DMA_TCIE | DMA_TEIE;
	i2s.tx.dma.strm->par = &i2s_ext->dr;
	i2s.rx.dma.strm->m0ar = i2s.tx.buf;
	i2s.rx.dma.strm->ndtr = 2 * AUDIO_FRAME_LEN;
	i2s.tx.dma.strm->fcr = DMA_DMDIS | DMA_FTH_FULL;
#endif

	/* Configure DMA channel */
	dma->s[I2S_DMA_RX_STRM].cr = DMA_CHSEL_SET(I2S_DMA_RX_CHAN) | 
		DMA_MBURST_1 | DMA_PBURST_1 | DMA_MSIZE_16 | DMA_PSIZE_16 | 
		DMA_CT_M0AR | DMA_DBM |  DMA_CIRC | DMA_MINC | DMA_DIR_PTM | 
		DMA_TCIE | DMA_TEIE | DMA_DMEIE;
	dma->s[I2S_DMA_RX_STRM].par = &spi->dr;
	dma->s[I2S_DMA_RX_STRM].m0ar = i2s.rx.buf[0];
	dma->s[I2S_DMA_RX_STRM].m1ar = i2s.rx.buf[1];
	dma->s[I2S_DMA_RX_STRM].ndtr = AUDIO_FRAME_LEN;
	dma->s[I2S_DMA_RX_STRM].fcr = DMA_FEIE | DMA_DMDIS | DMA_FTH_FULL;

	DCC_LOG(LOG_TRACE, "5.");

	dma->s[I2S_DMA_TX_STRM].cr = DMA_CHSEL_SET(I2S_DMA_TX_CHAN) | 
		DMA_MBURST_1 | DMA_PBURST_1 | DMA_MSIZE_16 | DMA_PSIZE_16 | 
		DMA_CT_M0AR | DMA_DBM | DMA_CIRC | DMA_MINC | DMA_DIR_MTP | 
		DMA_TCIE | DMA_TEIE	| DMA_DMEIE;
	dma->s[I2S_DMA_TX_STRM].par = &i2s_ext->dr;
	dma->s[I2S_DMA_TX_STRM].m0ar = i2s.tx.buf[0];
	dma->s[I2S_DMA_TX_STRM].m1ar = i2s.tx.buf[1];
	dma->s[I2S_DMA_TX_STRM].ndtr = AUDIO_FRAME_LEN;
	dma->s[I2S_DMA_TX_STRM].fcr = DMA_FEIE | DMA_DMDIS | DMA_FTH_FULL;

	DCC_LOG(LOG_TRACE, "6.");

	/* Set DMA to medium priority */
	cm3_irq_pri_set(I2S_DMA_RX_IRQ, 0x10);
	cm3_irq_enable(I2S_DMA_RX_IRQ);

	DCC_LOG(LOG_TRACE, "7.");

	cm3_irq_pri_set(I2S_DMA_TX_IRQ, 0x10);
	cm3_irq_enable(I2S_DMA_TX_IRQ);

	DCC_LOG(LOG_TRACE, "8.");

	i2s_test_init();
}

void i2s_enable(void)
{
	struct stm32f_spi * spi = STM32F_SPI2;
	struct stm32f_spi * i2s_ext = STM32F_I2S2EXT;
	struct stm32f_dma * dma = STM32F_DMA1;

	tracef("%s...", __func__);
	DCC_LOG(LOG_TRACE, "...");

#if 0
//	cm3_irq_enable(STM32F_IRQ_SPI2);
//	spi->cr2 |= SPI_RXNEIE | SPI_TXEIE | SPI_ERRIE;
// spi->cr2 |= SPI_ERRIE;
#endif
	/* enable DMA */
	dma->s[I2S_DMA_TX_STRM].cr |= DMA_EN;	
	dma->s[I2S_DMA_RX_STRM].cr |= DMA_EN;	

	/* Enable peripherals */
	spi->i2scfgr |= SPI_I2SE;
	i2s_ext->i2scfgr |= SPI_I2SE;

}


volatile uint32_t i2s_tx_cnt = 0;
volatile uint32_t i2s_rx_cnt = 0;

void i2s_stat(void)
{
	uint32_t tx_cnt = i2s_tx_cnt;
	uint32_t rx_cnt = i2s_rx_cnt;
	i2s_rx_cnt = 0;
	i2s_tx_cnt = 0;

	printf("-- I2S status --");
	printf("  Samples: RX=%d TX=%d", rx_cnt, tx_cnt);

//	cm3_irq_pend_set(I2S_DMA_RX_IRQ);
//	cm3_irq_pend_set(I2S_DMA_TX_IRQ);
}

/* TX DMA IRQ */
void stm32f_dma1_stream4_isr(void)
{
	struct stm32f_dma * dma = STM32F_DMA1;
	int16_t * usr_frm;

	if (dma->hisr & DMA_FEIF4) {
	//	trace("DMA_FEIF4");
		DCC_LOG(LOG_TRACE, "DMA_FEIF4");
		dma->hifcr = DMA_CFEIF4;
//		dma->s[I2S_DMA_TX_STRM].cr = 0;	
//		udelay(10000);
	}

	if (dma->hisr & DMA_DMEIF4) {
	//	trace("DMA_DMEIF4");
		DCC_LOG(LOG_TRACE, "DMA_DMEIF4");
		dma->hifcr = DMA_CDMEIF4;
	//	dma->s[I2S_DMA_TX_STRM].cr = 0;	
	//	udelay(10000);
	}

	if (dma->hisr & DMA_TEIF4) {
	//	trace("DMA_TEIF4");
		DCC_LOG(LOG_TRACE, "DMA_TEIF4");
		dma->hifcr = DMA_CTEIF4;
//		dma->s[I2S_DMA_TX_STRM].cr = 0;	
//		udelay(10000);
	}

	if (dma->hisr & DMA_HTIF4) {
	//	trace("DMA_HTIF4");
		DCC_LOG(LOG_MSG, "DMA_HTIF4");
//		dma->hifcr = DMA_CHTIF4;
	}

	if ((dma->hisr & DMA_TCIF4) == 0) {
	//	trace("!= DMA_TCIF4!!!");
		DCC_LOG(LOG_TRACE, "!=DMA_TCIF4");
		return;
	}

	//trace("DMA_TCIF4");

	/* clear the DMA transfer complete flag */
	dma->hifcr = DMA_CTCIF4;
//	dma->hifcr = DMA_CHTIF4 | DMA_CTEIF4 | DMA_CDMEIF4 | DMA_CFEIF4;

	if (dma->s[I2S_DMA_TX_STRM].cr & DMA_CT) {
		usr_frm = i2s.tx.buf[1];
		dma->s[I2S_DMA_TX_STRM].m0ar = i2s.tx.buf[0];
		DCC_LOG(LOG_INFO, "1.");
	} else {
		usr_frm = i2s.tx.buf[0];
		dma->s[I2S_DMA_TX_STRM].m1ar = i2s.tx.buf[1];
		DCC_LOG(LOG_INFO, "0.");
	}
	(void)usr_frm;
//	tonegen_apply(&tonegen, usr_frm);
//	pulse_apply(usr_frm);
	pattern_apply(usr_frm);
}


/* RX DMA IRQ */
void stm32f_dma1_stream3_isr(void)
{
	struct stm32f_dma * dma = STM32F_DMA1;
	int16_t * usr_frm;

	if ((dma->lisr & DMA_HTIF3) == 0) {
		trace("DMA_HTIF3");
		DCC_LOG(LOG_TRACE, "DMA_HTIF3");
		dma->lifcr = DMA_CHTIF3;
	}

	if ((dma->lisr & DMA_DMEIF3) == 0) {
		trace("DMA_DMEIF3");
		DCC_LOG(LOG_TRACE, "DMA_DMEIF3");
		dma->lifcr = DMA_CDMEIF3;
		/* disable DMA !!! */
		dma->s[I2S_DMA_RX_STRM].cr &= ~DMA_EN;	
	}

	if ((dma->lisr & DMA_FEIF3) == 0) {
		trace("DMA_FEIF3");
		DCC_LOG(LOG_TRACE, "DMA_FEIF3");
		dma->lifcr = DMA_CFEIF3;
		/* disable DMA !!! */
		dma->s[I2S_DMA_RX_STRM].cr &= ~DMA_EN;	
	}

	if ((dma->lisr & DMA_TCIF3) == 0) {
		trace("!= DMA_TCIF3!!!");
		DCC_LOG(LOG_TRACE, "!=DMA_TCIF3");
		return;
	}

	trace("DMA_TCIF3");

	/* clear the DMA transfer complete flag */
	dma->lifcr = DMA_CTCIF3;
//	dma->hifcr = DMA_CHTIF3 | DMA_CTEIF3 | DMA_CDMEIF3 | DMA_CFEIF3;

	if (dma->s[I2S_DMA_RX_STRM].cr & DMA_CT) {
		usr_frm = i2s.rx.buf[1];
		dma->s[I2S_DMA_RX_STRM].m0ar = i2s.rx.buf[0];
		DCC_LOG(LOG_INFO, "1.");
	} else {
		usr_frm = i2s.rx.buf[0];
		dma->s[I2S_DMA_RX_STRM].m1ar = i2s.rx.buf[1];
		DCC_LOG(LOG_INFO, "0.");
	}

	(void)usr_frm;
//	tonegen_apply(&tonegen, usr_frm);
}


void stm32f_spi2_isr(void)
{
	struct stm32f_spi * spi = STM32F_SPI2;
	uint32_t sr;
	uint32_t data;

	sr = spi->sr;

	if (sr & SPI_FRE) {
		trace("FRE");
	};

	if (sr & SPI_OVR) {
		trace("OVR");
	};

	if (sr & SPI_UDR) {
		trace("UDR");
	};

	if (sr & SPI_TXE) {
//		tracef("T");
		data = 0;
		spi->dr = data;
		i2s_tx_cnt++;
	};

	if (sr & SPI_RXNE) {
//		tracef("R");
		data = spi->dr;
		(void)data;
		i2s_rx_cnt++;
	};
	
}

