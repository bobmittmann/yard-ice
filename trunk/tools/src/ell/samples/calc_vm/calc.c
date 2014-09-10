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

int32_t __rand(void * env, int32_t argv[]) 
{
	return rand();
};

int32_t __isqrt(void * env, int32_t argv[])
{
	uint32_t x = argv[0];
	uint32_t rem = 0;
	uint32_t root = 0;
	int i;

	for (i = 0; i < 16; ++i) {
		root <<= 1;
		rem = ((rem << 2) + (x >> 30));
		x <<= 2;
		root++;
		if (root <= rem) {
			rem -= root;
			root++;
		} else
			root--;
	}

	return root >> 1;
}	

int32_t __ilog2(void * env, int32_t argv[])
{
	const uint8_t log2_debruijn_index[32] = {
		0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8, 
		31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9 };
	int32_t x = argv[0];

	x |= x >> 1; 
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	x = (x >> 1) + 1;
	x = (x * 0x077cb531UL) >> 27;
	return log2_debruijn_index[x];
}	

int32_t (* extern_call[])(void *, int32_t argv[]) = {
	__rand,
	__isqrt,
	__ilog2,
};

struct ext_entry {
	char nm[11];
	uint8_t argc;
};

const struct ext_entry externals[] = {
	{ .nm = "rand", .argc = 0 },
	{ .nm = "sqrt", .argc = 1 },
	{ .nm = "log2", .argc = 1 }
};

int external_lookup(const char * s, unsigned int len)
{
	int i;

	for (i = 0; i < sizeof(externals) / sizeof(struct ext_entry); ++i) {
		const struct ext_entry * e = &externals[i];
		if ((strncmp(e->nm, s, len) == 0) && (e->nm[len] == '\0'))
			return i;
	}

	return -1;
}

/* --------------------------------------------------------------------------
   Simple Strings Table
   -------------------------------------------------------------------------- */

#define STRINGS_MAX 63
#define STRINGS_POOL_SIZE (512 - (2 * (STRINGS_MAX + 1)))

struct str_pool {
	uint16_t cnt;
	uint16_t offs[STRINGS_MAX];
	char buf[STRINGS_POOL_SIZE];
};

void strings_init(struct str_pool * pool)
{
	/* Add null string */
	pool->buf[0] = '\0';
	pool->offs[0] = 0;
	pool->cnt = 1;
}

int str_lookup(struct str_pool * pool, const char * s, unsigned int len)
{
	char *buf = (char *)pool->buf;
	int i;

	for (i = 0; i < pool->cnt; ++i) {
		char * cstr = buf + pool->offs[i];
		if ((strncmp(cstr, s, len) == 0) && (cstr[len] == '\0'))
			return i;
	}

	return -1;
}

int str_add(struct str_pool * pool, const char * s, unsigned int len)
{
	char * dst;
	int idx;
	int offs;

	if ((idx = str_lookup(pool, s, len)) >= 0)
		return idx;

	offs = (pool->cnt > 0) ? pool->offs[pool->cnt - 1] : 0;
	offs += strlen(pool->buf + offs) + 1;

	if ((idx = pool->cnt) > STRINGS_MAX) {
		fprintf(stderr, "Too many strings.\n");
		return -1;
	}

	if ((offs + len + 1) >= STRINGS_POOL_SIZE) {
		fprintf(stderr, "Strings buffer overflow.\n");
		return -1;
	}

	/* Copy the string to the buffer */
	dst = pool->buf + offs;
	memcpy(dst, s, len);
	/* NULL terminate the string */
	dst[len] = '\0';
	pool->offs[idx] = dst - pool->buf;
	pool->cnt++;

	return idx;
}

const char * string(struct str_pool * str, int idx)
{
	return str->buf + str->offs[idx];
}

/* --------------------------------------------------------------------------
   Symbol table
   -------------------------------------------------------------------------- */

#define SYMBOLS_MAX 64

#define SYM_GLOBAL    (1 << 7)

#define SYM_REFERENCE (0x0 << 6)

struct sym {
	uint16_t addr;
	uint16_t size;
	uint8_t name;
	uint8_t flags;
};

#define SYM_OBJECT    (1 << 7)
#define SYM_ALLOC     (1 << 6)

/* object */
struct sym_obj {
	uint16_t addr;
	uint16_t size;
	uint8_t name;
	uint8_t flags;
};

#define SYM_EXTERN	(1 << 6)

/* external function */
struct sym_ext {
	uint16_t addr;
	uint16_t size;
	uint8_t name;
	uint8_t flags;
};

/* object reference */
struct sym_ref {
	uint16_t addr;
	uint16_t size;
	uint8_t oid;
	uint8_t flags;
};

struct sym_tab {
	struct str_pool str;
	uint16_t global;
	uint16_t local;
	struct sym sym[SYMBOLS_MAX];
};

void sym_tab_init(struct sym_tab * tab)
{
	strings_init(&tab->str);
	tab->global = 0;
	tab->local = SYMBOLS_MAX;
}

int sym_by_name(struct sym_tab * tab, const char * s, unsigned int len)
{
	int name;
	int i;

	if ((name = str_lookup(&tab->str, s, len)) < 0)
		return name;

	/* search in the local list */
	for (i = tab->local; i < SYMBOLS_MAX; ++i) {
		if (tab->sym[i].name == name)
			return i;
	}

	/* search in the global list */
	for (i = 0; i < tab->global; ++i) {
		if ((tab->sym[i].flags & SYM_OBJECT) && (tab->sym[i].name == name))
			return i;
	}

	return -1;
}

int sym_dump(FILE * f, struct sym_tab * tab)
{
	int i;

	/* search in the global list */
	for (i = 0; i < tab->global; ++i) {
		struct sym * sp = &tab->sym[i];
		if (sp->flags & SYM_OBJECT) {
			struct sym_obj * obj = (struct sym_obj *)sp;
			fprintf(f, "%04x g O .data   %04x    %s\n", obj->addr,
					obj->size, string(&tab->str, obj->name));
		} else if (sp->flags & SYM_EXTERN) {
			struct sym_ext * ext = (struct sym_ext *)sp;
			fprintf(f, "%04x g F .extern %04x    %s\n", ext->addr,
					ext->size, externals[ext->addr].nm);
		} else {
			struct sym_ref * ref = (struct sym_ref *)sp;
			struct sym_obj * obj = (struct sym_obj *)&tab->sym[ref->oid];
			fprintf(f, "%04x r   .text   %04x -> %s\n", ref->addr,
					ref->size, string(&tab->str, obj->name));
		}
	}

	return 0;
}

int sym_lookup(struct sym_tab * tab, const char * s, unsigned int len)
{
	return sym_by_name(tab, s, len);
}


struct sym_obj * sym_obj_new(struct sym_tab * tab, 
							 const char * s, unsigned int len)
{
	struct sym_obj * obj;
	int nm;
	int id;

	if ((id = sym_by_name(tab, s, len)) >= 0)
		return NULL;

	if (tab->global == tab->local)
		return NULL;

	if ((nm = str_add(&tab->str, s, len)) < 0)
		return NULL;

	obj = (struct sym_obj *)&tab->sym[tab->global++];
	obj->name = nm;
	obj->flags = SYM_OBJECT;
	obj->addr = 0;
	obj->size = 0;

	return obj;
}

struct sym_ref * sym_ref_new(struct sym_tab * tab, int oid)
{
	struct sym_ref * ref;

	if (tab->global == tab->local)
		return NULL;

	ref = (struct sym_ref *)&tab->sym[tab->global++];
	ref->oid = oid;
	ref->flags = 0;
	ref->addr = 0;
	ref->size = 0;

	return ref;
}

const char * sym_ref_name(struct sym_tab * tab, struct sym_ref * ref)
{
	struct sym * sp = &tab->sym[ref->oid];
	if (sp->flags & SYM_OBJECT) {
		return string(&tab->str, ((struct sym_obj *)sp)->name);
	}
	if (sp->flags & SYM_EXTERN) {
		return externals[((struct sym_ref *)sp)->addr].nm;
	}
	return "";
}

int sym_ext_new(struct sym_tab * tab, int exid)
{
	struct sym_ext * ext;

	if (exid < 0)
		return -1;

	if (tab->global == tab->local)
		return -1;

	ext = (struct sym_ext *)&tab->sym[tab->global++];
	ext->addr = exid;
	ext->flags = SYM_EXTERN;
	ext->size = 0;

	return (struct sym *)ext - tab->sym;
}

int sym_ext_id(struct sym_tab * tab, struct sym_ext * ext)
{
	return (struct sym *)ext - tab->sym;
}

int sym_add_local(struct sym_tab * tab, const char * s, unsigned int len)
{
	struct sym * sp;
	int nm;
	int id;

	if (tab->global == tab->local)
		return -1;

	if ((id = sym_by_name(tab, s, len)) >= 0)
		return id;

	if ((nm = str_add(&tab->str, s, len)) < 0)
		return nm;

	id = --tab->local;
	sp = &tab->sym[id];

	sp->name = nm;
	sp->flags = 0;
	sp->addr = 0;

	return id;
}

int sym_anom_push(struct sym_tab * tab)
{
	struct sym * sp;
	int id;

	if (tab->global == tab->local)
		return -1;

	id = --tab->local;
	sp = &tab->sym[id];

	sp->name = 0;
	sp->flags = 0;
	sp->addr = 0;

	return id;
}

int sym_anom_pop(struct sym_tab * tab)
{
	if (SYMBOLS_MAX == tab->local)
		return -1;

	return tab->local++;
}

int sym_anom_get(struct sym_tab * tab, int pos)
{
	if (SYMBOLS_MAX <= (tab->local + pos))
		return -1;

	return tab->local + pos;
}


const char * sym_name(struct sym_tab * tab, int id)
{
	return string(&tab->str, tab->sym[id].name);
}

int sym_addr_get(struct sym_tab * tab, int id)
{
	return tab->sym[id].addr;
}

void sym_addr_set(struct sym_tab * tab, int id, int addr)
{
	tab->sym[id].addr = addr;
}

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
		if (sp->flags & SYM_OBJECT) {
			struct sym_obj * obj = (struct sym_obj *)sp;
			int addr;
			if ((addr = alloc32(calc)) < 0)
				return -1;
			obj->addr = addr;
			obj->flags |= SYM_ALLOC;
		}
	}

	/* search in the global list */
	for (i = 0; i < tab->global; ++i) {
		struct sym_ref * ref = (struct sym_ref *)&tab->sym[i];
		if (!(ref->flags & (SYM_OBJECT | SYM_EXTERN))) {
			struct sym_obj * obj = (struct sym_obj *)&tab->sym[ref->oid];
//			printf("ref=0x%04x --> obj=0x%04x\n", ref->addr, obj->addr);
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
//	int addr;

	if ((obj = sym_obj_new(calc->tab, calc->tok.s, calc->tok.qlf)) == NULL) {
		fprintf(stderr, "can't create symbol.\n");
		return -1;
	}

//	if ((addr = alloc32(calc)) < 0)
//		return -1;

	obj->addr = 0;
	/* initial variables are int */
	obj->size = 4;

	return 0;
}

int op_lookup_id(struct calc * calc)
{
	int id;

	id = sym_lookup(calc->tab, calc->tok.s, calc->tok.qlf);
	if (id < 0) {
		int exid = external_lookup(calc->tok.s, calc->tok.qlf);
		if ((id = sym_ext_new(calc->tab, exid)) < 0) {
			fprintf(stderr, "undefined symbol: %s.\n", tok2str(calc->tok));
			return -1;
		}
	}

	calc->tmp[0] = id;

	return 0;
}

int op_push_id_addr(struct calc * calc)
{
	struct sym_ref * ref;
	int addr;

	ref = sym_ref_new(calc->tab, calc->tmp[0]);

	addr = 0;
	printf("%04x\tI16 \'%s\"\n", calc->pc, sym_ref_name(calc->tab, ref));

//	if (addr < 256) {
	if (0) {
		printf("I8 %d\n", addr);
		calc->code[calc->pc++] = OPC_I8;
		ref->size = 1;
		ref->addr = calc->pc;
		calc->code[calc->pc++] = addr;

	} else {
//		printf("I16 %d\n", addr);
		calc->code[calc->pc++] = OPC_I16;
		ref->size = 2;
		ref->addr = calc->pc;
		calc->code[calc->pc++] = addr;
		calc->code[calc->pc++] = addr >> 8;
	}

	return 0;
}

int op_load_var(struct calc * calc)
{
	printf("%04x\tLD\n", calc->pc);
	calc->code[calc->pc++] = OPC_LD;
	return 0;
}

int op_assign(struct calc * calc)
{
	printf("%04x\tST\n", calc->pc);
	calc->code[calc->pc++] = OPC_ST;
	return 0;
}

int op_print_int(struct calc * calc)
{
	printf("%04x\tPRINTI\n", calc->pc);
	calc->code[calc->pc++] = OPC_PRINT_INT;
	return 0;
}

int op_print_end(struct calc * calc)
{
	printf("%04x\tI8 0x%02x\n", calc->pc, '\n');
	calc->code[calc->pc++] = OPC_I8;
	calc->code[calc->pc++] = '\n';
	printf("%04x\tPRINTC\n", calc->pc);
	calc->code[calc->pc++] = OPC_PRINT_CHAR;
	return 0;
}

int op_print_comma(struct calc * calc)
{
	printf("%04x\tI8 0x%02x\n", calc->pc, ',');
	calc->code[calc->pc++] = OPC_I8;
	calc->code[calc->pc++] = ',';
	printf("%04x\tPRINTC\n", calc->pc);
	calc->code[calc->pc++] = OPC_PRINT_CHAR;

	printf("%04x\tI8 0x%02x\n", calc->pc, ' ');
	calc->code[calc->pc++] = OPC_I8;
	calc->code[calc->pc++] = ' ';
	printf("%04x\tPRINTC\n", calc->pc);
	calc->code[calc->pc++] = OPC_PRINT_CHAR;
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
 	[ACTION(A_OP_PRINT_END)] = op_print_end,
 	[ACTION(A_OP_PRINT_INT)] = op_print_int,
 	[ACTION(A_OP_PRINT_COMMA)] = op_print_comma,
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
 	[ACTION(A_OP_LOAD_VAR)] = op_load_var,
 	[ACTION(A_OP_PUSH_ID_ADDR)] = op_push_id_addr,
 	[ACTION(A_OP_LOOKUP_ID)] = op_lookup_id,
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

