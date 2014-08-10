/* $Id: dcclog.c,v 1.10 2006/09/28 19:31:45 bob Exp $ 
 *
 * File:	dcclog.c
 * Module:
 * Project:	ARM-DCC logger expander
 * Author:	Robinson Mittmann (bob@boreste.com, bob@methafora.com.br)
 * Target:
 * Comment:
 * Copyright(C) 2005 Robinson Mittmann. All Rights Reserved.
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
 * 
 */

#include <unistd.h>
#include <inttypes.h>
#include <stdbool.h>
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#if defined(WIN32)
  #include <io.h>
  #include <fcntl.h>
#else
  #include <assert.h>
#endif

#define VERSION_MAJOR 2
#define VERSION_MINOR 0

const char ctype_lut[128] = {
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
	_L, _L, _L, _P, _P, _P, _P, _C
};

int basic_tokenizer(char * script, unsigned int len)
{
	return 0;
}

const char * const keywords[] = {
	"for",
	"function"
	"if",
	"var",
	"while",
};

int microjs(char * script, unsigned int len)
{
	char * cp;
	int c;

	cp = script;

	printf("\n");

	for (i = 0; i < len; ++i)
		printf("%c", script[i]);

	printf("\n");

	for (;;) {
		/* search for any word */
		for (;;) {
			c = script[i];
			if ((c != ' ') || (c != '\t') || (c != '\r') || (c != '\n'))
				break;
			if (++i == len)
				return;
		}

		switch (c) {
		case '{':
			break;

		case 'f':
			break;

		case 'v':
			break;
		}

	}

	while ((c == ' ') || (c == '\t') || (c == '\r') && (c == '\n'))

	} ((c == ' ') || (c == '\t') || (c == '\r') && (c == '\n'))
	return 0;
}

