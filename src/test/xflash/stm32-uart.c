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

#include <stm32f/stm32f-usart.h>
#include <arch/cortex-m3.h>

/* systick must have been programmed to count 1 millisecond */

static int stm32_uart_getc(struct stm32_usart * usart, unsigned int msec)
{
	struct cm3_systick * systick = CM3_SYSTICK;

	(void)systick->ctrl;

	for (;;) {		
		if (usart->isr & USART_RXNE) {
			return usart->rdr;
		}
		if (systick->ctrl & SYSTICK_CTRL_COUNTFLAG) {
			if (--msec == 0)
				return -1;
		}
	}
}

int uart_recv(struct stm32_usart * usart, char * buf, 
			  unsigned int len, unsigned int msec)
{
	char * cp = (char *)buf;
	int c;

	c = stm32_uart_getc(usart, msec);

	if (c < 0)
		return 0;

	*cp = c;
		
	return 1;
}

static void stm32_uart_putc(struct stm32_usart * usart, int c)
{
	while (!(usart->isr & USART_TXE));

	usart->tdr = c;
}

int uart_send(struct stm32_usart * usart, const void * buf, 
			  unsigned int len)
{
	char * cp = (char *)buf;
	int c;
	int n;

	for (n = 0; n < len; n++) {
		c = cp[n];
		stm32_uart_putc(usart, c);
	}

	return n;
}

