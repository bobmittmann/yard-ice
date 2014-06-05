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

#define USART2_TX STM32_GPIOA, 2
#define USART2_RX STM32_GPIOA, 3

#define COMP1     STM32_GPIOA, 0
#define COMP2     STM32_GPIOB, 5

#define TRIG      STM32_GPIOB, 2

#ifdef __cplusplus
extern "C" {
#endif

void io_init(void);

static inline void led_on(struct stm32_gpio *__gpio, int __pin) {
	stm32_gpio_clr(__gpio, __pin);
}

static inline void led_off(struct stm32_gpio *__gpio, int __pin) {
	stm32_gpio_set(__gpio, __pin);
}


#ifdef __cplusplus
}
#endif

#endif /* __BOARD_H__ */


