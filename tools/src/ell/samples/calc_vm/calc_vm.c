/* $Id: dcclog.c,v 1.10 2006/09/28 19:31:45 bob Exp $ 
 *
 * File:	dcclog.c
 * Module:
 * Project:	ARM-DCC logger expander
 * Author:	Robinson Mittmann (bob@boreste.com, bob@methafora.com.br)
 * Target:
 * Comment:
 * Copyright(C) 2005 Robinson Mittmann. All Rights Reserved.
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
 * 
 */

#include <unistd.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "calc.h"

/* --------------------------------------------------------------------------
   Virtual machine
   -------------------------------------------------------------------------- */

static int32_t vm_rd32(struct calc_vm * vm, unsigned int addr)
{
	return vm->data[addr >> 2];
}

static void vm_wr32(struct calc_vm * vm, unsigned int addr, int32_t val)
{
	vm->data[addr >> 2] = val;
}

static void vm_push(struct calc_vm * vm, int32_t x)
{
	vm->sp -= 4;
	vm->data[vm->sp >> 2] = x;	
}

static int32_t vm_pop(struct calc_vm * vm)
{
	int32_t x;

	x = vm->data[vm->sp >> 2];
	vm->sp += 4;

	return x;
}

void calc_vm_init(struct calc_vm * vm, int32_t data[], unsigned int len)
{
	vm->data = data;
	vm->sp = len;
	vm->sl = 0;
	vm->trace = false;
}

int calc_exec(struct calc_vm * vm, uint8_t code[], unsigned int len)
{
	int32_t r0;
	int32_t r1;
	int32_t r2;
	int opc;
	int pc;
	int i;

	pc = 0; 
	r0 = 0;
	r1 = 0;
	i = 0;
	while (pc < len) {
		if (++i == 1000)
			return -1;
		/* fetch */
		if (vm->trace) {
			printf("%04x\t", pc);
			fflush(stdout);
		}
		opc = code[pc++];
		switch (opc) {

		case OPC_JMP:
			r0 = (int16_t)(code[pc] | code[pc + 1] << 8);
			pc += 2;
			pc += r0;
			if (vm->trace)
				printf("JMP 0x%04x (offs=%d)\n", pc, r0);
			break;

		case OPC_JNE:
			r0 = (int16_t)(code[pc] | code[pc + 1] << 8);
			pc += 2;
			r1 = vm_pop(vm);
			if (vm->trace)
				printf("JNE 0x%04x (%d)\n", pc + r0, r1);
			if (r1 != 0)
				pc += r0;
			break;

		case OPC_JEQ:
			r0 = (int16_t)(code[pc] | code[pc + 1] << 8);
			pc += 2;
			r1 = vm_pop(vm);
			if (vm->trace)
				printf("JEQ 0x%04x (%d)\n", pc + r0, r1);
			if (r1 == 0)
				pc += r0;
			break;

		case OPC_I32:
			r0 = code[pc++];
			r0 |= code[pc++] << 8;
			r0 |= code[pc++] << 16;
			r0 |= code[pc++] << 24;
			if (vm->trace)
				printf("I32 %d\n", r0);
			vm_push(vm, r0);
			break;

		case OPC_I16:
			r0 = code[pc++];
			r0 |= code[pc++] << 8;
			if (vm->trace)
				printf("I16 0x%04x\n", r0);
			vm_push(vm, r0);
			break;

		case OPC_I8:
			r0 = code[pc++];
			if (vm->trace)
				printf("I8 %d\n", r0);
			vm_push(vm, r0);
			break;

		case OPC_LD:
			r1 = vm_pop(vm);
			r0 = vm_rd32(vm, r1);
			if (vm->trace)
				printf("LD 0x%04x -> %d\n", r1, r0);
			vm_push(vm, r0);
			break;

		case OPC_ST:
			r0 = vm_pop(vm);
			r1 = vm_pop(vm);
			if (vm->trace)
				printf("ST 0x%04x <- %d\n", r1, r0);
			vm_wr32(vm, r1, r0);
			break;

		case OPC_LT:
			r0 = vm_pop(vm);
			r1 = vm_pop(vm);
			if (vm->trace)
				printf("LT %d %d\n", r0, r1);
			vm_push(vm, r1 < r0);
			break;

		case OPC_GT:
			r0 = vm_pop(vm);
			r1 = vm_pop(vm);
			if (vm->trace)
				printf("GT %d %d\n", r0, r1);
			vm_push(vm, r1 > r0);
			break;

		case OPC_EQ:
			r0 = vm_pop(vm);
			r1 = vm_pop(vm);
			if (vm->trace)
				printf("EQ %d %d\n", r0, r1);
			vm_push(vm, r1 == r0);
			break;

		case OPC_NE:
			r0 = vm_pop(vm);
			r1 = vm_pop(vm);
			if (vm->trace)
				printf("GT %d %d\n", r0, r1);
			vm_push(vm, r1 != r0);
			break;

		case OPC_LE:
			r0 = vm_pop(vm);
			r1 = vm_pop(vm);
			if (vm->trace)
				printf("LE %d %d\n", r0, r1);
			vm_push(vm, r1 <= r0);
			break;

		case OPC_GE:
			r0 = vm_pop(vm);
			r1 = vm_pop(vm);
			if (vm->trace)
				printf("GE %d %d\n", r0, r1);
			vm_push(vm, r1 >= r0);
			break;

		case OPC_ASR:
			r0 = vm_pop(vm);
			r1 = vm_pop(vm);
			vm_push(vm, r1 >> r0);
			break;

		case OPC_SHL:
			r0 = vm_pop(vm);
			r1 = vm_pop(vm);
			vm_push(vm, r1 << r0);
			break;

		case OPC_ADD:
			r0 = vm_pop(vm);
			r1 = vm_pop(vm);
			if (vm->trace)
				printf("ADD %d %d\n", r0, r1);
			vm_push(vm, r1 + r0);
			break;

		case OPC_SUB:
			r0 = vm_pop(vm);
			r1 = vm_pop(vm);
			if (vm->trace)
				printf("SUB %d %d\n", r0, r1);
			vm_push(vm, r1 - r0);
			break;

		case OPC_MUL:
			r0 = vm_pop(vm);
			r1 = vm_pop(vm);
			if (vm->trace)
				printf("MUL %d %d\n", r0, r1);
			vm_push(vm, r1 * r0);
			break;

		case OPC_DIV:
			r0 = vm_pop(vm);
			r1 = vm_pop(vm);
			if (vm->trace)
				printf("DIV %d %d\n", r0, r1);
			vm_push(vm, r1 / r0);
			break;

		case OPC_MOD:
			r0 = vm_pop(vm);
			r1 = vm_pop(vm);
			r2 = r1 % r0;
			if (vm->trace)
				printf("MOD %d %d -> %d\n", r1, r0, r2);
			vm_push(vm, r2);
			break;

		case OPC_OR:
			r0 = vm_pop(vm);
			r1 = vm_pop(vm);
			vm_push(vm, r1 | r0);
			break;

		case OPC_AND:
			r0 = vm_pop(vm);
			r1 = vm_pop(vm);
			vm_push(vm, r1 & r0);
			break;

		case OPC_XOR:
			r0 = vm_pop(vm);
			r1 = vm_pop(vm);
			vm_push(vm, r1 ^ r0);
			break;

		case OPC_INV:
			r0 = vm_pop(vm);
			vm_push(vm, ~r0);
			break;

		case OPC_NEG:
			r0 = vm_pop(vm);
			vm_push(vm, -r0);
			break;

		case OPC_PRINT:
			r0 = vm_pop(vm);
			if (vm->trace)
				printf("PRINT %d\n", r0);
			printf("%d\n", r0);
			break;
		
		default:
			fprintf(stderr, "Invalid OPC: %d\n", opc);
			return -1;
		}
	} 

	return 0;
}

