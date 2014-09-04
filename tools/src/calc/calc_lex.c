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

#include "config.h"
#include "calc.h"

const char lexer_keyword[15][9] = {
	"break",
	"case",
	"continue",
	"const",

	"else",
	"false",
	"for",
	"function",

	"if",
	"null",
	"return",
	"switch",

	"true",
	"var",
	"while",
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
	typ = TOK_ERR;
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
			unsigned int n; /* length of the string */
			int qt = c; /* quote character */
			
			k = off + 1;
			for (;;) {
				if (++off == len) {
					/* end of text, parse error, unclosed quotes */
					
					qlf = ERR_UNCLOSED_STRING;
					goto ret;
				}
				c = txt[off];
				if (c == qt) {
					off++;
					break;
				}
			}
			n = off - k;

			if (n > STRING_LEN_MAX) {
				
				qlf = ERR_STRING_TOO_LONG;
				goto ret;
			}

			typ = TOK_STRING;
			qlf = n;
			tok.s = (char *)&txt[k];
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
			typ = TOK_ID;
			tok.s = (char *)&txt[k];

			/* look up in the kwywords table */
			for (k = 0; k <= (TOK_WHILE - TOK_BREAK); ++k) {
				if (strncmp(lexer_keyword[k], s, n) == 0 &&
					lexer_keyword[k][n] == '\0') {
					typ = k + TOK_BREAK;
					tok.s = (char *)lexer_keyword[k];
					break;
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
		
			typ = TOK_INT;
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
			
			typ = TOK_DIV;
			goto ret;
		}

		/* Punctuation */
		if (c == '<') {
			if (++off < len)	{
				c = txt[off];
				if  (c == '<') {
					typ = TOK_SHL;
					goto inc_ret;
				} 
				if  (c == '=') {
					typ = TOK_LTE;
					goto inc_ret;
				}
			} 
			typ = TOK_LT;
			goto ret;
		}

		if (c == '>') {
			if (++off < len)	{
				c = txt[off];
				if  (c == '>') {
					typ = TOK_ASR;
					goto inc_ret;
				} 
				if  (c == '=') {
					typ = TOK_GTE;
					goto inc_ret;
				}
			} 
			typ = TOK_GT;
			goto ret;
		}

		if (c == '=') {
			if ((++off < len) && ((c = txt[off]) == '=')) {
				typ = TOK_EQ;
				goto inc_ret;
			}
			typ = TOK_ASSIGN;
			goto ret;
		}

		if (c == '!') {
			if ((++off < len) && ((c = txt[off]) == '=')) {
				typ = TOK_NEQ;
				goto inc_ret;
			}
			typ = TOK_NOT;
			goto ret;
		}

		if (c == '|') {
			if (++off < len)	{
				if  (c == '|') {
					typ = TOK_OR;
					goto inc_ret;
				}
			}
			typ = TOK_BITOR;
			goto ret;
		}

		if (c == '&') {
			if (++off < len)	{
				if  (c == '&') {
					typ = TOK_AND;
					goto inc_ret;
				}
			}
			typ = TOK_BITAND;
			goto ret;
		}

		switch (c) {
		case '.':
			typ = TOK_DOT;
			break;
		case ',':
			typ = TOK_COMMA;
			break;
		case ';':
			typ = TOK_SEMICOLON;
			break;
		case ':':
			typ = TOK_COLON;
			break;
		case '[':
			typ = TOK_LEFTBRACKET;
			break;
			break;
		case ']':
			typ = TOK_RIGHTBRACKET;
			break;
		case '(':
			typ = TOK_LEFTPAREN;
			break;
		case ')':
			typ = TOK_RIGHTPAREN;
			break;
		case '{':
			typ = TOK_LEFTBRACE;
			break;
		case '}':
			typ = TOK_RIGHTBRACE;
			break;
		case '^':
			typ = TOK_XOR;
			break;
		case '~':
			typ = TOK_BITINV;
			break;
		case '*':
			typ = TOK_MUL;
			break;
		case '%':
			typ = TOK_MOD;
			break;
		case '+':
			typ = TOK_PLUS;
			break;
		case '-':
			typ = TOK_MINUS;
			break;
		default:
			typ = TOK_ERR;
			qlf = ERR_UNEXPECTED_CHAR;
			goto ret;
		}

		goto inc_ret;
	}
	typ = TOK_EOF;
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

