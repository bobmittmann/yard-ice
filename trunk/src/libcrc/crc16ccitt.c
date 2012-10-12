/* 
 * File:	crc16ccitt.c
 * Author:  Robinson Mittmann (bobmittmann@gmail.com)
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

/*
   CRC algorithm using the CCITT 16bit polynomial: (X^16 + X^12 + X^5 + 1). 

   This function updates the crc for a single input character. 
   For block computation use crc16ccitt_blk() instead.
 */

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

