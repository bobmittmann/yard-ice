/* 
 * File:	 adc.c
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
#include <stdint.h>

#include <sys/dcclog.h>

#include "adc.h"

/* ----------------------------------------------------------------------
 * ADC
 * ----------------------------------------------------------------------
 */

#define ADC_IN0 STM32F_GPIOA, 0
#define ADC_IN1 STM32F_GPIOA, 1
#define ADC_IN2 STM32F_GPIOA, 2
#define ADC_IN3 STM32F_GPIOA, 3
#define ADC_IN6 STM32F_GPIOA, 6

static void adc_gpio_init(void)
{
	/* ADC Input pins */
	stm32f_gpio_mode(ADC_IN0, ANALOG, 0);
	stm32f_gpio_mode(ADC_IN1, ANALOG, 0);
	stm32f_gpio_mode(ADC_IN2, ANALOG, 0);
	stm32f_gpio_mode(ADC_IN3, ANALOG, 0);
	stm32f_gpio_mode(ADC_IN6, ANALOG, 0);
}

#define ADC_DMA_CHAN 0

static void adc_dma_init(void * dst, void * src, 
						 unsigned int ndt)
{
	struct stm32f_rcc * rcc = STM32F_RCC;
	struct stm32f_dma * dma = STM32F_DMA1;

	/* DMA clock enable */
	rcc->ahbenr |= RCC_DMA1EN;

	/* Disable DMA channel */
	dma->ch[ADC_DMA_CHAN].ccr = 0;
	while (dma->ch[ADC_DMA_CHAN].ccr & DMA_EN); 
	/* Wait for the channel to be ready .. */

	/* peripheral address */
	dma->ch[ADC_DMA_CHAN].cpar = src;
	/* DMA address */
	dma->ch[ADC_DMA_CHAN].cmar = dst;
	/* Number of data items to transfer */
	dma->ch[ADC_DMA_CHAN].cndtr = ndt;
	/* Configuration for double buffer circular, 
	   half-transfer interrupt  */
	dma->ch[ADC_DMA_CHAN].ccr = DMA_MSIZE_16 | DMA_PSIZE_16 | 
		DMA_MINC | DMA_CIRC | DMA_DIR_PTM;
	/* enable DMA */
	dma->ch[ADC_DMA_CHAN].ccr |= DMA_EN | DMA_HTIE | DMA_TCIE | DMA_TEIE;
}

static void adc_timer_init(uint32_t freq)
{
	struct stm32f_rcc * rcc = STM32F_RCC;
	struct stm32f_tim * tim = STM32F_TIM3;
	uint32_t div;
	uint32_t pre;
	uint32_t n;

	/* get the total divisior */
	div = (stm32f_tim1_hz + (freq / 2)) / freq;
	/* get the minimum pre scaler */
	pre = (div / 65536) + 1;
	/* get the reload register value */
	n = (div + pre / 2) / pre;

	DCC_LOG3(LOG_TRACE, "freq=%dHz pre=%d n=%d", freq, pre, n);

	/* Timer clock enable */
	rcc->apb1enr |= RCC_TIM3EN;

	/* ensure timer is disabled */
	tim->cr1 = 0; 

	/* Timer configuration */
	tim->psc = pre - 1;
	tim->arr = n - 1;
	tim->cnt = 0;
	tim->egr = 0;
	tim->dier = 0;
	tim->ccmr1 = TIM_OC1M_PWM_MODE1;
	tim->ccr1 = tim->arr / 2;
	tim->cr2 = TIM_MMS_OC1REF;
}

void adc_stop(void) 
{
	struct stm32f_tim * tim = STM32F_TIM3;

	tim->cr1 = 0; /* Disable counter */
}

void adc_start(void) 
{
	struct stm32f_tim * tim = STM32F_TIM3;

	tim->cr1 = TIM_URS | TIM_CEN; /* Enable counter */
}

struct adc_drv {
	uint16_t buf[2][ADC_CHANS];
	uint32_t dma_cnt;
	uint32_t avg[ADC_CHANS];
	uint16_t * val;
} adc_drv;

/***********************************************************
  ADC Configuration
 ***********************************************************/
void adc_init(uint16_t buf[])
{
	struct stm32f_rcc * rcc = STM32F_RCC;
	struct stm32f_adc * adc = STM32F_ADC1;
//	const uint8_t adc_chan_seq[] = {0, 1, 2, 3, 6};
//	const uint8_t adc_chan_seq[] = {6, 0, 1, 2, 3};
	const uint8_t adc_chan_seq[] = {1, 2, 3, 6, 0};

	/* Initialize runtime driver strcucture */
	adc_drv.dma_cnt = 0;
	adc_drv.val = buf;

	/* Configure timer */
	adc_timer_init(ADC_RATE);

	/* ADC clock enable */
	rcc->apb2enr |= RCC_ADC1EN;

	adc->cr1 = 0;
	adc->cr2 = ADC_RSTCAL | ADC_ADON;

	/* calibrate */
	adc->cr2 = ADC_CAL | ADC_ADON;
	while (adc->cr2 & ADC_CAL);

	/* configure for DMA use, select timer3 trigger */
	adc->cr1 = ADC_SCAN;
	adc->cr2 = ADC_TSVREFE | ADC_EXTTRIG | 
		ADC_EXTSEL_TIM3_TRGO | ADC_ADON | ADC_DMA;

	/* set the scan sequence */
	stm32f_adc_seq_set(adc, adc_chan_seq, ADC_CHANS);

	/* set the sample time */
	stm32f_adc_smp_set(adc, 0, ADC_SMP_41_CYC);
	stm32f_adc_smp_set(adc, 1, ADC_SMP_41_CYC);
	stm32f_adc_smp_set(adc, 2, ADC_SMP_41_CYC);
	stm32f_adc_smp_set(adc, 3, ADC_SMP_41_CYC);
	stm32f_adc_smp_set(adc, 6, ADC_SMP_41_CYC);

	adc_gpio_init();

	adc_dma_init(adc_drv.buf[0], (void *)&adc->dr, 2 * ADC_CHANS);

	/* Set DMA IRQ to low priority */
	cm3_irq_pri_set(STM32F_IRQ_DMA1_STREAM0, 0xc0);
	/* Enable DMA interrupt */
	cm3_irq_enable(STM32F_IRQ_DMA1_STREAM0);

}

void stm32f_dma1_stream0_isr(void)
{
	struct stm32f_dma * dma = STM32F_DMA1;
	uint16_t * data;
	uint32_t cnt;
	int i;

	if (dma->isr & DMA_TEIF1) {
		DCC_LOG(LOG_TRACE, "DMA_TEIF1");
		dma->ifcr = DMA_CTEIF1;
		return;
	}

	if (dma->isr & DMA_HTIF1) {
		/* clear the DMA half transfer flag */
		dma->ifcr = DMA_CHTIF1;
		data = adc_drv.buf[0];
	} 

	if (dma->isr & DMA_TCIF1) {
		/* clear the DMA transfer complete flag */
		dma->ifcr = DMA_CTCIF1;
		data = adc_drv.buf[1];
	}

	/* avg = (avg * 15 + x) / 16 */
	/* a16 = avg*16 = (avg * 15 + x) */
	/* a16 = (a16 * 15 / 16) + x) */

	#define ADC_AVG_N 16

	/* Filter */
	for (i = 0; i < ADC_CHANS; ++i) {
		adc_drv.avg[i] = (adc_drv.avg[i] * (ADC_AVG_N - 1) / 
						  ADC_AVG_N) + data[i];
	}

	cnt = adc_drv.dma_cnt + 1;

	if ((cnt & (ADC_AVG_N - 1)) == 0) {
		for (i = 0; i < ADC_CHANS; ++i)
			adc_drv.val[i] = adc_drv.avg[i] / ADC_AVG_N;
	}

	adc_drv.dma_cnt = cnt;
}

