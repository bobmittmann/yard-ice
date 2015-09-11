/* 
 * File:	 led.h
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

#include "config.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <thinkos.h>
#define __THINKOS_SYS__
#include <thinkos_sys.h>
#include <sys/stm32f.h>
#include <arch/cortex-m3.h>

/* USB */
#define USB_FS_DP STM32_GPIOA, 12
#define USB_FS_DM STM32_GPIOA, 11
#define USB_FS_VBUS STM32_GPIOB, 6 /* PB6 */

/* UART */
#define USART2_RX STM32_GPIOA, 3
#define USART2_TX STM32_GPIOA, 2

/* RS485 */
#define RS485_RXEN STM32_GPIOA, 0
#define RS485_TXEN STM32_GPIOA, 1

/* LEDs */
#define LED1_IO STM32_GPIOA, 6
#define LED2_IO STM32_GPIOA, 7

#define LED_RED 1
#define LED_AMBER 0

#ifdef __cplusplus
extern "C" {
#endif

void pin1_sel_usart2_rts(void);

static inline void rs485_rxen(void) {
	stm32_gpio_clr(RS485_RXEN);
}

static inline void rs485_rxdis(void) {
	stm32_gpio_set(RS485_RXEN);
}

static inline void rs485_txen(void) {
	stm32_gpio_set(RS485_TXEN);
}

static inline void rs485_txdis(void) {
	stm32_gpio_clr(RS485_TXEN);
}

static inline int rs485_is_txen(void) {
	return stm32_gpio_stat(RS485_TXEN);
}

void usb_vbus(bool on);

void io_init(void);

void leds_init(void);

void __attribute__((noreturn)) led_task(void);

void led_on(unsigned int id);

void led_off(unsigned int id);

void led_flash(unsigned int id, unsigned int ms);

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_H__ */


