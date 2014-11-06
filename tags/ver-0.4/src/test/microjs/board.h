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

#define LED1      STM32_GPIOB, 10
#define LED2      STM32_GPIOB, 11
#define LED3      STM32_GPIOB, 0
#define LED4      STM32_GPIOB, 1
#define LED5      STM32_GPIOA, 6
#define LED6      STM32_GPIOA, 7

#define ADDR0     STM32_GPIOA, 8
#define ADDR1     STM32_GPIOA, 9
#define ADDR2     STM32_GPIOA, 10
#define ADDR3     STM32_GPIOA, 11

#define ADDR4     STM32_GPIOA, 12
#define ADDR5     STM32_GPIOC, 13
#define ADDR6     STM32_GPIOC, 14
#define ADDR7     STM32_GPIOC, 15

#define MODSW     STM32_GPIOB, 4

#define USART2_TX STM32_GPIOA, 2
#define USART2_RX STM32_GPIOA, 3

#define COMP1     STM32_GPIOA, 0
#define COMP2     STM32_GPIOB, 5

#define TRIG_OUT  STM32_GPIOB, 2

#define SW3A      STM32_GPIOB, 15
#define SW3B      STM32_GPIOB, 14
#define SW4A      STM32_GPIOB, 13
#define SW4B      STM32_GPIOB, 12

#define VNEG_SW   STM32_GPIOA, 1 

#define CHRG_EN   STM32_GPIOB, 6

#define SINK1     STM32_GPIOB, 7
#define SINK2     STM32_GPIOB, 8
#define SINK3     STM32_GPIOB, 9
#define SINK4     STM32_GPIOA, 4
#define IRATE     STM32_GPIOA, 5

#define COMP1     STM32_GPIOA, 0
#define COMP2     STM32_GPIOB, 5



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
#define FLASH_BLK_FIRMWARE_SIZE     (52 * 1024)

#define FLASH_BLK_CONST_STRING_OFFS 0x0000d000
#define FLASH_BLK_CONST_STRING_SIZE (12 * 1024)

#define FLASH_BLK_SLOT1_OFFS        0x00010000
#define FLASH_BLK_SLOT1_SIZE        (4 * 1024)

#define FLASH_BLK_SLOT2_OFFS        0x00011000
#define FLASH_BLK_SLOT2_SIZE        (4 * 1024)

#define FLASH_BLK_SLOT3_OFFS        0x00012000
#define FLASH_BLK_SLOT3_SIZE        (4 * 1024)

#define FLASH_BLK_SLOT4_OFFS        0x00013000
#define FLASH_BLK_SLOT4_SIZE        (4 * 1024)

#define FLASH_BLK_SLOT5_OFFS        0x00014000
#define FLASH_BLK_SLOT5_SIZE        (4 * 1024)

#define FLASH_BLK_SLOT6_OFFS        0x00015000
#define FLASH_BLK_SLOT6_SIZE        (4 * 1024)

#define FLASH_BLK_SLOT7_OFFS        0x00016000
#define FLASH_BLK_SLOT7_SIZE        (4 * 1024)

#define FLASH_BLK_SLOT8_OFFS        0x00017000
#define FLASH_BLK_SLOT8_SIZE        (4 * 1024)

#define FLASH_BLK_SLOT9_OFFS        0x00018000
#define FLASH_BLK_SLOT9_SIZE        (4 * 1024)

#define FLASH_BLK_SLOT10_OFFS       0x00019000
#define FLASH_BLK_SLOT10_SIZE       (4 * 1024)

#define FLASH_BLK_SLOT11_OFFS       0x0001a000
#define FLASH_BLK_SLOT11_SIZE       (4 * 1024)

#define FLASH_BLK_SLOT12_OFFS       0x0001b000
#define FLASH_BLK_SLOT12_SIZE       (4 * 1024)

#define FLASH_BLK_SLOT13_OFFS       0x0001c000
#define FLASH_BLK_SLOT13_SIZE       (4 * 1024)

#define FLASH_BLK_SLOT14_OFFS       0x0001d000
#define FLASH_BLK_SLOT14_SIZE       (4 * 1024)

#define FLASH_BLK_SLOT15_OFFS       0x0001e000
#define FLASH_BLK_SLOT15_SIZE       (4 * 1024)

#define FLASH_BLK_SLOT16_OFFS       0x0001f000
#define FLASH_BLK_SLOT16_SIZE       (4 * 1024)

#ifdef __cplusplus
extern "C" {
#endif

/* low level led on/off functions */
static inline void __led_on(struct stm32_gpio *__gpio, int __pin) {
	stm32_gpio_mode_af(__gpio, __pin);
}

static inline void __led_off(struct stm32_gpio *__gpio, int __pin) {
	stm32_gpio_mode_out(__gpio, __pin);
}

static inline bool __is_led_on(struct stm32_gpio *__gpio, int __pin) {
	return stm32_gpio_is_mode_af(__gpio, __pin);
}

void io_init(void);

void led_on(unsigned int id);

void led_off(unsigned int id);

void system_reset(void);

#ifdef __cplusplus
}

#endif

#endif /* __BOARD_H__ */


