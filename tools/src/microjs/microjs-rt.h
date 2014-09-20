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
 * @file microjs-rt.h
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


/*****************************************************************************
 * MicroJS Runtime header file
 *****************************************************************************/

#ifndef __MICROJS_RT_H__
#define __MICROJS_RT_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define EXT_RAND    0
#define EXT_SQRT    1
#define EXT_LOG2    2
#define EXT_WRITE   3
#define EXT_TIME    4
#define EXT_SRAND   5
#define EXT_PRINT   6
#define EXT_PRINTF  7
#define EXT_MEMRD	8

/* --------------------------------------------------------------------------
   Virtual machine
   -------------------------------------------------------------------------- */

#define OPC_ABT      0
#define OPC_RET      1
#define OPC_INV      2
#define OPC_NEG      3
#define OPC_ASR      4
#define OPC_SHL      5
#define OPC_ADD      6
#define OPC_SUB      7
#define OPC_MUL      8
#define OPC_DIV      9
#define OPC_MOD      10
#define OPC_OR       11
#define OPC_AND      12
#define OPC_XOR      13
#define OPC_LT       14
#define OPC_GT       15
#define OPC_EQ       16
#define OPC_NE       17
#define OPC_LE       18
#define OPC_GE       19
#define OPC_I8       20
#define OPC_I16      21
#define OPC_I32      22
#define OPC_LD       23
#define OPC_ST       24
#define OPC_JMP      25
#define OPC_JEQ      26
#define OPC_EXT      27
#define OPC_POP      28
#define OPC_ISP      29 /* Increment SP */
#define OPC_XPT      30 /* exception */
#define OPC_PUHSX    31 /* Push exception frame */

#define OPC_IBP      32 /* Increment SP */
#define OPC_LDR      33 /* BP relative load */
#define OPC_STR      34 /* BP relative store */

/* --------------------------------------------------------------------------
   Runtime Environement
   -------------------------------------------------------------------------- */

struct microjs_env {
	FILE * fout;
	FILE * fin;
	FILE * ftrace;
	int32_t * data;
};

/* --------------------------------------------------------------------------
   Virtual Machine
   -------------------------------------------------------------------------- */

struct microjs_vm {
	struct microjs_env env;
	uint16_t xp; /* exception handler pointer */
	uint16_t bp; /* base pointer */
	uint16_t sp; /* stack pointer */
	uint16_t sl; /* stack limit */
	int32_t * data;
};

extern int32_t (* const microjs_extern[])(struct microjs_env *, int32_t [], int);

#ifdef __cplusplus
extern "C" {
#endif

void microjs_vm_init(struct microjs_vm * vm, int32_t data[], unsigned int len);

int microjs_exec(struct microjs_vm * vm, uint8_t code[], unsigned int len);

void strbuf_init(uint16_t * buf, unsigned int len);

#ifdef __cplusplus
}
#endif

#endif /* __MICROJS_RT_H__ */

