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

	for (;;) {		
		if (usart->isr & USART_RXNE) {
			return usart->rdr;
		}
		if (systick->ctrl & SYSTICK_CTRL_COUNTFLAG) {
			if (msec == 0)
				return -1;
			msec--;
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
		return c;

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


void uart_reset(struct stm32_usart * usart)
{
	/* disable all interrupts */
	usart->cr1 &= ~(USART_TXEIE | USART_TCIE | USART_IDLEIE | USART_RXNEIE);

	/* Disable DMA */
	usart->cr3 &= ~(USART_DMAT | USART_DMAR);
}

void uart_drain(struct stm32_usart * usart)
{
	while (!(usart->isr & USART_TC));
}

#if 0
int uint2hex(char * s, unsigned int val)
{
	int n;
	int c;
	int i;

	/* value is zero ? */
	if (!val) {
		*s++ = '0';
		*s = '\0';
		return 1;
	}

	n = 0;
	for (i = 0; i < (sizeof(unsigned int) * 2); i++) {
		c = val >> ((sizeof(unsigned int) * 8) - 4);
		val <<= 4;
		if ((c != 0) || (n != 0)) {
			s[n++] = c < 10 ? c + '0' : c + ('a' - 10);
		}
	}

	s[n] = '\0';

	return n;
}

void uart_send_hex(struct stm32_usart * usart, unsigned int val)
{
	char buf[16];
	char * cp = buf;;
	int n;

	*cp++ = ' ';
	*cp++ = '0';
	*cp++ = 'x';
	n = uint2hex(cp, val);
	cp += n;
	*cp++ = '\r';
	*cp++ = '\n';
	n += 5;

	uart_send(usart, buf, n);
}

#endif

