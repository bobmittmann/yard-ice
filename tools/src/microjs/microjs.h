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
	OK                      = 0,
	ERR_UNEXPECED_EOF       = 1,
	ERR_UNEXPECTED_CHAR     = 2,
	ERR_UNEXPECTED_SYMBOL   = 3,
	ERR_UNCLOSED_STRING     = 4,
	ERR_UNCLOSED_COMMENT    = 5,
	ERR_INVALID_LITERAL     = 6,
	ERR_INVALID_ID          = 7,
	ERR_STRINGS_UNSUPORTED  = 8,
	ERR_STRING_TOO_LONG     = 9,
	ERR_STRING_NOT_FOUND    = 10,
	ERR_STRBUF_OVERFLOW     = 11,
	ERR_SYNTAX_ERROR        = 12,
	ERR_DATA_OVERFLOW       = 13,
	ERR_VAR_UNKNOWN         = 14,
	ERR_EXTERN_UNKNOWN      = 15,
	ERR_ARG_MISSING         = 16,
	ERR_TOO_MANY_ARGS       = 17,
	ERR_SYM_PUSH_FAIL       = 18,
	ERR_SYM_POP_FAIL        = 19,
	ERR_OBJ_NEW_FAIL        = 20,
	ERR_SDT_STACK_OVERFLOW  = 21,
	ERR_GENERAL             = 22,
	ERR_CODE_MEM_OVERFLOW   = 23,
	ERR_RET_COUNT_MISMATCH  = 24,
	ERR_INVALID_INSTRUCTION = 25,
	ERR_STACK_OVERFLOW      = 26,
	ERR_STACK_UNDERFLOW     = 27,
	ERR_EXTERN_NOT_FUNCTION = 28,
	ERR_EXTERN_NOT_ARRAY    = 29,
	ERR_EXTERN_NOT_OBJECT   = 30,
	ERR_EXTERN_NOT_INTEGER  = 31,
	ERR_EXTERN_NOT_MEMBER   = 32,
	ERR_EXTERN_READONLY     = 33,
	ERR_NOT_IMPLEMENTED     = 24,
	ERR_ATTR_NOT_ARRAY      = 25,
	ERR_NULL_POINTER        = 26,
	ERR_EXTERN_NOT_SCALAR   = 27,
};

struct symstat {
	uint16_t sp;
	uint16_t bp;
};

struct symtab;
struct microjs_sdt;

/* --------------------------------------------------------------------------
   External objects/symbols/functions
   -------------------------------------------------------------------------- */

struct classdef {
	const char * nm;
	uint8_t first;
	uint8_t last;
};

/* external definition  */

#define O_ARRAY     (1 << 0)
#define O_MEMBER    (1 << 1)
#define O_SIZEOFFS  (1 << 2)
#define O_READONLY  (1 << 3)
#define O_CONST     (1 << 3)
#define O_SINGLETON (1 << 3)

#define O_FUNCTION (0 << 6)
#define O_INTEGER  (1 << 6)
#define O_OBJECT   (2 << 6)

#define EXTDEF_TYPE(_XP) ((_XP)->opt & (3 << 6))
#define EXTDEF_FLAG(_XP, _O) ((_XP)->opt & (_O))

struct extdef {
	const char * nm;
	uint8_t opt;
	union {
		struct {
			uint8_t argmin; /* minimum number of arguments */
			uint8_t argmax; /* maximum number of arguments */
			int8_t ret; /* number of returned values */ 
		} f; /* function */

		struct {
			uint8_t cdef; /* class definition */
			int16_t inst; /* object instance */
		} o; /* object */

		struct {
			uint8_t cdef; /* class definition */
		} ao; /* array of objects, require a lookup function to translate
				 array index to object instance. */

		struct {
			uint8_t cdef; /* class definition */
			uint8_t size;
			int16_t offs;
		} aos; /* array of objects, uses an offset and an object size
				  to translate array index to object instance: 
				  INSTANCE = offs + (size * INDEX) */

		struct {
			int32_t val;
		} ic; /* integer constant */

		struct {
		} i; /* integer variable */

		struct {
		} ai; /* array of integers */

		struct {
			uint8_t size;
			int16_t offs;
		} ais; /* array of integers */
	};
} __attribute__((packed));

/* --------------------------------------------------------------------------
   External library definition 
   -------------------------------------------------------------------------- */

struct ext_classtab {
	uint8_t ccnt;
	struct classdef cdef[];
};

struct ext_libdef {
	const char * name;
	const struct ext_classtab * classtab;
	uint8_t xcnt;
	struct extdef xdef[];
};

extern FILE * microjs_vm_tracef;

#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------------------------------------------------
   Strings 
   -------------------------------------------------------------------------- */

const char * str(unsigned int idx);

int str_add(const char * s, unsigned int len);

int str_lookup(const char * s, unsigned int len);

int cstr_decode(char * dst, const char * src, unsigned int len);

int cstr_add(const char * s, unsigned int len);


const char * const_str(unsigned int idx);

int const_str_add(const char * s, unsigned int len);

int const_str_lookup(const char * s, unsigned int len);

int const_strbuf_dump(FILE * f);

int const_strbuf_purge(void);


struct symtab * symtab_init(uint32_t sym_buf[], 
							unsigned int buf_len);

struct symtab * symtab_open(uint32_t * buf, unsigned int len);

struct symstat symtab_state_save(struct symtab * tab);

int symtab_data_size(struct symtab * tab);

int symtab_dump(FILE * f, struct symtab * tab);

struct microjs_rt * symtab_rt_get(struct symtab * tab);

void symtab_state_rollback(struct symtab * tab, struct symstat st);

struct microjs_sdt * microjs_sdt_init(uint32_t * sdt_buf, 
									  unsigned int sdt_size,
									  struct symtab * tab,
									  const struct ext_libdef * libdef);

int microjs_sdt_begin(struct microjs_sdt * microjs, 
					  uint8_t code[], unsigned int code_size);

int microjs_compile(struct microjs_sdt * microjs, 
					const char * txt, unsigned int len);

int microjs_sdt_end(struct microjs_sdt * microjs);

void microjs_sdt_reset(struct microjs_sdt * microjs);

void microjs_sdt_error(FILE * f, struct microjs_sdt * microjs, int err);

const char * microjs_strerr(int err);

#ifdef __cplusplus
}
#endif

#endif /* __MICROJS_H__ */

