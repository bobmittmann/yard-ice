/* 
 * File:	 adc_dma.c
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
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
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <arch/cortex-m3.h>
#include <sys/serial.h>
#include <sys/delay.h>
#include <sys/tty.h>
#include <sys/dcclog.h>

#include <thinkos.h>

void stdio_init(void);

struct thread_ctrl {
	volatile bool enabled;
};

#define DAC1_GPIO STM32_GPIOA
#define DAC1_PORT 4

#define DAC2_GPIO STM32_GPIOA
#define DAC2_PORT 5

#define ADC6_GPIO STM32_GPIOA
#define ADC6_PORT 6

const int32_t sine_wave[] = {
	2047, 2147, 2247, 2347, 2446, 2544, 2641, 2736,
	2830, 2922, 3011, 3099, 3184, 3266, 3345, 3421,
	3494, 3563, 3629, 3691, 3749, 3802, 3852, 3897,
	3938, 3974, 4005, 4032, 4054, 4071, 4084, 4091,
	4094, 4091, 4084, 4071, 4054, 4032, 4005, 3974,
	3938, 3897, 3852, 3802, 3749, 3691, 3629, 3563,
	3494, 3421, 3345, 3266, 3184, 3099, 3011, 2922,
	2830, 2736, 2641, 2544, 2446, 2347, 2247, 2147,
	2047, 1946, 1846, 1746, 1647, 1549, 1452, 1357,
	1263, 1171, 1082, 994, 909, 827, 748, 672,
	599, 530, 464, 402, 344, 291, 241, 196,
	155, 119, 88, 61, 39, 22, 9, 2,
	0, 2, 9, 22, 39, 61, 88, 119,
	155, 196, 241, 291, 344, 402, 464, 530,
	599, 672, 748, 827, 909, 994, 1082, 1171,
	1263, 1357, 1452, 1549, 1647, 1746, 1846, 1946
}; 

void timer_init(struct stm32f_tim * tim, uint32_t freq)
{
	uint32_t div;
	uint32_t pre;
	uint32_t n;

	/* get the total divisior */
	div = ((stm32f_tim1_hz) + (freq / 2)) / freq;
	/* get the minimum pre scaler */
	pre = (div / 65536) + 1;
	/* get the reload register value */
	n = (div + pre / 2) / pre;


	printf(" %s(): freq=%dHz pre=%d n=%d\n", 
		   __func__, freq, pre, n);

	/* Timer configuration */
	tim->psc = pre - 1;
	tim->arr = n - 1;
	tim->cnt = 0;
	tim->egr = 0;
	tim->dier = TIM_UIE; /* Update interrupt enable */
	tim->cr2 = 0;
	tim->cr1 = TIM_URS | TIM_CEN;

}

int dac_waveform_task(void * arg)
{
	struct thread_ctrl * ctrl = (struct thread_ctrl *)arg;
	struct stm32f_dac * dac = STM32F_DAC;
	struct stm32f_tim * tim7 = STM32F_TIM7;
	int self = thinkos_thread_self();
	unsigned int sin_freq;
	unsigned int dac_freq;
	int i = 0;

	printf(" [%d] started.\n", self);
	thinkos_sleep(100);

	/* Timer clock enable */
	stm32_clk_enable(STM32_RCC, STM32_CLK_TIM7);

	sin_freq = 440;
	dac_freq = (sin_freq * sizeof(sine_wave)) / sizeof(int32_t);

	timer_init(tim7, dac_freq);
	tim7->cr1 = TIM_URS | TIM_CEN;

	/* I/O pins config */
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOA);
	stm32_gpio_mode(DAC2_GPIO, DAC2_PORT, ANALOG, 0);

	/* DAC clock enable */
	stm32_clk_enable(STM32_RCC, STM32_CLK_DAC);
	dac->cr = DAC_EN2;
	dac->dhr12r2 = 2048;

	DCC_LOG(LOG_TRACE, "thinkos_irq_wait(STM32F_IRQ_TIM7)");
	while (ctrl->enabled) {
		thinkos_irq_wait(STM32F_IRQ_TIM7);
		if (tim7->sr == 0)
			continue;
		/* Clear update interrupt flag */
		tim7->sr = 0;
		/* write into ADC buffer */
		dac->dhr12r2 = sine_wave[i];
		i += (i == 127) ? -127 : 1;
	}

	return 0;
}

/***********************************************************
  ADC Configuration
 ***********************************************************/
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

/***********************************************************
  DMA Configuration
 ***********************************************************/
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

/***********************************************************
  Timer Configuration
 ***********************************************************/
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

#define VT25  760
#define AVG_SLOPE 2500
#define ADC_CHANS 4
#define ADC_SAMPLES 1024
#define ADC_RATE 10 * ADC_SAMPLES

struct dset {
	int32_t ch[ADC_CHANS];
};

int adc_capture_task(void * arg)
{
	struct thread_ctrl * ctrl = (struct thread_ctrl *)arg;
	struct stm32f_adc * adc = STM32F_ADC1;
	struct stm32f_dma * dma = STM32F_DMA2;
	int self = thinkos_thread_self();
	struct dset adc_buf[2][ADC_SAMPLES];
	struct dset * sample;
	int32_t scale[ADC_CHANS];
	int32_t val[ADC_CHANS];
	int32_t temp;
	int32_t sum;
	int cnt;
	int i;
	int j;

	printf(" [%d] started.\n", self);
	thinkos_sleep(100);

	/***********************************************************
	 I/O pin configuration
	 ***********************************************************/
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
	while (ctrl->enabled) {
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
			stm32_gpio_set(STM32_GPIOB, 6);
		else
			stm32_gpio_clr(STM32_GPIOB, 6);
	}

	return 0;
}

uint32_t capture_stack[512 + 2 * ADC_CHANS * ADC_SAMPLES];
 
uint32_t wave_stack[512];

int main(int argc, char ** argv)
{
	struct thread_ctrl ctrl;
	int capture_th;
	int wave_th;

	DCC_LOG_INIT();
	DCC_LOG_CONNECT();

	cm3_udelay_calibrate();

	stdio_init();

	printf("\n");
	printf("---------------------------------------------------------\n");
	printf(" ThinkOS ADC-DMA Test\n");
	printf("---------------------------------------------------------\n");
	printf("\n");

	thinkos_init(THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(0));

	printf("stack=%d\n", sizeof(capture_stack));
	thinkos_sleep(2000);

	ctrl.enabled = true;

	capture_th = thinkos_thread_create(adc_capture_task, (void *)&ctrl, 
						  capture_stack, sizeof(capture_stack));

	/* wait for the capture thread to finish */
	thinkos_join(capture_th);

	wave_th = thinkos_thread_create(dac_waveform_task, (void *)&ctrl, 
						  wave_stack, sizeof(wave_stack));

	thinkos_join(wave_th);

	printf("---------------------------------------------------------\n");
	udelay(10000);

	return 0;
}

