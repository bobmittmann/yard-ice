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
 * @file .c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#include <stdlib.h>
#include <stdio.h>
#include "val.h"
#include "var.h"
#include "dbglog.h"

extern uint32_t eval_ans;

int eval_uint32(value_t * val, int argc, char * argv[])
{
	value_t tmp;
	uint32_t op1;
	uint32_t op2;
	char * cp;
	int oper;
	int c1;
	int c2;
	int args;
	int n;

	DCC_LOG1(LOG_TRACE, "argc:%d", argc);

	if (val == NULL)
		return -1;

	if (argc == 0)
		return -2;
	

	op1 = 0;
	oper = 0;
	args = 0;
	for (n = 0; n < argc; n++) {
		cp = *argv++;
	
		c1 = cp[0];
		c2 = cp[1];

		if (c2 == '\0') {
			if ((c1 == '+') || (c1 == '-') || (c1 == '*') || (c1 == '/') || 
				(c1 == '%') || (c1 == '&') || (c1 == '|') || (c1 == '^')) {
				if (oper)
					return -8;
				oper = c1;
				continue;
			}
		}

		if (cp[2] == '\0') {
			if (((c1 == '>') && (c2 == '>')) || ((c1 == '<') || (c2 == '<'))) {
				if (oper)
					return -8;
				oper = c1;
				continue;
			}
		}

		if ((args == 1) && (oper == 0))
			break;

		if ((((c1 == '-') || (c1 == '+')) && (c2 >= '0') && (c2 <= '9')) || 
			((c1 >= '0') && (c1 <= '9'))) {
			op2 = op1;
			op1 = strtoul(cp, NULL, 0);
			args++;
		} else {
			if (((c1 >= 'a') && (c1 <= 'z')) || 
				((c1 >= 'A') && (c1 <= 'Z'))) {
				var_def_t * var;

				if ((var = var_global_lookup(cp)) == NULL) {
					return -1;
				}

				var_get(var, &tmp);
				op2 = op1;

				switch (var->type) {
				case TYPE_INT32:
					op1 = tmp.int32;
					break;
				case TYPE_UINT32:
					op1 = tmp.uint32;
					break;
				case TYPE_BOOL:
					op1 = tmp.uint32;
					break;
				default:
					return -2;
				}
				args++;
			} else {
				return -3;
			}
		}

		if (oper == 0)
			continue;

		switch (oper) { 
		case '|':
			op1 = op2 | op1; 
			break;
		case '^':
			op1 = op2 ^ op1; 
			break;
		case '&':
			op1 = op2 & op1; 
			break;
		case '+':
			op1 = op2 + op1; 
			break;
		case '-':
			op1 = op2 - op1; 
			break;
		case '*':
			op1 = op2 * op1; 
			break;
		case '/':
			op1 = op2 / op1; 
			break;
		case '%':
			op1 = op2 % op1; 
			break;
		case '>':
			op1 = op2 >> op1; 
			break;
		case '<':
			op1 = op2 << op1; 
			break;
		}

		oper = 0;
		args--;
	}

	eval_ans = op1;
	val->uint32 = op1;

	return n;
}

