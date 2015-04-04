/* 
 * Copyright(c) 2005-2012 BORESTE (www.boreste.com). All Rights Reserved.
 *
 * This file is part of the YARD-ICE.
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
 * @file calloc.c
 * @brief YARD-ICE libc
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <stdint.h>
#include <stdlib.h>

void * calloc(size_t __nmemb, size_t __size)
{
	unsigned int nbytes;
	uint8_t * ptr;
	uint8_t * zp;

	nbytes = __nmemb * __size;
	ptr = malloc(nbytes);
	zp = ptr;

	do {
		unsigned int mctmp = nbytes;
		int mcn;
		int z = 0;

		if (mctmp < 8)
			mcn = 0;
		else { 
			mcn = (mctmp-1)/8; 
			mctmp %= 8; 
		}
		switch (mctmp) {
		case 0: for(;;) { *zp++ = z;
		case 7:           *zp++ = z;
		case 6:           *zp++ = z;
		case 5:           *zp++ = z;
		case 4:           *zp++ = z;
		case 3:           *zp++ = z;
		case 2:           *zp++ = z;
		case 1:           *zp++ = z; 
						  if(mcn <= 0) 
							  break; 
						  mcn--; }
		}
	} while(0);

	return ptr;
}

