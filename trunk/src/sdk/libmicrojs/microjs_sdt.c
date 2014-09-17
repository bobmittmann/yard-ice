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

#include <sys/dcclog.h>

/* --------------------------------------------------------------------------
   Target memory operations
   -------------------------------------------------------------------------- */

static int tgt_alloc32(struct microjs_sdt * microjs)
{
	unsigned int addr;

	/* ensure memory alignment */
	addr = (microjs->tgt_heap + 3) & ~3;
	microjs->tgt_heap = addr + 4;

	if (microjs->tgt_heap > microjs->tgt_sp)
		return -ERR_HEAP_OVERFLOW;

	return addr;
}

/* --------------------------------------------------------------------------
   Code generation
   -------------------------------------------------------------------------- */

/* Variable declaration.
   Allocate a new integer in the target heap. */

int op_var_decl(struct microjs_sdt * microjs)
{
	struct sym_obj * obj;
	int addr;

	DCC_LOG(LOG_MSG, " >>> ...");

	if ((obj = sym_obj_scope_lookup(microjs->tab, microjs->tok.s, 
						   microjs->tok.qlf)) != NULL) {
		DCC_LOG(LOG_TRACE, "object exist in current scope!");
		return 0;
	}

	if ((obj = sym_obj_new(microjs->tab, microjs->tok.s, 
						   microjs->tok.qlf)) == NULL) {
		DCC_LOG(LOG_WARNING, "sym_obj_new() failed!");
		return -ERR_OBJ_NEW_FAIL;
	}

	if ((addr = tgt_alloc32(microjs)) < 0) {
		DCC_LOG(LOG_WARNING, "tgt_alloc32() failed!");
		return addr;
	}

	DCC_LOG1(LOG_MSG, "addr=0x%04x", addr);

	obj->addr = addr;
	/* initial variables are int */
	obj->size = 4;
	/* flag as allocated */
	obj->flags |= SYM_OBJ_ALLOC;
	return 0;
}

int op_push_tmp(struct microjs_sdt * microjs)
{
	struct sym_tmp tmp;

	tmp.flags = 0;
	tmp.s = microjs->tok.s;
	tmp.len = microjs->tok.qlf;

	DCC_LOG1(LOG_INFO, " >>> tmp.len=%d", tmp.len);

	if (!sym_tmp_push(microjs->tab, &tmp))
		return -ERR_SYM_PUSH_FAIL;

	return 0;
}

int op_pop_tmp(struct microjs_sdt * microjs)
{
	DCC_LOG(LOG_INFO, " >>> ...");

	sym_tmp_pop(microjs->tab, NULL);
	return 0;
}

int op_assign(struct microjs_sdt * microjs)
{
	struct sym_tmp tmp;
	struct sym_obj * obj;

	DCC_LOG(LOG_INFO, " >>> ...");

	if (!sym_tmp_pop(microjs->tab, &tmp))
		return -ERR_SYM_POP_FAIL;

	DCC_LOG1(LOG_INFO, "tmp.len=%d", tmp.len);

	if ((obj = sym_obj_lookup(microjs->tab, tmp.s, tmp.len)) == NULL)
		return -ERR_VAR_UNKNOWN;

	TRACEF("%04x\tST \"%s\" (%04x)\n", microjs->pc, 
		   sym_obj_name(microjs->tab, obj), obj->addr);
	microjs->code[microjs->pc++] = OPC_ST;
	microjs->code[microjs->pc++] = obj->addr >> 2;

	/* push back */
	sym_tmp_push(microjs->tab, &tmp);
	
	return 0;
}

int op_method(struct microjs_sdt * microjs)
{
	struct ext_fndef * fndef;
	struct sym_tmp tmp;
	int xid;

	if (!sym_tmp_pop(microjs->tab, &tmp)) {
		DCC_LOG(LOG_WARNING, "sym_tmp_po() failed!");
		return -ERR_SYM_POP_FAIL;
	}

	if ((xid = sym_extern_lookup(microjs->tab, tmp.s, tmp.len)) < 0) {
		DCC_LOG(LOG_WARNING, "sym_extern_lookup() failed!");
		return -ERR_EXTERN_UNKNOWN;
	}	

	fndef = sym_extern_get(microjs->tab, xid);

	DCC_LOG1(LOG_INFO, "external \"%s\"", fndef->nm);

	tmp.flags = SYM_METHOD | SYM_EXTERN;
	tmp.min = fndef->argmin;
	tmp.max = fndef->argmax;
	tmp.cnt = 0;
	tmp.xid = xid;

	/* prepare to call a function or method */
	/* push back; */
	sym_tmp_push(microjs->tab, &tmp);
	return 0;
}

int op_meth_or_attr(struct microjs_sdt * microjs)
{
	struct sym_tmp tmp;

	DCC_LOG(LOG_INFO, " >>>>>>>>> ...");

	if (!sym_tmp_pop(microjs->tab, &tmp))
		return -ERR_SYM_POP_FAIL;

	if (SYM_IS_EXTERN(tmp)) {
	
		DCC_LOG(LOG_INFO, " EXTERN ");

		if (tmp.cnt < tmp.min)
			return -ERR_ARG_MISSING;
		if (tmp.cnt > tmp.max)
			return -ERR_TOO_MANY_ARGS;

		TRACEF("%04x\tEXT \'%s\"\n", microjs->pc, 
			   sym_extern_name(microjs->tab, tmp.xid));
		microjs->code[microjs->pc++] = OPC_EXT;
		/* external call number */
		microjs->code[microjs->pc++] = tmp.xid;
		/* number of arguments */
		microjs->code[microjs->pc++] = tmp.cnt;
	} else {
		struct sym_obj * obj;

		DCC_LOG(LOG_INFO, " VAR");

		if ((obj = sym_obj_lookup(microjs->tab, tmp.s, tmp.len)) == NULL)
			return -ERR_VAR_UNKNOWN;

		TRACEF("%04x\tLD \'%s\" (%04x)\n", microjs->pc, 
			   sym_obj_name(microjs->tab, obj), obj->addr);
		microjs->code[microjs->pc++] = OPC_LD;
		microjs->code[microjs->pc++] = obj->addr >> 2;
	}

	/* push back; */
	sym_tmp_push(microjs->tab, &tmp);

	return 0;
}

int op_arg(struct microjs_sdt * microjs)
{
	struct sym_tmp tmp;

	DCC_LOG(LOG_INFO, " >>> ...");

	if (!sym_tmp_pop(microjs->tab, &tmp))
		return -ERR_SYM_POP_FAIL;

	tmp.cnt++; /* increment the argument counter */

	DCC_LOG1(LOG_INFO, "argc=%d", tmp.cnt);

	/* push back; */
	sym_tmp_push(microjs->tab, &tmp);

	return 0;
}

int op_ret_discard(struct microjs_sdt * microjs)
{
	TRACEF("%04x\tPOP\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_POP;
	return 0;
}

int op_blk_open(struct microjs_sdt * microjs) 
{
	DCC_LOG(LOG_INFO, "{");

	/* save the heap state */
	sym_addr_push(microjs->tab, &microjs->tgt_heap);
	/* save the stack frame */
	if (!sym_sf_push(microjs->tab))
		return -1;

	return 0;
}

int op_blk_close(struct microjs_sdt * microjs) 
{
	DCC_LOG(LOG_INFO, "}");

	/* restore the stack frame */
	if (!sym_sf_pop(microjs->tab))
		return -1;
	/* restore the heap state */
	if (!sym_addr_pop(microjs->tab, &microjs->tgt_heap))
		return -1;

	return 0;
}

int op_equ(struct microjs_sdt * microjs)
{
	TRACEF("%04x\tEQ\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_EQ;
	return 0;
}

int op_neq(struct microjs_sdt * microjs)
{
	TRACEF("%04x\tNE\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_NE;
	return 0;
}

int op_gt(struct microjs_sdt * microjs)
{
	TRACEF("%04x\tGT\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_GT;
	return 0;
}

int op_lt(struct microjs_sdt * microjs)
{
	TRACEF("%04x\tLT\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_LT;
	return 0;
}

int op_gte(struct microjs_sdt * microjs)
{
	TRACEF("%04x\tGE\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_GE;
	return 0;
}

int op_lte(struct microjs_sdt * microjs)
{
	TRACEF("%04x\tLE\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_LE;
	return 0;
}

int op_logic_or(struct microjs_sdt * microjs)
{
	TRACEF("%04x\tOR\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_OR;
	return 0;
}

int op_logic_and(struct microjs_sdt * microjs)
{
	TRACEF("%04x\tAND\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_AND;
	return 0;
}

int op_add(struct microjs_sdt * microjs)
{
	TRACEF("%04x\tADD\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_ADD;
	return 0;
}

int op_sub(struct microjs_sdt * microjs)
{
	TRACEF("%04x\tSUB\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_SUB;
	return 0;
}

int op_or(struct microjs_sdt * microjs)
{
	TRACEF("%04x\tOR\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_OR;
	return 0;
}

int op_xor(struct microjs_sdt * microjs)
{
	microjs->code[microjs->pc++] = OPC_XOR;
	return 0;
}

int op_mul(struct microjs_sdt * microjs)
{
	TRACEF("%04x\tMUL\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_MUL;
	return 0;
}

int op_div(struct microjs_sdt * microjs)
{
	TRACEF("%04x\tDIV\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_DIV;
	return 0;
}

int op_mod(struct microjs_sdt * microjs)
{
	microjs->code[microjs->pc++] = OPC_MOD;
	return 0;
}

int op_and(struct microjs_sdt * microjs)
{
	microjs->code[microjs->pc++] = OPC_AND;
	return 0;
}

int op_inv(struct microjs_sdt * microjs)
{
	microjs->code[microjs->pc++] = OPC_INV;
	return 0;
}

int op_minus(struct microjs_sdt * microjs)
{
	microjs->code[microjs->pc++] = OPC_NEG;
	return 0;
}

int op_not(struct microjs_sdt * microjs)
{
	microjs->code[microjs->pc++] = OPC_NEG;
	return 0;
}

int op_shl(struct microjs_sdt * microjs)
{
	microjs->code[microjs->pc++] = OPC_SHL;
	return 0;
}

int op_asr(struct microjs_sdt * microjs)
{
	microjs->code[microjs->pc++] = OPC_ASR;
	return 0;
}

int op_push_false(struct microjs_sdt * microjs)
{
	TRACEF("%04x\tI8 %d\n", microjs->pc, 0);
	microjs->code[microjs->pc++] = OPC_I8;
	microjs->code[microjs->pc++] = 0;
	return 0;
}

int op_push_true(struct microjs_sdt * microjs)
{
	TRACEF("%04x\tI8 %d\n", microjs->pc, 1);
	microjs->code[microjs->pc++] = OPC_I8;
	microjs->code[microjs->pc++] = 1;
	return 0;
}

int op_push_string(struct microjs_sdt * microjs) 
{
	int isz;

	if ((isz = cstr_add(microjs->tok.s, microjs->tok.qlf)) < 0) {
		fprintf(stderr, "can't create string!\n");
		return -1;
	}
	
	TRACEF("%04x\tI8 %d\n", microjs->pc, isz);
	microjs->code[microjs->pc++] = OPC_I8;
	microjs->code[microjs->pc++] = isz;
	return 0;
}

int op_push_int(struct microjs_sdt * microjs)
{
	int32_t x = microjs->tok.u32;

	TRACEF("%04x\t", microjs->pc);

	if (x >= 0) {
		if (x < 32768) {
			if (x < 128) {
				TRACEF("I8 %d\n", x);
				microjs->code[microjs->pc++] = OPC_I8;
				microjs->code[microjs->pc++] = x;
			} else {
				TRACEF("I16 %d\n", x);
				microjs->code[microjs->pc++] = OPC_I16;
				microjs->code[microjs->pc++] = x;
				microjs->code[microjs->pc++] = x >> 8;
			}
		} else {
			TRACEF("I32 %d\n", x);
			microjs->code[microjs->pc++] = OPC_I32;
			microjs->code[microjs->pc++] = x;
			microjs->code[microjs->pc++] = x >> 8;
			microjs->code[microjs->pc++] = x >> 16;
			microjs->code[microjs->pc++] = x >> 24;
		}
	} else {
		if (x >= -32768) {
			if (x >= -128) {
				TRACEF("I8 %d\n", x);
				microjs->code[microjs->pc++] = OPC_I8;
				microjs->code[microjs->pc++] = x;
			} else {
				TRACEF("I16 %d\n", x);
				microjs->code[microjs->pc++] = OPC_I16;
				microjs->code[microjs->pc++] = x;
				microjs->code[microjs->pc++] = x >> 8;
			}
		} else {
			TRACEF("I32 %d\n", x);
			microjs->code[microjs->pc++] = OPC_I32;
			microjs->code[microjs->pc++] = x;
			microjs->code[microjs->pc++] = x >> 8;
			microjs->code[microjs->pc++] = x >> 16;
			microjs->code[microjs->pc++] = x >> 24;
		}
	}

	return 0;
}

int op_if_cond(struct microjs_sdt * microjs)
{
	struct sym_ref ref;

	ref.lbl = sym_lbl_next(microjs->tab);

	TRACEF(".L%d:\n%04x\tJEQ xxxx\n", ref.lbl, microjs->pc);

	/* save current location on a temporary variable */
	microjs->code[microjs->pc++] = OPC_JEQ;
	ref.addr = microjs->pc;
	/* reserve 2 positions for jump address */
	microjs->pc += 2;

	/* Alloc a temporary reference for the loop jump */
	if (!sym_ref_push(microjs->tab, &ref))
		return -ERR_SYM_PUSH_FAIL;


	return 0;
}

int op_if_else(struct microjs_sdt * microjs)
{
	struct sym_ref ref;
	int offs;
	
	/* get temporary reference */
	if (!sym_ref_pop(microjs->tab, &ref))
		return -ERR_SYM_POP_FAIL;

	TRACEF("\tfix %04x -> JEQ %04x (.L%d)\n", ref.addr - 1, 
		   microjs->pc + 3, ref.lbl);
	offs = microjs->pc - (ref.addr - 1);
	microjs->code[ref.addr] = offs;
	microjs->code[ref.addr + 1] = offs >> 8;

	ref.lbl = sym_lbl_next(microjs->tab);
	TRACEF(".L%d:\n%04x\tJMP xxxx\n", ref.lbl, microjs->pc);
	/* save current location on the same temporary reference */
	microjs->code[microjs->pc++] = OPC_JMP;
	ref.addr = microjs->pc;
	/* reserve 2 positions for jump address */
	microjs->pc += 2;
	if (!sym_ref_push(microjs->tab, &ref))
		return -ERR_SYM_PUSH_FAIL;

	return 0;
}

int op_if_end(struct microjs_sdt * microjs)
{
	struct sym_ref ref;
	int offs;
	
	/* get temporary reference */
	if (!sym_ref_pop(microjs->tab, &ref))
		return -ERR_SYM_POP_FAIL;

	/* Adjust the conditinal jump */
	offs = (microjs->pc - 3) - (ref.addr - 1);
	microjs->code[ref.addr] = offs;
	microjs->code[ref.addr + 1] = offs >> 8;

	TRACEF("\tfix %04x -> Jxx %04x (.L%d)\n", ref.addr - 1, 
		   microjs->pc, ref.lbl);
	return 0;
}

/* --------------------------------------------------------------------------
   While Loop 
   -------------------------------------------------------------------------- */

int op_while_begin(struct microjs_sdt * microjs)
{
	struct sym_wld wld;

	/* Alloc a temporary reference for the loop jump */
	wld.loop = microjs->pc;
	wld.brk = 0;
	wld.ctn = 0;
	wld.lbl = sym_lbl_next(microjs->tab);

	if (!sym_wld_push(microjs->tab, &wld))
		return -ERR_SYM_PUSH_FAIL;

	TRACEF(".L%d.0:\n", wld.lbl);

	return 0;
}

int op_while_cond(struct microjs_sdt * microjs)
{
	struct sym_wld wld;

	if (!sym_wld_pop(microjs->tab, &wld))
		return -ERR_SYM_POP_FAIL;

	TRACEF(".L%d.1:\n%04x\tJEQ xxxx\n", wld.lbl, microjs->pc);

	microjs->code[microjs->pc++] = OPC_JEQ;
	/* save current location (condition) */
	wld.cond = microjs->pc;
	/* reserve 2 positions for jump address */
	microjs->pc += 2;

	/* push back */
	sym_wld_push(microjs->tab, &wld);

	return 0;
}


int op_while_end(struct microjs_sdt * microjs)
{
	struct sym_wld wld;
	int offs;

	/* Backpatch the conditinal jump */

	if (!sym_wld_pop(microjs->tab, &wld))
		return -ERR_SYM_POP_FAIL;

	offs = microjs->pc - (wld.cond - 1);
	TRACEF("\tfix %04x -> JEQ %04x (.L%d.1)\n", wld.cond - 1, 
		   microjs->pc + 3, wld.lbl);
	microjs->code[wld.cond ] = offs;
	microjs->code[wld.cond + 1] = offs >> 8;

	/* Repeat jump */
	offs = wld.loop - (microjs->pc + 3);
	TRACEF("%04x\tJMP %04x (.L%d.0 offs=%d)\n", microjs->pc, 
		   wld.loop, wld.lbl, offs);
	microjs->code[microjs->pc++] = OPC_JMP;
	microjs->code[microjs->pc++] = offs;
	microjs->code[microjs->pc++] = offs >> 8;

	return 0;
}


/* --------------------------------------------------------------------------
   For Loop 
   -------------------------------------------------------------------------- */

int op_for_init(struct microjs_sdt * microjs)
{
	struct sym_fld fld;

	fld.addr[3] = microjs->pc; /* save current location */
	fld.lbl = sym_lbl_next(microjs->tab); /* set a label (for debugging only) */
	fld.brk = 0;
	fld.ctn = 0;

	TRACEF(".L%d.3:\n", fld.lbl);
	if (!sym_fld_push(microjs->tab, &fld))
		return -ERR_SYM_PUSH_FAIL;

	return 0;
}

int op_for_cond(struct microjs_sdt * microjs)
{
	struct sym_fld fld;

	if (!sym_fld_pop(microjs->tab, &fld))
		return -ERR_SYM_POP_FAIL;

	/* Conditional jump to the body */
	TRACEF(".L%d.2:\n%04x\tJEQ xxxx\n", fld.lbl, microjs->pc);
	microjs->code[microjs->pc++] = OPC_JEQ;
	fld.addr[2] = microjs->pc; /* save current location */
	microjs->pc += 2; /* reserve space for jump address */

	/* Jump to the beginning of the body part */
	TRACEF(".L%d.1:\n%04x\tJMP xxxx\n", fld.lbl, microjs->pc);
	microjs->code[microjs->pc++] = OPC_JMP;
	fld.addr[1] = microjs->pc; /* save current location */
	microjs->pc += 2; /* reserve 2 positions for jump address */

	TRACEF(".L%d.0:\n", fld.lbl);
	fld.addr[0] = microjs->pc; /* save current location */

	/* push back */
	sym_fld_push(microjs->tab, &fld);

	return 0;
}

int op_for_after(struct microjs_sdt * microjs)
{
	struct sym_fld fld;
	int offs;
	int addr;

	if (!sym_fld_pop(microjs->tab, &fld))
		return -ERR_SYM_POP_FAIL;

	/* Adjust the jump to body */
	addr = fld.addr[1];
	offs = microjs->pc - (addr - 1);
	TRACEF("\tfix %04x -> JMP %04x (.L%d.1)\n", addr - 1, 
		   microjs->pc + 3, fld.lbl);
	microjs->code[addr] = offs;
	microjs->code[addr + 1] = offs >> 8;

	/* Repeat jump */
	addr = fld.addr[3];
	offs = addr - (microjs->pc + 3);
	TRACEF("%04x\tJMP %04x (.L%d.3 offs=%d)\n", microjs->pc, addr, 
		   fld.lbl, offs);
	microjs->code[microjs->pc++] = OPC_JMP;
	microjs->code[microjs->pc++] = offs;
	microjs->code[microjs->pc++] = offs >> 8;

	/* push back */
	sym_fld_push(microjs->tab, &fld);
	return 0;
}

int op_for_end(struct microjs_sdt * microjs)
{
	struct sym_fld fld;
	int offs;
	int addr;

	if (!sym_fld_pop(microjs->tab, &fld))
		return -ERR_SYM_POP_FAIL;

	/* Adjust the conditinal jump */
	addr = fld.addr[2];
	offs = microjs->pc - (addr - 1);
	TRACEF("\tfix %04x -> JEQ %04x (.L%d.2)\n", addr - 1, 
		   microjs->pc + 3, fld.lbl);
	microjs->code[addr] = offs;
	microjs->code[addr + 1] = offs >> 8;

	/* jump to the afterthought part */
	addr = fld.addr[0];
	offs = addr - (microjs->pc + 3);
	TRACEF("%04x\tJMP %04x (.L%d.0 offs=%d)\n", microjs->pc, 
		   addr, fld.lbl, offs);
	microjs->code[microjs->pc++] = OPC_JMP;
	microjs->code[microjs->pc++] = offs;
	microjs->code[microjs->pc++] = offs >> 8;

	return 0;
}

int (* op[])(struct microjs_sdt * microjs) = {
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
 	[ACTION(A_OP_PUSH_TMP)] = op_push_tmp,
 	[ACTION(A_OP_POP_TMP)] = op_pop_tmp,
 	[ACTION(A_OP_METHOD)] = op_method,
 	[ACTION(A_OP_ARG)] = op_arg,
 	[ACTION(A_OP_RET_DISCARD)] = op_ret_discard,
 	[ACTION(A_OP_PUSH_STRING)] = op_push_string,
 	[ACTION(A_OP_BLK_OPEN)] = op_blk_open,
 	[ACTION(A_OP_BLK_CLOSE)] = op_blk_close,

};

/* Syntax-directed translator */
/* Nonrecursive predictive parser */

int microjs_compile(struct microjs_sdt * microjs, 
					const char * txt, unsigned int len)
{
	struct lexer * lex = &microjs->lex;
	struct token tok;
	int lookahead;
	uint8_t * ll_sp;
	uint8_t * ll_sl;
	uint8_t * ll_top;
	int err;
	int sym;
	int k;

	if (len == 0)
		return 0;

	ll_sp = (uint8_t *)microjs + microjs->ll_sp;
	ll_sl = (uint8_t *)microjs + sizeof(struct microjs_sdt);
	ll_top = (uint8_t *)microjs + microjs->size;

	/* start the lexer */
	lexer_open(lex, txt, len);
	/* */
	lookahead = (tok = lexer_scan(lex)).typ;
	if (lookahead == T_ERR) {
		err = -tok.qlf;
		goto error;
	}
	while (ll_sp != ll_top) {
		/* pop the stack */
		sym = *ll_sp++;
#if MICROJS_TRACE_ENABLED
		DCC_LOG1(LOG_INFO, "<%s>", microjs_ll_sym[sym]);
#else
		DCC_LOG1(LOG_MSG, "<%d>", sym);
#endif
		if IS_A_TERMINAL(sym) {
			/* terminal */
			if (sym != lookahead) {
				err = ERR_UNEXPECTED_SYMBOL;
				goto error;
			}
			/* save the lookahead token */
			microjs->tok = tok;
			/* get next token */
			lookahead = (tok = lexer_scan(lex)).typ;
			if (lookahead == T_ERR) {
				err = -tok.qlf;
				goto error;
			}
		} else if IS_AN_ACTION(sym) {
			/* action */
			if ((err = op[ACTION(sym)](microjs)) < 0) {
				goto error;
			}
			/* FIXME: checking for the code buffer overflow at this
			   point is dangerous because we may have corrupted the 
			   mamory already!!!! */
			if (microjs->pc >= microjs->cdsz) {
				err = -ERR_CODE_MEM_OVERFLOW;
				goto error;
			}
		} else {
			/* non terminal */
			if ((k = microjs_ll_push(ll_sp, sym, lookahead)) < 0) {
				DCC_LOG2(LOG_WARNING, "sym=%d lookahed=%d", sym, lookahead);
				/* push the offending symbol back onto the stack */	
				ll_sp--;
				err = (lookahead == T_EOF) ? -ERR_UNEXPECED_EOF :
					-ERR_SYNTAX_ERROR;
#if MICROJS_TRACE_ENABLED
				fprintf(stderr, "microjs_ll_push() failed!\n");
				fprintf(stderr, "lookahead = %s\n", microjs_ll_sym[lookahead]);
				ll_stack_dump(stderr, ll_sp, ll_top);
#endif
				goto error;
			}
			ll_sp -= k;
			if (ll_sp < ll_sl) {
				err = -ERR_SDT_STACK_OVERFLOW;
				/* stack overflow */
				goto error;
			}
		}
	}

	microjs->ll_sp -= microjs_ll_start(ll_sp);

	/* save the parser's stack pointer */
	microjs->ll_sp = ll_sp - (uint8_t *)microjs;
	return microjs->pc;

error:
	/* save the parser's stack pointer */
	microjs->ll_sp = ll_sp - (uint8_t *)microjs;
	return err;
}

void microjs_sdt_reset(struct microjs_sdt * microjs)
{
	uint8_t * ll_sp;

	microjs->ll_sp = microjs->size;
	ll_sp = (uint8_t *)microjs + microjs->ll_sp;
	microjs->ll_sp -= microjs_ll_start(ll_sp);
}

struct microjs_sdt * microjs_sdt_init(uint32_t * sdt_buf, 
									  unsigned int sdt_size,
									  struct symtab * tab, 
									  uint8_t code[],
									  unsigned int code_size, 
									  unsigned int data_size)
{
	struct microjs_sdt * microjs = (struct microjs_sdt *)sdt_buf;

	microjs->tab = tab;
	/* data memory allocation info */
	microjs->tgt_heap = 0;
	microjs->tgt_sp = data_size;
	/* code memory */
	microjs->pc = 0;
	microjs->cdsz = code_size;
	microjs->code = code; /* code buffer */
	/* size of the buffer provided for parsing */
	microjs->size = sdt_size;
	microjs->ll_sp = sdt_size;

	microjs_sdt_reset(microjs);

	return microjs;
}

int microjs_sdt_done(struct microjs_sdt * microjs)
{
	if (microjs->pc + 1 > microjs->cdsz)
		return -ERR_CODE_MEM_OVERFLOW;

	/* code memory */
	TRACEF("%04x\tABT\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_ABT;

#if MICROJS_TRACE_ENABLED
	TRACEF("\nSYMBOL TABLE:\n");
	sym_dump(stdout, microjs->tab);
	TRACEF("\n");
#endif
	return microjs->pc;
}

void microjs_sdt_error(FILE * f, struct microjs_sdt * microjs, int err)
{
	struct lexer * lex = &microjs->lex;

	if (err < 0)
		err = -err;

	fflush(f);
	lexer_print_err(f, lex, err);
}

