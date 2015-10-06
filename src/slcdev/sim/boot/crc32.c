/* 
 * File:	 usb-test.c
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

#include <sys/stm32f.h>
#include <stdint.h>

uint32_t crc32(void  * buf, unsigned int len)
{
	struct stm32_crc * crc = STM32_CRC;
	uint8_t * src = (uint8_t *)buf;
	uint8_t * mrk;

	crc->cr = CRC_RESET;

	mrk = src + (len & ~3);
	while (src != mrk) {
		crc->dr = src[0] + (src[1] << 8) + (src[2] << 16) + (src[3] << 24);
		src += 4;
	}

	switch (len & 3) {
	case 3:
		crc->dr = src[0] + (src[1] << 8) + (src[2] << 16);
		break;
	case 2:
		crc->dr = src[0] + (src[1] << 8);
		break;
	case 1:
		crc->dr = src[0];
		break;
	}

	return crc->dr;
}

uint32_t crc32_align(void  * buf, unsigned int len)
{
	struct stm32_crc * crc = STM32_CRC;
	uint32_t * src = (uint32_t *)buf;
	uint32_t * mrk = src + (len >> 2);

	crc->cr = CRC_RESET;
	while (src != mrk)
		crc->dr = *src++;
	
	if (len & 3)
		crc->dr = *src & (0xffffffff >> ((4 - (len & 3)) * 8));

	return crc->dr;
}

