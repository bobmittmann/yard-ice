/* 
 * Copyright(c) 2004-2012 BORESTE (www.boreste.com). All Rights Reserved.
 *
 * This file is part of the YARD-ICE.
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
 * @file sys/sysclk.h
 * @brief
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <arch/cortex-m3.h>

#ifndef __SYS_SYSCLK_H__
#define __SYS_SYSCLK_H__

enum {
	SYSCLK_STM32_AHB  = 0,
	SYSCLK_STM32_APB1 = 1,
	SYSCLK_STM32_APB2 = 2,
	SYSCLK_STM32_TIM1 = 3,
	SYSCLK_STM32_TIM2 = 4,
	SYSCLK_STM32_HSI  = 5,
	SYSCLK_STM32_HSE  = 6,
	SYSCLK_STM32_LSI  = 7,
	SYSCLK_STM32_LSE  = 8
};

extern const uint32_t sysclk_hz[];

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* __SYS_SYSCLK_H__ */

