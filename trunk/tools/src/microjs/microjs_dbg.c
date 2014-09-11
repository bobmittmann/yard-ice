/* 
 * Copyright(C) 2014 Robinson Mittmann. All Rights Reserved.
 * 
 * This file is part of the MicroJs
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
 * @file microjs-i.h
 * @brief Syntax-directed translation compiler
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define __MICROJS_I__
#include "microjs-i.h"

static const char * const err_tab[] = {
	"Ok", 
	"unexpected char",
	"unclosed string",
	"unclosed comment",
	"invalid literal",
	"invalid identifier",
	"strings unsuported",
	"string too long",
	"bracket mismatch",
	"syntax error",
};

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

void lexer_print_err(FILE * f, struct lexer * lex, int err)
{
	char * txt = (char *)lex->txt;
	unsigned int len = lex->len;
	char * lp[5];
	int ln;
	int c;
	int i;

	fprintf(f, "error: %s:\n", err_tab[err]);

	lp[4] = NULL;
	lp[3] = NULL;
	lp[2] = NULL;
	lp[1] = NULL;
	lp[0] = txt;
	ln = 1;
	for (i = 0; ((c = txt[i]) != '\0') && (i < len); ++i) {
		if (txt[i - 1] == '\n') {
			lp[4] = lp[3];
			lp[3] = lp[2];
			lp[2] = lp[1];
			lp[1] = lp[0];
			lp[0] = &txt[i];
			ln++;
		}
		if (i == lex->off) {
			js_dump_line(f, ln - 4, lp[4]);
			js_dump_line(f, ln - 3, lp[3]);
			js_dump_line(f, ln - 2, lp[2]);
			js_dump_line(f, ln - 1, lp[1]);
			js_dump_line(f, ln - 0, lp[0]);
			break;
		}
	}
}

void dump_src(const char * txt, unsigned int len)
{
	bool crlf = false;
	int ln;
	int c;
	int i;

	ln = 0;
	crlf = true;
	for (i = 0; ((c = txt[i]) != '\0') && (i < len); ++i) {
		if (crlf) {
			printf("%4d: ", ++ln);
			crlf = false;
		}
		if (c == '\r')
			continue;
		if (c == '\n')
			crlf = true;

		printf("%c", c);
	}

	printf("\n");
	fflush(stdout);
}

#define STRING_LEN_MAX 64

const char token_nm[][4] = {
	[T_EOF] = "EOF",
	[T_DOT] = ".",
	[T_COMMA] = ",",
	[T_SEMICOLON] = ";",
	[T_COLON] = ":",
	[T_LBRACKET] = "[",
	[T_RBRACKET] = "]",
	[T_LPAREN] = "(",
	[T_RPAREN] = ")",
	[T_LBRACE] = "{",
	[T_RBRACE] = "}",
	[T_ASR] = ">>",
	[T_SHL] = "<<",
	[T_LTE] = "<=",
	[T_LT] = "<",
	[T_GTE] = ">=",
	[T_GT] = ">",
	[T_EQU] = "==",
	[T_NEQ] = "!=",
	[T_PLUS] = "+",
	[T_MINUS] = "-",
	[T_MUL] = "*",
	[T_DIV] = "/",
	[T_MOD] = "%",
	[T_OR] = "|",
	[T_LOR] = "||",
	[T_AND] = "&",
	[T_LAND] = "&&",
	[T_XOR] = "^",
	[T_NOT] = "!",
	[T_INV] = "~",
	[T_QUEST] = "?",
	[T_ASSIGN] = "=",
};

char * tok2str(struct token tok)
{
	static char buf[STRING_LEN_MAX + 3];
	unsigned int typ = tok.typ;

	if (typ == T_ERR) {
		sprintf(buf, "ERR: %s", err_tab[tok.qlf]);
	} else if (typ == T_ID) {
		unsigned int n = tok.qlf;
		memcpy(buf, tok.s, n);
		buf[n] = '\0';
	} else if (typ == T_INT) {
		sprintf(buf, "%d", tok.u32);
	} else
		sprintf(buf, "%s", token_nm[typ]);

	return buf;
}

int ll_stack_dump(FILE * f, uint8_t * sp, unsigned int cnt)
{
	int i;

	for (i = cnt - 1; i >= 0; --i) {
		fprintf(f, "\t%s\n", ll_sym_tab[sp[i]]);
	};

	return 0;
}

