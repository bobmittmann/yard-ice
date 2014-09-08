
/***********  LL(1) Parser  ************/

#include <stdio.h>

#include "exp1.y_par.h"


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

void prog() 
{
	switch (lookahead) {
	case LPAREN: case INT: 
		exp(); 
		match(EOF); 
		break;

	default:
		Error(0);
	}
} /* end of prog*/

void exp() 
{
	switch (lookahead) {
	case LPAREN: case INT: 
		mult_exp(); 
		exp1(); 
		break;

	default:
		Error(1);
	}
} /* end of exp*/

void mult_exp() 
{
	switch (lookahead) {
	case LPAREN: case INT: 
		factor(); 
		mult_exp1(); 
		break;

	default:
		Error(2);
	}
} /* end of mult_exp*/

void exp1() 
{
	switch (lookahead) {
	case EOF: case RPAREN: 
		break;

	case PLUS: 
		match(PLUS); 
		exp(); 
		break;

	case MINUS: 
		match(MINUS); 
		exp(); 
		break;

	default:
		Error(3);
	}
} /* end of exp1*/

void factor() 
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

	default:
		Error(4);
	}
} /* end of factor*/

void mult_exp1() 
{
	switch (lookahead) {
	case EOF: case PLUS: case MINUS: case RPAREN: 
		break;

	case MUL: 
		match(MUL); 
		mult_exp(); 
		break;

	case DIV: 
		match(DIV); 
		mult_exp(); 
		break;

	default:
		Error(5);
	}
} /* end of mult_exp1*/

