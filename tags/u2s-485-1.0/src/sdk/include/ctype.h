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
 * @file ctype.h
 * @brief YARD-ICE libc
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __CTYPE_H__
#define __CTYPE_H__

#define	_U	0x01
#define	_L	0x02
#define	_N	0x04
#define	_S	0x08
#define _P	0x10
#define _C	0x20
#define _X	0x40
#define	_B	0x80

#define isalnum(C)	(ctype_lut[(unsigned)(C)] & (_U + _L + _N))

#define iscntrl(C)	(ctype_lut[(unsigned)(C)] & _C)

#define	isalpha(C)	(ctype_lut[(unsigned)(C)] & (_U + _L))

#define	isdigit(C)	(ctype_lut[(unsigned)(C)] & _N)

#define	isgraph(C)	(ctype_lut[(unsigned)(C)] & (_P + _U + _L + _N))

#define	islower(C)	(ctype_lut[(unsigned)(C)] & _L)

#define isprint(C)	(ctype_lut[(unsigned)(C)] & (_P + _U + _L + _N + _B))

#define ispunct(C)	(ctype_lut[(unsigned)(C)] & _P)

#define	isspace(C)	(ctype_lut[(unsigned)(C)] & _S)

#define	isupper(C)	(ctype_lut[(unsigned)(C)] & _U)

#define	isxdigit(C)	(ctype_lut[(unsigned)(C)] & (_X + _N))

#define toupper(C) ({ islower(C) ? ((C) - 'a' + 'A') : (C);})

#define tolower(C) ({ isupper(C) ? ((C) - 'A' + 'a') : (C);})

extern const char ctype_lut[];

#endif /* __CTYPE_H__ */

