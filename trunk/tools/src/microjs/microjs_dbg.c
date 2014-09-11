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

const char microjs_tok_str[][4] = {
	"EOF", /* TOK_EOF */
	".",   /* TOK_DOT */
	",",   /* TOK_COMMA */
	";",   /* TOK_SEMICOLON */
	":",   /* TOK_COLON */
	"[",   /* TOK_LEFTBRACKET */
	"]",   /* TOK_RIGHTBRACKET */
	"(",   /* TOK_LEFTPAREN */
	")",   /* TOK_RIGHTPAREN */
	"{",   /* TOK_LEFTBRACE */
	"}",   /* TOK_RIGHTBRACE */
	">>",  /* TOK_ASR */
	"<<",  /* TOK_ASL */
	"<=",  /* TOK_LTE */
	"<",   /* TOK_LT */
	">=",  /* TOK_GTE */
	">",   /* TOK_GT */
	"==",  /* TOK_EQ */
	"!=",  /* TOK_NEQ */
	"+",   /* TOK_PLUS */
	"-",   /* TOK_MINUS */
	"*",   /* TOK_MUL */
	"/",   /* TOK_DIV */
	"%",   /* TOK_MOD */
	"|",   /* TOK_OR */
	"||",  /* TOK_BITOR */
	"&",   /* TOK_AND */
	"&&",  /* TOK_BITAND */
	"^",   /* TOK_XOR */
	"!",   /* TOK_NOT */
	"~",   /* TOK_BITNOT */
#if 0
	"?",   /* TOK_QUEST */
#endif
	"="   /* TOK_ASSIGN */
};

int microjs_tok_print(FILE * f, struct microjs_parser * p, int idx)
{
	uint32_t val;
	int tok;
	int len;
	char * s;

	tok = p->tok[idx++];
	if (tok >= TOK_STRING) {
		unsigned int offs;
		char buf[MICROJS_STRING_LEN_MAX + 1];
		len = tok - TOK_STRING;
		offs = p->tok[idx++];
		offs |= p->tok[idx++] << 8;
		s = (char *)p->txt + offs;
		memcpy(buf, s, len);
		buf[len] = '\0';
		fprintf(f, "\"%s\" ", buf);
	} else if (tok >= TOK_ID) {
		len = tok - TOK_ID + 1;
		s = (char *)&p->tok[idx];
		fprintf(f, "%s ", s);
		idx += len;
	} else if (tok == TOK_INT8) {
		val = p->tok[idx++];
		fprintf(f, "%d ", val);
	} else if (tok == TOK_INT16) {
		val = p->tok[idx++];
		val |= p->tok[idx++] << 8;
		fprintf(f, "%d ", val);
	} else if (tok == TOK_INT24) {
		val = p->tok[idx++];
		val |= p->tok[idx++] << 8;
		val |= p->tok[idx++] << 16;
		fprintf(f, "%d ", val);
	} else if (tok == TOK_INT32) {
		val = p->tok[idx++];
		val |= p->tok[idx++] << 8;
		val |= p->tok[idx++] << 16;
		val |= p->tok[idx++] << 24;
		fprintf(f, "%d ", val);
	} else if (tok >= TOK_BREAK) {
		s = (char *)microjs_keyword[tok - TOK_BREAK];
		fprintf(f, "%s ", s);
	} else if (tok == TOK_LEFTBRACE) {
		fprintf(f, "%s", microjs_tok_str[tok]);
	} else if (tok == TOK_RIGHTBRACE) {
		fprintf(f, "%s", microjs_tok_str[tok]);
	} else if (tok == TOK_SEMICOLON) {
		fprintf(f, ";");
	} else
		fprintf(f, "%s ", microjs_tok_str[tok]);

	return 0;
}

int microjs_tok_dump(FILE * f, struct microjs_parser * p)
{
	uint32_t val;
	int idx = 0;
	int tok;
	int len;
	char * s;
	int lvl = 0;
	int i;
	bool nl = true;

	for (idx = 0; idx < p->cnt; ) {

		if (nl) {
			fprintf(f, "\n");
			for (i = 0; i < lvl; ++i)
				fprintf(f, "    ");
			nl = false;
		}

		tok = p->tok[idx++];
		if (tok >= TOK_STRING) {
			unsigned int offs;
			char buf[MICROJS_STRING_LEN_MAX + 1];
			len = tok - TOK_STRING;
			offs = p->tok[idx++];
			offs |= p->tok[idx++] << 8;
			s = (char *)p->txt + offs;
			memcpy(buf, s, len);
			buf[len] = '\0';
			fprintf(f, "\"%s\" ", buf);
		} else if (tok >= TOK_ID) {
			len = tok - TOK_ID + 1;
			s = (char *)&p->tok[idx];
			fprintf(f, "%s ", s);
	//		idx += strlen(s) + 1;
			idx += len;
		} else if (tok == TOK_INT8) {
			val = p->tok[idx++];
			fprintf(f, "%d ", val);
		} else if (tok == TOK_INT16) {
			val = p->tok[idx++];
			val |= p->tok[idx++] << 8;
			fprintf(f, "%d ", val);
		} else if (tok == TOK_INT24) {
			val = p->tok[idx++];
			val |= p->tok[idx++] << 8;
			val |= p->tok[idx++] << 16;
			fprintf(f, "%d ", val);
		} else if (tok == TOK_INT32) {
			val = p->tok[idx++];
			val |= p->tok[idx++] << 8;
			val |= p->tok[idx++] << 16;
			val |= p->tok[idx++] << 24;

			fprintf(f, "%d ", val);
		} else if (tok >= TOK_BREAK) {
			s = (char *)microjs_keyword[tok - TOK_BREAK];
			fprintf(f, "%s ", s);
		} else if (tok == TOK_LEFTBRACE) {
			fprintf(f, "%s", microjs_tok_str[tok]);
			lvl++;
			nl = true;
		} else if (tok == TOK_RIGHTBRACE) {
			fprintf(f, "%s", microjs_tok_str[tok]);
			lvl--;
			nl = true;
		} else if (tok == TOK_SEMICOLON) {
			fprintf(f, ";");
			nl = true;
		} else
			fprintf(f, "%s ", microjs_tok_str[tok]);
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


static void js_dump_line(FILE * f, int ln, char * lp)
{
	char * cp;
	int c;

	if (lp == NULL)
		return;

	fprintf(f, "%4d: ", ln);
	for (cp = lp; (c = *cp) != '\0'; ++cp) {
		if ((c == '\r') || (c == '\n'))
			break;
		fprintf(f, "%c", c);
	}
	fprintf(f, "\n");
}

static const char * const err_tab[] = {
	"Ok", 
	"unexpected char",
	"token buffer overflow",
	"unclosed string",
	"unclosed comment",
	"invalid literal",
	"invalid identifier",
	"bracket mismatch",
	"empty source file",
	"empty token stack",
	"strings unsuported",
	"string too long",
	"invalid label",
	"object expected"
};

void microjs_print_err(FILE * f, struct microjs_parser * p, int err)
{
	char * js = (char *)p->txt;
	char * lp[5];
	int ln;
	int c;
	int i;

	fprintf(f, "error: %s: ", err_tab[-err]);

	lp[4] = NULL;
	lp[3] = NULL;
	lp[2] = NULL;
	lp[1] = NULL;
	lp[0] = js;
	ln = 1;
	for (i = 0; (c = js[i]) != '\0'; ++i) {
		if (js[i - 1] == '\n') {
			lp[4] = lp[3];
			lp[3] = lp[2];
			lp[2] = lp[1];
			lp[1] = lp[0];
			lp[0] = &js[i];
			ln++;
		}
		if (i == p->off) {
			js_dump_line(f, ln - 4, lp[4]);
			js_dump_line(f, ln - 3, lp[3]);
			js_dump_line(f, ln - 2, lp[2]);
			js_dump_line(f, ln - 1, lp[1]);
			js_dump_line(f, ln - 0, lp[0]);
			break;
		}
	}
}
