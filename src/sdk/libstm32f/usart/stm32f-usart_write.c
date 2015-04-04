/* 
 * File:	 stm32f-usart_write.c
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

int stm32_usart_write(struct stm32_usart * usart, const void * buf, 
					   unsigned int len)
{
	char * cp = (char *)buf;
	uint32_t cr1 = usart->cr1;
	int c;
	int n;

	cr1 = usart->cr1;
	usart->cr1 = cr1 & ~(USART_TXEIE | USART_TCIE);

	for (n = 0; n < len; n++) {
		c = cp[n];
		stm32_usart_putc(usart, c);
	}

	usart->cr1 = cr1;
	return n;
}

