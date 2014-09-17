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
#include <microjs-rt.h>

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
	ERR_SYM_PUSH_FAIL,
	ERR_SYM_POP_FAIL,
	ERR_OBJ_NEW_FAIL,
	ERR_ALOC32_FAIL,
};

struct symtab;
struct microjs_sdt;

/* --------------------------------------------------------------------------
   External objects/symbols/functions
   -------------------------------------------------------------------------- */

struct ext_fndef {
	const char * nm;
	uint8_t argmin;
	uint8_t argmax;
};

/* --------------------------------------------------------------------------
   External library definition 
   -------------------------------------------------------------------------- */

struct ext_libdef {
	const char * name;
	uint8_t fncnt;
	struct ext_fndef fndef[];
};


#ifdef __cplusplus
extern "C" {
#endif

struct symtab * symtab_init(uint32_t sym_buf[], 
							unsigned int buf_len, 
							const struct ext_libdef * libdef);

struct microjs_sdt * microjs_sdt_init(uint32_t sdt_buf[], 
									  unsigned int buf_size,
									  struct symtab * tab, 
									  uint8_t code[],
									  unsigned int code_size, 
									  unsigned int data_size);

int microjs_compile(struct microjs_sdt * microjs, 
					const char * txt, unsigned int len);

#ifdef __cplusplus
}
#endif

#endif /* __MICROJS_H__ */

