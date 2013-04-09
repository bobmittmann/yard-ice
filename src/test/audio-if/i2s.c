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

struct tonegen tonegen;

void i2s_test_init(void)
{
	tonegen_init(&tonegen, 16384, WAVE_A3);
}


#define I2S2_WS     STM32F_GPIOB, 12
#define I2S2_CK     STM32F_GPIOB, 13
#define I2S2EXT_SD  STM32F_GPIOB, 14
#define I2S2_SD     STM32F_GPIOB, 15

#define I2S_DMA_RX_STRM 3
#define I2S_DMA_RX_CHAN 0
#define I2S_DMA_RX_IRQ STM32F_IRQ_DMA1_STREAM3

#define I2S_DMA_TX_STRM 4
#define I2S_DMA_TX_CHAN 2
//#define I2S_DMA_TX_CHAN 0
#define I2S_DMA_TX_IRQ STM32F_IRQ_DMA1_STREAM4


struct {
	struct {
		struct stm32f_dmactl dma;
		int16_t buf[2][AUDIO_FRAME_LEN];
	} tx;
	struct {
		struct stm32f_dmactl dma;
		int16_t buf[2][AUDIO_FRAME_LEN];
	} rx;
} i2s;

void i2s_slave_init(void)
{
	struct stm32f_spi * spi = STM32F_SPI2;
	struct stm32f_spi * i2s_ext = STM32F_I2S2EXT;
	struct stm32f_rcc * rcc = STM32F_RCC;
	struct stm32f_dma * dma = STM32F_DMA1;

	fprintf(stderr, "%s...\n", __func__);

	fprintf(stderr, "%s SPI=0x%08x\n", __func__, (uint32_t)spi);
	fprintf(stderr, "%s I2S_EXT=0X%08x\n", __func__, (uint32_t)i2s_ext);

	stm32f_gpio_mode(I2S2_WS, ALT_FUNC, SPEED_MED);
	stm32f_gpio_mode(I2S2_CK, ALT_FUNC, SPEED_MED);
	stm32f_gpio_mode(I2S2EXT_SD, ALT_FUNC, PUSH_PULL | SPEED_MED);
	stm32f_gpio_mode(I2S2_SD, ALT_FUNC, SPEED_MED);

	stm32f_gpio_af(I2S2_WS, GPIO_AF5);
	stm32f_gpio_af(I2S2_CK, GPIO_AF5);
	stm32f_gpio_af(I2S2EXT_SD, GPIO_AF6);
	stm32f_gpio_af(I2S2_SD, GPIO_AF5);

	/* Enable SPI clock */
	rcc->apb1enr |= RCC_SPI2EN;

	spi->cr1 = 0;
	spi->cr2 = 0;
	spi->i2scfgr = SPI_I2SMOD | SPI_I2SCFG_SLV_RCV | 
		SPI_PCMSYNC_SHORT | SPI_I2SSTD_PCM | SPI_DATLEN_16 |
		SPI_CHLEN_16;
	spi->i2spr = 0;
	spi->cr2 = SPI_RXDMAEN;

	i2s_ext->cr1 = 0;
	i2s_ext->cr2 = 0;
	i2s_ext->i2scfgr = SPI_I2SMOD | SPI_I2SCFG_SLV_XMT | 
		SPI_PCMSYNC_SHORT | SPI_I2SSTD_PCM | SPI_DATLEN_16 |
		SPI_CHLEN_16;
	i2s_ext->i2spr = 0;
	i2s_ext->cr2 = SPI_TXDMAEN;


	/* DMA clock enable */
	rcc->ahb1enr |= RCC_DMA1EN;

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

	/* Disable DMA channel */
	dma->s[I2S_DMA_RX_STRM].cr = 0;
	while (dma->s[I2S_DMA_RX_STRM].cr & DMA_EN); 

	dma->s[I2S_DMA_RX_STRM].cr = DMA_CHSEL_SET(I2S_DMA_RX_CHAN) | 
		DMA_MBURST_1 | DMA_PBURST_1 | DMA_MSIZE_16 | DMA_PSIZE_16 | 
		DMA_CT_M0AR | DMA_DBM |  DMA_CIRC | DMA_MINC | DMA_DIR_PTM | 
		DMA_TCIE | DMA_TEIE;
	dma->s[I2S_DMA_RX_STRM].par = &spi->dr;
	dma->s[I2S_DMA_RX_STRM].m0ar = i2s.rx.buf[0];
	dma->s[I2S_DMA_RX_STRM].m1ar = i2s.rx.buf[1];
	dma->s[I2S_DMA_RX_STRM].ndtr = AUDIO_FRAME_LEN;
	dma->s[I2S_DMA_RX_STRM].fcr = DMA_DMDIS | DMA_FTH_FULL;

	dma->s[I2S_DMA_TX_STRM].cr = 0;
	while (dma->s[I2S_DMA_TX_STRM].cr & DMA_EN); 

	dma->s[I2S_DMA_TX_STRM].cr = DMA_CHSEL_SET(I2S_DMA_TX_CHAN) | 
		DMA_MBURST_1 | DMA_PBURST_1 | DMA_MSIZE_16 | DMA_PSIZE_16 | 
		DMA_CT_M0AR | DMA_DBM |  DMA_CIRC | DMA_MINC | DMA_DIR_MTP | 
		DMA_TCIE | DMA_TEIE;
	dma->s[I2S_DMA_TX_STRM].par = &i2s_ext->dr;
	dma->s[I2S_DMA_TX_STRM].m0ar = i2s.tx.buf[0];
	dma->s[I2S_DMA_TX_STRM].m1ar = i2s.tx.buf[1];
	dma->s[I2S_DMA_TX_STRM].ndtr = AUDIO_FRAME_LEN;
	dma->s[I2S_DMA_TX_STRM].fcr = DMA_DMDIS | DMA_FTH_FULL;

	/* Set DMA to medium priority */
	cm3_irq_pri_set(I2S_DMA_RX_IRQ, 0x40);
	cm3_irq_enable(I2S_DMA_RX_IRQ);

	cm3_irq_pri_set(I2S_DMA_TX_IRQ, 0x40);
	cm3_irq_enable(I2S_DMA_TX_IRQ);

	/* enable DMA */
	dma->s[I2S_DMA_TX_STRM].cr |= DMA_EN;	
	dma->s[I2S_DMA_RX_STRM].cr |= DMA_EN;	


	i2s_test_init();
}

void i2s_enable(void)
{
	struct stm32f_spi * spi = STM32F_SPI2;
	struct stm32f_spi * i2s_ext = STM32F_I2S2EXT;

	fprintf(stderr, "%s...\n", __func__);

	cm3_irq_enable(STM32F_IRQ_SPI2);

	/* Enable interrupts */
//	spi->cr2 |= SPI_RXNEIE | SPI_TXEIE | SPI_ERRIE;
// spi->cr2 |= SPI_ERRIE;

	/* Enable peripheral */
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
	printf("-- I2S status --\n");
	printf("  Samples: RX=%d TX=%d\n", rx_cnt, tx_cnt);

//	cm3_irq_pend_set(I2S_DMA_RX_IRQ);
//	cm3_irq_pend_set(I2S_DMA_TX_IRQ);
}

/* RX DMA IRQ */
void stm32f_dma1_stream3_isr(void)
{
	struct stm32f_dma * dma = STM32F_DMA1;

	if ((dma->lisr & DMA_TCIF3) == 0)
		return;

	/* clear the DMA transfer complete flag */
	dma->lifcr = DMA_CTCIF3;

	/* clear the the DMA trasfer complete flag */
//	i2s.tx.dma.ifcr[TCIF_BIT] = 1;
//	printf("[R]");
}

/* TX DMA IRQ */
void stm32f_dma1_stream4_isr(void)
{
	struct stm32f_dma * dma = STM32F_DMA1;
	int16_t * usr_frm;

	if ((dma->hisr & DMA_TCIF4) == 0)
		return;

	/* clear the DMA transfer complete flag */
	dma->hifcr = DMA_CTCIF4;

	if (dma->s[I2S_DMA_TX_STRM].cr & DMA_CT) {
		usr_frm = i2s.tx.buf[1];
		dma->s[I2S_DMA_TX_STRM].m0ar = i2s.tx.buf[0];
		DCC_LOG(LOG_INFO, "1.");
	} else {
		usr_frm = i2s.tx.buf[0];
		dma->s[I2S_DMA_TX_STRM].m1ar = i2s.tx.buf[1];
		DCC_LOG(LOG_INFO, "0.");
	}

	tonegen_apply(&tonegen, usr_frm);
}

void stm32f_spi2_isr(void)
{
	struct stm32f_spi * spi = STM32F_SPI2;
	uint32_t sr;
	uint32_t data;

	sr = spi->sr;

	if (sr & SPI_FRE) {
		fprintf(stderr, "FRE");
	};

	if (sr & SPI_OVR) {
		fprintf(stderr, "OVR");
	};

	if (sr & SPI_UDR) {
		fprintf(stderr, "UDR");
	};

	if (sr & SPI_TXE) {
//		fprintf(stderr, "T");
		data = 0;
		spi->dr = data;
		i2s_tx_cnt++;
	};

	if (sr & SPI_RXNE) {
//		fprintf(stderr, "R");
		data = spi->dr;
		(void)data;
		i2s_rx_cnt++;
	};
	
}

