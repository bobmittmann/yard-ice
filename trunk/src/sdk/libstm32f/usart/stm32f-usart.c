/* 
 * File:	stm32f-usart.c
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(C) 2011 Bob Mittmann. All Rights Reserved.
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

#include "usart-priv.h"

const struct stm32f_usart * const stm32f_usart_lut[] = {
	STM32_USART1,
	STM32_USART2,
	STM32_USART3,
	STM32_UART4,
	STM32_UART5,
#ifdef STM32F_USART6
	STM32_USART6
#endif
};

const uint8_t stm32f_usart_irq_lut[] = {
	STM32_IRQ_USART1,
	STM32_IRQ_USART2,
	STM32_IRQ_USART3,
#ifdef STM32_IRQ_UART4
	STM32_IRQ_UART4,
#endif
#ifdef STM32_IRQ_UART4
	STM32_IRQ_UART5,
#endif
#ifdef STM32F_IRQ_USART6
	STM32_IRQ_USART6
#endif
};

#if defined(STM32F2X) || defined(STM32F4X)
const struct stm32_clk stm32_usart_clk_lut[] = {  
	{ STM32_APB2,  4}, 
	{ STM32_APB1, 17}, 
	{ STM32_APB1, 18}, 
	{ STM32_APB1, 19}, 
	{ STM32_APB1, 20}, 
	{ STM32_APB2,  5}
};
#endif

#if defined(STM32F1X) || defined(STM32F3X)
const struct stm32_clk stm32_usart_clk_lut[] = {  
	{ STM32_APB2, 14}, 
	{ STM32_APB1, 17}, 
	{ STM32_APB1, 18}, 
	{ STM32_APB1, 19}, 
	{ STM32_APB1, 20}
};
#endif

#if defined(STM32L1X)
const struct stm32_clk stm32_usart_clk_lut[] = {  
	{ STM32_CLK_USART1 }, 
	{ STM32_CLK_USART2 }, 
	{ STM32_CLK_USART3 }, 
	{ STM32_CLK_UART4 }, 
	{ STM32_CLK_UART5 }
};
#endif

int stm32f_usart_lookup(struct stm32f_usart * usart)
{
	int id = sizeof(stm32f_usart_lut) / sizeof(struct stm32f_usart *);

	while ((--id >= 0) && (usart != stm32f_usart_lut[id]));

	return id;
}

