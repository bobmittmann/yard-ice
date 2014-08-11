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

int microjs(struct microjs_parser * p, const char * js, unsigned int len)
{
	int tok;
	int qt;
	int n;
	int c;
	int i;
	
	/* initialize token list */
	p->cnt = 0;

	DCC_LOG(LOG_TRACE, "parse start");
	DCC_LOG1(LOG_TRACE, "script length = %d bytes.", len);

	for (i = 0; i < len; ) {

		c = js[i];
		/* Remove lead blanks */
		if (isspace(c)) {
			i++;
			continue;
		}

		/* Quotes: copy verbatim */
		if ((c == '\'') || (c == '\"')) {
			char * s;
			int j;
			
			DCC_LOG(LOG_TRACE, "string");

			qt = c;
			j = 0;
			s = (char *)&p->tok[p->cnt + 1];
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
			continue;
		}

		/* Symbol */
		if (isalpha(c) || (c == '_')) {
			char * s;
			int k;
			int j;

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
					return -1;
				}

				/* Next digit should be kex digit */
				if (!isxdigit(c = js[i])) {
					DCC_LOG(LOG_WARNING, "invalid constant");
					return -1;
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
		
			p->tok[p->cnt++] = val;
			if ((val & 0xffffff00) == 0) {
				p->tok[p->cnt++] = TOK_INT8;
				continue;
			} 
			
			p->tok[p->cnt++] = val >> 8;
			if ((val & 0xffff0000) == 0) {
				p->tok[p->cnt++] = TOK_INT16;
				continue;
			}

			p->tok[p->cnt++] = val >> 16;
			if ((val & 0xffffff00) == 0) {
				p->tok[p->cnt++] = TOK_INT24;
				continue;
			}

			p->tok[p->cnt++] = val >> 24;
			p->tok[p->cnt++] = TOK_INT32;
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
						return -1;
					}
					c = js[i];

					if (c == '*') {
						if (++i == len)	{
							DCC_LOG(LOG_WARNING, "unclosed comment ");
							return -1;
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
			
			p->tok[p->cnt++] = TOK_DIV;
			continue;
		}

		/* Punctuation */
		if (c == '<') {
			if (++i < len)	{
				c = js[i];
				if  (c == '<') {
					p->tok[p->cnt++] = TOK_SHL;
					i++;
					continue;
				} 
				if  (c == '=') {
					p->tok[p->cnt++] = TOK_LTE;
					i++;
					continue;
				}
			} 
			p->tok[p->cnt++] = TOK_LT;
			continue;
		}

		if (c == '>') {
			if (++i < len)	{
				c = js[i];
				if  (c == '>') {
					p->tok[p->cnt++] = TOK_ASR;
					i++;
					continue;
				} 
				if  (c == '=') {
					p->tok[p->cnt++] = TOK_GTE;
					i++;
					continue;
				}
			} 
			p->tok[p->cnt++] = TOK_GT;
			continue;
		}

		if (c == '=') {
			if ((++i < len) && ((c = js[i]) == '=')) {
				p->tok[p->cnt++] = TOK_EQ;
				i++;
			} else 
				p->tok[p->cnt++] = TOK_LET;
			continue;
		}

		if (c == '!') {
			if ((++i < len) && ((c = js[i]) == '=')) {
				p->tok[p->cnt++] = TOK_NEQ;
				i++;
			} else
				p->tok[p->cnt++] = TOK_NOT;
			continue;
		}

		if (c == '+') {
			if ((++i < len) && ((c = js[i]) == '+')) {
				p->tok[p->cnt++] = TOK_INC;
				i++;
			} else
				p->tok[p->cnt++] = TOK_PLUS;
			continue;
		}

		if (c == '-') {
			if ((++i < len) && ((c = js[i]) == '-')) {
				p->tok[p->cnt++] = TOK_DEC;
				i++;
			} else
				p->tok[p->cnt++] = TOK_MINUS;
			continue;
		}

		if (c == '|') {
			if ((++i < len) && ((c = js[i]) == '|')) {
				p->tok[p->cnt++] = TOK_OR;
				i++;
			} else
				p->tok[p->cnt++] = TOK_BITOR;
			continue;
		}

		if (c == '&') {
			if ((++i < len) && ((c = js[i]) == '&')) {
				p->tok[p->cnt++] = TOK_AND;
				i++;
			} else
				p->tok[p->cnt++] = TOK_BITAND;
			continue;
		}

		switch (c) {
		case '\0':
			continue;
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
		default:
			DCC_LOG1(LOG_WARNING, "invalid character: '%c'.", c);
			return -1;
		}
			
		p->tok[p->cnt++] = tok;
		i++;
	}


	DCC_LOG1(LOG_INFO, "token stream length = %d bytes.", 
			(TOK_BUF_LEN - tok_sp) + tok_idx);
	DCC_LOG(LOG_INFO, "parse done.");

	return n;
}

