/* $Id: cmd_memrd.c,v 1.1 2008/06/08 01:17:42 bob Exp $
 *
 * File:	cmd_memrd.c
 * Module:
 * Project:
 * Author:	Robinson Mittmann (bob@boreste.com, bob@methafora.com.br)
 * Target:
 * Comment:
 * Copyright(c) 2008 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

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

	fprintf(f, "%08x: ", (unsigned int)base);

	p = (uint8_t *)buf;
	for (i = 0; i < n; i += sizeof(uint32_t)) {
		if (i < j) {
			fprintf(f, "________ ");
		} else {
			val = p[0] + (p[1] << 8) + (p[2] << 16) + (p[3] << 24);
			fprintf(f, "%08x ", (unsigned int)val);
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
	fprintf(f, "\r\n");
}

int hexdump(FILE * f, unsigned int addr, void * ptr, unsigned int count)
{
	uint8_t buf[16];
	unsigned int base;
	unsigned int n;
	uint8_t * p;

	p = (uint8_t *)(((uintptr_t)ptr) & ~(sizeof(uint32_t) - 1));
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
		memcpy(buf, p, n);
		show_hex32(f, addr, buf, n);
		p += n;
		addr += n;
		count -= n;
		n = 16;
	} while (count > 0);

	return 0;
}
