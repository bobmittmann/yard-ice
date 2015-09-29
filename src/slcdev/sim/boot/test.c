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
#include <sys/delay.h>
#include <crc.h>
#include <fixpt.h>

#include "board.h"

#define PWM_FREQ 100000

static void isink_init(void)
{
	struct stm32f_tim * tim = STM32_TIM2;
	struct stm32f_dac * dac = STM32_DAC;
	uint32_t div;
	uint32_t pre;
	uint32_t n;


	/* Negative volatge supply */
	stm32_gpio_mode(IO_VNEG_SW, ALT_FUNC, PUSH_PULL | SPEED_MED);
	stm32_gpio_af(IO_VNEG_SW, GPIO_AF1);

	/* Current rate control */
	stm32_gpio_mode(IO_IRATE, ANALOG, 0);

	/* Current sink */
	stm32_gpio_mode(IO_SINK1, OUTPUT, PUSH_PULL | SPEED_MED);
	stm32_gpio_mode(IO_SINK2, OUTPUT, PUSH_PULL | SPEED_MED);
	stm32_gpio_mode(IO_SINK3, OUTPUT, PUSH_PULL | SPEED_MED);
	stm32_gpio_mode(IO_SINK4, OUTPUT, PUSH_PULL | SPEED_MED);
	stm32_gpio_clr(IO_SINK1);
	stm32_gpio_clr(IO_SINK2);
	stm32_gpio_clr(IO_SINK3);
	stm32_gpio_clr(IO_SINK4);

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

char io_nm[][4] = { "GND", "VCC", "NC " };

static int ival(int k)
{
	uint32_t r40 = Q16(66.500);
	uint32_t r30 = Q16(44.200);
	uint32_t r35 = Q16(39.200);
	uint32_t r39 = Q16(24.000);
	uint32_t r32 =  Q16(6.190);
	uint32_t ra;
	uint32_t rb;
	uint32_t r[4] = { r40, r30, r35, r39 };
	int m[4];
	int j;
	uint32_t v;
	uint32_t i;
	int val;

	val = k;
	m[3] = val / 27;
	val -= m[3] * 27;
	m[2] = val / 9;
	val -= m[2] * 9;
	m[1] = val / 3;
	val -= m[1] * 3;
	m[0] = val;

	ra = r32;
	for (j = 0; j < 4; ++j) {
		if (m[j] == 0)
			ra = Q16_DIV(Q16_MUL(ra, r[j]), (ra + r[j]));
	}

	/* vcc connected equivalent resistor */
	rb = UINT32_MAX;
	for (j = 0; j < 4; ++j)  {
		if (m[j] == 1) {
			if (rb == UINT32_MAX)
				rb = r[j];
			else
				rb = Q16_DIV(Q16_MUL(rb, r[j]), (rb + r[j]));
		}
	}

	DCC_LOG2(LOG_JABBER, "ra=%d rb=%d", ra, rb);
	if (rb == UINT32_MAX)
		v = 0;
	else
		v = Q16_DIV(Q16_MUL(Q16(3.3), ra), (ra + rb));
	i = ((v * 1000) + 500) / Q16(4.99);
	v = ((v * 1000) + 500) / Q16(1.0);

	DCC_LOG7(LOG_TRACE, "%2d: %s %s %s %s -> %4d[mA] %4d[mV]", k, 
			 io_nm[m[3]], io_nm[m[2]], io_nm[m[1]], io_nm[m[0]], i, v);

	return i;
}

static void isink(int val)
{
	int j0;
	int j1;
	int j2;
	int j3;

	j3 = val / 27;
	val -= j3 * 27;
	j2 = val / 9;
	val -= j2 * 9;
	j1 = val / 3;
	val -= j1 * 3;
	j0 = val;

//	DCC_LOG4(LOG_TRACE, "%d %d %d %d", j3, j2, j1, j0);

	if (j0 == 2) {
		stm32_gpio_mode(IO_SINK1, OUTPUT, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_set(IO_SINK1);
	} else {
		stm32_gpio_mode(IO_SINK1, OUTPUT, PUSH_PULL | SPEED_HIGH);
		if (j0 == 1)
			stm32_gpio_set(IO_SINK1);
		else
			stm32_gpio_clr(IO_SINK1);
	}

	if (j1 == 2) {
		stm32_gpio_mode(IO_SINK2, OUTPUT, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_set(IO_SINK2);
	} else {
		stm32_gpio_mode(IO_SINK2, OUTPUT, PUSH_PULL | SPEED_HIGH);
		if (j1 == 1)
			stm32_gpio_set(IO_SINK2);
		else
			stm32_gpio_clr(IO_SINK2);
	}

	if (j2 == 2) {
		stm32_gpio_mode(IO_SINK3, OUTPUT, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_set(IO_SINK3);
	} else {
		stm32_gpio_mode(IO_SINK3, OUTPUT, PUSH_PULL | SPEED_HIGH);
		if (j2 == 1)
			stm32_gpio_set(IO_SINK3);
		else
			stm32_gpio_clr(IO_SINK3);
	}

	if (j3 == 2) {
		stm32_gpio_mode(IO_SINK4, OUTPUT, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_set(IO_SINK4);
	} else {
		stm32_gpio_mode(IO_SINK4, OUTPUT, PUSH_PULL | SPEED_HIGH);
		if (j3 == 1)
			stm32_gpio_set(IO_SINK4);
		else
			stm32_gpio_clr(IO_SINK4);
	}
}

static void irarte(int val)
{
	struct stm32f_dac * dac = STM32_DAC;

	dac->dhr12r2 = 4095 * val / 4;
}

static void isink_test(void)
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

static void leds_mode(int i) 
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

void __board_test(void)
{
	isink_init();

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

void __attribute__((noreturn)) app_default(void)
{
	int i;

	isink_init();

	irarte(2);

	cm3_cpsid_i();

	for (i = 0; i < 81; ++i) {
		ival(i);
	}

	for (;;) {
		udelay(50000);
		for (i = 1; i < 81; ++i) {
			isink(i);
			udelay(500);
			isink(0);
			udelay(250);
		}
	}	
}

