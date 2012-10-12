/* $Id: fgets.c,v 1.3 2008/04/17 18:24:21 bob Exp $ 
 *
 * File:	fgets.c
 * Module:
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com)
 * Target:	
 * Comment:
 * Copyright(c) 2006-2007 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/file.h>

#define IN_BS      '\x8'
#define IN_DEL      0x7F
#define IN_EOL      '\r'
#define IN_SKIP     '\3'
#define IN_EOF      '\x1A'
#define IN_ESC      '\033'

#define OUT_DEL     "\x8 \x8"
#define OUT_EOL     "\r\n"
#define OUT_SKIP    "^C\r\n"
#define OUT_EOF     "^Z"
#define OUT_BEL     "\7"

char * fgets(char * s, int size, FILE * f)
{
	char buf[1];
	int c;
	int pos;

	/* left room to '\0' */
	size--;

	if (f->op->read(f->data, buf, sizeof(char)) <= 0) {
		return NULL;
	}

	c = buf[0];
	
	pos = strlen(s);

	if ((c == IN_DEL) || (c == IN_BS)) {
		if (pos) {
			s[pos - 1] = '\0';
			f->op->write(f->data, OUT_DEL, sizeof(OUT_DEL) - 1);
		}
		return NULL;
	}

	if (c == IN_ESC) {
		return NULL;
	}

	if (c != IN_EOL) {
		if (pos == size) {
			f->op->write(f->data, OUT_BEL, sizeof(OUT_BEL) - 1);
			return NULL;
		}
		s[pos++] = c;
		s[pos] = '\0';
		f->op->write(f->data, &c, sizeof(char));
		return NULL;
	}

	f->op->write(f->data, OUT_EOL, sizeof(OUT_EOL) - 1);

	return s;
}

