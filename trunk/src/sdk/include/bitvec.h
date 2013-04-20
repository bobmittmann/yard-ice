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
 * @file bitvec.h
 * @brief YARD-ICE libbitvec
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 


#ifndef __BITVEC_H__
#define __BITVEC_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Compare two bit vectors for equality */
int vec_cmp(const void * vec_a, const void * vec_b, int len);

/* Rotate right */
int vec_rshift(uint32_t * dst, const uint32_t * src, int len, int bits);

/* Rotate left */
int vec_lshift(uint32_t * dst, const uint32_t * src, int len, int bits);

/* Convert the bit vector to string */
char * vec_fmt(char * buf, const void * vec, int len);

/* Create a random bit vector */
void vec_rand(void * vec, int len);

/* Create an all ones bit vector */
int vec_ones(void * vec, int len);

/* Create an all zeros bit vector */
int vec_zeros(void * vec, int len);

/* Concatenate two bit vectors */
int vec_cat(void * dst_vec, int dst_len, const void * src_vec, int src_len);

#ifdef __cplusplus
}
#endif

#endif /* __BITVEC_H__ */

