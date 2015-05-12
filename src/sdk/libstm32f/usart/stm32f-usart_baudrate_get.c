/* 
 * File:	 stm32f-usart_baudrate_get.c
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

unsigned int stm32_usart_baudrate_get(struct stm32_usart * us)
{
	unsigned int baudrate;
	uint32_t f_pclk;
	uint32_t div;
	uint32_t f;
	uint32_t m;
	int id;

	if ((id = stm32_usart_lookup(us)) < 0) {
		/* invalid UART ??? */
		return id;
	}

	if (stm32_usart_clk_lut[id].bus == STM32_APB2)
		f_pclk = stm32f_apb2_hz;
	else 
		f_pclk = stm32f_apb1_hz;

	m = us->brr >> 4;
	f = us->brr & 0x0f;

	if (us->cr1 & USART_OVER8) {
		div = (m << 3) | (f & 0x07);
	} else {
		div = (m << 4) | f;
	}

	baudrate = f_pclk / div;

	DCC_LOG3(LOG_TRACE, "baudrate=%d p_clk=%d div=%d", baudrate, f_pclk, div);

	return baudrate;
}

