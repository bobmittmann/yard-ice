/* 
 * File:	string.h
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(C) 2011 Bob Mittmann. All Rights Reserved.
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
 */



#ifndef __STRING_H__
#define	__STRING_H__

#define __need_size_t
#include <stddef.h>

#define DEBRUIJN32 0x077cb531UL
/* debruijn32 = 0000 0111 0111 1100 1011 0101 0011 0001 */

/*
extern inline int ffs(int i) {
extern const char ffs_debruijn_index[32];

	if (i)
		return ffs_debruijn_index[((i & -i) * DEBRUIJN32) >> 27];
	return 0;
}
*/

#ifdef __cplusplus
extern "C" {
#endif

long int strtol(const char *nptr, char **endptr, int base);
	
size_t strlen(const char * s);

char * strcat(char * dst, const char * src);

int strcmp(const char * s1, const char * s2);

char * strchr(const char * s, int c);

char * strcpy(char * dst, const char * src);

char * strtok(char *, const char *);

void * memcpy(void * dst, const void * src, size_t n);

void * memset(void * s, int c, size_t n);

int memcmp(const void * s1, const void * s2, size_t n);

int ffs(int i);

#ifdef __cplusplus
}
#endif
#endif /* __STRING_H__ */

