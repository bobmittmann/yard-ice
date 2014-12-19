/* 
 * File:	 stm32f-usart_getc.c
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
#include <sys/delay.h>

int stm32_usart_getc(struct stm32_usart * usart, unsigned int msec)
{
	int tm;

	tm = msec * 20;

	DCC_LOG1(LOG_INFO, "msec=%d", msec);

	for (;;) {		
		if (usart->isr & USART_RXNE) {
			return usart->rdr;
		}
		if (tm == 0) {
			return -2;
		}
		udelay(50);
		tm--;
	}
}

