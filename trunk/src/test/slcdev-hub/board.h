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

#define UART5_TX  STM32_GPIOC, 12
#define UART5_RX  STM32_GPIOD, 2

#define UART1_TX  STM32_GPIOB, 6
#define UART1_RX  STM32_GPIOB, 7

#define UART6_TX  STM32_GPIOC, 6
#define UART6_RX  STM32_GPIOD, 7

#define SPI3_SCK  STM32_GPIOC, 10
#define SPI3_MISO STM32_GPIOC, 11
#define SPI3_MOSI STM32_GPIOB, 5
#define SPI3_NSS  STM32_GPIOE, 1

#define FPGA_EXTI STM32_GPIOD, 6

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_H__ */


