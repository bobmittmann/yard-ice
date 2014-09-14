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
	MICROJS_OK                 = 0,
	MICROJS_UNEXPECTED_CHAR    = -1,
	MICROJS_TOKEN_BUF_OVF      = -3,
	MICROJS_UNCLOSED_STRING    = -4,
	MICROJS_UNCLOSED_COMMENT   = -5,
	MICROJS_INVALID_LITERAL    = -6,
	MICROJS_BRACKET_MISMATCH   = -7,
	MICROJS_STRING_TOO_LONG    = -8,
	MICROJS_EMPTY_FILE         = -9,
	MICROJS_EMPTY_STACK        = -10,
	MICROJS_STRINGS_UNSUPORTED = -11,
	MICROJS_INVALID_SYMBOL     = -12,
	MICROJS_INVALID_LABEL      = -13,
	MICROJS_OBJECT_EXPECTED    = -14,
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
	ERR_SYNTAX_ERROR,
	ERR_STRBUF_OVERFLOW,
	ERR_STRING_NOT_FOUND,
	ERR_HEAP_OVERFLOW,
	ERR_STACK_OVERFLOW,
	ERR_REF_PUSH_FAIL,
	ERR_TMP_PUSH_FAIL
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

/**********************************************************************
  JSON
 **********************************************************************/

struct microjs_val {
	union {
		struct {
			char * dat;
			uint16_t len;
		} str;
		struct {
			const char * sz;
			uint8_t id;
		} lbl;
		uint32_t u32;	
		int32_t i32;	
		bool logic;
	};
};

struct json_file { 
	const char * txt;
	uint16_t len;
	uint16_t crc;
};

enum {
	MICROJS_JSON_EOF        = 0,
	MICROJS_JSON_STOP       = 1,
	MICROJS_JSON_NULL       = 2,
	MICROJS_JSON_OBJECT     = 3,
	MICROJS_JSON_ARRAY      = 4,
	MICROJS_JSON_END_OBJECT = 5,
	MICROJS_JSON_END_ARRAY  = 6,
	MICROJS_JSON_BOOLEAN    = 7,
	MICROJS_JSON_INTEGER    = 8,
	MICROJS_JSON_LABEL      = 9,
	MICROJS_JSON_STRING     = 10,
	MICROJS_JSON_INVALID    = 11
};

struct microjs_json_parser {
	uint16_t idx;  /* token parser index */
	uint16_t cnt;  /* token count */

	uint16_t sp;   /* token buffer stack pointer */
	uint16_t top;  /* token buffer top pointer (size of the token buufer) */
	uint8_t * tok; /* token buffer */

	uint16_t off;  /* lexer text offset */
	uint16_t len;  /* lexer text length */
	const char * txt;   /* base pointer (original json txt file) */

	const char * const * lbl;   /* label table */
};

typedef int (* microjs_attr_parser_t)(struct microjs_json_parser * jsn, 
									  struct microjs_val * val, 
									  unsigned int opt, void * ptr);

struct microjs_attr_desc {
	char key[13];
	uint8_t type;	
	uint8_t opt;	
	uint16_t offs;	
	microjs_attr_parser_t parse;
};

/**********************************************************************
  Strings
 **********************************************************************/
struct microjs_str_pool {
	uint16_t * offs; /* point to the offset table */
	char * base;     /* base pointer */
	char * top;      /* top pointer */
	int (* write)(const struct microjs_str_pool *, 
				  const char *, unsigned int);
};

extern const struct microjs_str_pool microjs_str_const;
extern const struct microjs_str_pool microjs_str_var;

#ifdef __cplusplus
extern "C" {
#endif


/**********************************************************************
  JSON
 **********************************************************************/

int microjs_json_root_len(const char * js);

int microjs_json_init(struct microjs_json_parser * jsn, 
					 uint8_t * tok, unsigned int size,
					 const char * const label[]);

int microjs_json_open(struct microjs_json_parser * jsn, 
					  const char * txt, unsigned int len);

int microjs_json_scan(struct microjs_json_parser * jsn);

/* flushes the token buffer, but keep track of the file scanning */
void microjs_json_flush(struct microjs_json_parser * jsn);

int microjs_json_dump(FILE * f, struct microjs_json_parser * jsn);

int microjs_json_get_val(struct microjs_json_parser * jsn,
						   struct microjs_val * val);

int microjs_json_parse_obj(struct microjs_json_parser * jsn,
						   const struct microjs_attr_desc desc[],
						   void * ptr);

/* Encode a 16 bits integral value */
int microjs_u16_enc(struct microjs_json_parser * jsn, 
					struct microjs_val * val, 
					unsigned int opt, void * ptr);

/* Encode an 8 bits integral value */
int microjs_u8_enc(struct microjs_json_parser * jsn, 
				   struct microjs_val * val, 
				   unsigned int opt, void * ptr);

/* Encode an array of 8 bits integral values.
  The option parameter indicates the maximum length of the array */
int microjs_array_u8_enc(struct microjs_json_parser * jsn, 
				   struct microjs_val * val, 
				   unsigned int len, void * ptr);

/* Encode a boolean as a single bit */
int microjs_bit_enc(struct microjs_json_parser * jsn, 
					struct microjs_val * val, 
					unsigned int bit, void * ptr);

/* Encode a string as a index to the constant string pool */
int microjs_const_str_enc(struct microjs_json_parser * jsn, 
					struct microjs_val * val, 
					unsigned int bit, void * ptr);
/**********************************************************************
  Strings
 **********************************************************************/

char * const_str(int idx);

int microjs_str_pool_dump(const struct microjs_str_pool * pool);

int microjs_str_lookup(const struct microjs_str_pool * pool, 
					   const char * s, int len);

int const_str_lookup(const char * s, int len);

int const_str_write(const char * s, unsigned int len);



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

