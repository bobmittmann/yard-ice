/* 
 * File:	 usb-test.c
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

#include <thinkos.h>
#include <sys/serial.h>
#include <crc.h>

#include "board.h"
#include "simlnk.h"

#define PWM_FREQ 100000

void isink_init(void)
{
	struct stm32f_tim * tim = STM32_TIM2;
	struct stm32f_dac * dac = STM32_DAC;
	uint32_t div;
	uint32_t pre;
	uint32_t n;

	/* DAC clock enable */
	stm32_clk_enable(STM32_RCC, STM32_CLK_DAC);
	/* DAC channel 2 initial value */
	dac->dhr12r2 = 0;
	/* DAC configure */
	dac->cr = DAC_EN2;

	/* Timer clock enable */
	stm32_clk_enable(STM32_RCC, STM32_CLK_TIM2);
	/* get the total divisior */
	div = (stm32f_tim1_hz + (PWM_FREQ / 2)) / PWM_FREQ;
	/* get the minimum pre scaler */
	pre = (div / 65536) + 1;
	/* get the reload register value */
	n = (div * 2 + pre) / (2 * pre);
	/* Timer configuration */
	tim->psc = pre - 1;
	tim->arr = n - 1;
	tim->ccmr1 = TIM_OC1M_PWM_MODE1 | TIM_OC2M_PWM_MODE1;
	tim->ccer = TIM_CC1E | TIM_CC2E | TIM_CC3E | TIM_CC4E;
	tim->ccr1 = (tim->arr * 4) / 8;
	tim->ccr2 = (tim->arr * 4) / 8;
	tim->cr1 = TIM_URS | TIM_CEN; /* Enable counter */
}

void isink(int val)
{
	if (val & (1 << 0))
		stm32_gpio_set(IO_SINK1);
	else
		stm32_gpio_clr(IO_SINK1);

	if (val & (1 << 1))
		stm32_gpio_set(IO_SINK2);
	else
		stm32_gpio_clr(IO_SINK2);

	if (val & (1 << 2))
		stm32_gpio_set(IO_SINK3);
	else
		stm32_gpio_clr(IO_SINK3);

	if (val & (1 << 3))
		stm32_gpio_set(IO_SINK4);
	else
		stm32_gpio_clr(IO_SINK4);
}

void irarte(int val)
{
	struct stm32f_dac * dac = STM32_DAC;

	dac->dhr12r2 = 4095 * val / 4;
}

void isink_test(void)
{
	int i;

	for (i = 0; i < 4; ++i) {
		irarte(i);
		thinkos_sleep(4);
		isink(2);
		thinkos_sleep(1);
		isink(6);
		thinkos_sleep(1);
		isink(11);
		thinkos_sleep(1);
		isink(15);
		thinkos_sleep(1);
		isink(0);
	}
}

void leds_mode(int i) 
{
	int sw1 = 0;
	int sw2 = 0;

	sw1 |=  stm32_gpio_stat(IO_SW1_UP) == 0 ? SW_UP : 0;
	sw1 |=  stm32_gpio_stat(IO_SW1_DWN) == 0 ? SW_DWN : 0;

	sw2 |=  stm32_gpio_stat(IO_SW2_UP) == 0 ? SW_UP : 0;
	sw2 |=  stm32_gpio_stat(IO_SW2_DWN) == 0 ? SW_DWN : 0;

	if (sw1 == SW_UP) {
		__led_off(IO_LED1);
		__led_on(IO_LED2);
	} else if (sw1 == SW_DWN) {
		__led_on(IO_LED1);
		__led_off(IO_LED2);
	} else {
		switch (i % 5) {
		case 1:
			__led_on(IO_LED1);
			__led_off(IO_LED2);
			break;
		case 2:
			__led_off(IO_LED1);
			__led_on(IO_LED2);
			break;
		default:			
			__led_off(IO_LED1);
			__led_off(IO_LED2);
			break;
		}
	}

	if (sw2 == SW_UP) {
		__led_off(IO_LED3);
		__led_on(IO_LED4);
	} else if (sw2 == SW_DWN) {
		__led_on(IO_LED3);
		__led_off(IO_LED4);
	} else {
		switch (i % 5) {
		case 3:
			__led_off(IO_LED3);
			__led_on(IO_LED4);
			break;
		case 4:
			__led_on(IO_LED3);
			__led_off(IO_LED4);
			break;
		default:			
			__led_off(IO_LED3);
			__led_off(IO_LED4);
			break;
		}
	}
}

void board_test(void)
{
	isink_init();

	simlnk_init(NULL, "SIM", 0, NULL);

	for (;;) {
		leds_mode(0); 
		isink_test();

		thinkos_sleep(100);

		leds_mode(1); 

		thinkos_sleep(100);

		leds_mode(2); 
		
		thinkos_sleep(100);

		leds_mode(3); 

		thinkos_sleep(100);

		leds_mode(4); 

		thinkos_sleep(100);
	
	}	
}

