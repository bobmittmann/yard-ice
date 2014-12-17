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
#include <string.h>
#include <ctype.h>

#include "calc.h"

const struct {
	uint8_t typ;
	char nm[9];
} lex_keyword[] = {
	{ T_ELSE,  "else" },
	{ T_FALSE, "false" },
	{ T_FOR,   "for" },
	{ T_IF,    "if" },
	{ T_TRUE,  "true" },
	{ T_VAR,   "var" },
	{ T_WHILE, "while" },
	{ T_PRINT, "print" },
};

int lexer_open(struct lexer * lex, const char * txt, unsigned int len)
{
	/* set the base javascript file reference */
	lex->txt = txt;
	lex->len = len;
	lex->off = 0;
	return 0;
}

struct token lexer_scan(struct lexer * lex)
{
	struct token tok;
	unsigned int off;
	const char * txt;
	unsigned int len;
	unsigned int qlf;
	unsigned int typ;
	int c;
	
	/* initialize variables */
	qlf = 0;
	typ = T_ERR;
	txt = lex->txt;
	len = lex->len;

	for (off = lex->off; off < len; ) {
		c = txt[off];

		/* Remove lead blanks */
		if (isspace(c)) {
			off++;
			continue;
		}

		/* Quotes: copy verbatim */
		if ((c == '\'') || (c == '\"')) {
			unsigned int k;
			unsigned int n; /* length of the symbol */
			int qt = c; /* quote character */

			k = off + 1;
			do {
				if (++off == len) {
					/* parse error, unclosed quotes */
					qlf = ERR_UNCLOSED_STRING;
					goto ret;
				}
				c = txt[off];
			} while (c != qt);

			n = off - k;
			qlf = n;
			typ = T_STRING;
			tok.s = (char *)&txt[k];
			off++;
			goto ret;
		}

		/* keywork or identifier */
		if (isalpha(c) || (c == '_')) {
			unsigned int k;
			unsigned int n; /* length of the symbol */
			char * s;

			k = off;
			do {
				c = txt[++off];
			} while (isalnum(c) || (c == '_'));
			n = off - k;
			s = (char *)&txt[k];

			qlf = n;
			typ = T_ID;
			tok.s = (char *)&txt[k];

			/* look up in the kwywords table */
			for (k = 0; k <= (sizeof(lex_keyword) / 10); ++k) {
				if ((strncmp(lex_keyword[k].nm, s, n) == 0) 
					&& (lex_keyword[k].nm[n] == '\0')) {
					typ = lex_keyword[k].typ;
					tok.s = (char *)lex_keyword[k].nm;
				}
			}

			goto ret;
		}

		/* number */
		if (isdigit(c)) {
			uint32_t val = 0;
			
			if ((c == '0') && ((off + 1) < len) && 
				((txt[off + 1] == 'x') || (txt[off + 1] == 'X'))) {

				/* Hexadecimal */

				/* Skip 0x prefix. */
				off += 2;
				if (off == len) {
					qlf = ERR_INVALID_LITERAL;
					goto ret;
				}

				/* Next digit should be kex digit */
				if (!isxdigit(c = txt[off])) {
					
					qlf = ERR_INVALID_LITERAL;
					goto ret;
				}

				
				do {
					val = val << 4;
					
					if (c >= 'a')
						c -= ('a' - 10);
					else if (c >= 'A')
						c -= ('A' - 10);
					else 
						c -= '0';
					val += c;
					if (++off == len)	
						break;
					c = txt[off];
				} while (isxdigit(c));

				
			} else {
				/* Decimal */
				do {
					val = ((val << 2) + val) << 1;
					val += c - '0';
					if (++off == len)	
						break;
					c = txt[off];
				} while (isdigit(c));
			}

			if (isalpha(c) || (c == '_')) {
				/* catch errrors like: '123abc' */ 
				qlf = ERR_INVALID_LITERAL;
				goto ret;
			}
		
			typ = T_INT;
			tok.u32 = val;
			goto ret;
		}
	
		/* Comments or divider */
		if ((c == '/') && (++off < len)) {

			c = txt[off];
			if  (c == '/') {
				
				/* Single line comment */
				while ((c != '\n') && (++off < len)) {
					c = txt[off];
				}
				continue;
			} 
			
			if  (c == '*') {
				
				/* Multi-line comment */
				for (;;) {
					/* Single line comment */
					if (++off == len)	{
						
						qlf = ERR_UNCLOSED_COMMENT;
						goto ret;
					}
					c = txt[off];

					if (c == '*') {
						if (++off == len)	{
							
							qlf = ERR_UNCLOSED_COMMENT;
							goto ret;
						}
						c = txt[off];
						if (c == '/') {
							off++;
							break;
						}
					}
				}
				continue;
			} 
			
			typ = T_DIV;
			goto ret;
		}

		/* Punctuation */
		if (c == '<') {
			if (++off < len)	{
				c = txt[off];
				if  (c == '<') {
					typ = T_SHL;
					goto inc_ret;
				} 
				if  (c == '=') {
					typ = T_LTE;
					goto inc_ret;
				}
			} 
			typ = T_LT;
			goto ret;
		}

		if (c == '>') {
			if (++off < len)	{
				c = txt[off];
				if  (c == '>') {
					typ = T_ASR;
					goto inc_ret;
				} 
				if  (c == '=') {
					typ = T_GTE;
					goto inc_ret;
				}
			} 
			typ = T_GT;
			goto ret;
		}

		if (c == '=') {
			if ((++off < len) && ((c = txt[off]) == '=')) {
				typ = T_EQU;
				goto inc_ret;
			}
			typ = T_ASSIGN;
			goto ret;
		}

		if (c == '!') {
			if ((++off < len) && ((c = txt[off]) == '=')) {
				typ = T_NEQ;
				goto inc_ret;
			}
			typ = T_NOT;
			goto ret;
		}

		if (c == '|') {
			if (++off < len)	{
				if  (c == '|') {
					typ = T_LOR;
					goto inc_ret;
				}
			}
			typ = T_OR;
			goto ret;
		}

		if (c == '&') {
			if (++off < len)	{
				if  (c == '&') {
					typ = T_LAND;
					goto inc_ret;
				}
			}
			typ = T_AND;
			goto ret;
		}

		switch (c) {
		case ',':
			typ = T_COMMA;
			break;
		case ';':
			typ = T_SEMICOLON;
			break;
		case '^':
			typ = T_XOR;
			break;
		case '~':
			typ = T_INV;
			break;
		case '*':
			typ = T_MUL;
			break;
		case '%':
			typ = T_MOD;
			break;
		case '+':
			typ = T_PLUS;
			break;
		case '-':
			typ = T_MINUS;
			break;
		case '(':
			typ = T_LPAREN;
			break;
		case ')':
			typ = T_RPAREN;
			break;
		case '{':
			typ = T_LBRACE;
			break;
		case '}':
			typ = T_RBRACE;
			break;
		default:
			typ = T_ERR;
			qlf = ERR_UNEXPECTED_CHAR;
			goto ret;
		}

		goto inc_ret;
	}
	typ = T_EOF;
	goto ret;

inc_ret:
	/* increment the index pointer and ret a typen into the buffer */
	off++;

ret:
	lex->off = off;
	tok.typ = typ;
	tok.qlf = qlf;
	return tok;
}

