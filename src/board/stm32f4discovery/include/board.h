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

#define IO_LED4     STM32_GPIOD, 12 /* LED4 */
#define IO_LED3	    STM32_GPIOD, 13 /* LED3 */
#define IO_LED5	    STM32_GPIOD, 14 /* LED5 */
#define IO_LED6	    STM32_GPIOD, 15 /* LED6 */

#define IO_PUSH_BTN STM32_GPIOA, 0  /* Push button B1 */

#define IO_LIS302_MISO  STM32_GPIOA, 6
#define IO_LIS302_MOSI  STM32_GPIOA, 7
#define IO_LIS302_SCK   STM32_GPIOA, 5
#define IO_LIS302_CS    STM32_GPIOE, 3

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

