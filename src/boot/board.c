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

#include <sys/stm32f.h>
#include <sys/delay.h>
#include <sys/dcclog.h>
#include <stdint.h>
#include <sys/stm32f.h>
#define __THINKOS_DBGMON__
#include <thinkos_dmon.h>

#include "board.h"
#include "version.h"

static void io_init(void)
{
	DCC_LOG1(LOG_TRACE, "clk[AHB]=%d", stm32f_ahb_hz);
	DCC_LOG1(LOG_TRACE, "clk[APB1]=%d", stm32f_apb1_hz);
	DCC_LOG1(LOG_TRACE, "clk[TIM1]=%d", stm32f_tim1_hz);
	DCC_LOG1(LOG_TRACE, "clk[APB2]=%d", stm32f_apb2_hz);
	DCC_LOG1(LOG_TRACE, "clk[TIM2]=%d", stm32f_tim2_hz);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOA);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOC);

#if 0
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOD);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOB);

	/* - Relay ------------------------------------------------------------*/
	stm32_gpio_mode(IO_RELAY, OUTPUT, SPEED_LOW);
	stm32_gpio_clr(IO_RELAY);

	/* - External Power ---------------------------------------------------*/
	stm32_gpio_mode(IO_PWR_EN, OUTPUT, SPEED_LOW);
	stm32_gpio_clr(IO_PWR_EN);
	stm32_gpio_mode(IO_PWR_MON, INPUT, SPEED_LOW | PULL_UP);
	/* - Debug UART ---------------------------------------------------*/
	stm32_gpio_mode(IO_UART5_TX, OUTPUT, SPEED_LOW);
	stm32_gpio_clr(IO_UART5_TX);
#endif
	stm32_gpio_mode(IO_UART5_TX, INPUT, SPEED_LOW | PULL_UP);

	/* Adjust USB OTG FS interrupts priority */
	cm3_irq_pri_set(STM32F_IRQ_OTG_FS, MONITOR_PRIORITY);
	/* Enable USB OTG FS interrupts */
	cm3_irq_enable(STM32F_IRQ_OTG_FS);
}

bool board_init(void)
{
	io_init();

	return true;
}

void board_softreset(void)
{
	struct stm32_rcc * rcc = STM32_RCC;

	DCC_LOG(LOG_TRACE, "...");

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

	io_init();
}

bool board_autoboot(uint32_t tick)
{
#if 0
	if (tick < 2) {
		if (tick & 1)
			stm32_gpio_clr(IO_RELAY);
		else
			stm32_gpio_set(IO_RELAY);
	}
	if (tick == (3 * 8)) 
		return true;
#endif

//	DCC_LOG1(LOG_TRACE, "tick=%d", tick);
	return stm32_gpio_stat(IO_UART5_TX) ? true : false;
}

void board_on_appload(void)
{
}

bool board_configure(struct dmon_comm * comm)
{
	return true;
}

void board_upgrade(struct dmon_comm * comm)
{
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
		{ 0x08000000, BLK_RO, SZ_16K,  1 }, /* Boot loader */
		{ 0x08004000, BLK_RW, SZ_16K,  3 }, /* Application */
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
		.minor = 2,
	},
	.sw_ver = {
		.major = VERSION_MAJOR,
		.minor = VERSION_MINOR,
	},
	.memory = {
		.ram = &sram_desc,
		.flash = &flash_desc
	},
	.application = {
		.start_addr = 0x08020000,
		.block_size = 256 * 1024
	},
	.init = board_init,
	.softreset = board_softreset,
	.autoboot = board_autoboot,
	.configure = board_configure,
	.upgrade = board_upgrade,
	.on_appload = board_on_appload
};

