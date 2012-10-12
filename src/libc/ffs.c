/*
 *
 * File:    ffs.c
 * Module:
 * Project:
 * Author:	Robinson Mittmann (bob@boreste.com, bobmittmann@gmail.com)
 * Target:
 * Comment: 
 * Copyright(c) 2008 BORESTE (wwww.boreste.com). All Rights Reserved.
 *
 */

#include <string.h>

/* table to convert debruijn index to standard index */

const char __attribute__((section(".uthreads_data"))) ffs_debruijn_index[32] = 
{
	1, 2, 29, 3, 30, 15, 25, 4, 31, 23, 21, 16, 26, 18, 5, 9, 
	32, 28, 14, 24, 22, 20, 17, 8, 27, 13, 19, 7, 12, 6, 11, 10
} ;

int __attribute__((section(".uthreads_code"))) ffs(int i)
{
	if (i)
		return ffs_debruijn_index[((i & -i) * DEBRUIJN32) >> 27];

	return 0;
}

