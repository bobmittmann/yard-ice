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

#include <unistd.h>
#include <inttypes.h>
#include <stdbool.h>
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#if defined(WIN32)
  #include <io.h>
  #include <fcntl.h>
#else
  #include <assert.h>
#endif

#define VERSION_MAJOR 2
#define VERSION_MINOR 0

#if 0
const char _lut[128] = {
/*	NUL SOH STX ETX EOT ENQ ACK BEL */
	 _C, _C, _C, _C, _C, _C, _C, _C,
/*	BS  TAB LF  VT  FF  CR  SO  SI  */
	_C, _C + _S, _C + _S, _C + _S, _C + _S, _C + _S, _C, _C,
/*	DLE DC1 DC2 DC3 DC4 NAK SYN ETB */
	_C, _C, _C, _C, _C, _C, _C, _C,
/*	CAN EM  SUB ESC FS  GS  RS  US  */
	_C, _C, _C, _C, _C, _C, _C, _C,
/*	' ' !   "   #   $   %   &   '   */
	_S + _B, _P, _P, _P, _P, _P, _P, _P,
/*	(   )   *   +   ,   -   .   /   */
	_P, _P, _P, _P, _P, _P, _P, _P,
/*	0   1   2   3   4   5   6   7   */
	_N, _N, _N, _N, _N, _N, _N, _N,
/*	8   9   :   ;   <   =   >   ?   */
	_N, _N, _P, _P, _P, _P, _P, _P,
/*	@   A   B   C   D   E   F   G   */
	_P, _U + _X, _U + _X, _U + _X, _U + _X, _U + _X, _U + _X, _U,
/*	H   I   J   K   L   M   N   O   */
	_U, _U, _U, _U, _U, _U, _U, _U,
/*	P   Q   R   S   T   U   V   W   */
	_U, _U, _U, _U, _U, _U, _U, _U,
/*	X   Y   Z   [   \   ]   ^   _   */
	_U, _U, _U, _P, _P, _P, _P, _P,
/*	`   a   b   c   d   e   f   g   */
	_P, _L + _X, _L + _X, _L + _X, _L + _X, _L + _X, _L + _X, _L,
/*	h   i   j   k   l   m   n   o   */
	_L, _L, _L, _L, _L, _L, _L, _L,
/*	p   q   r   s   t   u   v   w   */
	_L, _L, _L, _L, _L, _L, _L, _L,
/*	x   y   z   {   |   }   ~   DEL */
	_L, _L, _L, _P, _P, _P, _P, _C
};

enum {
	TOK_SYMBOL,
	TOK_PLUS,
	TOK_MINUS,
	TOK_MUL,
	TOK_DIV,
	TOK_MOD,
	TOK_SEMICOLON,
	TOK_NUMBER
	TOK_COMMA,
	TOK_OR,
	TOK_AMD,
	TOK_ASTR,
	TOK_SLASH,
	TOK_HASH,
};


#endif

int basic_tokenizer(char * script, unsigned int len)
{
	return 0;
}

const char * const keywords[] = {
	"for",
	"function"
	"if",
	"var",
	"while",
};

#define TOK_STRING     0x80
#define TOK_SYMBOL     0x60
#define TOK_NULL          0
#define TOK_ASR           1 
#define TOK_ASL           2
#define TOK_LTE           3
#define TOK_LT            4
#define TOK_GTE           5
#define TOK_GT            6
#define TOK_EQ            7
#define TOK_NEQ           8
#define TOK_PLUS          9
#define TOK_MINUS        10
#define TOK_INC          11 
#define TOK_DEC          12
#define TOK_MUL          13
#define TOK_DIV          14
#define TOK_MOD          15
#define TOK_DOT          16
#define TOK_COMMA        17
#define TOK_SEMICOLON    18
#define TOK_COLON        19
#define TOK_LEFTBRACKET  20
#define TOK_RIGHTBRACKET 21
#define TOK_LEFTPAREN    22
#define TOK_RIGHTPAREN   23
#define TOK_LEFTBRACE    24
#define TOK_RIGHTBRACE   25
#define TOK_OR           26
#define TOK_BITOR        27
#define TOK_AND          28
#define TOK_BITAND       29
#define TOK_XOR          30
#define TOK_NOT          31
#define TOK_BITNOT       32
#define TOK_LET          33

#define TOK_BREAK        34
#define TOK_CASE         35
#define TOK_CONTINUE     36
#define TOK_FALSE        37
#define TOK_FOR          38
#define TOK_FUNCTION     39
#define TOK_RETURN       40
#define TOK_SWITCH       41
#define TOK_TRUE         42
#define TOK_VAR          43
#define TOK_WHILE        44

#define TOK_INTEGER      45

const char keyword[][10] = {
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
	"++", /* TOK_INC */
	"--", /* TOK_DEC */
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
};


#define TOK_BUF_LEN 8192
uint8_t tok_buf[TOK_BUF_LEN];
uint16_t tok_idx;
uint16_t tok_sp;

int microjs_dump(void)
{
	int idx = 0;
	int tok;
	int sp;
	int len;
	char * s;

	(void)len;

	for (sp = TOK_BUF_LEN - 1; sp > tok_sp; --sp) {
		tok = tok_buf[sp];
		if (tok & TOK_STRING) {
			len = tok & ~TOK_STRING;
			s = (char *)&tok_buf[idx];
			printf("\"%s\" ", s);
//			idx += len;
			idx += strlen(s) + 1;
		} else if ((tok & TOK_SYMBOL) == TOK_SYMBOL) {
			len = tok & ~TOK_SYMBOL;
			s = (char *)&tok_buf[idx];
			printf("%s ", s);
			idx += strlen(s) + 1;
		} else if (tok == TOK_INTEGER) {
			unsigned int val;
			
			val = tok_buf[idx++];
			val |= tok_buf[idx++] << 8;
			val |= tok_buf[idx++] << 16;
			val |= tok_buf[idx++] << 24;

			printf("%d ", val);
		} else if (tok >= TOK_BREAK) {
			s = (char *)keyword[tok - TOK_BREAK];
			printf("%s ", s);
		} else {
			printf("%s ", tok_str[tok]);
		}
	}

	return 0;
}

int microjs(char * script, unsigned int len)
{
	char * cp;
	int tok;
	int qt;
	int n;
	int c;

	
	/* initialize token list */
	tok_idx = 0;
	tok_sp = TOK_BUF_LEN;

	for (cp = script; (c = *cp) != '\0'; ) {

		/* Remove lead blanks */
		for (;;) {
			c = *cp;
			if (!isspace(c))
				break;
			cp++;
		}

		/* Quotes: copy verbatim */
		if ((c == '\'') || (c == '\"')) {
			char * s = ++cp;
			qt = c;
			for (; ((c = *cp) != qt); cp++) {
				if (c == '\0') {
					/* parse error, unclosed quotes */
					return -1;
				}
				tok_buf[tok_idx++] = c;
			}
			cp++;
			tok_buf[tok_idx++] = '\0';
			tok_buf[--tok_sp] = TOK_STRING + (cp - s);
			continue;
		}

		/* Symbol */
		if (isalpha(c) || (c == '_')) {
			int idx = tok_idx;
			char * s ;
			int i;

			tok_buf[idx++] = c;
			cp++;
			c = *cp;
			while (isalnum(c) || (c == '_')) {
				tok_buf[idx++] = (uint8_t)c;
				cp++;
				c = *cp;
			}
			tok_buf[idx++] = '\0';
			
			s = (char *)&tok_buf[tok_idx];

			for (i = 0; i <= (TOK_WHILE - TOK_BREAK); ++i) {
				if (strcmp(keyword[i], s) == 0) {
					tok_buf[--tok_sp] = i + TOK_BREAK;
					break;
				}
			}

			if (i > (TOK_WHILE - TOK_BREAK)) {
				tok_buf[--tok_sp] = TOK_SYMBOL + idx - tok_idx;
				tok_idx = idx;
			}
			continue;
		}

		/* Number */
		if (isdigit(c)) {
			unsigned long val = 0;

			if ((c == 0) && ((cp[1] == 'x') || (cp[1] == 'X'))) {
				cp++;
				/* Hexadecimal */
				while ((c = *cp) != '\0') {

					if (c >= 'a')
						c &= ~('a' - 'A');

					if ((c >= 'A') && (c <= 'F'))
						c -= ('A' - 10);
					else {
						if ((c >= '0') && (c <= '9'))
							c -= '0';
						else
							break;
					}

					val = val << 4;
					val += c;
					cp++;
				}

			} else {
				/* Decimal */
				while ((c = *cp) != '\0') {
					if ((c < '0') || (c > '9'))
						break;
					val = ((val << 2) + val) << 1;
					val += c - '0';
					cp++;
				}
			}
			
			tok_buf[tok_idx++] = val;
			tok_buf[tok_idx++] = val >> 8;
			tok_buf[tok_idx++] = val >> 16;
			tok_buf[tok_idx++] = val >> 24;

			tok_buf[--tok_sp] = TOK_INTEGER;
			continue;
		}
	
		/* Punctuation */
		if (c == '<') {
			cp++;
			c = *cp;
			if  (c == '<') {
				tok_buf[--tok_sp] = TOK_ASL;
				cp++;
			} else if  (c == '=') {
				tok_buf[--tok_sp] = TOK_LTE;
				cp++;
			} else
				tok_buf[--tok_sp] = TOK_LT;
			continue;
		}

		if (c == '>') {
			cp++;
			c = *cp;
			if  (c == '>') {
				tok_buf[--tok_sp] = TOK_ASR;
				cp++;
			} else if  (c == '=') {
				tok_buf[--tok_sp] = TOK_GTE;
				cp++;
			} else
				tok_buf[--tok_sp] = TOK_GT;
			continue;
		}

		if (c == '=') {
			cp++;
			c = *cp;
			if  (c == '=') {
				tok_buf[--tok_sp] = TOK_EQ;
				cp++;
			} else
				tok_buf[--tok_sp] = TOK_LET;
			continue;
		}

		if (c == '!') {
			cp++;
			c = *cp;
			if  (c != '=') {
				tok_buf[--tok_sp] = TOK_NEQ;
				cp++;
			} else
				tok_buf[--tok_sp] = TOK_NOT;
			continue;
		}

		if (c == '+') {
			cp++;
			c = *cp;
			if  (c == '+') {
				tok_buf[--tok_sp] = TOK_INC;
				cp++;
			} else
				tok_buf[--tok_sp] = TOK_PLUS;
			continue;
		}

		if (c == '-') {
			cp++;
			c = *cp;
			if  (c == '-') {
				tok_buf[--tok_sp] = TOK_DEC;
				cp++;
			} else
				tok_buf[--tok_sp] = TOK_MINUS;
			continue;
		}

		if (c == '|') {
			cp++;
			c = *cp;
			if  (c == '|') {
				tok_buf[--tok_sp] = TOK_OR;
				cp++;
			} else
				tok_buf[--tok_sp] = TOK_BITOR;
			continue;
		}

		if (c == '&') {
			cp++;
			c = *cp;
			if  (c == '&') {
				tok_buf[--tok_sp] = TOK_AND;
				cp++;
			} else
				tok_buf[--tok_sp] = TOK_BITAND;
			continue;
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
		default:
			return -1;
		}
			
		tok_buf[--tok_sp] = tok;
		cp++;
	}

	microjs_dump();

	return n;
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

