/* 
 * Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
 *
 * This file is part of the YARD-ICE.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You can receive a copy of the GNU Lesser General Public License from 
 * http://www.gnu.org/
 */

/** 
 * @file stm32f-adc.c
 * @brief 
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 


#include <stdint.h>

#include <sys/stm32f.h>
#include <arch/cortex-m3.h>

#define ADC6_GPIO STM32_GPIOA
#define ADC6_PORT 6

#ifdef STM32F2X

/***********************************************************
  DMA Configuration
 ***********************************************************/
static void adc_dma2_init(void * dst0, void * dst1, void * src, unsigned int ndt)
{
	struct stm32_rcc * rcc = STM32_RCC;
	struct stm32f_dma * dma = STM32F_DMA2;

	/* DMA clock enable */
	rcc->ahb1enr |= RCC_DMA2EN;

	/* Disable DMA channel */
	dma->s[0].cr = 0;
	while (dma->s[0].cr & DMA_EN); /* Wait for the channel to be ready .. */

	/* peripheral address */
	dma->s[0].par = src;
	/* DMA address */
	dma->s[0].m0ar = dst0;
	dma->s[0].m1ar = dst1;
	/* Number of data items to transfer */
	dma->s[0].ndtr = ndt;
	/* Configuration for double buffer circular */
	dma->s[0].cr = DMA_CHSEL_SET(0) | DMA_MBURST_1 | DMA_PBURST_1 | 
		DMA_CT_M0AR | DMA_DBM |  DMA_MSIZE_16 | DMA_PSIZE_16 | DMA_MINC | 
		DMA_CIRC | DMA_DIR_PTM | DMA_TCIE;
	/* enable DMA */
	dma->s[0].cr |= DMA_EN;	
}

/***********************************************************
  Timer Configuration
 ***********************************************************/
static void adc_tim2_init(uint32_t freq)
{
	struct stm32_rcc * rcc = STM32_RCC;
	struct stm32f_tim * tim2 = STM32F_TIM2;
	uint32_t div;
	uint32_t pre;
	uint32_t n;

	/* get the total divisior */
	div = ((2 * stm32f_apb1_hz) + (freq / 2)) / freq;
	/* get the minimum pre scaler */
	pre = (div / 65536) + 1;
	/* get the reload register value */
	n = (div + pre / 2) / pre;

	/* Timer clock enable */
	rcc->apb1enr |= RCC_TIM2EN;
	
	/* Timer configuration */
	tim2->psc = pre - 1;
	tim2->arr = n - 1;
	tim2->cnt = 0;
	tim2->egr = 0;
	tim2->dier = TIM_UIE; /* Update interrupt enable */
	tim2->ccmr1 = TIM_OC1M_PWM_MODE1;
	tim2->ccr1 = tim2->arr - 2;
	tim2->cr2 = TIM_MMS_OC1REF;
	tim2->cr1 = TIM_URS | TIM_CEN; /* Enable counter */
}

/***********************************************************
  I/O pin configuration
 ***********************************************************/
static void adc_gpio_init(void)
{
	/* ADC Input pins */
	stm32_gpio_clock_en(ADC6_GPIO);
	stm32_gpio_mode(ADC6_GPIO, ADC6_PORT, ANALOG, 0);
}

#define ADC_CHANS 3
#define ADC_RATE 10

static uint16_t adc_buf[2][ADC_CHANS];
static uint32_t adc_dma_cnt;

static int16_t adc_vin;
static int16_t adc_vbat;
static int16_t adc_temp;

#if (ENABLE_ADC_SYNC)
static int adc_dma_sync;
#endif

/***********************************************************
  ADC Configuration
 ***********************************************************/
void stm32f_adc_init(void)
{
	struct stm32_rcc * rcc = STM32_RCC;
	struct stm32f_adc * adc = STM32F_ADC1;
	const uint8_t adc_chan_seq[] = {6, 18, 6};

#ifdef	STM32F_ADCC
	struct stm32f_adcc * adcc = STM32F_ADCC;
	/* Common Control */
	adcc->ccr = ADC_TSVREFE | ADC_VBATE | ADC_ADCPRE_4;
	/* PCLK2 = 60MHz
	   ADCCLK = PCLK2/4 = 15MHz */
#endif

	/* ADC clock enable */
	rcc->apb2enr |= RCC_ADC1EN;

	/* configure for DMA use, select timer2 trigger */
	adc->cr1 = ADC_RES_12BIT | ADC_SCAN;
	adc->cr2 = ADC_EXTEN_RISING | ADC_EXTSEL_TIM2_TRGO | ADC_ADON |
			   ADC_DDS | ADC_DMA;
	/* Chan 6 is external
	   Chan 18 is the battery (VBAT)
	   Chan 16 is the internal temperature sensor */
	stm32f_adc_seq_set(adc, adc_chan_seq, 3);
	/* set the sample time */
	stm32f_adc_smp_set(adc, 6, ADC_SMP_56_CYC);
	stm32f_adc_smp_set(adc, 18, ADC_SMP_56_CYC);
	stm32f_adc_smp_set(adc, 16, ADC_SMP_56_CYC);

	adc_gpio_init();

	adc_dma2_init(adc_buf[0], adc_buf[1], (void *)&adc->dr, ADC_CHANS);

#if (ENABLE_ADC_SYNC)
	/* synchronization event */
	adc_dma_sync = thinkos_flag_alloc(); 
#endif
	
	/* Set DMA to very low priority */
	cm3_irq_pri_set(STM32F_IRQ_DMA2_STREAM0, 0xf0);
	/* Enable DMA interrupt */
	cm3_irq_enable(STM32F_IRQ_DMA2_STREAM0);

	/* Configure timer and start periodic conversion */
	adc_tim2_init(ADC_RATE);
}

#define ADC_INPUT_6_SCALE 6600
#define ADC_VBAT_SCALE 6600
#define ADC_TEMP_SENS_SCALE 3300

void stm32f_dma2_stream0_isr(void)
{
	struct stm32f_dma * dma = STM32F_DMA2;
	uint16_t * data;

	if ((dma->lisr & DMA_TCIF0) == 0)
		return;

	/* clear the DMA transfer complete flag */
	dma->lifcr = DMA_CTCIF0;

	/* get a pointer to the last filled DMA transfer buffer */
	data = adc_buf[adc_dma_cnt++ & 1];

	/* scale and sotore the samples */
	adc_vin = (data[0] * ADC_INPUT_6_SCALE) / 4096;
	adc_vbat = (data[1] * ADC_INPUT_6_SCALE) / 4096;
	adc_temp = (data[2] * ADC_TEMP_SENS_SCALE) / 4096;

#if (ENABLE_ADC_SYNC)
	__thinkos_flag_signal(adc_dma_sync);
#endif
}

#define VT25  760
#define AVG_SLOPE 2500

int32_t supv_temperature_get(void)
{
	return (((adc_temp - VT25) * 1000) / AVG_SLOPE) + 25;
}

int32_t supv_vin_get(void)
{
	return adc_vin;
}

int32_t supv_vbat_get(void)
{
	return adc_vbat;
}

#if (ENABLE_ADC_SYNC)
void supv_sync(void)
{
	thinkos_flag_clr(adc_dma_sync);
	thinkos_flag_wait(adc_dma_sync);
}
#endif

#endif

