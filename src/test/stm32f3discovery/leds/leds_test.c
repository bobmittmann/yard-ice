/* 
 * File:	 leds_test.c
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


#include <stdlib.h>
#include <sys/delay.h>
#include <sys/stm32f.h>

/* GPIO pin description */ 
struct stm32f_io {
	struct stm32_gpio * gpio;
	uint8_t pin;
};

/* ----------------------------------------------------------------------
 * LEDs 
 * ----------------------------------------------------------------------
 */

const struct stm32f_io led_io[] = {
	{ STM32_GPIOE,  9 }, /* LED3 */
	{ STM32_GPIOE, 10 }, /* LED5 */
	{ STM32_GPIOE, 11 }, /* LED7 */
	{ STM32_GPIOE, 12 }, /* LED9 */
	{ STM32_GPIOE, 13 }, /* LED10 */
	{ STM32_GPIOE, 14 }, /* LED8 */
	{ STM32_GPIOE, 15 }, /* LED6 */
	{ STM32_GPIOE,  8 }, /* LED4 */
};

#define LED_COUNT (sizeof(led_io) / sizeof(struct stm32f_io))

void led_on(int id)
{
	stm32_gpio_set(led_io[id].gpio, led_io[id].pin);
}

void led_off(int id)
{
	stm32_gpio_clr(led_io[id].gpio, led_io[id].pin);
}

void leds_io_init(void)
{
	int i;

	/* Enable GPIO clock */
	stm32_gpio_clock_en(STM32_GPIOE);

	for (i = 0; i < LED_COUNT; ++i) {
		/* configure GPIO as output */
		stm32_gpio_mode(led_io[i].gpio, led_io[i].pin,
						 OUTPUT, PUSH_PULL | SPEED_LOW);
	}
}

int main(int argc, char ** argv)
{
	int i;

	/* calibrate usecond delay loop */
	cm3_udelay_calibrate();

	/* initialize leds IO */
	leds_io_init();

	for (i = 0; ; ++i) {
		led_off((i - 2) & 0x7);
		led_on(i & 0x7);
		/* wait 100 ms */
		udelay(100000);
	}

	return 0;
}

