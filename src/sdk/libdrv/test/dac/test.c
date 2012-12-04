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
#include <arch/cortex-m3.h>
#include <sys/delay.h>
#include <sys/serial.h>
#include <sys/param.h>
#include <sys/file.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <thinkos.h>

#include <sys/dcclog.h>

#define DAC1_GPIO STM32F_GPIOA
#define DAC1_PORT 4

#define DAC2_GPIO STM32F_GPIOA
#define DAC2_PORT 5

#ifndef CM3_SYSTICK_CLK_HZ
#error "CM3_SYSTICK_CLK_HZ undefined"
#endif


void dac_init(void)
{
	struct stm32f_dac * dac = STM32F_DAC;
	struct stm32f_rcc * rcc = STM32F_RCC;

	/* clock enable */
	rcc->apb1enr |= RCC_DACEN;

	stm32f_gpio_mode(DAC1_GPIO, DAC1_PORT, ANALOG, 0);
	stm32f_gpio_mode(DAC2_GPIO, DAC2_PORT, ANALOG, 0);

/*	dac->cr = DAC_TSEL2_SET(7) | DAC_TEN2 | DAC_EN2 | 
		DAC_TSEL1_SET(7) | DAC_TEN1 | DAC_EN1; */
	dac->cr = DAC_EN2 | DAC_EN1 | DAC_WAVE1_SET(3);
	dac->dhr12r1 = 2048;
	dac->dhr12r2 = 2048;
}

const int32_t sine_wave[] = {
	0,
	783,
	1447,
	1891,
	2047,
	1891,
	1447,
	783,
	0,
	-783,
	-1447,
	-1891,
	-2047,
	-1891,
	-1447,
	-783,
};

const int32_t envelope[] = {
	   8,   32,  128,  512,
	2048, 2048, 2048, 2048, 
	2048, 2048, 2048, 2048, 
	2048, 2048, 2048, 2048, 

	2048, 2048, 2048, 2048, 
	2048, 2048, 2048, 2048, 
	1722, 1448, 1217, 1024,
	 861,  724,  608,  512,

	 430,  362,  304,  256,
	 215,  181,  152,  128,
	 107,   90,   76,   64,
	  53,   45,   38,   32,

      26,   22,   19,   16,
	  13,   11,    9,    8,
	   6,    5,    4,    4,
	   3,    2,    2,    0
};

void vout(unsigned int mv)
{
	struct stm32f_dac * dac = STM32F_DAC;

	dac->dhr12r2 = (5160 * mv) / 8192;
}

const int32_t _sine_wave[] = {
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

volatile uint32_t wave_clk = 0;
volatile uint32_t wave_div = 250;
volatile uint32_t wave_gain = 0;

#define FRAME_SIZE 16

volatile uint8_t wave[FRAME_SIZE];

void wave_zero(void)
{
	int i;

	for (i = 0; i < FRAME_SIZE; i++)
		wave[i] = 128;
}

void wave_set(uint32_t gain)
{
	int i;

	for (i = 0; i < FRAME_SIZE; i++)
		wave[i] = 128 + ((sine_wave[i] * gain) / (2048 << 4));
}

#define DAC1_DMA 5
#define DAC1_DMA_CHAN 7

int dac_waveform_task(void * arg)
{
	struct stm32f_rcc * rcc = STM32F_RCC;
	struct stm32f_dac * dac = STM32F_DAC;
	struct stm32f_tim * tim7 = STM32F_TIM7;
	struct stm32f_dma * dma = STM32F_DMA1;

	printf(" [%d] started.\n", thinkos_thread_self());

	thinkos_sleep(100);

	printf("...\n");

	/* Timer clock enable */
	rcc->apb1enr |= RCC_TIM7EN;
	tim7->cnt = 0;
	tim7->psc = 60 - 1; /* 2 * APB1_CLK(30MHz) / 15 = 4MHz */
	tim7->arr = wave_div - 1; /* 4MHz / 256 = 16000 Hz*/
	tim7->egr = 0; /* Update generation */
	tim7->dier = TIM_UIE; /* Update interrupt enable */
	tim7->cr2 = TIM_MMS_OC1REF;
	tim7->cr1 = TIM_URS | TIM_CEN;
	tim7->ccmr1 = TIM_OC1M_PWM_MODE1;
	tim7->ccr1 = tim7->arr - 2;

	/* I/O pins config */
	stm32f_gpio_clock_en(DAC2_GPIO);
	stm32f_gpio_mode(DAC2_GPIO, DAC2_PORT, ANALOG, 0);
	stm32f_gpio_mode(DAC1_GPIO, DAC1_PORT, ANALOG, 0);

	/* DAC clock enable */
	rcc->apb1enr |= RCC_DACEN;
	dac->cr = DAC_EN2 | DAC_EN1 | DAC_TSEL1_TIMER7 | DAC_TEN1;
	dac->dhr12r1 = 2048;
	dac->dhr12r1 = 2048;

	/*  DMA Configuration */

	/* DMA clock enable */
	rcc->ahb1enr |= RCC_DMA1EN;

	/* Disable DMA channel */
	dma->s[DAC1_DMA].cr = 0;
	/* Wait for the channel to be ready .. */
	while (dma->s[DAC1_DMA].cr & DMA_EN); 

	/* peripheral address */
	dma->s[DAC1_DMA].par = &dac->dhr8r1;
	/* DMA address */
	dma->s[DAC1_DMA].m0ar = wave;
	/* Number of data items to transfer */
	dma->s[DAC1_DMA].ndtr = FRAME_SIZE;
	/* Configuration single buffer circular */
	dma->s[DAC1_DMA].cr = DMA_CHSEL_SET(DAC1_DMA_CHAN) | 
		DMA_MBURST_1 | DMA_PBURST_1 | 
		DMA_MSIZE_8 | DMA_PSIZE_8 | DMA_MINC | 
		DMA_CIRC | DMA_DIR_MTP | DMA_TCIE;

	/* enable DMA */
	dma->s[DAC1_DMA].cr |= DMA_EN;	


#if 0
	while (1) {
		thinkos_irq_wait(STM32F_IRQ_TIM7);
		if (tim7->sr == 0)
			continue;
		/* Clear update interrupt flag */
		tim7->sr = 0;
		
		/* write into ADC buffer */
		dac->dhr8r1 = wave[wave_clk++ & (FRAME_SIZE - 1)];
	}
#endif

	while (1) {
		/* wait for the DMA transfer to complete */
		thinkos_irq_wait(STM32F_IRQ_DMA1_STREAM5);
		if ((dma->hisr & DMA_TCIF5) == 0) {
			/* XXX: ??? */
			dma->hifcr = dma->hisr;
			continue;
		}
		/* clear the DMA transfer complete flag */
		dma->hifcr = DMA_CTCIF5;

		printf(".");
	}

	return 0;
}

uint32_t wave_stack[512];

struct file * uart_console_open(unsigned int baudrate, unsigned int flags);

int main(int argc, char ** argv)
{
	int c;
	struct stm32f_tim * tim7 = STM32F_TIM7;

//	uint32_t now;
//	uint32_t prev;
//	uint32_t dt;

	cm3_udelay_calibrate();
	thinkos_init(THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(0));


	stdout = uart_console_open(115200, SERIAL_8N1);
	stdout = stm32f_usart_open(STM32F_UART5, 115200, SERIAL_8N1);
	stdin = stdout;

	printf("\n");
	printf("------------------------------------------------------\n");
	printf("- USB test\n");
	printf("------------------------------------------------------\n");
	printf("\n");
	udelay(1000);

	printf("\r\n");

	thinkos_thread_create(dac_waveform_task, (void *)NULL, 
						  wave_stack, sizeof(wave_stack), 
						  THINKOS_OPT_PRIORITY(0));

//	prev = wave_clk;
	for (;;) {
		c = getchar();
		printf("%c", c);
		switch (c) {
		case '0':
			wave_div = 500 / 4;
			break;
		case '1':
			wave_div = 500 / 5;
			break;
		case '2':
			wave_div = 500 / 6;
			break;
		case '3':
			wave_div = 500 / 7;
			break;
		case '4':
			wave_div = 500 / 8;
			break;
		case '5':
			wave_div = 500 / 9;
			break;
		case '6':
			wave_div = 500 / 10;
			break;
		case '7':
			wave_div = 500 / 11;
			break;
		case '8':
			wave_div = 500 / 12;
			break;
		case '9':
			wave_div = 500 / 13;
			break;
		}
		tim7->arr = wave_div - 1; /* 4MHz / 256 = 16000 Hz*/
		wave_set(2048);
		thinkos_sleep(500);
		wave_zero();
		wave_gain = 0;
//		now = wave_clk;
//		dt = now - prev;
//		prev = now;
//		printf("%d ", dt);
	}

	return 0;
}

