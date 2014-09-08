
/***********  LL(1) Parser  ************/

#include <stdio.h>

#include "exp.y_par.h"


int lookahead;

void SyntaxError(int code) 
{
	printf("Error:%d\n",code);
	exit(1); 
}

void match(int t) 
{
	 if(lookahead == t) lookahead = getToken();
	 else SyntaxError(-1);
}

void program() 
{
	switch (lookahead) {
	case EOF: 
		match(EOF); 
		break;

	case ID: case SEMICOLON: case LBRACE: case FUNCTION: case VAR: case IF: case WHILE: case BREAK: case CONTINUE: case RETURN: 
		element(); 
		program(); 
		break;

	default:
		Error(0);
	}
} /* end of program*/

void element() 
{
	switch (lookahead) {
	case FUNCTION: 
		match(FUNCTION); 
		match(ID); 
		match(LPAREN); 
		param_list_opt(); 
		match(RPAREN); 
		compound_stat(); 
		break;

	case ID: case SEMICOLON: case LBRACE: case VAR: case IF: case WHILE: case BREAK: case CONTINUE: case RETURN: 
		stat(); 
		break;

	default:
		Error(1);
	}
} /* end of element*/

void param_list_opt() 
{
	switch (lookahead) {
	case RPAREN: 
		break;

	case ID: 
		param_list(); 
		break;

	default:
		Error(2);
	}
} /* end of param_list_opt*/

void compound_stat() 
{
	switch (lookahead) {
	case LBRACE: 
		match(LBRACE); 
		stat_list(); 
		match(RBRACE); 
		break;

	default:
		Error(3);
	}
} /* end of compound_stat*/

void stat() 
{
	switch (lookahead) {
	case SEMICOLON: 
		match(SEMICOLON); 
		break;

	case IF: 
		match(IF); 
		condition(); 
		compound_stat(); 
		else_opt(); 
		break;

	case WHILE: 
		match(WHILE); 
		condition(); 
		compound_stat(); 
		break;

	case BREAK: 
		match(BREAK); 
		match(SEMICOLON); 
		break;

	case CONTINUE: 
		match(CONTINUE); 
		match(SEMICOLON); 
		break;

	case RETURN: 
		match(RETURN); 
		exp_opt(); 
		match(SEMICOLON); 
		break;

	case VAR: 
		match(VAR); 
		var_list(); 
		match(SEMICOLON); 
		break;

	case LBRACE: 
		compound_stat(); 
		break;

	case ID: 
		assign_or_call(); 
		match(SEMICOLON); 
		break;

	default:
		Error(4);
	}
} /* end of stat*/

void param_list() 
{
	switch (lookahead) {
	case ID: 
		match(ID); 
		param_list1(); 
		break;

	default:
		Error(5);
	}
} /* end of param_list*/

void param_list1() 
{
	switch (lookahead) {
	case RPAREN: 
		break;

	case COMMA: 
		match(COMMA); 
		param_list(); 
		break;

	default:
		Error(6);
	}
} /* end of param_list1*/

void condition() 
{
	switch (lookahead) {
	case LPAREN: 
		match(LPAREN); 
		exp(); 
		match(RPAREN); 
		break;

	default:
		Error(7);
	}
} /* end of condition*/

void else_opt() 
{
	switch (lookahead) {
	case EOF: case ID: case SEMICOLON: case LBRACE: case RBRACE: case FUNCTION: case VAR: case IF: case WHILE: case BREAK: case CONTINUE: case RETURN: 
		break;

	case ELSE: 
		match(ELSE); 
		compound_stat(); 
		break;

	default:
		Error(8);
	}
} /* end of else_opt*/

void exp_opt() 
{
	switch (lookahead) {
	case SEMICOLON: 
		break;

	case PLUS: case MINUS: case LPAREN: case INT: case FALSE: case TRUE: case NULL: case ID: case LBKT: case INV: case NOT: 
		exp(); 
		break;

	default:
		Error(9);
	}
} /* end of exp_opt*/

void var_list() 
{
	switch (lookahead) {
	case ID: 
		var(); 
		var_list1(); 
		break;

	default:
		Error(10);
	}
} /* end of var_list*/

void assign_or_call() 
{
	switch (lookahead) {
	case ID: 
		match(ID); 
		assign_or_call1(); 
		break;

	default:
		Error(11);
	}
} /* end of assign_or_call*/

void exp() 
{
	switch (lookahead) {
	case PLUS: case MINUS: case LPAREN: case INT: case FALSE: case TRUE: case NULL: case ID: case INV: case NOT: 
		shift_exp(); 
		exp1(); 
		break;

	case LBKT: 
		match(LBKT); 
		arg_list_opt(); 
		match(RBKT); 
		break;

	default:
		Error(12);
	}
} /* end of exp*/

void stat_list() 
{
	switch (lookahead) {
	case RBRACE: 
		break;

	case ID: case SEMICOLON: case LBRACE: case VAR: case IF: case WHILE: case BREAK: case CONTINUE: case RETURN: 
		stat(); 
		stat_list(); 
		break;

	default:
		Error(13);
	}
} /* end of stat_list*/

void var() 
{
	switch (lookahead) {
	case ID: 
		match(ID); 
		var1(); 
		break;

	default:
		Error(14);
	}
} /* end of var*/

void var_list1() 
{
	switch (lookahead) {
	case SEMICOLON: 
		break;

	case COMMA: 
		match(COMMA); 
		var_list(); 
		break;

	default:
		Error(15);
	}
} /* end of var_list1*/

void var1() 
{
	switch (lookahead) {
	case COMMA: case SEMICOLON: 
		break;

	case ASSIGN: 
		match(ASSIGN); 
		exp(); 
		break;

	default:
		Error(16);
	}
} /* end of var1*/

void assign_or_call1() 
{
	switch (lookahead) {
	case ASSIGN: 
		match(ASSIGN); 
		exp(); 
		break;

	case DOT: 
		match(DOT); 
		assign_or_call(); 
		break;

	case LBKT: 
		match(LBKT); 
		exp(); 
		match(RBKT); 
		assign_or_call2(); 
		break;

	case LPAREN: 
		match(LPAREN); 
		arg_list_opt(); 
		match(RPAREN); 
		assign_or_call3(); 
		break;

	default:
		Error(17);
	}
} /* end of assign_or_call1*/

void assign_or_call2() 
{
	switch (lookahead) {
	case ASSIGN: 
		match(ASSIGN); 
		exp(); 
		break;

	case DOT: 
		match(DOT); 
		assign_or_call(); 
		break;

	default:
		Error(18);
	}
} /* end of assign_or_call2*/

void arg_list_opt() 
{
	switch (lookahead) {
	case RPAREN: case RBKT: 
		break;

	case PLUS: case MINUS: case LPAREN: case INT: case FALSE: case TRUE: case NULL: case ID: case LBKT: case INV: case NOT: 
		arg_list(); 
		break;

	default:
		Error(19);
	}
} /* end of arg_list_opt*/

void assign_or_call3() 
{
	switch (lookahead) {
	case SEMICOLON: 
		break;

	case DOT: 
		match(DOT); 
		assign_or_call(); 
		break;

	default:
		Error(20);
	}
} /* end of assign_or_call3*/

void shift_exp() 
{
	switch (lookahead) {
	case PLUS: case MINUS: case LPAREN: case INT: case FALSE: case TRUE: case NULL: case ID: case INV: case NOT: 
		additive_exp(); 
		shift_exp1(); 
		break;

	default:
		Error(21);
	}
} /* end of shift_exp*/

void exp1() 
{
	switch (lookahead) {
	case RPAREN: case RBKT: case COMMA: case SEMICOLON: 
		break;

	case LT: 
		match(LT); 
		exp(); 
		break;

	case GT: 
		match(GT); 
		exp(); 
		break;

	case LTE: 
		match(LTE); 
		exp(); 
		break;

	case GTE: 
		match(GTE); 
		exp(); 
		break;

	case EQU: 
		match(EQU); 
		exp(); 
		break;

	case NEQ: 
		match(NEQ); 
		exp(); 
		break;

	default:
		Error(22);
	}
} /* end of exp1*/

void additive_exp() 
{
	switch (lookahead) {
	case PLUS: case MINUS: case LPAREN: case INT: case FALSE: case TRUE: case NULL: case ID: case INV: case NOT: 
		mult_exp(); 
		additive_exp1(); 
		break;

	default:
		Error(23);
	}
} /* end of additive_exp*/

void shift_exp1() 
{
	switch (lookahead) {
	case LT: case GT: case LTE: case GTE: case EQU: case NEQ: case RPAREN: case RBKT: case COMMA: case SEMICOLON: 
		break;

	case SHL: 
		match(SHL); 
		shift_exp(); 
		break;

	case ASR: 
		match(ASR); 
		shift_exp(); 
		break;

	default:
		Error(24);
	}
} /* end of shift_exp1*/

void mult_exp() 
{
	switch (lookahead) {
	case PLUS: case MINUS: case LPAREN: case INT: case FALSE: case TRUE: case NULL: case ID: case INV: case NOT: 
		unary_exp(); 
		mult_exp1(); 
		break;

	default:
		Error(25);
	}
} /* end of mult_exp*/

void additive_exp1() 
{
	switch (lookahead) {
	case LT: case GT: case LTE: case GTE: case EQU: case NEQ: case SHL: case ASR: case RPAREN: case RBKT: case COMMA: case SEMICOLON: 
		break;

	case PLUS: 
		match(PLUS); 
		additive_exp(); 
		break;

	case MINUS: 
		match(MINUS); 
		additive_exp(); 
		break;

	case BITOR: 
		match(BITOR); 
		additive_exp(); 
		break;

	case XOR: 
		match(XOR); 
		additive_exp(); 
		break;

	case OR: 
		match(OR); 
		additive_exp(); 
		break;

	default:
		Error(26);
	}
} /* end of additive_exp1*/

void unary_exp() 
{
	switch (lookahead) {
	case LPAREN: case INT: case FALSE: case TRUE: case NULL: case ID: 
		primary_exp(); 
		break;

	case INV: 
		match(INV); 
		unary_exp(); 
		break;

	case NOT: 
		match(NOT); 
		unary_exp(); 
		break;

	case PLUS: 
		match(PLUS); 
		unary_exp(); 
		break;

	case MINUS: 
		match(MINUS); 
		unary_exp(); 
		break;

	default:
		Error(27);
	}
} /* end of unary_exp*/

void mult_exp1() 
{
	switch (lookahead) {
	case LT: case GT: case LTE: case GTE: case EQU: case NEQ: case SHL: case ASR: case PLUS: case MINUS: case BITOR: case XOR: case OR: case RPAREN: case RBKT: case COMMA: case SEMICOLON: 
		break;

	case MUL: 
		match(MUL); 
		mult_exp(); 
		break;

	case DIV: 
		match(DIV); 
		mult_exp(); 
		break;

	case MOD: 
		match(MOD); 
		mult_exp(); 
		break;

	case BITAND: 
		match(BITAND); 
		mult_exp(); 
		break;

	case AND: 
		match(AND); 
		mult_exp(); 
		break;

	default:
		Error(28);
	}
} /* end of mult_exp1*/

void primary_exp() 
{
	switch (lookahead) {
	case LPAREN: 
		match(LPAREN); 
		exp(); 
		match(RPAREN); 
		break;

	case INT: 
		match(INT); 
		break;

	case FALSE: 
		match(FALSE); 
		break;

	case TRUE: 
		match(TRUE); 
		break;

	case NULL: 
		match(NULL); 
		break;

	case ID: 
		met_or_attr(); 
		break;

	default:
		Error(29);
	}
} /* end of primary_exp*/

void met_or_attr() 
{
	switch (lookahead) {
	case ID: 
		match(ID); 
		met_or_attr1(); 
		break;

	default:
		Error(30);
	}
} /* end of met_or_attr*/

void met_or_attr1() 
{
	switch (lookahead) {
	case LT: case GT: case LTE: case GTE: case EQU: case NEQ: case SHL: case ASR: case PLUS: case MINUS: case BITOR: case XOR: case OR: case MUL: case DIV: case MOD: case BITAND: case AND: case RPAREN: case RBKT: case COMMA: case SEMICOLON: 
		break;

	case DOT: 
		match(DOT); 
		met_or_attr(); 
		break;

	case LBKT: 
		match(LBKT); 
		exp(); 
		match(RBKT); 
		met_or_attr2(); 
		break;

	case LPAREN: 
		match(LPAREN); 
		arg_list_opt(); 
		match(RPAREN); 
		met_or_attr2(); 
		break;

	default:
		Error(31);
	}
} /* end of met_or_attr1*/

void met_or_attr2() 
{
	switch (lookahead) {
	case LT: case GT: case LTE: case GTE: case EQU: case NEQ: case SHL: case ASR: case PLUS: case MINUS: case BITOR: case XOR: case OR: case MUL: case DIV: case MOD: case BITAND: case AND: case RPAREN: case RBKT: case COMMA: case SEMICOLON: 
		break;

	case DOT: 
		match(DOT); 
		met_or_attr(); 
		break;

	default:
		Error(32);
	}
} /* end of met_or_attr2*/

void arg_list() 
{
	switch (lookahead) {
	case PLUS: case MINUS: case LPAREN: case INT: case FALSE: case TRUE: case NULL: case ID: case LBKT: case INV: case NOT: 
		exp(); 
		arg_list1(); 
		break;

	default:
		Error(33);
	}
} /* end of arg_list*/

void arg_list1() 
{
	switch (lookahead) {
	case RPAREN: case RBKT: 
		break;

	case COMMA: 
		match(COMMA); 
		arg_list(); 
		break;

	default:
		Error(34);
	}
} /* end of arg_list1*/

