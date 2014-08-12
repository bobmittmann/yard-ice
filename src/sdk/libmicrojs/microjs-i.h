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


/*****************************************************************************
 * CHIME internal (private) header file
 *****************************************************************************/

#ifndef __MICROJS_I_H__
#define __MICROJS_I_H__

#ifndef __MICROJS_I__
#error "Never use <microjs-i.h> directly; include <microjs.h> instead."
#endif

#ifdef CONFIG_H
#include "config.h"
#endif

#include <stdint.h>

#define TOK_STRING     0x80
#define TOK_SYMBOL     0x60

#define TOK_NULL          0
#define TOK_ASR           1 
#define TOK_SHL           2
#define TOK_LTE           3
#define TOK_LT            4
#define TOK_GTE           5
#define TOK_GT            6
#define TOK_EQ            7
#define TOK_NEQ           8
#define TOK_PLUS          9
#define TOK_MINUS        10
#define TOK_MUL          11
#define TOK_DIV          12
#define TOK_MOD          13
#define TOK_DOT          14
#define TOK_COMMA        15
#define TOK_SEMICOLON    16
#define TOK_COLON        17
#define TOK_LEFTBRACKET  18
#define TOK_RIGHTBRACKET 19
#define TOK_LEFTPAREN    20
#define TOK_RIGHTPAREN   21
#define TOK_LEFTBRACE    22
#define TOK_RIGHTBRACE   23
#define TOK_OR           24
#define TOK_BITOR        25
#define TOK_AND          26
#define TOK_BITAND       27
#define TOK_XOR          28
#define TOK_NOT          29
#define TOK_BITNOT       30
#define TOK_LET          31
#define TOK_INC          32 
#define TOK_DEC          33

#define TOK_BREAK        34
#define TOK_CASE         35
#define TOK_CONTINUE     36
#define TOK_FALSE        37
#define TOK_FOR          38
#define TOK_FUNCTION     39
#define TOK_RETURN       40
#define TOK_SWITCH       41
#define TOK_TRUE         42
#define TOK_VAR          43
#define TOK_WHILE        44

#define TOK_INT8         45
#define TOK_INT16        46
#define TOK_INT24        47
#define TOK_INT32        48

struct microjs_parser {
	uint16_t cnt;
	uint16_t size;
	uint8_t * tok;
};

extern const char microjs_keyword[11][9];

#define MICROJS_VM_STACK_SIZE 256 

#define LIT   (0 << 13) /* 000x xxxx xxxx xxxx */
#define LOD   (1 << 13) /* 001x xxxx xxxx xxxx */
#define STO   (2 << 13) /* 100x xxxx xxxx xxxx */ 
#define CAL   (3 << 13) /* 101x xxxx xxxx xxxx */     
#define INT   (4 << 13) /* 101x xxxx xxxx xxxx */ 
#define JMP   (5 << 13) /* 110x xxxx xxxx xxxx */ 
#define JPC   (6 << 13) /* 110x xxxx xxxx xxxx */ 
#define OPR   (7 << 13) /* 111x xxxx xxxx xxxx */ 

#define NEG   (OPR + 1)
#define ADD   (OPR + 2)
#define SUB   (OPR + 3)
#define MUL   (OPR + 4)
#define DIV   (OPR + 5)
#define BIT   (OPR + 6)
#define EQ    (OPR + 7)
#define NEQ   (OPR + 8)
#define LT    (OPR + 9)
#define GTE   (OPR + 10)
#define GT    (OPR + 11)
#define LTE   (OPR + 12)

struct microjs_vm {
	uint16_t ip;
	uint16_t sp;
	uint32_t r1;
	uint32_t r2;
	uint32_t r3;
	uint8_t stack[MICROJS_VM_STACK_SIZE];
};

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif

#endif /* __SHELL_I_H__ */

