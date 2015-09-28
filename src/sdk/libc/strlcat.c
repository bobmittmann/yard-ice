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
 * @file strlcat.c
 * @brief YARD-ICE libc
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 


#include <stdlib.h>
#include <string.h>

/* FIXME:  this implementation is wrong */
size_t strlcat(char * restrict dst, const char * restrict src, size_t size)
{
	register char *cp = dst;

	if ((!cp) || (!src))
		return 0;

	while (*cp)
    	cp++;

	while (size-- != 0 && (*cp++ = *src++)) {
		if (n == 0)
			*cp = '\0';
    }

	return cp - dst;
}
