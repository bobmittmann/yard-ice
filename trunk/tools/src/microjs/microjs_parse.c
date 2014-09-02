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

#define __MICROJS_I__
#include "microjs-i.h"

/* recursive descent parser */

/* is a terminal */
static bool term(struct microjs_parser * p, unsigned int tok)
{
	unsigned int lokahead = p->tok[p->next++];

	if (lokahead >= TOK_STRING) {
		p->next += 2;
		return (tok == TOK_STRING);
	} 

	if (lokahead >= TOK_ID) {
		p->next += lokahead - TOK_ID + 1;
		return (tok == TOK_ID);
	} 

	if (lokahead >= TOK_INT8) {
		p->next++;
		if (tok >= TOK_INT16) {
			p->next++;
			if (tok == TOK_INT24) {
				p->next++;
				if (tok >= TOK_INT32)
					p->next++;
			}
		} 
		return (tok == TOK_INT32);
	} 

	return (tok == lokahead);
}

/*
const_exp		: conditional_exp
*/

/*
argument_exp_list	: assignment_exp
			| argument_exp_list ',' assignment_exp
*/


static bool _exp_(struct microjs_parser * p);

static bool _const_(struct microjs_parser * p)
{
/*
const		: int_const
			| char_const
			| float_const
			| enumeration_const
*/
	return term(p, TOK_INT32);
}

static bool primary_exp(struct microjs_parser * p)
{
	unsigned int save = p->next;
/*
primary_exp	: id
			| const
			| string
			| '(' exp ')'
*/
	return term(p, TOK_ID)
		|| (p->next = save, _const_(p)) 
		|| (p->next = save, term(p, TOK_LEFTPAREN) 
			&& _exp_(p) 
			&& term(p, TOK_RIGHTPAREN)); 
}

static bool postfix_exp(struct microjs_parser * p)
{
	unsigned int save = p->next;
/*
postfix_exp : primary_exp
			| postfix_exp '[' exp ']'
			| postfix_exp '(' argument_exp_list ')'
			| postfix_exp '('			')'
			| postfix_exp '.' id
*/
	return primary_exp(p) 
		|| (p->next = save, postfix_exp(p) 
			&& term(p, TOK_LEFTBRACKET) 
			&& _exp_(p) 
			&& term(p, TOK_RIGHTBRACKET)) 
/*		|| (p->next = save, postfix_exp(p) 
			&& term(p, TOK_LEFTPAREN) 
			&& argument_exp_list(p) 
			&& term(p, TOK_RIGHTPAREN)) */
		|| (p->next = save, postfix_exp(p) 
			&& term(p, TOK_LEFTPAREN) 
			&& term(p, TOK_RIGHTPAREN)) 
		|| (p->next = save, postfix_exp(p) 
			&& term(p, TOK_DOT) 
			&& term(p, TOK_ID));
}

static bool unary_operator(struct microjs_parser * p)
{
	unsigned int save = p->next;
/*
unary_operator	: '+' | '-' | '~' | '!'
*/
	return term(p, TOK_PLUS)
		|| (p->next = save, term(p, TOK_MINUS))
		|| (p->next = save, term(p, TOK_BITINV))
		|| (p->next = save, term(p, TOK_NOT));
}

static bool unary_exp(struct microjs_parser * p)
{
	unsigned int save = p->next;
/*
unary_exp	: postfix_exp
			| unary_operator unary_exp
*/
	return postfix_exp(p) 
		|| (p->next = save, unary_operator(p) && unary_exp(p));
}

static bool mult_exp(struct microjs_parser * p)
{
	unsigned int save = p->next;
/*
mult_exp		: unary_exp
			| mult_exp '*' unary_exp
			| mult_exp '/' unary_exp
			| mult_exp '%' unary_exp
*/
	return unary_exp(p) 
		|| (p->next = save, mult_exp(p) 
			&& term(p, TOK_MUL) 
			&& unary_exp(p))
		|| (p->next = save, mult_exp(p) 
			&& term(p, TOK_DIV) 
			&& unary_exp(p))
		|| (p->next = save, mult_exp(p) 
			&& term(p, TOK_MOD) 
			&& unary_exp(p));
}

static bool additive_exp(struct microjs_parser * p)
{
	unsigned int save = p->next;
/*
additive_exp		: mult_exp
			| additive_exp '+' mult_exp
			| additive_exp '-' mult_exp
*/
	return mult_exp(p) 
		|| (p->next = save, additive_exp(p) 
			&& term(p, TOK_PLUS) 
			&& mult_exp(p))
		|| (p->next = save, additive_exp(p) 
			&& term(p, TOK_MINUS) 
			&& mult_exp(p));
}

static bool shift_expression(struct microjs_parser * p)
{
	unsigned int save = p->next;
/*
shift_expression	: additive_exp
			| shift_expression '<<' additive_exp
			| shift_expression '>>' additive_exp
*/
	return additive_exp(p) 
		|| (p->next = save, shift_expression(p) 
			&& term(p, TOK_SHL) 
			&& additive_exp(p))
		|| (p->next = save, shift_expression(p) 
			&& term(p, TOK_ASR) 
			&& additive_exp(p));
}

static bool relational_exp(struct microjs_parser * p)
{
	unsigned int save = p->next;
/*
relational_exp		: shift_expression
			| relational_exp '<' shift_expression
			| relational_exp '>' shift_expression
			| relational_exp '<=' shift_expression
			| relational_exp '>=' shift_expression
*/
	return shift_expression(p) 
		|| (p->next = save, relational_exp(p) 
			&& term(p, TOK_LT) 
			&& shift_expression(p))
		|| (p->next = save, relational_exp(p) 
			&& term(p, TOK_GT) 
			&& shift_expression(p))
		|| (p->next = save, relational_exp(p) 
			&& term(p, TOK_LTE) 
			&& shift_expression(p))
		|| (p->next = save, relational_exp(p) 
			&& term(p, TOK_GTE) 
			&& shift_expression(p));
}

static bool equality_exp(struct microjs_parser * p)
{
	unsigned int save = p->next;
/*
equality_exp		: relational_exp
			| equality_exp '==' relational_exp
			| equality_exp '!=' relational_exp
*/
	return relational_exp(p) 
		|| (p->next = save, equality_exp(p) 
			&& term(p, TOK_EQ) 
			&& relational_exp(p))
		|| (p->next = save, equality_exp(p) 
			&& term(p, TOK_NEQ) 
			&& relational_exp(p));
}

static bool and_exp(struct microjs_parser * p)
{
	unsigned int save = p->next;
/*
and_exp			: equality_exp
			| and_exp '&' equality_exp
*/
	return equality_exp(p) 
		|| (p->next = save, and_exp(p) 
			&& term(p, TOK_BITAND) 
			&& equality_exp(p));
}

static bool exclusive_or_exp(struct microjs_parser * p)
{
	unsigned int save = p->next;
/*
exclusive_or_exp	: and_exp
			| exclusive_or_exp '^' and_exp
*/
	return and_exp(p) 
		|| (p->next = save, exclusive_or_exp(p) 
			&& term(p, TOK_XOR) 
			&& and_exp(p));
}


static bool inclusive_or_exp(struct microjs_parser * p)
{
	unsigned int save = p->next;
/*
inclusive_or_exp	: exclusive_or_exp
			| inclusive_or_exp '|' exclusive_or_exp
*/
	return exclusive_or_exp(p) 
		|| (p->next = save, inclusive_or_exp(p) 
			&& term(p, TOK_OR) 
			&& exclusive_or_exp(p));
}


static bool logical_and_exp(struct microjs_parser * p)
{
	unsigned int save = p->next;
/*
logical_and_exp		: inclusive_or_exp
			| logical_and_exp '&&' inclusive_or_exp
*/
	return inclusive_or_exp(p) 
		|| (p->next = save, logical_and_exp(p) 
			&& term(p, TOK_BITAND) 
			&& inclusive_or_exp(p));
}

static bool logical_or_exp(struct microjs_parser * p)
{
	unsigned int save = p->next;
/*
logical_or_exp	: logical_and_exp
			| logical_or_exp '||' logical_and_exp
*/
	return logical_and_exp(p) 
		|| (p->next = save, logical_or_exp(p) 
			&& term(p, TOK_BITOR) 
			&& logical_and_exp(p));
}

static bool assignment_exp(struct microjs_parser * p)
{
/*
assignment_exp : unary_exp '=' logical_or_exp
*/
	return unary_exp(p) && term(p, TOK_ASSIGN) && logical_or_exp(p);
}

static bool _exp_(struct microjs_parser * p)
{
	unsigned int save = p->next;
/*
exp : assignment_exp
	| exp ',' assignment_exp
*/
	DCC_LOG(LOG_TRACE, "...");

	return assignment_exp(p) 
		|| (p->next = save, _exp_(p) 
			&& term(p, TOK_COLON) 
			&& assignment_exp(p));
}

static bool exp_stat(struct microjs_parser * p)
{
	unsigned int save = p->next;
/*
exp_stat : exp ';'
			|	';'
*/
	return (_exp_(p) && term(p, TOK_SEMICOLON)) ||
		(p->next = save, term(p, TOK_SEMICOLON));
}

static bool labeled_stat(struct microjs_parser * p)
{
	DCC_LOG(LOG_TRACE, "...");
	return false;
}

static bool compound_stat(struct microjs_parser * p)
{
	DCC_LOG(LOG_TRACE, "...");
	return false;
}
static bool selection_stat(struct microjs_parser * p)
{
	DCC_LOG(LOG_TRACE, "...");
	return false;
}

static bool iteration_stat(struct microjs_parser * p)
{
	DCC_LOG(LOG_TRACE, "...");
	return false;
}

static bool jump_stat(struct microjs_parser * p)
{
	DCC_LOG(LOG_TRACE, "...");
	return false;
}

static bool stat(struct microjs_parser * p)
{
	unsigned int save = p->next;
/*
stat	: labeled_stat
			| exp_stat
			| compound_stat
			| selection_stat
			| iteration_stat
			| jump_stat
   */

	return labeled_stat(p) ||
		(p->next = save, exp_stat(p)) ||
		(p->next = save, compound_stat(p)) ||
		(p->next = save, selection_stat(p)) ||
		(p->next = save, iteration_stat(p)) ||
		(p->next = save, jump_stat(p));
}

int microjs_parse(struct microjs_parser * p, 
				  uint8_t code[], unsigned int size)
{
	p->idx = 0;
	p->next = 0;

	while (!term(p, TOK_EOF)) {
	   if (!stat(p))
		   return -1;
	}

	return 0;
}

