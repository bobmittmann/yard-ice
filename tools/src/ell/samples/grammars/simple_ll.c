/* LL(1) Embeddedd Nonrecursive Predictive Parser */

#include "simple_ll.h"

/* Token-Rule pair (predict set) */
struct tr_pair {
	uint8_t t;
	uint8_t r;
};

/* Predict sets */
static const struct tr_pair predict_vec[] = {
	/*  28 (program) */
	{  0,  0}, { 21,  1}, { 23,  1}, 
	/*  29 (assign_exp) */
	{ 21,  2}, { 23,  3}, 
	/*  30 (int_exp) */
	{  4,  4}, { 15,  4}, { 26,  4}, { 27,  4}, 
	/*  31 (fix_exp) */
	{  4, 17}, { 15, 17}, { 26, 17}, { 27, 17}, 
	/*  32 (int_mul_exp) */
	{  4,  8}, { 15,  8}, { 26,  8}, { 27,  8}, 
	/*  33 (int_add_exp) */
	{  3,  6}, {  4,  7}, { 16,  5}, 
	/*  34 (int_uny_exp) */
	{  4, 13}, { 15, 12}, { 26, 12}, { 27, 12}, 
	/*  35 (int_mul_exp1) */
	{  3,  9}, {  4,  9}, { 16,  9}, { 24, 10}, { 25, 11}, 
	/*  36 (int_pri_exp) */
	{ 15, 16}, { 26, 14}, { 27, 15}, 
	/*  37 (fix_mul_exp) */
	{  4, 21}, { 15, 21}, { 26, 21}, { 27, 21}, 
	/*  38 (fix_add_exp) */
	{  3, 19}, {  4, 20}, { 16, 18}, 
	/*  39 (fix_uny_exp) */
	{  4, 26}, { 15, 25}, { 26, 25}, { 27, 25}, 
	/*  40 (fix_mul_exp1) */
	{  3, 22}, {  4, 22}, { 16, 22}, { 24, 23}, { 25, 24}, 
	/*  41 (fix_pri_exp) */
	{ 15, 29}, { 26, 28}, { 27, 27}, 
};
/* 102 bytes; */

static const struct {
	uint8_t off;
	uint8_t cnt;
} predict_idx[] = {
	{   0,  3},
	{   3,  2},
	{   5,  4},
	{   9,  4},
	{  13,  4},
	{  17,  3},
	{  20,  4},
	{  24,  5},
	{  29,  3},
	{  32,  4},
	{  36,  3},
	{  39,  4},
	{  43,  5},
	{  48,  3},
};
/* 28 bytes */

/* Rules vectors table */
static const uint8_t rule_vec[] = {
	/*  0:program(1) -> */
	T_EOF, 
	/*  1:program(1) -> */
	N_ASSIGN_EXP, 
	/*  2:assign_exp(3) -> */
	T_INT_VAR, T_EQUALS, N_INT_EXP, 
	/*  3:assign_exp(3) -> */
	T_FIX_VAR, T_EQUALS, N_FIX_EXP, 
	/*  4:int_exp(2) -> */
	N_INT_MUL_EXP, N_INT_ADD_EXP, 
	/*  5:int_add_exp(0) -> */
	
	/*  6:int_add_exp(2) -> */
	T_PLUS, N_INT_ADD_EXP, 
	/*  7:int_add_exp(2) -> */
	T_MINUS, N_INT_ADD_EXP, 
	/*  8:int_mul_exp(2) -> */
	N_INT_UNY_EXP, N_INT_MUL_EXP1, 
	/*  9:int_mul_exp1(0) -> */
	
	/* 10:int_mul_exp1(2) -> */
	T_STAR, N_INT_MUL_EXP, 
	/* 11:int_mul_exp1(2) -> */
	T_SLASH, N_INT_MUL_EXP, 
	/* 12:int_uny_exp(1) -> */
	N_INT_PRI_EXP, 
	/* 13:int_uny_exp(2) -> */
	T_MINUS, N_INT_UNY_EXP, 
	/* 14:int_pri_exp(1) -> */
	T_INT_NUM, 
	/* 15:int_pri_exp(2) -> */
	T_FIX_NUM, A_OP_FIX2INT, 
	/* 16:int_pri_exp(3) -> */
	T_LPAREN, N_INT_EXP, T_RPAREN, 
	/* 17:fix_exp(2) -> */
	N_FIX_MUL_EXP, N_FIX_ADD_EXP, 
	/* 18:fix_add_exp(0) -> */
	
	/* 19:fix_add_exp(2) -> */
	T_PLUS, N_FIX_ADD_EXP, 
	/* 20:fix_add_exp(2) -> */
	T_MINUS, N_FIX_ADD_EXP, 
	/* 21:fix_mul_exp(2) -> */
	N_FIX_UNY_EXP, N_FIX_MUL_EXP1, 
	/* 22:fix_mul_exp1(0) -> */
	
	/* 23:fix_mul_exp1(2) -> */
	T_STAR, N_FIX_MUL_EXP, 
	/* 24:fix_mul_exp1(2) -> */
	T_SLASH, N_FIX_MUL_EXP, 
	/* 25:fix_uny_exp(1) -> */
	N_FIX_PRI_EXP, 
	/* 26:fix_uny_exp(2) -> */
	T_MINUS, N_FIX_UNY_EXP, 
	/* 27:fix_pri_exp(1) -> */
	T_FIX_NUM, 
	/* 28:fix_pri_exp(2) -> */
	T_INT_NUM, A_OP_INT2FIX, 
	/* 29:fix_pri_exp(3) -> */
	T_LPAREN, N_FIX_EXP, T_RPAREN, 
};
/* 50 bytes */

static const struct {
	uint8_t off;
	uint8_t cnt;
} rule_idx[] = {
	{   0,  1},
	{   1,  1},
	{   2,  3},
	{   5,  3},
	{   8,  2},
	{  10,  0},
	{  10,  2},
	{  12,  2},
	{  14,  2},
	{  16,  0},
	{  16,  2},
	{  18,  2},
	{  20,  1},
	{  21,  2},
	{  23,  1},
	{  24,  2},
	{  26,  3},
	{  29,  2},
	{  31,  0},
	{  31,  2},
	{  33,  2},
	{  35,  2},
	{  37,  0},
	{  37,  2},
	{  39,  2},
	{  41,  1},
	{  42,  2},
	{  44,  1},
	{  45,  2},
	{  47,  3},
};
/* 60 bytes */

/* Total: 240 bytes */


int simple_ll_push(uint8_t * sp, unsigned int sym, unsigned int tok)
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

int simple_ll_start(uint8_t * sp)
{
	sp[-1] = T_EOF;
	sp[-2] = N_PROGRAM;

	return 2;
}

const const char * const simple_ll_sym[] = {
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
 	"INT_VAR",
 	"EQUALS",
 	"FIX_VAR",
 	"STAR",
 	"SLASH",
 	"INT_NUM",
 	"FIX_NUM",
 	"program",
 	"assign_exp",
 	"int_exp",
 	"fix_exp",
 	"int_mul_exp",
 	"int_add_exp",
 	"int_uny_exp",
 	"int_mul_exp1",
 	"int_pri_exp",
 	"fix_mul_exp",
 	"fix_add_exp",
 	"fix_uny_exp",
 	"fix_mul_exp1",
 	"fix_pri_exp",
 	"op_fix2int",
 	"op_int2fix",
 };

