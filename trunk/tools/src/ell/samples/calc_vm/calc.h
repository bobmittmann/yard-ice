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
	struct token tok;
	int32_t * mem;
	uint8_t * code;
	uint16_t pc;
	uint16_t heap;
	uint16_t stack;
	uint16_t sp;
	uint16_t nest;
	struct sym_tab * tab;
};

/* --------------------------------------------------------------------------
   Virtual machine
   -------------------------------------------------------------------------- */


#define OPC_ASR      0
#define OPC_SHL      1
#define OPC_ADD      2
#define OPC_SUB      3
#define OPC_MUL      4
#define OPC_DIV      5
#define OPC_MOD      6
#define OPC_OR       7
#define OPC_AND      8
#define OPC_XOR      9
#define OPC_INV      10
#define OPC_NEG      11
#define OPC_I8       12
#define OPC_I16      13
#define OPC_I32      14
#define OPC_LD       15
#define OPC_ST       16
#define OPC_CMP      18
#define OPC_JMP      19
#define OPC_JEQ      20
#define OPC_JNE      21
#define OPC_LT       22
#define OPC_GT       23
#define OPC_EQ       24
#define OPC_NE       25
#define OPC_LE       26
#define OPC_GE       27
#define OPC_LOR      28
#define OPC_LAND     29
#define OPC_PRINT    30

struct calc_vm {
	uint16_t sp;
	uint16_t sl;
	bool trace;
	int32_t * data;
};

#ifdef __cplusplus
extern "C" {
#endif

int lexer_open(struct lexer * lex, const char * txt, unsigned int len);

struct token lexer_scan(struct lexer * lex);

void lexer_print_err(FILE * f, struct lexer * lex, int err);

int calc_init(struct calc * calc, struct sym_tab * tab, 
			  int32_t stack[], unsigned int size);

int calc_compile(struct calc * p, uint8_t code[], 
				 const char * txt, unsigned int len);

void calc_vm_init(struct calc_vm * vm, int32_t data[], unsigned int len);

int calc_exec(struct calc_vm * vm, uint8_t code[], unsigned int len);

#ifdef __cplusplus
}
#endif

#endif /* __CALC_H__ */

