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
#include <ctype.h>

#define __MICROJS_I__
#include "microjs-i.h"

#ifndef MICROJS_ENABLE_INCDEC
#define MICROJS_ENABLE_INCDEC 0
#endif

#ifndef MICROJS_ENABLE_STRING
#define MICROJS_ENABLE_STRING 0
#endif

#include <sys/dcclog.h>

const char microjs_keyword[11][9] = {
	"break",
	"case",
	"continue",
	"false",

	"for",
	"function",
	"return",
	"switch",

	"true",
	"var",
	"while",
};

int microjs_init(struct microjs_parser * p, uint8_t * tok, unsigned int size)
{
	p->cnt = 0;
	p->size = size;
	p->tok = tok;
	p->js = NULL;

	DCC_LOG2(LOG_TRACE, "tok=0x%08x size=%d", tok, size);

	return 0;
}

int microjs_str_decode(struct microjs_parser * p, 
					   const char * js, unsigned int len, 
					   unsigned int offs)
{
#if MICROJS_ENABLE_DECODE_STRING
	uint16_t offs;
	char * s;
	int qt;
	int j;

	qt = c;
	j = 0;
	DCC_LOG(LOG_TRACE, "string");
	s = (char *)&p->tok[p->cnt + 1];
	/* string offset in the file */
	offs = i + 1;
	for (;;) {
		if (++i == len) {
			/* parse error, unclosed quotes */
			DCC_LOG(LOG_WARNING, "unclosed quotes");
			return -1;
		}
		c = js[i];
		if (c == qt) {
			i++;
			break;
		}
		s[j++] = c;
	}
	s[j++] = '\0';
	p->tok[p->cnt++] = TOK_STRING + j;
	p->cnt += j;
#endif
	return 0;
}

int microjs_parse(struct microjs_parser * p, 
				  const char * js, unsigned int len)
{
	unsigned int i;
	unsigned int tok;
	int err;
	int c;
	
	/* initialize token list */
	p->cnt = 0;
	/* set the base javascript file reference */
	p->js = js;

	DCC_LOG(LOG_TRACE, "parse start");
	DCC_LOG1(LOG_TRACE, "script length = %d bytes.", len);

	for (i = 0; i < len; ) {

		c = js[i];
		/* Remove lead blanks */
		if (isspace(c)) {
			i++;
			continue;
		}

		DCC_LOG1(LOG_MSG, "c=0x%02x", c);

		/* Quotes: copy verbatim */
		if ((c == '\'') || (c == '\"')) {
#if MICROJS_ENABLE_STRING
			unsigned int offs = i + 1; /* string offset in the file */
			unsigned int j = 0; /* length of the string */
			int qt = c; /* quote character */

			DCC_LOG(LOG_INFO, "string");
			for (;;) {
				if (++i == len) {
					/* parse error, unclosed quotes */
					DCC_LOG(LOG_WARNING, "unclosed quotes");
					err = MICROJS_UNCLOSED_STRING;
					goto error;
				}
				c = js[i];
				if (c == qt) {
					i++;
					break;
				}
				j++;
			}

			if ((p->size - p->cnt) < 3) {
				DCC_LOG(LOG_WARNING, "token buffer overflow!");
				err = MICROJS_TOKEN_BUF_OVF;
				goto error;
			}

			p->tok[p->cnt++] = TOK_STRING + j;
			p->tok[p->cnt++] = offs;
			p->tok[p->cnt++] = (offs >> 8);
			continue;
#else
			DCC_LOG(LOG_WARNING, "unsupported string!");
			err = MICROJS_STRINGS_UNSUPORTED;
			goto error;
#endif /* MICROJS_ENABLE_STRING */
		}

		/* Symbol */
		if (isalpha(c) || (c == '_')) {
			unsigned int j;
			char * s;
			int k;

			if ((p->size - p->cnt) < MICROJS_SYMBOL_LEN_MAX) {
				DCC_LOG(LOG_WARNING, "token buffer overflow!");
				err = MICROJS_TOKEN_BUF_OVF;
				goto error;
			}

			j = 0;
			s = (char *)&p->tok[p->cnt + 1];
			do {
				s[j++] = c;
				if (++i == len)	
					break;
				c = js[i];
			} while (isalnum(c) || (c == '_'));
			s[j++] = '\0';

			/* look up in the kwywords table */
			for (k = 0; k <= (TOK_WHILE - TOK_BREAK); ++k) {
				if (strcmp(microjs_keyword[k], s) == 0) {
					DCC_LOG(LOG_INFO, "Keyword");
					p->tok[p->cnt++] = k + TOK_BREAK;
					break;
				}
			}

			if (k > (TOK_WHILE - TOK_BREAK)) {
				/* symbol */
				DCC_LOG(LOG_INFO, "Symbol");
				p->tok[p->cnt++] = TOK_SYMBOL + j;
				p->cnt += j;
			}

			continue;
		}

		/* Number */
		if (isdigit(c)) {
			uint32_t val = 0;

			DCC_LOG(LOG_INFO, "Number");
			if ((c == '0') && ((i + 1) < len) && 
				((js[i + 1] == 'x') || (js[i + 1] == 'X'))) {

				/* Hexadecimal */

				/* Skip 0x prefix. */
				i += 2;
				if (i == len) {
					DCC_LOG(LOG_WARNING, "invalid constant");
					err = MICROJS_INVALID_LITERAL;
					goto error;
				}

				/* Next digit should be kex digit */
				if (!isxdigit(c = js[i])) {
					DCC_LOG(LOG_WARNING, "invalid constant");
					err = MICROJS_INVALID_LITERAL;
					goto error;
				}

				DCC_LOG(LOG_INFO, "Hexadecimal");
				do {
					val = val << 4;
					DCC_LOG1(LOG_INFO, "Hex: %c", c);
					if (c >= 'a')
						c -= ('a' - 10);
					else if (c >= 'A')
						c -= ('A' - 10);
					else 
						c -= '0';
					val += c;
					if (++i == len)	
						break;
					c = js[i];
				} while (isxdigit(c));

				DCC_LOG(LOG_INFO, "Hexadecimal");
			} else {
				/* Decimal */
				do {
					val = ((val << 2) + val) << 1;
					val += c - '0';
					if (++i == len)	
						break;
					c = js[i];
				} while (isdigit(c));
			}
		
			if ((p->size - p->cnt) < 5) {
				DCC_LOG(LOG_WARNING, "token buffer overflow!");
				err = MICROJS_TOKEN_BUF_OVF;
				goto error;
			}

			if ((val & 0xffffff00) == 0) {
				p->tok[p->cnt++] = TOK_INT8;
				p->tok[p->cnt++] = val;
				continue;
			} 
			
			if ((val & 0xffff0000) == 0) {
				p->tok[p->cnt++] = TOK_INT16;
				p->tok[p->cnt++] = val;
				p->tok[p->cnt++] = val >> 8;
				continue;
			}

			if ((val & 0xffffff00) == 0) {
				p->tok[p->cnt++] = TOK_INT24;
				p->tok[p->cnt++] = val;
				p->tok[p->cnt++] = val >> 8;
				p->tok[p->cnt++] = val >> 16;
				continue;
			}

			p->tok[p->cnt++] = TOK_INT32;
			p->tok[p->cnt++] = val;
			p->tok[p->cnt++] = val >> 8;
			p->tok[p->cnt++] = val >> 16;
			p->tok[p->cnt++] = val >> 24;
			continue;
		}
	
		/* Comments */
		if ((c == '/') && (++i < len)) {

			c = js[i];
			if  (c == '/') {
				DCC_LOG(LOG_INFO, "Single line comment");
				/* Single line comment */
				while ((c != '\n') && (++i < len)) {
					c = js[i];
				}
				continue;
			} 
			
			if  (c == '*') {
				DCC_LOG(LOG_INFO, "Multi line comment");
				/* Multi-line comment */
				for (;;) {
					/* Single line comment */
					if (++i == len)	{
						DCC_LOG(LOG_WARNING, "unclosed comment ");
						err = MICROJS_UNCLOSED_COMMENT;
						goto error;
					}
					c = js[i];

					if (c == '*') {
						if (++i == len)	{
							DCC_LOG(LOG_WARNING, "unclosed comment ");
							err = MICROJS_UNCLOSED_COMMENT;
							goto error;
						}
						c = js[i];
						if (c == '/') {
							i++;
							break;
						}
					}
				}
				continue;
			} 
			
			tok = TOK_DIV;
			goto push;
		}

		/* Punctuation */
		if (c == '<') {
			if (++i < len)	{
				c = js[i];
				if  (c == '<') {
					tok = TOK_SHL;
					goto inc_push;
				} 
				if  (c == '=') {
					tok = TOK_LTE;
					goto inc_push;
				}
			} 
			tok = TOK_LT;
			goto push;
		}

		if (c == '>') {
			if (++i < len)	{
				c = js[i];
				if  (c == '>') {
					tok = TOK_ASR;
					goto inc_push;
				} 
				if  (c == '=') {
					tok = TOK_GTE;
					goto inc_push;
				}
			} 
			tok = TOK_GT;
			goto push;
		}

		if (c == '=') {
			if ((++i < len) && ((c = js[i]) == '=')) {
				tok = TOK_EQ;
				goto inc_push;
			}
			tok = TOK_LET;
			goto push;
		}

		if (c == '!') {
			if ((++i < len) && ((c = js[i]) == '=')) {
				tok = TOK_NEQ;
				goto inc_push;
			}
			tok = TOK_NOT;
			goto push;
		}

#if MICROJS_ENABLE_INCDEC
		if (c == '+') {
			if ((++i < len) && ((c = js[i]) == '+')) {
				tok = TOK_INC;
				goto inc_push;
			}
			tok = TOK_PLUS;
			goto push;
		}

		if (c == '-') {
			if ((++i < len) && ((c = js[i]) == '-')) {
				tok = TOK_DEC;
				goto inc_push;
			}
			tok = TOK_MINUS;
			goto push;
		}
#endif

		if (c == '|') {
			if ((++i < len) && ((c = js[i]) == '|')) {
				tok = TOK_OR;
				goto inc_push;
			}
			tok = TOK_BITOR;
			goto push;
		}

		if (c == '&') {
			if ((++i < len) && ((c = js[i]) == '&')) {
				tok = TOK_AND;
				goto inc_push;
			}
			tok = TOK_BITAND;
			goto push;
		}

		switch (c) {
		case '\0':
			tok = TOK_NULL;
			break;
		case '.':
			tok = TOK_DOT;
			break;
		case ',':
			tok = TOK_COMMA;
			break;
		case ';':
			tok = TOK_SEMICOLON;
			break;
		case ':':
			tok = TOK_COLON;
			break;
		case '[':
			tok = TOK_LEFTBRACKET;
			break;
		case ']':
			tok = TOK_RIGHTBRACKET;
			break;
		case '(':
			tok = TOK_LEFTPAREN;
			break;
		case ')':
			tok = TOK_RIGHTPAREN;
			break;
		case '{':
			tok = TOK_LEFTBRACE;
			break;
		case '}':
			tok = TOK_RIGHTBRACE;
			break;
		case '^':
			tok = TOK_XOR;
			break;
		case '~':
			tok = TOK_BITNOT;
			break;
		case '*':
			tok = TOK_MUL;
			break;
		case '%':
			tok = TOK_MOD;
			break;
#if (!MICROJS_ENABLE_INCDEC)
		case '+':
			tok = TOK_PLUS;
			break;
		case '-':
			tok = TOK_MINUS;
			break;
#endif
		default:
			DCC_LOG1(LOG_WARNING, "invalid character: '%c'.", c);
			err = MICROJS_UNEXPECTED_CHAR;
			goto error;
		}

inc_push:
		i++;
push:
		if ((p->size - p->cnt) < 1) {
			DCC_LOG(LOG_WARNING, "token buffer overflow!");
			err = MICROJS_TOKEN_BUF_OVF;
			goto error;
		}
		p->tok[p->cnt++] = tok;

		DCC_LOG1(LOG_INFO, "%s", microjs_tok_str[tok]);
	}

	DCC_LOG1(LOG_INFO, "%s", microjs_tok_str[tok]);

	DCC_LOG1(LOG_TRACE, "token stream length = %d bytes.", p->cnt);
	DCC_LOG(LOG_INFO, "parse done.");

	return p->cnt;

error:
	p->err_offs = i;
	p->err_code = err;
	return -1;
}

