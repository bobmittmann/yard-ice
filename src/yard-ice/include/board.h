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
#define IO_UART5_TX       STM32_GPIOC, 12
#define IO_UART5_RX       STM32_GPIOD, 2

/**************************************************************************
 * Relay and Power
 **************************************************************************/

#define IO_RELAY_GPIO     STM32_GPIOB, 9
#define IO_PWR_EN_GPIO    STM32_GPIOD, 12 
#define IO_PWR_MON_GPIO   STM32_GPIOD, 11

/**************************************************************************
 * Altera Cyclone FPGA connections
 **************************************************************************/

#define IO_FPGA_EXTI      STM32_GPIOB, 15
#define IO_UART6_TX       STM32_GPIOC, 6
#define IO_UART6_RX       STM32_GPIOD, 7


#define FLASH_BLK_FS_OFFS     0x00020000
#define FLASH_BLK_FS_SIZE     (256 * 1024)

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_H__ */

