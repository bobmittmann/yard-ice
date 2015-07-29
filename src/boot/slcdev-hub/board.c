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
#include <gdb.h>

#include "board.h"

void io_init(void)
{
	DCC_LOG1(LOG_TRACE, "clk[AHB]=%d", stm32f_ahb_hz);
	DCC_LOG1(LOG_TRACE, "clk[APB1]=%d", stm32f_apb1_hz);
	DCC_LOG1(LOG_TRACE, "clk[TIM1]=%d", stm32f_tim1_hz);
	DCC_LOG1(LOG_TRACE, "clk[APB2]=%d", stm32f_apb2_hz);
	DCC_LOG1(LOG_TRACE, "clk[TIM2]=%d", stm32f_tim2_hz);

	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOA);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOB);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOC);
}

void board_init(void)
{
}

void board_idle_tick(unsigned int cnt) 
{
}

void board_app_ready(void)
{
}

const struct mem_desc sram_desc = {
	.name = "RAM",
	.blk = {
		{ 0x10000000, BLK_RW, SZ_64K,  1 }, /*  CCM - Main Stack */
		{ 0x20000000, BLK_RO, SZ_4K,   1 }, /* Bootloader: 4KiB */
		{ 0x20001000, BLK_RW, SZ_4K,  27 }, /* Application: 108KiB */
		{ 0x2001c000, BLK_RW, SZ_16K,  1 }, /* SRAM 2: 16KiB */
		{ 0x00000000, 0, 0, 0 }
	}
}; 

const struct mem_desc flash_desc = {
	.name = "FLASH",
	.blk = {
		{ 0x08000000, BLK_RO, SZ_16K,  4 }, /* Bootloader */
		{ 0x08010000, BLK_RW, SZ_64K,  1 }, /* Application */
		{ 0x08020000, BLK_RW, SZ_128K, 7 }, /* Application */
		{ 0x00000000, 0, 0, 0 }
	}
}; 

const struct gdb_target board_gdb_target = {
	.name = "SLCDEV-HUB",
	.mem = {
		.ram = &sram_desc,
		.flash = &flash_desc
	},
	.app = {
		.start_addr = 0x08010000,
		.block_size = (64 + 7 * 128) * 1024
	}
};

