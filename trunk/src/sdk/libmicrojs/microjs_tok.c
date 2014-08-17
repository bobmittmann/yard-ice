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

const char microjs_keyword[12][9] = {
	"break",
	"case",
	"continue",
	"const",

	"false",
	"for",
	"function",
	"return",

	"switch",
	"true",
	"var",
	"while",
};

int microjs_tok_init(struct microjs_tokenizer * tkn, 
					 uint8_t * tok, unsigned int size)
{
	tkn->cnt = 0;
	tkn->offs = 0;
	tkn->err = 0;
	tkn->size = size;
	tkn->tok = tok;
	tkn->js = NULL;

	DCC_LOG2(LOG_TRACE, "tok=0x%08x size=%d", tok, size);

	return 0;
}

#define MICROJS_BRACKET_STACK_SIZE 32

int microjs_tokenize(struct microjs_tokenizer * tkn, 
					 const char * js, unsigned int len)
{
	uint8_t bkt_tok[MICROJS_BRACKET_STACK_SIZE];
	unsigned int bkt_sp;
	unsigned int ltok;
	unsigned int tok;
	unsigned int i;
	int err;
	int c;
	
	/* initialize token list */
	tkn->cnt = 0;
	/* set the base javascript file reference */
	tkn->js = js;

	DCC_LOG(LOG_TRACE, "parse start");
	DCC_LOG1(LOG_TRACE, "script length = %d bytes.", len);

	/* initialize bracket matching stack pointer */
	bkt_sp = 0;
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

			if (j > MICROJS_STRING_LEN_MAX) {
				DCC_LOG(LOG_WARNING, "string too long!");
				err = MICROJS_STRING_TOO_LONG;
				goto error;
			}

			if ((tkn->size - tkn->cnt) < 3) {
				DCC_LOG(LOG_WARNING, "token buffer overflow!");
				err = MICROJS_TOKEN_BUF_OVF;
				goto error;
			}

			tkn->tok[tkn->cnt++] = TOK_STRING + j;
			tkn->tok[tkn->cnt++] = offs;
			tkn->tok[tkn->cnt++] = (offs >> 8);
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

			if ((tkn->size - tkn->cnt) < MICROJS_SYMBOL_LEN_MAX) {
				DCC_LOG(LOG_WARNING, "token buffer overflow!");
				err = MICROJS_TOKEN_BUF_OVF;
				goto error;
			}

			j = 0;
			s = (char *)&tkn->tok[tkn->cnt + 1];
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
					tkn->tok[tkn->cnt++] = k + TOK_BREAK;
					break;
				}
			}

			if (k > (TOK_WHILE - TOK_BREAK)) {
				/* symbol */
				DCC_LOG(LOG_INFO, "Symbol");
				tkn->tok[tkn->cnt++] = TOK_SYMBOL + j - 1;
				tkn->cnt += j;
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
		
			if ((tkn->size - tkn->cnt) < 5) {
				DCC_LOG(LOG_WARNING, "token buffer overflow!");
				err = MICROJS_TOKEN_BUF_OVF;
				goto error;
			}

			if ((val & 0xffffff00) == 0) {
				tkn->tok[tkn->cnt++] = TOK_INT8;
				tkn->tok[tkn->cnt++] = val;
				continue;
			} 
			
			if ((val & 0xffff0000) == 0) {
				tkn->tok[tkn->cnt++] = TOK_INT16;
				tkn->tok[tkn->cnt++] = val;
				tkn->tok[tkn->cnt++] = val >> 8;
				continue;
			}

			if ((val & 0xffffff00) == 0) {
				tkn->tok[tkn->cnt++] = TOK_INT24;
				tkn->tok[tkn->cnt++] = val;
				tkn->tok[tkn->cnt++] = val >> 8;
				tkn->tok[tkn->cnt++] = val >> 16;
				continue;
			}

			tkn->tok[tkn->cnt++] = TOK_INT32;
			tkn->tok[tkn->cnt++] = val;
			tkn->tok[tkn->cnt++] = val >> 8;
			tkn->tok[tkn->cnt++] = val >> 16;
			tkn->tok[tkn->cnt++] = val >> 24;
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
			if (++i < len)	{
				if  (c == '+') {
					tok = TOK_INC;
					goto inc_push;
				}
				if  (c == '=') {
					tok = TOK_PLUS_LET;
					goto inc_push;
				}
			}
			tok = TOK_PLUS;
			goto push;
		}

		if (c == '-') {
			if (++i < len)	{
				if  (c == '-') {
					tok = TOK_DEC;
					goto inc_push;
				}
				if  (c == '=') {
					tok = TOK_MINUS_LET;
					goto inc_push;
				}
			}
			tok = TOK_MINUS;
			goto push;
		}
#endif

		if (c == '|') {
			if (++i < len)	{
				if  (c == '|') {
					tok = TOK_OR;
					goto inc_push;
				}
#if MICROJS_ENABLE_INCDEC
				if  (c == '=') {
					tok = TOK_OR_LET;
					goto inc_push;
				}
#endif
			}
			tok = TOK_BITOR;
			goto push;
		}

		if (c == '&') {
			if (++i < len)	{
				if  (c == '&') {
					tok = TOK_AND;
					goto inc_push;
				}
#if MICROJS_ENABLE_INCDEC
				if  (c == '=') {
					tok = TOK_AND_LET;
					goto inc_push;
				}
#endif
			}
			tok = TOK_BITAND;
			goto push;
		}

		switch (c) {
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
			goto bkt_push;
		case ']':
			tok = TOK_RIGHTBRACKET;
			ltok = TOK_LEFTBRACKET;
			goto bkt_pop;
		case '(':
			tok = TOK_LEFTPAREN;
			goto bkt_push;
		case ')':
			tok = TOK_RIGHTPAREN;
			ltok = TOK_LEFTPAREN;
			goto bkt_pop;
		case '{':
			tok = TOK_LEFTBRACE;
			goto bkt_push;
		case '}':
			tok = TOK_RIGHTBRACE;
			ltok = TOK_LEFTBRACE;
			goto bkt_pop;
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
		/* increment the index pointer and push a token into the buffer */
		i++;
push:
		/* push a token into the buffer */
		if ((tkn->size - tkn->cnt) < 1) {
			DCC_LOG(LOG_WARNING, "token buffer overflow!");
			err = MICROJS_TOKEN_BUF_OVF;
			goto error;
		}
		tkn->tok[tkn->cnt++] = tok;

		DCC_LOG1(LOG_INFO, "%s", microjs_tok_str[tok]);
	}

	/* the matching bracket stack must be empty at this point */
	if (bkt_sp != 0) {
		DCC_LOG(LOG_WARNING, "bracket closing mismatch!");
		err = MICROJS_BRACKET_MISMATCH;
		goto error;
	}

	DCC_LOG1(LOG_INFO, "%s", microjs_tok_str[tok]);

	/* push a token into the buffer */
	if ((tkn->size - tkn->cnt) < 1) {
		DCC_LOG(LOG_WARNING, "token buffer overflow!");
		err = MICROJS_TOKEN_BUF_OVF;
		goto error;
	}
	tkn->tok[tkn->cnt++] = TOK_EOF;

	DCC_LOG1(LOG_TRACE, "token stream length = %d bytes.", tkn->cnt);
	DCC_LOG(LOG_INFO, "parse done.");

	return tkn->cnt;

bkt_push:
	/* insert a brakcet into the stack */
	if (bkt_sp == MICROJS_BRACKET_STACK_SIZE) {
		DCC_LOG(LOG_WARNING, "maximum nesting level exceeded!");
		err = MICROJS_MAX_NEST_LEVEL;
		goto error;
	}
	bkt_tok[bkt_sp++] = tok;
	goto inc_push;

bkt_pop:
	/* push a brakcet from the stack and check for matching pair */
	if ((bkt_sp == 0) || (bkt_tok[--bkt_sp] != ltok)) {
		DCC_LOG(LOG_WARNING, "bracket closing mismatch!");
		err = MICROJS_BRACKET_MISMATCH;
		goto error;
	}
	goto inc_push;

error:
	tkn->offs = i;
	tkn->err = err;

	return -1;
}

int microjs_token_get(struct microjs_tokenizer * tkn, 
					  struct microjs_val * val)
{
	unsigned int offs;
	int idx = 0;
	uint32_t x;
	int tok;
	int len;
	(void)len;

//	idx = tkn->idx;
	tok = tkn->tok[idx++];
	if (tok >= TOK_STRING) {
		len = tok - TOK_STRING;
		offs = tkn->tok[idx++];
		offs |= tkn->tok[idx++] << 8;
		val->str.dat = (char *)tkn->js + offs;
		val->str.len = len;
		tok = TOK_STRING;
	} else if (tok >= TOK_SYMBOL) {
		len = tok - TOK_SYMBOL + 1;
		val->str.dat = (char *)&tkn->tok[idx];
		val->str.len = len;
		idx += len;
		tok = TOK_SYMBOL;
	} else if (tok >= TOK_INT8) {
		x = tkn->tok[idx++];
		if (tok >= TOK_INT16) {
			x |= tkn->tok[idx++] << 8;
			if (tok == TOK_INT24) {
				x |= tkn->tok[idx++] << 16;
				if (tok >= TOK_INT32)
					x |= tkn->tok[idx++] << 24;
			}
		} 
		val->u32 = x;
		tok = TOK_INT32;
	} 

//	tkn->idx = idx;

	return tok;
}

