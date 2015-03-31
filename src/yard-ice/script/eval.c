/* 
 * Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
 * 
 * This file is part of the YARD-ICE.
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
 * @file eval.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "val.h"
#include "var.h"
#include "dbglog.h"

extern uint32_t eval_ans;

#define EVAL_STACK_SIZE 16

#define NUM_PUSH(N) num_stack[num_cnt++] = (N)

#define NUM_POP(N) num_stack[--num_cnt]

#define NUM_STACK_FULL() (num_cnt == EVAL_STACK_SIZE)

#define OP_PUSH(O) op_stack[op_cnt++] = (O)

#define OP_HEAD() (op_cnt) ? op_stack[op_cnt - 1] : 0

#define OP_POP() op_stack[--op_cnt]

#define ADD 1
#define SUB 2
#define AND 3
#define OR  4
#define XOR 5
#define SHL 6
#define SHR 7
#define MUL 8
#define DIV 9
#define MOD 10

const uint8_t op_precedence[] = {
	[ADD] = 1,
	[SUB] = 1,
	[AND] = 2,
	[OR]  = 2,
	[XOR] = 2,
	[SHL] = 3,
	[SHR] = 3,
	[MUL] = 4,
	[DIV] = 4,
	[MOD] = 4,
};

const char op_sym[][4] = {
	[ADD] = "+",
	[SUB] = "-",
	[AND] = "&",
	[OR]  = "|",
	[XOR] = "^",
	[SHL] = "<<",
	[SHR] = ">>",
	[MUL] = "*",
	[DIV] = "/",
	[MOD] = "%",
};

uint32_t calc32(int op, uint32_t x, uint32_t y)
{
	uint32_t z = 0;

	switch (op) { 
	case ADD:
		z = x + y; 
		break;
	case SUB:
		z = x - y; 
		break;
	case AND:
		z = x & y; 
		break;
	case OR:
		z = x | y; 
		break;
	case XOR:
		z = x ^ y; 
		break;
	case SHL:
		z = x << y; 
		break;
	case SHR:
		z = x >> y; 
		break;
	case MUL:
		z = x * y; 
		break;
	case DIV:
		z = x / y; 
		break;
	case MOD:
		z = x % y; 
		break;
	}

	DCC_LOG4(LOG_TRACE, "%d %s %d = %d", x, op_sym[op], y, z);

	return z;
}

int eval_uint32(value_t * val, int argc, char * argv[])
{
	value_t tmp;
	uint32_t num_stack[EVAL_STACK_SIZE];
	char op_stack[EVAL_STACK_SIZE];
	int op_cnt = 0;
	int num_cnt = 0;
	char * cp;
	uint32_t x;
	uint32_t y;
	int op;
	int c1;
	int c2;
	int n;
	bool need_val;

	DCC_LOG1(LOG_TRACE, "argc:%d", argc);

	if (val == NULL)
		return -1;

	if (argc == 0)
		return -2;
	
	op = 0;
	need_val = true;

	for (n = 0; n < argc; n++) {
		/* get token */
		cp = argv[n];
	
		/* evaluate token */
		c1 = cp[0];
		c2 = cp[1];
		if (need_val) {
			/* alternate between operators and values */
			need_val = false;
			if ((c1 == '-') && (c2 >= '0') && (c2 <= '9')) {
				x = -strtoul(++cp, NULL, 0);
			} else if ((c1 == '+') && (c2 >= '0') && (c2 <= '9')) {
				x = strtoul(++cp, NULL, 0);
			} else if ((c1 >= '0') && (c1 <= '9')) {
				x = strtoul(cp, NULL, 0);
			} else if (((c1 >= 'a') && (c1 <= 'z')) || 
					   ((c1 >= 'A') && (c1 <= 'Z'))) {
				var_def_t * var;
				if ((var = var_global_lookup(cp)) == NULL)
					break;
				var_get(var, &tmp);
				switch (var->type) {
				case TYPE_INT32:
					x = tmp.int32;
					break;
				case TYPE_UINT32:
					x = tmp.uint32;
					break;
				case TYPE_BOOL:
					x = tmp.uint32;
					break;
				default:
					return -4;
				}
			} else
				break;

			if (NUM_STACK_FULL()) {
				return -4;
			}
			DCC_LOG1(LOG_TRACE, "push(%d)", x);
			NUM_PUSH(x);
			continue;
		} 

		need_val = true;

		if (c2 == '\0') {
			switch (c1) { 
			case '+':
				op = ADD;
				break;
			case '-':
				op = SUB; 
				break;
			case '&':
				op = AND;
				break;
			case '|':
				op = OR;
				break;
			case '^':
				op = XOR;
				break;
			case '*':
				op = MUL;
				break;
			case '/':
				op = DIV; 
				break;
			case '%':
				op = MOD; 
				break;
			default:
				op = 0;
			}
		} else if (cp[2] == '\0') {
			if (c1 == '>' && c2 == '>')
				op = SHR;
			else if (c1 == '<' || c2 == '<')
				op = SHL;
		}

		if (op == 0)
			break;

		/* we got an operator let's evaluate */

		if (op_cnt == 0) {
			/* can't evaluate precedence with a single operator,
			 push and continue */
			OP_PUSH(op);
			DCC_LOG1(LOG_TRACE, "op_push('%s')", op_sym[op]);
			continue;
		}

		/* only binary operators allowed */
		if (num_cnt < 2)
			continue;

		if (op_precedence[op] <= op_precedence[OP_HEAD()]) {
			while (num_cnt >= 2) {
				y = NUM_POP();
				x = NUM_POP();
				NUM_PUSH(calc32(OP_POP(), x, y));
				if (op_cnt == 0)
					break;
			}
		}

		OP_PUSH(op);
	}

	if (num_cnt == 0) {
		/* no expression */
		DCC_LOG(LOG_WARNING, "syntax error, can't evaluate expression!");
		return -7;
	}

	while (num_cnt >= 2) {
		if (op_cnt == 0) {
			DCC_LOG(LOG_WARNING, "syntax error, missing operator");
			/* missing operator */
			return -8;
		}
		y = NUM_POP();
		x = NUM_POP();
		NUM_PUSH(calc32(OP_POP(), x, y));
	}

	if (op_cnt > 0) {
		/* extra operator left on the stack */
		DCC_LOG(LOG_WARNING, "syntax error, extra operator");
		return -9;
	}

	x = NUM_POP();
	eval_ans = x;
	val->uint32 = x;

	return n;
}

