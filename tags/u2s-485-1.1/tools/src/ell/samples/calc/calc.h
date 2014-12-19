/* 
 * Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
 * 
 * This file is part of the YARD-ICE.
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
 * @file calc.h
 * @brief 
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#ifndef __CALC_H__
#define __CALC_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include "calc_ll.h"

struct token {
	uint8_t typ; /* token type (class) */
	uint8_t qlf; /* qualifier */
	uint16_t off; /* offset */
	union {
		char * s;
		uint32_t u32;	
		int32_t i32;	
	};
};

enum {
	OK = 0,
	ERR_UNEXPECTED_CHAR,
	ERR_UNCLOSED_STRING,
	ERR_UNCLOSED_COMMENT,
	ERR_INVALID_LITERAL,
	ERR_INVALID_ID,
	ERR_STRINGS_UNSUPORTED,
	ERR_STRING_TOO_LONG,
	ERR_BRACKET_MISMATCH,
	ERR_SYNTAX_ERROR
};

/**********************************************************************
  Lexical Analyzer
 **********************************************************************/

struct lexer {
	uint16_t off;  /* lexer text offset */
	uint16_t len;  /* lexer text length */
	const char * txt;   /* base pointer (original js txt file) */
};

/**********************************************************************
 Calculator
 **********************************************************************/

struct sym_tab;

struct calc {
	struct lexer lex;
	struct token tok;
	int32_t * mem;
	uint16_t heap;
	uint16_t stack;
	uint16_t sp;
	struct sym_tab * tab;
};


#ifdef __cplusplus
extern "C" {
#endif

int lexer_open(struct lexer * lex, const char * txt, unsigned int len);

struct token lexer_scan(struct lexer * lex);

void lexer_print_err(FILE * f, struct lexer * lex, int err);

int calc_init(struct calc * calc, struct sym_tab * tab, 
			  int32_t stack[], unsigned int size);

int calc_parse(struct calc * p, const char * txt, unsigned int len);

#ifdef __cplusplus
}
#endif

#endif /* __CALC_H__ */

