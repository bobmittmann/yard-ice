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

/**************************************************************************
 * Sebug serial
 **************************************************************************/
#define UART5_TX       STM32_GPIOC, 12
#define UART5_RX       STM32_GPIOD, 2

/**************************************************************************
 * Lattice FPGA (iCE40LP384) connections
 **************************************************************************/

#define IO_ICE40_SPI_SCK  STM32_GPIOC, 10
#define IO_ICE40_SPI_SDO  STM32_GPIOC, 11
#define IO_ICE40_SPI_SDI  STM32_GPIOB, 5
#define IO_ICE40_SPI_SS   STM32_GPIOE, 1
#define ICE40_SPI         STM32F_SPI3
#define ICE40_CLK_SPI     STM32_CLK_SPI3
#define ICE4_SPI_AF       GPIO_AF6

#define IO_ICE40_CRESET   STM32_GPIOE, 0
#define IO_ICE40_CDONE    STM32_GPIOB, 9

#define IO_RS485_TX       STM32_GPIOB, 6
#define IO_RS485_RX       STM32_GPIOB, 7
//#define RS485_CK       STM32_GPIOA, 8
#define IO_RS485_TRG      STM32_GPIOA, 8
#define IO_RS485_MODE     STM32_GPIOD, 6
#define IO_RS485_USART    STM32_USART1
#define RS485_USART_AF    GPIO_AF7

/**************************************************************************
 * Altera Cyclone FPGA connections
 **************************************************************************/

#define FPGA_EXTI      STM32_GPIOB, 15

#define UART6_TX       STM32_GPIOC, 6
#define UART6_RX       STM32_GPIOD, 7

#define FLASH_BLK_FS_OFFS     0x00020000
#define FLASH_BLK_FS_SIZE     (256 * 1024)

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_H__ */

