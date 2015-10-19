/* 
 * File:	 xflash.h
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

#ifndef __XFLASH_H__
#define __XFLASH_H__

struct magic_rec {
	uint32_t mask;
	uint32_t comp;
};

struct magic_hdr {
	uint16_t pos;
	uint16_t cnt;
};

struct magic {
	struct magic_hdr hdr;
	struct magic_rec rec[];
};

#ifdef __cplusplus
extern "C" {
#endif

void flash_unlock(void);
int flash_write(uint32_t offs, const void * buf, unsigned int len);
int flash_erase(uint32_t offs, unsigned int len);

void uart_reset(void * uart);
void uart_drain(void * uart);
int uart_send(void * uart, const void * buf, unsigned int len);
int uart_recv(void * uart, void * buf, unsigned int len, unsigned int msec);

int usb_send(int ep_id, const void * buf, unsigned int len);
int usb_recv(int ep_id, void * buf, unsigned int len, unsigned int msec);
int usb_drain(int ep_id);

int xflash(uint32_t blk_offs, unsigned int blk_size, 
		   const struct magic * magic);

int yflash(uint32_t blk_offs, unsigned int blk_size, 
		   const struct magic * magic);

#ifdef __cplusplus
}
#endif

#endif /* __XFLASH_H__ */
