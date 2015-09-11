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
#define __THINKOS_IRQ__
#include <thinkos_irq.h>

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

#ifdef __cplusplus
extern "C" {
#endif

/* low level led on/off functions */
static inline void __led_on(struct stm32_gpio *__gpio, int __pin) {
//	stm32_gpio_mode_af(__gpio, __pin);
	stm32_gpio_clr(__gpio, __pin);
}

static inline void __led_off(struct stm32_gpio *__gpio, int __pin) {
//	stm32_gpio_mode_out(__gpio, __pin);
	stm32_gpio_set(__gpio, __pin);
}

static inline bool __is_led_on(struct stm32_gpio *__gpio, int __pin) {
	return stm32_gpio_is_mode_af(__gpio, __pin);
}

void __attribute__((noreturn)) io_event_task(void);
void io_init(void);

void isink_start(unsigned int mode, unsigned int pre, unsigned int pulse);
void isink_stop(void);

void irate_set(unsigned int mv);

void lamp_test(void);

void led_on(unsigned int id);

void led_off(unsigned int id);

bool led_status(unsigned int id);

void led_flash(unsigned int id, unsigned int ms);

unsigned int timer_get(unsigned int id);

void timer_set(unsigned int id, unsigned int ms);

void io_shutdown(void);

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_H__ */


