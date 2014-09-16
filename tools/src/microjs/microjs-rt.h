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

/* --------------------------------------------------------------------------
   Virtual machine
   -------------------------------------------------------------------------- */

#define OPC_ABT      0
#define OPC_ASR      1
#define OPC_SHL      2
#define OPC_ADD      3
#define OPC_SUB      4
#define OPC_MUL      5
#define OPC_DIV      6
#define OPC_MOD      7
#define OPC_OR       8
#define OPC_AND      9
#define OPC_XOR      10
#define OPC_INV      11
#define OPC_NEG      12
#define OPC_LT       13
#define OPC_GT       14
#define OPC_EQ       15
#define OPC_NE       16
#define OPC_LE       17
#define OPC_GE       18
#define OPC_I8       19
#define OPC_I16      20
#define OPC_I32      21
#define OPC_LD       22
#define OPC_ST       23
#define OPC_JMP      24
#define OPC_JEQ      25
#define OPC_EXT      26
#define OPC_POP      27

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

