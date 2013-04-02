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
#include <sys/usb-cdc.h>
#include "wavetab.h"

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
 * DAC
 * ----------------------------------------------------------------------
 */
#define DAC1_GPIO STM32F_GPIOA, 4
#define DAC2_GPIO STM32F_GPIOA, 5

#define DAC1_DMA_CHAN 3
#define DAC2_DMA_CHAN 4

void wave_set(int dac, uint8_t * wave, unsigned int len)
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

void wave_play(int dac)
{
	struct stm32f_dma * dma = STM32F_DMA1;
	/* enable DMA */
	if (dac)
		dma->ch[DAC2_DMA_CHAN].ccr |= DMA_EN;
	else
		dma->ch[DAC1_DMA_CHAN].ccr |= DMA_EN;
}

void wave_pause(int dac)
{
	struct stm32f_dma * dma = STM32F_DMA1;
	/* disable DMA */
	if (dac)
		dma->ch[DAC2_DMA_CHAN].ccr &= ~DMA_EN;
	else
		dma->ch[DAC1_DMA_CHAN].ccr &= ~DMA_EN;

}

void tone_play(int dac, unsigned int tone, unsigned int ms)
{
	uint8_t * wave;
	unsigned int len;

	wave = (uint8_t *)tone_lut[tone].buf;
	len = tone_lut[tone].len;

	wave_set(dac, wave, len);
	wave_play(dac);
	/* FIXME: this should be handled by an interrupt or other task. 
	   This function should return immediately */
	thinkos_sleep(ms);
	wave_pause(dac);
}

#define DAC_TIMER_DIV 1

void stm32f_dac_init(void)
{
	struct stm32f_rcc * rcc = STM32F_RCC;
	struct stm32f_dac * dac = STM32F_DAC;
	struct stm32f_tim * tim2 = STM32F_TIM2;
	struct stm32f_dma * dma = STM32F_DMA1;
	uint32_t f_tmr = (stm32f_apb1_hz * 2) / DAC_TIMER_DIV;

	/* I/O pins config */
	stm32f_gpio_mode(DAC2_GPIO, ANALOG, 0);
	stm32f_gpio_mode(DAC1_GPIO, ANALOG, 0);

	/* DAC clock enable */
	rcc->apb1enr |= RCC_DACEN;
	/* DAC disable */
	dac->cr = 0;

	/* Timer clock enable */
	rcc->apb1enr |= RCC_TIM2EN;
	/* Timer disable */
	tim2->cr1 = TIM_URS | TIM_CEN;

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
	dac->dhr12r2 = 2048;
	/* DAC channel 1 initial value */
	dac->dhr12r1 = 2048;

	/* Timer clock enable */
	rcc->apb1enr |= RCC_TIM2EN;
	tim2->psc = DAC_TIMER_DIV - 1; /* 2 * APB1_CLK(22.579200 MHz) / 1 = 4MHz */
	tim2->arr = (f_tmr / SAMPLE_RATE) - 1; /* 22.579200 MHz / 2822 = 8001 Hz*/
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
	dma->ch[DAC1_DMA_CHAN].cpar = &dac->dhr8r1;
	/* Memory address */
	dma->ch[DAC1_DMA_CHAN].cmar = (void *)a3;
	/* Number of data items to transfer */
	dma->ch[DAC1_DMA_CHAN].cndtr = sizeof(a3);
	/* Configuration single buffer circular */
	dma->ch[DAC1_DMA_CHAN].ccr = DMA_MSIZE_8 | DMA_PSIZE_8 | DMA_MINC |
		DMA_CIRC | DMA_DIR_MTP;

	/*  DMA Configuration */
	/* Peripheral address */
	dma->ch[DAC2_DMA_CHAN].cpar = &dac->dhr8r2;
	/* Memory address */
	dma->ch[DAC2_DMA_CHAN].cmar = (void *)d3;
	/* Number of data items to transfer */
	dma->ch[DAC2_DMA_CHAN].cndtr = sizeof(d3);
	/* Configuration single buffer circular */
	dma->ch[DAC2_DMA_CHAN].ccr = DMA_MSIZE_8 | DMA_PSIZE_8 | DMA_MINC |
		DMA_CIRC | DMA_DIR_MTP;
}

int main(int argc, char ** argv)
{
	int i = 0;

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
	printf(" Tone generator test\n");
	printf("-----------------------------------------\n");
	printf("\n");

	stm32f_dac_init();
	wave_play(0);
	wave_play(1);

	for (i = 0; ; i++) {
		printf(" - %d\n", i);
		DCC_LOG1(LOG_TRACE, "%d", i);

		thinkos_sleep(1000);
		relay_on(0);
		led_on(0);

		thinkos_sleep(1000);
		relay_off(0);
		led_off(0);

		thinkos_sleep(1000);
		
	}

	return 0;
}

