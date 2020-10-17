/* LL(1) Embeddedd Nonrecursive Predictive Parser */

#include "calc_ll.h"

/* Token-Rule pair (predict set) */
struct tr_pair {
	uint8_t t;
	uint8_t r;
};

/* Predict sets */
const struct tr_pair predict_vec[] = {
	/*  22 (program) */
	{  0,  0}, { 14,  1}, { 17,  1}, { 19,  1}, { 21,  1}, 
	/*  23 (stat) */
	{ 14,  2}, { 17,  3}, { 19,  4}, { 21,  5}, 
	/*  24 (exp) */
	{  4,  6}, { 11,  6}, { 15,  6}, { 18,  6}, { 19,  6}, 
	/*  25 (additive_exp) */
	{  4, 10}, { 11, 10}, { 15, 10}, { 18, 10}, { 19, 10}, 
	/*  26 (shift_exp1) */
	{  1,  9}, {  2,  8}, { 14,  7}, { 16,  7}, 
	/*  27 (mult_exp) */
	{  4, 16}, { 11, 16}, { 15, 16}, { 18, 16}, { 19, 16}, 
	/*  28 (additive_exp1) */
	{  1, 11}, {  2, 11}, {  3, 12}, {  4, 13}, {  8, 15}, { 10, 14}, 
	{ 14, 11}, { 16, 11}, 
	/*  29 (unary_exp) */
	{  4, 24}, { 11, 23}, { 15, 22}, { 18, 22}, { 19, 22}, 
	/*  30 (mult_exp1) */
	{  1, 17}, {  2, 17}, {  3, 17}, {  4, 17}, {  5, 18}, {  6, 19}, 
	{  7, 20}, {  8, 17}, {  9, 21}, { 10, 17}, { 14, 17}, { 16, 17}, 
	/*  31 (primary_exp) */
	{ 15, 25}, { 18, 26}, { 19, 27}, 
};
/* 112 bytes; */

static const struct {
	uint8_t off;
	uint8_t cnt;
} predict_idx[] = {
	{   0,  5},
	{   5,  4},
	{   9,  5},
	{  14,  5},
	{  19,  4},
	{  23,  5},
	{  28,  8},
	{  36,  5},
	{  41, 12},
	{  53,  3},
};
/* 20 bytes */

/* Rules vectors table */
const uint8_t rule_vec[] = {
	/*  0:program(1) -> */
	T_EOF, 
	/*  1:program(2) -> */
	N_STAT, N_PROGRAM, 
	/*  2:stat(1) -> */
	T_SEMICOLON, 
	/*  3:stat(4) -> */
	T_VAR, T_ID, A_OP_VAR_DECL, T_SEMICOLON, 
	/*  4:stat(6) -> */
	T_ID, A_OP_LOOKUP_ID, T_ASSIGN, N_EXP, T_SEMICOLON, A_OP_ASSIGN, 
	/*  5:stat(4) -> */
	T_PRINT, N_EXP, T_SEMICOLON, A_OP_PRINT, 
	/*  6:exp(2) -> */
	N_ADDITIVE_EXP, N_SHIFT_EXP1, 
	/*  7:shift_exp1(0) -> */
	
	/*  8:shift_exp1(3) -> */
	T_SHL, N_EXP, A_OP_SHL, 
	/*  9:shift_exp1(3) -> */
	T_ASR, N_EXP, A_OP_ASR, 
	/* 10:additive_exp(2) -> */
	N_MULT_EXP, N_ADDITIVE_EXP1, 
	/* 11:additive_exp1(0) -> */
	
	/* 12:additive_exp1(3) -> */
	T_PLUS, N_ADDITIVE_EXP, A_OP_ADD, 
	/* 13:additive_exp1(3) -> */
	T_MINUS, N_ADDITIVE_EXP, A_OP_SUB, 
	/* 14:additive_exp1(3) -> */
	T_XOR, N_ADDITIVE_EXP, A_OP_XOR, 
	/* 15:additive_exp1(3) -> */
	T_OR, N_ADDITIVE_EXP, A_OP_OR, 
	/* 16:mult_exp(2) -> */
	N_UNARY_EXP, N_MULT_EXP1, 
	/* 17:mult_exp1(0) -> */
	
	/* 18:mult_exp1(3) -> */
	T_MUL, N_MULT_EXP, A_OP_MUL, 
	/* 19:mult_exp1(3) -> */
	T_DIV, N_MULT_EXP, A_OP_DIV, 
	/* 20:mult_exp1(3) -> */
	T_MOD, N_MULT_EXP, A_OP_MOD, 
	/* 21:mult_exp1(3) -> */
	T_AND, N_MULT_EXP, A_OP_AND, 
	/* 22:unary_exp(1) -> */
	N_PRIMARY_EXP, 
	/* 23:unary_exp(3) -> */
	T_INV, N_UNARY_EXP, A_OP_INV, 
	/* 24:unary_exp(3) -> */
	T_MINUS, N_UNARY_EXP, A_OP_MINUS, 
	/* 25:primary_exp(3) -> */
	T_LPAREN, N_EXP, T_RPAREN, 
	/* 26:primary_exp(2) -> */
	T_INT, A_OP_PUSH_INT, 
	/* 27:primary_exp(3) -> */
	T_ID, A_OP_LOOKUP_ID, A_OP_PUSH_ID, 
};
/* 69 bytes */

static const struct {
	uint8_t off;
	uint8_t cnt;
} rule_idx[] = {
	{   0,  1},
	{   1,  2},
	{   3,  1},
	{   4,  4},
	{   8,  6},
	{  14,  4},
	{  18,  2},
	{  20,  0},
	{  20,  3},
	{  23,  3},
	{  26,  2},
	{  28,  0},
	{  28,  3},
	{  31,  3},
	{  34,  3},
	{  37,  3},
	{  40,  2},
	{  42,  0},
	{  42,  3},
	{  45,  3},
	{  48,  3},
	{  51,  3},
	{  54,  1},
	{  55,  3},
	{  58,  3},
	{  61,  3},
	{  64,  2},
	{  66,  3},
};
/* 56 bytes */

/* Total: 257 bytes */


int ll_rule_push(uint8_t * sp, unsigned int sym, unsigned int tok)
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

int ll_start(uint8_t * sp)
{
	sp[-1] = T_EOF;
	sp[-2] = N_PROGRAM;

	return 2;
}

const const char * const ll_sym_tab[] = {
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
 	"PRINT",
 	"program",
 	"stat",
 	"exp",
 	"additive_exp",
 	"shift_exp1",
 	"mult_exp",
 	"additive_exp1",
 	"unary_exp",
 	"mult_exp1",
 	"primary_exp",
 	"op_var_decl",
 	"op_lookup_id",
 	"op_assign",
 	"op_print",
 	"op_shl",
 	"op_asr",
 	"op_add",
 	"op_sub",
 	"op_xor",
 	"op_or",
 	"op_mul",
 	"op_div",
 	"op_mod",
 	"op_and",
 	"op_inv",
 	"op_minus",
 	"op_push_int",
 	"op_push_id",
 };

