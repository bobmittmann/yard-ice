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
#include "lattice.h"

#define LCD_SET_DDRAM_ADDR (1 << 7)

#define LCD_FUNCTION_SET   (1 << 5)
#define LCD_DL             (1 << 4)
#define LCD_N              (1 << 3)
#define LCD_F              (1 << 2)

#define	LCD_ON_OFF_CTRL    (1 << 3)
#define LCD_DISPLAY        (1 << 2)
#define LCD_CURSOR         (1 << 1)
#define LCD_BLINK          (1 << 0)

#define	LCD_ENTRY_MODE_SET (1 << 2)

#define LCD_ID             (1 << 1)
#define LCD_SH             (1 << 0)

#define	LCD_DISPLAY_CLEAR  (1 << 0)

static void lcd_out(unsigned int data)
{
	uint32_t mask = stm32_gpio_rd(LCD_GPIO) & ~0xff;
	stm32_gpio_wr(LCD_GPIO, mask | data);
	/* EN pulse */
	udelay(1);
	stm32_gpio_set(LCD_EN);
	udelay(1);
	stm32_gpio_clr(LCD_EN);
};

static void lcd_wr(unsigned int data)
{
	/* data out */
	stm32_gpio_set(LCD_RS);
	lcd_out(data);
}

static void lcd_ctrl(unsigned int data)
{
	/* data out */
	stm32_gpio_clr(LCD_RS);
	lcd_out(data);
}

void firelink_lcd_puts(char * s)
{
	int c;

	while ((c = *s++) != '\0') {
		lcd_wr(c);
		udelay(37);
	}
}

void firelink_lcd_putc(int c)
{
	lcd_wr(c);
//	udelay(37);
}

void firelink_lcd_clear(void)
{
	lcd_ctrl(LCD_DISPLAY_CLEAR);
	udelay(1600);
}

void firelink_lcd_init(void)
{
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOJ);

	stm32_gpio_mode(LCD_POWER, OUTPUT, PUSH_PULL| SPEED_MED);
	stm32_gpio_clr(LCD_POWER);
	stm32_gpio_mode(LCD_BACKLIGHT, OUTPUT, PUSH_PULL| SPEED_MED);
	stm32_gpio_clr(LCD_BACKLIGHT);

	stm32_gpio_mode(LCD_EN, OUTPUT, PUSH_PULL| SPEED_MED);
	stm32_gpio_mode(LCD_RNW, OUTPUT, PUSH_PULL| SPEED_MED);
	stm32_gpio_mode(LCD_NRE, OUTPUT, PUSH_PULL| SPEED_MED);
	stm32_gpio_mode(LCD_RS, OUTPUT, PUSH_PULL| SPEED_MED);

	stm32_gpio_mode(LCD_D0, OUTPUT, PUSH_PULL| SPEED_MED);
	stm32_gpio_mode(LCD_D1, OUTPUT, PUSH_PULL| SPEED_MED);
	stm32_gpio_mode(LCD_D2, OUTPUT, PUSH_PULL| SPEED_MED);
	stm32_gpio_mode(LCD_D3, OUTPUT, PUSH_PULL| SPEED_MED);
	stm32_gpio_mode(LCD_D4, OUTPUT, PUSH_PULL| SPEED_MED);
	stm32_gpio_mode(LCD_D5, OUTPUT, PUSH_PULL| SPEED_MED);
	stm32_gpio_mode(LCD_D6, OUTPUT, PUSH_PULL| SPEED_MED);
	stm32_gpio_mode(LCD_D7, OUTPUT, PUSH_PULL| SPEED_MED);

	stm32_gpio_clr(LCD_EN);
	stm32_gpio_clr(LCD_RNW);
	stm32_gpio_clr(LCD_RS);
	stm32_gpio_set(LCD_NRE);
	stm32_gpio_clr(LCD_D0);
	stm32_gpio_clr(LCD_D1);
	stm32_gpio_clr(LCD_D2);
	stm32_gpio_clr(LCD_D3);
	stm32_gpio_clr(LCD_D4);
	stm32_gpio_clr(LCD_D5);
	stm32_gpio_clr(LCD_D6);
	stm32_gpio_clr(LCD_D7);

	udelay(10);
	stm32_gpio_set(LCD_POWER);
	stm32_gpio_set(LCD_BACKLIGHT);
	udelay(10);
	/* function set */
	lcd_ctrl(LCD_FUNCTION_SET + LCD_DL);
	udelay(4100);
	/* function set */
	lcd_ctrl(LCD_FUNCTION_SET + LCD_DL);
	udelay(200);
	/* function set */
	lcd_ctrl(LCD_FUNCTION_SET + LCD_DL);
	udelay(50);
	/* function set */
	lcd_ctrl(LCD_FUNCTION_SET + LCD_DL + LCD_N);
	udelay(50);
	/* ON/OFF control */
	lcd_ctrl(LCD_ON_OFF_CTRL);
	udelay(50);
	/* Display clear */
	lcd_ctrl(LCD_DISPLAY_CLEAR);
	udelay(1520);
	/* Entry mode set */
	lcd_ctrl(LCD_ENTRY_MODE_SET + LCD_ID);
	udelay(50);
	/* ON/OFF control */
	lcd_ctrl(LCD_ON_OFF_CTRL + LCD_DISPLAY);
	udelay(50);
}

void io_init(void)
{
	DCC_LOG1(LOG_TRACE, "clk[AHB]=%d", stm32f_ahb_hz);
	DCC_LOG1(LOG_TRACE, "clk[APB1]=%d", stm32f_apb1_hz);
	DCC_LOG1(LOG_TRACE, "clk[TIM1]=%d", stm32f_tim1_hz);
	DCC_LOG1(LOG_TRACE, "clk[APB2]=%d", stm32f_apb2_hz);
	DCC_LOG1(LOG_TRACE, "clk[TIM2]=%d", stm32f_tim2_hz);

	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOB);
	stm32_gpio_mode(BUZZER, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32_gpio_clr(BUZZER);

}

extern const uint8_t ice40lp384_bin[];
extern const unsigned int sizeof_ice40lp384_bin;

void board_init(void)
{
	firelink_lcd_init();
	firelink_lcd_puts("ThinkOS 0.22        ");
	firelink_lcd_puts("Bootloader          ");
	firelink_lcd_puts("Debug Monitor       ");

    /* IO init */
    stm32_gpio_mode(IO_RS485_RX, INPUT, PULL_UP);

    stm32_gpio_mode(IO_RS485_TX, OUTPUT, PUSH_PULL | SPEED_LOW);
    stm32_gpio_set(IO_RS485_TX);

    stm32_gpio_mode(IO_RS485_MODE, OUTPUT, PUSH_PULL | SPEED_LOW);
    stm32_gpio_set(IO_RS485_MODE);

    lattice_ice40_configure(ice40lp384_bin, sizeof_ice40lp384_bin);
}

void board_idle_tick(unsigned int cnt) 
{
	if (cnt < 20)
		firelink_lcd_putc('.');
}

void board_app_ready(void)
{
}

void board_configure(struct dmon_comm * comm)
{
}

void board_bootloader_upgrade(void)
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
	.name = "Firelink",
	.mem = {
		.ram = &sram_desc,
		.flash = &flash_desc
	},
	.app = {
		.start_addr = 0x08010000,
		.block_size = (64 + 7 * 128) * 1024
	}
};

