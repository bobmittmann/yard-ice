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

/* --------------------------------------------------------------------------
   String pool
   -------------------------------------------------------------------------- */

struct str_pool {
	char * buf;
	uint16_t len;
	uint8_t cnt;
	uint8_t max;
	uint16_t offs[];
};

#define SYMBOLS_MAX 64

#define STRINGS_MAX 63
#define STRINGS_POOL_LEN (512 - (2 * (STRINGS_MAX + 1)))


struct sym {
	uint8_t flags;
	uint8_t nm;
	uint16_t addr;
	uint16_t size;
};

struct sym_tab {
	struct {
		struct str_pool str;
		uint16_t offs[STRINGS_MAX];
		char str_buf[STRINGS_POOL_LEN];
	};
	uint16_t global;
	uint16_t local;
	struct sym sym[SYMBOLS_MAX];
};

struct microjs_compiler {
	struct token tok;
	int32_t * mem;
	uint8_t * code;
	uint16_t pc;
	uint16_t heap;
	uint16_t stack;
	uint16_t sp;
	struct sym_tab * tab;
};

struct microjs_vm {
	FILE * ftrace;
	uint16_t sp;
	uint16_t sl;
	int32_t * data;
};

#ifdef __cplusplus
extern "C" {
#endif

int microjs_compiler_init(struct microjs_compiler * microjs, 
						  struct sym_tab * tab, int32_t stack[], 
						  unsigned int size);

int microjs_compile(struct microjs_compiler * p, uint8_t code[], 
				 const char * txt, unsigned int len);

void microjs_vm_init(struct microjs_vm * vm, int32_t data[], unsigned int len);

int microjs_exec(struct microjs_vm * vm, uint8_t code[], unsigned int len);

void sym_tab_init(struct sym_tab * tab);

#ifdef __cplusplus
}
#endif

#endif /* __MICROJS_H__ */

