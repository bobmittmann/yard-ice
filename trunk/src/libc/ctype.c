/* $Id: ctype.c,v 2.1 2006/06/14 20:51:51 bob Exp $ 
 *
 * File:	ctype.c
 * Module:	ulibc
 * Project:
 * Author:	Robinson Mittmann (bob@boreste.com)
 * Target:	
 * Comment:
 * Copyright(c) 2003-2006 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#include <ctype.h>

const char _ctype_[256] = {
	_C, _C, _C, _C, _C, _C, _C, _C,
	_C, _C | _S, _C | _S, _C | _S, _C | _S, _C | _S, _C, _C,
	_C, _C, _C, _C, _C, _C, _C, _C,
	_C, _C, _C, _C, _C, _C, _C, _C,
	_S | _B, _P, _P, _P, _P, _P, _P, _P,
	_P, _P, _P, _P, _P, _P, _P, _P,
	_N, _N, _N, _N, _N, _N, _N, _N,
	_N, _N, _P, _P, _P, _P, _P, _P,
	_P, _U | _X, _U | _X, _U | _X, _U | _X, _U | _X, _U | _X, _U,
	_U, _U, _U, _U, _U, _U, _U, _U,
	_U, _U, _U, _U, _U, _U, _U, _U,
	_U, _U, _U, _P, _P, _P, _P, _P,
	_P, _L | _X, _L | _X, _L | _X, _L | _X, _L | _X, _L | _X, _L,
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

