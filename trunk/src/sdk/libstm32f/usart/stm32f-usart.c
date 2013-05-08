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
	STM32F_USART1,
	STM32F_USART2,
	STM32F_USART3,
	STM32F_UART4,
	STM32F_UART5,
#ifdef STM32F_USART6
	STM32F_USART6
#endif
};

const uint8_t stm32f_usart_irq_lut[] = {
	STM32F_IRQ_USART1,
	STM32F_IRQ_USART2,
	STM32F_IRQ_USART3,
	STM32F_IRQ_UART4,
	STM32F_IRQ_UART5,
#ifdef STM32F_USART6
	STM32F_IRQ_USART6
#endif
};

#if defined(STM32F2X) || defined(STM32F4X)
const struct stm32f_clk stm32f_usart_clk_lut[] = {  
	{ STM32F_APB2,  4}, 
	{ STM32F_APB1, 17}, 
	{ STM32F_APB1, 18}, 
	{ STM32F_APB1, 19}, 
	{ STM32F_APB1, 20}, 
	{ STM32F_APB2,  5}
};
#endif

#if defined(STM32F1X)
const struct stm32f_clk stm32f_usart_clk_lut[] = {  
	{ STM32F_APB2, 14}, 
	{ STM32F_APB1, 17}, 
	{ STM32F_APB1, 18}, 
	{ STM32F_APB1, 19}, 
	{ STM32F_APB1, 20}
};
#endif


int stm32f_usart_lookup(struct stm32f_usart * usart)
{
	int id = sizeof(stm32f_usart_lut) / sizeof(struct stm32f_usart *);

	while ((--id >= 0) && (usart != stm32f_usart_lut[id]));

	return id;
}

