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

const struct stm32f_usart * stm32f_usart_lut[6] = {
	STM32F_USART1,
	STM32F_USART2,
	STM32F_USART3,
	STM32F_UART4,
	STM32F_UART5,
#ifdef STM32F_USART6
	STM32F_USART6
#endif
};

/* APB clock */

#define APB1 (1 << 5)
#define APB2 (1 << 6)
#define CLK(APB, BIT) (APB | BIT)
#define CLK_BIT 0x1f

#ifdef STM32F2X
const uint8_t stm32f_us_clk_lut[] = {  
	CLK(APB2, 4), 
	CLK(APB1, 17), 
	CLK(APB1, 18), 
	CLK(APB1, 19), 
	CLK(APB1, 20), 
	CLK(APB2,  5)
};
#endif

#ifdef STM32F1X
const uint8_t stm32f_us_clk_lut[] = {  
	CLK(APB2, 14), 
	CLK(APB1, 17), 
	CLK(APB1, 18), 
	CLK(APB1, 19), 
	CLK(APB1, 20)
};
#endif


int stm32f_usart_lookup(struct stm32f_usart * us)
{
	int id = sizeof(stm32f_usart_lut) / sizeof(struct stm32f_usart *);

	while ((--id >= 0) && (us != stm32f_usart_lut[id]));

	return id;
}

