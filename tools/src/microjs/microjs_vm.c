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

#include <sys/dcclog.h>

/* --------------------------------------------------------------------------
   Virtual machine
   -------------------------------------------------------------------------- */

#define SIGNEXT4BIT(_X) ({ struct { int32_t x: 4; } s; \
						  s.x = (_X); (int32_t)s.x; })

#define SIGNEXT12BIT(_X) ({ struct { int32_t x: 12; } s; \
						  s.x = (_X); (int32_t)s.x; })

#define SIZEOF_WORD ((int)sizeof(int32_t))

#if 0
#define STACK_POP() *sp++
#define STACK_ADJUST(_N) sp += (_N)
#define STACK_PUSH(_X) *(--sp) = (_X)
#endif

#define STACK_POP() *(--sp)
#define STACK_ADJUST(_N) sp -= (_N)
#define STACK_PUSH(_X) *(sp++) = (_X)

#if MICROJS_TRACE_ENABLED
FILE * microjs_vm_tracef = NULL;
#else
#define microjs_vm_tracef NULL
#endif

void microjs_vm_init(struct microjs_vm * vm, const struct microjs_rt * rt,
					 const void * env, int32_t data[], int32_t stack[])
{
	vm->bp = data;
	if ((data == stack) || (stack == NULL)) {
		vm->sp = data + (rt->data_sz / sizeof(int32_t));
	} else {
//		vm->sp = stack + (rt->stack_sz / sizeof(int32_t));
		vm->sp = stack;
	}
	vm->env = (void *)env;
}

void microjs_vm_clr_data(struct microjs_vm * vm, 
						 const struct microjs_rt * rt)
{
	int i;

	/* initialize the VM's data memory */
	for (i = 0; i < rt->data_sz / sizeof(int32_t); ++i)
		vm->bp[i] = 0;
}

int __attribute__((optimize(3))) microjs_exec(struct microjs_vm * vm, 
											  uint8_t code[], unsigned int len)
{
	FILE * f = microjs_vm_tracef;
	bool trace = (f == NULL) ? false : true;
	int32_t r0;
	int32_t r1;
	int32_t r2;
	uint8_t * pc = code;
	int32_t * data = vm->bp;
	int32_t * sp = vm->sp;
	int32_t * xp = sp;
	int opc;
	int opt;
//	int cnt = 0;

	DCC_LOG3(LOG_TRACE, "begin: SP=0x%04x XP=0x%04x PC=0x%04x", 
			 (int)((int)(sp - data) * sizeof(int32_t)),
			 (int)((int)(xp - data) * sizeof(int32_t)),
			 (int)(pc - code));

	if (trace)
		FTRACEF(f, "SP=0x%04x\n", (int)((int)(sp - data) * sizeof(int32_t)));

	for (;;) {
//		if (++cnt == 80)
//			return 1;

		/* fetch */
		if (trace)
			FTRACEF(f, "%04x\t", (int)(pc - code));

		opc = *pc++;
		opt = opc & 0xf;
		opc >>= 4;
	
		DCC_LOG2(LOG_MSG, "OPC=0x%02x OPT=0x%02x", opc, opt);

		switch (opc) {
			/* get the relative address */
		case (OPC_JMP >> 4):
			r0 = SIGNEXT12BIT((*pc++ << 4) + opt);
			pc += r0;
			if (trace)
				FTRACEF(f, "JMP 0x%04x (offs=%d)\n", (int)(pc - code), r0);
			break;

		case (OPC_JEQ >> 4): /* coniditional JMP */
			r0 = SIGNEXT12BIT((*pc++ << 4) + opt);
			r1 = STACK_POP();
			if (trace)
				FTRACEF(f, "JEQ 0x%04x (%d)\n", (int)(pc - code) + r0, r1);
			if (r1 == 0)
				pc += r0;
			break;

		case (OPC_PUSHX >> 4):
			/* get the exception absolute jump address */
			r0 = (*pc++ << 4) + opt;
			/* combine with the current exception frame pointer */
			r0 |= ((xp - data) << 16);
			STACK_PUSH(r0);
			xp = sp; /* update the exception pointer */
			if (trace)
				FTRACEF(f, "PUSHX 0x%04x (XP=0x%04x->0x%04x) \n", 
						r0 & 0xffff, (r0 >> 16) * SIZEOF_WORD, 
						(int)(xp - data) * SIZEOF_WORD);
			break;

		case (OPC_ISP >> 4):
			r0 = SIGNEXT12BIT((*pc++ << 4) + opt);
			STACK_ADJUST(r0);
			if (trace)
				FTRACEF(f, "ISP %d\n", r0);
			break;

		case (OPC_LD >> 4):
			r1 = (*pc++ << 4) + opt;
			r0 = data[r1];
			STACK_PUSH(r0);
			if (trace)
				FTRACEF(f, "LD 0x%04x -> %d\n", r1 * SIZEOF_WORD, r0);
			break;

		case (OPC_ST >> 4):
			r1 = (*pc++ << 4) + opt;
			r0 = STACK_POP();
			data[r1] = r0;
			if (trace)
				FTRACEF(f, "ST 0x%04x <- %d\n", r1 * SIZEOF_WORD, r0);
			break;

#if MICROJS_FUNCTIONS_ENABLED
		case OPC_LDR:
			r1 = *pc++;
			r0 = bp[r1];
			STACK_PUSH(r0);
			if (trace)
				FTRACEF(f, "LDR 0x%04x -> %d\n", r1 * SIZEOF_WORD, r0);
			break;

		case OPC_STR:
			r1 = *pc++;
			r0 = STACK_POP();
			bp[r1] = r0;
			if (trace)
				FTRACEF(f, "STR 0x%04x <- %d\n", r1 * SIZEOF_WORD, r0);
			break;

		case OPC_IBP:
			r0 = (int8_t)*pc++;
			bp += r0;
			if (trace)
				FTRACEF(f, "ISP %d\n", r0);
			break;
#endif
		case (OPC_I4 >> 4): 
			r0 = SIGNEXT4BIT(opt);
			STACK_PUSH(r0);
			if (trace)
				FTRACEF(f, "I4 %d\n", r0);
			break;

		case (INTOP >> 4):
			r0 = STACK_POP();
			r1 = STACK_POP();

			switch (opt) {
			case OPC_LT - INTOP:
				r2 = r1 < r0;
				if (trace)
					FTRACEF(f, "LT %d %d -> %d\n", r0, r1, r2);
				break;

			case OPC_GT - INTOP:
				r2 = r1 > r0;
				if (trace)
					FTRACEF(f, "GT %d %d -> %d\n", r0, r1, r2);
				break;

			case OPC_EQ - INTOP:
				r2 = r1 == r0;
				if (trace)
					FTRACEF(f, "EQ %d %d -> %d\n", r0, r1, r2);
				break;

			case OPC_NE - INTOP:
				r2 = r1 != r0;
				if (trace)
					FTRACEF(f, "NE %d %d -> %d\n", r1, r0, r2);
				break;

			case OPC_LE - INTOP:
				r2 = r1 <= r0;
				if (trace)
					FTRACEF(f, "LE %d %d -> %d\n", r1, r0, r2);
				break;

			case OPC_GE - INTOP:
				r2 = r1 >= r0;
				if (trace)
					FTRACEF(f, "GE %d %d -> %d\n", r1, r0, r2);
				break;

			case OPC_ASR - INTOP:
				r2 = r1 >> r0;
				if (trace)
					FTRACEF(f, "ASR %d %d -> %d\n", r1, r0, r2);
				break;

			case OPC_SHL - INTOP:
				r2 = r1 << r0;
				if (trace)
					FTRACEF(f, "SHL %d %d -> %d\n", r1, r0, r2);
				break;

			case OPC_ADD - INTOP:
				r2 = r1 + r0;
				if (trace)
					FTRACEF(f, "ADD %d %d -> %d\n", r1, r0, r2);
				break;

			case OPC_SUB - INTOP:
				r2 = r1 - r0;
				if (trace)
					FTRACEF(f, "SUB %d %d -> %d\n", r1, r0, r2);
				break;

			case OPC_MUL - INTOP:
				r2 = r1 * r0;
				if (trace)
					FTRACEF(f, "MUL %d %d -> %d\n", r1, r0, r2);
				break;

			case OPC_DIV - INTOP:
				r2 = r1 / r0;
				if (trace)
					FTRACEF(f, "DIV %d %d -> %d\n", r1, r0, r2);
				break;

			case OPC_MOD - INTOP:
				r2 = r1 % r0;
				if (trace)
					FTRACEF(f, "MOD %d %d -> %d\n", r1, r0, r2);
				break;

			case OPC_OR - INTOP:
				r2 = r1 | r0;
				if (trace)
					FTRACEF(f, "OR %d %d -> %d\n", r1, r0, r2);
				break;

			case OPC_AND - INTOP:
				r2 = r1 & r0;
				if (trace)
					FTRACEF(f, "AND %d %d -> %d\n", r1, r0, r2);
				break;

			case OPC_XOR - INTOP:
				r2 = r1 ^ r0;
				if (trace)
					FTRACEF(f, "XOR %d %d -> %d\n", r1, r0, r2);
				break;
			}
			STACK_PUSH(r2);
			break;

		case (MISCOP >> 4):
			switch (opt) {

			case OPC_INV:
				r0 = STACK_POP();
				r1 = ~r0;
				STACK_PUSH(r1);
				if (trace)
					FTRACEF(f, "INV %d -> %d\n", r0, r1);
				break;

			case OPC_NEG:
				r0 = STACK_POP();
				r1 = -r0;
				STACK_PUSH(r1);
				if (trace)
					FTRACEF(f, "NEG %d -> %d\n", r0, r1);
				break;

			case OPC_NOT:
				r0 = STACK_POP();
				r1 = !r0;
				STACK_PUSH(r1);
				if (trace)
					FTRACEF(f, "NOT %d -> %d\n", r0, r1);
				break;

			case OPC_INC:
				r0 = STACK_POP();
				STACK_PUSH(r0 + 1);
				if (trace)
					FTRACEF(f, "INC %d\n", r0);
				break;
				
			case OPC_DEC:
				r0 = STACK_POP();
				STACK_PUSH(r0 - 1);
				if (trace)
					FTRACEF(f, "DEC %d\n", r0);
				break;

			case OPC_POP:
				if (trace)
					FTRACEF(f, "POP\n");
				STACK_ADJUST(1);
				break;

			case OPC_I32:
				r0 = (int32_t)(pc[0] | pc[1] << 8 | 
							   pc[2] << 16 | pc[3] << 24);
				pc += 4;
				STACK_PUSH(r0);
				if (trace)
					FTRACEF(f, "I32 %d\n", r0);
				break;

			case OPC_I16:
				r0 = (int16_t)(pc[0] | pc[1] << 8);
				pc += 2;
				STACK_PUSH(r0);
				if (trace)
					FTRACEF(f, "I16 0x%04x\n", r0);
				break;

			case OPC_I8:
				r0 = (int8_t)*pc++;
				STACK_PUSH(r0);
				if (trace)
					FTRACEF(f, "I8 %d\n", r0);
				break;

			case OPC_EXT:
				r0 = *pc++;
				r1 = *pc++;
				if (trace)
					FTRACEF(f, "EXT %d, %d\n", r0, r1);
				r0 = microjs_extern[r0](&vm->env, sp - r1, r1);
				if (r0 < 0) {
					DCC_LOG1(LOG_INFO, "exception %d!", r0);
					r1 = -r0;
					goto except;
				}
				/* adjust the stack pointer */
				STACK_ADJUST(r1 - r0);
				break;


			case OPC_XPT: 
				r1 = *sp; /* get the exception code */
except:
				sp = xp;  /* set the stack pointer to the exception frame */
				r0 = *sp; /* get the exception frame */
				*sp = r1; /* push the exeption code */
				pc = code + (r0 & 0xffff); /* go to the exception handler */
				xp = data + (r0 >> 16); /* update the exception frame */
				if (trace)
					FTRACEF(f, "XPT (PC=0x%04x XP=0x%04x)\n", 
							r0 & 0xffff, (r0 >> 16) * SIZEOF_WORD);
				break;

			case OPC_RET:
				r0 = STACK_POP();
				if (trace)
					FTRACEF(f, "RET %d\n", r0);
				DCC_LOG1(LOG_INFO, "return %d!", r0);
				goto done;

			case OPC_ABT:
				r0 = 0;
				if (trace)
					FTRACEF(f, "ABT\n");
				goto done;

			default:
				r1 = ERR_INVALID_INSTRUCTION;
				DCC_LOG1(LOG_INFO, "Invalid instruction, MISC(%d)", opt);
				goto except;
			}
			break;

		default:
			DCC_LOG1(LOG_INFO, "Invalid instruction: %d", opc);
			r1 = ERR_INVALID_INSTRUCTION;
			goto except;
		}
	} 

done:
	if (trace)
		FTRACEF(f, "SP=0x%04x\n", (int)(sp - data) * SIZEOF_WORD);

	DCC_LOG3(LOG_TRACE, "end: SP=0x%04x XP=0x%04x PC=0x%04x", 
			(int)((int)(sp - data) * sizeof(int32_t)),
			(int)((int)(xp - data) * sizeof(int32_t)), 
			(int)(pc - code));

	return r0;
}

