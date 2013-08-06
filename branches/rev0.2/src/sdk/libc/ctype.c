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
	_C, _C, _C, _C, _C, _C, _C, _C,
	_C, _C + _S, _C + _S, _C + _S, _C + _S, _C + _S, _C, _C,
	_C, _C, _C, _C, _C, _C, _C, _C,
	_C, _C, _C, _C, _C, _C, _C, _C,
	_S + _B, _P, _P, _P, _P, _P, _P, _P,
	_P, _P, _P, _P, _P, _P, _P, _P,
	_N, _N, _N, _N, _N, _N, _N, _N,
	_N, _N, _P, _P, _P, _P, _P, _P,
	_P, _U + _X, _U + _X, _U + _X, _U + _X, _U + _X, _U + _X, _U,
	_U, _U, _U, _U, _U, _U, _U, _U,
	_U, _U, _U, _U, _U, _U, _U, _U,
	_U, _U, _U, _P, _P, _P, _P, _P,
	_P, _L + _X, _L + _X, _L + _X, _L + _X, _L + _X, _L + _X, _L,
	_L, _L, _L, _L, _L, _L, _L, _L,
	_L, _L, _L, _L, _L, _L, _L, _L,
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

