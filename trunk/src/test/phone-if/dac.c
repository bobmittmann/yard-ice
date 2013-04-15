/* 
 * File:	 dig-pot.c
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
#include <sys/delay.h>
#include <stdint.h>
#include <stdio.h>

#include <sys/dcclog.h>

#include "wavetab.h"

/* ----------------------------------------------------------------------
 * DAC
 * ----------------------------------------------------------------------
 */
#define DAC1_GPIO STM32F_GPIOA, 4
#define DAC2_GPIO STM32F_GPIOA, 5

#define DAC1_DMA_CHAN 2
#define DAC2_DMA_CHAN 3

struct {
	uint16_t * wave;
	uint32_t len;
} dac_chan[2];

void dac_play(int dac)
{
	struct stm32f_dma * dma = STM32F_DMA1;

	/* enable DMA */
	if (dac)
		dma->ch[DAC2_DMA_CHAN].ccr |= DMA_EN;
	else
		dma->ch[DAC1_DMA_CHAN].ccr |= DMA_EN;
}

void dac_pause(int dac)
{
	struct stm32f_dma * dma = STM32F_DMA1;

	/* disable DMA */
	if (dac)
		dma->ch[DAC2_DMA_CHAN].ccr &= ~DMA_EN;
	else
		dma->ch[DAC1_DMA_CHAN].ccr &= ~DMA_EN;

}

unsigned int dac_wave_set(int dac, unsigned int wid)
{
	struct stm32f_dma * dma = STM32F_DMA1;

	dac_chan[dac].wave = (uint16_t *)wave_lut[wid].buf;
	dac_chan[dac].len = wave_lut[wid].len;

	/* enable DMA Interrupt */
	if (dac)
		dma->ch[DAC2_DMA_CHAN].ccr |= DMA_TCIE | DMA_EN;
	else
		dma->ch[DAC1_DMA_CHAN].ccr |= DMA_TCIE | DMA_EN;

	return wave_lut[wid].freq;
}

unsigned int wave_freq_get(unsigned int wid)
{
	return wave_lut[wid].freq;
}

static void dac_timer_init(uint32_t freq)
{
	struct stm32f_rcc * rcc = STM32F_RCC;
	struct stm32f_tim * tim = STM32F_TIM2;
	uint32_t div;
	uint32_t pre;
	uint32_t n;

	/* get the total divisior */
	div = ((stm32f_tim1_hz) + (freq / 2)) / freq;
	/* get the minimum pre scaler */
	pre = (div / 65536) + 1;
	/* get the reload register value */
	n = (div * 2 + pre) / (2 * pre);

	DCC_LOG1(LOG_TRACE, "stm32f_tim1_hz=%dHz", stm32f_tim1_hz);
	DCC_LOG3(LOG_TRACE, "freq=%dHz pre=%d n=%d", freq, pre, n);
	DCC_LOG1(LOG_TRACE, "real freq=%dHz\n", stm32f_tim1_hz / pre / n);

	/* Timer clock enable */
	rcc->apb1enr |= RCC_TIM2EN;
	
	/* Timer configuration */
	tim->psc = pre - 1;
	tim->arr = n - 1;
	tim->cnt = 0;
	tim->egr = 0;
	tim->dier = TIM_UIE; /* Update interrupt enable */
	tim->ccmr1 = TIM_OC1M_PWM_MODE1;
	tim->ccr1 = tim->arr / 2;
	tim->cr2 = TIM_MMS_OC1REF;
}

void dac_start(void)
{
	struct stm32f_tim * tim = STM32F_TIM2;

	tim->cr1 = TIM_URS | TIM_CEN; /* Enable counter */
}

void dac_stop(void)
{
	struct stm32f_tim * tim = STM32F_TIM2;

	tim->cr1 = 0;
}

void dac_init(void)
{
	struct stm32f_rcc * rcc = STM32F_RCC;
	struct stm32f_dac * dac = STM32F_DAC;
	struct stm32f_dma * dma = STM32F_DMA1;

	/* I/O pins config */
	stm32f_gpio_mode(DAC2_GPIO, ANALOG, 0);
	stm32f_gpio_mode(DAC1_GPIO, ANALOG, 0);

	/* DAC clock enable */
	rcc->apb1enr |= RCC_DACEN;
	/* DAC disable */
	dac->cr = 0;

	/* DMA clock enable */
	rcc->ahbenr |= RCC_DMA1EN;

	/* DMA Disable */
	dma->ch[DAC1_DMA_CHAN].ccr = 0;
	/* Wait for the channel to be ready .. */
	while (dma->ch[DAC1_DMA_CHAN].ccr & DMA_EN);

	/* DMA Disable */
	dma->ch[DAC2_DMA_CHAN].ccr = 0;
	/* Wait for the channel to be ready .. */
	while (dma->ch[DAC2_DMA_CHAN].ccr & DMA_EN);

	/* DAC configure */
	dac->cr = DAC_EN2 | DAC_TSEL2_TIMER2 | DAC_TEN2 | DAC_DMAEN2 |
			  DAC_EN1 | DAC_TSEL1_TIMER2 | DAC_TEN1 | DAC_DMAEN1;

	/* DAC channel 2 initial value */
	dac->dhr12r2 = 2482;
	/* DAC channel 1 initial value */
	dac->dhr12r1 = 2482;

	/*  DMA Configuration */
	/* Peripheral address */
	dma->ch[DAC1_DMA_CHAN].cpar = &dac->dhr12r1;
	/* Memory pointer */
	dma->ch[DAC1_DMA_CHAN].cmar = (void *)wave_lut[0].buf;
	/* Number of data items to transfer */
	dma->ch[DAC1_DMA_CHAN].cndtr = wave_lut[0].len;
	/* Configuration single buffer circular */
	dma->ch[DAC1_DMA_CHAN].ccr = DMA_MSIZE_16 | DMA_PSIZE_16 | DMA_MINC |
		DMA_CIRC | DMA_DIR_MTP | DMA_EN;

	/*  DMA Configuration */
	/* Peripheral address */
	dma->ch[DAC2_DMA_CHAN].cpar = &dac->dhr12r2;
	/* Memory pointer */
	dma->ch[DAC2_DMA_CHAN].cmar = (void *)wave_lut[0].buf;
	/* Number of data items to transfer */
	dma->ch[DAC2_DMA_CHAN].cndtr = wave_lut[0].len;
	/* Configuration single buffer circular */
	dma->ch[DAC2_DMA_CHAN].ccr = DMA_MSIZE_16 | DMA_PSIZE_16 | DMA_MINC |
		DMA_CIRC | DMA_DIR_MTP | DMA_EN;

	dac_timer_init(SAMPLE_RATE);

	/* Set DMA IRQ priority */
	cm3_irq_pri_set(STM32F_IRQ_DMA1_STREAM2, 0x10);
	/* Enable DMA interrupt */
	cm3_irq_enable(STM32F_IRQ_DMA1_STREAM2);

	/* Set DMA IRQ priority */
	cm3_irq_pri_set(STM32F_IRQ_DMA1_STREAM3, 0x10);
	/* Enable DMA interrupt */
	cm3_irq_enable(STM32F_IRQ_DMA1_STREAM3);
}

void stm32f_dma1_stream2_isr(void)
{
	struct stm32f_dma * dma = STM32F_DMA1;
	struct stm32f_dma_channel * ch = &dma->ch[DAC1_DMA_CHAN];
	uint32_t ccr;


	if (dma->isr & DMA_TCIF3) {
		/* clear the DMA transfer complete flag */
		dma->ifcr = DMA_CTCIF3;

		/* disable DMA and Interrupts */
		ch->ccr &= ~(DMA_EN | DMA_TCIE);
		/* Wait for the channel to be ready .. */
		while ((ccr = ch->ccr) & DMA_EN);
		/* Memory address */
		ch->cmar = dac_chan[0].wave;
		/* Number of data items to transfer */
		ch->cndtr = dac_chan[0].len;
		/* enable interrupt */
		ch->ccr = ccr | DMA_EN;
	}
}

void stm32f_dma1_stream3_isr(void)
{
	struct stm32f_dma * dma = STM32F_DMA1;
	struct stm32f_dma_channel * ch = &dma->ch[DAC2_DMA_CHAN];
	uint32_t ccr;

	if (dma->isr & DMA_TCIF4) {
		/* clear the DMA transfer complete flag */
		dma->ifcr = DMA_CTCIF4;

		/* disable DMA and Interrupts */
		ch->ccr &= ~(DMA_EN | DMA_TCIE);
		/* Wait for the channel to be ready .. */
		while ((ccr = ch->ccr) & DMA_EN);
		/* Memory address */
		ch->cmar = dac_chan[1].wave;
		/* Number of data items to transfer */
		ch->cndtr = dac_chan[1].len;
		/* enable interrupt */
		ch->ccr = ccr | DMA_EN;
	}
}

