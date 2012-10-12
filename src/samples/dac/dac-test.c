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
#include <sys/delay.h>
#include <sys/serial.h>

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

/*	dac->cr = CR_TSEL2_SET(7) | CR_TEN2 | CR_EN2 | 
		CR_TSEL1_SET(7) | CR_TEN1 | CR_EN1; */
	dac->cr = CR_EN2 | CR_EN1;
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

void SysTick_Handler(void)
{
	static int i = 0;
	static int j = 0;
	static int v = 0;
	static int32_t k = 0;

	int32_t x;
	struct stm32f_dac * dac = STM32F_DAC;

	x = ((sine_wave[i] * k) / 2048) + 2048;

	dac->dhr12r1 = x;

	if (i == 15) {
		i = 0;
		if (j == 500) { 
			j = 0;
			v += 250;
			if (v > 3250)
				v = 0;
			
			vout(v);

		} else {
			j++;
		}
		if (j < 64)
			k = envelope[j];
	} else {
		i++;
	}
}

void cm3_systick_init(void)
{
	struct cm3_systick * systick = CM3_SYSTICK;

	/* 0.10ms reload */
	systick->load = CM3_SYSTICK_CLK_HZ / 10000;
	systick->val = 0;
	systick->ctrl = SYSTICK_CTRL_ENABLE | SYSTICK_CTRL_TICKINT;
}

void dac_test(void)
{
	printf("DAC test!\r\n");

	dac_init();
	cm3_systick_init();
}

FILE * stdout = (FILE *)&stm32f_usart5_file;
FILE * stderr = (FILE *)&stm32f_usart5_file;

int main(int argc, char ** argv)
{
	struct stm32f_usart * us = STM32F_USART5;

	stm32f_usart_open(us, 115200, SERIAL_8N1);
	cm3_udelay_calibrate();

	printf("\r\n");

	dac_test();

	for (;;) {
		asm("nop\n");
	}

	return 0;
}

