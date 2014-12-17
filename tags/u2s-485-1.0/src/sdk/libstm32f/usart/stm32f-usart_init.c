/* 
 * File:     stm32f-usart_init.c
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(C) 2013 Bob Mittmann. All Rights Reserved.
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

int stm32_usart_init(struct stm32_usart * us)
{
	int id;

	if ((id = stm32_usart_lookup(us)) < 0) {
		/* invalid UART ??? */
		return id;
	}

	DCC_LOG2(LOG_INFO, "USART %d -> 0x%08x.", id + 1, us);

	/* disable all interrupts */
	us->cr1 = 0;

	/* Enable peripheral clock */
	stm32_clk_enable(STM32_RCC, stm32_usart_clk_lut[id].bus, 
					 stm32_usart_clk_lut[id].bit);

	/* output drain */
//	while (!(us->sr & USART_TXE));

	us->cr1 = 0;
	us->cr2 = 0;
	us->cr3 = 0;
	us->gtpr = 0;

	DCC_LOG(LOG_INFO, "done.");

	return id;
}

