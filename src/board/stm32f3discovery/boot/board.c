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

#define __THINKOS_DMON__
#include <thinkos_dmon.h>
#include "board.h"

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

#if 0
void led_toggle(unsigned int id)
{
	__led_toggle(led_io[id].gpio, led_io[id].pin);
}
#endif

static void io_init(void)
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

	stm32_gpio_mode(SW_B1_IO, INPUT, PULL_UP | SPEED_LOW);
}

bool board_init(void)
{
	io_init();

	led_on(0);

	cm3_irq_pri_set(STM32F_IRQ_USB_HP, MONITOR_PRIORITY);
	cm3_irq_pri_set(STM32F_IRQ_USB_LP, MONITOR_PRIORITY);

	return true;
}

void board_comm_irqen(void)
{
	/* Enable USB interrupts */
	cm3_irq_enable(STM32F_IRQ_USB_HP);
	cm3_irq_enable(STM32F_IRQ_USB_LP);
}

void board_softreset(void)
{
	struct stm32_rcc * rcc = STM32_RCC;

	/* Reset all peripherals except USB_OTG and GPIOA */
	rcc->ahbrstr = ~(1 << RCC_IOPA); 
	rcc->apb1rstr = ~(1 << RCC_USB);
	rcc->apb2rstr = ~(0);

	rcc->ahbrstr = 0;
	rcc->apb1rstr = 0;
	rcc->apb2rstr = 0;

	/* disable all peripherals clock sources except USB_OTG, GPIOA */
	rcc->ahbenr = (1 << RCC_IOPA); 
	rcc->apb1enr = (1 << RCC_USB);
	rcc->apb2enr = 0;

	/* reinitialize IO's */
	io_init();

	/* restore USB interrupt priority */
	cm3_irq_pri_set(STM32F_IRQ_USB_HP, MONITOR_PRIORITY);
	cm3_irq_pri_set(STM32F_IRQ_USB_LP, MONITOR_PRIORITY);

	/* Enable USB interrupts */
	board_comm_irqen();
}

bool board_autoboot(uint32_t tick)
{
	led_off(~(tick - 1) & 0x7);
	led_on(~tick & 0x7);

	/* Time window autoboot */
	return (tick < 40) ? false : true;
}

void board_on_appload(void)
{
	int i;

	for (i = 0; i < LED_COUNT; ++i) 
		led_off(i);
}

void board_upgrade(struct dmon_comm * comm)
{
}

bool board_configure(struct dmon_comm * comm)
{
	return true;
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
		{ 0x08000000, BLK_RO, SZ_2K,  20 }, /* Bootloader: 40 KiB */
		{ 0x0800a000, BLK_RW, SZ_2K, 108 }, /* Application:  */
		{ 0x00000000, 0, 0, 0 }
	}
}; 

const struct thinkos_board this_board = {
	.name = "STM32F3Discovery",
	.hw_ver = {
		.major = 0,
		.minor = 1,
	},
	.sw_ver = {
		.major = 0,
		.minor = 2,
	},
	.memory = {
		.ram = &sram_desc,
		.flash = &flash_desc
	},
	.application = {
		.start_addr = 0x0800a000,
		.block_size = 2048 * 108
	},

	.init = board_init,
	.softreset = board_softreset,
	.autoboot = board_autoboot,
	.configure = board_configure,
	.upgrade = board_upgrade,
	.on_appload = board_on_appload,
	.comm_irqen = board_comm_irqen
};


