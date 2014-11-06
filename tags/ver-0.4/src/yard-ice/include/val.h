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
 * @file val.h
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */



#ifndef __YARD_VAL_H__
#define __YARD_VAL_H__

#include <stdint.h>

enum {
	TYPE_VOID,
	TYPE_INT32,
	TYPE_UINT32,
	TYPE_BOOL,
	TYPE_STRING,
	TYPE_TAG,
	TYPE_RANGE32,
	TYPE_INT64,
	TYPE_UINT64,
	TYPE_DBLOCK32,
	TYPE_FREQ
};


/**
 * struct range32 - define an address range
 * @from: start address
 * @to: end address
 */
struct range32 {
	uint32_t from;
	uint32_t to;
};

/**
 * struct dblock32 - define a data block with upto 32bits length
 * @ptr: pointer to the data block
 * @len: the size of the data block in bytes
 */
struct dblock32 {
	uint8_t * ptr;
	uint32_t len;
};

/**
 * struct value - a generic container for yard variable values
 */
struct value {
	union {
		int logic;
		int32_t int32;
		uint32_t uint32;
		struct range32 range32;
		int64_t int64;
		uint64_t uint64;
		char * string;
		char tag[8];
		struct dblock32 dblock32;
	};
};

typedef struct value value_t;

typedef int (* val_encode_t)(value_t * val, const char * s);
typedef int (* val_decode_t)(const value_t * val, char * s);

#ifndef YARD_TYPE_NAME_MAX
#define YARD_TYPE_NAME_MAX 8
#endif

/**
 * struct type_def - definition of a value type
 */
struct type_def {
	char name[YARD_TYPE_NAME_MAX];
	val_encode_t encode;
	val_decode_t decode;
};

typedef struct type_def type_def_t;

/*
 * standard type definitions 
 */
extern const struct type_def type_def_void;
extern const struct type_def type_def_int32;
extern const struct type_def type_def_uint32;
extern const struct type_def type_def_bool;
extern const struct type_def type_def_string;
extern const struct type_def type_def_range32;
extern const struct type_def type_def_freq;
extern const struct type_def type_def_tag;

/*
 * standard type definitions 
 */
extern const struct type_def * const type_def_tab[];

extern inline type_def_t * def_of(int type) {
	return (type_def_t *)type_def_tab[type];
}

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  val_encode - Convert from string to value_t
 *  @param val The value to be encoded
 *  @param s An string with the value to be converted to
 *  @returns Return the size of the encoded data on success, 
 *  or -1 if an error occurred.
 */
int val_encode(const struct type_def * def, value_t * val, const char * s);

/**
 *  val_decode - Convert from val_t to string 
 *  @param val The value to be decoded
 *  @param s An string to hold the decoded value
 *  @returns Return the decoded string
 */
char * val_decode(const struct type_def * def, const value_t * val, char * s);


char * type_name(int type);

#ifdef __cplusplus
}
#endif

#endif /* __YARD_VAL_H__ */

