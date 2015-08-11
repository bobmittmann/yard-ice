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

#define IO_LCD_D0         STM32_GPIOJ, 0
#define IO_LCD_D1         STM32_GPIOJ, 1
#define IO_LCD_D2         STM32_GPIOJ, 2
#define IO_LCD_D3         STM32_GPIOJ, 3
#define IO_LCD_D4         STM32_GPIOJ, 4
#define IO_LCD_D5         STM32_GPIOJ, 5
#define IO_LCD_D6         STM32_GPIOJ, 6
#define IO_LCD_D7         STM32_GPIOJ, 7

#define IO_LCD_BACKLIGHT  STM32_GPIOJ, 8
#define IO_LCD_POWER      STM32_GPIOJ, 9
#define IO_LCD_EN         STM32_GPIOJ, 10
#define IO_LCD_RNW        STM32_GPIOJ, 11
#define IO_LCD_NRE        STM32_GPIOJ, 12
#define IO_LCD_RS         STM32_GPIOJ, 13

/* -------------------------------------------------------------------------
 * Buzzer
 * ------------------------------------------------------------------------- */

#define IO_BUZZER         STM32_GPIOB, 7

/* -------------------------------------------------------------------------
 * LEDs
 * ------------------------------------------------------------------------- */

#define IO_LED_TRBL       STM32_GPIOC, 6
#define IO_LED_ALRM       STM32_GPIOI, 12
#define IO_LED_AC_PWR     STM32_GPIOC, 7
#define IO_LED_SPR        STM32_GPIOE, 10
#define IO_LED_SLNC       STM32_GPIOB, 11

/* -------------------------------------------------------------------------
 * Panel Switches
 * ------------------------------------------------------------------------- */

#define IO_SW_NAV0        STM32_GPIOF, 0
#define IO_SW_NAV1        STM32_GPIOF, 1
#define IO_SW_NAV2        STM32_GPIOA, 12
#define IO_SW_NAV3        STM32_GPIOB, 10
#define IO_SW_NAV4        STM32_GPIOB, 9

/* -------------------------------------------------------------------------
 * Zigbee Module I/O
 * ------------------------------------------------------------------------- */

#define IO_UART2_TX       STM32_GPIOD, 5
#define IO_UART2_RX       STM32_GPIOD, 6
#define IO_UART2_CTS      STM32_GPIOD, 3
#define IO_UART2_RTS      STM32_GPIOD, 4

/* -------------------------------------------------------------------------
 * Debug
 * ------------------------------------------------------------------------- */



/* -------------------------------------------------------------------------
 * Lattice FPGA (iCE40LP384) connections
 * ------------------------------------------------------------------------- */

#define IO_ICE40_SPI_SCK  STM32_GPIOI, 1
#define IO_ICE40_SPI_SDO  STM32_GPIOI, 3
#define IO_ICE40_SPI_SDI  STM32_GPIOI, 2
#define IO_ICE40_SPI_SS   STM32_GPIOI, 0
#define ICE40_SPI         STM32F_SPI2
#define ICE40_CLK_SPI     STM32_CLK_SPI2

#define IO_ICE40_CRESET   STM32_GPIOE, 15
#define IO_ICE40_CDONE    STM32_GPIOH, 8


#define IO_RS485_TX       STM32_GPIOF, 7
#define IO_RS485_RX       STM32_GPIOF, 6
#define IO_RS485_MODE     STM32_GPIOF, 11

#define IO_RS485_USART    STM32_UART7
#define RS485_USART_AF    GPIO_AF8

/* -------------------------------------------------------------------------
 * Serial Flash  connections
 * ------------------------------------------------------------------------- */

#define IO_SFLASH_SCK     STM32_GPIOC, 10
#define IO_SFLASH_MISO    STM32_GPIOC, 11
#define IO_SFLASH_MOSI    STM32_GPIOC, 12
#define IO_SFLASH_CS      STM32_GPIOA, 4
#define SPI_SFLASH        STM32F_SPI3

/* low level led on/off functions */
static inline void __led_on(struct stm32_gpio *__gpio, int __pin) {
	stm32_gpio_set(__gpio, __pin);
}

static inline void __led_off(struct stm32_gpio *__gpio, int __pin) {
	stm32_gpio_clr(__gpio, __pin);
}

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_H__ */

