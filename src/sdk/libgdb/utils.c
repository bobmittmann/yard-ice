/* 
 * Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
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
 * @file gdb-rsp.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

int uint2hex(char * s, unsigned int val);
int uint2dec(char * s, unsigned int val);
int int2dec(char * s, int val);

unsigned long hex2int(const char * __s, char ** __endp)
{
	unsigned long val = 0;
	char * cp = (char *)__s;
	char c;

	for (; ((c = *cp) == ' '); cp++);

	while ((c = *cp) != '\0') {

		if (c >= 'a')
			c &= ~('a' - 'A');

		if ((c >= 'A') && (c <= 'F'))
			c -= ('A' - 10);
		else {
			if ((c >= '0') && (c <= '9'))
				c -= '0';
			else
				break;
		}

		val = val << 4;
		val += c;
		cp++;
	}

	if (__endp)
		*__endp = cp;

	return val;
}

bool prefix(const char * __s, const char * __prefix)
{
	int c1;
	int c2;

	for (;;) {
		c1 = *__s++;
		c2 = *__prefix++;
	
		if (c2 == '\0')
			return true;

		if ((c2 = c1 - c2) != 0)
			return false;
	} 
}

const char hextab[] = { 
	'0', '1', '2', '3', '4', '5', '6', '7',
	'8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
};

int char2hex(char * pkt, int c)
{
	pkt[0] = hextab[((c >> 4) & 0xf)];
	pkt[1] = hextab[c & 0xf];

	return 2;
}

int short2hex(char * pkt, unsigned int val)
{
	pkt[0] = hextab[((val >> 4) & 0xf)];
	pkt[1] = hextab[val & 0xf];
	pkt[2] = hextab[((val >> 12) & 0xf)];
	pkt[3] = hextab[((val >> 8) & 0xf)];

	return 4;
}

int long2hex(char * pkt, unsigned long val)
{
	pkt[0] = hextab[((val >> 4) & 0xf)];
	pkt[1] = hextab[(val & 0xf)];
	pkt[2] = hextab[((val >> 12) & 0xf)];
	pkt[3] = hextab[((val >> 8) & 0xf)];
	pkt[4] = hextab[((val >> 20) & 0xf)];
	pkt[5] = hextab[((val >> 16) & 0xf)];
	pkt[6] = hextab[((val >> 28) & 0xf)];
	pkt[7] = hextab[((val >> 24) & 0xf)];

	return 8;
}

int str2str(char * pkt, const char * s)
{
	char * cp;
	int n = 0;
	for (cp = (char *)s; *cp != '\0'; ++cp)
		pkt[n++] = *cp;

	return n;
}

int str2hex(char * pkt, const char * s)
{
	char * cp;
	int c;
	int n;

	n = 0;
	for (cp = (char *)s; *cp != '\0'; ++cp) {
		c = hextab[((*cp >> 4) & 0xf)];
		pkt[n++] = c;
		c = hextab[*cp & 0xf];
		pkt[n++] = c;
	}

	return n;
}

int bin2hex(char * pkt, const void * buf, int len)
{
	char * cp = (char *)buf;
	int c;
	int i;

	for (i = 0; i < len; ++i) {
		c = hextab[((cp[i] >> 4) & 0xf)];
		pkt[i * 2] = c;
		c = hextab[cp[i] & 0xf];
		pkt[i * 2 + 1] = c;
	}

	return i * 2;
}

int int2str2hex(char * pkt, unsigned int val)
{
	char s[12];
	int2dec(s, val);
	return str2hex(pkt, s);
}

int uint2hex2hex(char * pkt, unsigned int val)
{
	char s[10];
	uint2hex(s, val);
	return str2hex(pkt, s);
}

inline int hex_digit(int c)
{
	if (c >= 'a')
		c &= ~('a' - 'A');

	if ((c >= 'A') && (c <= 'F'))
		c -= ('A' - 10);
	else if ((c >= '0') && (c <= '9'))
		c -= '0';

	return c;
}

int hex2char(char * hex)
{
	return (hex_digit(hex[0]) << 4) + hex_digit(hex[1]);
}

