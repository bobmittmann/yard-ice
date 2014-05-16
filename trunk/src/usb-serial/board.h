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

#include <sys/stm32f.h>

#define USB_FS_DP STM32F_GPIOA, 12
#define USB_FS_DM STM32F_GPIOA, 11

#define USB_FS_VBUS STM32F_GPIOB, 6 /* PB6 */

#define PUSHBTN_IO STM32F_GPIOB, 8
#define EXTRST0_IO STM32F_GPIOB, 0 

/* ---------------------------------------------------------------------------
 * PIN1 -  PA1 (USART2_RTS) | PB11 (USART3_RX) |      |
 * PIN2 -  PA4 (USART2_CK)  | PB10 (USART3_TX) | PA5  | 
 * PIN3 -  GND              |                  |      |
 * PIN4 -  PA3 (USART2_RX)  | PA10 (USART1_RX) |      |
 * PIN5 -  PA2 (USART2_TX)  |  PA9 (USART1_TX) |      |
 * ---------------------------------------------------------------------------
 */

/* Pin 1 */
#define USART2_RTS STM32F_GPIOA, 1
#define USART3_RX STM32F_GPIOB, 11

/* Pin 2 */
#define USART2_CK STM32F_GPIOA, 4
#define USART3_TX STM32F_GPIOB, 10
#define EXTRST1_IO STM32F_GPIOA, 5 /* PA5 */

/* Pin 4 */
#define USART2_RX STM32F_GPIOA, 3
#define USART1_RX STM32F_GPIOA, 10

/* Pin 5 */
#define USART2_TX STM32F_GPIOA, 2
#define USART1_TX STM32F_GPIOA, 9

/* LEDs */
#define LED1_IO STM32F_GPIOA, 6
#define LED2_IO STM32F_GPIOA, 7


#ifdef __cplusplus
extern "C" {
#endif

/* Pin 1 */

void pin1_sel_input(void);

void pin1_sel_vcc(void);

void pin1_sel_gnd(void);

void pin1_sel_usart3_rx(void);

void pin1_sel_i2c_sda(void);

/* Pin 2 */

void pin2_sel_input(void);

void pin2_sel_open_drain(void);

void pin2_sel_vcc(void);

void pin2_sel_gnd(void);

void pin2_sel_usart3_tx(void);

void pin2_sel_i2c_scl(void);

/* Pin 4 */

void pin4_sel_input(void);

void pin4_sel_vcc(void);

void pin4_sel_gnd(void);

void pin4_sel_usart2_rx(void);

/* Pin 5 */

void pin5_sel_input(void);

void pin5_sel_vcc(void);

void pin5_sel_gnd(void);

void pin5_sel_usart2_tx(void);

void pin5_sel_usart1_tx(void);

/* USART1 and USART2 pins are connected together.
   Only one TX pin must be enable at any time */

/* Select USART2 TX */
void io_sel_usart2(void);

/* Select USART1 TX */
void io_sel_usart1(void);

void usb_vbus(bool on);

void io_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_H__ */


