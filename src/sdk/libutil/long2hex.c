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
 * @file long2hex.c
 * @brief YARD-ICE libc
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

extern const char __hextab[];

int long2hex_be(char * s, unsigned long val)
{
	s[0] = __hextab[((val >> 4) & 0xf)];
	s[1] = __hextab[(val & 0xf)];
	s[2] = __hextab[((val >> 12) & 0xf)];
	s[3] = __hextab[((val >> 8) & 0xf)];
	s[4] = __hextab[((val >> 20) & 0xf)];
	s[5] = __hextab[((val >> 16) & 0xf)];
	s[6] = __hextab[((val >> 28) & 0xf)];
	s[7] = __hextab[((val >> 24) & 0xf)];

	return 8;
}

int long2hex_le(char * s, unsigned long val)
{
	s[0] = __hextab[((val >> 28) & 0xf)];
	s[1] = __hextab[((val >> 24) & 0xf)];
	s[2] = __hextab[((val >> 20) & 0xf)];
	s[3] = __hextab[((val >> 16) & 0xf)];
	s[4] = __hextab[((val >> 12) & 0xf)];
	s[5] = __hextab[((val >> 8) & 0xf)];
	s[6] = __hextab[((val >> 4) & 0xf)];
	s[7] = __hextab[(val & 0xf)];

	return 8;
}

