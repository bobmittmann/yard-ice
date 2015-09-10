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
#include <thinkos.h>
#define __THINKOS_SYS__
#include <thinkos_sys.h>

#define IO_LED1         STM32_GPIOA, 6
#define IO_LED2         STM32_GPIOA, 7
#define IO_LED3         STM32_GPIOB, 0
#define IO_LED4         STM32_GPIOB, 1

#define IO_SW1UP        STM32_GPIOB, 15
#define IO_SW1DWN       STM32_GPIOB, 14
#define IO_SW2UP        STM32_GPIOB, 13
#define IO_SW2DWN       STM32_GPIOB, 12

#define IO_UART_TX      STM32_GPIOA, 2
#define IO_UART_RX      STM32_GPIOA, 3

#define IO_COMP         STM32_GPIOB, 5

#define IO_VNEG_SW      STM32_GPIOA, 1 

#define IO_IRATE        STM32_GPIOA, 5

#define IO_SINK1        STM32_GPIOB, 6
#define IO_SINK2        STM32_GPIOB, 7
#define IO_SINK3        STM32_GPIOB, 8
#define IO_SINK4        STM32_GPIOB, 9

#define SW1_OFF (0 << 0)
#define SW1_A   (1 << 0)
#define SW1_B   (2 << 0)
#define SW1_MSK (3 << 0)

#define SW2_OFF (0 << 2)
#define SW2_A   (1 << 2)
#define SW2_B   (2 << 2)
#define SW2_MSK (3 << 2)

/*****************************************************************************
 * FLASH memory map
 *****************************************************************************/
/*
 | Block      | Block      |        |                               |
 | Start      | End        |   Size | Description                   |
 +------------+------------+--------+-------------------------------+
 | 0x08000000 | 0x0800bfff |  48KiB | Firmware                      |
 | 0x0800c000 | 0x08011fff |  24KiB | Device Database Compiled      |  
 | 0x08012000 | 0x08017fff |  24KiB | Device Database File (JSON)   |  
 | 0x08018000 | 0x0801ffff |  32KiB | Simulator Config File (JSON)  |  
 +------------+------------+--------+-------------------------------+
*/

#define FLASH_BLK_FIRMWARE_OFFS     0x00000000
#define FLASH_BLK_FIRMWARE_SIZE     (60 * 1024)

#define FLASH_BLK_CONST_STRING_OFFS 0x0000f000
#define FLASH_BLK_CONST_STRING_SIZE (8 * 1024)

#define FLASH_BLK_DB_BIN_OFFS       0x00011000
#define FLASH_BLK_DB_BIN_SIZE       (8 * 1024)

#define FLASH_BLK_CFG_BIN_OFFS      0x00013000
#define FLASH_BLK_CFG_BIN_SIZE      (16 * 1024)

#define FLASH_BLK_DB_JSON_OFFS      0x00017000
#define FLASH_BLK_DB_JSON_SIZE      (20 * 1024)

#define FLASH_BLK_CFG_JSON_OFFS     0x0001c000
#define FLASH_BLK_CFG_JSON_SIZE     (16 * 1024)

//#define FLASH_BLK_CODE_BIN_OFFS     0x00010000
//#define FLASH_BLK_CODE_BIN_SIZE     (0 * 1024)

/*****************************************************************************
 * EEPROM memory map
 *****************************************************************************/

/*
 | Offset |    Size | Description                   |
 +------------------+-------------------------------+
 | 0x0000 |   512   | String index table            |
 | 0x0200 |   512   | Trouble log                   |  
 +--------+---------+--------+----------------------+
*/
#define EEPROM_BLK_STRING_IDX_OFFS   0x0000
#define EEPROM_BLK_TROUBLE_LOG_OFFS  0x0200

/* -------------------------------------------------------------------------
 * ThinkOS kernel descriptors
 * ------------------------------------------------------------------------- */

/* serial driver transmission flag */
//#define SERDRV_RX_FLAG THINKOS_FLAG_DESC(0) 
#define SERDRV_RX_GATE THINKOS_GATE_DESC(1) 
/* serial driver reception gate */
#define SERDRV_TX_GATE THINKOS_GATE_DESC(0) 
/* SLC device driver event set */
#define SLCDEV_DRV_EV  THINKOS_EVENT_DESC(0) 

#define IO_POLL_PERIOD_MS 16

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

void board_init(void);
void board_test(void);

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_H__ */


