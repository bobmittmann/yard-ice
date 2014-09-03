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
	JS_PARAM_LIST_OPT,
	JS_PARAM_LIST,
	JS_OR_EXP,
	JS_ASSIGN_EXP,
	JS_EXP_STAT,
	JS_EXP,
	JS_BLOCK,
	JS_DECL_LIST,
	JS_STAT_LIST,
	JS_STAT,
	JS_CONDITION,
	JS_EXPRESSION,
	JS_VAR_LIST,
	JS_VAR
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

static bool js_conditional_exp(struct microjs_parser * p, 
						   struct tree * t, int parent);

static bool js_assignment_exp(struct microjs_parser * p, 
						   struct tree * t, int parent);

static bool js_var(struct microjs_parser * p, 
				   struct tree * t, int parent);

static bool js_var_lst(struct microjs_parser * p, 
					  struct tree * t, int parent);

static bool js_var_or_exp(struct microjs_parser * p, struct tree * t, 
						  int parent);

static bool js_compound_stat(struct microjs_parser * p, 
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



int microjs_parse(struct microjs_parser * p, 
				  uint8_t code[], unsigned int size)
{
	struct tree my_tree;
	struct tree * t = &my_tree;
	int root;

	p->idx = 0;
	p->next = 0;
	p->t = t;
	root = tree_init(t, JS_PROGRAM);

/*
program: empty
		| element program
*/

	while (p->tok[p->next] != TOK_EOF) {
	   if (!js_element(p, t, root))
		   return -1;
	}

	return 0;
}

static bool js_element(struct microjs_parser * p, 
					   struct tree * t, int parent)
{
	unsigned int node = tree_add_node(t, parent, JS_ELEMENT);
	unsigned int save = p->next;

/*
element : function_def : 'function' id '(' param_list_opt ')' compound_stat
			| stat
*/
	return (term(p, TOK_FUNCTION) 
			&& term(p, TOK_ID) 
			&& term(p, TOK_LEFTPAREN)
			&& js_param_list_opt(p, t, node)
			&& term(p, TOK_LEFTPAREN)
			&& js_compound_stat(p, t, node)) 
		|| (p->next = save, js_stat(p, t, node));
}


static bool js_param_list_opt(struct microjs_parser * p, 
							  struct tree * t, int parent)
{
	unsigned int save = p->next;

/*
param_list_opt		: empty
			| param_list 
*/

	while (js_param_list(p, t, parent))
		save = p->next;

	p->next = save;

	return true;
}

static bool js_param_list(struct microjs_parser * p, 
					  struct tree * t, int parent)
{
	unsigned int node = tree_add_node(t, parent, JS_PARAM_LIST);
	unsigned int save = p->next;

/*
param_list : id 
			| id ',' param_list
*/

	return term(p, TOK_ID)
		|| (p->next = save, 
			term(p, TOK_ID) 
			&& term(p, TOK_COMMA) 
			&& js_param_list(p, t, node));
}

static bool js_compound_stat(struct microjs_parser * p, 
						  struct tree * t, int parent)
{
	unsigned int node = tree_add_node(t, parent, JS_BLOCK);
/*
compound_stat : '{' stat_list '}'
*/
	return (term(p, TOK_LEFTBRACE) 
		   && js_stat_list(p, t, node)
		   && term(p, TOK_RIGHTBRACE));
}

static bool js_stat_list(struct microjs_parser * p, 
						 struct tree * t, int parent)
{
	unsigned int node = tree_add_node(t, parent, JS_STAT_LIST);
//	unsigned int save = p->next;

/*
stat_list : empty
			| stat stat_list
*/

	if (!js_stat(p, t, node))
		return false;

	while (js_stat(p, t, node));

	return true;
}

static bool js_stat(struct microjs_parser * p, struct tree * t, int parent)
{
	unsigned int node = tree_add_node(t, parent, JS_STAT);
	unsigned int save = p->next;

/*
stat	:  ';'
			| 'if' condition stat 
			| 'if' condition stat else stat 
			| 'while' condition stat 
			| 'return' expression_opt ';'
			| compound_stat
			| var_or_exp ';'
*/

	return (term(p, TOK_IF) 
			&& js_condition(p, t, node)
			&& js_stat(p, t, node))
		|| (p->next = save, term(p, TOK_IF) 
			&& js_condition(p, t, node)
			&& term(p, TOK_ELSE))
		|| (p->next = save, term(p, TOK_WHILE) 
			&& js_condition(p, t, node)
			&& js_stat(p, t, node))
		|| (p->next = save, term(p, TOK_RETURN) 
			&& js_exp_opt(p, t, node))
		|| (p->next = save, js_compound_stat(p, t, node))
		|| (p->next = save, js_var_or_exp(p, t, node));
}

static bool js_condition(struct microjs_parser * p, 
						 struct tree * t, int parent)
{
	unsigned int node = tree_add_node(t, parent, JS_CONDITION);
/*
condition : '(' exp ')'
*/

	return (term(p, TOK_LEFTPAREN)
			&& js_exp(p, t, node)
			&& term(p, TOK_LEFTPAREN));
}

static bool js_var_or_exp(struct microjs_parser * p, struct tree * t, 
						  int parent)
{
	unsigned int node = tree_add_node(t, parent, JS_STAT);
	unsigned int save = p->next;

/*
js_var_or_exp :  'var' var_lst
			| exp
*/

	return (term(p, TOK_VAR) && js_var_lst(p, t, node))
		|| (p->next = save, js_exp(p, t, node));
}




static bool js_var_lst(struct microjs_parser * p, 
					  struct tree * t, int parent)
{
	unsigned int node = tree_add_node(t, parent, JS_VAR_LIST);
	unsigned int save = p->next;
/*
var_lst :  var
			| var ',' var_lst
*/

	return js_var(p, t, node)
		|| (p->next = save, 
			js_var(p, t, node)
			&& term(p, TOK_COMMA) 
			&& js_var_lst(p, t, node));
}

static bool js_var(struct microjs_parser * p, 
				   struct tree * t, int parent)
{
	unsigned int node = tree_add_node(t, parent, JS_VAR);
	unsigned int save = p->next;
/*
var:  id
			| id '=' assignment_exp
*/
	return (term(p, TOK_ID)) 
		|| (p->next = save, term(p, TOK_ID)
			&& term(p, TOK_ASSIGN) 
			&& js_assignment_exp(p, t, node));
}

static bool js_exp_opt(struct microjs_parser * p, 
					   struct tree * t, int parent)
{
	unsigned int save = p->next;

/*
expression_opt : empty
			| expresssion
*/

	while (js_exp(p, t, parent))
		save = p->next;

	p->next = save;

	return true;
}


static bool js_exp(struct microjs_parser * p, 
				   struct tree * t, int parent)
{
	unsigned int node = tree_add_node(t, parent, JS_EXP);
	unsigned int save = p->next;
/*
exp : assignment_exp
	| assignment_exp, ',' exp 
*/
	DCC_LOG(LOG_TRACE, "...");

	return js_assignment_exp(p, t, node) 
		|| (p->next = save, js_assignment_exp(p, t, node) 
			&& term(p, TOK_COLON) 
			&& js_exp(p, t, node));
}

static bool js_assignment_exp(struct microjs_parser * p, 
						   struct tree * t, int parent)
{
	unsigned int node = tree_add_node(t, parent, JS_ASSIGN_EXP);
	unsigned int save = p->next;
/*
assignment_exp : conditional_exp 
		| conditional_exp '=' assignment_exp
*/
	return js_conditional_exp(p, t, node)
		|| (p->next = save, js_conditional_exp(p, t, node)
			&& term(p, TOK_ASSIGN) 
			&& js_assignment_exp(p, t, node));
}

static bool js_conditional_exp(struct microjs_parser * p, 
						   struct tree * t, int parent)
{
	unsigned int node = tree_add_node(t, parent, JS_ASSIGN_EXP);
/*
conditional_exp : logical_or_exp
		| logical_or_exp '?' assignment_exp ':' assignment_exp 
*/
	return js_logical_or_exp(p, t, node);
}

static bool js_logical_or_exp(struct microjs_parser * p, 
						   struct tree * t, int parent)
{
	unsigned int node = tree_add_node(t, parent, JS_OR_EXP);
	unsigned int save = p->next;
/*
logical_or_exp	: logical_and_exp
			| logical_and_exp '||' logical_or_exp
*/
	return js_logical_and_exp(p, t, node) 
		|| (p->next = save, js_logical_and_exp(p, t, node) 
			&& term(p, TOK_OR) 
			&& js_logical_or_exp(p, t, node));
}

static bool js_logical_and_exp(struct microjs_parser * p, 
						   struct tree * t, int parent)
{
	unsigned int node = tree_add_node(t, parent, JS_OR_EXP);
	unsigned int save = p->next;
/*
logical_and_exp : bitwise_or_exp
			| bitwise_or_exp '&&' logical_and_exp 
*/
	return js_bitwise_or_exp(p, t, node) 
		|| (p->next = save, js_bitwise_or_exp(p, t, node) 
			&& term(p, TOK_AND) 
			&& js_logical_and_exp(p, t, node));
}

static bool js_bitwise_or_exp(struct microjs_parser * p, 
						   struct tree * t, int parent)
{
	unsigned int node = tree_add_node(t, parent, JS_OR_EXP);
	unsigned int save = p->next;
/*
bitwise_or_exp : bitwise_xor_exp
			| bitwise_xor_exp '|' bitwise_or_exp
*/
	return js_bitwise_xor_exp(p, t, node) 
		|| (p->next = save, js_bitwise_xor_exp(p, t, node) 
			&& term(p, TOK_BITOR) 
			&& js_bitwise_or_exp(p, t, node));
}

static bool js_bitwise_xor_exp(struct microjs_parser * p, 
						   struct tree * t, int parent)
{
	unsigned int node = tree_add_node(t, parent, JS_OR_EXP);
	unsigned int save = p->next;
/*
bitwise_xor_exp : bitwise_and_exp
			| bitwise_and_exp '^' bitwise_xor_exp
*/
	return js_bitwise_and_exp(p, t, node) 
		|| (p->next = save, js_bitwise_and_exp(p, t, node) 
			&& term(p, TOK_XOR) 
			&& js_bitwise_xor_exp(p, t, node));
}

static bool js_bitwise_and_exp(struct microjs_parser * p, 
						   struct tree * t, int parent)
{
	unsigned int node = tree_add_node(t, parent, JS_OR_EXP);
	unsigned int save = p->next;
/*
bitwise_and_exp : equality_exp
			| equality_exp '&' bitwise_and_exp
*/
	return js_equality_exp(p, t, node) 
		|| (p->next = save, js_equality_exp(p, t, node) 
			&& term(p, TOK_XOR) 
			&& js_bitwise_and_exp(p, t, node));
}

static bool js_equality_exp(struct microjs_parser * p, 
						   struct tree * t, int parent)
{
	unsigned int n = tree_add_node(t, parent, JS_OR_EXP);
/*
equality_exp : relational_exp
			| relational_exp '==' equality_exp 
			| relational_exp '!=' equality_exp 
*/
	while (js_relational_exp(p, t, n)) {
		unsigned int save = p->next;
		if (term(p, TOK_EQ)) 
			return js_equality_exp(p, t, n);
		if (term(p, TOK_NEQ))
			return js_equality_exp(p, t, n);
		p->next = save;
	}

	return true;
}

static bool js_relational_exp(struct microjs_parser * p, 
						   struct tree * t, int parent)
{
	unsigned int n = tree_add_node(t, parent, JS_OR_EXP);
	unsigned int save = p->next;
/*
relational_exp		: shift_exp
			| shift_exp '<' relational_exp
			| shift_exp '>' relational_exp
			| shift_exp '<=' relational_exp
			| shift_exp '>=' relational_exp
*/
	return js_shift_exp(p, t, n) 
		|| (p->next = save, js_shift_exp(p, t, n) 
			&& term(p, TOK_LT) 
			&& js_relational_exp(p, t, n))
		|| (p->next = save, js_shift_exp(p, t, n) 
			&& term(p, TOK_GT) 
			&& js_relational_exp(p, t, n))
		|| (p->next = save, js_shift_exp(p, t, n) 
			&& term(p, TOK_LTE) 
			&& js_relational_exp(p, t, n))
		|| (p->next = save, js_shift_exp(p, t, n) 
			&& term(p, TOK_GTE) 
			&& js_relational_exp(p, t, n));
}

static bool js_shift_exp(struct microjs_parser * p, 
						   struct tree * t, int parent)
{
	unsigned int n = tree_add_node(t, parent, JS_OR_EXP);
	unsigned int save = p->next;
/*
shift_expression	: additive_exp
			| additive_exp '<<' shift_expression 
			| additive_exp '>>' shift_expression 
*/
	return js_shift_exp(p, t, n) 
		|| (p->next = save, js_additive_exp(p, t, n) 
			&& term(p, TOK_SHL) 
			&& js_shift_exp(p, t, n))
		|| (p->next = save, js_additive_exp(p, t, n) 
			&& term(p, TOK_ASR) 
			&& js_shift_exp(p, t, n));
}

static bool js_additive_exp(struct microjs_parser * p,
						   struct tree * t, int parent)
{
	unsigned int n = tree_add_node(t, parent, JS_OR_EXP);
	unsigned int save = p->next;
/*
additive_exp : mult_exp
			| mult_exp '+' additive_exp 
			| mult_exp '-' additive_exp 
*/
	return js_mult_exp(p, t, n) 
		|| (p->next = save, js_mult_exp(p, t, n) 
			&& term(p, TOK_PLUS) 
			&& js_additive_exp(p, t, n))
		|| (p->next = save, js_mult_exp(p, t, n) 
			&& term(p, TOK_MINUS) 
			&& js_additive_exp(p, t, n));
}

static bool js_mult_exp(struct microjs_parser * p,
						   struct tree * t, int parent)
{
	unsigned int n = tree_add_node(t, parent, JS_OR_EXP);
	unsigned int save = p->next;
/*
mult_exp		: unary_exp
			| unary_exp '*' mult_exp
			| unary_exp '/' mult_exp
			| unary_exp '%' mult_exp
*/
	return js_unary_exp(p, t, n) 
		|| (p->next = save, js_unary_exp(p, t, n) 
			&& term(p, TOK_MUL) 
			&& js_mult_exp(p, t, n))
		|| (p->next = save, js_unary_exp(p, t, n) 
			&& term(p, TOK_DIV) 
			&& js_mult_exp(p, t, n))
		|| (p->next = save, js_unary_exp(p, t, n) 
			&& term(p, TOK_MOD) 
			&& js_mult_exp(p, t, n));
}


static bool js_unary_exp(struct microjs_parser * p,
						   struct tree * t, int parent)
{
	unsigned int n = tree_add_node(t, parent, JS_OR_EXP);
	unsigned int save = p->next;
/*
unary_exp	: member_exp
			| '+' | '-' | '~' | '!' unary_exp
*/
	return js_member_exp(p, t, n) 
		|| (p->next = save, term(p, TOK_PLUS) && js_unary_exp(p, t, n))
		|| (p->next = save, term(p, TOK_MINUS) && js_unary_exp(p, t, n))
		|| (p->next = save, term(p, TOK_BITINV) && js_unary_exp(p, t, n))
		|| (p->next = save, term(p, TOK_NOT) && js_unary_exp(p, t, n));
}

static bool js_member_exp(struct microjs_parser * p,
						   struct tree * t, int parent)
{
	unsigned int n = tree_add_node(t, parent, JS_OR_EXP);
/*
member_exp	: primary_exp
*/
	return js_primary_exp(p, t, n);
}

static bool js_primary_exp(struct microjs_parser * p,
						   struct tree * t, int parent)
{
//	unsigned int n = tree_add_node(t, parent, JS_OR_EXP);
/*
primary_exp	: member_exp
*/
	return term(p, TOK_INT32);
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

