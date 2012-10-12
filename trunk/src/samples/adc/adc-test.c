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
#include <arch/cortex-m3.h>
#include <sys/serial.h>
#include <sys/delay.h>

#define DAC1_GPIO STM32F_GPIOA
#define DAC1_PORT 4

#define DAC2_GPIO STM32F_GPIOA
#define DAC2_PORT 5

#define ADC6_GPIO STM32F_GPIOA
#define ADC6_PORT 6

#ifndef CM3_SYSTICK_CLK_HZ
#error "CM3_SYSTICK_CLK_HZ undefined"
#endif

void dac_init(void)
{
	struct stm32f_dac * dac = STM32F_DAC;
	struct stm32f_rcc * rcc = STM32F_RCC;

	/* clock enable */
	rcc->apb1enr |= RCC_DACEN;
	/* I/O pins config */
//	stm32f_gpio_mode(DAC1_GPIO, DAC1_PORT, ANALOG, 0);
	stm32f_gpio_mode(DAC2_GPIO, DAC2_PORT, ANALOG, 0);

	dac->cr = CR_EN2;
//	dac->dhr12r1 = 2048;
	dac->dhr12r2 = 2048;
}

int adc_scan(int32_t * buf, int32_t * scale)
{
	struct stm32f_adc * adc = STM32F_ADC1;
	int n;
	int i;

	/* get the number of chanels in the scan list */
	n = ADC_L_GET(adc->sqr1) + 1;


	for (i = 0; i < n; i++) {
		/* Start conversion of regular channels */
		adc->cr2 |= ADC_SWSTART;
		while ((adc->sr & ADC_EOC) == 0) {
//			printf(".");
		};

		/* scale the signal */
		buf[i] = (adc->dr * scale[i]) / 4096;
	}

	return n;
}

void adc_init(void)
{
	struct stm32f_rcc * rcc = STM32F_RCC;
	struct stm32f_adc * adc = STM32F_ADC1;
	struct stm32f_adcc * adcc = STM32F_ADCC;

	/* clock enable */
	rcc->apb2enr |= RCC_ADC1EN;
	/* I/O pin config */
	stm32f_gpio_clock_en(ADC6_GPIO);
	stm32f_gpio_mode(ADC6_GPIO, ADC6_PORT, ANALOG, 0);

	adc->cr1 = ADC_RES_12BIT | ADC_DISCNUM_SET(0) | ADC_DISCEN | ADC_SCAN;
//	adc->cr1 = ADC_RES_12BIT;
	adc->cr2 = ADC_ADON | ADC_EOCS;
//	adc->cr2 = ADC_ADON;
	adc->sqr1 = ADC_L_SET(2); 
	adc->sqr2 = 0;
	adc->sqr3 = ADC_SQ1_SET(6) | ADC_SQ2_SET(16) | ADC_SQ3_SET(18);


	/* set the sample time */
	stm32f_adc_smp_set(adc, 6, 7);
	stm32f_adc_smp_set(adc, 16, 7);
	stm32f_adc_smp_set(adc, 17, 7);
	stm32f_adc_smp_set(adc, 18, 7);

	/* Common Control */
	adcc->ccr = ADC_TSVREFE | ADC_VBATE | ADC_ADCPRE_6;
	/* PCLK2 = 60MHz
	   ADCCLK = PCLK2/6 = 10MHz */

	printf(" -  SQR1=0x%08x @[0x%08x]\n", adc->sqr1, (int)&adc->sqr1);
	printf(" -  SQR2=0x%08x\n", adc->sqr2);
	printf(" -  SQR3=0x%08x\n", adc->sqr3);
	printf(" - SMPR1=0x%08x @[0x%08x]\n", adc->smpr1, (int)&adc->smpr1);
	printf(" - SMPR2=0x%08x\n", adc->smpr2);
	printf(" - CCR=0x%08x @[0x%08x]\n", adcc->ccr, (int)&adcc->ccr);
}

void vout(unsigned int mv)
{
	struct stm32f_dac * dac = STM32F_DAC;

	dac->dhr12r2 = (5160 * mv) / 8192;
}

void SysTick_Handler(void)
{
}

void cm3_systick_init(void)
{
	struct cm3_systick * systick = CM3_SYSTICK;

	/* 0.10ms reload */
	systick->load = CM3_SYSTICK_CLK_HZ / 10000;
	systick->val = 0;
	systick->ctrl = SYSTICK_CTRL_ENABLE | SYSTICK_CTRL_TICKINT;
}

#define VT25  760
#define AVG_SLOPE 2500

void show_temp(int32_t v_sense)
{
	int32_t temp;

	temp = (((v_sense - VT25) * 1000) / AVG_SLOPE) + 25;
	printf(" temp: %d[dg. C]", temp);
}

void adc_test(void)
{
	int32_t val[8];
	int32_t scale[8];
	int n;
	int i;
	printf("DAC test!\r\n");

	dac_init();
	adc_init();
	cm3_systick_init();

	scale[0] = 6600; /* ADC channel 6 */
	scale[1] = 3300; /* Temperature sensor */
	scale[2] = 6600; /* VBAT */

	for (;;) {
		vout(0);
		udelay(100000);
		printf("\n -");
		n = adc_scan(val, scale);
		for (i = 0; i < n; i++)
			printf(" %2d.%03d", val[i] / 1000, val[i] % 1000);
		if (n > 1)
			show_temp(val[1]);

		vout(1000);
		udelay(20000);
		printf("\n -");
		n = adc_scan(val, scale);
		for (i = 0; i < n; i++)
			printf(" %2d.%03d", val[i] / 1000, val[i] % 1000);
		if (n > 1)
			show_temp(val[1]);

		vout(2000);
		udelay(20000);
		printf("\n -");
		n = adc_scan(val, scale);
		for (i = 0; i < n; i++)
			printf(" %2d.%03d", val[i] / 1000, val[i] % 1000);
		if (n > 1)
			show_temp(val[1]);

		vout(3000);
		udelay(20000);
		printf("\n -");
		n = adc_scan(val, scale);
		for (i = 0; i < n; i++)
			printf(" %2d.%03d", val[i] / 1000, val[i] % 1000);
		if (n > 1)
			show_temp(val[1]);
		printf("\n");



		udelay(500000);
		udelay(500000);
		udelay(500000);
		udelay(500000);
	}
}

FILE * stdout = (FILE *)&stm32f_usart5_file;

int main(int argc, char ** argv)
{
	struct stm32f_usart * us = STM32F_USART5;

	stm32f_usart_open(us, 115200, SERIAL_8N1);
	cm3_udelay_calibrate();

	printf("\r\n");

	adc_test();

	for (;;) {
		asm("nop\n");
	}

	return 0;
}

