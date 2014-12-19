/* 
 * Copyright(c) 2004-2012 BORESTE (www.boreste.com). All Rights Reserved.
 *
 * This file is part of the YARD-ICE
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You can receive a copy of the GNU Lesser General Public License from 
 * http://www.gnu.org/
 */

/** 
 * @file crc16ccitt.c
 * @brief YARD-ICE libcrc
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

/* CRC algorithm using the CCITT 16bit polynomial: (X^16 + X^12 + X^5 + 1). */
unsigned int crc16ccitt(unsigned int crc, const void * buf, int len)
{
	unsigned char * cp;
	unsigned int c;
	int i;

	cp = (unsigned char *)buf;

	for (i = 0; i < len; i++) {
		c = cp[i];
		crc = (crc >> 8) | ((crc & 0xff) << 8);
		crc ^= c;
		crc ^= (crc & 0xff) >> 4;
		crc ^= (crc & 0x0f) << 12;
		crc ^= (crc & 0xff) << 5;
	}

	return crc;
}

