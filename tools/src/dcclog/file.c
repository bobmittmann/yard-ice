/* $Id: dcclog.c,v 1.10 2006/09/28 19:31:45 bob Exp $ 
 *
 * File:	dcclog.c
 * Module:
 * Project:	ARM-DCC logger expander
 * Author:	Robinson Mittmann (bob@boreste.com, bob@methafora.com.br)
 * Target:
 * Comment:
 * Copyright(C) 2005 Robinson Mittmann. All Rights Reserved.
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
 * 
 */

#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "dcc.h"

extern const struct dcc_lnk_op file_op;
extern char * prog;

int file_read_addr(FILE * f, uint32_t * addr)
{
	if (fread(addr, 4, 1, f) != 1)
		return -1;
	return 4;
}

uint32_t file_read_u32(FILE * f)
{
	uint32_t val;

	if (fread(&val, 4, 1, f) != 1)
		return 0;

	return val;
}

float file_read_float(FILE * f)
{
	float val;

	if (fread(&val, 4, 1, f) != 1)
		return 0;

	return val;
}


void * file_read_ptr(FILE * f)
{
	uint32_t addr;

	if (fread(&addr, 4, 1, f) != 1)
		return 0;

	return image_ptr(addr);
}

char * file_read_str(FILE * f)
{
	static char s[8192 + 1];
	uint32_t val;
	int i;
	int c;

	i = 0;
	do {
		if (fread(&val, 4, 1, f) != 1)
			return NULL;
		c = val & 0xff;
		if (c != '\0') {
			s[i++] = c;
			c = (val >> 8) & 0xff;
			if (c != '\0') {
				s[i++] = c;
				c = (val >> 16) & 0xff;
				if (c != '\0') {
					s[i++] = c;
					c = (val >> 24)  & 0xff;
				}
			}
		}
		s[i++] = c;
	} while ((c != '\0') && (i < 8192)) ;

	return s;
}

uint8_t * file_read_bin(FILE * f, unsigned int len)
{
	static uint8_t s[8192 + 1];
	uint32_t val;
	int i;
	int c;

	for (i = 0; i < len; ++i) {
		if ((i % 4) == 0) {
			if (fread(&val, 4, 1, f) != 1)
				return NULL;
		}
		c = val & 0xff;
		s[i] = c;
		val >>= 8;
	} 

	return s;
}

void file_close(FILE * f)
{
	fclose(f);
}

const struct dcc_lnk_op file_lnk_op = {
	.read_addr = (void *)file_read_addr,
	.read_u32 = (void *)file_read_u32,
	.read_ptr = (void *)file_read_ptr,
	.read_str = (void *)file_read_str,
	.read_float = (void *)file_read_float,
	.read_bin = (void *)file_read_bin,
	.close = (void *)file_close
};

int dcc_file_link(struct dcc_lnk * lnk, FILE * f)
{
	lnk->op = &file_lnk_op;
	lnk->drv = f;

	return 0;
}

