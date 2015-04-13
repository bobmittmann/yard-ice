/* $Id: dcclog.c,v 1.10 2006/09/28 19:31:45 bob Exp $ 
 *
 * File:	tokenizer.c
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <setjmp.h>

#include "config.h"
#include "calc.h"

#define DEBUG 1

#include "debug.h"

int calc_init(struct calc * calc, int32_t stack[], unsigned int size)
{
	calc->sp = stack;
	calc->sf = stack;
	calc->sl = &stack[size / sizeof(uint32_t)];

	return 0;
}

int push(struct calc * calc, int32_t x)
{
	if (calc->sp == calc->sl)
		return -1;

	*calc->sp++ = x;	
	return 0;
}

int32_t pop(struct calc * calc)
{
	if (calc->sp == calc->sf) {
		return 0;
	}

	calc->sp--;
	return calc->sp[0];
}

int op_print(struct calc * calc)
{
	int32_t x = pop(calc);
	printf("%d\n", x);
	return 0;
}

int op_assign(struct calc * calc)
{
	int32_t x = pop(calc);
	printf("%d\n", x);
	return 0;
}

int op_add(struct calc * calc)
{
	int32_t x = pop(calc);
	int32_t y = pop(calc);
	return push(calc, x + y);
}

int op_sub(struct calc * calc)
{
	int32_t x = pop(calc);
	int32_t y = pop(calc);
	return push(calc, x - y);
}

int op_or(struct calc * calc)
{
	int32_t x = pop(calc);
	int32_t y = pop(calc);
	return push(calc, x | y);
}

int op_xor(struct calc * calc)
{
	int32_t x = pop(calc);
	int32_t y = pop(calc);
	return push(calc, x ^ y);
}

int op_mul(struct calc * calc)
{
	int32_t x = pop(calc);
	int32_t y = pop(calc);
	return push(calc, x * y);
}

int op_div(struct calc * calc)
{
	int32_t x = pop(calc);
	int32_t y = pop(calc);
	return push(calc, x / y);
}

int op_mod(struct calc * calc)
{
	int32_t x = pop(calc);
	int32_t y = pop(calc);
	return push(calc, x % y);
}

int op_and(struct calc * calc)
{
	int32_t x = pop(calc);
	int32_t y = pop(calc);
	return push(calc, x & y);
}

int op_inv(struct calc * calc)
{
	int32_t x = pop(calc);
	return push(calc, ~x);
}

int op_minus(struct calc * calc)
{
	int32_t x;

	x = pop(calc);
	push(calc, -x);
	return 0;
}

int op_shl(struct calc * calc)
{
	int32_t x = pop(calc);
	int32_t y = pop(calc);
	return push(calc, x << y);
}

int op_asr(struct calc * calc)
{
	int32_t x = pop(calc);
	int32_t y = pop(calc);
	return push(calc, x >> y);
}

int op_push_int(struct calc * calc)
{
	int32_t x = calc->tok.u32;
	return push(calc, x);
}

int op_var_decl(struct calc * calc)
{
	DBG("_");
	return 0;
}
int op_push_id(struct calc * calc)
{
	DBG("_");
	return 0;
}

int (* op[])(struct calc * calc) = {
 	[ACTION(A_OP_VAR_DECL)] = op_var_decl,
 	[ACTION(A_OP_ASSIGN)] = op_assign,
 	[ACTION(A_OP_PRINT)] = op_print,
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
 	[ACTION(A_OP_MINUS)] = op_minus,
 	[ACTION(A_OP_PUSH_INT)] = op_push_int,
 	[ACTION(A_OP_PUSH_ID)] = op_push_id,
};

/* Nonrecursive predictive parser */

#define STACK_SIZE 512

int calc_parse(struct calc * calc, const char * txt, unsigned int len)
{
	uint8_t parser_stack[STACK_SIZE];
	struct token tok;
	uint8_t * pp_sp;
	uint8_t * pp_sl;
	int sym;
	int k;

	lexer_open(&calc->lex, txt, len);
	pp_sp = parser_stack + STACK_SIZE;
	pp_sl = parser_stack;
	if ((k = calc_ll_start(pp_sp)) < 0) {
		return ERR_SYNTAX_ERROR;
	}
	pp_sp -= k;
	
	tok = lexer_scan(&calc->lex);
	while (tok.typ != T_EOF) {
		/* pop the stack */
		sym = *pp_sp++;
		if (sym == tok.typ) {
			/* matching terminal */
			calc->tok = tok;
			tok = lexer_scan(&calc->lex);
		} else if IS_AN_ACTION(sym) {
			int ret;
			if ((ret = op[ACTION(sym)](calc)) < 0) {
				WARN("ll_rule_push() failed!");
				return ret;
			}
		} else {
			//printf("%s\n", sym_nm[sym]);
			if ((k = calc_ll_push(pp_sp, sym, tok.typ)) < 0) {
				WARN("ll_rule_push() failed!");
				return k;
			}
			pp_sp -= k;
			if (pp_sp < pp_sl) {
				WARN("stack overflow!");
				/* stack overflow */
				return ERR_SYNTAX_ERROR;
			}
		}
	}

	return OK;
}

