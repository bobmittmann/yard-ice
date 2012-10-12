/*
 * File:	memcpy.c
 * Module:
 * Project:
 * Author:	Robinson Mittmann (bob@boreste.com)
 * Target:	
 * Comment:
 * Copyright(c) 2003-2006 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#include <stdint.h>
#include <string.h>

void * __memcpy(void * dst, const void * src, size_t n)
{
	register uint8_t *cpsrc = (uint8_t *)src;
	register uint8_t *cpdst = (uint8_t *)dst;
	register uint8_t *cpend = (uint8_t *)src + n;

	while (cpsrc != cpend)
    	*cpdst++ = *cpsrc++;

	return dst;
}

void * memcpy(void *, const void *, size_t) 
	__attribute__ ((weak, alias ("__memcpy")));

