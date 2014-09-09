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
   Symbol table
   -------------------------------------------------------------------------- */
struct sym_tab {
	unsigned int cnt;
	unsigned int len;
	char * buf;
	uint16_t offs[256];
	int16_t addr[256];
};

void sym_tab_init(struct sym_tab * tab, char * buf, unsigned int len)
{
	tab->buf = buf;
	tab->len = len;
	/* Add null string */
	buf[0] = '\0';
	tab->offs[0] = 0;
	tab->addr[0] = -1;
	tab->cnt = 1;
}

int sym_lookup(struct sym_tab * tab, const char * s, unsigned int len)
{
	char *buf = (char *)tab->buf;
	int i;

	for (i = 0; i < tab->cnt; ++i) {
		char * cstr = buf + tab->offs[i];
		if ((strncmp(cstr, s, len) == 0) && (strlen(cstr) == len))
			return i;
	}

	return -1;
}

int sym_add(struct sym_tab * tab, const char * s, unsigned int len)
{
	char * dst;
	int idx;
	int offs;

	if ((idx = sym_lookup(tab, s, len)) >= 0) {
		return idx;
	}

	offs = (tab->cnt > 0) ? tab->offs[tab->cnt - 1] : 0;
	dst = tab->buf + offs;
	dst += strlen(dst) + 1;

	idx = tab->cnt++;
	/* NULL terminate the string */
	memcpy(dst, s, len);
	dst[len] = '\0';
	tab->offs[idx] = dst - tab->buf;

	return idx;
}

const char * sym_name(struct sym_tab * tab, int idx)
{
	return tab->buf + tab->offs[idx];
}

int sym_addr_get(struct sym_tab * tab, int idx)
{
	return tab->addr[idx];
}

void sym_addr_set(struct sym_tab * tab, int idx, int addr)
{
	tab->addr[idx] = addr;
}

int alloc32(struct calc * calc)
{
	unsigned int addr;

	/* ensure memory alignment */
	addr = (calc->heap + 3) & ~3;
	calc->heap = addr + 4;

	if (calc->heap > calc->sp) {
		fprintf(stderr, "heap overflow!\n");
		return 0;
	}

	return addr;
}

/* --------------------------------------------------------------------------
   Memory operations
   -------------------------------------------------------------------------- */

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

/* --------------------------------------------------------------------------
   Code generation
   -------------------------------------------------------------------------- */

int op_var_decl(struct calc * calc)
{
	int id;
	int addr;

	id = sym_add(calc->tab, calc->tok.s, calc->tok.qlf);

	if (id < 0) {
		fprintf(stderr, "can't create symbol.\n");
		return -1;
	}

	if ((addr = alloc32(calc)) < 0) {
		return -1;
	}

	/* bind address to symol */
	sym_addr_set(calc->tab, id, addr);

	return 0;
}


int op_print(struct calc * calc)
{
	printf("%04x\tPRINT\n", calc->pc);
	calc->code[calc->pc++] = OPC_PRINT;
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

int op_lookup_id(struct calc * calc)
{
	int addr;
	int id;

	id = sym_lookup(calc->tab, calc->tok.s, calc->tok.qlf);

	if (id < 0) {
		fprintf(stderr, "undefined symbol: %s.\n", calc->tok.s);
		return -1;
	}

	addr = sym_addr_get(calc->tab, id);
	printf("%04x\tI16 0x%04x\n", calc->pc, addr);

	calc->code[calc->pc++] = OPC_I16;
	calc->code[calc->pc++] = addr;
	calc->code[calc->pc++] = addr >> 8;

	return 0;
}

int op_push_id(struct calc * calc)
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

int op_while_begin(struct calc * calc)
{
	char s[8];
	int n;
	int id;

	n = sprintf(s, ".L%d", calc->nest++);
	if ((id = sym_add(calc->tab, s, n)) < 0) {
		fprintf(stderr, "can't create symbol.\n");
		return id;
	}

	/* save current location on a temporary variable */
	sym_addr_set(calc->tab, id, calc->pc);
	printf("%s:\n", s);
	return 0;
}

int op_while_cond(struct calc * calc)
{
	char s[8];
	int n;
	int id;

	n = sprintf(s, ".L%d", calc->nest++);
	if ((id = sym_add(calc->tab, s, n)) < 0) {
		fprintf(stderr, "can't create symbol.\n");
		return id;
	}

	/* save current location on a temporary variable */
	sym_addr_set(calc->tab, id, calc->pc);
	printf("%s:\n%04x\tJEQ xxxx\n", s, calc->pc);
	/* reserve 3 positions for jump calculation */
	calc->pc += 3;
	return 0;
}


int op_while_end(struct calc * calc)
{
	char s[8];
	int n;
	int id;
	int addr;
	int offs;

	/* Adjust the conditinal jump */
	n = sprintf(s, ".L%d", --calc->nest);
	id = sym_lookup(calc->tab, s, n);
	addr = sym_addr_get(calc->tab, id);
	offs = calc->pc - addr;
	printf("\tfix %04x -> JEQ %04x (%s)\n", addr, calc->pc + 3, s);
	calc->code[addr++] = OPC_JEQ;
	calc->code[addr++] = offs;
	calc->code[addr++] = offs >> 8;

	/* Repeat */
	n = sprintf(s, ".L%d", --calc->nest);
	id = sym_lookup(calc->tab, s, n);
	addr = sym_addr_get(calc->tab, id);
	offs = addr - calc->pc;
	printf("%04x\tJMP %04x (%s offs=%d)\n", calc->pc, addr, s, offs);
	calc->code[calc->pc++] = OPC_JMP;
	calc->code[calc->pc++] = offs;
	calc->code[calc->pc++] = offs >> 8;

	return 0;
}


int (* op[])(struct calc * calc) = {
 	[ACTION(A_OP_VAR_DECL)] = op_var_decl,
 	[ACTION(A_OP_ASSIGN)] = op_assign,
 	[ACTION(A_OP_PRINT)] = op_print,
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
 	[ACTION(A_OP_PUSH_ID)] = op_push_id,
 	[ACTION(A_OP_LOOKUP_ID)] = op_lookup_id,
 	[ACTION(A_OP_WHILE_BEGIN)] = op_while_begin,
 	[ACTION(A_OP_WHILE_COND)] = op_while_cond,
 	[ACTION(A_OP_WHILE_END)] = op_while_end,

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
	calc->nest = 0;

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
				fprintf(stderr, "action(%s) failed!\n", ll_sym_tab[sym]);
				goto error;
			}
		} else {
			/* non terminal */
			if ((k = ll_rule_push(pp_sp, sym, lookahead)) < 0) {
				fprintf(stderr, "ll_rule_push() failed!\n");
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

	return calc->pc;

error:
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
	uint8_t code[128];
	char strings[512];
	struct calc calc; 
	struct calc_vm vm; 
	struct sym_tab sym_tab;

	sym_tab_init(&sym_tab, strings, sizeof(strings));
	calc_init(&calc, &sym_tab, data, sizeof(data));
	calc_vm_init(&vm, data, sizeof(data));

	if (argc > 1) {
		char * script = NULL;
		unsigned int len = 0;
		int i;
		int n;

		for (i = 1; i < argc; ++i) {
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

