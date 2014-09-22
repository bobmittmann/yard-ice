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

#include <sys/param.h>
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

	if ((obj = sym_obj_scope_lookup(microjs->tab, microjs->tok.s, 
						   microjs->tok.qlf)) != NULL) {
		DCC_LOG(LOG_INFO, "object exist in current scope!");
		return 0;
	}

	if ((obj = sym_obj_new(microjs->tab, microjs->tok.s, 
						   microjs->tok.qlf)) == NULL) {
		DCC_LOG(LOG_WARNING, "sym_obj_new() failed!");
		return -ERR_OBJ_NEW_FAIL;
	}

#if MICROJS_FUNCTIONS_ENABLED
#error "functions not implemented!!!" 
	if ((obj->flags & SYM_OBJ_GLOBAL) == 0) {
		/* FIXME stack allocation */
		DCC_LOG(LOG_TRACE, "stack allocation!!!");
		return 0;
	}
#endif

	if ((addr = tgt_alloc32(microjs)) < 0) {
		DCC_LOG(LOG_WARNING, "tgt_alloc32() failed!");
		return addr;
	}

	DCC_LOG1(LOG_INFO, "addr=0x%04x", addr);

	obj->addr = addr;
	/* initial variables are int */
	obj->size = 4;
	/* flag as allocated */
	obj->flags |= SYM_OBJ_ALLOC;

	TRACEF(".WORD \"%s\" (%04x)\n", 
		   sym_obj_name(microjs->tab, obj), obj->addr);
	return 0;
}

int op_push_tmp(struct microjs_sdt * microjs)
{
	struct sym_tmp tmp;

	tmp.s = microjs->tok.s;
	tmp.len = microjs->tok.qlf;

	DCC_LOG1(LOG_INFO, "tmp.len=%d", tmp.len);

	return sym_tmp_push(microjs->tab, &tmp);
}

int op_pop_tmp(struct microjs_sdt * microjs)
{
	DCC_LOG(LOG_INFO, " >>> ...");
	return sym_tmp_pop(microjs->tab, NULL);
}

int op_assign(struct microjs_sdt * microjs)
{
	struct sym_obj * obj;
	struct sym_tmp tmp;
	uint16_t addr;
	int ret;

	if ((ret = sym_tmp_pop(microjs->tab, &tmp)) < 0)
		return ret;

	DCC_LOG1(LOG_INFO, "tmp.len=%d", tmp.len);

	if ((obj = sym_obj_lookup(microjs->tab, tmp.s, tmp.len)) == NULL)
		return -ERR_VAR_UNKNOWN;

	addr = (obj->addr >> 2) & 0x0fff;
#if MICROJS_FUNCTIONS_ENABLED
	if ((obj->flags & SYM_OBJ_GLOBAL) == 0) {
		TRACEF("%04x\tSTR \"%s\" (%04x)\n", microjs->pc, 
			   sym_obj_name(microjs->tab, obj), obj->addr);
		microjs->code[microjs->pc++] = OPC_STR;
		microjs->code[microjs->pc++] = obj->addr >> 2;
		return 0;
	}
#endif

	TRACEF("%04x\tST \"%s\" (%04x)\n", microjs->pc, 
		   sym_obj_name(microjs->tab, obj), obj->addr);
	microjs->code[microjs->pc++] = OPC_ST + (addr & 0x0f);
	microjs->code[microjs->pc++] = addr >> 4;
	
	return 0;
}

int op_attr(struct microjs_sdt * microjs)
{
	struct sym_obj * obj;
	struct sym_tmp tmp;
	uint16_t addr;
	int ret;

	if ((ret = sym_tmp_pop(microjs->tab, &tmp)) < 0)
		return ret;

	if ((obj = sym_obj_lookup(microjs->tab, tmp.s, tmp.len)) == NULL)
		return -ERR_VAR_UNKNOWN;

	addr = (obj->addr >> 2) & 0x0fff;
#if MICROJS_FUNCTIONS_ENABLED
	if ((obj->flags & SYM_OBJ_GLOBAL) == 0) {
		TRACEF("%04x\tLDR \'%s\" (%04x)\n", microjs->pc, 
			   sym_obj_name(microjs->tab, obj), obj->addr);
		microjs->code[microjs->pc++] = OPC_LDR;
		microjs->code[microjs->pc++] = obj->addr >> 2;
		return 0;
	} 
#endif

	TRACEF("%04x\tLD \'%s\" (%04x)\n", microjs->pc, 
		   sym_obj_name(microjs->tab, obj), obj->addr);
	microjs->code[microjs->pc++] = OPC_LD + (addr & 0x0f);
	microjs->code[microjs->pc++] = addr >> 4;

	return 0;
}

int op_method(struct microjs_sdt * microjs)
{
	struct ext_fndef * fndef;
	struct sym_call call;
	struct sym_tmp tmp;
	int xid;
	int ret;

	if ((ret = sym_tmp_pop(microjs->tab, &tmp)) < 0)
		return ret;

	if ((xid = sym_extern_lookup(microjs->tab, tmp.s, tmp.len)) < 0) {
		DCC_LOG(LOG_WARNING, "sym_extern_lookup() failed!");
		return -ERR_EXTERN_UNKNOWN;
	}	

	fndef = sym_extern_get(microjs->tab, xid);

	DCC_LOG1(LOG_INFO, "external \"%s\"", fndef->nm);

	/* prepare to call a function or method */
	call.xid = xid;
	call.retcnt = fndef->ret;
	call.argmin = fndef->argmin;
	call.argmax = fndef->argmax;
	call.argcnt = 0;

	return sym_call_push(microjs->tab, &call);
}

int op_arg(struct microjs_sdt * microjs)
{
	struct sym_call call;
	int ret;

	if ((ret = sym_call_pop(microjs->tab, &call)) < 0)
		return ret;

	call.argcnt++; /* increment the argument counter */

	/* push back; */
	return sym_call_push(microjs->tab, &call);
}

int op_call(struct microjs_sdt * microjs)
{
	struct sym_call call;
	int ret;

	if ((ret = sym_call_pop(microjs->tab, &call)) < 0)
		return ret;

	DCC_LOG(LOG_INFO, " EXTERN ");

	if (call.argcnt < call.argmin)
		return -ERR_ARG_MISSING;
	if (call.argcnt > call.argmax)
		return -ERR_TOO_MANY_ARGS;

	TRACEF("%04x\tEXT \'%s\" %d\n", microjs->pc, 
		   sym_extern_name(microjs->tab, call.xid), call.argcnt);

	microjs->code[microjs->pc++] = OPC_EXT;
	/* external call number */
	microjs->code[microjs->pc++] = call.xid;
	/* stack size */
	microjs->code[microjs->pc++] = call.argcnt;

	/* push back; */
	return sym_call_push(microjs->tab, &call);
}

int op_ret_discard(struct microjs_sdt * microjs)
{
	struct sym_call call;
	int ret;
	int n;

	if ((ret = sym_call_pop(microjs->tab, &call)) < 0)
		return ret;

	DCC_LOG2(LOG_INFO, "argcnt=%d, retcnt=%d", call.argcnt, call.retcnt);

	if ((n = call.retcnt) > 0) {
		if (n > 1) {
			TRACEF("%04x\tISP %d\n", microjs->pc, n);
			microjs->code[microjs->pc++] = OPC_ISP;
			microjs->code[microjs->pc++] = n;
		} else {
			TRACEF("%04x\tPOP\n", microjs->pc);
			microjs->code[microjs->pc++] = OPC_POP;
		}
	}

	return 0;
}

int op_call_ret(struct microjs_sdt * microjs)
{
	struct sym_call call;
	int ret;
	int n;

	if ((ret = sym_call_pop(microjs->tab, &call)) < 0)
		return ret;

	DCC_LOG2(LOG_TRACE, "argcnt=%d, retcnt=%d", call.argcnt, call.retcnt);
	if (call.retcnt < 1) {
		return -ERR_RET_COUNT_MISMATCH;
	}

	/* discard all but one of the returning values */
	if ((n = call.retcnt - 1) > 0) {
		if (n > 1) {
			TRACEF("%04x\tISP %d\n", microjs->pc, n);
			microjs->code[microjs->pc++] = OPC_ISP;
			microjs->code[microjs->pc++] = n;
		} else {
			TRACEF("%04x\tPOP\n", microjs->pc);
			microjs->code[microjs->pc++] = OPC_POP;
		}
	} else if (n < 0) {
		return -ERR_GENERAL;
	}

	return 0;
}

int op_blk_open(struct microjs_sdt * microjs) 
{
	/* save the heap state */
	sym_addr_push(microjs->tab, &microjs->tgt_heap);
	/* save the stack frame */
	return sym_sf_push(microjs->tab);
}

int op_blk_close(struct microjs_sdt * microjs) 
{
	int ret;
	/* restore the stack frame */
	if ((ret = sym_sf_pop(microjs->tab)) < 0)
		return ret;

	/* restore the heap state */
	return sym_addr_pop(microjs->tab, &microjs->tgt_heap);
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
	TRACEF("%04x\tMOD\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_MOD;
	return 0;
}

int op_and(struct microjs_sdt * microjs)
{
	TRACEF("%04x\tAND\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_AND;
	return 0;
}

int op_inv(struct microjs_sdt * microjs)
{
	TRACEF("%04x\tINV\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_INV;
	return 0;
}

int op_minus(struct microjs_sdt * microjs)
{
	TRACEF("%04x\tNEG\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_NEG;
	return 0;
}

int op_not(struct microjs_sdt * microjs)
{
	TRACEF("%04x\tNOT\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_NEG;
	return 0;
}

int op_shl(struct microjs_sdt * microjs)
{
	TRACEF("%04x\tSHL\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_SHL;
	return 0;
}

int op_asr(struct microjs_sdt * microjs)
{
	TRACEF("%04x\tASR\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_ASR;
	return 0;
}

int op_push_false(struct microjs_sdt * microjs)
{
	TRACEF("%04x\tI4 %d\n", microjs->pc, 0);
	microjs->code[microjs->pc++] = OPC_I4 + 0;
	return 0;
}

int op_push_true(struct microjs_sdt * microjs)
{
	TRACEF("%04x\tI4 %d\n", microjs->pc, 1);
	microjs->code[microjs->pc++] = OPC_I4 + 1;
	return 0;
}

int op_push_string(struct microjs_sdt * microjs) 
{
#if MICROJS_STRINGS_ENABLED
	int isz;

	if ((isz = cstr_add(microjs->tok.s, microjs->tok.qlf)) < 0) {
		fprintf(stderr, "can't create string!\n");
		/* FIXME: more specific error */
		return -ERR_GENERAL;
	}

	if (isz < 8) {
		TRACEF("%04x\tI4 %d\n", microjs->pc, isz);
		microjs->code[microjs->pc++] = OPC_I4 + isz;
	} else {
		TRACEF("%04x\tI8 %d\n", microjs->pc, isz);
		microjs->code[microjs->pc++] = OPC_I8;
		microjs->code[microjs->pc++] = isz;
	}

	return 0;
#else
	return -ERR_STRINGS_UNSUPORTED;
#endif
}

int op_push_int(struct microjs_sdt * microjs)
{
	int32_t x = microjs->tok.u32;

	TRACEF("%04x\t", microjs->pc);

	if (x >= 0) {
		if (x < 32768) {
			if (x < 128) {
				if (x < 8) {
					TRACEF("I4 %d\n", x);
					microjs->code[microjs->pc++] = OPC_I4 + x;
				} else {
					TRACEF("I8 %d\n", x);
					microjs->code[microjs->pc++] = OPC_I8;
					microjs->code[microjs->pc++] = x;
				}
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
				if (x >= -8) {
					TRACEF("I4 %d\n", x);
					microjs->code[microjs->pc++] = OPC_I4 + (x & 0x0f);
				} else {
					TRACEF("I8 %d\n", x);
					microjs->code[microjs->pc++] = OPC_I8;
					microjs->code[microjs->pc++] = x;
				}
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

int op_try_begin(struct microjs_sdt * microjs) 
{
	struct sym_ref ref;

	DCC_LOG(LOG_INFO, "<<<<<<");

	/* save current location on a temporary reference */
	ref.lbl = sym_lbl_next(microjs->tab);
	ref.addr = microjs->pc;
	TRACEF(".L%d:\n%04x\tPUSHX xxxx\n", ref.lbl, microjs->pc);
	/* Insert the opcode, the address will be backpatched alter */
	microjs->code[microjs->pc] = OPC_PUHSX;
	microjs->pc += 2;

	return sym_ref_push(microjs->tab, &ref);
}

int op_try_end(struct microjs_sdt * microjs) 
{
	struct sym_ref ref;
	int offs;
	int ret;
	
	DCC_LOG(LOG_INFO, ">>>>>>");

	/* get the temporary reference */
	if ((ret = sym_ref_pop(microjs->tab, &ref)) < 0)
		return ret;

	/* skip the exception rethrow */
	offs = 1;
	TRACEF("%04x\tJMP %04x\n", microjs->pc, microjs->pc + offs + 3);
	microjs->code[microjs->pc++] = OPC_JMP + (offs & 0x0f);
	microjs->code[microjs->pc++] = offs >> 4;

	/* adjust the exception handling pointer to hit the rethrow */
	microjs->code[ref.addr] += microjs->pc & 0x0f ;
	microjs->code[ref.addr + 1] = microjs->pc >> 4;
	TRACEF("\tfix %04x -> PUSHX %04x (.L%d)\n", ref.addr, 
		   microjs->pc, ref.lbl);

	/* rethrow the same exception */
	TRACEF("%04x\tXPT\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_XPT;

	return 0;
}

int op_catch(struct microjs_sdt * microjs) 
{
	struct sym_ref ref1;
	struct sym_ref ref2;
	int ret;

	DCC_LOG(LOG_INFO, "-----");

	/* get the try exception frame reference */
	if ((ret = sym_ref_pop(microjs->tab, &ref1)) < 0)
		return ret;

	/* save current location */
	ref2.lbl = sym_lbl_next(microjs->tab);
	ref2.addr = microjs->pc;
	TRACEF(".L%d:\n%04x\tJMP xxxx\n", ref2.lbl, microjs->pc);
	microjs->code[microjs->pc] = OPC_JMP;
	microjs->pc += 2;
	if ((ret = sym_ref_push(microjs->tab, &ref2)) < 0)
		return ret;

	/* Adjust the exception handling pointer */
	microjs->code[ref1.addr] += microjs->pc & 0x0f ;
	microjs->code[ref1.addr + 1] = microjs->pc >> 4;
	TRACEF("\tfix %04x -> PUSHX %04x (.L%d)\n", ref1.addr, 
		   microjs->pc, ref1.lbl);

	return 0;
}

/*
 */
int op_patch_ref(struct microjs_sdt * microjs) 
{
	struct sym_ref ref;
	int offs;
	int ret;
	
	DCC_LOG(LOG_INFO, "-----");

	/* get temporary reference */
	if ((ret = sym_ref_pop(microjs->tab, &ref)) < 0)
		return ret;

	/* Backpatch the jump */
	offs = (microjs->pc - 2) - ref.addr;
	TRACEF("\tfix %04x -> Jxx %04x (.L%d)\n", ref.addr, 
		   microjs->pc, ref.lbl);
	microjs->code[ref.addr ] += offs & 0x0f;
	microjs->code[ref.addr + 1] = offs >> 4;

	return 0;
}

int op_throw(struct microjs_sdt * microjs) 
{
	TRACEF("%04x\tXPT\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_XPT;
	return 0;
}

int op_if_cond(struct microjs_sdt * microjs)
{
	struct sym_ref ref;

	/* save current location on a temporary variable */
	ref.addr = microjs->pc;
	ref.lbl = sym_lbl_next(microjs->tab);
	TRACEF(".L%d:\n%04x\tJEQ xxxx\n", ref.lbl, microjs->pc);
	/* reserve 2 positions for opcode + jump address */
	microjs->code[microjs->pc] = OPC_JEQ;
	microjs->pc += 2;

	/* Alloc a temporary reference for the loop jump */
	return sym_ref_push(microjs->tab, &ref);
}

int op_if_else(struct microjs_sdt * microjs)
{
	struct sym_ref ref;
	int offs;
	int ret;
	
	/* get temporary reference */
	if ((ret = sym_ref_pop(microjs->tab, &ref)) < 0)
		return ret;

	/* Backpatch the jump */
	offs = microjs->pc - ref.addr;
	TRACEF("\tfix %04x -> JEQ %04x (.L%d)\n", ref.addr, 
		   microjs->pc + 2, ref.lbl);
	microjs->code[ref.addr ] += offs & 0x0f;
	microjs->code[ref.addr + 1] = offs >> 4;

	/* save current location on the same temporary reference */
	ref.lbl = sym_lbl_next(microjs->tab);
	ref.addr = microjs->pc;
	TRACEF(".L%d:\n%04x\tJMP xxxx\n", ref.lbl, microjs->pc);
	microjs->code[microjs->pc] = OPC_JMP;
	microjs->pc += 2;

	return sym_ref_push(microjs->tab, &ref);
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
	TRACEF(".L%d.0:\n", wld.lbl);

	return sym_wld_push(microjs->tab, &wld);
}

int op_while_cond(struct microjs_sdt * microjs)
{
	struct sym_wld wld;
	int ret;

	if ((ret = sym_wld_pop(microjs->tab, &wld)) < 0)
		return ret;

	TRACEF(".L%d.1:\n%04x\tJEQ xxxx\n", wld.lbl, microjs->pc);

	/* save current location (condition) */
	wld.cond = microjs->pc;
	/* reserve 2 positions for opcode + jump address */
	microjs->code[microjs->pc] = OPC_JEQ;
	microjs->pc += 2;

	/* push back */
	return sym_wld_push(microjs->tab, &wld);
}


int op_while_end(struct microjs_sdt * microjs)
{
	struct sym_wld wld;
	int offs;
	int ret;

	if ((ret = sym_wld_pop(microjs->tab, &wld)) < 0)
		return ret;

	/* Backpatch the conditinal jump */
	offs = microjs->pc - wld.cond;
	TRACEF("\tfix %04x -> JEQ %04x (.L%d.1)\n", wld.cond, 
		   microjs->pc + 2, wld.lbl);
	microjs->code[wld.cond ] += offs & 0x0f;
	microjs->code[wld.cond + 1] = offs >> 4;

	/* Repeat jump */
	offs = wld.loop - (microjs->pc + 2);
	TRACEF("%04x\tJMP %04x (.L%d.0 offs=%d)\n", microjs->pc, 
		   wld.loop, wld.lbl, offs);
	microjs->code[microjs->pc++] = OPC_JMP + (offs & 0x0f);
	microjs->code[microjs->pc++] = offs >> 4;

	return 0;
}


/* --------------------------------------------------------------------------
   For Loop 
   -------------------------------------------------------------------------- */

int op_for_init(struct microjs_sdt * microjs)
{
	struct sym_fld fld;

	fld.addr[0] = microjs->pc; /* save current location */
	fld.lbl = sym_lbl_next(microjs->tab); /* set a label (for debugging only) */
	fld.brk = 0;
	fld.ctn = 0;
	TRACEF(".L%d.0:\n", fld.lbl);

	return sym_fld_push(microjs->tab, &fld);
}

int op_for_cond(struct microjs_sdt * microjs)
{
	struct sym_fld fld;
	int ret;

	if ((ret = sym_fld_pop(microjs->tab, &fld)) < 0)
		return ret;

	/* Conditional jump to the body */
	TRACEF(".L%d.1:\n%04x\tJEQ xxxx\n", fld.lbl, microjs->pc);
	fld.addr[1] = microjs->pc; /* save current location */
	microjs->code[microjs->pc] = OPC_JEQ;
	microjs->pc += 2; /* reserve space for jump address */

	/* Jump to the beginning of the body part */
	TRACEF(".L%d.2:\n%04x\tJMP xxxx\n", fld.lbl, microjs->pc);
	fld.addr[2] = microjs->pc; /* save current location */
	microjs->code[microjs->pc] = OPC_JMP;
	microjs->pc += 2; /* reserve 2 positions for jump address */

	TRACEF(".L%d.3:\n", fld.lbl);
	fld.addr[3] = microjs->pc; /* save current location */

	/* push back */
	return sym_fld_push(microjs->tab, &fld);
}

int op_for_after(struct microjs_sdt * microjs)
{
	struct sym_fld fld;
	int offs;
	int addr;
	int ret;

	if ((ret = sym_fld_pop(microjs->tab, &fld)) < 0)
		return ret;

	/* Adjust the jump to body */
	addr = fld.addr[2];
	offs = microjs->pc - addr;
	TRACEF("\tfix %04x -> JMP %04x (.L%d.2)\n", addr, 
		   microjs->pc + 2, fld.lbl);
	microjs->code[addr] += offs & 0x0f;
	microjs->code[addr + 1] = offs >> 4;

	/* Repeat jump */
	addr = fld.addr[0];
	offs = addr - (microjs->pc + 2);
	TRACEF("%04x\tJMP %04x (.L%d.0 offs=%d)\n", microjs->pc, addr, 
		   fld.lbl, offs);
	microjs->code[microjs->pc++] = OPC_JMP + (offs & 0x0f);
	microjs->code[microjs->pc++] = offs >> 4;

	/* push back */
	return sym_fld_push(microjs->tab, &fld);
}

int op_for_end(struct microjs_sdt * microjs)
{
	struct sym_fld fld;
	int offs;
	int addr;
	int ret;

	if ((ret = sym_fld_pop(microjs->tab, &fld)) < 0)
		return ret;

	/* Adjust the conditinal jump */
	addr = fld.addr[1];
	offs = microjs->pc - addr;
	TRACEF("\tfix %04x -> JEQ %04x (.L%d.1)\n", addr, 
		   microjs->pc + 2, fld.lbl);
	microjs->code[addr] += offs & 0x0f;
	microjs->code[addr + 1] = offs >> 4;

	/* jump to the afterthought part */
	addr = fld.addr[3];
	offs = addr - (microjs->pc + 2);
	TRACEF("%04x\tJMP %04x (.L%d.3 offs=%d)\n", microjs->pc, 
		   addr, fld.lbl, offs);
	microjs->code[microjs->pc++] = OPC_JMP + (offs & 0x0f);
	microjs->code[microjs->pc++] = offs >> 4;

	return 0;
}

int (* const op[])(struct microjs_sdt * microjs) = {
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
 	[ACTION(A_OP_IF_END)] = op_patch_ref,
 	[ACTION(A_OP_FOR_INIT)] = op_for_init,
 	[ACTION(A_OP_FOR_COND)] = op_for_cond,
 	[ACTION(A_OP_FOR_AFTER)] = op_for_after,
 	[ACTION(A_OP_FOR_END)] = op_for_end,
 	[ACTION(A_OP_PUSH_TMP)] = op_push_tmp,
 	[ACTION(A_OP_POP_TMP)] = op_pop_tmp,
 	[ACTION(A_OP_METHOD)] = op_method,
 	[ACTION(A_OP_CALL)] = op_call,
 	[ACTION(A_OP_ATTR)] = op_attr,
 	[ACTION(A_OP_ARG)] = op_arg,
 	[ACTION(A_OP_RET_DISCARD)] = op_ret_discard,
 	[ACTION(A_OP_CALL_RET)] = op_call_ret,
 	[ACTION(A_OP_PUSH_STRING)] = op_push_string,
 	[ACTION(A_OP_BLK_OPEN)] = op_blk_open,
 	[ACTION(A_OP_BLK_CLOSE)] = op_blk_close,
 	[ACTION(A_OP_TRY_BEGIN)] = op_try_begin,
 	[ACTION(A_OP_TRY_END)] = op_try_end,
 	[ACTION(A_OP_CATCH)] = op_catch,
 	[ACTION(A_OP_CATCH_END)] = op_patch_ref,
 	[ACTION(A_OP_THROW)] = op_throw

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
				DCC_LOG(LOG_WARNING, "syntax action failed!");
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

#if MICROJS_TRACE_ENABLED
	TRACEF("\nSYMBOL TABLE:\n");
	sym_dump(stderr, microjs->tab);
	TRACEF("\n");
#endif
	return err;
}

void microjs_sdt_reset(struct microjs_sdt * microjs)
{
	uint8_t * ll_sp;

	microjs->ll_sp = microjs->size;
	ll_sp = (uint8_t *)microjs + microjs->ll_sp;
	/* intialize the parser */
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

	/* generate the default exception handler */
	op_try_begin(microjs);

	return microjs;
}

int microjs_sdt_done(struct microjs_sdt * microjs)
{
	struct sym_ref ref;

	if (microjs->pc + 3 > microjs->cdsz)
		return -ERR_CODE_MEM_OVERFLOW;

	/* remove the exception handler frame from the stack */
	TRACEF("%04x\tPOP\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_POP;

	/* stop execution return 0 */
	TRACEF("%04x\tABT\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_ABT;

	/* get the default exception handler */
	sym_pick(microjs->tab, 0, &ref, sizeof(struct sym_ref));

	/* patch the default exception handling pointer */
	microjs->code[ref.addr] += microjs->pc & 0x0f;
	microjs->code[ref.addr + 1] = microjs->pc >> 4;
	TRACEF("\tfix %04x -> PUSHX %04x (.L%d)\n", ref.addr, 
		   microjs->pc, ref.lbl);

	/* stop execution return exception error code */
	TRACEF("%04x\tRET\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_RET;


#if MICROJS_TRACE_ENABLED
	TRACEF("\nSYMBOL TABLE:\n");
	sym_dump(stdout, microjs->tab);
	TRACEF("\n");
#endif
	return microjs->pc;
}

void microjs_sdt_error(FILE * f, struct microjs_sdt * microjs, int err)
{
#if MICROJS_VERBOSE_ENABLED
	struct lexer * lex = &microjs->lex;

	if (err < 0)
		err = -err;

	fflush(f);
	lexer_print_err(f, lex, err);
#endif
}

int microjs_tgt_heap(struct microjs_sdt * microjs)
{
	return microjs->tgt_heap;
}

