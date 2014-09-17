/* 
 * Copyright(C) 2014 Robinson Mittmann. All Rights Reserved.
 * 
 * This file is part of the MicroJs
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
 * @file microjs_vm.c
 * @brief MicroJS Virtual Machine
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */



#define __MICROJS_I__
#include "microjs-i.h"

/* --------------------------------------------------------------------------
   Virtual machine
   -------------------------------------------------------------------------- */

void microjs_vm_init(struct microjs_vm * vm, int32_t data[], unsigned int len)
{
	vm->data = data;
	vm->sp = len;
	/* FIXME: adjust bas pointer according to heap allocation */
	vm->bp = 0;
	vm->sl = 0;
	vm->env.ftrace = NULL;
	vm->env.fin = stdin;
	vm->env.fout = stdout;

	FTRACEF(stdout, "SP=0x%04x\n", vm->sp);
	FTRACEF(stdout, "BP=0x%04x\n", vm->bp);
	FTRACEF(stdout, "SL=0x%04x\n", vm->sl);
}

int microjs_exec(struct microjs_vm * vm, uint8_t code[], unsigned int len)
{
	FILE * f = vm->env.ftrace;
	bool trace = (f == NULL) ? false : true;
	int32_t r0;
	int32_t r1;
	int32_t r2;
	uint8_t * pc = code;
	int32_t * data = vm->data;
	int32_t * sp = data + (vm->sp / sizeof(int32_t));
	int32_t * bp = data + (vm->bp / sizeof(int32_t));
	uint8_t * xp = code;
	int opc;

	if (trace)
		FTRACEF(f, "SP=0x%04x\n", (int)((int)(sp - data) * sizeof(int32_t)));

	for (;;) {
		/* fetch */
		if (trace)
			FTRACEF(f, "%04x\t", (int)(pc - code));

		opc = *pc++;
		switch (opc) {

		case OPC_JMP:
			r0 = (int16_t)(pc[0] | pc[1] << 8);
			pc += 2;
			pc += r0;
			if (trace)
				FTRACEF(f, "JMP 0x%04x (offs=%d)\n", (int)(pc - code), r0);
			break;

		case OPC_JEQ: /* coniditional JMP */
			r0 = (int16_t)(pc[0] | pc[1] << 8);
			pc += 2;
			r1 = *sp++;
			if (trace)
				FTRACEF(f, "JEQ 0x%04x (%d)\n", (int)(pc - code) + r0, r1);
			if (r1 == 0)
				pc += r0;
			break;

		case OPC_XPT: 
			if (trace)
				FTRACEF(f, "XPT\n");
			pc = xp;
			break;

		case OPC_POP:
			if (trace)
				FTRACEF(f, "POP\n");
			sp++;
			break;

		case OPC_I32:
			r0 = (int32_t)(pc[0] | pc[1] << 8 | pc[2] << 16 | pc[3] << 24);
			pc += 4;
			*(--sp) = r0;
			if (trace)
				FTRACEF(f, "I32 %d\n", r0);
			break;

		case OPC_I16:
			r0 = (int16_t)(pc[0] | pc[1] << 8);
			pc += 2;
			*(--sp) = r0;
			if (trace)
				FTRACEF(f, "I16 0x%04x\n", r0);
			break;

		case OPC_I8:
			r0 = *pc++;
			*(--sp) = r0;
			if (trace)
				FTRACEF(f, "I8 %d\n", r0);
			break;

		case OPC_LD:
			r1 = *pc++;
			r0 = data[r1];
			*(--sp) = r0;
			if (trace)
				FTRACEF(f, "LD 0x%04x -> %d\n", r1, r0);
			break;

		case OPC_ST:
			r1 = *pc++;
			r0 = *sp++;
			data[r1] = r0;
			if (trace)
				FTRACEF(f, "ST 0x%04x <- %d\n", r1, r0);
			break;

		case OPC_LDR:
			r1 = *pc++;
			r0 = bp[r1];
			*(--sp) = r0;
			if (trace)
				FTRACEF(f, "LDR 0x%04x -> %d\n", r1, r0);
			break;

		case OPC_STR:
			r1 = *pc++;
			r0 = *sp++;
			bp[r1] = r0;
			if (trace)
				FTRACEF(f, "STR 0x%04x <- %d\n", r1, r0);
			break;

		case OPC_SBP:
			r0 = (uint16_t)(pc[0] | pc[1] << 8);
			pc += 2;
			bp = data + r0;
			if (trace)
				FTRACEF(f, "SBP 0x%04x\n", r0);
			break;

		case OPC_SXP:
			r0 = (uint16_t)(pc[0] | pc[1] << 8);
			pc += 2;
			xp = code + r0;
			if (trace)
				FTRACEF(f, "SXP 0x%04x\n", r0);
			break;

		case OPC_EXT:
			r0 = *pc++;
			r1 = *pc++;
			if (trace)
				FTRACEF(f, "EXT %d, %d\n", r0, r1);
			r0 = extern_call[r0](&vm->env, sp, r1);
			/* remove the stack frame */
			sp += r1;
			*(--sp) = r0;
			break;

		case OPC_LT:
			r0 = *sp++;
			r1 = *sp++;
			r2 = r1 < r0;
			*(--sp) = r2;
			if (trace)
				FTRACEF(f, "LT %d %d -> %d\n", r0, r1, r2);
			break;

		case OPC_GT:
			r0 = *sp++;
			r1 = *sp++;
			r2 = r1 > r0;
			*(--sp) = r2;
			if (trace)
				FTRACEF(f, "GT %d %d\n", r0, r1);
			break;

		case OPC_EQ:
			r0 = *sp++;
			r1 = *sp++;
			r2 = r1 == r0;
			*(--sp) = r2;
			if (trace)
				FTRACEF(f, "EQ %d %d -> %d\n", r0, r1, r2);
			break;

		case OPC_NE:
			r0 = *sp++;
			r1 = *sp++;
			r2 = r1 != r0;
			*(--sp) = r2;
			if (trace)
				FTRACEF(f, "GT %d %d\n", r0, r1);
			break;

		case OPC_LE:
			r0 = *sp++;
			r1 = *sp++;
			r2 = r1 <= r0;
			*(--sp) = r2;
			if (trace)
				FTRACEF(f, "LE %d %d\n", r0, r1);
			break;

		case OPC_GE:
			r0 = *sp++;
			r1 = *sp++;
			r2 = r1 >= r0;
			*(--sp) = r2;
			if (trace)
				FTRACEF(f, "GE %d %d\n", r0, r1);
			break;

		case OPC_ASR:
			r0 = *sp++;
			r1 = *sp++;
			r2 = r1 >> r0;
			*(--sp) = r2;
			if (trace)
				FTRACEF(f, "ASR %d %d\n", r0, r1);
			break;

		case OPC_SHL:
			r0 = *sp++;
			r1 = *sp++;
			r2 = r1 << r0;
			*(--sp) = r2;
			if (trace)
				FTRACEF(f, "SHL %d %d\n", r0, r1);
			break;

		case OPC_ADD:
			r0 = *sp++;
			r1 = *sp++;
			r2 = r1 + r0;
			*(--sp) = r2;
			if (trace)
				FTRACEF(f, "ADD %d %d\n", r0, r1);
			break;

		case OPC_SUB:
			r0 = *sp++;
			r1 = *sp++;
			r2 = r1 - r0;
			*(--sp) = r2;
			if (trace)
				FTRACEF(f, "SUB %d %d\n", r0, r1);
			break;

		case OPC_MUL:
			r0 = *sp++;
			r1 = *sp++;
			r2 = r1 * r0;
			*(--sp) = r2;
			if (trace)
				FTRACEF(f, "MUL %d %d\n", r0, r1);
			break;

		case OPC_DIV:
			r0 = *sp++;
			r1 = *sp++;
			r2 = r1 / r0;
			*(--sp) = r2;
			if (trace)
				FTRACEF(f, "DIV %d %d\n", r0, r1);
			break;

		case OPC_MOD:
			r0 = *sp++;
			r1 = *sp++;
			r2 = r1 % r0;
			*(--sp) = r2;
			if (trace)
				FTRACEF(f, "MOD %d %d -> %d\n", r1, r0, r2);
			break;

		case OPC_OR:
			r0 = *sp++;
			r1 = *sp++;
			r2 = r1 | r0;
			*(--sp) = r2;
			if (trace)
				FTRACEF(f, "OR %d %d -> %d\n", r1, r0, r2);
			break;

		case OPC_AND:
			r0 = *sp++;
			r1 = *sp++;
			r2 = r1 & r0;
			*(--sp) = r2;
			if (trace)
				FTRACEF(f, "AND %d %d -> %d\n", r1, r0, r2);
			break;

		case OPC_XOR:
			r0 = *sp++;
			r1 = *sp++;
			r2 = r1 ^ r0;
			*(--sp) = r2;
			if (trace)
				FTRACEF(f, "XOR %d %d -> %d\n", r1, r0, r2);
			break;

		case OPC_INV:
			r0 = *sp++;
			*(--sp) = ~r0;
			if (trace)
				FTRACEF(f, "INV %d\n", r0);
			break;

		case OPC_NEG:
			r0 = *sp++;
			*(--sp) = -r0;
			if (trace)
				FTRACEF(f, "NEG %d\n", r0);
			break;

		case OPC_ABT:
			if (trace)
				FTRACEF(f, "ABT\n");
			goto done;

		default:
			FTRACEF(f, "Invalid OPC: %d\n", opc);
			return -1;
		}
	} 

done:
	if (trace)
		FTRACEF(f, "SP=0x%04x\n", (int)((sp - data) * sizeof(int32_t)));

	return 0;
}

