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
 * @file ctype.c
 * @brief YARD-ICE libc
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <ctype.h>

const char ctype_lut[256] = {
/*	NUL SOH STX ETX EOT ENQ ACK BEL */
	 _C, _C, _C, _C, _C, _C, _C, _C,
/*	BS  TAB LF  VT  FF  CR  SO  SI  */
	_C, _C + _S, _C + _S, _C + _S, _C + _S, _C + _S, _C, _C,
/*	DLE DC1 DC2 DC3 DC4 NAK SYN ETB */
	_C, _C, _C, _C, _C, _C, _C, _C,
/*	CAN EM  SUB ESC FS  GS  RS  US  */
	_C, _C, _C, _C, _C, _C, _C, _C,
/*	' ' !   "   #   $   %   &   '   */
	_S + _B, _P, _P, _P, _P, _P, _P, _P,
/*	(   )   *   +   ,   -   .   /   */
	_P, _P, _P, _P, _P, _P, _P, _P,
/*	0   1   2   3   4   5   6   7   */
	_N, _N, _N, _N, _N, _N, _N, _N,
/*	8   9   :   ;   <   =   >   ?   */
	_N, _N, _P, _P, _P, _P, _P, _P,
/*	@   A   B   C   D   E   F   G   */
	_P, _U + _X, _U + _X, _U + _X, _U + _X, _U + _X, _U + _X, _U,
/*	H   I   J   K   L   M   N   O   */
	_U, _U, _U, _U, _U, _U, _U, _U,
/*	P   Q   R   S   T   U   V   W   */
	_U, _U, _U, _U, _U, _U, _U, _U,
/*	X   Y   Z   [   \   ]   ^   _   */
	_U, _U, _U, _P, _P, _P, _P, _P,
/*	`   a   b   c   d   e   f   g   */
	_P, _L + _X, _L + _X, _L + _X, _L + _X, _L + _X, _L + _X, _L,
/*	h   i   j   k   l   m   n   o   */
	_L, _L, _L, _L, _L, _L, _L, _L,
/*	p   q   r   s   t   u   v   w   */
	_L, _L, _L, _L, _L, _L, _L, _L,
/*	x   y   z   {   |   }   ~   DEL */
	_L, _L, _L, _P, _P, _P, _P, _C,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0
};

