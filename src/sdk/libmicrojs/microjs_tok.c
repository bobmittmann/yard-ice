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

const char microjs_keyword[13][9] = {
	"break",
	"case",
	"continue",
	"const",

	"false",
	"for",
	"function",
	"null",

	"return",
	"switch",
	"true",
	"var",

	"while",
};

int microjs_init(struct microjs_parser * jp, 
					 uint8_t * tok, unsigned int size)
{
	jp->cnt = 0;
	jp->off = 0;
	jp->top = size;
	jp->sp = size;
	jp->tok = tok;
	jp->len = 0;
	jp->txt = NULL;

	DCC_LOG2(LOG_TRACE, "tok=0x%08x size=%d", tok, size);

	return MICROJS_OK;
}

int microjs_open(struct microjs_parser * jp, 
				 const char * txt, unsigned int len)
{
	/* set the base javascript file reference */
	jp->txt = txt;
	jp->len = len;
	jp->off = 0;

	if (len == 0)
		return MICROJS_EMPTY_FILE;

	return MICROJS_OK;
}

int microjs_scan(struct microjs_parser * jp)
{
	unsigned int sp;
	unsigned int cnt;
	unsigned int tok;
	unsigned int i;
	const char * txt;
	unsigned int len;
	int err;
	int c;
	
	/* initialize variables */
	cnt = jp->cnt;
	sp = jp->sp;
	txt = jp->txt;
	len = jp->len;

	DCC_LOG(LOG_TRACE, "parse start");
	DCC_LOG1(LOG_TRACE, "script length = %d bytes.", len);

	for (i = jp->off; i < len; ) {

		c = txt[i];
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
				c = txt[i];
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

			if ((jp->sp - cnt) < 3) {
				DCC_LOG(LOG_WARNING, "token buffer overflow!");
				err = MICROJS_TOKEN_BUF_OVF;
				goto error;
			}

			jp->tok[cnt++] = TOK_STRING + j;
			jp->tok[cnt++] = offs;
			jp->tok[cnt++] = (offs >> 8);
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

			if ((jp->sp - cnt) < MICROJS_SYMBOL_LEN_MAX) {
				DCC_LOG(LOG_WARNING, "token buffer overflow!");
				err = MICROJS_TOKEN_BUF_OVF;
				goto error;
			}

			j = 0;
			s = (char *)&jp->tok[cnt + 1];
			do {
				s[j++] = c;
				if (++i == len)	
					break;
				c = txt[i];
			} while (isalnum(c) || (c == '_'));
			s[j++] = '\0';

			/* look up in the kwywords table */
			for (k = 0; k <= (TOK_WHILE - TOK_BREAK); ++k) {
				if (strcmp(microjs_keyword[k], s) == 0) {
					DCC_LOG(LOG_INFO, "Keyword");
					jp->tok[cnt++] = k + TOK_BREAK;
					break;
				}
			}

			if (k > (TOK_WHILE - TOK_BREAK)) {
				/* symbol */
				DCC_LOG(LOG_INFO, "Symbol");
				jp->tok[cnt++] = TOK_SYMBOL + j - 1;
				cnt += j;
			}

			continue;
		}

		/* Number */
		if (isdigit(c)) {
			uint32_t val = 0;

			DCC_LOG(LOG_INFO, "Number");
			if ((c == '0') && ((i + 1) < len) && 
				((txt[i + 1] == 'x') || (txt[i + 1] == 'X'))) {

				/* Hexadecimal */

				/* Skip 0x prefix. */
				i += 2;
				if (i == len) {
					DCC_LOG(LOG_WARNING, "invalid constant");
					err = MICROJS_INVALID_LITERAL;
					goto error;
				}

				/* Next digit should be kex digit */
				if (!isxdigit(c = txt[i])) {
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
					c = txt[i];
				} while (isxdigit(c));

				DCC_LOG(LOG_INFO, "Hexadecimal");
			} else {
				/* Decimal */
				do {
					val = ((val << 2) + val) << 1;
					val += c - '0';
					if (++i == len)	
						break;
					c = txt[i];
				} while (isdigit(c));
			}
		
			if ((jp->sp - cnt) < 5) {
				DCC_LOG(LOG_WARNING, "token buffer overflow!");
				err = MICROJS_TOKEN_BUF_OVF;
				goto error;
			}

			if ((val & 0xffffff00) == 0) {
				jp->tok[cnt++] = TOK_INT8;
				jp->tok[cnt++] = val;
				continue;
			} 
			
			if ((val & 0xffff0000) == 0) {
				jp->tok[cnt++] = TOK_INT16;
				jp->tok[cnt++] = val;
				jp->tok[cnt++] = val >> 8;
				continue;
			}

			if ((val & 0xffffff00) == 0) {
				jp->tok[cnt++] = TOK_INT24;
				jp->tok[cnt++] = val;
				jp->tok[cnt++] = val >> 8;
				jp->tok[cnt++] = val >> 16;
				continue;
			}

			jp->tok[cnt++] = TOK_INT32;
			jp->tok[cnt++] = val;
			jp->tok[cnt++] = val >> 8;
			jp->tok[cnt++] = val >> 16;
			jp->tok[cnt++] = val >> 24;
			continue;
		}
	
		/* Comments */
		if ((c == '/') && (++i < len)) {

			c = txt[i];
			if  (c == '/') {
				DCC_LOG(LOG_INFO, "Single line comment");
				/* Single line comment */
				while ((c != '\n') && (++i < len)) {
					c = txt[i];
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
					c = txt[i];

					if (c == '*') {
						if (++i == len)	{
							DCC_LOG(LOG_WARNING, "unclosed comment ");
							err = MICROJS_UNCLOSED_COMMENT;
							goto error;
						}
						c = txt[i];
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
				c = txt[i];
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
				c = txt[i];
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
			if ((++i < len) && ((c = txt[i]) == '=')) {
				tok = TOK_EQ;
				goto inc_push;
			}
			tok = TOK_LET;
			goto push;
		}

		if (c == '!') {
			if ((++i < len) && ((c = txt[i]) == '=')) {
				tok = TOK_NEQ;
				goto inc_push;
			}
			tok = TOK_NOT;
			goto push;
		}

		if (c == '|') {
			if (++i < len)	{
				if  (c == '|') {
					tok = TOK_OR;
					goto inc_push;
				}
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
			/* push closing bracket into stack */
			jp->tok[--sp] = TOK_RIGHTBRACKET;
			break;
		case ']':
			tok = TOK_RIGHTBRACKET;
			goto bkt_pop;
		case '(':
			tok = TOK_LEFTPAREN;
			/* push closing paren into stack */
			jp->tok[--sp] = TOK_RIGHTPAREN;
			break;
		case ')':
			tok = TOK_RIGHTPAREN;
			goto bkt_pop;
		case '{':
			tok = TOK_LEFTBRACE;
			/* push closing brace into stack */
			jp->tok[--sp] = TOK_RIGHTBRACE;
			break;
		case '}':
			tok = TOK_RIGHTBRACE;
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
		case '+':
			tok = TOK_PLUS;
			break;
		case '-':
			tok = TOK_MINUS;
			break;
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
		if ((sp - cnt) < 1) {
			DCC_LOG(LOG_WARNING, "token buffer overflow!");
			err = MICROJS_TOKEN_BUF_OVF;
			goto error;
		}
		jp->tok[cnt++] = tok;

		DCC_LOG1(LOG_INFO, "%s", microjs_tok_str[tok]);
	}

	/* the matching bracket stack must be empty at this point */
	if (sp != jp->top) {
		DCC_LOG(LOG_WARNING, "bracket closing mismatch!");
		err = MICROJS_BRACKET_MISMATCH;
		goto error;
	}

	DCC_LOG1(LOG_INFO, "%s", microjs_tok_str[tok]);

	/* push a token into the buffer */
	if ((sp - cnt) < 1) {
		DCC_LOG(LOG_WARNING, "token buffer overflow!");
		err = MICROJS_TOKEN_BUF_OVF;
		goto error;
	}
	jp->tok[cnt++] = TOK_EOF;

	DCC_LOG1(LOG_TRACE, "token stream length = %d bytes.", cnt);
	DCC_LOG(LOG_INFO, "parse done.");

	jp->cnt = cnt;
	jp->sp = sp;
	jp->off = i;
	return MICROJS_OK;

bkt_pop:
	/* get a brakcet from the stack and check for matching pair */
	if (sp == jp->top) {
		DCC_LOG(LOG_WARNING, "bracket stack empty!");
		err = MICROJS_EMPTY_STACK;
		goto error;
	}
	if (jp->tok[sp++] != tok) {
		DCC_LOG(LOG_WARNING, "bracket closing mismatch!");
		err = MICROJS_BRACKET_MISMATCH;
		goto error;
	}
	goto inc_push;

error:
	jp->cnt = cnt;
	jp->sp = sp;
	jp->off = i;

	return err;
}

int microjs_token_get(struct microjs_parser * jp, 
					  struct microjs_val * val)
{
	unsigned int offs;
	int idx = 0;
	uint32_t x;
	int tok;
	int len;
	(void)len;

//	idx = jp->idx;
	tok = jp->tok[idx++];
	if (tok >= TOK_STRING) {
		len = tok - TOK_STRING;
		offs = jp->tok[idx++];
		offs |= jp->tok[idx++] << 8;
		val->str.dat = (char *)jp->txt + offs;
		val->str.len = len;
		tok = TOK_STRING;
	} else if (tok >= TOK_SYMBOL) {
		len = tok - TOK_SYMBOL + 1;
		val->str.dat = (char *)&jp->tok[idx];
		val->str.len = len;
		idx += len;
		tok = TOK_SYMBOL;
	} else if (tok >= TOK_INT8) {
		x = jp->tok[idx++];
		if (tok >= TOK_INT16) {
			x |= jp->tok[idx++] << 8;
			if (tok == TOK_INT24) {
				x |= jp->tok[idx++] << 16;
				if (tok >= TOK_INT32)
					x |= jp->tok[idx++] << 24;
			}
		} 
		val->u32 = x;
		tok = TOK_INT32;
	} 

//	jp->idx = idx;

	return tok;
}

static bool labeled_stat(struct microjs_parser * p, unsigned int tok)
{
	DCC_LOG(LOG_TRACE, "...");
	return false;
}

/*
const_exp		: conditional_exp
*/

/*
logical_and_exp		: inclusive_or_exp
			| logical_and_exp '&&' inclusive_or_exp
*/
/*
inclusive_or_exp	: exclusive_or_exp
			| inclusive_or_exp '|' exclusive_or_exp
*/
/*
exclusive_or_exp	: and_exp
			| exclusive_or_exp '^' and_exp
*/
/*
and_exp			: equality_exp
			| and_exp '&' equality_exp
*/
/*
equality_exp		: relational_exp
			| equality_exp '==' relational_exp
			| equality_exp '!=' relational_exp
*/
/*
relational_exp		: shift_expression
			| relational_exp '<' shift_expression
			| relational_exp '>' shift_expression
			| relational_exp '<=' shift_expression
			| relational_exp '>=' shift_expression
*/
/*
shift_expression	: additive_exp
			| shift_expression '<<' additive_exp
			| shift_expression '>>' additive_exp
*/
/*
additive_exp		: mult_exp
			| additive_exp '+' mult_exp
			| additive_exp '-' mult_exp
*/
/*
mult_exp		: unary_exp
			| mult_exp '*' unary_exp
			| mult_exp '/' unary_exp
			| mult_exp '%' unary_exp
*/
/*

unary_exp		: postfix_exp
			| unary_operator unary_exp
*/

/*
unary_operator		: '&' | '+' | '-' | '~' | '!'
*/

/*
postfix_exp		: primary_exp
			| postfix_exp '[' exp ']'
			| postfix_exp '(' argument_exp_list ')'
			| postfix_exp '('			')'
			| postfix_exp '.' id
*/

static bool conditional_exp(struct microjs_parser * p, unsigned int tok)
{
/*
conditional_exp		: logical_or_exp
			| logical_or_exp '?' exp ':' conditional_exp
*/
	DCC_LOG(LOG_TRACE, "...");
	return false;
}

static bool assignment_exp(struct microjs_parser * p, unsigned int tok)
{
/*
assignment_exp : unary_exp '=' conditional_exp
*/
	DCC_LOG(LOG_TRACE, "...");
	if (tok == TOK_SEMICOLON)
		return true;

	if (conditional_exp(p, tok) && (tok = p->tok[p->cnt++]) == TOK_SEMICOLON) {
		return true;
	}

	return false;
}

static bool __exp(struct microjs_parser * p, unsigned int tok)
{
/*
exp			: assignment_exp
			| exp ',' assignment_exp
			;
*/
	DCC_LOG(LOG_TRACE, "...");

	if (assignment_exp(p, tok)) {
		return true;
	}

	if (__exp(p, tok) && (tok = p->tok[p->cnt++]) == TOK_COLON &&
		assignment_exp(p, p->tok[p->cnt++])) {
		return true;
	}

	return false;
}

static bool exp_stat(struct microjs_parser * p, unsigned int tok)
{
/*
exp_stat		: exp ';'
			|	';'
			;
*/
	DCC_LOG(LOG_TRACE, "...");
	if (tok == TOK_SEMICOLON)
		return true;

	if (__exp(p, tok) && (tok = p->tok[p->cnt++]) == TOK_SEMICOLON) {
		return true;
	}
	return false;
}

static bool compound_stat(struct microjs_parser * p, unsigned int tok)
{
	DCC_LOG(LOG_TRACE, "...");
	return false;
}
static bool selection_stat(struct microjs_parser * p, unsigned int tok)
{
	DCC_LOG(LOG_TRACE, "...");
	return false;
}

static bool iteration_stat(struct microjs_parser * p, unsigned int tok)
{
	DCC_LOG(LOG_TRACE, "...");
	return false;
}

static bool jump_stat(struct microjs_parser * p, unsigned int tok)
{
	DCC_LOG(LOG_TRACE, "...");
	return false;
}

int microjs_compile(struct microjs_parser * p, 
					uint8_t code[], unsigned int size)
{
	unsigned int tok;

	p->idx = 0;

	while ((tok = p->tok[p->idx++]) != TOK_EOF) {
/*
stat	: labeled_stat
			| exp_stat
			| compound_stat
			| selection_stat
			| iteration_stat
			| jump_stat
   */
		if (!(labeled_stat(p, tok) |
			exp_stat(p, tok) |
			compound_stat(p, tok) |
			selection_stat(p, tok) |
			iteration_stat(p, tok) |
			jump_stat(p, tok))) {
			return -1;
		}
	}

	return 0;
}

