/* 
 * File:	 gpio-test.c
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

#define RELAY_GPIO STM32F_GPIOB
#define RELAY_PORT 9

#define EXT_EN_GPIO STM32F_GPIOD
#define EXT_EN_PORT 12

void v_ext_init(void)
{
	struct stm32f_gpio * gpio = EXT_EN_GPIO;

	stm32f_gpio_clock_en(gpio);
	stm32f_gpio_mode(gpio, EXT_EN_PORT, OUTPUT, SPEED_LOW);
}

void v_ext_on(void)
{
	struct stm32f_gpio * gpio = EXT_EN_GPIO;

	gpio->bsrrl = GPIO_SET(EXT_EN_PORT);
}

void v_ext_off(void)
{
	struct stm32f_gpio * gpio = EXT_EN_GPIO;

	gpio->bsrrh = GPIO_RESET(EXT_EN_PORT);
}



void relay_init(void)
{
	struct stm32f_gpio * gpio = RELAY_GPIO;

	stm32f_gpio_clock_en(gpio);
	stm32f_gpio_mode(gpio, RELAY_PORT, OUTPUT, SPEED_LOW);
}

void relay_on(void)
{
	struct stm32f_gpio * gpio = RELAY_GPIO;

	gpio->bsrrl = GPIO_SET(RELAY_PORT);
}

void relay_off(void)
{
	struct stm32f_gpio * gpio = RELAY_GPIO;

	gpio->bsrrh = GPIO_RESET(RELAY_PORT);
}

void delay(unsigned int count)
{
	unsigned int i;

	for (i = 0; i < count; i++) {
		__NOP();
		__NOP();
		__NOP();
		__NOP();
	}
}

/* Hardware initialization */
void relay_test(void)
{
	printf("Relay Test!\r\n");
	relay_init();
	relay_on();
	delay(1000000);
	relay_off();
	delay(1000000);
	relay_on();
	delay(1000000);
	relay_off();
	delay(1000000);
	relay_on();
	delay(1000000);
	relay_off();
	delay(1000000);
}

#define STM32F_AHB_HZ 120000000

#ifndef CM3_SYSTICK_CLK_HZ
#error "CM3_SYSTICK_CLK_HZ undefined"
#endif

void SysTick_Handler(void)
{
	static int on = 0;

	if (on) {
		v_ext_off();
		on = 0;
	} else {
		v_ext_on();
		on = 1;
	}
}

void v_ext_test(void)
{
	int i;

	printf("V ext test... ");
	v_ext_init();

	for (i = 0; i < 500; i++) {
		v_ext_on();
		udelay(1000);
		v_ext_off();
		udelay(1000);
	}
	printf("done.\r\n");
}

int main(int argc, char ** argv)
{
	stdout = stm32f_usart_open(STM32F_UART5, 115200, SERIAL_8N1);
	cm3_udelay_calibrate();

	printf("\r\n");
	printf("Hello World!\r\n");

	relay_test();

	v_ext_test();

	return 0;
}

