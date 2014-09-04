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

#include "config.h"
#include "calc.h"

#define DEBUG 1

#include "debug.h"

enum {
	PROGRAM,
	ELEMENT,
	PARAM_LIST,
	EXP_STAT,
	EXP,
	BLOCK,
	DECL_LIST,
	STAT_LIST,
	STAT,
	CONDITION,
	EXPRESSION,
	VAR_LIST,
	VAR,
	ASSIGNMENT_EXP,
	CONDITIONAL_EXP,
	LOGICAL_OR_EXP,
	LOGICAL_AND_EXP,
	BITWISE_OR_EXP,
	BITWISE_XOR_EXP,
	BITWISE_AND_EXP,
	EQUALITY_EXP,
	RELATIONAL_EXP,
	SHIFT_EXP,
	ADDITIVE_EXP,
	MULT_EXP,
	UNARY_EXP,
	MEMBER_EXP,
	ARG_LIST,
	PRIMARY_EXP
};

static bool _element(struct parser * p, 
					   struct tree * t, int parent);

static bool _param_list_opt(struct parser * p, 
							  struct tree * t, int parent);

static bool _exp_opt(struct parser * p, 
					   struct tree * t, int parent);

static bool _exp(struct parser * p, 
				   struct tree * t, int parent);

static bool _stat(struct parser * p, 
					struct tree * t, int parent);

static bool _stat_list(struct parser * p, 
						 struct tree * t, int parent);

static bool _condition(struct parser * p, 
						 struct tree * t, int parent);

static bool _var(struct parser * p, 
				   struct tree * t, int parent);

static bool _var_list(struct parser * p, 
					   struct tree * t, int parent);

static bool _stat(struct parser * p, 
					struct tree * t, int parent);

static bool _compound_stat(struct parser * p, 
							 struct tree * t, int parent);

static bool _param_list(struct parser * p, 
						  struct tree * t, int parent);
static bool _shift_exp(struct parser * p, 
						 struct tree * t, int parent);

static bool _additive_exp(struct parser * p,
							struct tree * t, int parent);

static bool _mult_exp(struct parser * p,
						struct tree * t, int parent);

static bool _unary_exp(struct parser * p,
						 struct tree * t, int parent);

static bool _method_or_attr(struct parser * p,
						  struct tree * t, int parent);

static bool _arg_list_opt(struct parser * p, 
							struct tree * t, int parent);

static bool _arg_list(struct parser * p, 
						struct tree * t, int parent);

static bool _primary_exp(struct parser * p,
						   struct tree * t, int parent);

static bool _assign_or_call(struct parser * p,
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


static void fetch(struct parser * p)
{
	p->tok = lexer_scan(&p->lex);
	p->cnt++;
}

/* is a terminal */
static unsigned int lookahead(struct parser * p)
{
	return p->tok.typ;
}

static bool match(struct parser * p, unsigned int tok)
{
	if (p->tok.typ != tok)
		return false;

	fetch(p);

	return true;
}

/* recursive descent parser */

int calc_open(struct parser * p, const char * txt, unsigned int len)
{
	p->cnt = 0;
	p->tok.typ = TOK_VOID;
	return lexer_open(&p->lex, txt, len);
}

int calc_parse(struct parser * p, uint8_t code[], unsigned int size)
{
	struct tree my_tree;
	struct tree * t = &my_tree;
	int root;
	int cnt = 0;

	root = tree_init(t, PROGRAM);
	p->t = t;

/*
program			: <empty>
				| element program
*/

	fetch(p);
	while (lookahead(p) != TOK_EOF) {
		if (!_element(p, t, root))
			return ERR_SYNTAX_ERROR;
		cnt++;
		if (cnt > 200)
			return ERR_SYNTAX_ERROR;
	}

	return OK;
}

static bool _element(struct parser * p, 
					   struct tree * t, int parent)
{
	unsigned int n = tree_add_node(t, parent, ELEMENT);

/*
element 		: 'function' <id> '(' param_list_opt ')' compound_stat
				| stat
*/
	if (match(p, TOK_FUNCTION)) {
		DBG("function");
		if (!(match(p, TOK_ID) && match(p, TOK_LEFTPAREN)
			&& _param_list_opt(p, t, n) && match(p, TOK_RIGHTPAREN)
			&& _compound_stat(p, t, n))) {
			WARN("function definition");
			return false;
		};
		return true;
	}

	return _stat(p, t, n);
}

static bool _stat(struct parser * p, struct tree * t, int parent)
{
	unsigned int n = tree_add_node(t, parent, STAT);
/*
stat			: ';'
				| 'if' condition compound_stat
				| 'if' condition compound_stat 'else' compound_stat
				| 'while' condition compound_stat
				| 'break' ';'
				| 'continue' ';'
				| 'return' expression_opt ';'
				| 'var' var_list ';'
				| compound_stat
				| assign_or_call ';'

*/
//	printf(" %s", __func__);

	switch (lookahead(p)) {
	case TOK_SEMICOLON:
		fetch(p);
		DBG("';'");
		return true;

	case TOK_IF:
		fetch(p);
		DBG("<IF>");
		if (_condition(p, t, n) && _stat(p, t, n)) {
			if (match(p, TOK_ELSE)) {
				printf(" ELSE");
				if (_stat(p, t, n)) {
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
		fetch(p);
		DBG("<WHILE>");
		return _condition(p, t, n) && _stat(p, t, n);

	case TOK_BREAK:
		fetch(p);
		DBG("<BREAK>");
		return match(p, TOK_BREAK) && match(p, TOK_SEMICOLON);

	case TOK_CONTINUE:
		fetch(p);
		DBG("<CONTINUE>");
		return match(p, TOK_CONTINUE) && match(p, TOK_SEMICOLON);

	case TOK_RETURN:
		fetch(p);
		DBG("<RETURN>");
		return _exp_opt(p, t, n) && match(p, TOK_SEMICOLON);

	case TOK_VAR:
		fetch(p);
		DBG("<VAR>");
		return _var_list(p, t, parent);
	}
	
	if (_compound_stat(p, t, n))
		return true;

	if (_assign_or_call(p, t, n)) {
		if (match(p, TOK_SEMICOLON)) {
			return true;
		}
		WARN("expecting ';'");
	}

	return false;
}

static bool _compound_stat(struct parser * p, 
						  struct tree * t, int parent)
{
	unsigned int n = tree_add_node(t, parent, BLOCK);
/*
compound_stat	: '{' stat_list '}'

*/
	if (match(p, TOK_LEFTBRACE)) {
		DBG("'{'");
		if (!(_stat_list(p, t, n) && match(p, TOK_RIGHTBRACE))) {
			WARN("expecting '}'");
			return false;
		}
		DBG("'}'");
		return true;
	}

	return false;
}

static bool _stat_list(struct parser * p, 
						 struct tree * t, int parent)
{
	unsigned int n = tree_add_node(t, parent, STAT_LIST);
/*
stat_list		: <empty>
				| stat stat_list
*/

	while (_stat(p, t, n));

	return true;
}

static bool _param_list_opt(struct parser * p, 
							  struct tree * t, int parent)
{
/*
param_list_opt	: <empty>
				| param_list 
*/
	_param_list(p, t, parent);

	return true;
}

static bool _param_list(struct parser * p, 
					  struct tree * t, int parent)
{
//	unsigned int n = tree_add_node(t, parent, PARAM_LIST);
/*
param_list 		: <id> 
				| <id> ',' param_list
*/
	if (lookahead(p) == TOK_ID) {
		do {
			if (!match(p, TOK_ID))
				return false;
		} while (match(p, TOK_COMMA));
	}
	return false;
}

static bool _condition(struct parser * p, 
						 struct tree * t, int parent)
{
	unsigned int n = tree_add_node(t, parent, CONDITION);
/*
condition 		: '(' exp ')'
*/
	if (match(p, TOK_LEFTPAREN)) {
		DBG("'('");
		if (!(_exp(p, t, n) && match(p, TOK_RIGHTPAREN))) {
			WARN("expecting ')'");
			return false;
		}
		DBG("')'");
		return true;
	}

	return false;
}


static bool _var_list(struct parser * p, 
					  struct tree * t, int parent)
{
	unsigned int n = tree_add_node(t, parent, VAR_LIST);
/*
var_list 		: var
				| var ',' var_list
*/
	if (!_var(p, t, n))
		return false;

	while (match(p, TOK_COMMA)) {
		if (!_var(p, t, n))
			return false;
	}

	return true;
}

static bool _var(struct parser * p, 
				   struct tree * t, int parent)
{
	unsigned int n = tree_add_node(t, parent, VAR);
	struct token tok = p->tok;
/*
var				: <id>
				| <id> '=' exp

*/
	if (!match(p, TOK_ID)) {
		WARN("unexpected: %s", tok2str(p->tok));
		return false;
	}

	DBG("%s", tok2str(tok));

	if (!match(p, TOK_ASSIGN)) 
		return true;

	DBG("'='");

	return _exp(p, t, n);
}

static bool _exp_opt(struct parser * p, 
					   struct tree * t, int parent)
{
/*
expression_opt 	: <empty>
				| exp
*/
	_exp(p, t, parent);

	return true;
}

static bool _assign_or_call(struct parser * p,
						   struct tree * t, int parent)
{
	unsigned int n = tree_add_node(t, parent, PRIMARY_EXP);
	struct token tok;

/*
assign_or_call  : <id> '=' exp
				| <id> '.' assign_or_call 
				| <id> '[' exp ']' '=' exp
				| <id> '[' exp ']' '.' assign_or_call
				| <id> '(' arg_list_opt ')'
				| <id> '(' arg_list_opt ')' '.' assign_or_call
*/

	if (lookahead(p) != TOK_ID)
		return false;

	tok = p->tok;

	while (match(p, TOK_ID)) {
		DBG("%s", tok2str(tok));

		switch (lookahead(p)) {
		case TOK_ASSIGN:
			fetch(p);
			DBG("'='");
			return _exp(p, t, n);

		case TOK_DOT:
			fetch(p);
			break;

		case TOK_LEFTBRACKET:
			fetch(p);

			DBG("'['");
			if (!(_exp(p, t, n) && match(p, TOK_RIGHTBRACKET))) 
				return false;
			DBG("']'");

			if (match(p, TOK_ASSIGN)) {
				DBG("'='");
				return _exp(p, t, n);
			}

			if (!match(p, TOK_DOT))
				return false;
			break;

		case TOK_LEFTPAREN: 
			fetch(p);

			DBG("'('");
			if (!(_arg_list_opt(p, t, n) && match(p, TOK_RIGHTPAREN)))
				return false;
			DBG("')'");

			if (!match(p, TOK_DOT))
				return true;
			break;

		}
		DBG("'.'");
		tok = p->tok;
	}

	return false;
}


static bool _arg_list_opt(struct parser * p, 
							struct tree * t, int parent)
{
/*
arg_list_opt	: empty
				| arg_list
*/
	_arg_list(p, t, parent);

	return true;
}

static bool _arg_list(struct parser * p, 
							struct tree * t, int parent)
{
	unsigned int n = tree_add_node(t, parent, ARG_LIST);
/*
arg_list		: exp  
				| exp ',' arg_list

*/
	if (!_exp(p, t, n)) 
		return false;

	while (match(p, TOK_COMMA)) {
		if (!_exp(p, t, n)) 
			return false;
	}

	return true;
}



static bool _exp(struct parser * p, 
				   struct tree * t, int parent)
{
	unsigned int n = tree_add_node(t, parent, EXP);
/*
exp 			: '[' arg_list_opt ']'
				| shift_exp
				| shift_exp '<' relational_exp
				| shift_exp '>' relational_exp
				| shift_exp '<=' relational_exp
				| shift_exp '>=' relational_exp
				| shift_exp '==' relational_exp
				| shift_exp '!=' relational_exp
*/

	if (lookahead(p) == TOK_LEFTBRACKET) {
		fetch(p);
		DBG("'['");
		if (!(_arg_list_opt(p, t, n) && match(p, TOK_RIGHTBRACKET))) 
			return false;
		DBG("']'");
		return true;
	}

	while (_shift_exp(p, t, n)) {
		switch (lookahead(p)) {
		case TOK_LT:
			DBG("'<'");
			break;
		case TOK_GT:
			DBG("'>'");
			break;
		case TOK_LTE:
			DBG("'<='");
			break;
		case TOK_GTE:
			DBG("'>='");
			break;
		case TOK_EQ:
			DBG("'=='");
			break;
		case TOK_NEQ:
			DBG("'!='");
			break;
		default:
			return true;
		}
		fetch(p);
	} 

	return false;
}

static bool _shift_exp(struct parser * p, 
						   struct tree * t, int parent)
{
	unsigned int n = tree_add_node(t, parent, SHIFT_EXP);
/*
shift_exp		: additive_exp
				| additive_exp '<<' shift_exp
				| additive_exp '>>' shift_exp

*/

//	printf(" %s", __func__);

	while (_additive_exp(p, t, n)) {
		switch (lookahead(p)) {
		case TOK_SHL:
			DBG("'<<'");
			break;
		case TOK_ASR:
			DBG("'>>'");
			break;
		default:
			return true;
		}
		fetch(p);
	} 

	return false;
}

static bool _additive_exp(struct parser * p,
						   struct tree * t, int parent)
{
	unsigned int n = tree_add_node(t, parent, ADDITIVE_EXP);
	int op;
/*
additive_exp 	: mult_exp
				| mult_exp '+' additive_exp 
				| mult_exp '-' additive_exp 
				| mult_exp '|' additive_exp 
				| mult_exp '^' additive_exp 
				| mult_exp '||' additive_exp 
*/
	if (!_mult_exp(p, t, n))
		return false;

	for (;;) {
		switch (op = lookahead(p)) {
		case TOK_PLUS:
			break;
		case TOK_MINUS:
			break;
		case TOK_BITOR:
			break;
		case TOK_XOR:
			break;
		case TOK_OR:
			break;
		default:
			return true;
		}
		fetch(p);
		if (!_mult_exp(p, t, n))
			return false;

		switch (op) {
		case TOK_PLUS:
			DBG("'+'");
			break;
		case TOK_MINUS:
			DBG("'-'");
			break;
		case TOK_BITOR:
			DBG("'|'");
			break;
		case TOK_XOR:
			DBG("'^'");
			break;
		case TOK_OR:
			DBG("'||'");
			break;
		}
	}
}

static bool _mult_exp(struct parser * p,
						   struct tree * t, int parent)
{
	unsigned int n = tree_add_node(t, parent, MULT_EXP);
	int op;
/*
mult_exp		: unary_exp
				| unary_exp '*' mult_exp
				| unary_exp '/' mult_exp
				| unary_exp '%' mult_exp
				| unary_exp '&' mult_exp
				| unary_exp '&&' mult_exp

*/
	if (!_unary_exp(p, t, n))
		return false;

	for (;;) {
		switch (op = lookahead(p)) {
		case TOK_MUL:
			break;
		case TOK_DIV:
			break;
		case TOK_MOD:
			break;
		case TOK_AND:
			break;
		case TOK_BITAND:
			break;
		default:
			return true;
		}
		fetch(p);

		if (!_unary_exp(p, t, n))
			return false;

		switch (op) {
		case TOK_MUL:
			DBG("'*'");
			break;
		case TOK_DIV:
			DBG("'/'");
			break;
		case TOK_MOD:
			DBG("'%%'");
			break;
		case TOK_AND:
			DBG("'&'");
			break;
		case TOK_BITAND:
			DBG("'&&'");
			break;
		}
	}
}


static bool _unary_exp(struct parser * p,
						   struct tree * t, int parent)
{
	unsigned int n = tree_add_node(t, parent, UNARY_EXP);
	int op;
	int stack[32];
	int i = 0;
/*
unary_exp		: primary_exp
				| '~' unary_exp
				| '!' unary_exp
				| '+' unary_exp
				| '-' unary_exp

*/

	if (_primary_exp(p, t, n))
		return true;

	do {
		switch (op = lookahead(p)) {
		case TOK_BITINV:
			stack[i++] = op;
			fetch(p);
			DBG2("'~'");
			continue;
		case TOK_NOT:
			DBG2("'!'");
			stack[i++] = op;
			fetch(p);
			continue;
		case TOK_PLUS:
			stack[i++] = op;
			DBG2("'+'");
			fetch(p);
			continue;
		case TOK_MINUS:
			stack[i++] = op;
			DBG2("'-'");
			fetch(p);
			continue;
		}
		break;
	} while (1);

	if (!_primary_exp(p, t, n))
		return false;

	while (i) {
		op = stack[--i];
		switch (op) {
		case TOK_BITINV:
			DBG("'~'");
			break;
		case TOK_NOT:
			DBG("'!'");
			break;
		case TOK_PLUS:
			DBG("'+'");
			break;
		case TOK_MINUS:
			DBG("'-'");
			break;

		} 
	}

	return true;
}

static bool _primary_exp(struct parser * p,
						   struct tree * t, int parent)
{
	unsigned int n = tree_add_node(t, parent, PRIMARY_EXP);
	struct token tok;
/*
primary_exp		: '(' exp ')'
				| <int_literal> 
				| <float_literal> 
				| <string_literal> 
				| <false> 
				| <true> 
				| <null> 
				| method_or_attr

*/
//	printf(" %s", __func__);

	tok = p->tok;

	switch (lookahead(p)) {
	case TOK_LEFTPAREN: 
		DBG("'('");
		fetch(p);
		if (!(_exp(p, t, n) && match(p, TOK_RIGHTPAREN)))
			return false;
		DBG("')'");
		return true;
	case TOK_INT:
		DBG("%s", tok2str(tok));
		fetch(p);
		return true;
	case TOK_FALSE:
		printf("false");
		fetch(p);
		return true;
	case TOK_TRUE:
		fetch(p);
		printf("true");
		return true;
	case TOK_NULL:
		printf("null");
		fetch(p);
		return true;
	}

	return _method_or_attr(p, t, n);
}

static bool _method_or_attr(struct parser * p,
							struct tree * t, int parent)
{
	unsigned int n = tree_add_node(t, parent, MEMBER_EXP);
	struct token tok;

/*
method_or_attr	: <id> 
				| <id> '.' method_or_attr
				| <id> '[' exp ']'
				| <id> '[' exp ']' '.' method_or_attr
				| <id> '(' arg_list_opt ')'
				| <id> '(' arg_list_opt ')' '.' method_or_attr
*/

	if (lookahead(p) != TOK_ID)
		return false;

	tok = p->tok;

	while (match(p, TOK_ID)) {

		DBG("%s", tok2str(tok));

		switch (lookahead(p)) {

		case TOK_LEFTBRACKET:
			fetch(p);
			DBG("'['");
			if (!(_exp(p, t, n) && match(p, TOK_RIGHTBRACKET))) 
				return false;
			DBG("']'");

			if (!match(p, TOK_DOT))
				return true;
			break;

		case TOK_LEFTPAREN: 
			fetch(p);
			DBG("'('");
			if (!(_arg_list_opt(p, t, n) && match(p, TOK_RIGHTPAREN)))
				return false;
			DBG("')'");

			break;
		}

		if (!match(p, TOK_DOT))
			return true;

		DBG("'.'");
		tok = p->tok;
	}

	return false;
}


int test(void)
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

