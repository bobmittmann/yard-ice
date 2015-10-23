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
#define __THINKOS_DMON__
#include <thinkos_dmon.h>
#include <gdb.h>

#include "board.h"
#include "lattice.h"

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
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOD);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOE);

    stm32_gpio_mode(IO_RS485_RX, INPUT, PULL_UP);

    stm32_gpio_mode(IO_RS485_TX, OUTPUT, PUSH_PULL | SPEED_LOW);
    stm32_gpio_set(IO_RS485_TX);

    stm32_gpio_mode(IO_RS485_MODE, OUTPUT, PUSH_PULL | SPEED_LOW);
    stm32_gpio_set(IO_RS485_MODE);
}

extern const uint8_t ice40lp384_bin[];
extern const unsigned int sizeof_ice40lp384_bin;

bool board_init(void)
{
	io_init();

    lattice_ice40_configure(ice40lp384_bin, sizeof_ice40lp384_bin);

	return true;
}

void board_comm_irqen(void)
{
	/* Enable USB OTG FS interrupts */
	cm3_irq_enable(STM32F_IRQ_OTG_FS);
}

void board_softreset(void)
{
	struct stm32_rcc * rcc = STM32_RCC;

	/* Reset all peripherals except USB_OTG and GPIOA */
	rcc->ahb1rstr = ~(1 << RCC_GPIOA); 
	rcc->ahb2rstr = ~(1 << RCC_OTGFS);
	rcc->ahb3rstr = ~(0);
	rcc->apb1rstr = ~(0);
	rcc->apb2rstr = ~(0);


	rcc->ahb1rstr = 0;
	rcc->ahb2rstr = 0;
	rcc->ahb3rstr = 0;
	rcc->apb1rstr = 0;
	rcc->apb2rstr = 0;

	/* disable all peripherals clock sources except USB_OTG and GPIOA */
	rcc->ahb1enr = (1 << RCC_CCMDATARAM) | (1 << RCC_GPIOA); 
	rcc->ahb2enr = (1 << RCC_OTGFS);
	rcc->ahb3enr = 0;
	rcc->apb1enr = 0;
	rcc->apb2enr = 0;

	/* reinitialize IO's */
	io_init();

	/* Enable USB OTG FS interrupts */
	board_comm_irqen();
}

bool board_autoboot(uint32_t tick)
{
	/* Time window autoboot */
//	return (tick < 40) ? false : true;
	return (tick == 8) ? true : false;
}

void board_on_appload(void)
{
}

struct board_cfg {
	uint32_t magic;
	uint32_t mstp_addr;
	uint32_t ip_addr;
};

#define CFG_MAGIC 0x01020304

#define IP4_ADDR1(A) ((int)(((uint32_t)(A)) & 0xff))
#define IP4_ADDR2(A) ((int)((((uint32_t)(A)) >> 8) & 0xff))
#define IP4_ADDR3(A) ((int)((((uint32_t)(A)) >> 16) & 0xff))
#define IP4_ADDR4(A) ((int)((((uint32_t)(A)) >> 24) & 0xff))
#define IP4_ADDR(A, B, C, D) ((((A) & 0xff) << 0) | \
		(((B) & 0xff) << 8) | (((C) & 0xff) << 16) | (((D) & 0xff) << 24))

bool board_configure(struct dmon_comm * comm)
{
	uint32_t offs = 0x10000 - 0x0100;
	struct board_cfg * cfg = (struct board_cfg *)(0x08000000 + offs);
	struct board_cfg buf;
	int n;
	int ip[4];

	if (cfg->magic == CFG_MAGIC) {
		dmprintf(comm, "MS/TP address: %d\r\n", cfg->mstp_addr);
		dmprintf(comm, "IP address: %d.%d.%d.%d\r\n", 
				 IP4_ADDR1(cfg->ip_addr),
				 IP4_ADDR2(cfg->ip_addr),
				 IP4_ADDR3(cfg->ip_addr),
				 IP4_ADDR4(cfg->ip_addr));
		return false;
	}
	do {
		dmprintf(comm, "MS/TP address > ");
		n = dmscanf(comm, "%d", &buf.mstp_addr);
		if (n < 0)
			return false;
	} while (n != 1);
	do {
		dmprintf(comm, "IP address > ");
		n = dmscanf(comm, "%3d.%3d.%3d.%3d", &ip[0], &ip[1], &ip[2], &ip[3]);
		if (n < 0)
			return false;
	} while (n != 4);

	buf.ip_addr = IP4_ADDR(ip[0], ip[1], ip[2], ip[3]);
	buf.magic = CFG_MAGIC;

	stm32_flash_write(offs, &buf, sizeof(buf));

	dmprintf(comm, "Configuation saved.\r\n");

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

const struct magic bootloader_magic = {
	.cnt = 4,
	.rec = {
		{  0x08000000, 0xffffffff, 0x10010000 },
		{  0x08000004, 0xffff0000, 0x08000000 },
		{  0x08000008, 0xffff0000, 0x08000000 },
		{  0x0800000c, 0xffff0000, 0x08000000 }
	}
};

void board_upgrade(struct dmon_comm * comm)
{
	uint32_t * xflash_code = (uint32_t *)(0x20001000);
	int (* xflash_ram)(uint32_t, uint32_t, const struct magic *) = 
		((void *)xflash_code) + 1;

	cm3_cpsid_f();
	__thinkos_memcpy(xflash_code, otg_xflash_pic, sizeof_otg_xflash_pic);
	xflash_ram(0, 65536, &bootloader_magic);
}

const struct mem_desc sram_desc = {
	.name = "RAM",
	.blk = {
		{ 0x10000000, BLK_RW, SZ_64K,  1 }, /*  CCM - Main Stack */

		{ 0x20000000, BLK_RO, SZ_4K,   1 }, /* Bootloader: 4KiB */
		{ 0x20001000, BLK_RW, SZ_4K,  27 }, /* Application: 108KiB */
		{ 0x2001c000, BLK_RW, SZ_16K,  1 }, /* SRAM 2: 16KiB */

		{ 0x22000000, BLK_RO, SZ_512K, 1 }, /* Bootloader - bitband */
		{ 0x22080000, BLK_RW, SZ_512K, 27}, /* Application - bitband */
		{ 0x22e00000, BLK_RW, SZ_2M,   1 }, /* SRAM 2 - bitband */

#ifdef ENABLE_PRIPHERAL_MEM
		{ 0x40000000, BLK_RO, SZ_1M,   1 },  /* Peripheral */
		{ 0x42000000, BLK_RO, SZ_128M, 1 },  /* Peripheral - bitband */
#endif

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

const struct thinkos_board this_board = {
	.name = "SLCDEV-HUB",
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
		.start_addr = 0x08010000,
		.block_size = (64 + 7 * 128) * 1024
	},
	.init = board_init,
	.softreset = board_softreset,
	.autoboot = board_autoboot,
	.configure = board_configure,
	.upgrade = board_upgrade,
	.on_appload = board_on_appload,
	.comm_irqen = board_comm_irqen
};

