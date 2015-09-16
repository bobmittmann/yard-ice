/* 
 * File:	 board.h
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(c) 2003-2006 BORESTE (www.boreste.com). All Rights Reserved.
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

#ifndef __BOARD_H__
#define __BOARD_H__

#include <sys/stm32f.h>

/* -------------------------------------------------------------------------
 * Firelink Zigbee Coordinator
 * ------------------------------------------------------------------------- */


/* -------------------------------------------------------------------------
 * LDC Display
 * ------------------------------------------------------------------------- */

#define LCD_GPIO       STM32_GPIOJ

#define LCD_D0         STM32_GPIOJ, 0
#define LCD_D1         STM32_GPIOJ, 1
#define LCD_D2         STM32_GPIOJ, 2
#define LCD_D3         STM32_GPIOJ, 3
#define LCD_D4         STM32_GPIOJ, 4
#define LCD_D5         STM32_GPIOJ, 5
#define LCD_D6         STM32_GPIOJ, 6
#define LCD_D7         STM32_GPIOJ, 7

#define LCD_BACKLIGHT  STM32_GPIOJ, 8
#define LCD_POWER      STM32_GPIOJ, 9
#define LCD_EN         STM32_GPIOJ, 10
#define LCD_RNW        STM32_GPIOJ, 11
#define LCD_NRE        STM32_GPIOJ, 12
#define LCD_RS         STM32_GPIOJ, 13

/* -------------------------------------------------------------------------
 * Buzzer
 * ------------------------------------------------------------------------- */

#define BUZZER         STM32_GPIOB, 7

/* -------------------------------------------------------------------------
 * Zigbee Module I/O
 * ------------------------------------------------------------------------- */

#define UART2_TX       STM32_GPIOD, 5
#define UART2_RX       STM32_GPIOD, 6
#define UART2_CTS      STM32_GPIOD, 3
#define UART2_RTS      STM32_GPIOD, 4

/* -------------------------------------------------------------------------
 * Debug
 * ------------------------------------------------------------------------- */

#define LED1           STM32_GPIOD, 6
#define LED2           STM32_GPIOD, 5

/* -------------------------------------------------------------------------
 * Lattice FPGA (iCE40LP384) connections
 * ------------------------------------------------------------------------- */

#define IO_ICE40_SPI_SCK  STM32_GPIOI, 1
#define IO_ICE40_SPI_SDO  STM32_GPIOI, 3
#define IO_ICE40_SPI_SDI  STM32_GPIOI, 2
#define IO_ICE40_SPI_SS   STM32_GPIOI, 0
#define ICE40_SPI         STM32F_SPI2
#define ICE40_CLK_SPI     STM32_CLK_SPI2
#define ICE4_SPI_AF       GPIO_AF5

#define IO_ICE40_CRESET   STM32_GPIOE, 15
#define IO_ICE40_CDONE    STM32_GPIOH, 8


#define IO_RS485_TX       STM32_GPIOF, 7
#define IO_RS485_RX       STM32_GPIOF, 6
#define IO_RS485_MODE     STM32_GPIOF, 11

#define IO_RS485_USART    STM32_UART7

#define FLASH_BLK_FS_OFFS     0x00020000
#define FLASH_BLK_FS_SIZE     (256 * 1024)

/* low level led on/off functions */
static inline void __led_on(struct stm32_gpio *__gpio, int __pin) {
	stm32_gpio_clr(__gpio, __pin);
}

static inline void __led_off(struct stm32_gpio *__gpio, int __pin) {
	stm32_gpio_set(__gpio, __pin);
}

#ifdef __cplusplus
extern "C" {
#endif

struct lcd_dev * lcd20x4_init(void);

void lcd_puts(struct lcd_dev * dev, char * s);

void lcd_putc(struct lcd_dev * dev, int c);

void lcd_clear(struct lcd_dev * dev);

void lcd_on(struct lcd_dev * dev);

void lcd_off(struct lcd_dev * dev);

void lcd_set_pos(struct lcd_dev * dev, unsigned int lin, unsigned int col);

void lcd_cursor_on(struct lcd_dev * dev, bool blink);

void lcd_cursor_off(struct lcd_dev * dev);

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_H__ */
