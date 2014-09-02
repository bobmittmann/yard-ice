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

/**********************************************************************
  Tokenizer
 **********************************************************************/

struct microjs_parser {
	uint16_t cnt;  /* token count */
	uint16_t idx;  /* token pointer */

	uint16_t sp; /* stack pointer */
	uint16_t top; /* parser error code */
	uint8_t * tok; /* token buffer */

	uint16_t off;  /* lexer text offset */
	uint16_t len;  /* lexer text length */
	const char * txt;   /* base pointer (original js txt file) */
};

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

/**********************************************************************
  JSON
 **********************************************************************/

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

int microjs_init(struct microjs_parser * jp, 
				 uint8_t tok[], unsigned int size);

int microjs_open(struct microjs_parser * jp, 
				 const char * txt, unsigned int len);

int microjs_scan(struct microjs_parser * jp);

int microjs_compile(struct microjs_parser * p, 
					uint8_t code[], unsigned int size);

int microjs_tok_dump(FILE * f, struct microjs_parser * jp);


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

#ifdef __cplusplus
}
#endif

#endif /* __MICROJS_H__ */

