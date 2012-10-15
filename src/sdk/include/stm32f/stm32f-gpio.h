/* 
 * Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
 * 
 * This file is part of the libstm32f.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You can receive a copy of the GNU Lesser General Public License from 
 * http://www.gnu.org/
 */

/** 
 * @file stm32f-gpio.h
 * @brief
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __STM32F_GPIO_H__
#define __STM32F_GPIO_H__

/*-------------------------------------------------------------------------
  GPIO
  -------------------------------------------------------------------------*/

/* GPIO port mode register */
#define GPIO_MODER 0x00
#define GPIO_MODE_INPUT(P) (0 << (2 * (P)))
#define GPIO_MODE_OUTPUT(P) (1 << (2 * (P)))
#define GPIO_MODE_ALT_FUNC(P) (2 << (2 * (P)))
#define GPIO_MODE_ANALOG(P) (3 << (2 * (P)))
#define GPIO_MODE_MASK(P) (3 << (2 * (P)))

/* GPIO port output type register */
#define GPIO_OTYPER 0x04
#define GPIO_PUSH_PULL(Y) (0 << (Y))
#define GPIO_OPEN_DRAIN(Y) (1 << (Y))
#define GPIO_OTYPE_MASK(Y) (1 << (Y))

/* GPIO port output speed register */
#define GPIO_OSPEEDR 0x08
#define GPIO_OSPEED_LOW(P) (0 << (2 * (P)))
#define GPIO_OSPEED_MED(P) (1 << (2 * (P)))
#define GPIO_OSPEED_FAST(P) (2 << (2 * (P)))
#define GPIO_OSPEED_HIGH(P) (3 << (2 * (P)))
#define GPIO_OSPEED_MASK(Y) (3 << (2 * (Y)))

/* GPIO port pull-up/pull-down register */
#define GPIO_PUPDR 0x0c
#define GPIO_PULL_NONE(P) (0 << (2 * (P)))
#define GPIO_PULL_UP(P) (1 << (2 * (P)))
#define GPIO_PULL_DOWN(P) (2 << (2 * (P)))
#define GPIO_PULL_MASK(Y) (3 << (2 * (Y)))

/* GPIO port input data register */
#define GPIO_IDR 0x10

/* GPIO port output data register */
#define GPIO_ODR 0x14

/* GPIO port bit set/reset register */
#define GPIO_BSRR 0x18
#define GPIO_SET(Y) (1 << (Y))
#define GPIO_RESET(Y) (1 << (Y))

/* GPIO port configuration lock register */
#define GPIO_LCKR 0x1c
#define GPIO_LCKK (1 << 16)

/* GPIO alternate function low register */
#define GPIO_AFRL 0x20
#define GPIO_AFRL_SET(P, F) ((F) << (4 * (P)))
#define GPIO_AFRL_MASK(P) ((0xf) << (4 * (P)))

/* GPIO alternate function high register */
#define GPIO_AFRH 0x24
#define GPIO_AFRH_SET(P, F) ((F) << (4 * ((P) - 8)))
#define GPIO_AFRH_MASK(P) ((0xf) << (4 * ((P) - 8)))

#define GPIO_AF0 0
#define GPIO_AF1 1
#define GPIO_AF2 2
#define GPIO_AF3 3
#define GPIO_AF4 4
#define GPIO_AF5 5
#define GPIO_AF6 6
#define GPIO_AF7 7
#define GPIO_AF8 8
#define GPIO_AF9 9
#define GPIO_AF10 10
#define GPIO_AF11 11
#define GPIO_AF12 12
#define GPIO_AF13 13
#define GPIO_AF14 14
#define GPIO_AF15 15

#ifndef __ASSEMBLER__

#include <stdint.h>

struct stm32f_gpio {
	volatile uint32_t moder;
	volatile uint32_t otyper;
	volatile uint32_t ospeedr;
	volatile uint32_t pupdr;
	volatile uint32_t idr;
	volatile uint32_t odr;
	volatile uint16_t bsrrl;
	volatile uint16_t bsrrh;
	volatile uint32_t lckr;
	volatile uint32_t afrl;
	volatile uint32_t afrh;
};

static inline void stm32f_gpio_set(struct stm32f_gpio * __gpio, int __pin) {
	__gpio->bsrrl = GPIO_SET(__pin);
};

static inline void stm32f_gpio_clr(struct stm32f_gpio * __gpio, int __pin) {
	__gpio->bsrrh = GPIO_RESET(__pin);
};

/* pin status */
static inline int stm32f_gpio_status(struct stm32f_gpio * __gpio, int __pin) {
	return __gpio->idr & (1 << __pin);
};

#endif /* __ASSEMBLER__ */


#endif /* __STM32F_GPIO_H__ */
