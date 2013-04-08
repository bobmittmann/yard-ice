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

static void dac_dma_set(int dac, uint16_t * wave, unsigned int len)
{
	struct stm32f_dma * dma = STM32F_DMA1;
	struct stm32f_dma_channel * ch;

	if (dac)
		ch = &dma->ch[DAC2_DMA_CHAN];
	else
		ch = &dma->ch[DAC1_DMA_CHAN];

	/* disable DMA */
	ch->ccr &= ~DMA_EN;
	/* Wait for the channel to be ready .. */
	while (ch->ccr & DMA_EN);
	/* Memory address */
	ch->cmar = wave;
	/* Number of data items to transfer */
	ch->cndtr = len;
}

void dac_wave_set(int dac, unsigned int wid)
{
	uint16_t * wave;
	unsigned int len;

	wave = (uint16_t *)wave_lut[wid].buf;
	len = wave_lut[wid].len;

	dac_dma_set(dac, wave, len);
}

static void dac_timer_init(uint32_t freq)
{
	struct stm32f_rcc * rcc = STM32F_RCC;
	struct stm32f_tim * tim = STM32F_TIM2;
	uint32_t div;
	uint32_t pre;
	uint32_t n;

	/* get the total divisior */
	div = ((2 * stm32f_apb1_hz) + (freq / 2)) / freq;
	/* get the minimum pre scaler */
	pre = (div / 65536) + 1;
	/* get the reload register value */
	n = (div + pre / 2) / pre;

	DCC_LOG3(LOG_TRACE, "freq=%dHz pre=%d n=%d", freq, pre, n);

	/* Timer clock enable */
	rcc->apb1enr |= RCC_TIM2EN;
	
	/* Timer configuration */
	tim->psc = pre - 1;
	tim->arr = n - 1;
	tim->cnt = 0;
	tim->egr = 0;
	tim->dier = TIM_UIE; /* Update interrupt enable */
	tim->ccmr1 = TIM_OC1M_PWM_MODE1;
	tim->ccr1 = tim->arr - 2;
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
	/* Number of data items to transfer */
	dma->ch[DAC1_DMA_CHAN].cndtr = 0;
	/* Configuration single buffer circular */
	dma->ch[DAC1_DMA_CHAN].ccr = DMA_MSIZE_16 | DMA_PSIZE_16 | DMA_MINC |
		DMA_CIRC | DMA_DIR_MTP;

	/*  DMA Configuration */
	/* Peripheral address */
	dma->ch[DAC2_DMA_CHAN].cpar = &dac->dhr12r2;
	/* Number of data items to transfer */
	dma->ch[DAC2_DMA_CHAN].cndtr = 0;
	/* Configuration single buffer circular */
	dma->ch[DAC2_DMA_CHAN].ccr = DMA_MSIZE_16 | DMA_PSIZE_16 | DMA_MINC |
		DMA_CIRC | DMA_DIR_MTP;

	dac_timer_init(SAMPLE_RATE);
}

