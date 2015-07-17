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

#define SIZEOF_WORD ((int)sizeof(int32_t))

#define SIGNEXT4BIT(_X) ({ struct { int32_t x: 4; } s; \
						  s.x = (_X); (int32_t)s.x; })

static int tgt_alloc32(struct microjs_sdt * microjs)
{
	unsigned int addr;

	/* ensure memory alignment */
	addr = (microjs->data_pos + SIZEOF_WORD - 1) & ~(SIZEOF_WORD - 1);

	microjs->data_pos = addr + SIZEOF_WORD;
	DCC_LOG1(LOG_TRACE, "data_pos=%d", microjs->data_pos);

	if (microjs->data_pos > microjs->tab->rt.data_sz) {
		microjs->tab->rt.data_sz = microjs->data_pos;
	}

	return addr;
}

static int tgt_stack_push(struct microjs_sdt * microjs)
{
	/* update stack usage */
	microjs->stack_pos += SIZEOF_WORD;

	if (microjs->stack_pos > microjs->tab->rt.stack_sz)
		microjs->tab->rt.stack_sz = microjs->stack_pos;

	return 0;
}

static int tgt_stack_pop(struct microjs_sdt * microjs)
{
	if (microjs->stack_pos == 0)
		return -ERR_STACK_UNDERFLOW;

	microjs->stack_pos -= SIZEOF_WORD;

	return 0;
}

static int tgt_stack_adjust(struct microjs_sdt * microjs, int cnt)
{
	int addr = microjs->stack_pos + cnt * SIZEOF_WORD;

	if (addr < 0)
		return -ERR_STACK_UNDERFLOW;

	microjs->stack_pos = addr;

	return 0;
}

/* --------------------------------------------------------------------------
   Code generation
   -------------------------------------------------------------------------- */

static int encode_int(struct microjs_sdt * microjs, int32_t x)
{
#if MICROJS_OPTIMIZATION_ENABLED
	microjs->spc = microjs->pc; /* save code pointer */
#endif

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

static bool is_constant(struct microjs_sdt * microjs, 
						unsigned int spc, int32_t * xp)
{
	do {
		int opc;
		int32_t val;
		uint8_t * pc = &microjs->code[spc];

		opc = *pc++;
		if ((opc & 0xf0) == OPC_I4) {
			val = SIGNEXT4BIT(opc & 0xf);
		} else if (opc == OPC_I8) {
			val = (int8_t)pc[0];
		} else if (opc == OPC_I16) {
			val = (int16_t)(pc[0] | pc[1] << 8);
		} else if (opc == OPC_I32) {
			val = (int32_t)(pc[0] | pc[1] << 8 | 
							pc[2] << 16 | pc[3] << 24);
		} else
			break;
		*xp = val;
		return true;
	} while (0);

	return false;
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

/* --------------------------------------------------------------------------
   Scoping
   -------------------------------------------------------------------------- */

int op_blk_open(struct microjs_sdt * microjs) 
{
	/* save the heap state */
	sym_addr_push(microjs->tab, &microjs->data_pos);
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
	return sym_addr_pop(microjs->tab, &microjs->data_pos);
}

/* --------------------------------------------------------------------------
   Variables
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
		DCC_LOG(LOG_INFO, "sym_obj_new() failed!");
		return -ERR_OBJ_NEW_FAIL;
	}

#if MICROJS_FUNCTIONS_ENABLED
#error "functions not implemented!!!" 
	if ((obj->flags & SYM_OBJ_GLOBAL) == 0) {
		/* FIXME stack allocation */
		DCC_LOG(LOG_INFO, "stack allocation!!!");
		return 0;
	}
#endif

	if ((addr = tgt_alloc32(microjs)) < 0) {
		DCC_LOG(LOG_INFO, "tgt_alloc32() failed!");
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

int op_var_eval(struct microjs_sdt * microjs)
{
	struct sym_obj * obj;
	struct sym_tmp tmp;
	uint16_t addr;
	int ret;

	if ((ret = sym_tmp_pop(microjs->tab, &tmp)) < 0)
		return ret;

	if ((obj = sym_obj_lookup(microjs->tab, tmp.s, tmp.len)) == NULL)
		return -ERR_VAR_UNKNOWN;

#if MICROJS_OPTIMIZATION_ENABLED
	microjs->spc = microjs->pc;        /* save code pointer */
#endif

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

	return tgt_stack_push(microjs);
}

int op_var_assign(struct microjs_sdt * microjs)
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

#if MICROJS_OPTIMIZATION_ENABLED
	microjs->spc = microjs->pc;        /* save code pointer */
#endif

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

	return tgt_stack_pop(microjs);
}

int op_object_get(struct microjs_sdt * microjs)
{
	struct extdef * xdef;
	struct sym_cld cld;
	struct sym_tmp tmp;
	int ret;
	int xid;

	if ((ret = sym_tmp_pop(microjs->tab, &tmp)) < 0)
		return ret;

	if ((xid = lib_lookup(microjs->libdef, tmp.s, tmp.len)) < 0) {
		DCC_LOG(LOG_WARNING, "extern unknown!");
		return -ERR_EXTERN_UNKNOWN;
	}

	xdef = lib_extern_get(microjs->libdef, xid);

	if (EXTDEF_TYPE(xdef) != O_OBJECT)
		return -ERR_EXTERN_NOT_OBJECT;

	if (EXTDEF_FLAG(xdef, O_ARRAY))
		return -ERR_EXTERN_NOT_SCALAR;

	if (EXTDEF_FLAG(xdef, O_SINGLETON)) {
		encode_int(microjs, xdef->o.inst);
	} else {
		/* Object instance resolution: if this isn't a singleton we have 
		   to provide a call for retrieving the object's instance */
		/* XXX: the object instance accepts no argument 
		   and return 1 value. This call could be optimized */
		TRACEF("%04x\tEXT \'%s\" %d\n", microjs->pc, 
			   sym_extern_name(microjs->libdef, xid), 1);
		microjs->code[microjs->pc++] = OPC_EXT;
		microjs->code[microjs->pc++] = xid; /* external call number */
		microjs->code[microjs->pc++] = 0; /* stack size (arguments) */
	}
	/* pushing one value into stack, 
	   inform the stack evaluator. */ 
	tgt_stack_push(microjs);

	cld.cid = xdef->o.cdef;
	/* push  the class definition */
	return sym_cld_push(microjs->tab, &cld);


}
/* --------------------------------------------------------------------------
   Array index translation
   -------------------------------------------------------------------------- */

int op_array_xlat(struct microjs_sdt * microjs)
{
	struct extdef * xdef;
	struct sym_cld cld;
	struct sym_tmp tmp;
	int ret;
	int xid;

	if ((ret = sym_tmp_pop(microjs->tab, &tmp)) < 0)
		return ret;

	if ((xid = lib_lookup(microjs->libdef, tmp.s, tmp.len)) < 0)
		return -ERR_EXTERN_UNKNOWN;

	xdef = lib_extern_get(microjs->libdef, xid);

	if (EXTDEF_TYPE(xdef) != O_OBJECT)
		return -ERR_EXTERN_NOT_OBJECT;

	if (!EXTDEF_FLAG(xdef, O_ARRAY))
		return -ERR_EXTERN_NOT_ARRAY;

	if (EXTDEF_FLAG(xdef, O_SIZEOFFS)) {
		DCC_LOG1(LOG_INFO, "array object xid=%d (size and offs)", xid);
		/* Array of objects with size and offset */
		cld.cid = xdef->aos.cdef;
		if (xdef->aos.size > 1) { /* multiply the index by the size */
			encode_int(microjs, xdef->aos.size);
			TRACEF("%04x\tMUL\n", microjs->pc);
			microjs->code[microjs->pc++] = OPC_MUL;
		}
		if (xdef->aos.offs != 0) { /* add the offset */
			encode_int(microjs, xdef->aos.offs);
			TRACEF("%04x\tADD\n", microjs->pc);
			microjs->code[microjs->pc++] = OPC_ADD;
		}
	} else {
		DCC_LOG1(LOG_INFO, "array object xid=%d (xlat call)", xid);
		/* Array of objects with index translator call */
		cld.cid = xdef->ao.cdef;
		/* XXX: the index translator has to accept a single argument 
		   and return 1 value. This call could be optimized,
		 since the stack size is allways one at the entry end exit! */
		TRACEF("%04x\tEXT \'%s\" %d\n", microjs->pc, 
			   sym_extern_name(microjs->libdef, xid), 1);
		microjs->code[microjs->pc++] = OPC_EXT;
		microjs->code[microjs->pc++] = xid; /* external call number */
		microjs->code[microjs->pc++] = 1; /* stack size (arguments) */
	}

	/* push  the class definition */
	return sym_cld_push(microjs->tab, &cld);
}

/* --------------------------------------------------------------------------
   External object evaluation and assignement
   -------------------------------------------------------------------------- */

#if 0
int op_object_eval(struct microjs_sdt * microjs)
{
	return -ERR_NOT_IMPLEMENTED;
}

int op_object_assign(struct microjs_sdt * microjs)
{
	return -ERR_NOT_IMPLEMENTED;
}
#endif

/* --------------------------------------------------------------------------
   External Object attribute evaluation and assignement
   -------------------------------------------------------------------------- */

static int class_member_pop(struct microjs_sdt * microjs, 
							struct extdef ** xdefp)
{
	struct extdef * xdef;
	struct sym_cld cld;
	struct sym_tmp tmp;
	int ret;
	int xid;

	/* get the member attribute name */
	if ((ret = sym_tmp_pop(microjs->tab, &tmp)) < 0)
		return ret;

	/* get the class definition */
	if ((ret = sym_cld_pop(microjs->tab, &cld)) < 0)
		return ret;

	DCC_LOG2(LOG_INFO, "class[%d]: '%s'", cld.cid,
			 lib_class_name(microjs->libdef, cld.cid));

	if ((xid = lib_member_lookup(microjs->libdef, cld.cid, 
								 tmp.s, tmp.len)) < 0)
		return -ERR_EXTERN_UNKNOWN;

	xdef = lib_extern_get(microjs->libdef, xid);

	DCC_LOG2(LOG_INFO, "member[%d]: '%s'", xid, xdef->nm);

	if (!EXTDEF_FLAG(xdef, O_MEMBER))
		return -ERR_EXTERN_NOT_MEMBER;

	*xdefp = xdef;

	return xid;
}

int op_attr_eval(struct microjs_sdt * microjs)
{
	struct extdef * xdef;
	int xid;

	DCC_LOG(LOG_INFO, "...");

	if ((xid = class_member_pop(microjs, &xdef)) < 0)
		return xid;

	if (EXTDEF_TYPE(xdef) != O_INTEGER)
		return -ERR_EXTERN_NOT_INTEGER;

	if (EXTDEF_FLAG(xdef, O_ARRAY))
		return -ERR_EXTERN_NOT_SCALAR;

	/* XXX: This call could be optimized, attribute eval functions,
	   should receive 1 prameter: (object_id) and return the 
	   attribute value. */
	TRACEF("%04x\tEXT \'%s\" %d\n", microjs->pc, 
		   sym_extern_name(microjs->libdef, xid), 1);
	microjs->code[microjs->pc++] = OPC_EXT;
	microjs->code[microjs->pc++] = xid; /* external call number */
	microjs->code[microjs->pc++] = 1; /* stack size (arguments) */

	return 0;
}

int op_array_eval(struct microjs_sdt * microjs)
{
	struct extdef * xdef;
	int xid;

	DCC_LOG(LOG_INFO, "...");

	if ((xid = class_member_pop(microjs, &xdef)) < 0)
		return xid;

	if (EXTDEF_TYPE(xdef) != O_INTEGER)
		return -ERR_EXTERN_NOT_INTEGER;

	if (!EXTDEF_FLAG(xdef, O_ARRAY))
		return -ERR_EXTERN_NOT_ARRAY;

	/* XXX: This call could be optimized, attribute eval functions,
	   should receive 2 prameter: (object_id, array_idx) and return the 
	   attribute value. */
	TRACEF("%04x\tEXT \'%s\" %d\n", microjs->pc, 
		   sym_extern_name(microjs->libdef, xid), 1);
	microjs->code[microjs->pc++] = OPC_EXT;
	microjs->code[microjs->pc++] = xid; /* external call number */
	microjs->code[microjs->pc++] = 2; /* stack size (arguments) */

	return 0;
}


int op_attr_assign(struct microjs_sdt * microjs)
{
	struct extdef * xdef;
	int xid;

	DCC_LOG(LOG_INFO, "...");

	if ((xid = class_member_pop(microjs, &xdef)) < 0)
		return xid;

	if (EXTDEF_TYPE(xdef) != O_INTEGER)
		return -ERR_EXTERN_NOT_INTEGER;

	if (EXTDEF_FLAG(xdef, O_READONLY))
		return -ERR_EXTERN_READONLY;

	if (EXTDEF_FLAG(xdef, O_ARRAY))
		return -ERR_EXTERN_NOT_SCALAR;

	DCC_LOG1(LOG_INFO, "xid=%d.", xid);

	/* XXX: This call could be optimized, attribute assignement functions,
	   should receive 2 prameters: (object_id, attr_val) and return nothing */
	TRACEF("%04x\tEXT \'%s\" %d\n", microjs->pc, 
		   sym_extern_name(microjs->libdef, xid), 1);
	microjs->code[microjs->pc++] = OPC_EXT;
	microjs->code[microjs->pc++] = xid; /* external call number */
	microjs->code[microjs->pc++] = 2; /* stack size (arguments) */

	/* the call will consume the 2 values on the stack and 
	   will not push back any value, inform the stack evaluator. */ 
	return tgt_stack_adjust(microjs, -2);
}

int op_array_assign(struct microjs_sdt * microjs)
{
	struct extdef * xdef;
	int xid;

	DCC_LOG(LOG_INFO, "...");

	if ((xid = class_member_pop(microjs, &xdef)) < 0)
		return xid;

	if (EXTDEF_TYPE(xdef) != O_INTEGER)
		return -ERR_EXTERN_NOT_INTEGER;

	if (EXTDEF_FLAG(xdef, O_READONLY))
		return -ERR_EXTERN_READONLY;

	if (!EXTDEF_FLAG(xdef, O_ARRAY))
		return -ERR_EXTERN_NOT_ARRAY;

	/* XXX: This call could be optimized, attribute assignement functions,
	   should receive 2 prameters: (object_id, array_idx, attr_val) 
	   and return nothing */
	TRACEF("%04x\tEXT \'%s\" %d\n", microjs->pc, 
		   sym_extern_name(microjs->libdef, xid), 1);
	microjs->code[microjs->pc++] = OPC_EXT;
	microjs->code[microjs->pc++] = xid; /* external call number */
	microjs->code[microjs->pc++] = 3; /* stack size (arguments) */

	/* the call will consume the 3 values from the stack and 
	   will not push back any value, inform the stack evaluator. */ 
	return tgt_stack_adjust(microjs, -3);
}

/* --------------------------------------------------------------------------
   Object method calls
   -------------------------------------------------------------------------- */

int op_method_lookup(struct microjs_sdt * microjs)
{
	struct extdef * xdef;
	struct sym_call call;
	int xid;

	/* get the class member */
	if ((xid = class_member_pop(microjs, &xdef)) < 0)
		return xid;

	if (EXTDEF_TYPE(xdef) != O_FUNCTION)
		return -ERR_EXTERN_NOT_FUNCTION;

	/* prepare to call a method */
	call.xid = xid;
	call.retcnt = xdef->f.ret;
	call.argmin = xdef->f.argmin;
	call.argmax = xdef->f.argmax;
	call.argcnt = 1;

	return sym_call_push(microjs->tab, &call);
}

/* --------------------------------------------------------------------------
   External Function calls
   -------------------------------------------------------------------------- */

int op_function_lookup(struct microjs_sdt * microjs)
{
	struct extdef * xdef;
	struct sym_call call;
	struct sym_tmp tmp;
	int xid;
	int ret;

	if ((ret = sym_tmp_pop(microjs->tab, &tmp)) < 0)
		return ret;

	if ((xid = lib_lookup(microjs->libdef, tmp.s, tmp.len)) < 0)
		return -ERR_EXTERN_UNKNOWN;

	xdef = lib_extern_get(microjs->libdef, xid);

	if (EXTDEF_TYPE(xdef) != O_FUNCTION)
		return -ERR_EXTERN_NOT_FUNCTION;

	/* prepare to call a function */
	call.xid = xid;
	call.retcnt = xdef->f.ret;
	call.argmin = xdef->f.argmin;
	call.argmax = xdef->f.argmax;
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

#if MICROJS_OPTIMIZATION_ENABLED
	microjs->spc = microjs->pc; /* save code pointer */
#endif
	TRACEF("%04x\tEXT \'%s\" %d\n", microjs->pc, 
		   sym_extern_name(microjs->libdef, call.xid), call.argcnt);
	microjs->code[microjs->pc++] = OPC_EXT;
	microjs->code[microjs->pc++] = call.xid; /* external call number */
	microjs->code[microjs->pc++] = call.argcnt; /* stack size */

	if ((ret = tgt_stack_adjust(microjs, call.retcnt - call.argcnt)) < 0)
		return ret;

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
#if MICROJS_OPTIMIZATION_ENABLED
		microjs->spc = microjs->pc; /* save code pointer */
#endif
		if (n > 1) {
			TRACEF("%04x\tISP %d\n", microjs->pc, n);
			microjs->code[microjs->pc++] = OPC_ISP;
			microjs->code[microjs->pc++] = n;
		} else {
			TRACEF("%04x\tPOP\n", microjs->pc);
			microjs->code[microjs->pc++] = OPC_POP;
		}
	}

	return tgt_stack_adjust(microjs, -call.retcnt);
}

int op_call_ret(struct microjs_sdt * microjs)
{
	struct sym_call call;
	int ret;
	int n;

	if ((ret = sym_call_pop(microjs->tab, &call)) < 0)
		return ret;

	DCC_LOG2(LOG_INFO, "argcnt=%d, retcnt=%d", call.argcnt, call.retcnt);
	if (call.retcnt < 1) {
		return -ERR_RET_COUNT_MISMATCH;
	}

	/* discard all but one of the returning values */
	if ((n = call.retcnt - 1) > 0) {
#if MICROJS_OPTIMIZATION_ENABLED
		microjs->spc = microjs->pc; /* save code pointer */
#endif
		if (n > 1) {
			TRACEF("%04x\tISP %d\n", microjs->pc, n);
			microjs->code[microjs->pc++] = OPC_ISP;
			microjs->code[microjs->pc++] = n;
		} else {
			TRACEF("%04x\tPOP\n", microjs->pc);
			microjs->code[microjs->pc++] = OPC_POP;
		}
	
		if ((ret = tgt_stack_adjust(microjs, -n)) < 0)
			return ret;

	} else if (n < 0) {
		return -ERR_GENERAL;
	}

	return 0;
}

/* --------------------------------------------------------------------------
   Binary operations 
   -------------------------------------------------------------------------- */

int op_equ(struct microjs_sdt * microjs)
{
#if MICROJS_OPTIMIZATION_ENABLED
	int32_t val;
	if (is_constant(microjs, microjs->spc, &val) & (val == 0)) {
		microjs->pc = microjs->spc; /* rollback */
		TRACEF("%04x\tNOT (optimizing EQ 0)\n", microjs->pc);
		microjs->code[microjs->pc++] = OPC_NOT;
		return 0;
	} 
	microjs->spc = microjs->pc; /* save code pointer */
#endif
	TRACEF("%04x\tEQ\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_EQ;

	return tgt_stack_pop(microjs);
}

int op_neq(struct microjs_sdt * microjs)
{
#if MICROJS_OPTIMIZATION_ENABLED
	microjs->spc = microjs->pc; /* save code pointer */
#endif
	TRACEF("%04x\tNE\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_NE;
	return tgt_stack_pop(microjs);
}

int op_gt(struct microjs_sdt * microjs)
{
#if MICROJS_OPTIMIZATION_ENABLED
	microjs->spc = microjs->pc; /* save code pointer */
#endif
	TRACEF("%04x\tGT\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_GT;
	return tgt_stack_pop(microjs);
}

int op_lt(struct microjs_sdt * microjs)
{
#if MICROJS_OPTIMIZATION_ENABLED
	microjs->spc = microjs->pc; /* save code pointer */
#endif
	TRACEF("%04x\tLT\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_LT;
	return tgt_stack_pop(microjs);
}

int op_gte(struct microjs_sdt * microjs)
{
#if MICROJS_OPTIMIZATION_ENABLED
	microjs->spc = microjs->pc; /* save code pointer */
#endif
	TRACEF("%04x\tGE\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_GE;
	return tgt_stack_pop(microjs);
}

int op_lte(struct microjs_sdt * microjs)
{
#if MICROJS_OPTIMIZATION_ENABLED
	microjs->spc = microjs->pc; /* save code pointer */
#endif
	TRACEF("%04x\tLE\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_LE;
	return tgt_stack_pop(microjs);
}

int op_logic_or(struct microjs_sdt * microjs)
{
#if MICROJS_OPTIMIZATION_ENABLED
	microjs->spc = microjs->pc; /* save code pointer */
#endif
	TRACEF("%04x\tOR\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_OR;
	return tgt_stack_pop(microjs);
}

int op_logic_and(struct microjs_sdt * microjs)
{
#if MICROJS_OPTIMIZATION_ENABLED
	microjs->spc = microjs->pc; /* save code pointer */
#endif
	TRACEF("%04x\tAND\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_AND;
	return tgt_stack_pop(microjs);
}

int op_add(struct microjs_sdt * microjs)
{
#if MICROJS_OPTIMIZATION_ENABLED
	int32_t val;
	if (is_constant(microjs, microjs->spc, &val) & (val == 1)) {
		microjs->pc = microjs->spc; /* rollback */
		TRACEF("%04x\tINC (optimizing ADD)\n", microjs->pc);
		microjs->code[microjs->pc++] = OPC_INC;
		/* the INC operation do change the stack but we have to remove the
		   constant pushed to the stack that was optimized away */
		return tgt_stack_pop(microjs);
	} 
	microjs->spc = microjs->pc; /* save code pointer */
#endif
	TRACEF("%04x\tADD\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_ADD;
	return tgt_stack_pop(microjs);
}

int op_sub(struct microjs_sdt * microjs)
{
#if MICROJS_OPTIMIZATION_ENABLED
	int32_t val;
	if (is_constant(microjs, microjs->spc, &val) & (val == 1)) {
		microjs->pc = microjs->spc; /* rollback */
		TRACEF("%04x\tDEC (optimizing SUB)\n", microjs->pc);
		microjs->code[microjs->pc++] = OPC_DEC;
		return tgt_stack_pop(microjs);
	} 
	microjs->spc = microjs->pc; /* save code pointer */
#endif
	TRACEF("%04x\tSUB\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_SUB;
	return tgt_stack_pop(microjs);
}

int op_or(struct microjs_sdt * microjs)
{
#if MICROJS_OPTIMIZATION_ENABLED
	microjs->spc = microjs->pc; /* save code pointer */
#endif
	TRACEF("%04x\tOR\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_OR;
	return tgt_stack_pop(microjs);
}

int op_xor(struct microjs_sdt * microjs)
{
#if MICROJS_OPTIMIZATION_ENABLED
	microjs->spc = microjs->pc; /* save code pointer */
#endif
	TRACEF("%04x\tXOR\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_XOR;
	return tgt_stack_pop(microjs);
}

int op_mul(struct microjs_sdt * microjs)
{
#if MICROJS_OPTIMIZATION_ENABLED
	microjs->spc = microjs->pc; /* save code pointer */
#endif
	TRACEF("%04x\tMUL\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_MUL;
	return tgt_stack_pop(microjs);
}

int op_div(struct microjs_sdt * microjs)
{
#if MICROJS_OPTIMIZATION_ENABLED
	microjs->spc = microjs->pc; /* save code pointer */
#endif
	TRACEF("%04x\tDIV\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_DIV;
	return tgt_stack_pop(microjs);
}

int op_mod(struct microjs_sdt * microjs)
{
#if MICROJS_OPTIMIZATION_ENABLED
	microjs->spc = microjs->pc; /* save code pointer */
#endif
	TRACEF("%04x\tMOD\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_MOD;
	return tgt_stack_pop(microjs);
}

int op_and(struct microjs_sdt * microjs)
{
#if MICROJS_OPTIMIZATION_ENABLED
	microjs->spc = microjs->pc; /* save code pointer */
#endif
	TRACEF("%04x\tAND\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_AND;
	return tgt_stack_pop(microjs);
}

int op_shl(struct microjs_sdt * microjs)
{
#if MICROJS_OPTIMIZATION_ENABLED
	microjs->spc = microjs->pc; /* save code pointer */
#endif
	TRACEF("%04x\tSHL\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_SHL;
	return tgt_stack_pop(microjs);
}

int op_asr(struct microjs_sdt * microjs)
{
#if MICROJS_OPTIMIZATION_ENABLED
	microjs->spc = microjs->pc; /* save code pointer */
#endif
	TRACEF("%04x\tASR\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_ASR;
	return tgt_stack_pop(microjs);
}

/* --------------------------------------------------------------------------
   Unary operations 
   -------------------------------------------------------------------------- */

int op_inv(struct microjs_sdt * microjs)
{
#if MICROJS_OPTIMIZATION_ENABLED
	microjs->spc = microjs->pc; /* save code pointer */
#endif
	TRACEF("%04x\tINV\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_INV;
	return 0;
}

int op_minus(struct microjs_sdt * microjs)
{
#if MICROJS_OPTIMIZATION_ENABLED
	int32_t val;
	if (is_constant(microjs, microjs->spc, &val)) {
		/* rollback */
		microjs->pc = microjs->spc;
		TRACEF("%04x\tNEG (optimizing %d -> %d)\n", microjs->pc, val, -val);
		return encode_int(microjs, -val);
	} 

	microjs->spc = microjs->pc; /* save code pointer */
#endif
	TRACEF("%04x\tNEG\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_NEG;
	return 0;
}

int op_not(struct microjs_sdt * microjs)
{
#if MICROJS_OPTIMIZATION_ENABLED
	microjs->spc = microjs->pc; /* save code pointer */
#endif
	TRACEF("%04x\tNOT\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_NOT;
	return 0;
}

/* --------------------------------------------------------------------------
   Constants 
   -------------------------------------------------------------------------- */

int op_push_false(struct microjs_sdt * microjs)
{
#if MICROJS_OPTIMIZATION_ENABLED
	microjs->spc = microjs->pc; /* save code pointer */
#endif
	TRACEF("%04x\tI4 %d\n", microjs->pc, 0);
	microjs->code[microjs->pc++] = OPC_I4 + 0;
	return tgt_stack_push(microjs);
}

int op_push_true(struct microjs_sdt * microjs)
{
#if MICROJS_OPTIMIZATION_ENABLED
	microjs->spc = microjs->pc; /* save code pointer */
#endif
	TRACEF("%04x\tI4 %d\n", microjs->pc, 1);
	microjs->code[microjs->pc++] = OPC_I4 + 1;
	return tgt_stack_push(microjs);
}

int op_push_string(struct microjs_sdt * microjs) 
{
#if MICROJS_STRINGS_ENABLED
	int isz;

	if ((isz = cstr_add(microjs->tok.s, microjs->tok.qlf)) < 0) {
		fprintf(stderr, "can't create string!\n");
		/* FIXME: more specific error */
		return isz;
	}

#if MICROJS_OPTIMIZATION_ENABLED
	microjs->spc = microjs->pc; /* save code pointer */
#endif
	if (isz < 4) {
		TRACEF("%04x\tI4 %d\n", microjs->pc, isz);
		microjs->code[microjs->pc++] = OPC_I4 + isz;
	} else {
		TRACEF("%04x\tI8 %d\n", microjs->pc, isz);
		microjs->code[microjs->pc++] = OPC_I8;
		microjs->code[microjs->pc++] = isz;
	}

	return tgt_stack_push(microjs);
#else
	return -ERR_STRINGS_UNSUPORTED;
#endif
}

int op_push_int(struct microjs_sdt * microjs)
{
	encode_int(microjs, (int32_t)microjs->tok.u32);
	return tgt_stack_push(microjs);
}

/* --------------------------------------------------------------------------
   Commom backpatch helper
   -------------------------------------------------------------------------- */

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

/* --------------------------------------------------------------------------
   Exception handling
   -------------------------------------------------------------------------- */

int op_try_begin(struct microjs_sdt * microjs) 
{
	struct sym_ref ref;
	int ret;

#if MICROJS_OPTIMIZATION_ENABLED
	microjs->spc = microjs->pc; /* save code pointer */
#endif
	/* save current location on a temporary reference */
#if MICROJS_TRACE_ENABLED
	ref.lbl = sym_lbl_next(microjs->tab);
#endif
	ref.addr = microjs->pc;
	TRACEF(".L%d:\n%04x\tPUSHX xxxx\n", ref.lbl, microjs->pc);
	/* Insert the opcode, the address will be backpatched alter */
	microjs->code[microjs->pc] = OPC_PUSHX;
	microjs->pc += 2;

	if ((ret = tgt_stack_push(microjs)) < 0)
		return ret;

	return sym_ref_push(microjs->tab, &ref);
}

int op_try_end(struct microjs_sdt * microjs) 
{
	struct sym_ref ref;
	int offs;
	int ret;

	/* get the temporary reference */
	if ((ret = sym_ref_pop(microjs->tab, &ref)) < 0)
		return ret;

#if MICROJS_OPTIMIZATION_ENABLED
	microjs->spc = microjs->pc; /* save code pointer */
#endif

	offs = 1; /* skip the exception rethrow */
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

	return tgt_stack_pop(microjs);
}

int op_catch(struct microjs_sdt * microjs) 
{
	struct sym_ref ref1;
	struct sym_ref ref2;
	int ret;

	/* get the try exception frame reference */
	if ((ret = sym_ref_pop(microjs->tab, &ref1)) < 0)
		return ret;

#if MICROJS_OPTIMIZATION_ENABLED
	microjs->spc = microjs->pc; /* save code pointer */
#endif

	/* save current location */
#if MICROJS_TRACE_ENABLED
	ref2.lbl = sym_lbl_next(microjs->tab);
#endif
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

int op_throw(struct microjs_sdt * microjs) 
{
#if MICROJS_OPTIMIZATION_ENABLED
	microjs->spc = microjs->pc; /* save code pointer */
#endif
	TRACEF("%04x\tXPT\n", microjs->pc);
	microjs->code[microjs->pc++] = OPC_XPT;
	return 0;
}

/* --------------------------------------------------------------------------
   "if {} else {}" construct
   -------------------------------------------------------------------------- */

int op_if_cond(struct microjs_sdt * microjs)
{
	struct sym_ref ref;

#if MICROJS_OPTIMIZATION_ENABLED
	microjs->spc = microjs->pc; /* save code pointer */
#endif
	/* save current location on a temporary variable */
	ref.addr = microjs->pc;
#if MICROJS_TRACE_ENABLED
	ref.lbl = sym_lbl_next(microjs->tab);
#endif
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

#if MICROJS_OPTIMIZATION_ENABLED
	microjs->spc = microjs->pc; /* save code pointer */
#endif
	/* Backpatch the jump */
	offs = microjs->pc - ref.addr;
	TRACEF("\tfix %04x -> JEQ %04x (.L%d)\n", ref.addr, 
		   microjs->pc + 2, ref.lbl);
	microjs->code[ref.addr ] += offs & 0x0f;
	microjs->code[ref.addr + 1] = offs >> 4;

	/* save current location on the same temporary reference */
#if MICROJS_TRACE_ENABLED
	ref.lbl = sym_lbl_next(microjs->tab);
#endif
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
#if 0
	wld.brk = 0;
	wld.ctn = 0;
#endif
#if MICROJS_TRACE_ENABLED
	wld.lbl = sym_lbl_next(microjs->tab);
#endif
	TRACEF(".L%d.0:\n", wld.lbl);

	return sym_wld_push(microjs->tab, &wld);
}

int op_while_cond(struct microjs_sdt * microjs)
{
	struct sym_wld wld;
	int ret;

	if ((ret = sym_wld_pop(microjs->tab, &wld)) < 0)
		return ret;

#if MICROJS_OPTIMIZATION_ENABLED
	microjs->spc = microjs->pc; /* save code pointer */
#endif
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

#if MICROJS_OPTIMIZATION_ENABLED
	microjs->spc = microjs->pc; /* save code pointer */
#endif
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
#if MICROJS_TRACE_ENABLED
	fld.lbl = sym_lbl_next(microjs->tab); /* set a label (for debugging only) */
#endif
#if 0
	fld.brk = 0;
	fld.ctn = 0;
#endif
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

#if MICROJS_OPTIMIZATION_ENABLED
	microjs->spc = microjs->pc; /* save code pointer */
#endif
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

#if MICROJS_OPTIMIZATION_ENABLED
	microjs->spc = microjs->pc; /* save code pointer */
#endif
	addr = fld.addr[0]; /* Repeat jump */
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

#if MICROJS_OPTIMIZATION_ENABLED
	microjs->spc = microjs->pc; /* save code pointer */
#endif
	addr = fld.addr[1]; /* adjust the conditinal jump */
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
 	[ACTION(A_OP_PUSH_TMP)] = op_push_tmp,
 	[ACTION(A_OP_POP_TMP)] = op_pop_tmp,
 	[ACTION(A_OP_VAR_DECL)] = op_var_decl,
 	[ACTION(A_OP_VAR_EVAL)] = op_var_eval,
 	[ACTION(A_OP_VAR_ASSIGN)] = op_var_assign,
 	[ACTION(A_OP_ARRAY_XLAT)] = op_array_xlat,
 	[ACTION(A_OP_OBJECT_GET)] = op_object_get,
#if 0
 	[ACTION(A_OP_OBJECT_EVAL)] = op_object_eval,
 	[ACTION(A_OP_OBJECT_ASSIGN)] = op_object_assign,
#endif
 	[ACTION(A_OP_ATTR_EVAL)] = op_attr_eval,
 	[ACTION(A_OP_ARRAY_EVAL)] = op_array_eval,
 	[ACTION(A_OP_ATTR_ASSIGN)] = op_attr_assign,
 	[ACTION(A_OP_ARRAY_ASSIGN)] = op_array_assign,
 	[ACTION(A_OP_METHOD_LOOKUP)] = op_method_lookup,
 	[ACTION(A_OP_FUNCTION_LOOKUP)] = op_function_lookup,
 	[ACTION(A_OP_ARG)] = op_arg,
 	[ACTION(A_OP_CALL)] = op_call,
 	[ACTION(A_OP_CALL_RET)] = op_call_ret,
 	[ACTION(A_OP_RET_DISCARD)] = op_ret_discard,
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
 	[ACTION(A_OP_PUSH_STRING)] = op_push_string,
 	[ACTION(A_OP_BLK_OPEN)] = op_blk_open,
 	[ACTION(A_OP_BLK_CLOSE)] = op_blk_close,
 	[ACTION(A_OP_TRY_BEGIN)] = op_try_begin,
 	[ACTION(A_OP_TRY_END)] = op_try_end,
 	[ACTION(A_OP_CATCH)] = op_catch,
 	[ACTION(A_OP_CATCH_END)] = op_patch_ref,
 	[ACTION(A_OP_THROW)] = op_throw,
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
		DCC_LOG1(LOG_MSG, "<%d>", sym);
#endif
		if IS_A_TERMINAL(sym) {
			/* terminal */
			if (sym != lookahead) {
				err = -ERR_UNEXPECTED_SYMBOL;
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
				DCC_LOG(LOG_INFO, "syntax action failed!");
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
				DCC_LOG2(LOG_INFO, "sym=%d lookahed=%d", sym, lookahead);
				/* push the offending symbol back onto the stack */	
				ll_sp--;
				err = (lookahead == T_EOF) ? -ERR_UNEXPECED_EOF :
					-ERR_SYNTAX_ERROR;
#if MICROJS_TRACE_ENABLED
				ll_stack_dump(stderr, ll_sp, ll_top);
#endif
				goto error;
			}
			ll_sp -= k;
			if (ll_sp < ll_sl) {
				DCC_LOG(LOG_WARNING, "stack overflow!");
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

int microjs_sdt_begin(struct microjs_sdt * microjs, uint8_t code[], 
					  unsigned int code_size)
{
	struct sym_ref ref;

	/* code memory */
	microjs->pc = 0;
	microjs->cdsz = code_size;
	microjs->code = code; /* code buffer */

	microjs_sdt_reset(microjs);

	/* generate the default exception handler */
	if (symtab_isempty(microjs->tab)) {
		/* save current location on a temporary reference */
#if MICROJS_TRACE_ENABLED
		ref.lbl = 0;
#endif
		ref.addr = microjs->pc;
		sym_ref_push(microjs->tab, &ref);
	} else { 
		/* get the default exception handler */
		sym_pick(microjs->tab, 0, &ref, sizeof(struct sym_ref));
	}

#if MICROJS_OPTIMIZATION_ENABLED
	microjs->spc = microjs->pc;        /* save code pointer */
#endif

	TRACEF(".L%d:\n%04x\tPUSHX xxxx\n", ref.lbl, microjs->pc);
	/* Insert the opcode, the address will be backpatched alter */
	microjs->code[microjs->pc] = OPC_PUSHX;
	microjs->pc += 2;

	/* the exception frame goes into the stack, record this */
	return tgt_stack_push(microjs);
}

int microjs_sdt_end(struct microjs_sdt * microjs)
{
	struct sym_ref ref;
	int ret;

	/* we need 3 more bytes to encode the program end */
	if (microjs->pc + 3 > microjs->cdsz)
		return -ERR_CODE_MEM_OVERFLOW;

	if ((ret = tgt_stack_pop(microjs)) < 0)
		return ret;

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
									  const struct ext_libdef * libdef)

{
	struct microjs_sdt * microjs = (struct microjs_sdt *)sdt_buf;
	int data_sz;

	microjs->tab = tab;
	microjs->libdef = libdef;

	data_sz = symtab_data_size(tab);
	if (data_sz > tab->rt.data_sz) {
		DCC_LOG(LOG_ERROR, "Symbol table invalid!");
		DCC_LOG2(LOG_TRACE, "data_sz(%d) > tab->rt.data_sz(%d)", 
				 data_sz, tab->rt.data_sz);
		return NULL;
	}

	/* data memory allocation info */
	microjs->data_pos = data_sz;
	microjs->stack_pos = 0; /* initial stack position */

	/* size of the buffer provided for parsing */
	microjs->size = sdt_size;
	microjs->ll_sp = sdt_size;

	return microjs;
}

