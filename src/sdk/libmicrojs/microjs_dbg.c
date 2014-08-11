/* $Id: dcclog.c,v 1.10 2006/09/28 19:31:45 bob Exp $ 
 *
 * File:	tokenizer.c
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define __MICROJS_I__
#include "microjs-i.h"

#include <sys/dcclog.h>

const char tok_str[][4] = {
	"",   /* TOK_NULL */
	">>", /* TOK_ASR */
	"<<", /* TOK_ASL */
	"<=", /* TOK_LTE */
	"<",  /* TOK_LT */
	">=", /* TOK_GTE */
	">",  /* TOK_GT */
	"==", /* TOK_EQ */
	"!=", /* TOK_NEQ */
	"+",  /* TOK_PLUS */
	"-",  /* TOK_MINUS */
	"*",  /* TOK_MUL */
	"/",  /* TOK_DIV */
	"%",  /* TOK_MOD */
	".",  /* TOK_DOT */
	",",  /* TOK_COMMA */
	";",  /* TOK_SEMICOLON */
	":",  /* TOK_COLON */
	"[",  /* TOK_LEFTBRACKET */
	"]",  /* TOK_RIGHTBRACKET */
	"(",  /* TOK_LEFTPAREN */
	")",  /* TOK_RIGHTPAREN */
	"{",  /* TOK_LEFTBRACE */
	"}",  /* TOK_RIGHTBRACE */
	"|",  /* TOK_OR */
	"||", /* TOK_BITOR */
	"&",  /* TOK_AND */
	"&&", /* TOK_BITAND */
	"^",  /* TOK_XOR */
	"!",  /* TOK_NOT */
	"~",  /* TOK_BITNOT */
	"=",  /* TOK_LET */
	"++", /* TOK_INC */
	"--", /* TOK_DEC */
};

int microjs_dump(struct microjs_parser * p)
{
	int idx = 0;
	int tok;
	int len;
	char * s;

	(void)len;

	for (idx = 0; idx < p->cnt; ) {
		tok = p->tok[idx++];
		if (tok & TOK_STRING) {
			len = tok & ~TOK_STRING;
			s = (char *)&p->tok[idx];
			printf("\"%s\" ", s);
			idx += len;
		} else if ((tok & TOK_SYMBOL) == TOK_SYMBOL) {
			len = tok & ~TOK_SYMBOL;
			s = (char *)&p->tok[idx];
			printf("__%s__ ", s);
	//		idx += strlen(s) + 1;
			idx += len;
		} else if (tok == TOK_INT8) {
			uint32_t val;

			val = p->tok[idx++];
			printf("%d ", val);
		} else if (tok == TOK_INT16) {
			uint32_t val;

			val = p->tok[idx++];
			val |= p->tok[idx++] << 8;
			printf("%d ", val);
		} else if (tok == TOK_INT24) {
			uint32_t val;
			
			val = p->tok[idx++];
			val |= p->tok[idx++] << 8;
			val |= p->tok[idx++] << 16;

			printf("%d ", val);
		} else if (tok == TOK_INT32) {
			uint32_t val;
			
			val = p->tok[idx++];
			val |= p->tok[idx++] << 8;
			val |= p->tok[idx++] << 16;
			val |= p->tok[idx++] << 24;

			printf("%d ", val);
		} else if (tok >= TOK_BREAK) {
			s = (char *)microjs_keyword[tok - TOK_BREAK];
			printf("%s ", s);
		} else {
			printf("%s ", tok_str[tok]);
//			if ((tok == TOK_SEMICOLON) || (tok == TOK_LEFTBRACE) 
			if ((tok == TOK_LEFTBRACE) || (tok == TOK_RIGHTBRACE)) {
				printf("\n");
			}
		}
	}

	return 0;
}

int dump_js(char * script, unsigned int len)
{
	int i;

	printf("\n");

	for (i = 0; i < len; ++i)
		printf("%c", script[i]);

	printf("\n");

	return 0;
}

