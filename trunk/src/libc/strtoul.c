/* 
 * File:	strtoul.c
 * Module:
 * Project:
 * Author:	Robinson Mittmann (bob@boreste.com, bobmittmann@gmail.com)
 * Target:	
 * Comment:	
 * Copyright(c) 2003-2009 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */
 
#include <stdlib.h>

unsigned long strtoul(const char * __s, char ** __endp, int base)
{
	char c;
	char * cp = (char *)__s;
	int val = 0;

	for (; ((c = *cp) == ' '); cp++);

	if (base == 0) {
		if (*cp == '0') {
			cp++;
			if ((*cp & ~('a' - 'A')) == 'X') {
				base = 16;
				cp++;
			} else 
				base = 8;
		} else
			base = 10;
	}

	switch (base) {
		while ((c = *cp) != '\0') {
			if ((c < '0') || (c > '7'))
				break;
			val <<= 3;
			val += c - '0';
			cp++;
		}
		break;
	case 10:
		while ((c = *cp) != '\0') {
			if ((c < '0') || (c > '9'))
				break;
			val = ((val << 2) + val) << 1;
			val += c - '0';
			cp++;
		}
		break;
	case 16:
		while ((c = *cp) != '\0') {
			if (c >= 'a')
				c &= ~('a' - 'A');

			if ((c >= 'A') && (c <= 'F'))
				c -= ('A' - 10);
			else {
				if ((c >='0') && (c <= '9'))
					c -= '0';
				else
					break;
			}

			val = val << 4;
			val += c;
			cp++;
		}
		break;
	}
	
	if (__endp)
		*__endp = cp;

	return val;
}

