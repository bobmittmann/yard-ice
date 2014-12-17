/* 
 * File:	 io.c
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
#include <sys/delay.h>
#include <stdint.h>

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

void leds_all_off(void)
{
	int i;

	for (i = 0; i < sizeof(led_io) / sizeof(struct stm32f_io); ++i)
		stm32f_gpio_clr(led_io[i].gpio, led_io[i].pin);
}

void leds_all_on(void)
{
	int i;

	for (i = 0; i < sizeof(led_io) / sizeof(struct stm32f_io); ++i)
		stm32f_gpio_set(led_io[i].gpio, led_io[i].pin);
}

void leds_init(void)
{
	int i;

	for (i = 0; i < sizeof(led_io) / sizeof(struct stm32f_io); ++i) {
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
 * CODEC I/O 
 * ----------------------------------------------------------------------
 */
#define TLV320RST STM32F_GPIOB, 10
#define TLV320CLK STM32F_GPIOA, 8

void codec_rst_lo(void)
{
	stm32f_gpio_clr(TLV320RST);
}

void codec_rst_hi(void)
{
	stm32f_gpio_set(TLV320RST);
}

void codec_io_init(void)
{
	stm32f_gpio_mode(TLV320CLK, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32f_gpio_mode(TLV320RST, OUTPUT, PUSH_PULL | SPEED_LOW);

	stm32f_gpio_clr(TLV320RST);
}

