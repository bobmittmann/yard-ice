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

enum {
	JS_PROGRAM,
	JS_ELEMENT,
	JS_PARAM_LIST,
	JS_EXP_STAT,
	JS_EXP,
	JS_BLOCK,
	JS_DECL_LIST,
	JS_STAT_LIST,
	JS_STAT,
	JS_CONDITION,
	JS_EXPRESSION,
	JS_VAR_LIST,
	JS_VAR,
	JS_ASSIGNMENT_EXP,
	JS_CONDITIONAL_EXP,
	JS_LOGICAL_OR_EXP,
	JS_LOGICAL_AND_EXP,
	JS_BITWISE_OR_EXP,
	JS_BITWISE_XOR_EXP,
	JS_BITWISE_AND_EXP,
	JS_EQUALITY_EXP,
	JS_RELATIONAL_EXP,
	JS_SHIFT_EXP,
	JS_ADDITIVE_EXP,
	JS_MULT_EXP,
	JS_UNARY_EXP,
	JS_MEMBER_EXP,
	JS_ARG_LIST,
	JS_PRIMARY_EXP
};

static bool js_element(struct microjs_parser * p, 
					   struct tree * t, int parent);

static bool js_param_list_opt(struct microjs_parser * p, 
							  struct tree * t, int parent);

static bool js_exp_opt(struct microjs_parser * p, 
					   struct tree * t, int parent);

static bool js_exp(struct microjs_parser * p, 
				   struct tree * t, int parent);

static bool js_stat(struct microjs_parser * p, 
					struct tree * t, int parent);

static bool js_stat_list(struct microjs_parser * p, 
						 struct tree * t, int parent);

static bool js_condition(struct microjs_parser * p, 
						 struct tree * t, int parent);

static bool js_assignment_exp(struct microjs_parser * p, 
							  struct tree * t, int parent);

static bool js_var(struct microjs_parser * p, 
				   struct tree * t, int parent);

static bool js_var_list(struct microjs_parser * p, 
					   struct tree * t, int parent);

static bool js_var_or_exp(struct microjs_parser * p, 
						  struct tree * t, int parent);

static bool js_stat(struct microjs_parser * p, 
					struct tree * t, int parent);

static bool js_compound_stat(struct microjs_parser * p, 
							 struct tree * t, int parent);

static bool js_param_list(struct microjs_parser * p, 
						  struct tree * t, int parent);

static bool js_logical_or_exp(struct microjs_parser * p, 
							  struct tree * t, int parent);

static bool js_logical_and_exp(struct microjs_parser * p, 
							   struct tree * t, int parent);

static bool js_bitwise_or_exp(struct microjs_parser * p, 
							  struct tree * t, int parent);

static bool js_bitwise_xor_exp(struct microjs_parser * p, 
							   struct tree * t, int parent);

static bool js_bitwise_and_exp(struct microjs_parser * p, 
							   struct tree * t, int parent);

static bool js_equality_exp(struct microjs_parser * p, 
							struct tree * t, int parent);

static bool js_relational_exp(struct microjs_parser * p, 
							  struct tree * t, int parent);

static bool js_shift_exp(struct microjs_parser * p, 
						 struct tree * t, int parent);

static bool js_additive_exp(struct microjs_parser * p,
							struct tree * t, int parent);

static bool js_mult_exp(struct microjs_parser * p,
						struct tree * t, int parent);

static bool js_unary_exp(struct microjs_parser * p,
						 struct tree * t, int parent);

static bool js_member_exp(struct microjs_parser * p,
						  struct tree * t, int parent);

static bool js_arg_list_opt(struct microjs_parser * p, 
							struct tree * t, int parent);

static bool js_arg_list(struct microjs_parser * p, 
						struct tree * t, int parent);

static bool js_primary_exp(struct microjs_parser * p,
						   struct tree * t, int parent);

struct node {
	uint16_t child;
	uint16_t next;
	uint16_t val;
};

struct tree {
	uint32_t last;
	struct node node[65536];
};

int tree_init(struct tree * t, unsigned int val)
{
	struct node * n;

	t->last = 1;
	n = &t->node[0];

	n->next = 0;
	n->child = 0;
	n->val = val;

	return 0;
}

int tree_add_node(struct tree * t, unsigned int parent, 
				  unsigned int val)
{
	struct node * p;
	struct node * c;
	struct node * k;
	int child;

	child = t->last++;
	p = &t->node[parent];
	c = &t->node[child];

	if (p->child == 0)
		p->child = child;
	else {
		k = &t->node[p->child];
		while (k->next)
			k = &t->node[k->next];
		k->next = child;
	}

	c->next = 0;
	c->child = 0;
	c->val = val;

	return child;
}


void print_node(struct tree * t, unsigned int branch, int lvl)
{
	int i;

	for (i = 0; i < lvl - 1; ++i)
		printf("  |  ");

	if (branch == 0)
		printf("[ROOT]\n");
	else
		printf("  +-[%3d]\n", branch);
}

void print_leaf(struct tree * t, unsigned int leaf, int lvl)
{
	int i;

	for (i = 0; i < lvl - 1; ++i)
		printf("  |  ");

	printf("  +-(%3d)\n", leaf);
}

int print_branch(struct tree * t, unsigned int branch, int lvl)
{
	struct node * n;
	struct node * p;
	unsigned int k;

	p = &t->node[branch];
	if ((k = p->child) == 0) {
		print_leaf(t, branch, lvl);
	} else {
		print_node(t, branch, lvl);
		do {
			print_branch(t, k, lvl + 1);
			n = &t->node[k];
		} while ((k = n->next) != 0);
	}

	return 0;
}

int print_tree(struct tree * t)
{
	if (t->last > 0)
		print_branch(t, 0, 0);
	else
		printf("<empty>\n");

	return 0;
}


/* recursive descent parser */

/* is a terminal */
static bool term(struct microjs_parser * p, unsigned int tok)
{
	unsigned int lokahead = p->tok[p->next];

	printf("_");
	microjs_tok_print(stdout, p, p->next);
	printf("_");

	p->next++;

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
		if (lokahead >= TOK_INT16) {
			p->next++;
			if (lokahead >= TOK_INT24) {
				p->next++;
				if (lokahead >= TOK_INT32)
					p->next++;
			}
		} 
		
		return (tok == TOK_INT32);
	} 

	return (tok == lokahead);
}

static unsigned int lookahead(struct microjs_parser * p)
{
	unsigned int tok = p->tok[p->next];

	if (tok >= TOK_STRING)
		return TOK_STRING;

	if (tok >= TOK_ID)
		return TOK_ID;

	if (tok >= TOK_INT8)
		return TOK_INT32;

	return tok;
}

static bool match(struct microjs_parser * p, unsigned int tok)
{
	if (lookahead(p) != tok)
		return false;

	return term(p, tok);
}

int microjs_parse(struct microjs_parser * p, 
				  uint8_t code[], unsigned int size)
{
	struct tree my_tree;
	struct tree * t = &my_tree;
	int root;
	int cnt = 0;

	p->idx = 0;
	p->next = 0;
	p->t = t;
	root = tree_init(t, JS_PROGRAM);

/*
program			: <empty>
				| element program
*/

	while (lookahead(p) != TOK_EOF) {
		printf("\n %3d:", cnt);

		if (!js_element(p, t, root))
			return MICROJS_SYNTAX_ERROR;
		cnt++;
		if (cnt > 10)
			return MICROJS_SYNTAX_ERROR;
	}

	return MICROJS_OK;
}

static bool js_element(struct microjs_parser * p, 
					   struct tree * t, int parent)
{
	unsigned int n = tree_add_node(t, parent, JS_ELEMENT);

/*
element 		: 'function' id '(' param_list_opt ')' compound_stat
				| stat
*/
//	printf("%s", __func__);

	if (match(p, TOK_FUNCTION)) {
		if (term(p, TOK_ID) && term(p, TOK_LEFTPAREN)
			&& js_param_list_opt(p, t, n) && term(p, TOK_RIGHTPAREN)
			&& js_compound_stat(p, t, n)) {
			printf("function definition!\n");
			return true;
		};
		printf("\n#error: in function definition!\n");
		return false;
	}

	return js_stat(p, t, n);
}


static bool js_param_list_opt(struct microjs_parser * p, 
							  struct tree * t, int parent)
{
/*
param_list_opt	: <empty>
				| param_list 
*/
	js_param_list(p, t, parent);

	return true;
}

static bool js_param_list(struct microjs_parser * p, 
					  struct tree * t, int parent)
{
//	unsigned int n = tree_add_node(t, parent, JS_PARAM_LIST);
/*
param_list 		: <id> 
				| <id> ',' param_list
*/
	if (lookahead(p) == TOK_ID) {
		do {
			if (!term(p, TOK_ID))
				return false;
		} while (match(p, TOK_COMMA));
	}
	return false;
}

static bool js_compound_stat(struct microjs_parser * p, 
						  struct tree * t, int parent)
{
	unsigned int n = tree_add_node(t, parent, JS_BLOCK);
/*
compound_stat	: '{' stat_list '}'
*/
//		printf(" %s", __func__);
	if (match(p, TOK_LEFTBRACE)) {
		printf("\n BLOCK {\n");
		if (js_stat_list(p, t, n)) {
			if (match(p, TOK_RIGHTBRACE)) {
				printf("}\n");
				return true;
			}
			printf("\n#error: expecting '}'\n");
		}
	}

	return false;
}

static bool js_stat_list(struct microjs_parser * p, 
						 struct tree * t, int parent)
{
	unsigned int n = tree_add_node(t, parent, JS_STAT_LIST);
/*
stat_list		: <empty>
				| stat stat_list
*/

	printf(" %s START ----------\n", __func__);

//	if (!js_stat(p, t, n))
//		return true;

	while (js_stat(p, t, n));

	printf(" %s END ---------- ", __func__);

	return true;
}

static bool js_stat(struct microjs_parser * p, struct tree * t, int parent)
{
	unsigned int n = tree_add_node(t, parent, JS_STAT);
/*
stat			: ';'
				| 'if' condition stat 
				| 'if' condition stat 'else' stat 
				| 'while' condition stat 
				| 'break' ';'
				| 'continue' ';'
				| 'return' expression_opt ';'
				| compound_stat
				| var_list_or_exp ';'
*/
//	printf(" %s", __func__);

	switch (lookahead(p)) {
	case TOK_SEMICOLON:
		printf(" ;\n");
		term(p, TOK_SEMICOLON); 
		return true;

	case TOK_IF:
		term(p, TOK_IF); 
		printf(" IF");
		if (js_condition(p, t, n) && js_stat(p, t, n)) {
			if (match(p, TOK_ELSE)) {
				printf(" ELSE");
				if (js_stat(p, t, n)) {
					printf(" ENDIF");
					return true;
				}	
				return false;
			}
			printf(" ENDIF");
			return true;
		}
		return false;

	case TOK_WHILE:
		term(p, TOK_WHILE); 
		printf(" WHILE");
		return js_condition(p, t, n) && js_stat(p, t, n);

	case TOK_BREAK:
		printf(" BREAK");
		return term(p, TOK_BREAK) && term(p, TOK_SEMICOLON);

	case TOK_CONTINUE:
		printf(" CONTINUE");
		return term(p, TOK_CONTINUE) && term(p, TOK_SEMICOLON);

	case TOK_RETURN:
		term(p, TOK_RETURN); 
		printf(" RETURN");
		return js_exp_opt(p, t, n) && term(p, TOK_SEMICOLON);
	}
	
	printf(" %s 1.\n", __func__);
	if (js_compound_stat(p, t, n))
		return true;
	
	printf(" %s 2.\n", __func__);
	if (js_var_or_exp(p, t, n)) {
		printf(" %s 3.\n", __func__);
		if (term(p, TOK_SEMICOLON)) {
			printf(" %s 4.\n", __func__);
			return true;
		}

		printf("\n#error: expecting ';'\n");
	}

	return false;
}

static bool js_condition(struct microjs_parser * p, 
						 struct tree * t, int parent)
{
	unsigned int n = tree_add_node(t, parent, JS_CONDITION);
/*
condition 		: '(' exp ')'
*/
	printf(" %s", __func__);

	if (match(p, TOK_LEFTPAREN)) {
		printf(" '('");
		if (js_exp(p, t, n)) {
			if (term(p, TOK_RIGHTPAREN)) {
				printf(" ')'");
				return true;
			}
			printf("\n#error: expecting ')' got: '");
			microjs_tok_print(stdout, p, p->next - 1);
			printf("'\n");
		}
	}

	return false;
}

static bool js_var_or_exp(struct microjs_parser * p, struct tree * t, 
						  int parent)
{
/*
var_list_or_exp	:  'var' var_list
				| exp
*/
	if (match(p, TOK_VAR)) {
		printf(" 'VAR'");
		return js_var_list(p, t, parent);
	}

	return js_exp(p, t, parent);
}


static bool js_var_list(struct microjs_parser * p, 
					  struct tree * t, int parent)
{
	unsigned int n = tree_add_node(t, parent, JS_VAR_LIST);
/*
var_list 		: var
				| var ',' var_list
*/

//	printf(" %s", __func__);

	if (!js_var(p, t, n))
		return false;

	while (match(p, TOK_COMMA)) {
		if (!js_var(p, t, n))
			return false;
	}

	return true;
}

static bool js_var(struct microjs_parser * p, 
				   struct tree * t, int parent)
{
	unsigned int n = tree_add_node(t, parent, JS_VAR);
/*
var				: <id>
				| <id> '=' assignment_exp

*/
	if (!match(p, TOK_ID))
		return false;

//	printf(" %s", __func__);

	if (!match(p, TOK_ASSIGN)) 
		return true;

	printf(" '='");

	return js_assignment_exp(p, t, n);
}

static bool js_exp_opt(struct microjs_parser * p, 
					   struct tree * t, int parent)
{
/*
expression_opt 	: <empty>
				| expresssion
*/
	js_exp(p, t, parent);

	return true;
}


static bool js_exp(struct microjs_parser * p, 
				   struct tree * t, int parent)
{
	unsigned int n = tree_add_node(t, parent, JS_EXP);
/*
exp 			: assignment_exp
				| assignment_exp ',' exp 
*/

	printf(" %s -<", __func__);

	if (!js_assignment_exp(p, t, n))
		return false;

	while (match(p, TOK_COMMA)) {
		if (!js_assignment_exp(p, t, n))
			return false;
	}

	printf(" >-\n");

	return true;
}

static bool js_assignment_exp(struct microjs_parser * p, 
						   struct tree * t, int parent)
{
	unsigned int n = tree_add_node(t, parent, JS_ASSIGNMENT_EXP);
/*
assignment_exp	: logical_or_exp
				| logical_or_exp '=' assignment_exp
*/
	if (!js_logical_or_exp(p, t, n))
		return false;

//	printf(" %s", __func__);

	while (match(p, TOK_ASSIGN)) {

		printf(" :=");

		if (!js_logical_or_exp(p, t, n))
			return false;
	}

	return true;
}

static bool js_logical_or_exp(struct microjs_parser * p, 
						   struct tree * t, int parent)
{
	unsigned int n = tree_add_node(t, parent, JS_LOGICAL_OR_EXP);
/*
logical_or_exp	: logical_and_exp
				| logical_and_exp '||' logical_or_exp
*/
	if (!js_logical_and_exp(p, t, n))
		return false;

	while (match(p, TOK_OR)) {
		printf(" ||");

		if (!js_logical_and_exp(p, t, n))
			return false;
	}

	return true;
}

static bool js_logical_and_exp(struct microjs_parser * p, 
						   struct tree * t, int parent)
{
	unsigned int n = tree_add_node(t, parent, JS_LOGICAL_AND_EXP);
/*
logical_and_exp	: bitwise_or_exp
				| bitwise_or_exp '&&' logical_and_exp 
*/
	printf(" %s", __func__);

	if (!js_bitwise_or_exp(p, t, n))
		return false;

	while (match(p, TOK_AND)) {

		printf(" &&");

		if (!js_bitwise_or_exp(p, t, n))
			return false;
	}

	return true;
}

static bool js_bitwise_or_exp(struct microjs_parser * p, 
						   struct tree * t, int parent)
{
	unsigned int n = tree_add_node(t, parent, JS_BITWISE_OR_EXP);
/*
bitwise_or_exp	: bitwise_xor_exp
				| bitwise_xor_exp '|' bitwise_or_exp
*/
//	printf(" %s", __func__);

	if (!js_bitwise_xor_exp(p, t, n))
		return false;

	while (match(p, TOK_BITOR)) {

		printf(" ||");

		if (!js_bitwise_xor_exp(p, t, n))
			return false;
	}

	return true;
}

static bool js_bitwise_xor_exp(struct microjs_parser * p, 
						   struct tree * t, int parent)
{
	unsigned int n = tree_add_node(t, parent, JS_BITWISE_XOR_EXP);
/*
bitwise_xor_exp : bitwise_and_exp
				| bitwise_and_exp '^' bitwise_xor_exp
*/
	if (!js_bitwise_and_exp(p, t, n))
		return false;

	while (match(p, TOK_XOR)) {

		printf(" XOR");

		if (!js_bitwise_and_exp(p, t, n))
			return false;
	}

	return true;
}

static bool js_bitwise_and_exp(struct microjs_parser * p, 
						   struct tree * t, int parent)
{
	unsigned int n = tree_add_node(t, parent, JS_BITWISE_AND_EXP);
/*
bitwise_and_exp : equality_exp
				| equality_exp '&' bitwise_and_exp
*/
	if (!js_equality_exp(p, t, n))
		return false;

	while (match(p, TOK_BITAND)) {

		printf(" AND");

		if (!js_equality_exp(p, t, n))
			return false;
	}

	return true;
}

static bool js_equality_exp(struct microjs_parser * p, 
						   struct tree * t, int parent)
{
	unsigned int n = tree_add_node(t, parent, JS_EQUALITY_EXP);
/*
equality_exp	: relational_exp
				| relational_exp '==' equality_exp 
				| relational_exp '!=' equality_exp 
*/
	printf(" %s", __func__);

	if (!js_relational_exp(p, t, n))
		return false;

	for (;;) {
		unsigned int tok = lookahead(p);

		if (tok == TOK_EQ) {
			term(p, TOK_EQ);
			printf(" ==");
		} else if (tok == TOK_NEQ) {
			term(p, TOK_NEQ);
			printf(" !=");
		} else 
			break;

		if (!js_relational_exp(p, t, n))
			return false;
	}

	return true;
}

static bool js_relational_exp(struct microjs_parser * p, 
						   struct tree * t, int parent)
{
	unsigned int n = tree_add_node(t, parent, JS_RELATIONAL_EXP);
/*
relational_exp	: shift_exp
				| shift_exp '<' relational_exp
				| shift_exp '>' relational_exp
				| shift_exp '<=' relational_exp
				| shift_exp '>=' relational_exp
*/
//	printf(" %s", __func__);

	if (!js_shift_exp(p, t, n))
		return false;

	for (;;) {
		unsigned int tok = lookahead(p);

		if (tok == TOK_LT) {
			term(p, TOK_LT);
			printf(" <");
		} else if (tok == TOK_GT) {
			term(p, TOK_GT);
			printf(" >");
		} else if (tok == TOK_LTE) {
			term(p, TOK_LTE);
			printf(" <=");
		} else if (tok == TOK_GTE) {
			term(p, TOK_GTE);
			printf(" >=");
		} else {
			break;
		}

		if (!js_shift_exp(p, t, n))
			return false;
	}

	return true;
}

static bool js_shift_exp(struct microjs_parser * p, 
						   struct tree * t, int parent)
{
	unsigned int n = tree_add_node(t, parent, JS_SHIFT_EXP);
/*
shift_exp	: additive_exp
			| additive_exp '<<' shift_expression 
			| additive_exp '>>' shift_expression 
*/

	printf(" %s", __func__);

	if (!js_additive_exp(p, t, n))
		return false;

	for (;;) {
		unsigned int tok = lookahead(p);

		if (tok == TOK_SHL) {

			printf(" <<");
			term(p, TOK_SHL);

		} else if (tok == TOK_ASR) {

			printf(" >>");
			term(p, TOK_ASR);

		} else 
			break;

		if (!js_additive_exp(p, t, n))
			return false;
	}

	return true;
}

static bool js_additive_exp(struct microjs_parser * p,
						   struct tree * t, int parent)
{
	unsigned int n = tree_add_node(t, parent, JS_ADDITIVE_EXP);
/*
additive_exp	: mult_exp
				| mult_exp '+' additive_exp 
				| mult_exp '-' additive_exp 
*/
	if (!js_mult_exp(p, t, n))
		return false;

	for (;;) {
		unsigned int tok = lookahead(p);

		if (tok == TOK_PLUS) {

			printf(" +");
			term(p, TOK_PLUS);
		} else if (tok == TOK_MINUS) {

			printf(" -");
			term(p, TOK_MINUS);
		} else 
			break;

		if (!js_mult_exp(p, t, n))
			return false;
	}

	return true;
}

static bool js_mult_exp(struct microjs_parser * p,
						   struct tree * t, int parent)
{
	unsigned int n = tree_add_node(t, parent, JS_MULT_EXP);
/*
mult_exp		: unary_exp
				| unary_exp '*' mult_exp
				| unary_exp '/' mult_exp
				| unary_exp '%' mult_exp
*/

	printf(" %s", __func__);

	if (!js_unary_exp(p, t, n))
		return false;

	for (;;) {
		unsigned int tok = lookahead(p);

		if (tok == TOK_MUL) {
			printf(" '*'");
			term(p, TOK_MUL);
		} else if (tok == TOK_DIV) {
			printf(" '//'");
			term(p, TOK_DIV);
		} else if (tok == TOK_MOD) {
			printf(" '%%'");
			term(p, TOK_MOD);
		} else 
			break;

		if (!js_unary_exp(p, t, n))
			return false;
	}

	return true;
}


static bool js_unary_exp(struct microjs_parser * p,
						   struct tree * t, int parent)
{
	unsigned int n = tree_add_node(t, parent, JS_UNARY_EXP);
/*
unary_exp		: member_exp
				| '~' unary_exp
				| '!' unary_exp
				| '+' unary_exp
				| '-' unary_exp
*/

	if (js_member_exp(p, t, n))
		return true;

	for (;;) {
		unsigned int tok = lookahead(p);

		if (tok == TOK_BITINV) {
			printf(" ~");
			term(p, TOK_BITINV);
		} else if (tok == TOK_NOT) {
			printf(" !");
			term(p, TOK_NOT);
		} else if (tok == TOK_PLUS) {
			printf(" +");
			term(p, TOK_PLUS);
		} else if (tok == TOK_MINUS) {
			printf(" -");
			term(p, TOK_MINUS);
		} else 
			break;

		if (!js_member_exp(p, t, n))
			return false;
	}

	return false;
}

static bool js_member_exp(struct microjs_parser * p,
						   struct tree * t, int parent)
{
	unsigned int n = tree_add_node(t, parent, JS_MEMBER_EXP);
/*
member_exp		: primary_exp
				| primary_exp '.' member_exp
				| primary_exp '[' exp ']'
				| primary_exp '(' arg_list_opt ')'
*/
	if (js_primary_exp(p, t, n)) {
		switch (lookahead(p)) {
		case TOK_DOT:
			printf(" '.'");
			term(p, TOK_DOT);
			return js_member_exp(p, t, n);

		case TOK_LEFTBRACKET:
			printf(" '['");
			term(p, TOK_LEFTBRACKET); 
			if (js_exp(p, t, n)) {
				if (term(p, TOK_RIGHTBRACKET)) {
					printf(" ']'");
					return true;
				}
			}
			return false;

		case TOK_LEFTPAREN:
			printf(" '('");
			term(p, TOK_LEFTPAREN); 
			if (js_arg_list_opt(p, t, n)) {
				if (term(p, TOK_RIGHTPAREN)) {
					printf(" ')'");
					return true;
				}
			}
			return false;
		}

		return true;
	} 
	return false;
}

static bool js_arg_list_opt(struct microjs_parser * p, 
							struct tree * t, int parent)
{
/*
arg_list_opt	: empty
				| arg_list
*/
	js_arg_list(p, t, parent);

	return true;
}

static bool js_arg_list(struct microjs_parser * p, 
							struct tree * t, int parent)
{
	unsigned int n = tree_add_node(t, parent, JS_ARG_LIST);
/*
arg_list		: assignment_exp
				| assignment_exp ',' arg_list
*/
	if (!js_assignment_exp(p, t, n)) 
		return false;

	while (match(p, TOK_COMMA)) {
		if (!js_assignment_exp(p, t, n)) 
			return false;
	}

	return true;
}


static bool js_primary_exp(struct microjs_parser * p,
						   struct tree * t, int parent)
{
	unsigned int n = tree_add_node(t, parent, JS_PRIMARY_EXP);
/*
primary_exp		: '(' exp ')'
				| <id> 
				| <int_literal> 
				| <float_literal> 
				| <string_literal> 
				| <false> 
				| <true> 
				| <null> 
*/
//	printf(" %s", __func__);

	switch (lookahead(p)) {
	case TOK_LEFTPAREN: 
		printf(" (");
		term(p, TOK_LEFTPAREN);
		if (js_exp(p, t, n)) {
			if (term(p, TOK_RIGHTPAREN)) {
				printf(" )");
				return true;
			}
			printf("\n#error: expecting ')'\n");
		}
		return false;
	case TOK_ID:
		term(p, TOK_ID);
		printf(" <id>");
		return true;
	case TOK_INT32:
		term(p, TOK_INT32);
		printf(" <int>");
		return true;
	case TOK_FALSE:
		term(p, TOK_FALSE);
		printf(" <false>");
		return true;
	case TOK_TRUE:
		term(p, TOK_TRUE);
		printf(" <true>");
		return true;
	case TOK_NULL:
		term(p, TOK_NULL);
		printf(" <null>");
		return true;
	}

	return false;
}

int microjs_test(void)
{
	struct tree my_tree;
	struct tree * t = &my_tree;
	int node = 0;
	int n1 = 0;
	int n2 = 0;
	int n3 = 0;
	int n22 = 0;
	int n33 = 0;

	node = tree_init(t, 0);
	
	n1 = tree_add_node(t, node, 10);
	n2 = tree_add_node(t, node, 20);
	n3 = tree_add_node(t, node, 30);

	tree_add_node(t, n1, 11);
	tree_add_node(t, n1, 12);
	tree_add_node(t, n1, 13);
	tree_add_node(t, n1, 14);

	tree_add_node(t, n2, 21);
	n22 = tree_add_node(t, n2, 22);
	tree_add_node(t, n2, 23);
	tree_add_node(t, n2, 24);

	tree_add_node(t, n3, 31);
	tree_add_node(t, n3, 32);
	n33 = tree_add_node(t, n3, 33);
	tree_add_node(t, n3, 34);


	tree_add_node(t, n22, 221);
	tree_add_node(t, n22, 222);
	tree_add_node(t, n22, 223);
	tree_add_node(t, n22, 224);

	tree_add_node(t, n33, 331);
	tree_add_node(t, n33, 332);
	tree_add_node(t, n33, 333);
	tree_add_node(t, n33, 334);

	tree_add_node(t, n1, 15);


	return print_tree(t);
}

