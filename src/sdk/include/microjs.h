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

enum {
	MICROJS_ERR_NONE = 0,
	MICROJS_UNEXPECTED_CHAR,
	MICROJS_TOKEN_BUF_OVF,
	MICROJS_UNCLOSED_STRING,
	MICROJS_UNCLOSED_COMMENT,
	MICROJS_INVALID_LITERAL,
	MICROJS_STRINGS_UNSUPORTED,
};

struct microjs_parser {
	uint16_t cnt;  /* token count */
	uint16_t size; /* token buffer size */
	uint16_t err_offs; /* parser error offset */
	uint16_t err_code; /* parser error code */
	uint8_t * tok; /* token buffer */
	const char * js;   /* base pointer (original js file) */
};

struct microjs_str_pool {
	uint16_t * offs; /* point to the offset table */
	char * base;     /* base pointer */
	char * top;      /* top pointer */
};

extern const struct microjs_str_pool microjs_str_const;
extern const struct microjs_str_pool microjs_str_var;

#ifdef __cplusplus
extern "C" {
#endif

int microjs_init(struct microjs_parser * p, uint8_t * tok, unsigned int size);

int microjs_parse(struct microjs_parser * p, 
				  const char * js, unsigned int len);

int microjs_str_lookup(const struct microjs_str_pool * pool, 
					   const char * s, int len);

int const_str_lookup(const char * s, int len);

char * const_str(int idx);

int microjs_dump(struct microjs_parser * p);

int microjs_str_pool_dump(const struct microjs_str_pool * pool);

#ifdef __cplusplus
}
#endif

#endif /* __MICROJS_H__ */

