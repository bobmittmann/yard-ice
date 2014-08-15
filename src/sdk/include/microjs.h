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

enum {
	MICROJS_ERR_NONE = 0,
	MICROJS_UNEXPECTED_CHAR,
	MICROJS_TOKEN_BUF_OVF,
	MICROJS_UNCLOSED_STRING,
	MICROJS_UNCLOSED_COMMENT,
	MICROJS_INVALID_LITERAL,
	MICROJS_STRINGS_UNSUPORTED,
	MICROJS_MAX_NEST_LEVEL,
	MICROJS_BRACKET_MISMATCH,
	MICROJS_STRING_TOO_LONG
};

/**********************************************************************
  Tokenizer
 **********************************************************************/

struct microjs_tokenizer {
	uint16_t cnt;  /* token count */
	uint16_t size; /* token buffer size */
	uint16_t offs; /* parser offset */
	uint16_t err; /* parser error code */
	uint8_t * tok; /* token buffer */
	const char * js;   /* base pointer (original js file) */
};

struct microjs_val {
	union {
		struct {
			uint16_t len;
			char * dat;
		} str;
		uint32_t u32;	
		int32_t i32;	
		bool logic;
	};
};

/**********************************************************************
  JSON
 **********************************************************************/

enum {
	MICROJS_JSON_INVALID = 0,
	MICROJS_JSON_OBJECT,
	MICROJS_JSON_ARRAY,
	MICROJS_JSON_LABEL,
	MICROJS_JSON_NUMBER,
	MICROJS_JSON_STRING,
	MICROJS_JSON_BOOLEAN
};

struct microjs_json_parser {
	struct microjs_tokenizer * tkn;
	uint16_t idx;  /* token index */
};

struct microjs_attr_desc {
	char key[13];
	uint8_t type;	
	uint8_t opt;	
	uint16_t offs;	
	int (* parse)(struct microjs_json_parser * jsn, 
				   struct microjs_val * val, 
				   unsigned int opt, void * ptr);
};

/**********************************************************************
  Strings
 **********************************************************************/
struct microjs_str_pool {
	uint16_t * offs; /* point to the offset table */
	char * base;     /* base pointer */
	char * top;      /* top pointer */
	int (* write)(struct microjs_str_pool *, const char *, unsigned int);
};

extern const struct microjs_str_pool microjs_str_const;
extern const struct microjs_str_pool microjs_str_var;

#ifdef __cplusplus
extern "C" {
#endif

int microjs_tok_init(struct microjs_tokenizer * tkn, 
					 uint8_t * tok, unsigned int size);

int microjs_tokenize(struct microjs_tokenizer * tkn, 
				  const char * js, unsigned int len);

int microjs_tok_dump(FILE * f, struct microjs_tokenizer * tkn);



int microjs_json_init(struct microjs_json_parser * jsn, 
					  struct microjs_tokenizer * tkn);

bool microjs_json_expect(struct microjs_json_parser * jsn, unsigned int type);

int microjs_json_parse_val(struct microjs_json_parser * jsn,
						   struct microjs_val * val);

int microjs_json_parse_obj(struct microjs_json_parser * jsn,
						   const struct microjs_attr_desc desc[],
						   void * ptr);

int microjs_str_lookup(const struct microjs_str_pool * pool, 
					   const char * s, int len);

int const_str_lookup(const char * s, int len);

int const_str_write(const char * s, unsigned int len);


char * const_str(int idx);

int microjs_str_pool_dump(const struct microjs_str_pool * pool);


/* Encode a 16bit integral value */
int microjs_u16_enc(struct microjs_json_parser * jsn, 
					struct microjs_val * val, 
					unsigned int opt, void * ptr);

/* Encode a boolean as a single bit */
int microjs_bit_enc(struct microjs_json_parser * jsn, 
					struct microjs_val * val, 
					unsigned int bit, void * ptr);

/* Encode a string as a index to the constant string pool */
int microjs_const_str_enc(struct microjs_json_parser * jsn, 
					struct microjs_val * val, 
					unsigned int bit, void * ptr);

#ifdef __cplusplus
}
#endif

#endif /* __MICROJS_H__ */

