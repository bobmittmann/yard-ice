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
 * @file ull2dec.c
 * @brief YARD-ICE libc
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

static const unsigned long long __lldiv_tab[21] = {
	10000000000000000000ULL, 
	8000000000000000000ULL, 
	800000000000000000ULL, 
	80000000000000000ULL, 
	8000000000000000ULL, 
	800000000000000ULL, 
	80000000000000ULL, 
	8000000000000ULL, 
	800000000000ULL, 
	80000000000ULL, 
	8000000000ULL, 
	800000000ULL, 
	80000000ULL, 
	8000000LL, 
	800000LL, 
	80000LL, 
	8000LL, 
	800LL, 
	80LL, 
	8LL, 
	0LL };

int ull2dec(char * s, unsigned long long val)
{
	unsigned long long * dvp;
	unsigned long long dv;
	int n;
	int c;
	int i;

	if (!val) {
		*s++ = '0';
		*s = '\0';
		return 1;
	}

	dvp = (unsigned long long *)__lldiv_tab;
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

