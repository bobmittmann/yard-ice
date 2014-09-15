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
 * @file microjs-i.h
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#ifndef __MICROJS_H__
#define __MICROJS_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

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
	ERR_SYNTAX_ERROR,
	ERR_STRBUF_OVERFLOW,
	ERR_STRING_NOT_FOUND,
	ERR_HEAP_OVERFLOW,
	ERR_STACK_OVERFLOW,
	ERR_VAR_UNKNOWN,
	ERR_INTERNAL_ERROR,
	ERR_EXTERN_UNKNOWN,
	ERR_ARG_MISSING,
	ERR_TOO_MANY_ARGS,
	ERR_TMP_PUSH_FAIL,
	ERR_TMP_POP_FAIL,
	ERR_TMP_GET_FAIL,
	ERR_REF_PUSH_FAIL,
	ERR_REF_POP_FAIL,
	ERR_REF_GET_FAIL,
	ERR_REF_SET_FAIL,
};

/* --------------------------------------------------------------------------
   Compiler 
   -------------------------------------------------------------------------- */

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

struct symtab;

struct microjs_compiler {
	struct symtab * tab;
	struct token tok;
	int32_t * mem;
	uint8_t * code;
	uint16_t pc;
	uint16_t heap;
	uint16_t stack;
	uint16_t sp;
};

/* --------------------------------------------------------------------------
   Runtime Environement
   -------------------------------------------------------------------------- */

struct microjs_env {
	FILE * fout;
	FILE * fin;
	FILE * ftrace;
};

/* --------------------------------------------------------------------------
   Virtual Machine
   -------------------------------------------------------------------------- */

struct microjs_vm {
	struct microjs_env env;
	uint16_t sp;
	uint16_t sl;
	int32_t * data;
};

#ifdef __cplusplus
extern "C" {
#endif

struct symtab * symtab_init(uint32_t * buf, unsigned int len);

int microjs_compiler_init(struct microjs_compiler * microjs, 
						  struct symtab * tab, int32_t stack[], 
						  unsigned int size);

int microjs_compile(struct microjs_compiler * p, uint8_t code[], 
					const char * txt, unsigned int len);


void microjs_vm_init(struct microjs_vm * vm, int32_t data[], unsigned int len);

int microjs_exec(struct microjs_vm * vm, uint8_t code[], unsigned int len);

void strbuf_init(uint16_t * buf, unsigned int len);

#ifdef __cplusplus
}
#endif

#endif /* __MICROJS_H__ */

