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
 * @file hello.c
 * @brief application test
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <thinkos.h>
#include <sys/console.h>
#include <sys/stm32f.h>

#include "board.h"

void stdio_init(void)
{
	FILE * f;

	f = console_fopen();
	/* initialize STDIO */
	stderr = f;
	stdout = f;
	stdin = f;
}


void io_init(void)
{
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOG);
	stm32_gpio_mode(LED1, OUTPUT, OPEN_DRAIN | SPEED_MED);
	stm32_gpio_mode(LED2, OUTPUT, OPEN_DRAIN | SPEED_MED);
	stm32_gpio_mode(LED3, OUTPUT, OPEN_DRAIN | SPEED_MED);
	stm32_gpio_mode(LED4, OUTPUT, OPEN_DRAIN | SPEED_MED);

	stm32_gpio_set(LED1);
	stm32_gpio_set(LED2);
	stm32_gpio_set(LED3);
	stm32_gpio_set(LED4);
}

/* -------------------------------------------------------------------------
 * LEDs
 * ------------------------------------------------------------------------- */

void led_on(struct stm32_gpio *__gpio, int __pin) {
	stm32_gpio_clr(__gpio, __pin);
}

void led_off(struct stm32_gpio *__gpio, int __pin) {
	stm32_gpio_set(__gpio, __pin);
}


int led_task(void * arg)
{
	int i;

	for (i = 1000000; i > 0; --i) {
		led_on(LED1);
		thinkos_sleep(167);
		led_on(LED2);
		thinkos_sleep(166);
		led_off(LED1);
		led_on(LED3);
		thinkos_sleep(166);
		led_off(LED2);
		led_on(LED4);
		thinkos_sleep(166);
		led_off(LED3);
		thinkos_sleep(166);
		led_off(LED4);
		thinkos_sleep(166);
	}

	return 0;
}

void leds_init(void)
{
	static uint32_t led_stack[256];

	static const struct thinkos_thread_inf led_inf = {
			.stack_ptr = led_stack,
			.stack_size = sizeof(led_stack),
			.priority = 8,
			.thread_id = 2,
			.paused = 0,
			.tag = "LED"
	};

	thinkos_thread_create_inf(led_task, NULL, &led_inf);
}

/* -------------------------------------------------------------------------
 * ADC Configuration
 * ------------------------------------------------------------------------- */

void adc1_init(unsigned int chans)
{
	struct stm32f_adc * adc = STM32F_ADC1;
	struct stm32f_adcc * adcc = STM32F_ADCC;

	/* ADC clock enable */
	stm32_clk_enable(STM32_RCC, STM32_CLK_ADC1);

	/* configure for DMA use */
	adc->cr1 = ADC_RES_12BIT | ADC_SCAN;
	adc->cr2 = ADC_EXTEN_RISING | ADC_EXTSEL_TIM2_TRGO | ADC_ADON |
			   ADC_DDS | ADC_DMA;
	adc->sqr1 = ADC_L_SET(chans - 1); 
	adc->sqr2 = 0;
	adc->sqr3 = ADC_SQ1_SET(6) | ADC_SQ2_SET(17) | 
		ADC_SQ3_SET(18) | ADC_SQ4_SET(16);

	/* set the sample time */
	stm32f_adc_smp_set(adc, 6, 3);
	stm32f_adc_smp_set(adc, 17, 3);
	stm32f_adc_smp_set(adc, 18, 3);
	stm32f_adc_smp_set(adc, 16, 3);

	/* Common Control */
	adcc->ccr = ADC_TSVREFE | ADC_VBATE | ADC_ADCPRE_4;
	/* PCLK2 = 60MHz
	   ADCCLK = PCLK2/4 = 15MHz */
}

/* -------------------------------------------------------------------------
 * DMA Configuration
 * ------------------------------------------------------------------------- */

void dma2_init(void * dst0, void * dst1, void * src, unsigned int ndt)
{
	struct stm32f_dma * dma = STM32F_DMA2;

	/* DMA clock enable */
	stm32_clk_enable(STM32_RCC, STM32_CLK_DMA2);

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
		DMA_CT_M0AR | DMA_DBM |  DMA_MSIZE_32 | DMA_PSIZE_32 | DMA_MINC | 
		DMA_CIRC | DMA_DIR_PTM | DMA_TCIE;

	/* enable DMA */
	dma->s[0].cr |= DMA_EN;	
}

/* -------------------------------------------------------------------------
 * Timer Configuration
 * ------------------------------------------------------------------------- */

void tim2_init(uint32_t freq)
{
	struct stm32f_tim * tim2 = STM32F_TIM2;
	uint32_t div;
	uint32_t pre;
	uint32_t n;

	/* get the total divisior */
	div = ((2 * stm32f_tim1_hz) + (freq / 2)) / freq;
	/* get the minimum pre scaler */
	pre = (div / 65536) + 1;
	/* get the reload register value */
	n = (div + pre / 2) / pre;

	printf(" %s(): div=%d pre=%d n=%d\n", __func__, pre, div, n);

	/* Timer clock enable */
	stm32_clk_enable(STM32_RCC, STM32_CLK_TIM2);
	
	/* Timer configuration */
	tim2->psc = pre - 1;
	tim2->arr = n - 1;
	tim2->cnt = 0;
	tim2->egr = 0;
	tim2->dier = TIM_UIE; /* Update interrupt enable */
	tim2->cr2 = TIM_MMS_OC1REF;
	tim2->cr1 = TIM_URS | TIM_CEN;
	tim2->ccmr1 = TIM_OC1M_PWM_MODE1;
	tim2->ccr1 = tim2->arr - 2;
	printf(" %s(): PSC=%d AAR=%d.\n", __func__, tim2->psc, tim2->arr);
	printf(" %s(): F=%dHz\n", __func__, (2 * stm32f_tim1_hz) /
		   ((tim2->psc + 1) * ( tim2->arr + 1)));
}

/* -------------------------------------------------------------------------
 * ADC Capture Thread
 * ------------------------------------------------------------------------- */

#define VT25  760
#define AVG_SLOPE 2500
#define ADC_CHANS 4
#define ADC_SAMPLES 16
#define ADC_RATE 10 * ADC_SAMPLES

struct adc_set {
	int32_t ch[ADC_CHANS];
};

int adc_task(void * arg)
{
	struct stm32f_adc * adc = STM32F_ADC1;
	struct stm32f_dma * dma = STM32F_DMA2;
	int self = thinkos_thread_self();
	struct adc_set adc_buf[2][ADC_SAMPLES];
	struct adc_set * sample;
	int32_t scale[ADC_CHANS];
	int32_t val[ADC_CHANS];
	int32_t temp;
	int32_t sum;
	int cnt;
	int i;
	int j;

	printf(" [%d] started.\n", self);
	thinkos_sleep(100);

	/* GPIO */
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOB);
	stm32_gpio_mode(STM32_GPIOB, 6, OUTPUT, PUSH_PULL | SPEED_MED);
	/* ADC Input pins */
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOA);
	stm32_gpio_mode(ADC6_GPIO, ADC6_PORT, ANALOG, 0);

	adc1_init(ADC_CHANS);

	dma2_init(adc_buf[0], adc_buf[1], (void *)&adc->dr, 
			  ADC_CHANS * ADC_SAMPLES);

	tim2_init(ADC_RATE);

	/* set the scale factor to get a millivolts readout */
	scale[0] = 6600; /* ADC channel 6 */
	scale[1] = 3300; /* VREFINT */
	scale[2] = 6600; /* VBAT */
	scale[3] = 3300; /* Temperature sensor */

	cnt = 0;

	while (1) {
		/* wait for the DMA transfer to complete */
		thinkos_irq_wait(STM32F_IRQ_DMA2_STREAM0);
		if ((dma->lisr & DMA_TCIF0) == 0) {
			dma->lifcr = dma->lisr;
			continue;
		}
		/* clear the DMA transfer complete flag */
		dma->lifcr = DMA_CTCIF0;

		/* get a pointer to the DMA sample buffer */
		sample = adc_buf[cnt++ & 1];

		/* read from DMA buffer and scale the signal */
		for (i = 0; i < ADC_CHANS; i++) {
			/* average the samples */
			for (j = 0, sum = 0; j < ADC_SAMPLES; j++)
				sum += sample[j].ch[i];
			val[i] = ((sum / ADC_SAMPLES) * scale[i]) / 4096;
		}

		printf("\r %4d - ", cnt);
		printf(" %2d.%03d[V]", val[0] / 1000, val[0] % 1000);
		printf(" %2d.%03d[V]", val[1] / 1000, val[1] % 1000);
		printf(" %2d.%03d[V]", val[2] / 1000, val[2] % 1000);
		temp = (((val[3] - VT25) * 1000) / AVG_SLOPE) + 25;
		printf(" %2d[dg.C]", temp);

		if (cnt & 1)
			led_on(LED3);
		else
			led_off(LED3);
	}

	return 0;
}

void adc_init(void)
{
	static uint32_t adc_stack[256 + 2 * ADC_CHANS * ADC_SAMPLES];
	static const struct thinkos_thread_inf adc_inf = {
		.stack_ptr = adc_stack, 
		.stack_size = sizeof(adc_stack), 
		.priority = 8,
		.thread_id = 31, 
		.paused = 0,
		.tag = "ADC"
	};
	thinkos_thread_create_inf(adc_task, NULL, &adc_inf);
}

int main(int argc, char ** argv)
{
	uint32_t clk;

	io_init();

	stdio_init();
/*
	printf("\r\n");
	printf("---------------------------------------------------------\r\n");
	printf(" ThinkOS ADC-DMA Test\r\n");
	printf("---------------------------------------------------------\r\n");
	printf("\r\n");
*/
	leds_init();

	adc_init();

	clk = thinkos_clock();

	for (; ; ) {
		clk += 1000;
		thinkos_alarm(clk);
	}

	return 0;
}


void _exit(int status)
{
	for (;;);
}

