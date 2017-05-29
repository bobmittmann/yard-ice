/* LL(1) Embeddedd Nonrecursive Predictive Parser */

#include "cmd_ll.h"

/* Token-Rule pair (predict set) */
struct tr_pair {
	uint8_t t;
	uint8_t r;
};

/* Predict sets */
static const struct tr_pair predict_vec[] = {
	/*  24 (program) */
	{  0,  0}, { 17,  1}, { 19,  1}, { 22,  1}, 
	/*  25 (stat) */
	{ 17,  2}, { 19,  3}, { 22,  4}, 
	/*  26 (exp) */
	{  4,  5}, { 15,  5}, { 18,  5}, { 19,  5}, { 23,  5}, 
	/*  27 (cmd_arg) */
	{ 15, 24}, { 18, 23}, 
	/*  28 (mult_exp) */
	{  4,  9}, { 15,  9}, { 18,  9}, { 19,  9}, { 23,  9}, 
	/*  29 (additive_exp) */
	{  3,  7}, {  4,  8}, { 16,  6}, { 21,  6}, 
	/*  30 (unary_exp) */
	{  4, 15}, { 15, 14}, { 18, 14}, { 19, 14}, { 23, 16}, 
	/*  31 (mult_exp1) */
	{  3, 10}, {  4, 10}, {  5, 11}, {  6, 12}, {  7, 13}, { 16, 10}, 
	{ 21, 10}, 
	/*  32 (primary_exp) */
	{ 15, 19}, { 18, 17}, { 19, 18}, 
	/*  33 (meth_or_attr) */
	{ 19, 20}, 
	/*  34 (meth_or_attr1) */
	{  3, 21}, {  4, 21}, {  5, 21}, {  6, 21}, {  7, 21}, { 15, 22}, 
	{ 16, 21}, { 21, 21}, 
};
/* 94 bytes; */

static const struct {
	uint8_t off;
	uint8_t cnt;
} predict_idx[] = {
	{   0,  4},
	{   4,  3},
	{   7,  5},
	{  12,  2},
	{  14,  5},
	{  19,  4},
	{  23,  5},
	{  28,  7},
	{  35,  3},
	{  38,  1},
	{  39,  8},
};
/* 22 bytes */

/* Rules vectors table */
static const uint8_t rule_vec[] = {
	/*  0:program(1) -> */
	T_EOF, 
	/*  1:program(2) -> */
	N_STAT, N_PROGRAM, 
	/*  2:stat(3) -> */
	T_VAR, T_ID, T_EOL, 
	/*  3:stat(4) -> */
	T_ID, T_ASSIGN, N_EXP, T_EOL, 
	/*  4:stat(4) -> */
	T_CMD, N_CMD_ARG, N_CMD_ARG, T_EOL, 
	/*  5:exp(2) -> */
	N_MULT_EXP, N_ADDITIVE_EXP, 
	/*  6:additive_exp(0) -> */
	
	/*  7:additive_exp(2) -> */
	T_PLUS, N_ADDITIVE_EXP, 
	/*  8:additive_exp(2) -> */
	T_MINUS, N_ADDITIVE_EXP, 
	/*  9:mult_exp(2) -> */
	N_UNARY_EXP, N_MULT_EXP1, 
	/* 10:mult_exp1(0) -> */
	
	/* 11:mult_exp1(2) -> */
	T_MUL, N_MULT_EXP, 
	/* 12:mult_exp1(2) -> */
	T_DIV, N_MULT_EXP, 
	/* 13:mult_exp1(2) -> */
	T_MOD, N_MULT_EXP, 
	/* 14:unary_exp(1) -> */
	N_PRIMARY_EXP, 
	/* 15:unary_exp(2) -> */
	T_MINUS, N_UNARY_EXP, 
	/* 16:unary_exp(2) -> */
	T_NOT, N_UNARY_EXP, 
	/* 17:primary_exp(1) -> */
	T_INT, 
	/* 18:primary_exp(1) -> */
	N_METH_OR_ATTR, 
	/* 19:primary_exp(3) -> */
	T_LPAREN, N_EXP, T_RPAREN, 
	/* 20:meth_or_attr(2) -> */
	T_ID, N_METH_OR_ATTR1, 
	/* 21:meth_or_attr1(0) -> */
	
	/* 22:meth_or_attr1(2) -> */
	T_LPAREN, T_RPAREN, 
	/* 23:cmd_arg(1) -> */
	T_INT, 
	/* 24:cmd_arg(3) -> */
	T_LPAREN, N_EXP, T_RPAREN, 
};
/* 46 bytes */

static const struct {
	uint8_t off;
	uint8_t cnt;
} rule_idx[] = {
	{   0,  1},
	{   1,  2},
	{   3,  3},
	{   6,  4},
	{  10,  4},
	{  14,  2},
	{  16,  0},
	{  16,  2},
	{  18,  2},
	{  20,  2},
	{  22,  0},
	{  22,  2},
	{  24,  2},
	{  26,  2},
	{  28,  1},
	{  29,  2},
	{  31,  2},
	{  33,  1},
	{  34,  1},
	{  35,  3},
	{  38,  2},
	{  40,  0},
	{  40,  2},
	{  42,  1},
	{  43,  3},
};
/* 50 bytes */

/* Total: 212 bytes */


int cmd_ll_push(uint8_t * sp, unsigned int sym, unsigned int tok)
{
	const struct tr_pair * vec;
	uint8_t * p;
	int imax;
	int imin;
	int n;
	int i;

	/* Get the index for the symbol at the stack's head */
	i = sym - NONTERM_BASE;
	if (i < 0) /* Shuld be nonterminal */
		return -1;
	vec = &predict_vec[predict_idx[i].off];
	imax = predict_idx[i].cnt;
	imin = 1;

	/* Is the production list empty ? !!! 
	if (imax < 0)
		return -1;
	*/

	/* Binary search for the matching rule */
	while (imin < imax) {
		int imid = (imin + imax) / 2;
		if (vec[imid - 1].t < tok) 
			imin = imid + 1;
		else
			imax = imid;
	}

	if (vec[imin - 1].t != tok) {
		/* No matching rule, return error. */
		return -1;
	}

	i = vec[imin - 1].r;
	n = rule_idx[i].cnt;
	p = (uint8_t *)&rule_vec[rule_idx[i].off];
	sp -= n;

	/* Push the production into the stack */
	for (i = 0; i < n; ++i)
		sp[i] = p[i];

	/* Return the lenght of the production */
	return n;
}

int cmd_ll_start(uint8_t * sp)
{
	sp[-1] = T_EOF;
	sp[-2] = N_PROGRAM;

	return 2;
}

const const char * const cmd_ll_sym[] = {
	"EOF",
 	"ASR",
 	"SHL",
 	"PLUS",
 	"MINUS",
 	"MUL",
 	"DIV",
 	"MOD",
 	"OR",
 	"AND",
 	"XOR",
 	"INV",
 	"ASSIGN",
 	"COMMA",
 	"SEMICOLON",
 	"LPAREN",
 	"RPAREN",
 	"VAR",
 	"INT",
 	"ID",
 	"ERR",
 	"EOL",
 	"CMD",
 	"NOT",
 	"program",
 	"stat",
 	"exp",
 	"cmd_arg",
 	"mult_exp",
 	"additive_exp",
 	"unary_exp",
 	"mult_exp1",
 	"primary_exp",
 	"meth_or_attr",
 	"meth_or_attr1",
 };

