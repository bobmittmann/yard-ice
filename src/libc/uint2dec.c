/* 
 * Copyright(c) 2004-2012 BORESTE (www.boreste.com). All Rights Reserved.
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
 * @file uint2dec.c
 * @brief YARD-ICE libc
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

static const unsigned int __div_tab[11] = {
	4000000000U, 
	800000000, 
	80000000, 
	8000000, 
	800000, 
	80000, 
	8000, 
	800, 
	80, 
	8, 
	0 };

int uint2dec(char * s, unsigned int val)
{
	unsigned int * dvp;
	unsigned int dv;
	int n;
	int c;
	int i;

	if (!val) {
		*s++ = '0';
		*s = '\0';
		return 1;
	}

	dvp = (unsigned int *)__div_tab;
	n = 0;
	c = 0;
	i = 4;
	while ((dv = *dvp++)) {	
		c = 0;
		for (; (i); i >>= 1) {
			if (val >= dv) {
				val -= dv;
				c += i;
			}
			dv >>= 1;
		}
		if ((c) || (n)) {
			*s++ = c + '0';
			n++;
		}
		i = 8;
	};
	*s = '\0';

	return n;
}

