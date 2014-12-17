/* 
 * File:	 stm32f-usart_baudrate_set.c
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

int stm32_usart_baudrate_set(struct stm32_usart * us, unsigned int baudrate)
{
	uint32_t div;
	uint32_t f;
	uint32_t m;
	uint32_t cr1;
	uint32_t f_pclk;
	int id;

	if ((id = stm32_usart_lookup(us)) < 0) {
		/* invalid UART ??? */
		return id;
	}

	if (baudrate <= 100) {
		DCC_LOG(LOG_WARNING, "invalid baudrate");
		return -1;
	}

	/* disable TX and RX and interrupts */
	cr1 = us->cr1;
	us->cr1 = cr1 & ~(USART_UE | USART_TE | USART_RE | USART_TXEIE | 
					  USART_TCIE | USART_IDLEIE | USART_RXNEIE);

	if (stm32_usart_clk_lut[id].bus == STM32_APB2)
		f_pclk = stm32f_apb2_hz;
	else 
		f_pclk = stm32f_apb1_hz;

	div = f_pclk / baudrate;

	DCC_LOG3(LOG_TRACE, "baudrate=%d p_clk=%d div=%d", baudrate, f_pclk, div);

	m = div >> 4;
	f = div & 0x0f;
	us->brr = (m << 4) | f;

	/* restore cr1 */
	us->cr1 = cr1;

	return 0;
}

