/* 
 * File:	 adc_test.c
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
#include <arch/cortex-m3.h>
#include <sys/delay.h>
#include <sys/serial.h>
#include <sys/param.h>
#include <sys/file.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define __THINKOS_IRQ__
#include <thinkos_irq.h>
#include <thinkos.h>

#include <sys/dcclog.h>

/* GPIO pin description */ 
struct stm32f_io {
	struct stm32f_gpio * gpio;
	uint8_t pin;
};

/* ----------------------------------------------------------------------
 * LEDs 
 * ----------------------------------------------------------------------
 */

const struct stm32f_io led_io[] = {
	{ STM32F_GPIOB, 14 },
	{ STM32F_GPIOB, 15 },
	{ STM32F_GPIOC, 13 },
	{ STM32F_GPIOC, 14 },
	{ STM32F_GPIOC, 15 }
};

void led_on(int id)
{
	stm32f_gpio_set(led_io[id].gpio, led_io[id].pin);
}

void led_off(int id)
{
	stm32f_gpio_clr(led_io[id].gpio, led_io[id].pin);
}

void leds_init(void)
{
	int i;

	for (i = 0; i < 5; ++i) {
		stm32f_gpio_mode(led_io[i].gpio, led_io[i].pin,
						 OUTPUT, PUSH_PULL | SPEED_LOW);

		stm32f_gpio_clr(led_io[i].gpio, led_io[i].pin);
	}
}

/* ----------------------------------------------------------------------
 * Relays 
 * ----------------------------------------------------------------------
 */

const struct stm32f_io relay_io[] = {
	{ STM32F_GPIOA, 9 },
	{ STM32F_GPIOA, 10 },
	{ STM32F_GPIOA, 11 },
	{ STM32F_GPIOA, 12 },
	{ STM32F_GPIOB, 2 }
};

void relay_on(int id)
{
	stm32f_gpio_set(relay_io[id].gpio, relay_io[id].pin);
}

void relay_off(int id)
{
	stm32f_gpio_clr(relay_io[id].gpio, relay_io[id].pin);
}

void relays_init(void)
{
	int i;

	for (i = 0; i < 5; ++i) {
		stm32f_gpio_mode(relay_io[i].gpio, relay_io[i].pin,
						 OUTPUT, PUSH_PULL | SPEED_LOW);

		stm32f_gpio_clr(relay_io[i].gpio, relay_io[i].pin);
	}
}

/* ----------------------------------------------------------------------
 * Console 
 * ----------------------------------------------------------------------
 */

#define USART1_TX STM32F_GPIOB, 6
#define USART1_RX STM32F_GPIOB, 7

struct file stm32f_uart1_file = {
	.data = STM32F_USART1, 
	.op = &stm32f_usart_fops 
};

void stdio_init(void)
{
	struct stm32f_usart * us = STM32F_USART1;
	struct stm32f_afio * afio = STM32F_AFIO;

	/* USART1_TX */
	stm32f_gpio_mode(USART1_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	/* USART1_RX */
	stm32f_gpio_mode(USART1_RX, INPUT, PULL_UP);
	/* Use alternate pins for USART1 */
	afio->mapr |= AFIO_USART1_REMAP;

	stm32f_usart_init(us);
	stm32f_usart_baudrate_set(us, 115200);
	stm32f_usart_mode_set(us, SERIAL_8N1);
	stm32f_usart_enable(us);

	stdin = &stm32f_uart1_file;
	stdout = &stm32f_uart1_file;
	stderr = &stm32f_uart1_file;
}

/* ----------------------------------------------------------------------
 * Console 
 * ----------------------------------------------------------------------
 */
void io_init(void)
{
	struct stm32f_rcc * rcc = STM32F_RCC;

	DCC_LOG(LOG_MSG, "Configuring GPIO pins...");

	stm32f_gpio_clock_en(STM32F_GPIOA);
	stm32f_gpio_clock_en(STM32F_GPIOB);
	stm32f_gpio_clock_en(STM32F_GPIOC);

	/* Enable Alternate Functions IO clock */
	rcc->apb2enr |= RCC_AFIOEN;

}

/* ----------------------------------------------------------------------
 * ADC
 * ----------------------------------------------------------------------
 */

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
		DMA_MINC | DMA_CIRC | DMA_DIR_PTM | DMA_HTIE | DMA_TCIE;
	/* enable DMA */
	dma->ch[ADC_DMA_CHAN].ccr |= DMA_EN;	
}

static void adc_timer_init(uint32_t freq)
{
	struct stm32f_rcc * rcc = STM32F_RCC;
	struct stm32f_tim * tim = STM32F_TIM3;
	uint32_t div;
	uint32_t pre;
	uint32_t n;

	/* get the total divisior */
	div = ((2 * stm32f_apb1_hz) + (freq / 2)) / freq;
	/* get the minimum pre scaler */
	pre = (div / 65536) + 1;
	/* get the reload register value */
	n = (div + pre / 2) / pre;


	printf(" %s(): freq=%dHz pre=%d n=%d\n", 
		   __func__, freq, pre, n);

	/* Timer clock enable */
	rcc->apb1enr |= RCC_TIM3EN;
	
	/* Timer configuration */
	tim->psc = pre - 1;
	tim->arr = n - 1;
	tim->cnt = 0;
	tim->egr = 0;
	tim->dier = TIM_UIE; /* Update interrupt enable */
	tim->ccmr1 = TIM_OC1M_PWM_MODE1;
	tim->ccr1 = tim->arr - 2;
	tim->cr2 = TIM_MMS_OC1REF;
	tim->cr1 = TIM_URS | TIM_CEN; /* Enable counter */

	/* Enable DMA interrupt */
//	cm3_irq_enable(STM32F_IRQ_TIM3);
}

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

#define ADC_CHANS 5
#define ADC_RATE 1000

uint16_t adc_buf[2][ADC_CHANS];
volatile uint32_t adc_dma_cnt;
uint32_t adc_avg[ADC_CHANS];

/***********************************************************
  ADC Configuration
 ***********************************************************/
void stm32f_adc_init(void)
{
	struct stm32f_rcc * rcc = STM32F_RCC;
	struct stm32f_adc * adc = STM32F_ADC1;
	const uint8_t adc_chan_seq[] = {0, 1, 2, 3, 6};

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
	stm32f_adc_smp_set(adc, 0, ADC_SMP_56_CYC);
	stm32f_adc_smp_set(adc, 1, ADC_SMP_56_CYC);
	stm32f_adc_smp_set(adc, 2, ADC_SMP_56_CYC);
	stm32f_adc_smp_set(adc, 3, ADC_SMP_56_CYC);
	stm32f_adc_smp_set(adc, 6, ADC_SMP_56_CYC);

	adc_gpio_init();

	adc_dma_init(adc_buf[0], (void *)&adc->dr, 2 * ADC_CHANS);

	/* Set DMA to very low priority */
	cm3_irq_pri_set(STM32F_IRQ_DMA1_STREAM0, 0xf0);
	/* Enable DMA interrupt */
	cm3_irq_enable(STM32F_IRQ_DMA1_STREAM0);

	/* Configure timer and start periodic conversion */
	adc_timer_init(ADC_RATE);
}

void stm32f_dma1_stream0_isr(void)
{
	struct stm32f_dma * dma = STM32F_DMA1;
	uint16_t * data;
	uint32_t cnt;
	int i;

	if (dma->isr & DMA_HTIF1) {
		/* clear the DMA half transfer flag */
		dma->ifcr = DMA_CHTIF1;
		data = adc_buf[0];
//		printf("-");
	} 

	if (dma->isr & DMA_TCIF1) {
		/* clear the DMA transfer complete flag */
		dma->ifcr = DMA_CTCIF1;
		data = adc_buf[1];
//		printf("=");
	}

	cnt = adc_dma_cnt + 1;
	adc_dma_cnt = cnt;

	/* avg = (avg * 15 + x) / 16 */
	/* a16 = avg*16 = (avg * 15 + x) */
	/* a16 = (a16 * 15 / 16) + x) */

	#define ADC_AVG_N 16

	for (i = 0; i < ADC_CHANS; ++i) {
		adc_avg[i] = (adc_avg[i] * (ADC_AVG_N - 1) / ADC_AVG_N) + data[i];
//		adc_avg[i] = data[i];
	}

}

void stm32f_tim3_isr(void)
{
	struct stm32f_tim * tim = STM32F_TIM3;

	/* Clear interrupt flags */
	tim->sr = 0;

	printf("+");
}

int main(int argc, char ** argv)
{
	int led;
	int i;
	int j;

	DCC_LOG_INIT();
	DCC_LOG_CONNECT();

	/* calibrate usecond delay loop */
	cm3_udelay_calibrate();

	DCC_LOG(LOG_TRACE, "1. io_init()");
	io_init();

	DCC_LOG(LOG_TRACE, "2. leds_init()");
	leds_init();

	DCC_LOG(LOG_TRACE, "3. relays_init()");
	relays_init();

	DCC_LOG(LOG_TRACE, "4. thinkos_init()");
	thinkos_init(THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(32));

	DCC_LOG(LOG_TRACE, "5. stdio_init()");
	stdio_init();

	printf("\n\n");
	printf("-----------------------------------------\n");
	printf(" ADC test\n");
	printf("-----------------------------------------\n");
	printf("\n");

	stm32f_adc_init();

	for (i = 0; ; i++) {
		led = i % 5;

//		led_on(led);
		thinkos_sleep(100);
//		led_off(led);
		thinkos_sleep(900);
	

		printf(" - %3d %6d ", i, adc_dma_cnt);

		for (j = 0; j < ADC_CHANS; ++j) {
			printf(" %6d", adc_avg[j]);
			if (adc_avg[j] > 2000) {
				led_on(j);
				relay_on(j);
			} else {
				led_off(j);
				relay_off(j);
			}
		}

		printf("\n");
	}

	return 0;
}

