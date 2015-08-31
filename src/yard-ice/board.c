/* 
 * Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
 *
 * This file is part of the YARD-ICE.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You can receive a copy of the GNU Lesser General Public License from 
 * http://www.gnu.org/
 */

/** 
 * @file stm32f-io.c
 * @brief 
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 


#include <stdint.h>
#include <sys/stm32f.h>
#define __THINKOS_DMON__
#include <thinkos_dmon.h>

#define RELAY_GPIO STM32_GPIOB, 9
#define PWR_EN_GPIO STM32_GPIOD, 12 
#define PWR_MON_GPIO STM32_GPIOD, 11

static void io_init(void)
{
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOA);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOB);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOC);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOD);

	/* - Relay ------------------------------------------------------------*/
	stm32_gpio_mode(RELAY_GPIO, OUTPUT, SPEED_LOW);
	stm32_gpio_clr(RELAY_GPIO);

	/* - External Power ---------------------------------------------------*/
	stm32_gpio_mode(PWR_EN_GPIO, OUTPUT, SPEED_LOW);
	stm32_gpio_clr(PWR_EN_GPIO);
	stm32_gpio_mode(PWR_MON_GPIO, INPUT, SPEED_LOW | PULL_UP);

}

void relay_on(void)
{
	stm32_gpio_set(RELAY_GPIO);
}

void relay_off(void)
{
	stm32_gpio_clr(RELAY_GPIO);
}

int relay_stat(void)
{
	return stm32_gpio_stat(RELAY_GPIO);
}

void ext_pwr_on(void)
{
	stm32_gpio_set(PWR_EN_GPIO);
}

void ext_pwr_off(void)
{
	stm32_gpio_clr(PWR_EN_GPIO);
}

int ext_pwr_stat(void)
{
	return stm32_gpio_stat(PWR_EN_GPIO);
}

int ext_pwr_mon(void)
{
	return stm32_gpio_stat(PWR_MON_GPIO) ? 0 : 1;
}

bool board_init(void)
{
	io_init();

	return true;
}

void board_softreset(void)
{
	struct stm32_rcc * rcc = STM32_RCC;

	/* Reset all peripherals except USB_OTG and GPIOA */
	rcc->ahb1rstr = ~(1 << RCC_GPIOA); 
	rcc->ahb2rstr = ~(1 << RCC_OTGFS);
	rcc->apb1rstr = ~(0);
	rcc->apb2rstr = ~(0);
	/* clear reset bits */
	rcc->ahb1rstr = 0;
	rcc->ahb2rstr = 0;
	rcc->apb1rstr = 0;
	rcc->apb2rstr = 0;
	/* disable all peripherals clock sources except USB_OTG and GPIOA */
	rcc->ahb1enr = (1 << RCC_GPIOA); 
	rcc->ahb2enr = (1 << RCC_OTGFS);
	rcc->apb1enr = 0;
	rcc->apb2enr = 0;

	/* reinitialize IO's */
	io_init();

	/* Enable USB OTG FS interrupts */
	cm3_irq_enable(STM32F_IRQ_OTG_FS);
}

bool board_autoboot(uint32_t tick)
{
	return true;
}

void board_on_appload(void)
{
}

bool board_configure(struct dmon_comm * comm)
{
	return true;
}

extern const uint8_t otg_xflash_pic[];
extern const unsigned int sizeof_otg_xflash_pic;

struct magic {
	uint32_t cnt;
	struct {
		uint32_t addr;
		uint32_t mask;
		uint32_t comp;
	} rec[];
};

int usb_xflash(uint32_t offs, uint32_t len);

void board_upgrade(struct dmon_comm * comm)
{
	cm3_cpsid_f();
	usb_xflash(0, 256 * 1024);
}

const struct mem_desc sram_desc = {
	.name = "RAM",
	.blk = {
		{ 0x20000000, BLK_RW, SZ_16K,  7 }, /* SRAM 1: 112KiB */
		{ 0x2001c000, BLK_RW, SZ_16K,  1 }, /* SRAM 2: 16KiB */
		{ 0x00000000, 0, 0, 0 }
	}
}; 

const struct mem_desc flash_desc = {
	.name = "FLASH",
	.blk = {
		{ 0x08000000, BLK_RO, SZ_16K,  4 }, /* Application */
		{ 0x08010000, BLK_RW, SZ_64K,  1 }, /* Application */
		{ 0x08020000, BLK_RW, SZ_128K, 3 }, /* Application */
		{ 0x00000000, 0, 0, 0 }
	}
}; 

void yard_ice(void);

const struct thinkos_board this_board = {
	.name = "YARD-ICE",
	.hw_ver = {
		.major = 0,
		.minor = 1,
	},
	.sw_ver = {
		.major = 0,
		.minor = 1,
	},
	.memory = {
		.ram = &sram_desc,
		.flash = &flash_desc
	},
	.application = {
		.start_addr = 0x08000000,
		.block_size = 256 * 1024
	},
	.init = board_init,
	.softreset = board_softreset,
	.autoboot = board_autoboot,
	.configure = board_configure,
	.upgrade = board_upgrade,
	.on_appload = board_on_appload
};

