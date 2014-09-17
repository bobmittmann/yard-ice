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

static inline int32_t vm_rd32(struct microjs_vm * vm, unsigned int addr)
{
	return vm->data[addr >> 2];
}

static inline void vm_wr32(struct microjs_vm * vm, unsigned 
						   int addr, int32_t val)
{
	vm->data[addr >> 2] = val;
}

static inline void vm_push(struct microjs_vm * vm, int32_t x)
{
	vm->sp -= 4;
	vm->data[vm->sp >> 2] = x;	
}

static inline int32_t vm_pop(struct microjs_vm * vm)
{
	int32_t x;

	x = vm->data[vm->sp >> 2];
	vm->sp += 4;

	return x;
}

void microjs_vm_init(struct microjs_vm * vm, int32_t data[], unsigned int len)
{
	vm->data = data;
	vm->sp = len;
	vm->sl = 0;
	vm->env.ftrace = NULL;
	vm->env.fin = stdin;
	vm->env.fout = stdout;
}

int microjs_exec(struct microjs_vm * vm, uint8_t code[], unsigned int len)
{
	FILE * f = vm->env.ftrace;
	bool trace = (f == NULL) ? false : true;
	int32_t r0;
	int32_t r1;
	int32_t r2;
	int opc;
	int pc;

	if (trace)
		FTRACEF(f, "SP=0x%04x\n", vm->sp);

	pc = 0; 
	r0 = 0;
	r1 = 0;
	for (;;) {
		/* fetch */
		if (trace)
			FTRACEF(f, "%04x\t", pc);

		opc = code[pc++];
		switch (opc) {

		case OPC_JMP:
			r0 = (int16_t)(code[pc] | code[pc + 1] << 8);
			pc += 2;
			pc += r0;
			if (trace)
				FTRACEF(f, "JMP 0x%04x (offs=%d)\n", pc, r0);
			break;

		case OPC_JEQ: /* coniditional JMP */
			r0 = (int16_t)(code[pc] | code[pc + 1] << 8);
			pc += 2;
			r1 = vm_pop(vm);
			if (trace)
				FTRACEF(f, "JEQ 0x%04x (%d)\n", pc + r0, r1);
			if (r1 == 0)
				pc += r0;
			break;

		case OPC_POP:
			r0 = vm_pop(vm);
			break;

		case OPC_I32:
			r0 = code[pc++];
			r0 |= code[pc++] << 8;
			r0 |= code[pc++] << 16;
			r0 |= code[pc++] << 24;
			if (trace)
				FTRACEF(f, "I32 %d\n", r0);
			vm_push(vm, r0);
			break;

		case OPC_I16:
			r0 = code[pc++];
			r0 |= code[pc++] << 8;
			if (trace)
				FTRACEF(f, "I16 0x%04x\n", r0);
			vm_push(vm, r0);
			break;

		case OPC_I8:
			r0 = code[pc++];
			if (trace)
				FTRACEF(f, "I8 %d\n", r0);
			vm_push(vm, r0);
			break;

		case OPC_LD:
			r1 = code[pc++];
			r0 = vm->data[r1];
			if (trace)
				FTRACEF(f, "LD 0x%04x -> %d\n", r1, r0);
			vm_push(vm, r0);
			break;

		case OPC_ST:
			r1 = code[pc++];
			r0 = vm_pop(vm);
			if (trace)
				FTRACEF(f, "ST 0x%04x <- %d\n", r1, r0);
			vm->data[r1] = r0;
			break;

		case OPC_EXT:
			r0 = code[pc++];
			r1 = code[pc++];
			if (trace)
				FTRACEF(f, "EXT %d, %d\n", r0, r1);
			r0 = extern_call[r0](&vm->env, &vm->data[vm->sp >> 2], r1);
			/* remove the stack frame */
			vm->sp += 4 * r1;
			vm_push(vm, r0);
			break;

		case OPC_LT:
			r0 = vm_pop(vm);
			r1 = vm_pop(vm);
			r2 = r1 < r0;
			if (trace)
				FTRACEF(f, "LT %d %d\n", r0, r1);
			vm_push(vm, r2);
			break;

		case OPC_GT:
			r0 = vm_pop(vm);
			r1 = vm_pop(vm);
			r2 = r1 > r0;
			if (trace)
				FTRACEF(f, "GT %d %d\n", r0, r1);
			vm_push(vm, r2);
			break;

		case OPC_EQ:
			r0 = vm_pop(vm);
			r1 = vm_pop(vm);
			r2 = r1 == r0;
			if (trace)
				FTRACEF(f, "EQ %d %d -> %d\n", r0, r1, r2);
			vm_push(vm, r2);
			break;

		case OPC_NE:
			r0 = vm_pop(vm);
			r1 = vm_pop(vm);
			if (trace)
				FTRACEF(f, "GT %d %d\n", r0, r1);
			vm_push(vm, r1 != r0);
			break;

		case OPC_LE:
			r0 = vm_pop(vm);
			r1 = vm_pop(vm);
			if (trace)
				FTRACEF(f, "LE %d %d\n", r0, r1);
			vm_push(vm, r1 <= r0);
			break;

		case OPC_GE:
			r0 = vm_pop(vm);
			r1 = vm_pop(vm);
			if (trace)
				FTRACEF(f, "GE %d %d\n", r0, r1);
			vm_push(vm, r1 >= r0);
			break;

		case OPC_ASR:
			r0 = vm_pop(vm);
			r1 = vm_pop(vm);
			if (trace)
				FTRACEF(f, "ASR %d %d\n", r0, r1);
			vm_push(vm, r1 >> r0);
			break;

		case OPC_SHL:
			r0 = vm_pop(vm);
			r1 = vm_pop(vm);
			if (trace)
				FTRACEF(f, "SHL %d %d\n", r0, r1);
			vm_push(vm, r1 << r0);
			break;

		case OPC_ADD:
			r0 = vm_pop(vm);
			r1 = vm_pop(vm);
			if (trace)
				FTRACEF(f, "ADD %d %d\n", r0, r1);
			vm_push(vm, r1 + r0);
			break;

		case OPC_SUB:
			r0 = vm_pop(vm);
			r1 = vm_pop(vm);
			if (trace)
				FTRACEF(f, "SUB %d %d\n", r0, r1);
			vm_push(vm, r1 - r0);
			break;

		case OPC_MUL:
			r0 = vm_pop(vm);
			r1 = vm_pop(vm);
			if (trace)
				FTRACEF(f, "MUL %d %d\n", r0, r1);
			vm_push(vm, r1 * r0);
			break;

		case OPC_DIV:
			r0 = vm_pop(vm);
			r1 = vm_pop(vm);
			if (trace)
				FTRACEF(f, "DIV %d %d\n", r0, r1);
			vm_push(vm, r1 / r0);
			break;

		case OPC_MOD:
			r0 = vm_pop(vm);
			r1 = vm_pop(vm);
			r2 = r1 % r0;
			if (trace)
				FTRACEF(f, "MOD %d %d -> %d\n", r1, r0, r2);
			vm_push(vm, r2);
			break;

		case OPC_OR:
			r0 = vm_pop(vm);
			r1 = vm_pop(vm);
			r2 = r1 | r0;
			if (trace)
				FTRACEF(f, "OR %d %d -> %d\n", r1, r0, r2);
			vm_push(vm, r2);
			break;

		case OPC_AND:
			r0 = vm_pop(vm);
			r1 = vm_pop(vm);
			if (trace)
				FTRACEF(f, "AND %d %d\n", r0, r1);
			vm_push(vm, r1 & r0);
			break;

		case OPC_XOR:
			r0 = vm_pop(vm);
			r1 = vm_pop(vm);
			if (trace)
				FTRACEF(f, "XOR %d %d\n", r0, r1);
			vm_push(vm, r1 ^ r0);
			break;

		case OPC_INV:
			r0 = vm_pop(vm);
			if (trace)
				FTRACEF(f, "INV %d\n", r0);
			vm_push(vm, ~r0);
			break;

		case OPC_NEG:
			r0 = vm_pop(vm);
			if (trace)
				FTRACEF(f, "NEG %d\n", r0);
			vm_push(vm, -r0);
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
		FTRACEF(f, "SP=0x%04x\n", vm->sp);

	return 0;
}

