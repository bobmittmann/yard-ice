/* $Id: ilog2.S,v 1.15 2008/06/08 01:18:27 bob Exp $ 
 *
 * File:    ilog2.c
 * Module:
 * Project:
 * Author:	Robinson Mittmann (bob@boreste.com, bobmittmann@gmail.com)
 * Target:
 * Comment: 
 * Copyright(c) 2009 BORESTE (wwww.boreste.com). All Rights Reserved.
 *
 */

#include <string.h>
#include <stdio.h>

/* table to convert debruijn index to standard index */

const char log2_debruijn_index[32] = 
{
  0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8, 
  31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
};

int ilog2(int v)
{
	int x;

	v |= v >> 1; // first round down to power of 2 
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	v = (v >> 1) + 1;

	x = (v * 0x077cb531UL) >> 27;

	return log2_debruijn_index[x];
}

