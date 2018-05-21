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
 * USB
 **************************************************************************/
#define OTG_FS_DP   STM32_GPIOA, 12
#define OTG_FS_DM   STM32_GPIOA, 11
#define OTG_FS_VBUS STM32_GPIOA, 9

/**************************************************************************
 * Sebug serial
 **************************************************************************/
#define IO_UART5_TX       STM32_GPIOC, 12
#define IO_UART5_RX       STM32_GPIOD, 2

/**************************************************************************
 * Relay and Power
 **************************************************************************/

#define IO_RELAY          STM32_GPIOB, 9
#define IO_PWR_EN         STM32_GPIOD, 12 
#define IO_PWR_MON        STM32_GPIOD, 11

/**************************************************************************
 * Altera Cyclone FPGA connections
 **************************************************************************/

#define IO_FPGA_EXTI      STM32_GPIOB, 15
#define IO_UART6_TX       STM32_GPIOC, 6
#define IO_UART6_RX       STM32_GPIOD, 7

#define IO_N_CONFIG       STM32_GPIOE, 0
#define IO_CONF_DONE      STM32_GPIOE, 1
#define IO_N_STATUS       STM32_GPIOC, 11
#define IO_DCLK           STM32_GPIOC, 10
#define IO_DATA0          STM32_GPIOB, 5

/**************************************************************************
 * FLASH memory partitions
 **************************************************************************/
#define FLASH_BLK_BOOT_OFFS     0x00000000
#define FLASH_BLK_BOOT_SIZE     (16 * 1024)

#define FLASH_BLK_FS_OFFS       0x00004000
#define FLASH_BLK_FS_SIZE       (48 * 1024)

#define FLASH_BLK_RBF_OFFS      0x00010000
#define FLASH_BLK_RBF_SIZE      (64 * 1024)

#define FLASH_BLK_APP_OFFS      0x00020000
#define FLASH_BLK_APP_SIZE      (384 * 1024)

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_H__ */

