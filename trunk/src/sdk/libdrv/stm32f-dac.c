/* 
 * File:	 dac-test.c
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
#include <stdio.h>

#include <sys/dcclog.h>

#include "wavetab.h"

#define DAC1_GPIO STM32F_GPIOA
#define DAC1_PORT 4

#define DAC2_GPIO STM32F_GPIOA
#define DAC2_PORT 5

#define DAC1_DMA 5
#define DAC1_DMA_CHAN 7

void wave_set(uint8_t * wave, unsigned int len)
{
	struct stm32f_dma * dma = STM32F_DMA1;

	/* disable DMA */
	dma->s[DAC1_DMA].cr &= ~DMA_EN;	
	/* Wait for the channel to be ready .. */
	while (dma->s[DAC1_DMA].cr & DMA_EN); 
	/* Memory address */
	dma->s[DAC1_DMA].m0ar = wave;
	/* Number of data items to transfer */
	dma->s[DAC1_DMA].ndtr = len;
}

void wave_play(void)
{
	struct stm32f_dma * dma = STM32F_DMA1;
	/* enable DMA */
	dma->s[DAC1_DMA].cr |= DMA_EN;	
}

void wave_pause(void)
{
	struct stm32f_dma * dma = STM32F_DMA1;
	/* disable DMA */
	dma->s[DAC1_DMA].cr &= ~DMA_EN;	
}

void vout_set(unsigned int mv)
{
	struct stm32f_dac * dac = STM32F_DAC;

	dac->dhr12r2 = (5160 * mv) / 8192;
}

void stm32f_dac_init(void)
{
	struct stm32f_rcc * rcc = STM32F_RCC;
	struct stm32f_dac * dac = STM32F_DAC;
	struct stm32f_tim * tim2 = STM32F_TIM2;
	struct stm32f_dma * dma = STM32F_DMA1;

	/* I/O pins config */
	stm32f_gpio_clock_en(DAC2_GPIO);
	stm32f_gpio_mode(DAC2_GPIO, DAC2_PORT, ANALOG, 0);
	stm32f_gpio_mode(DAC1_GPIO, DAC1_PORT, ANALOG, 0);

	/* DAC clock enable */
	rcc->apb1enr |= RCC_DACEN;
	/* DAC disable */
	dac->cr = 0;

	/* Timer clock enable */
	rcc->apb1enr |= RCC_TIM2EN;
	/* Timer disable */
	tim2->cr1 = TIM_URS | TIM_CEN;

	/* DMA clock enable */
	rcc->ahb1enr |= RCC_DMA1EN;
	/* DMA Disable */
	dma->s[DAC1_DMA].cr = 0;
	/* Wait for the channel to be ready .. */
	while (dma->s[DAC1_DMA].cr & DMA_EN); 

	/* DAC configure */
	dac->cr = DAC_EN2 | DAC_EN1 | DAC_TSEL1_TIMER2 | DAC_TEN1 | DAC_DMAEN1;
	/* DAC channel 2 initial value */
	dac->dhr12r2 = 2048;

	/* Timer clock enable */
	rcc->apb1enr |= RCC_TIM2EN;
	tim2->psc = 15 - 1; /* 2 * APB1_CLK(30MHz) / 15 = 4MHz */
	tim2->arr = (4000000 / SAMPLE_RATE) - 1; /* 4MHz / 500 = 8000 Hz*/
	tim2->cnt = 0;
	tim2->egr = 0; /* Update generation */
	tim2->dier = TIM_UIE; /* Update interrupt enable */
	tim2->cr2 = TIM_MMS_OC1REF;
	tim2->ccmr1 = TIM_OC1M_PWM_MODE1;
	tim2->ccr1 = tim2->arr - 2;
	/* enable timer */
	tim2->cr1 = TIM_URS | TIM_CEN;

	/*  DMA Configuration */
	/* Peripheral address */
	dma->s[DAC1_DMA].par = &dac->dhr8r1;
	/* Memory address */
	dma->s[DAC1_DMA].m0ar = (void *)sin1khz;
	/* Number of data items to transfer */
	dma->s[DAC1_DMA].ndtr = sizeof(sin1khz);
	/* Configuration single buffer circular */
	dma->s[DAC1_DMA].cr = DMA_CHSEL_SET(DAC1_DMA_CHAN) | 
		DMA_MBURST_1 | DMA_PBURST_1 | 
		DMA_MSIZE_8 | DMA_PSIZE_8 | DMA_MINC | 
		DMA_CIRC | DMA_DIR_MTP;
}

struct file * uart_console_open(unsigned int baudrate, unsigned int flags);
