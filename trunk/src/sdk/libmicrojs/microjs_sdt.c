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
 * @file microjs-i.h
 * @brief Syntax-directed translation compiler
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#define __MICROJS_I__
#include "microjs-i.h"

#define __DEF_CONST_STR__
#include "const_str.h"

#define CONST_NM (256 - CONST_STRINGS_MAX)

/* --------------------------------------------------------------------------
   External symbols
   -------------------------------------------------------------------------- */

const struct ext_entry externals[] = {
	[EXT_RAND] = { .nm = CONST_NM + NM_RAND, .argmin = 0, .argmax = 0 },
	[EXT_SQRT] = { .nm = CONST_NM + NM_SQRT, .argmin = 1, .argmax = 1 },
	[EXT_LOG2] = { .nm = CONST_NM + NM_LOG2, .argmin = 1, .argmax = 1 },
	[EXT_WRITE] = { .nm = CONST_NM + NM_WRITE, .argmin = 0, .argmax = 128 },
	[EXT_PRINT] = { .nm = CONST_NM + NM_PRINT, .argmin = 0, .argmax = 128 },
	[EXT_PRINTF] = { .nm = CONST_NM + NM_PRINTF, .argmin = 1, .argmax = 128 },
	[EXT_SRAND] = { .nm = CONST_NM + NM_SRAND, .argmin = 1, .argmax = 1 },
	[EXT_TIME] = { .nm = CONST_NM + NM_TIME, .argmin = 0, .argmax = 0 },
};

int extern_lookup(int nm)
{
	int i;

	for (i = 0; i < sizeof(externals) / sizeof(struct ext_entry); ++i) {
		if (externals[i].nm == nm)
			return i;
	}

	return -1;
}

struct ext_entry * extern_get(unsigned int exid)
{
	if (exid >= sizeof(externals) / sizeof(struct ext_entry))
		return NULL;
	return(struct ext_entry *)&externals[exid];
}

/* --------------------------------------------------------------------------
   Memory operations
   -------------------------------------------------------------------------- */

int alloc32(struct microjs_compiler * microjs)
{
	unsigned int addr;

	/* ensure memory alignment */
	addr = (microjs->heap + 3) & ~3;
	microjs->heap = addr + 4;

	if (microjs->heap > microjs->sp) {
		fprintf(stderr, "heap overflow!\n");
		return -1;
	}

	return addr;
}


int32_t mem_rd32(struct microjs_compiler * microjs, unsigned int addr)
{
	return microjs->mem[addr >> 2];
}

void mem_wr32(struct microjs_compiler * microjs, unsigned int addr, int32_t val)
{
	microjs->mem[addr >> 2] = val;
}

int push(struct microjs_compiler * microjs, int32_t x)
{
	microjs->sp -= 4;
	microjs->mem[microjs->sp >> 2] = x;	

	if (microjs->sp < microjs->heap) {
		fprintf(stderr, "stack overflow!\n");
		return -1;
	}
	return 0;
}

int32_t pop(struct microjs_compiler * microjs)
{
	int32_t x;

	if (microjs->sp == microjs->stack) {
		return 0;
	}

	x = microjs->mem[microjs->sp >> 2];
	microjs->sp += 4;

	return x;
}

int mem_bind(struct microjs_compiler * microjs)
{
	struct symtab * tab = microjs->tab;
	int i;

	/* search in the global list */
	for (i = 0; i < tab->global; ++i) {
		struct sym * sp = &tab->sym[i];
		if ((sp->flags & SYM_OBJECT) && !(sp->flags & SYM_OBJ_ALLOC)) {
			struct sym_obj * obj = (struct sym_obj *)sp;
			int addr;
			if ((addr = alloc32(microjs)) < 0)
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
			microjs->code[ref->addr] = obj->addr;
			microjs->code[ref->addr + 1] = obj->addr >> 8;
		}
	}

	return 0;
}




/* --------------------------------------------------------------------------
   Code generation
   -------------------------------------------------------------------------- */

int op_var_decl(struct microjs_compiler * microjs)
{
	struct sym_obj * obj;
	int addr;

	if ((obj = sym_obj_new(microjs->tab, microjs->tok.s, microjs->tok.qlf)) == NULL) {
		fprintf(stderr, "can't create symbol.\n");
		return -1;
	}

	if ((addr = alloc32(microjs)) < 0)
		return -1;

	obj->addr = addr;
	/* initial variables are int */
	obj->size = 4;
	/* flag as allocated */
	obj->flags |= SYM_OBJ_ALLOC;
	return 0;
}

int op_push_sym(struct microjs_compiler * microjs)
{
	struct sym_tmp * tmp;
		
	if ((tmp = sym_tmp_push(microjs->tab, 
							microjs->tok.s, microjs->tok.qlf)) == NULL) {
		fprintf(stderr, "cant create temp symbol: %s.\n", 
				tok2str(microjs->tok));
		return -1;
	}

	DBG("tmp=\"%s\"", sym_name(microjs->tab, tmp->nm));
	return 0;
}

int op_meth_or_attr(struct microjs_compiler * microjs)
{
	struct sym_tmp * tmp = sym_tmp_get(microjs->tab, 0);

	if (SYM_IS_METHOD(tmp)) {
		DBG("method=\"%s\" (%d)", sym_name(microjs->tab, tmp->nm), tmp->xid);
		DBG("min=%d max=%d argc=%d", tmp->min, tmp->max, tmp->cnt);

		if (tmp->cnt < tmp->min) {
			fprintf(stderr, "argument missing: %s\n", 
					sym_name(microjs->tab, tmp->nm));
			return -1;
		}
		if (tmp->cnt > tmp->max) {
			fprintf(stderr, "too many arguments to function: %s\n", 
					sym_name(microjs->tab, tmp->nm));
			return -1;
		}

		/* number of arguments */
		printf("%04x\tI18 %d\n", microjs->pc, tmp->xid);
		microjs->code[microjs->pc++] = OPC_I8;
		microjs->code[microjs->pc++] = tmp->cnt;
		/* external call number */
		printf("%04x\tI18 %d\n", microjs->pc, tmp->xid);
		microjs->code[microjs->pc++] = OPC_I8;
		microjs->code[microjs->pc++] = tmp->xid;
		printf("%04x\tEXT \'%s\"\n", microjs->pc, 
			   sym_name(microjs->tab, tmp->nm));
		microjs->code[microjs->pc++] = OPC_EXT;
	} else {
		struct sym_obj * obj;
		DBG("attribute=\"%s\"", sym_name(microjs->tab, tmp->nm));
		if ((obj = sym_obj_lookup(microjs->tab, tmp->nm)) == NULL) {
			fprintf(stderr, "undefined symbol: %s.\n", 
					sym_name(microjs->tab, tmp->nm));
			return -1;
		}
#if 0
		struct sym_ref * ref;
		ref = sym_ref_new(microjs->tab, obj);
		printf("ref=0x%04x oid=%d --> obj=0x%04x\n", ref->addr, 
			   ref->oid, obj->addr);
		ref->addr = microjs->pc + 1;
#endif
		printf("%04x\tI16 \'%s\"\n", microjs->pc, 
			   sym_name(microjs->tab, tmp->nm));
		microjs->code[microjs->pc++] = OPC_I16;
		microjs->code[microjs->pc++] = obj->addr;
		microjs->code[microjs->pc++] = obj->addr >> 8;
		printf("%04x\tLD\n", microjs->pc);
		microjs->code[microjs->pc++] = OPC_LD;
	}

	return 0;
}

int op_pop_sym(struct microjs_compiler * microjs)
{
	sym_pop(microjs->tab);
	return 0;
}

int op_method(struct microjs_compiler * microjs)
{
	struct ext_entry * ext;
	struct sym_tmp * tmp;
	int xid;
	tmp = sym_tmp_get(microjs->tab, 0);
	tmp->flags |= SYM_METHOD;

	xid = extern_lookup(tmp->nm);
	if ((ext = extern_get(xid)) == NULL) {
		fprintf(stderr, "undefined external: %s.\n", 
				sym_name(microjs->tab, tmp->nm));
		return -1;
	}
	DBG("method=\"%s\" (%d)", sym_name(microjs->tab, tmp->nm), xid);
	tmp->min = ext->argmin;
	tmp->max = ext->argmax;
	tmp->cnt = 0;
	tmp->xid = xid;

	/* prepare to call a function or method */
	return 0;
}

int op_arg(struct microjs_compiler * microjs)
{
	struct sym_tmp * tmp;
	tmp = sym_tmp_get(microjs->tab, 0);
	tmp->cnt++; /* increment the argument counter */
	DBG("argc=%d", tmp->cnt);
	return 0;
}

int op_ret_discard(struct microjs_compiler * microjs)
{
	printf("%04x\tPOP\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_POP;
	return 0;
}

#if 0
int op_assign(struct microjs_compiler * microjs)
{
	struct sym_tmp * tmp = sym_tmp_get(microjs->tab, 0);
	struct sym_obj * obj;
	struct sym_ref * ref;

	DBG("assign=\"%s\"", sym_name(microjs->tab, tmp->nm));
	if ((obj = sym_obj_lookup(microjs->tab, tmp->nm)) == NULL) {
		fprintf(stderr, "undefined symbol: %s.\n", 
				sym_name(microjs->tab, tmp->nm));
		return -1;
	}
	ref = sym_ref_new(microjs->tab, obj);
	printf("ref=0x%04x oid=%d --> obj=0x%04x\n", ref->addr, 
		   ref->oid, obj->addr);
	printf("%04x\tI16 \'%s\"\n", microjs->pc, sym_name(microjs->tab, tmp->nm));
	microjs->code[microjs->pc++] = OPC_I16;
	ref->addr = microjs->pc;
	microjs->code[microjs->pc++] = 0;
	microjs->code[microjs->pc++] = 0;
	printf("%04x\tST\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_ST;
	return 0;
}
#endif

int op_assign(struct microjs_compiler * microjs)
{
	struct sym_tmp * tmp = sym_tmp_get(microjs->tab, 0);
	struct sym_obj * obj;

	DBG("assign=\"%s\"", sym_name(microjs->tab, tmp->nm));
	if ((obj = sym_obj_lookup(microjs->tab, tmp->nm)) == NULL) {
		fprintf(stderr, "undefined symbol: %s.\n", 
				sym_name(microjs->tab, tmp->nm));
		return -1;
	}
	printf("%04x\tI16 \'%s\"\n", microjs->pc, sym_name(microjs->tab, tmp->nm));
	microjs->code[microjs->pc++] = OPC_I16;
	microjs->code[microjs->pc++] = obj->addr;
	microjs->code[microjs->pc++] = obj->addr >> 8;
	printf("%04x\tST\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_ST;
	return 0;
}

int op_equ(struct microjs_compiler * microjs)
{
	printf("%04x\tEQ\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_EQ;
	return 0;
}

int op_neq(struct microjs_compiler * microjs)
{
	printf("%04x\tNE\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_NE;
	return 0;
}

int op_gt(struct microjs_compiler * microjs)
{
	printf("%04x\tGT\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_GT;
	return 0;
}

int op_lt(struct microjs_compiler * microjs)
{
	printf("%04x\tLT\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_LT;
	return 0;
}

int op_gte(struct microjs_compiler * microjs)
{
	printf("%04x\tGE\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_GE;
	return 0;
}

int op_lte(struct microjs_compiler * microjs)
{
	printf("%04x\tLE\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_LE;
	return 0;
}

int op_logic_or(struct microjs_compiler * microjs)
{
	printf("%04x\tOR\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_OR;
	return 0;
}

int op_logic_and(struct microjs_compiler * microjs)
{
	printf("%04x\tAND\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_AND;
	return 0;
}

int op_add(struct microjs_compiler * microjs)
{
	printf("%04x\tADD\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_ADD;
	return 0;
}

int op_sub(struct microjs_compiler * microjs)
{
	printf("%04x\tSUB\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_SUB;
	return 0;
}

int op_or(struct microjs_compiler * microjs)
{
	printf("%04x\tOR\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_OR;
	return 0;
}

int op_xor(struct microjs_compiler * microjs)
{
	microjs->code[microjs->pc++] = OPC_XOR;
	return 0;
}

int op_mul(struct microjs_compiler * microjs)
{
	printf("%04x\tMUL\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_MUL;
	return 0;
}

int op_div(struct microjs_compiler * microjs)
{
	printf("%04x\tDIV\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_DIV;
	return 0;
}

int op_mod(struct microjs_compiler * microjs)
{
	microjs->code[microjs->pc++] = OPC_MOD;
	return 0;
}

int op_and(struct microjs_compiler * microjs)
{
	microjs->code[microjs->pc++] = OPC_AND;
	return 0;
}

int op_inv(struct microjs_compiler * microjs)
{
	microjs->code[microjs->pc++] = OPC_INV;
	return 0;
}

int op_minus(struct microjs_compiler * microjs)
{
	microjs->code[microjs->pc++] = OPC_NEG;
	return 0;
}

int op_not(struct microjs_compiler * microjs)
{
	microjs->code[microjs->pc++] = OPC_NEG;
	return 0;
}

int op_shl(struct microjs_compiler * microjs)
{
	microjs->code[microjs->pc++] = OPC_SHL;
	return 0;
}

int op_asr(struct microjs_compiler * microjs)
{
	microjs->code[microjs->pc++] = OPC_ASR;
	return 0;
}

int op_push_false(struct microjs_compiler * microjs)
{
	printf("%04x\tI8 %d\n", microjs->pc, 0);
	microjs->code[microjs->pc++] = OPC_I8;
	microjs->code[microjs->pc++] = 0;
	return 0;
}

int op_push_true(struct microjs_compiler * microjs)
{
	printf("%04x\tI8 %d\n", microjs->pc, 1);
	microjs->code[microjs->pc++] = OPC_I8;
	microjs->code[microjs->pc++] = 1;
	return 0;
}

int op_push_string(struct microjs_compiler * microjs) 
{
	int isz;

	if ((isz = cstr_add(microjs->tok.s, microjs->tok.qlf)) < 0) {
		fprintf(stderr, "can't create string!\n");
		return -1;
	}
	
	printf("%04x\tI8 %d\n", microjs->pc, isz);
	microjs->code[microjs->pc++] = OPC_I8;
	microjs->code[microjs->pc++] = isz;
	return 0;
}

int op_push_int(struct microjs_compiler * microjs)
{
	int32_t x = microjs->tok.u32;

	printf("%04x\t", microjs->pc);

	if (x >= 0) {
		if (x < 32768) {
			if (x < 128) {
				printf("I8 %d\n", x);
				microjs->code[microjs->pc++] = OPC_I8;
				microjs->code[microjs->pc++] = x;
			} else {
				printf("I16 %d\n", x);
				microjs->code[microjs->pc++] = OPC_I16;
				microjs->code[microjs->pc++] = x;
				microjs->code[microjs->pc++] = x >> 8;
			}
		} else {
			printf("I32 %d\n", x);
			microjs->code[microjs->pc++] = OPC_I32;
			microjs->code[microjs->pc++] = x;
			microjs->code[microjs->pc++] = x >> 8;
			microjs->code[microjs->pc++] = x >> 16;
			microjs->code[microjs->pc++] = x >> 24;
		}
	} else {
		if (x >= -32768) {
			if (x >= -128) {
				printf("I8 %d\n", x);
				microjs->code[microjs->pc++] = OPC_I8;
				microjs->code[microjs->pc++] = x;
			} else {
				printf("I16 %d\n", x);
				microjs->code[microjs->pc++] = OPC_I16;
				microjs->code[microjs->pc++] = x;
				microjs->code[microjs->pc++] = x >> 8;
			}
		} else {
			printf("I32 %d\n", x);
			microjs->code[microjs->pc++] = OPC_I32;
			microjs->code[microjs->pc++] = x;
			microjs->code[microjs->pc++] = x >> 8;
			microjs->code[microjs->pc++] = x >> 16;
			microjs->code[microjs->pc++] = x >> 24;
		}
	}

	return 0;
}

int op_while_begin(struct microjs_compiler * microjs)
{
	int id;

	if ((id = sym_anom_push(microjs->tab)) < 0)
		return id;

	printf(".L%d:\n", id);
	/* save current location on a temporary variable */
	sym_addr_set(microjs->tab, id, microjs->pc);
	return 0;
}

int op_while_cond(struct microjs_compiler * microjs)
{
	int id;

	if ((id = sym_anom_push(microjs->tab)) < 0)
		return id;

	printf(".L%d:\n%04x\tJEQ xxxx\n", id, microjs->pc);

	/* save current location on a temporary variable */
	microjs->code[microjs->pc++] = OPC_JEQ;
	sym_addr_set(microjs->tab, id, microjs->pc);
	/* reserve 2 positions for jump address */
	microjs->pc += 2;
	return 0;
}


int op_while_end(struct microjs_compiler * microjs)
{
	int id;
	int addr;
	int offs;

	/* Adjust the conditinal jump */
	id = sym_anom_pop(microjs->tab);
	addr = sym_addr_get(microjs->tab, id);
	offs = microjs->pc - (addr - 1);
	printf("\tfix %04x -> JEQ %04x (.L%d)\n", addr - 1, microjs->pc + 3, id);
	microjs->code[addr++] = offs;
	microjs->code[addr++] = offs >> 8;

	/* Repeat jump */
	id = sym_anom_pop(microjs->tab);
	addr = sym_addr_get(microjs->tab, id);
	offs = addr - (microjs->pc + 3);
	printf("%04x\tJMP %04x (.L%d offs=%d)\n", microjs->pc, addr, id, offs);
	microjs->code[microjs->pc++] = OPC_JMP;
	microjs->code[microjs->pc++] = offs;
	microjs->code[microjs->pc++] = offs >> 8;

	return 0;
}


int op_if_cond(struct microjs_compiler * microjs)
{
	int id;

	if ((id = sym_anom_push(microjs->tab)) < 0)
		return id;

	printf(".L%d:\n%04x\tJEQ xxxx\n", id, microjs->pc);
	/* save current location on a temporary variable */
	microjs->code[microjs->pc++] = OPC_JEQ;
	sym_addr_set(microjs->tab, id, microjs->pc);
	/* reserve 2 positions for jump address */
	microjs->pc += 2;

	return 0;
}

int op_if_else(struct microjs_compiler * microjs)
{
	int id;
	int addr;
	int offs;

	id = sym_anom_get(microjs->tab, 0);
	addr = sym_addr_get(microjs->tab, id);

	offs = microjs->pc - (addr - 1);
	printf("\tfix %04x -> JEQ %04x (.L%d)\n", addr - 1, microjs->pc + 3, id);
	microjs->code[addr++] = offs;
	microjs->code[addr++] = offs >> 8;

	printf(".L%d:\n%04x\tJMP xxxx\n", id, microjs->pc);
	/* save current location on the same temporary variable */
	microjs->code[microjs->pc++] = OPC_JMP;
	sym_addr_set(microjs->tab, id, microjs->pc);
	/* reserve 2 positions for jump address */
	microjs->pc += 2;

	return 0;
}

int op_if_end(struct microjs_compiler * microjs)
{
	int id;
	int addr;
	int offs;

	/* Adjust the conditinal jump */
	id = sym_anom_pop(microjs->tab);
	addr = sym_addr_get(microjs->tab, id);

	offs = (microjs->pc - 3) - (addr - 1);
	printf("\tfix %04x -> Jxx %04x (.L%d)\n", addr - 1, microjs->pc, id);
	microjs->code[addr++] = offs;
	microjs->code[addr++] = offs >> 8;

	return 0;
}

int op_for_init(struct microjs_compiler * microjs)
{
	int id;

	/* allocate 4 anonymous symbols */
	if ((id = sym_anom_push(microjs->tab)) < 0)
		return id;

	printf(".L%d:\n", id);
	/* save current location */
	sym_addr_set(microjs->tab, id, microjs->pc);

	if ((id = sym_anom_push(microjs->tab)) < 0)
		return id;
	if ((id = sym_anom_push(microjs->tab)) < 0)
		return id;
	if ((id = sym_anom_push(microjs->tab)) < 0)
		return id;

	return 0;
}

int op_for_cond(struct microjs_compiler * microjs)
{
	int id;

	id = sym_anom_get(microjs->tab, 2);
	/* Conditional jump to the body */
	printf(".L%d:\n%04x\tJEQ xxxx\n", id, microjs->pc);
	/* save current location on a temporary variable */
	microjs->code[microjs->pc++] = OPC_JEQ;
	sym_addr_set(microjs->tab, id, microjs->pc);
	/* reserve 2 positions for jump address */
	microjs->pc += 2;

	id = sym_anom_get(microjs->tab, 1);
	/* Jump to the beginning of the body part */
	printf(".L%d:\n%04x\tJMP xxxx\n", id, microjs->pc);
	/* save current location on a temporary variable */
	microjs->code[microjs->pc++] = OPC_JMP;
	sym_addr_set(microjs->tab, id, microjs->pc);
	/* reserve 2 positions for jump address */
	microjs->pc += 2;

	id = sym_anom_get(microjs->tab, 0);
	printf(".L%d:\n", id);
	/* save current location */
	sym_addr_set(microjs->tab, id, microjs->pc);

	return 0;
}

int op_for_after(struct microjs_compiler * microjs)
{
	int id;
	int addr;
	int offs;

	/* Adjust the jump to body */
	id = sym_anom_get(microjs->tab, 1);
	addr = sym_addr_get(microjs->tab, id);
	offs = microjs->pc - (addr - 1);
	printf("\tfix %04x -> JMP %04x (.L%d)\n", addr - 1, microjs->pc + 3, id);
	microjs->code[addr++] = offs;
	microjs->code[addr++] = offs >> 8;

	/* Repeat jump */
	id = sym_anom_get(microjs->tab, 3);
	addr = sym_addr_get(microjs->tab, id);
	offs = addr - (microjs->pc + 3);
	printf("%04x\tJMP %04x (.L%d offs=%d)\n", microjs->pc, addr, id, offs);
	microjs->code[microjs->pc++] = OPC_JMP;
	microjs->code[microjs->pc++] = offs;
	microjs->code[microjs->pc++] = offs >> 8;

	return 0;
}

int op_for_end(struct microjs_compiler * microjs)
{
	int id;
	int addr;
	int offs;

	/* Adjust the conditinal jump */
	id = sym_anom_get(microjs->tab, 2);
	addr = sym_addr_get(microjs->tab, id);

	offs = microjs->pc - (addr - 1);
	printf("\tfix %04x -> JEQ %04x (.L%d)\n", addr - 1, microjs->pc + 3, id);
	microjs->code[addr++] = offs;
	microjs->code[addr++] = offs >> 8;

	/* jump to the afterthought part */
	id = sym_anom_get(microjs->tab, 0);
	addr = sym_addr_get(microjs->tab, id);
	offs = addr - (microjs->pc + 3);
	printf("%04x\tJMP %04x (.L%d offs=%d)\n", microjs->pc, addr, id, offs);
	microjs->code[microjs->pc++] = OPC_JMP;
	microjs->code[microjs->pc++] = offs;
	microjs->code[microjs->pc++] = offs >> 8;

	/* remove temp symbols */
	sym_anom_pop(microjs->tab);
	sym_anom_pop(microjs->tab);
	sym_anom_pop(microjs->tab);
	sym_anom_pop(microjs->tab);

	return 0;
}


int (* op[])(struct microjs_compiler * microjs) = {
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

int microjs_compile(struct microjs_compiler * microjs, uint8_t code[],
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
	microjs->pc = 0;
	microjs->code = code;

	/* */
	lookahead = (tok = lexer_scan(&lex)).typ;
	while (pp_sp != pp_top) {
		/* pop the stack */
		sym = *pp_sp++;
//		printf("<%s>\n", ll_symtab[sym]);
		if IS_A_TERMINAL(sym) {
			/* terminal */
			if (sym != lookahead) {
				fprintf(stderr, "unexpected symbol!\n");
				goto error;
			}
			/* save the lookahead token */
			microjs->tok = tok;
			/* get next token */
			lookahead = (tok = lexer_scan(&lex)).typ;
//			printf("[%s]\n", ll_symtab[lookahead]);
		} else if IS_AN_ACTION(sym) {
			/* action */
			int ret;
			if ((ret = op[ACTION(sym)](microjs)) < 0) {
//				fprintf(stderr, "action(%s) failed!\n", ll_symtab[sym]);
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

	if (mem_bind(microjs) < 0)
		return -1;

	printf("\nSYMBOL TABLE:\n");
	sym_dump(stdout, microjs->tab);
	printf("\n");
	fflush(stdout);

	return microjs->pc;

error:
	printf("\n");
	fflush(stdout);
	fflush(stderr);
	lexer_print_err(stderr, &lex, err);
	return -1;
}

int microjs_compiler_init(struct microjs_compiler * microjs, 
						  struct symtab * tab, 
						  int32_t mem[], unsigned int size)
{
	microjs->tab = tab;
	microjs->mem = mem;
	microjs->heap = 0;
	microjs->stack = size;
	microjs->sp = size;

	return 0;
}
