/* 
 * File:	 led.h
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(c) 2003-2006 BORESTE (www.boreste.com). All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef __JSON_H__
#define __JSON_H__

#include <stdint.h>
#include "jsmn.h"

#define JSON_STR_LEN_MAX 128

struct json_string {
	uint16_t pos;
	uint16_t len;
};

enum {
	JSON_ERR_GENERAL = 1,
	JSON_ERR_INVALID_TOKEN,
	JSON_ERR_INVALID_TYPE,
	JSON_ERR_NOT_PRIMITIVE,
	JSON_ERR_NOT_STRING,
	JSON_ERR_NOT_OBJECT,
	JSON_ERR_NOT_ARRAY,
	JSON_ERR_EMPTY_OBJECT,
	JSON_ERR_EMPTY_ARRAY,
	JSON_ERR_NUM_CHILDREN,
	JSON_ERR_NOMEM,
	JSON_ERR_INVALID_CHAR,
	JSON_ERR_INCOMPLETE,
	JSON_ERR_PKT_EMPTY,
	JSON_ERR_KEY_TYPE_INVALID,
	JSON_ERR_KEY_NAME_INVALID,
	JSON_ERR_NULL_POINTER,
	JSON_ERR_INVALID_BOOLEAN,
	JSON_ERR_MISSING_VALUE,
	JSON_ERR_EXTRA_VALUE,
	JSON_ERR_INVALID_OBJECT,
	JSON_ERR_STACK_OVERFLOW,
	JSON_ERR_HEAP_OVERFLOW,
	JSON_ERR_FLASH_WRITE
};


struct obj_desc {
	char key[12];
	int (* parser)(char * js, jsmntok_t * t, void * ptr);
	unsigned int offs;	
};

/*
[JSON_ERR_KEY_TYPE_INVALID] = "object keys must be strings.",
*/

extern jsmntok_t * err_tok;

#ifdef __cplusplus
extern "C" {
#endif

int json_token_tostr(char * s, unsigned int max, char * js, jsmntok_t *t);

int json_root_len(char * js);

int json_walk_node(FILE * f, char * js, jsmntok_t *t, int lvl);

int json_walk_object(FILE * f, char * js, jsmntok_t *t, int lvl);

int json_walk_array(FILE * f, char * js, jsmntok_t *t, int lvl);


void json_dump_err(char * js, int pos);

int json_parse_dump(char * js, jsmntok_t * t, void * ptr);

int json_parse_uint16(char * js, jsmntok_t * t, void * ptr);

int json_parse_boolean(char * js, jsmntok_t * t, void * ptr);

int json_parse_string(char * js, jsmntok_t * t, void * ptr);

int json_parse_object(char * js, jsmntok_t * t, 
					  const struct obj_desc * desc, void * obj);

#ifdef __cplusplus
}
#endif

#endif /* __JSON_H__ */


