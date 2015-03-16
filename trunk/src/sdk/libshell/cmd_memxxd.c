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
 * @file cmd_reboot.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <sys/shell.h>

static void show_hex32(FILE * f, uint32_t  addr, const void * buf, int count)
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

	fprintf(f, "%08x: ", base);

	p = (uint8_t *)buf; 
	for (i = 0; i < n; i += sizeof(uint32_t)) {
		if (i < j) {
			fprintf(f, "________ ");
		} else {
			val = p[0] + (p[1] << 8) + (p[2] << 16) + (p[3] << 24);
			fprintf(f, "%08x ", val);
			p += sizeof(uint32_t);
		}
	}

	for (; i < 16; i += 4) {
		fprintf(f, "         ");
	}

	fprintf(f, ": ");

	p = (uint8_t *)buf; 
	for (i = 0; i < n; i++) {
		if (i == 8)
			fprintf(f, " ");

		if (i < j) {
			fprintf(f, ".");
		} else {
			c = *p++;
			fprintf(f, "%c", ((c < ' ') || (c > 126)) ? '.' : c);
		}
	}
	fprintf(f, "\n");
}

int cmd_memxxd(FILE *f, int argc, char ** argv)
{
	uint8_t buf[16];
	unsigned int base;
	unsigned int n;
	unsigned int addr;
	unsigned int count;

	if (argc < 2)
		return SHELL_ERR_ARG_MISSING;

	if (argc > 3)
		return SHELL_ERR_EXTRA_ARGS;

	addr = strtoul(argv[1], NULL, 0);

	if (argc == 3)
		count = strtoul(argv[2], NULL, 0);
	else
		count = 64;

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

		show_hex32(f, addr, buf, n);
		addr += n;
		count -= n;
		n = 16;
	} while (count > 0);


	return 0;
}


