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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include "calc.h"

#define DEBUG 0
#include "debug.h"

int32_t __rand(void * env, int32_t argv[], int argc) 
{
	return rand();
};

int32_t __srand(void * env, int32_t argv[], int argc) 
{
	srand(argv[0]);
	return 0;
};


int32_t __isqrt(void * env, int32_t argv[], int argc)
{
	uint32_t x = argv[0];
	uint32_t rem = 0;
	uint32_t root = 0;
	int i;

	for (i = 0; i < 16; ++i) {
		root <<= 1;
		rem = ((rem << 2) + (x >> 30));
		x <<= 2;
		root++;
		if (root <= rem) {
			rem -= root;
			root++;
		} else
			root--;
	}

	return root >> 1;
}	

int32_t __ilog2(void * env, int32_t argv[], int argc)
{
	const uint8_t log2_debruijn_index[32] = {
		0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8, 
		31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9 };
	int32_t x = argv[0];

	x |= x >> 1; 
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	x = (x >> 1) + 1;
	x = (x * 0x077cb531UL) >> 27;
	return log2_debruijn_index[x];
}	

int32_t __write(void * env, int32_t argv[], int argc)
{
	int i;

	for(i = argc - 1; i >= 0; --i) {
		if (i < argc - 1)
			printf(", ");
		printf("%d", argv[i]);
	}
	printf("\n");
	return 0;
}	

int32_t __time(void * env, int32_t argv[], int argc)
{
	return (int32_t)time(NULL);
}	

/* --------------------------------------------------------------------------
   Native (external) call table
   -------------------------------------------------------------------------- */

int32_t (* extern_call[])(void *, int32_t argv[], int argc) = {
	[EXT_RAND] = __rand,
	[EXT_SQRT] = __isqrt,
	[EXT_LOG2] = __ilog2,
	[EXT_WRITE] = __write,
	[EXT_TIME] = __time,
	[EXT_SRAND] = __srand,
	[EXT_PRINT] = __write,
};

