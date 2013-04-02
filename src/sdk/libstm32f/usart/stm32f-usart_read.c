/* 
 * File:	 stm32f-usart_read.c
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

int stm32f_usart_read(struct stm32f_usart * usart, char * buf, 
					  unsigned int len, unsigned int msec)
{
	char * cp = (char *)buf;
	int c;

	c = stm32f_usart_getc(usart, msec);

	if (c < 0)
		return 0;

	*cp = c;
		
	return 1;
}

