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

#define LED3_IO	 STM32_GPIOE,  9 /* LED3 */
#define LED4_IO  STM32_GPIOE,  8 /* LED4 */
#define LED5_IO  STM32_GPIOE, 10 /* LED5 */
#define LED6_IO  STM32_GPIOE, 15 /* LED6 */
#define LED7_IO  STM32_GPIOE, 11 /* LED7 */
#define LED8_IO  STM32_GPIOE, 14 /* LED8 */
#define LED9_IO  STM32_GPIOE, 12 /* LED9 */
#define LED10_IO STM32_GPIOE, 13 /* LED10 */

#define SW_B1_IO STM32_GPIOA,  0 /* Push button B1 */

#define LSM303_SCL  STM32_GPIOB, 6
#define LSM303_SDA  STM32_GPIOB, 7
#define LSM303_INT1_GIO  STM32_GPIOE, 5
#define LSM303_INT1_EXTI (1 << 5)
#define LSM303_INT2_GIO  STM32_GPIOE, 4
#define LSM303_INT2_EXTI (1 << 4)
#define LSM303_DRDY_GIO  STM32_GPIOE, 2
#define LSM303_DRDY_EXTI (1 << 2)

#define L3GD20_SCK  STM32_GPIOA, 5
#define L3GD20_MOSI STM32_GPIOA, 7
#define L3GD20_MISO STM32_GPIOA, 6
#define L3GD20_CS   STM32_GPIOE, 3
#define L3GD20_INT2 STM32_GPIOE, 1
#define L3GD20_INT1 STM32_GPIOE, 0

/* low level led on/off functions */
static inline void __led_on(struct stm32_gpio *__gpio, int __pin) {
	stm32_gpio_set(__gpio, __pin);
}

static inline void __led_off(struct stm32_gpio *__gpio, int __pin) {
	stm32_gpio_clr(__gpio, __pin);
}

static inline void __led_toggle(struct stm32_gpio *__gpio, int __pin) {
	if (stm32_gpio_stat(__gpio, __pin))
		stm32_gpio_clr(__gpio, __pin);
	else
		stm32_gpio_set(__gpio, __pin);
}

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_H__ */

