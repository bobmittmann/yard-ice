
/***********  LL(1) Parser  ************/

#include <stdio.h>

#include "types_par.h"


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

	case FOR: case IF: case THROW: case TRY: case WHILE: case BOOL: case INT: case FIX: 
		stat(); 
		program(); 
		break;

	case LBRACE: 
		compound_stat(); 
		program(); 
		break;

	default:
		Error(0);
	}
} /* end of program*/

void stat() 
{
	switch (lookahead) {
	case FOR: 
		match(FOR); 
		match(LPAREN); 
		for_lst_opt(); 
		match(SEMICOLON); 
		int_exp(); 
		match(SEMICOLON); 
		for_lst_opt(); 
		match(RPAREN); 
		compound_stat(); 
		break;

	case WHILE: 
		match(WHILE); 
		condition(); 
		compound_stat(); 
		break;

	case IF: 
		match(IF); 
		condition(); 
		compound_stat(); 
		else_opt(); 
		break;

	case TRY: 
		match(TRY); 
		compound_stat(); 
		catch_opt(); 
		break;

	case THROW: 
		match(THROW); 
		int_exp(); 
		match(SEMICOLON); 
		break;

	case BOOL: 
		match(BOOL); 
		bool_var_list(); 
		match(SEMICOLON); 
		break;

	case INT: 
		match(INT); 
		int_var_list(); 
		match(SEMICOLON); 
		break;

	case FIX: 
		match(FIX); 
		fix_var_list(); 
		match(SEMICOLON); 
		break;

	default:
		Error(1);
	}
} /* end of stat*/

void compound_stat() 
{
	switch (lookahead) {
	case LBRACE: 
		match(LBRACE); 
		stat_list(); 
		match(RBRACE); 
		break;

	default:
		Error(2);
	}
} /* end of compound_stat*/

void stat_list() 
{
	switch (lookahead) {
	case RBRACE: 
		break;

	case FOR: case IF: case THROW: case TRY: case WHILE: case BOOL: case INT: case FIX: 
		stat(); 
		stat_list(); 
		break;

	default:
		Error(3);
	}
} /* end of stat_list*/

void for_lst_opt() 
{
	switch (lookahead) {
	case SEMICOLON: case RPAREN: 
		break;

	case INT_VAR: case INT_FUN: case FIX_VAR: case FIX_FUN: case BOOL_VAR: case BOOL_FUN: 
		for_lst(); 
		break;

	default:
		Error(4);
	}
} /* end of for_lst_opt*/

void int_exp() 
{
	switch (lookahead) {
	case TILDE: case LPAREN: case INT_VAL: case CHAR: case INT_VAR: case INT_FUN: case FIX2INT: case BOOL2INT: 
		and_exp(); 
		or_exp(); 
		break;

	default:
		Error(5);
	}
} /* end of int_exp*/

void condition() 
{
	switch (lookahead) {
	case LPAREN: 
		match(LPAREN); 
		int_exp(); 
		match(RPAREN); 
		break;

	default:
		Error(6);
	}
} /* end of condition*/

void else_opt() 
{
	switch (lookahead) {
	case EOF: case LBRACE: case RBRACE: case FOR: case IF: case THROW: case TRY: case WHILE: case BOOL: case INT: case FIX: 
		break;

	case ELSE: 
		match(ELSE); 
		compound_stat(); 
		break;

	default:
		Error(7);
	}
} /* end of else_opt*/

void catch_opt() 
{
	switch (lookahead) {
	case EOF: case LBRACE: case RBRACE: case FOR: case IF: case THROW: case TRY: case WHILE: case BOOL: case INT: case FIX: 
		break;

	case CATCH: 
		match(CATCH); 
		match(LPAREN); 
		match(ID); 
		match(RPAREN); 
		match(LBRACE); 
		stat_list(); 
		match(RBRACE); 
		break;

	default:
		Error(8);
	}
} /* end of catch_opt*/

void bool_var_list() 
{
	switch (lookahead) {
	case ID: 
		bool_var(); 
		bool_var_list1(); 
		break;

	default:
		Error(9);
	}
} /* end of bool_var_list*/

void int_var_list() 
{
	switch (lookahead) {
	case ID: 
		int_var(); 
		int_var_list1(); 
		break;

	default:
		Error(10);
	}
} /* end of int_var_list*/

void fix_var_list() 
{
	switch (lookahead) {
	case ID: 
		fix_var(); 
		fix_var_list1(); 
		break;

	default:
		Error(11);
	}
} /* end of fix_var_list*/

void assign_or_call() 
{
	switch (lookahead) {
	case INT_VAR: 
		match(INT_VAR); 
		match(EQUALS); 
		int_exp(); 
		match(SEMICOLON); 
		break;

	case INT_FUN: 
		match(INT_FUN); 
		function_call(); 
		match(SEMICOLON); 
		break;

	case FIX_VAR: 
		match(FIX_VAR); 
		match(EQUALS); 
		fix_exp(); 
		match(SEMICOLON); 
		break;

	case FIX_FUN: 
		match(FIX_FUN); 
		function_call(); 
		match(SEMICOLON); 
		break;

	case BOOL_VAR: 
		match(BOOL_VAR); 
		match(EQUALS); 
		bool_exp(); 
		match(SEMICOLON); 
		break;

	case BOOL_FUN: 
		match(BOOL_FUN); 
		function_call(); 
		match(SEMICOLON); 
		break;

	default:
		Error(12);
	}
} /* end of assign_or_call*/

void function_call() 
{
	switch (lookahead) {
	case LPAREN: 
		match(LPAREN); 
		arg_list_opt(); 
		match(RPAREN); 
		break;

	default:
		Error(13);
	}
} /* end of function_call*/

void fix_exp() 
{
	switch (lookahead) {
	case MINUS: case FIX: case FIX_VAR: 
		fix_add_exp(); 
		break;

	default:
		Error(14);
	}
} /* end of fix_exp*/

void bool_exp() 
{
	switch (lookahead) {
	case LPAREN: case FALSE: case TRUE: case NOT: 
		bool_and_exp(); 
		bool_or_exp(); 
		break;

	default:
		Error(15);
	}
} /* end of bool_exp*/

void arg_list_opt() 
{
	switch (lookahead) {
	case RPAREN: 
		break;

	case TILDE: case LPAREN: case INT_VAL: case CHAR: case INT_VAR: case INT_FUN: case FIX2INT: case BOOL2INT: 
		arg_list(); 
		break;

	default:
		Error(16);
	}
} /* end of arg_list_opt*/

void arg_list() 
{
	switch (lookahead) {
	case TILDE: case LPAREN: case INT_VAL: case CHAR: case INT_VAR: case INT_FUN: case FIX2INT: case BOOL2INT: 
		exp(); 
		arg_list1(); 
		break;

	default:
		Error(17);
	}
} /* end of arg_list*/

void exp() 
{
	switch (lookahead) {
	case TILDE: case LPAREN: case INT_VAL: case CHAR: case INT_VAR: case INT_FUN: case FIX2INT: case BOOL2INT: 
		int_exp(); 
		break;

	default:
		Error(18);
	}
} /* end of exp*/

void arg_list1() 
{
	switch (lookahead) {
	case RPAREN: 
		break;

	case COMMA: 
		match(COMMA); 
		arg_list(); 
		break;

	default:
		Error(19);
	}
} /* end of arg_list1*/

void for_lst() 
{
	switch (lookahead) {
	case INT_VAR: case INT_FUN: case FIX_VAR: case FIX_FUN: case BOOL_VAR: case BOOL_FUN: 
		assign_or_call(); 
		for_lst1(); 
		break;

	default:
		Error(20);
	}
} /* end of for_lst*/

void for_lst1() 
{
	switch (lookahead) {
	case SEMICOLON: case RPAREN: 
		break;

	case COMMA: 
		match(COMMA); 
		for_lst(); 
		break;

	default:
		Error(21);
	}
} /* end of for_lst1*/

void bool_var() 
{
	switch (lookahead) {
	case ID: 
		match(ID); 
		bool_var_assign_opt(); 
		break;

	default:
		Error(22);
	}
} /* end of bool_var*/

void bool_var_list1() 
{
	switch (lookahead) {
	case SEMICOLON: 
		break;

	case COMMA: 
		match(COMMA); 
		bool_var_list(); 
		break;

	default:
		Error(23);
	}
} /* end of bool_var_list1*/

void bool_var_assign_opt() 
{
	switch (lookahead) {
	case COMMA: case SEMICOLON: 
		break;

	case EQUALS: 
		match(EQUALS); 
		bool_exp(); 
		break;

	default:
		Error(24);
	}
} /* end of bool_var_assign_opt*/

void bool_and_exp() 
{
	switch (lookahead) {
	case LPAREN: case FALSE: case TRUE: case NOT: 
		bool_unary_exp(); 
		bool_and_exp1(); 
		break;

	default:
		Error(25);
	}
} /* end of bool_and_exp*/

void bool_or_exp() 
{
	switch (lookahead) {
	case COMMA: case SEMICOLON: case RPAREN: 
		break;

	case LOGICOR: 
		match(LOGICOR); 
		bool_exp(); 
		break;

	default:
		Error(26);
	}
} /* end of bool_or_exp*/

void bool_unary_exp() 
{
	switch (lookahead) {
	case LPAREN: case FALSE: case TRUE: 
		bool_pri_exp(); 
		break;

	case NOT: 
		match(NOT); 
		bool_unary_exp(); 
		break;

	default:
		Error(27);
	}
} /* end of bool_unary_exp*/

void bool_and_exp1() 
{
	switch (lookahead) {
	case COMMA: case SEMICOLON: case RPAREN: case LOGICOR: 
		break;

	case LOGICAND: 
		match(LOGICAND); 
		bool_and_exp(); 
		break;

	default:
		Error(28);
	}
} /* end of bool_and_exp1*/

void bool_pri_exp() 
{
	switch (lookahead) {
	case TRUE: 
		match(TRUE); 
		break;

	case FALSE: 
		match(FALSE); 
		break;

	case LPAREN: 
		match(LPAREN); 
		bool_exp(); 
		match(RPAREN); 
		break;

	default:
		Error(29);
	}
} /* end of bool_pri_exp*/

void rel_exp() 
{
	switch (lookahead) {
	case TILDE: case LPAREN: case INT_VAL: case CHAR: case INT_VAR: case INT_FUN: case FIX2INT: case BOOL2INT: 
		int_rel_exp(); 
		break;

	default:
		Error(30);
	}
} /* end of rel_exp*/

void int_rel_exp() 
{
	switch (lookahead) {
	case TILDE: case LPAREN: case INT_VAL: case CHAR: case INT_VAR: case INT_FUN: case FIX2INT: case BOOL2INT: 
		int_exp(); 
		int_rel_exp1(); 
		break;

	default:
		Error(31);
	}
} /* end of int_rel_exp*/

void int_var() 
{
	switch (lookahead) {
	case ID: 
		match(ID); 
		int_var_assign_opt(); 
		break;

	default:
		Error(32);
	}
} /* end of int_var*/

void int_var_list1() 
{
	switch (lookahead) {
	case SEMICOLON: 
		break;

	case COMMA: 
		match(COMMA); 
		int_var_list(); 
		break;

	default:
		Error(33);
	}
} /* end of int_var_list1*/

void int_var_assign_opt() 
{
	switch (lookahead) {
	case COMMA: case SEMICOLON: 
		break;

	case EQUALS: 
		match(EQUALS); 
		int_exp(); 
		break;

	default:
		Error(34);
	}
} /* end of int_var_assign_opt*/

void int_rel_exp1() 
{
	switch (lookahead) {
	case LESSTHEN: 
		match(LESSTHEN); 
		int_exp(); 
		break;

	case GREATTHEN: 
		match(GREATTHEN); 
		int_exp(); 
		break;

	case EQU: 
		match(EQU); 
		int_exp(); 
		break;

	case NEQ: 
		match(NEQ); 
		int_exp(); 
		break;

	case GTE: 
		match(GTE); 
		int_exp(); 
		break;

	case LTE: 
		match(LTE); 
		int_exp(); 
		break;

	default:
		Error(35);
	}
} /* end of int_rel_exp1*/

void and_exp() 
{
	switch (lookahead) {
	case TILDE: case LPAREN: case INT_VAL: case CHAR: case INT_VAR: case INT_FUN: case FIX2INT: case BOOL2INT: 
		shift_exp(); 
		and_exp1(); 
		break;

	default:
		Error(36);
	}
} /* end of and_exp*/

void or_exp() 
{
	switch (lookahead) {
	case COMMA: case SEMICOLON: case LESSTHEN: case GREATTHEN: case RPAREN: case GTE: case LTE: case EQU: case NEQ: 
		break;

	case BAR: 
		match(BAR); 
		int_exp(); 
		break;

	case CARET: 
		match(CARET); 
		int_exp(); 
		break;

	default:
		Error(37);
	}
} /* end of or_exp*/

void shift_exp() 
{
	switch (lookahead) {
	case TILDE: case LPAREN: case INT_VAL: case CHAR: case INT_VAR: case INT_FUN: case FIX2INT: case BOOL2INT: 
		int_add_exp(); 
		shift_exp1(); 
		break;

	default:
		Error(38);
	}
} /* end of shift_exp*/

void and_exp1() 
{
	switch (lookahead) {
	case COMMA: case SEMICOLON: case BAR: case CARET: case LESSTHEN: case GREATTHEN: case RPAREN: case GTE: case LTE: case EQU: case NEQ: 
		break;

	case AMPERSAND: 
		match(AMPERSAND); 
		and_exp(); 
		break;

	default:
		Error(39);
	}
} /* end of and_exp1*/

void int_add_exp() 
{
	switch (lookahead) {
	case TILDE: case LPAREN: case INT_VAL: case CHAR: case INT_VAR: case INT_FUN: case FIX2INT: case BOOL2INT: 
		int_mult_exp(); 
		int_add_exp1(); 
		break;

	default:
		Error(40);
	}
} /* end of int_add_exp*/

void shift_exp1() 
{
	switch (lookahead) {
	case COMMA: case SEMICOLON: case AMPERSAND: case BAR: case CARET: case LESSTHEN: case GREATTHEN: case RPAREN: case GTE: case LTE: case EQU: case NEQ: 
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
		Error(41);
	}
} /* end of shift_exp1*/

void int_mult_exp() 
{
	switch (lookahead) {
	case TILDE: case LPAREN: case INT_VAL: case CHAR: case INT_VAR: case INT_FUN: case FIX2INT: case BOOL2INT: 
		int_unary_exp(); 
		int_mult_exp1(); 
		break;

	default:
		Error(42);
	}
} /* end of int_mult_exp*/

void int_add_exp1() 
{
	switch (lookahead) {
	case COMMA: case SEMICOLON: case AMPERSAND: case BAR: case CARET: case LESSTHEN: case GREATTHEN: case RPAREN: case GTE: case LTE: case EQU: case NEQ: case ASR: case SHL: 
		break;

	case PLUS: 
		match(PLUS); 
		int_add_exp(); 
		break;

	case MINUS: 
		match(MINUS); 
		int_add_exp(); 
		break;

	default:
		Error(43);
	}
} /* end of int_add_exp1*/

void int_unary_exp() 
{
	switch (lookahead) {
	case LPAREN: case INT_VAL: case CHAR: case INT_VAR: case INT_FUN: case FIX2INT: case BOOL2INT: 
		int_primary_exp(); 
		break;

	case TILDE: 
		match(TILDE); 
		int_primary_exp(); 
		break;

	default:
		Error(44);
	}
} /* end of int_unary_exp*/

void int_mult_exp1() 
{
	switch (lookahead) {
	case COMMA: case SEMICOLON: case PLUS: case MINUS: case AMPERSAND: case BAR: case CARET: case LESSTHEN: case GREATTHEN: case RPAREN: case GTE: case LTE: case EQU: case NEQ: case ASR: case SHL: 
		break;

	case SLASH: 
		match(SLASH); 
		int_mult_exp(); 
		break;

	case PERCENT: 
		match(PERCENT); 
		int_mult_exp(); 
		break;

	case STAR: 
		match(STAR); 
		int_mult_exp(); 
		break;

	default:
		Error(45);
	}
} /* end of int_mult_exp1*/

void int_primary_exp() 
{
	switch (lookahead) {
	case INT_VAL: 
		match(INT_VAL); 
		break;

	case CHAR: 
		match(CHAR); 
		break;

	case INT_VAR: 
		match(INT_VAR); 
		break;

	case INT_FUN: 
		match(INT_FUN); 
		function_call(); 
		break;

	case FIX2INT: 
		match(FIX2INT); 
		match(LPAREN); 
		fix_exp(); 
		match(RPAREN); 
		break;

	case BOOL2INT: 
		match(BOOL2INT); 
		match(LPAREN); 
		fix_exp(); 
		match(RPAREN); 
		break;

	case LPAREN: 
		match(LPAREN); 
		int_exp(); 
		match(RPAREN); 
		break;

	default:
		Error(46);
	}
} /* end of int_primary_exp*/

void fix_var() 
{
	switch (lookahead) {
	case ID: 
		match(ID); 
		fix_var_assign_opt(); 
		break;

	default:
		Error(47);
	}
} /* end of fix_var*/

void fix_var_list1() 
{
	switch (lookahead) {
	case SEMICOLON: 
		break;

	case COMMA: 
		match(COMMA); 
		fix_var_list(); 
		break;

	default:
		Error(48);
	}
} /* end of fix_var_list1*/

void fix_var_assign_opt() 
{
	switch (lookahead) {
	case COMMA: case SEMICOLON: 
		break;

	case EQUALS: 
		match(EQUALS); 
		fix_exp(); 
		break;

	default:
		Error(49);
	}
} /* end of fix_var_assign_opt*/

void fix_rel_exp() 
{
	switch (lookahead) {
	case MINUS: case FIX: case FIX_VAR: 
		fix_exp(); 
		fix_rel_exp1(); 
		break;

	default:
		Error(50);
	}
} /* end of fix_rel_exp*/

void fix_rel_exp1() 
{
	switch (lookahead) {
	case LESSTHEN: 
		match(LESSTHEN); 
		fix_exp(); 
		break;

	case GREATTHEN: 
		match(GREATTHEN); 
		fix_exp(); 
		break;

	case EQU: 
		match(EQU); 
		fix_exp(); 
		break;

	case NEQ: 
		match(NEQ); 
		fix_exp(); 
		break;

	case GTE: 
		match(GTE); 
		fix_exp(); 
		break;

	case LTE: 
		match(LTE); 
		fix_exp(); 
		break;

	default:
		Error(51);
	}
} /* end of fix_rel_exp1*/

void fix_add_exp() 
{
	switch (lookahead) {
	case MINUS: case FIX: case FIX_VAR: 
		fix_mult_exp(); 
		fix_add_exp1(); 
		break;

	default:
		Error(52);
	}
} /* end of fix_add_exp*/

void fix_mult_exp() 
{
	switch (lookahead) {
	case MINUS: case FIX: case FIX_VAR: 
		fix_unary_exp(); 
		fix_mult_exp1(); 
		break;

	default:
		Error(53);
	}
} /* end of fix_mult_exp*/

void fix_add_exp1() 
{
	switch (lookahead) {
	case COMMA: case SEMICOLON: case LESSTHEN: case GREATTHEN: case RPAREN: case GTE: case LTE: case EQU: case NEQ: 
		break;

	case PLUS: 
		match(PLUS); 
		fix_add_exp(); 
		break;

	case MINUS: 
		match(MINUS); 
		fix_add_exp(); 
		break;

	default:
		Error(54);
	}
} /* end of fix_add_exp1*/

void fix_unary_exp() 
{
	switch (lookahead) {
	case FIX: case FIX_VAR: 
		fix_primary_exp(); 
		break;

	case MINUS: 
		match(MINUS); 
		fix_primary_exp(); 
		break;

	default:
		Error(55);
	}
} /* end of fix_unary_exp*/

void fix_mult_exp1() 
{
	switch (lookahead) {
	case COMMA: case SEMICOLON: case PLUS: case MINUS: case LESSTHEN: case GREATTHEN: case RPAREN: case GTE: case LTE: case EQU: case NEQ: 
		break;

	case SLASH: 
		match(SLASH); 
		fix_mult_exp(); 
		break;

	case STAR: 
		match(STAR); 
		fix_mult_exp(); 
		break;

	default:
		Error(56);
	}
} /* end of fix_mult_exp1*/

void fix_primary_exp() 
{
	switch (lookahead) {
	case FIX: 
		match(FIX); 
		break;

	case FIX_VAR: 
		match(FIX_VAR); 
		break;

	default:
		Error(57);
	}
} /* end of fix_primary_exp*/

