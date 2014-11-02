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

#define STM32F_BASE_UART5   0x40005000
#define STM32_UART5 ((struct stm32_usart *)STM32F_BASE_UART5)

static void stm32_uart_putc(struct stm32_usart * usart, int c)
{
	while (!(usart->isr & USART_TXE));

	usart->tdr = c;
}

void dbg_putstr(const char * s)
{
	struct stm32_usart * usart = STM32_UART5;
	int c;
	int n;

	for (n = 0; (c = s[n]) != '\0'; ++n) {
		if (c == '\n')
			stm32_uart_putc(usart, '\r');
		stm32_uart_putc(usart, c);
	}
}

void dbg_puthex(unsigned int val)
{
	struct stm32_usart * usart = STM32_UART5;
	int c;
	int n;
	int i;

	/* value is zero ? */
	if (!val)
		stm32_uart_putc(usart, '0');

	n = 0;
	for (i = 0; i < (sizeof(unsigned int) * 2); i++) {
		c = val >> ((sizeof(unsigned int) * 8) - 4);
		val <<= 4;
		if ((c != 0) || (n != 0))
			 stm32_uart_putc(usart, c < 10 ? c + '0' : c + ('a' - 10));
	}
}

void dbg_reset(void)
{
	struct stm32_usart * usart = STM32_UART5;

	/* disable all interrupts */
	usart->cr1 &= ~(USART_TXEIE | USART_TCIE | USART_IDLEIE | USART_RXNEIE);
	/* Disable DMA */
	usart->cr3 &= ~(USART_DMAT | USART_DMAR);
}

