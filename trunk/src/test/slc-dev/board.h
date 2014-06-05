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

#define LED1 STM32_GPIOB, 10
#define LED2 STM32_GPIOB, 11

/* ---------------------------------------------------------------------------
 * PIN1 -  PA1 (USART2_RTS) | PB11 (USART3_RX) |      |
 * PIN2 -  PA4 (USART2_CK)  | PB10 (USART3_TX) | PA5  | 
 * PIN3 -  GND              |                  |      |
 * PIN4 -  PA3 (USART2_RX)  | PA10 (USART1_RX) |      |
 * PIN5 -  PA2 (USART2_TX)  |  PA9 (USART1_TX) |      |
 * ---------------------------------------------------------------------------
 */

/* Pin 1 */
#define USART2_RTS STM32_GPIOA, 1
#define USART3_RX STM32_GPIOB, 11

/* Pin 2 */
#define USART2_CK STM32_GPIOA, 4
#define USART3_TX STM32_GPIOB, 10
#define EXTRST1_IO STM32_GPIOA, 5 /* PA5 */

/* Pin 4 */
#define USART2_RX STM32_GPIOA, 3
#define USART1_RX STM32_GPIOA, 10

/* Pin 5 */
#define USART2_TX STM32_GPIOA, 2
#define USART1_TX STM32_GPIOA, 9

/* LEDs */
#define LED1_IO STM32_GPIOA, 6
#define LED2_IO STM32_GPIOA, 7

#define TIM4_CH3 STM32_GPIOB, 8
#define TIM4_CH4 STM32_GPIOB, 9


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


