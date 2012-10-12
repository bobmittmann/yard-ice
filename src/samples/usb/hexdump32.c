/* 
 * File:	 hexdump32.c
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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

void show_hex32(uint32_t  addr, const void * buf, int count)
{
	uint32_t base;
	uint8_t * p;
	uint32_t val;
	int i;
	int j;
	int n;
	int c;

	/* 32bits addr alignement */
	addr &= ~(sizeof(uint32_t) - 1);
	/* 16 bytes base alignment */
	base = addr & ~(16 - 1);

	if (base < addr) {
		j = (addr - base);
		n = count + j;
	} else {
		j = 0;
		n = count;
	}

	p = (uint8_t *)buf; 

	printf("%08x: ", base);

	for (i = 0; i < n; i += sizeof(uint32_t)) {
		if (i < j) {
			printf("________ ");
		} else {
			val = p[0] + (p[1] << 8) + (p[2] << 16) + (p[3] << 24);
			printf("%08x ", val);
			p += sizeof(uint32_t);
		}
	}

	for (; i < 16; i += 4) {
		printf("         ");
	}

	printf(": ");

	p = (uint8_t *)buf; 
	for (i = 0; i < n; i++) {
		if (i == 8)
			printf(" ");

		if (i < j) {
			printf(".");
		} else {
			c = *p++;
			printf("%c", ((c < ' ') || (c > 127)) ? '.' : c);
		}
	}
	printf("\n");
}

int hexdump32(uint32_t addr, int count)
{
	uint8_t buf[16];
	unsigned int base;
	unsigned int n;
	
	n = (count + (sizeof(uint32_t) - 1)) & ~(sizeof(uint32_t) - 1);

	addr &= ~(sizeof(uint32_t) - 1);
	base = addr & ~(16 - 1);
	if (base < addr) {
		n = (base + 16) - addr;
	} else {
		n = 16;
	}

	do {
		if (count < n)
			n = count;

		memcpy(buf, (void *)addr, n);

		show_hex32(addr, buf, n);
		addr += n;
		count -= n;
		n = 16;
	} while (count > 0);

	return 0;
}

