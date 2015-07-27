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

#include "board.h"
#include <sys/delay.h>
#include <gdb.h>

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
	{ LED3_IO }, /* LED3 */
	{ LED5_IO }, /* LED5 */
	{ LED7_IO }, /* LED7 */
	{ LED9_IO }, /* LED9 */
	{ LED10_IO }, /* LED10 */
	{ LED8_IO }, /* LED8 */
	{ LED6_IO }, /* LED6 */
	{ LED4_IO }, /* LED4 */
};

#define LED_COUNT (sizeof(led_io) / sizeof(struct stm32f_io))

void led_on(unsigned int id)
{
	__led_on(led_io[id].gpio, led_io[id].pin);
}

void led_off(unsigned int id)
{
	__led_off(led_io[id].gpio, led_io[id].pin);
}

void led_toggle(unsigned int id)
{
	__led_toggle(led_io[id].gpio, led_io[id].pin);
}

void io_init(void)
{
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOA);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOE);

	stm32_gpio_mode(LED3_IO, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32_gpio_mode(LED4_IO, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32_gpio_mode(LED5_IO, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32_gpio_mode(LED6_IO, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32_gpio_mode(LED7_IO, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32_gpio_mode(LED8_IO, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32_gpio_mode(LED9_IO, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32_gpio_mode(LED10_IO, OUTPUT, PUSH_PULL | SPEED_LOW);

	stm32_gpio_mode(SW_B1_IO, INPUT, SPEED_LOW);
}

void board_init(void)
{
#if 0
	int i;

	for (i = 0; i < 16; ++i) {
		led_off((i - 2) & 0x7);
		led_on(i & 0x7);
		/* wait 100 ms */
		udelay(100000);
	}

	led_off((i - 2) & 0x7);
	udelay(100000);
	led_off((i - 1) & 0x7);
#endif
}

void board_tick(unsigned int cnt) 
{
	led_off((cnt - 1) & 0x1);
	led_on(cnt & 0x1);
}

const struct mem_desc sram_desc = {
	.name = "RAM",
	.blk = {
		{ 0x10000000, BLK_RW, SZ_8K,  1 }, /*  CCM - Main Stack */
		{ 0x20000000, BLK_RO, SZ_4K,  1 },  /* Bootloader: 4KiB */
		{ 0x20001000, BLK_RW, SZ_4K,  11 }, /* Application: 44KiB */
		{ 0x00000000, 0, 0, 0 }
	}
}; 

const struct mem_desc flash_desc = {
	.name = "FLASH",
	.blk = {
		{ 0x08000000, BLK_RO, SZ_2K,  24 }, /* Bootloader */
		{ 0x0800c000, BLK_RW, SZ_8K, 104 }, /* Application */
		{ 0x00000000, 0, 0, 0 }
	}
}; 

const struct gdb_target board_gdb_target = {
	.name = "STM32F3Discovery",
	.ram = &sram_desc,
	.flash = &flash_desc
};

