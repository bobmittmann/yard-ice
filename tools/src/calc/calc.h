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

enum {
	TOK_VOID = 0,
	TOK_EOF,
/* puctuation */
	TOK_DOT,
	TOK_COMMA,
	TOK_SEMICOLON,
	TOK_COLON,
	TOK_LEFTBRACKET,
	TOK_RIGHTBRACKET,
	TOK_LEFTPAREN,
	TOK_RIGHTPAREN,
	TOK_LEFTBRACE,
	TOK_RIGHTBRACE,
/* operators */
	TOK_ASR,
	TOK_SHL,
	TOK_LTE,
	TOK_LT,
	TOK_GTE,
	TOK_GT,
	TOK_EQ,
	TOK_NEQ,
	TOK_PLUS,
	TOK_MINUS,
	TOK_MUL,
	TOK_DIV,
	TOK_MOD,
	TOK_OR,
	TOK_BITOR,
	TOK_AND,
	TOK_BITAND,
	TOK_XOR,
	TOK_NOT,
	TOK_BITINV,
	TOK_ASSIGN,
/* keywords */
	TOK_BREAK,
	TOK_CASE,
	TOK_CONTINUE,
	TOK_CONST,
	TOK_ELSE,
	TOK_FALSE,
	TOK_FOR,
	TOK_FUNCTION,
	TOK_IF,
	TOK_NULL,
	TOK_RETURN,
	TOK_SWITCH,
	TOK_TRUE,
	TOK_VAR,
	TOK_WHILE,
/* integral values */
	TOK_INT,
	TOK_ID,
	TOK_STRING,
/* errors */
	TOK_ERR,
};

#define STRING_LEN_MAX 255

extern const char keyword[15][9];

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

struct tree;

struct lexer {
	uint16_t off;  /* lexer text offset */
	uint16_t len;  /* lexer text length */
	const char * txt;   /* base pointer (original js txt file) */
};


struct parser {
	struct lexer lex;
	struct token tok; /* lookahead token */
	uint16_t cnt;  /* token count */
	struct tree * t;
};

#ifdef __cplusplus
extern "C" {
#endif

int lexer_open(struct lexer * lex, const char * txt, unsigned int len);

struct token lexer_scan(struct lexer * lex);

int token_print(FILE * f, struct token tok);

int lexer_test(struct lexer * lex);

void lexer_print_err(FILE * f, struct lexer * lex, int err);

void dump_src(const char * txt, unsigned int len);

int calc_open(struct parser * p, const char * txt, unsigned int len);

int calc_parse(struct parser * p, uint8_t code[], unsigned int size);

char * tok2str(struct token tok);

#ifdef __cplusplus
}
#endif

#endif /* __CALC_H__ */

