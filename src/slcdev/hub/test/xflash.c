/* 
 * File:	 xflash.c
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(C) 2011 Bob Mittmann. All Rights Reserved.
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

#include <stdint.h>
#include <string.h>
#include <arch/cortex-m3.h>
#include <sys/dcclog.h>

extern const uint8_t otg_xflash_pic[];
extern const unsigned int sizeof_otg_xflash_pic;
extern uint32_t __data_start[]; 

int usb_xflash(uint32_t offs, uint32_t len)
{
	uint32_t * xflash_code = __data_start;
	int (* xflash_ram)(uint32_t, uint32_t) = ((void *)xflash_code) + 1;

	DCC_LOG3(LOG_TRACE, "sp=%08x offs=%08x len=%d", cm3_sp_get(), offs, len);

	memcpy(xflash_code, otg_xflash_pic, sizeof_otg_xflash_pic);

	return xflash_ram(offs, len);
}

extern const uint8_t uart_xflash_pic[];
extern const unsigned int sizeof_uart_xflash_pic;

int usart_xflash(void * uart, uint32_t offs, uint32_t len)
{
	uint32_t * xflash_code = __data_start;
	int (* xflash_ram)(void *, uint32_t, uint32_t) = ((void *)xflash_code) + 1;

	DCC_LOG3(LOG_TRACE, "sp=%08x offs=%08x len=%d", cm3_sp_get(), offs, len);

	memcpy(xflash_code, uart_xflash_pic, sizeof_uart_xflash_pic);

	return xflash_ram(uart, offs, len);
}
