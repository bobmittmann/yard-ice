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

/* --------------------------------------------------------------------------
   Virtual machine
   -------------------------------------------------------------------------- */

/* All the rest  */
#define MISCOP       (0 << 4)
#define OPC_ABT      0
#define OPC_RET      1
#define OPC_XPT      2 /* exception */
#define OPC_POP      3
#define OPC_INV      4
#define OPC_NEG      5
#define OPC_I8       6
#define OPC_I16      7
#define OPC_I32      8
#define OPC_EXT      9
#define OPC_INC      10
#define OPC_DEC      11
#define OPC_NOT      12
#define OPC_NOP3     13
#define OPC_NOP4     14
#define OPC_NOP5     15

#define OPC_I4       (1 << 4)

/* Binary integer operations */
#define INTOP        (2 << 4)
#define OPC_ADD      (INTOP + 0)
#define OPC_SUB      (INTOP + 1)
#define OPC_MUL      (INTOP + 2)
#define OPC_DIV      (INTOP + 3)
#define OPC_MOD      (INTOP + 4)
#define OPC_ASR      (INTOP + 5)
#define OPC_SHL      (INTOP + 6)
#define OPC_OR       (INTOP + 7)
#define OPC_AND      (INTOP + 8)
#define OPC_XOR      (INTOP + 9)
#define OPC_LT       (INTOP + 10)
#define OPC_GT       (INTOP + 11)
#define OPC_EQ       (INTOP + 12)
#define OPC_NE       (INTOP + 13)
#define OPC_LE       (INTOP + 14)
#define OPC_GE       (INTOP + 15)

#define OPC_ISP      (3 << 4) /* Add SP */
#define OPC_LD       (4 << 4)
#define OPC_ST       (5 << 4)
#define OPC_PUSHX    (6 << 4) /* Push exception frame */
#define OPC_JMP      (7 << 4)
#define OPC_JEQ      (8 << 4)

#define OPC_RES0     (9 << 4)
#define OPC_RES1     (10 << 4)
#define OPC_RES2     (11 << 4)
#define OPC_RES3     (12 << 4)
#define OPC_RES4     (13 << 4)
#define OPC_RES5     (14 << 4)
#define OPC_RES6     (15 << 4)

#define OPC_IBP      (10 << 4) /* Increment PB */
#define OPC_LDR      (11 << 4) /* BP relative load */
#define OPC_STR      (12 << 4) /* BP relative store */

/* --------------------------------------------------------------------------
   Virtual Machine
   -------------------------------------------------------------------------- */

struct microjs_rt {
	uint16_t stack_sz; /* estimated maximum stack size */
	uint16_t data_sz; /* initial data size */
};

struct microjs_vm {
	volatile uint8_t abort;
	uint8_t trace_en: 1;
	uint8_t res: 7;
	uint16_t pc;
	uint16_t sp;
	uint16_t xp;
	int32_t * data;  /* host data area pointer */
	int32_t * stack; /* host stack area */
	void * env; /* environment */
};

extern int32_t (* const microjs_extern[])(void *, int32_t [], int);

#ifdef __cplusplus
extern "C" {
#endif

/* Initialize the microjs Virtual Machine  */

void microjs_vm_init(struct microjs_vm * vm, const struct microjs_rt * rt,
					 const void * env, int32_t data[], int32_t stack[]);

void microjs_vm_clr_data(struct microjs_vm * vm, 
						 const struct microjs_rt * rt);

int microjs_exec(struct microjs_vm * vm, uint8_t code[]);

void microjs_abort(struct microjs_vm * vm);

void strbuf_init(uint16_t * buf, unsigned int len);

#ifdef __cplusplus
}
#endif

#endif /* __MICROJS_RT_H__ */

