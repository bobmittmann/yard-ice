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

/***********************************************************
  DMA Configuration
 ***********************************************************/
static void adc_dma2_init(void * dst0, void * dst1, void * src, unsigned int ndt)
{
	struct stm32f_rcc * rcc = STM32F_RCC;
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
	struct stm32f_rcc * rcc = STM32F_RCC;
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
	stm32f_gpio_clock_en(ADC6_GPIO);
	stm32f_gpio_mode(ADC6_GPIO, ADC6_PORT, ANALOG, 0);
}

#define ADC_CHANS 3
#define ADC_RATE 10

static uint16_t adc_buf[2][ADC_CHANS];
static uint32_t adc_dma_cnt;

static int16_t adc_vin;
static int16_t adc_vbat;
static int16_t adc_temp;

#if (ENABLE_ADC_SYNC)
static uint8_t ev_adc_dma;
#endif

/***********************************************************
  ADC Configuration
 ***********************************************************/
void stm32f_adc_init(void)
{
	struct stm32f_rcc * rcc = STM32F_RCC;
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
	ev_adc_dma = thinkos_ev_alloc(); 
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
	__thinkos_ev_raise(ev_adc_dma);
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
	__thinkos_ev_wait(ev_adc_dma);
}
#endif

#endif

/* ----------------------------------------------------------------------
 * ADC
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

