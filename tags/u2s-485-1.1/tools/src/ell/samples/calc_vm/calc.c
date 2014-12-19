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

#define DEBUG 0
#include "debug.h"

#include "calc.h"

/* --------------------------------------------------------------------------
   Memory operations
   -------------------------------------------------------------------------- */

int alloc32(struct calc * calc)
{
	unsigned int addr;

	/* ensure memory alignment */
	addr = (calc->heap + 3) & ~3;
	calc->heap = addr + 4;

	if (calc->heap > calc->sp) {
		fprintf(stderr, "heap overflow!\n");
		return -1;
	}

	return addr;
}


int32_t mem_rd32(struct calc * calc, unsigned int addr)
{
	return calc->mem[addr >> 2];
}

void mem_wr32(struct calc * calc, unsigned int addr, int32_t val)
{
	calc->mem[addr >> 2] = val;
}

int push(struct calc * calc, int32_t x)
{
	calc->sp -= 4;
	calc->mem[calc->sp >> 2] = x;	

	if (calc->sp < calc->heap) {
		fprintf(stderr, "stack overflow!\n");
		return -1;
	}
	return 0;
}

int32_t pop(struct calc * calc)
{
	int32_t x;

	if (calc->sp == calc->stack) {
		return 0;
	}

	x = calc->mem[calc->sp >> 2];
	calc->sp += 4;

	return x;
}

int mem_bind(struct calc * calc)
{
	struct sym_tab * tab = calc->tab;
	int i;

	/* search in the global list */
	for (i = 0; i < tab->global; ++i) {
		struct sym * sp = &tab->sym[i];
		if ((sp->flags & SYM_OBJECT) && !(sp->flags & SYM_OBJ_ALLOC)) {
			struct sym_obj * obj = (struct sym_obj *)sp;
			int addr;
			if ((addr = alloc32(calc)) < 0)
				return -1;
			obj->addr = addr;
			obj->flags |= SYM_OBJ_ALLOC;
		}
	}

	/* search in the global list */
	for (i = 0; i < tab->global; ++i) {
		struct sym_ref * ref = (struct sym_ref *)&tab->sym[i];
		if (!(ref->flags & (SYM_OBJECT | SYM_EXTERN))) {
			struct sym_obj * obj = (struct sym_obj *)&tab->sym[ref->oid];
			printf("ref=0x%04x oid=%d --> obj=0x%04x\n", ref->addr, 
				   ref->oid, obj->addr);
			calc->code[ref->addr] = obj->addr;
			calc->code[ref->addr + 1] = obj->addr >> 8;
		}
	}

	return 0;
}




/* --------------------------------------------------------------------------
   Code generation
   -------------------------------------------------------------------------- */

int op_var_decl(struct calc * calc)
{
	struct sym_obj * obj;
	int addr;

	if ((obj = sym_obj_new(calc->tab, calc->tok.s, calc->tok.qlf)) == NULL) {
		fprintf(stderr, "can't create symbol.\n");
		return -1;
	}

	if ((addr = alloc32(calc)) < 0)
		return -1;

	obj->addr = addr;
	/* initial variables are int */
	obj->size = 4;
	/* flag as allocated */
	obj->flags |= SYM_OBJ_ALLOC;
	return 0;
}

int op_push_sym(struct calc * calc)
{
	struct sym_tmp * tmp;
		
	if ((tmp = sym_tmp_push(calc->tab, calc->tok.s, calc->tok.qlf)) == NULL) {
		fprintf(stderr, "cant create temp symbol: %s.\n", tok2str(calc->tok));
		return -1;
	}

	DBG("tmp=\"%s\"", sym_name(calc->tab, tmp->nm));
	return 0;
}

int op_meth_or_attr(struct calc * calc)
{
	struct sym_tmp * tmp = sym_tmp_get(calc->tab, 0);

	if (SYM_IS_METHOD(tmp)) {
		DBG("method=\"%s\" (%d)", sym_name(calc->tab, tmp->nm), tmp->xid);
		DBG("min=%d max=%d argc=%d", tmp->min, tmp->max, tmp->cnt);

		if (tmp->cnt < tmp->min) {
			fprintf(stderr, "argument missing: %s\n", 
					sym_name(calc->tab, tmp->nm));
			return -1;
		}
		if (tmp->cnt > tmp->max) {
			fprintf(stderr, "too many arguments to function: %s\n", 
					sym_name(calc->tab, tmp->nm));
			return -1;
		}

		/* number of arguments */
		printf("%04x\tI18 %d\n", calc->pc, tmp->xid);
		calc->code[calc->pc++] = OPC_I8;
		calc->code[calc->pc++] = tmp->cnt;
		/* external call number */
		printf("%04x\tI18 %d\n", calc->pc, tmp->xid);
		calc->code[calc->pc++] = OPC_I8;
		calc->code[calc->pc++] = tmp->xid;
		printf("%04x\tEXT \'%s\"\n", calc->pc, sym_name(calc->tab, tmp->nm));
		calc->code[calc->pc++] = OPC_EXT;
	} else {
		struct sym_obj * obj;
		DBG("attribute=\"%s\"", sym_name(calc->tab, tmp->nm));
		if ((obj = sym_obj_lookup(calc->tab, tmp->nm)) == NULL) {
			fprintf(stderr, "undefined symbol: %s.\n", 
					sym_name(calc->tab, tmp->nm));
			return -1;
		}
#if 0
		struct sym_ref * ref;
		ref = sym_ref_new(calc->tab, obj);
		printf("ref=0x%04x oid=%d --> obj=0x%04x\n", ref->addr, 
			   ref->oid, obj->addr);
		ref->addr = calc->pc + 1;
#endif
		printf("%04x\tI16 \'%s\"\n", calc->pc, sym_name(calc->tab, tmp->nm));
		calc->code[calc->pc++] = OPC_I16;
		calc->code[calc->pc++] = obj->addr;
		calc->code[calc->pc++] = obj->addr >> 8;
		printf("%04x\tLD\n", calc->pc);
		calc->code[calc->pc++] = OPC_LD;
	}

	return 0;
}

int op_pop_sym(struct calc * calc)
{
	sym_pop(calc->tab);
	return 0;
}

int op_method(struct calc * calc)
{
	struct ext_entry * ext;
	struct sym_tmp * tmp;
	int xid;
	tmp = sym_tmp_get(calc->tab, 0);
	tmp->flags |= SYM_METHOD;

	xid = extern_lookup(tmp->nm);
	if ((ext = extern_get(xid)) == NULL) {
		fprintf(stderr, "undefined external: %s.\n", 
				sym_name(calc->tab, tmp->nm));
		return -1;
	}
	DBG("method=\"%s\" (%d)", sym_name(calc->tab, tmp->nm), xid);
	tmp->min = ext->argmin;
	tmp->max = ext->argmax;
	tmp->cnt = 0;
	tmp->xid = xid;

	/* prepare to call a function or method */
	return 0;
}

int op_arg(struct calc * calc)
{
	struct sym_tmp * tmp;
	tmp = sym_tmp_get(calc->tab, 0);
	tmp->cnt++; /* increment the argument counter */
	DBG("argc=%d", tmp->cnt);
	return 0;
}

int op_ret_discard(struct calc * calc)
{
	printf("%04x\tPOP\n", calc->pc);
	calc->code[calc->pc++] = OPC_POP;
	return 0;
}

#if 0
int op_assign(struct calc * calc)
{
	struct sym_tmp * tmp = sym_tmp_get(calc->tab, 0);
	struct sym_obj * obj;
	struct sym_ref * ref;

	DBG("assign=\"%s\"", sym_name(calc->tab, tmp->nm));
	if ((obj = sym_obj_lookup(calc->tab, tmp->nm)) == NULL) {
		fprintf(stderr, "undefined symbol: %s.\n", 
				sym_name(calc->tab, tmp->nm));
		return -1;
	}
	ref = sym_ref_new(calc->tab, obj);
	printf("ref=0x%04x oid=%d --> obj=0x%04x\n", ref->addr, 
		   ref->oid, obj->addr);
	printf("%04x\tI16 \'%s\"\n", calc->pc, sym_name(calc->tab, tmp->nm));
	calc->code[calc->pc++] = OPC_I16;
	ref->addr = calc->pc;
	calc->code[calc->pc++] = 0;
	calc->code[calc->pc++] = 0;
	printf("%04x\tST\n", calc->pc);
	calc->code[calc->pc++] = OPC_ST;
	return 0;
}
#endif

int op_assign(struct calc * calc)
{
	struct sym_tmp * tmp = sym_tmp_get(calc->tab, 0);
	struct sym_obj * obj;

	DBG("assign=\"%s\"", sym_name(calc->tab, tmp->nm));
	if ((obj = sym_obj_lookup(calc->tab, tmp->nm)) == NULL) {
		fprintf(stderr, "undefined symbol: %s.\n", 
				sym_name(calc->tab, tmp->nm));
		return -1;
	}
	printf("%04x\tI16 \'%s\"\n", calc->pc, sym_name(calc->tab, tmp->nm));
	calc->code[calc->pc++] = OPC_I16;
	calc->code[calc->pc++] = obj->addr;
	calc->code[calc->pc++] = obj->addr >> 8;
	printf("%04x\tST\n", calc->pc);
	calc->code[calc->pc++] = OPC_ST;
	return 0;
}

int op_equ(struct calc * calc)
{
	printf("%04x\tEQ\n", calc->pc);
	calc->code[calc->pc++] = OPC_EQ;
	return 0;
}

int op_neq(struct calc * calc)
{
	printf("%04x\tNE\n", calc->pc);
	calc->code[calc->pc++] = OPC_NE;
	return 0;
}

int op_gt(struct calc * calc)
{
	printf("%04x\tGT\n", calc->pc);
	calc->code[calc->pc++] = OPC_GT;
	return 0;
}

int op_lt(struct calc * calc)
{
	printf("%04x\tLT\n", calc->pc);
	calc->code[calc->pc++] = OPC_LT;
	return 0;
}

int op_gte(struct calc * calc)
{
	printf("%04x\tGE\n", calc->pc);
	calc->code[calc->pc++] = OPC_GE;
	return 0;
}

int op_lte(struct calc * calc)
{
	printf("%04x\tLE\n", calc->pc);
	calc->code[calc->pc++] = OPC_LE;
	return 0;
}

int op_logic_or(struct calc * calc)
{
	printf("%04x\tOR\n", calc->pc);
	calc->code[calc->pc++] = OPC_OR;
	return 0;
}

int op_logic_and(struct calc * calc)
{
	printf("%04x\tAND\n", calc->pc);
	calc->code[calc->pc++] = OPC_AND;
	return 0;
}

int op_add(struct calc * calc)
{
	printf("%04x\tADD\n", calc->pc);
	calc->code[calc->pc++] = OPC_ADD;
	return 0;
}

int op_sub(struct calc * calc)
{
	printf("%04x\tSUB\n", calc->pc);
	calc->code[calc->pc++] = OPC_SUB;
	return 0;
}

int op_or(struct calc * calc)
{
	printf("%04x\tOR\n", calc->pc);
	calc->code[calc->pc++] = OPC_OR;
	return 0;
}

int op_xor(struct calc * calc)
{
	calc->code[calc->pc++] = OPC_XOR;
	return 0;
}

int op_mul(struct calc * calc)
{
	printf("%04x\tMUL\n", calc->pc);
	calc->code[calc->pc++] = OPC_MUL;
	return 0;
}

int op_div(struct calc * calc)
{
	printf("%04x\tDIV\n", calc->pc);
	calc->code[calc->pc++] = OPC_DIV;
	return 0;
}

int op_mod(struct calc * calc)
{
	calc->code[calc->pc++] = OPC_MOD;
	return 0;
}

int op_and(struct calc * calc)
{
	calc->code[calc->pc++] = OPC_AND;
	return 0;
}

int op_inv(struct calc * calc)
{
	calc->code[calc->pc++] = OPC_INV;
	return 0;
}

int op_minus(struct calc * calc)
{
	calc->code[calc->pc++] = OPC_NEG;
	return 0;
}

int op_not(struct calc * calc)
{
	calc->code[calc->pc++] = OPC_NEG;
	return 0;
}

int op_shl(struct calc * calc)
{
	calc->code[calc->pc++] = OPC_SHL;
	return 0;
}

int op_asr(struct calc * calc)
{
	calc->code[calc->pc++] = OPC_ASR;
	return 0;
}

int op_push_false(struct calc * calc)
{
	printf("%04x\tI8 %d\n", calc->pc, 0);
	calc->code[calc->pc++] = OPC_I8;
	calc->code[calc->pc++] = 0;
	return 0;
}

int op_push_true(struct calc * calc)
{
	printf("%04x\tI8 %d\n", calc->pc, 1);
	calc->code[calc->pc++] = OPC_I8;
	calc->code[calc->pc++] = 1;
	return 0;
}

int op_push_string(struct calc * calc) 
{
	int isz;

	if ((isz = cstr_add(calc->tok.s, calc->tok.qlf)) < 0) {
		fprintf(stderr, "can't create string!\n");
		return -1;
	}
	
	printf("%04x\tI8 %d\n", calc->pc, isz);
	calc->code[calc->pc++] = OPC_I8;
	calc->code[calc->pc++] = isz;
	return 0;
}

int op_push_int(struct calc * calc)
{
	int32_t x = calc->tok.u32;

	printf("%04x\t", calc->pc);

	if (x >= 0) {
		if (x < 32768) {
			if (x < 128) {
				printf("I8 %d\n", x);
				calc->code[calc->pc++] = OPC_I8;
				calc->code[calc->pc++] = x;
			} else {
				printf("I16 %d\n", x);
				calc->code[calc->pc++] = OPC_I16;
				calc->code[calc->pc++] = x;
				calc->code[calc->pc++] = x >> 8;
			}
		} else {
			printf("I32 %d\n", x);
			calc->code[calc->pc++] = OPC_I32;
			calc->code[calc->pc++] = x;
			calc->code[calc->pc++] = x >> 8;
			calc->code[calc->pc++] = x >> 16;
			calc->code[calc->pc++] = x >> 24;
		}
	} else {
		if (x >= -32768) {
			if (x >= -128) {
				printf("I8 %d\n", x);
				calc->code[calc->pc++] = OPC_I8;
				calc->code[calc->pc++] = x;
			} else {
				printf("I16 %d\n", x);
				calc->code[calc->pc++] = OPC_I16;
				calc->code[calc->pc++] = x;
				calc->code[calc->pc++] = x >> 8;
			}
		} else {
			printf("I32 %d\n", x);
			calc->code[calc->pc++] = OPC_I32;
			calc->code[calc->pc++] = x;
			calc->code[calc->pc++] = x >> 8;
			calc->code[calc->pc++] = x >> 16;
			calc->code[calc->pc++] = x >> 24;
		}
	}

	return 0;
}

int op_while_begin(struct calc * calc)
{
	int id;

	if ((id = sym_anom_push(calc->tab)) < 0)
		return id;

	printf(".L%d:\n", id);
	/* save current location on a temporary variable */
	sym_addr_set(calc->tab, id, calc->pc);
	return 0;
}

int op_while_cond(struct calc * calc)
{
	int id;

	if ((id = sym_anom_push(calc->tab)) < 0)
		return id;

	printf(".L%d:\n%04x\tJEQ xxxx\n", id, calc->pc);

	/* save current location on a temporary variable */
	calc->code[calc->pc++] = OPC_JEQ;
	sym_addr_set(calc->tab, id, calc->pc);
	/* reserve 2 positions for jump address */
	calc->pc += 2;
	return 0;
}


int op_while_end(struct calc * calc)
{
	int id;
	int addr;
	int offs;

	/* Adjust the conditinal jump */
	id = sym_anom_pop(calc->tab);
	addr = sym_addr_get(calc->tab, id);
	offs = calc->pc - (addr - 1);
	printf("\tfix %04x -> JEQ %04x (.L%d)\n", addr - 1, calc->pc + 3, id);
	calc->code[addr++] = offs;
	calc->code[addr++] = offs >> 8;

	/* Repeat jump */
	id = sym_anom_pop(calc->tab);
	addr = sym_addr_get(calc->tab, id);
	offs = addr - (calc->pc + 3);
	printf("%04x\tJMP %04x (.L%d offs=%d)\n", calc->pc, addr, id, offs);
	calc->code[calc->pc++] = OPC_JMP;
	calc->code[calc->pc++] = offs;
	calc->code[calc->pc++] = offs >> 8;

	return 0;
}


int op_if_cond(struct calc * calc)
{
	int id;

	if ((id = sym_anom_push(calc->tab)) < 0)
		return id;

	printf(".L%d:\n%04x\tJEQ xxxx\n", id, calc->pc);
	/* save current location on a temporary variable */
	calc->code[calc->pc++] = OPC_JEQ;
	sym_addr_set(calc->tab, id, calc->pc);
	/* reserve 2 positions for jump address */
	calc->pc += 2;

	return 0;
}

int op_if_else(struct calc * calc)
{
	int id;
	int addr;
	int offs;

	id = sym_anom_get(calc->tab, 0);
	addr = sym_addr_get(calc->tab, id);

	offs = calc->pc - (addr - 1);
	printf("\tfix %04x -> JEQ %04x (.L%d)\n", addr - 1, calc->pc + 3, id);
	calc->code[addr++] = offs;
	calc->code[addr++] = offs >> 8;

	printf(".L%d:\n%04x\tJMP xxxx\n", id, calc->pc);
	/* save current location on the same temporary variable */
	calc->code[calc->pc++] = OPC_JMP;
	sym_addr_set(calc->tab, id, calc->pc);
	/* reserve 2 positions for jump address */
	calc->pc += 2;

	return 0;
}

int op_if_end(struct calc * calc)
{
	int id;
	int addr;
	int offs;

	/* Adjust the conditinal jump */
	id = sym_anom_pop(calc->tab);
	addr = sym_addr_get(calc->tab, id);

	offs = (calc->pc - 3) - (addr - 1);
	printf("\tfix %04x -> Jxx %04x (.L%d)\n", addr - 1, calc->pc, id);
	calc->code[addr++] = offs;
	calc->code[addr++] = offs >> 8;

	return 0;
}

int op_for_init(struct calc * calc)
{
	int id;

	/* allocate 4 anonymous symbols */
	if ((id = sym_anom_push(calc->tab)) < 0)
		return id;

	printf(".L%d:\n", id);
	/* save current location */
	sym_addr_set(calc->tab, id, calc->pc);

	if ((id = sym_anom_push(calc->tab)) < 0)
		return id;
	if ((id = sym_anom_push(calc->tab)) < 0)
		return id;
	if ((id = sym_anom_push(calc->tab)) < 0)
		return id;

	return 0;
}

int op_for_cond(struct calc * calc)
{
	int id;

	id = sym_anom_get(calc->tab, 2);
	/* Conditional jump to the body */
	printf(".L%d:\n%04x\tJEQ xxxx\n", id, calc->pc);
	/* save current location on a temporary variable */
	calc->code[calc->pc++] = OPC_JEQ;
	sym_addr_set(calc->tab, id, calc->pc);
	/* reserve 2 positions for jump address */
	calc->pc += 2;

	id = sym_anom_get(calc->tab, 1);
	/* Jump to the beginning of the body part */
	printf(".L%d:\n%04x\tJMP xxxx\n", id, calc->pc);
	/* save current location on a temporary variable */
	calc->code[calc->pc++] = OPC_JMP;
	sym_addr_set(calc->tab, id, calc->pc);
	/* reserve 2 positions for jump address */
	calc->pc += 2;

	id = sym_anom_get(calc->tab, 0);
	printf(".L%d:\n", id);
	/* save current location */
	sym_addr_set(calc->tab, id, calc->pc);

	return 0;
}

int op_for_after(struct calc * calc)
{
	int id;
	int addr;
	int offs;

	/* Adjust the jump to body */
	id = sym_anom_get(calc->tab, 1);
	addr = sym_addr_get(calc->tab, id);
	offs = calc->pc - (addr - 1);
	printf("\tfix %04x -> JMP %04x (.L%d)\n", addr - 1, calc->pc + 3, id);
	calc->code[addr++] = offs;
	calc->code[addr++] = offs >> 8;

	/* Repeat jump */
	id = sym_anom_get(calc->tab, 3);
	addr = sym_addr_get(calc->tab, id);
	offs = addr - (calc->pc + 3);
	printf("%04x\tJMP %04x (.L%d offs=%d)\n", calc->pc, addr, id, offs);
	calc->code[calc->pc++] = OPC_JMP;
	calc->code[calc->pc++] = offs;
	calc->code[calc->pc++] = offs >> 8;

	return 0;
}

int op_for_end(struct calc * calc)
{
	int id;
	int addr;
	int offs;

	/* Adjust the conditinal jump */
	id = sym_anom_get(calc->tab, 2);
	addr = sym_addr_get(calc->tab, id);

	offs = calc->pc - (addr - 1);
	printf("\tfix %04x -> JEQ %04x (.L%d)\n", addr - 1, calc->pc + 3, id);
	calc->code[addr++] = offs;
	calc->code[addr++] = offs >> 8;

	/* jump to the afterthought part */
	id = sym_anom_get(calc->tab, 0);
	addr = sym_addr_get(calc->tab, id);
	offs = addr - (calc->pc + 3);
	printf("%04x\tJMP %04x (.L%d offs=%d)\n", calc->pc, addr, id, offs);
	calc->code[calc->pc++] = OPC_JMP;
	calc->code[calc->pc++] = offs;
	calc->code[calc->pc++] = offs >> 8;

	/* remove temp symbols */
	sym_anom_pop(calc->tab);
	sym_anom_pop(calc->tab);
	sym_anom_pop(calc->tab);
	sym_anom_pop(calc->tab);

	return 0;
}


int (* op[])(struct calc * calc) = {
 	[ACTION(A_OP_VAR_DECL)] = op_var_decl,
 	[ACTION(A_OP_ASSIGN)] = op_assign,
 	[ACTION(A_OP_EQU)] = op_equ,
 	[ACTION(A_OP_NEQ)] = op_neq,
 	[ACTION(A_OP_LT)] = op_lt,
 	[ACTION(A_OP_GT)] = op_gt,
 	[ACTION(A_OP_GTE)] = op_gte,
 	[ACTION(A_OP_LTE)] = op_lte,
 	[ACTION(A_OP_SHL)] = op_shl,
 	[ACTION(A_OP_ASR)] = op_asr,
	[ACTION(A_OP_ADD)] = op_add,
	[ACTION(A_OP_SUB)] = op_sub,
	[ACTION(A_OP_XOR)] = op_xor,
	[ACTION(A_OP_OR)] = op_or,
	[ACTION(A_OP_MUL)] = op_mul,
	[ACTION(A_OP_DIV)] = op_div,
	[ACTION(A_OP_MOD)] = op_mod,
	[ACTION(A_OP_AND)] = op_and,
	[ACTION(A_OP_INV)] = op_inv,
	[ACTION(A_OP_NOT)] = op_not,
	[ACTION(A_OP_LOGIC_AND)] = op_logic_and,
	[ACTION(A_OP_LOGIC_OR)] = op_logic_or,
 	[ACTION(A_OP_MINUS)] = op_minus,
 	[ACTION(A_OP_PUSH_INT)] = op_push_int,
 	[ACTION(A_OP_PUSH_TRUE)] = op_push_true,
 	[ACTION(A_OP_PUSH_FALSE)] = op_push_false,
 	[ACTION(A_OP_WHILE_BEGIN)] = op_while_begin,
 	[ACTION(A_OP_WHILE_COND)] = op_while_cond,
 	[ACTION(A_OP_WHILE_END)] = op_while_end,
 	[ACTION(A_OP_IF_COND)] = op_if_cond,
 	[ACTION(A_OP_IF_ELSE)] = op_if_else,
 	[ACTION(A_OP_IF_END)] = op_if_end,
 	[ACTION(A_OP_FOR_INIT)] = op_for_init,
 	[ACTION(A_OP_FOR_COND)] = op_for_cond,
 	[ACTION(A_OP_FOR_AFTER)] = op_for_after,
 	[ACTION(A_OP_FOR_END)] = op_for_end,
 	[ACTION(A_OP_METH_OR_ATTR)] = op_meth_or_attr,
 	[ACTION(A_OP_PUSH_SYM)] = op_push_sym,
 	[ACTION(A_OP_POP_SYM)] = op_pop_sym,
 	[ACTION(A_OP_METHOD)] = op_method,
 	[ACTION(A_OP_ARG)] = op_arg,
 	[ACTION(A_OP_RET_DISCARD)] = op_ret_discard,
 	[ACTION(A_OP_PUSH_STRING)] = op_push_string,

};

/* Syntax-directed translator */
/* Nonrecursive predictive parser */

#define STACK_SIZE 512

int calc_compile(struct calc * calc, uint8_t code[],
				 const char * txt, unsigned int len)
{
	uint8_t parser_stack[STACK_SIZE];
	struct lexer lex;
	struct token tok;
	int lookahead;
	uint8_t * pp_sp;
	uint8_t * pp_sl;
	uint8_t * pp_top;
	int sym;
	int k;
	int err = ERR_SYNTAX_ERROR;

	lexer_open(&lex, txt, len);
	pp_sp = parser_stack + STACK_SIZE;
	pp_sl = parser_stack;
	pp_top = pp_sp;
	pp_sp -= ll_start(pp_sp);
	calc->pc = 0;
	calc->code = code;

	/* */
	lookahead = (tok = lexer_scan(&lex)).typ;
	while (pp_sp != pp_top) {
		/* pop the stack */
		sym = *pp_sp++;
//		printf("<%s>\n", ll_sym_tab[sym]);
		if IS_A_TERMINAL(sym) {
			/* terminal */
			if (sym != lookahead) {
				fprintf(stderr, "unexpected symbol!\n");
				goto error;
			}
			/* save the lookahead token */
			calc->tok = tok;
			/* get next token */
			lookahead = (tok = lexer_scan(&lex)).typ;
//			printf("[%s]\n", ll_sym_tab[lookahead]);
		} else if IS_AN_ACTION(sym) {
			/* action */
			int ret;
			if ((ret = op[ACTION(sym)](calc)) < 0) {
//				fprintf(stderr, "action(%s) failed!\n", ll_sym_tab[sym]);
				goto error;
			}
		} else {
			/* non terminal */
			if ((k = ll_rule_push(pp_sp, sym, lookahead)) < 0) {
				fprintf(stderr, "ll_rule_push() failed!\n");
				ll_stack_dump(stderr, pp_sp, pp_top - pp_sp);
				fprintf(stderr, "lookahead = %s\n", ll_sym_tab[lookahead]);
				goto error;
			}
			pp_sp -= k;
			if (pp_sp < pp_sl) {
				fprintf(stderr, "stack overflow!\n");
				/* stack overflow */
				goto error;
			}
		}
	}

	if (mem_bind(calc) < 0)
		return -1;

	printf("\nSYMBOL TABLE:\n");
	sym_dump(stdout, calc->tab);
	printf("\n");
	fflush(stdout);

	return calc->pc;

error:
	printf("\n");
	fflush(stdout);
	fflush(stderr);
	lexer_print_err(stderr, &lex, err);
	return -1;
}

int calc_init(struct calc * calc, struct sym_tab * tab, 
			  int32_t mem[], unsigned int size)
{
	calc->tab = tab;
	calc->mem = mem;
	calc->heap = 0;
	calc->stack = size;
	calc->sp = size;

	return 0;
}

int load_script(const char * pathname, char ** cpp, unsigned int * lenp)
{
	char * cp;
	FILE * f;
	int ret;
	int len;

	if ((f = fopen(pathname, "r")) == NULL) {
		fprintf(stderr, "ERROR: %s: open(): %s.\n",
				__func__, strerror(errno));
		return -1;
	}

	fseek(f, 0, SEEK_END);
	len = ftell(f);

	if ((cp = realloc(*cpp, *lenp + len)) == NULL) {
		fprintf(stderr, "ERROR: %s: frealloc(): %s.\n",
				__func__, strerror(errno));
		fclose(f);
		return -1;
	}
	*cpp = cp;

	cp += *lenp;

	fseek(f, 0, SEEK_SET);

	if ((ret = fread(cp, len, 1, f)) != 1) {
		fprintf(stderr, "ERROR: %s: fread(): %s.\n",
				__func__, strerror(errno));
		fclose(f);
		return ret;
	}

	fclose(f);

	*lenp += len;

	return len;
}

int main(int argc, char *argv[])
{
	int32_t data[128];
	uint8_t code[512];
	struct calc calc; 
	struct calc_vm vm; 
	struct sym_tab sym_tab;

	sym_tab_init(&sym_tab);
	calc_init(&calc, &sym_tab, data, sizeof(data));
	calc_vm_init(&vm, data, sizeof(data));

	if (argc > 1) {
		char * script = NULL;
		unsigned int len = 0;
		int i = 1;
		int n;

		if (strcmp(argv[i], "-t") == 0) {
			vm.trace = true;
			++i;
		}

		for (; i < argc; ++i) {
			if (load_script(argv[i], &script, &len) < 0)
				return 1;
		}

		if ((n = calc_compile(&calc, code, script, len)) < 0)
			return 1;

		if (calc_exec(&vm, code, n) < 0)
			return 1;
	}

	return 0;
}

