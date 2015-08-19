/* 
 * File:	 stm32f-usart_mode_set.c
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
#include <sys/serial.h>

int stm32_usart_mode_set(struct stm32_usart * us, unsigned int flags)
{
	uint32_t cr1;
	uint32_t cr2;
	uint32_t cr_en;
	int bits;
	int id;

	if ((id = stm32_usart_lookup(us)) < 0) {
		/* invalid UART ??? */
		return id;
	}

	/* save bits that should be restored */
	cr_en = us->cr1 & (USART_UE | USART_TE | USART_RE | USART_TXEIE | 
					  USART_TCIE | USART_IDLEIE | USART_RXNEIE);
	if (cr_en & USART_TE) {
		/* drain output buffer */
		while (!(us->sr & USART_TXE));
	}

	/* disable TX, RX and all interrupts */
	us->cr1 = 0;

	cr1 = 0;
	cr2 = 0;

	bits = SERIAL_DATABITS(flags);
	switch (bits) {
	case SERIAL_DATABITS_7:
		DCC_LOG(LOG_INFO, "7 data bits");
		break;
	case SERIAL_DATABITS_9:
		DCC_LOG(LOG_INFO, "9 data bits");
		break;
	case SERIAL_DATABITS_8:
	default:
		DCC_LOG(LOG_INFO, "8 data bits");
		break;
	}

	/* parity and data bits */
	switch (SERIAL_PARITY(flags)) {
	case SERIAL_PARITY_EVEN:
		DCC_LOG(LOG_INFO, "parity EVEN");
		cr1 |= USART_PCE | USART_PS_EVEN;
		cr1 |= (bits == SERIAL_DATABITS_8) ? USART_M9 : USART_M8;
		break;
	case SERIAL_PARITY_ODD:
		DCC_LOG(LOG_INFO, "parity ODD");
		cr1 |= USART_PCE | USART_PS_ODD;
		cr1 |= (bits == SERIAL_DATABITS_8) ? USART_M9 : USART_M8;
		break;
	case SERIAL_PARITY_NONE:
	default:
		DCC_LOG(LOG_INFO, "parity NONE");
		cr1 |= (bits == SERIAL_DATABITS_9) ? USART_M9 : USART_M8;
		break;
	}

	/* stop bits */
	switch (SERIAL_STOPBITS(flags)) {
	case SERIAL_STOPBITS_2:
		DCC_LOG(LOG_TRACE, "2 stop bits");
		cr2 |= USART_STOP_2;
		break;
	case SERIAL_STOPBITS_1:
	default:
		DCC_LOG(LOG_INFO, "1 stop bit");
		cr2 |= USART_STOP_1;
		break;
	}

	us->cr1 = cr1 | cr_en;
	us->cr2 = cr2;

	return 0;
}


