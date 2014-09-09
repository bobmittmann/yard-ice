/* LL(1) Embeddedd Nonrecursive Predictive Parser */

#include "calc_ll.h"

/* Token-Rule pair (predict set) */
struct tr_pair {
	uint8_t t;
	uint8_t r;
};

/* Predict sets */
const struct tr_pair predict_vec[] = {
	/*  36 (program) */
	{  0,  0}, { 22,  1}, { 27,  1}, { 28,  1}, { 29,  1}, { 31,  1}, 
	/*  37 (stat) */
	{ 22,  5}, { 27,  6}, { 28,  8}, { 29,  9}, { 31,  7}, 
	/*  38 (compound) */
	{ 25,  2}, 
	/*  39 (stat_list) */
	{ 22,  4}, { 26,  3}, { 27,  4}, { 28,  4}, { 29,  4}, { 31,  4}, 
	/*  40 (exp) */
	{ 12, 11}, { 19, 11}, { 23, 11}, { 30, 11}, { 31, 11}, { 32, 11}, 
	{ 33, 11}, { 35, 11}, 
	/*  41 (condition) */
	{ 23, 10}, 
	/*  42 (shift_exp) */
	{ 12, 19}, { 19, 19}, { 23, 19}, { 30, 19}, { 31, 19}, { 32, 19}, 
	{ 33, 19}, { 35, 19}, 
	/*  43 (relational_exp) */
	{  1, 13}, {  2, 14}, {  3, 15}, {  4, 16}, {  5, 17}, {  6, 18}, 
	{ 22, 12}, { 24, 12}, 
	/*  44 (additive_exp) */
	{ 12, 23}, { 19, 23}, { 23, 23}, { 30, 23}, { 31, 23}, { 32, 23}, 
	{ 33, 23}, { 35, 23}, 
	/*  45 (shift_exp1) */
	{  1, 20}, {  2, 20}, {  3, 20}, {  4, 20}, {  5, 20}, {  6, 20}, 
	{  9, 22}, { 10, 21}, { 22, 20}, { 24, 20}, 
	/*  46 (mult_exp) */
	{ 12, 30}, { 19, 30}, { 23, 30}, { 30, 30}, { 31, 30}, { 32, 30}, 
	{ 33, 30}, { 35, 30}, 
	/*  47 (additive_exp1) */
	{  1, 24}, {  2, 24}, {  3, 24}, {  4, 24}, {  5, 24}, {  6, 24}, 
	{  7, 29}, {  9, 24}, { 10, 24}, { 11, 25}, { 12, 26}, { 16, 28}, 
	{ 18, 27}, { 22, 24}, { 24, 24}, 
	/*  48 (unary_exp) */
	{ 12, 39}, { 19, 38}, { 23, 37}, { 30, 37}, { 31, 37}, { 32, 37}, 
	{ 33, 37}, { 35, 40}, 
	/*  49 (mult_exp1) */
	{  1, 31}, {  2, 31}, {  3, 31}, {  4, 31}, {  5, 31}, {  6, 31}, 
	{  7, 31}, {  8, 36}, {  9, 31}, { 10, 31}, { 11, 31}, { 12, 31}, 
	{ 13, 32}, { 14, 33}, { 15, 34}, { 16, 31}, { 17, 35}, { 18, 31}, 
	{ 22, 31}, { 24, 31}, 
	/*  50 (primary_exp) */
	{ 23, 41}, { 30, 42}, { 31, 43}, { 32, 44}, { 33, 45}, 
};
/* 234 bytes; */

static const struct {
	uint8_t off;
	uint8_t cnt;
} predict_idx[] = {
	{   0,  6},
	{   6,  5},
	{  11,  1},
	{  12,  6},
	{  18,  8},
	{  26,  1},
	{  27,  8},
	{  35,  8},
	{  43,  8},
	{  51, 10},
	{  61,  8},
	{  69, 15},
	{  84,  8},
	{  92, 20},
	{ 112,  5},
};
/* 30 bytes */

/* Rules vectors table */
const uint8_t rule_vec[] = {
	/*  0:program(1) -> */
	T_EOF, 
	/*  1:program(2) -> */
	N_STAT, N_PROGRAM, 
	/*  2:compound(3) -> */
	T_LBRACE, N_STAT_LIST, T_RBRACE, 
	/*  3:stat_list(0) -> */
	
	/*  4:stat_list(2) -> */
	N_STAT, N_STAT_LIST, 
	/*  5:stat(1) -> */
	T_SEMICOLON, 
	/*  6:stat(4) -> */
	T_VAR, T_ID, A_OP_VAR_DECL, T_SEMICOLON, 
	/*  7:stat(6) -> */
	T_ID, A_OP_LOOKUP_ID, T_ASSIGN, N_EXP, T_SEMICOLON, A_OP_ASSIGN, 
	/*  8:stat(4) -> */
	T_PRINT, N_EXP, T_SEMICOLON, A_OP_PRINT, 
	/*  9:stat(6) -> */
	T_WHILE, A_OP_WHILE_BEGIN, N_CONDITION, A_OP_WHILE_COND, N_COMPOUND, A_OP_WHILE_END, 
	/* 10:condition(3) -> */
	T_LPAREN, N_EXP, T_RPAREN, 
	/* 11:exp(2) -> */
	N_SHIFT_EXP, N_RELATIONAL_EXP, 
	/* 12:relational_exp(0) -> */
	
	/* 13:relational_exp(3) -> */
	T_EQU, N_EXP, A_OP_EQU, 
	/* 14:relational_exp(3) -> */
	T_NEQ, N_EXP, A_OP_NEQ, 
	/* 15:relational_exp(3) -> */
	T_LT, N_EXP, A_OP_LT, 
	/* 16:relational_exp(3) -> */
	T_GT, N_EXP, A_OP_GT, 
	/* 17:relational_exp(3) -> */
	T_GTE, N_EXP, A_OP_GTE, 
	/* 18:relational_exp(3) -> */
	T_LTE, N_EXP, A_OP_LTE, 
	/* 19:shift_exp(2) -> */
	N_ADDITIVE_EXP, N_SHIFT_EXP1, 
	/* 20:shift_exp1(0) -> */
	
	/* 21:shift_exp1(3) -> */
	T_SHL, N_SHIFT_EXP, A_OP_SHL, 
	/* 22:shift_exp1(3) -> */
	T_ASR, N_SHIFT_EXP, A_OP_ASR, 
	/* 23:additive_exp(2) -> */
	N_MULT_EXP, N_ADDITIVE_EXP1, 
	/* 24:additive_exp1(0) -> */
	
	/* 25:additive_exp1(3) -> */
	T_PLUS, N_ADDITIVE_EXP, A_OP_ADD, 
	/* 26:additive_exp1(3) -> */
	T_MINUS, N_ADDITIVE_EXP, A_OP_SUB, 
	/* 27:additive_exp1(3) -> */
	T_XOR, N_ADDITIVE_EXP, A_OP_XOR, 
	/* 28:additive_exp1(3) -> */
	T_OR, N_ADDITIVE_EXP, A_OP_OR, 
	/* 29:additive_exp1(3) -> */
	T_LOR, N_ADDITIVE_EXP, A_OP_LOGIC_OR, 
	/* 30:mult_exp(2) -> */
	N_UNARY_EXP, N_MULT_EXP1, 
	/* 31:mult_exp1(0) -> */
	
	/* 32:mult_exp1(3) -> */
	T_MUL, N_MULT_EXP, A_OP_MUL, 
	/* 33:mult_exp1(3) -> */
	T_DIV, N_MULT_EXP, A_OP_DIV, 
	/* 34:mult_exp1(3) -> */
	T_MOD, N_MULT_EXP, A_OP_MOD, 
	/* 35:mult_exp1(3) -> */
	T_AND, N_MULT_EXP, A_OP_AND, 
	/* 36:mult_exp1(3) -> */
	T_LAND, N_MULT_EXP, A_OP_LOGIC_AND, 
	/* 37:unary_exp(1) -> */
	N_PRIMARY_EXP, 
	/* 38:unary_exp(3) -> */
	T_INV, N_UNARY_EXP, A_OP_INV, 
	/* 39:unary_exp(3) -> */
	T_MINUS, N_UNARY_EXP, A_OP_MINUS, 
	/* 40:unary_exp(3) -> */
	T_NOT, N_UNARY_EXP, A_OP_NOT, 
	/* 41:primary_exp(3) -> */
	T_LPAREN, N_EXP, T_RPAREN, 
	/* 42:primary_exp(2) -> */
	T_INT, A_OP_PUSH_INT, 
	/* 43:primary_exp(3) -> */
	T_ID, A_OP_LOOKUP_ID, A_OP_PUSH_ID, 
	/* 44:primary_exp(2) -> */
	T_TRUE, A_OP_PUSH_TRUE, 
	/* 45:primary_exp(2) -> */
	T_FALSE, A_OP_PUSH_FALSE, 
};
/* 116 bytes */

static const struct {
	uint8_t off;
	uint8_t cnt;
} rule_idx[] = {
	{   0,  1},
	{   1,  2},
	{   3,  3},
	{   6,  0},
	{   6,  2},
	{   8,  1},
	{   9,  4},
	{  13,  6},
	{  19,  4},
	{  23,  6},
	{  29,  3},
	{  32,  2},
	{  34,  0},
	{  34,  3},
	{  37,  3},
	{  40,  3},
	{  43,  3},
	{  46,  3},
	{  49,  3},
	{  52,  2},
	{  54,  0},
	{  54,  3},
	{  57,  3},
	{  60,  2},
	{  62,  0},
	{  62,  3},
	{  65,  3},
	{  68,  3},
	{  71,  3},
	{  74,  3},
	{  77,  2},
	{  79,  0},
	{  79,  3},
	{  82,  3},
	{  85,  3},
	{  88,  3},
	{  91,  3},
	{  94,  1},
	{  95,  3},
	{  98,  3},
	{ 101,  3},
	{ 104,  3},
	{ 107,  2},
	{ 109,  3},
	{ 112,  2},
	{ 114,  2},
};
/* 92 bytes */

/* Total: 472 bytes */


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
 	"EQU",
 	"NEQ",
 	"LT",
 	"GT",
 	"GTE",
 	"LTE",
 	"LOR",
 	"LAND",
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
 	"LBRACE",
 	"RBRACE",
 	"VAR",
 	"PRINT",
 	"WHILE",
 	"INT",
 	"ID",
 	"TRUE",
 	"FALSE",
 	"ERR",
 	"NOT",
 	"program",
 	"stat",
 	"compound",
 	"stat_list",
 	"exp",
 	"condition",
 	"shift_exp",
 	"relational_exp",
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
 	"op_while_begin",
 	"op_while_cond",
 	"op_while_end",
 	"op_equ",
 	"op_neq",
 	"op_lt",
 	"op_gt",
 	"op_gte",
 	"op_lte",
 	"op_shl",
 	"op_asr",
 	"op_add",
 	"op_sub",
 	"op_xor",
 	"op_or",
 	"op_logic_or",
 	"op_mul",
 	"op_div",
 	"op_mod",
 	"op_and",
 	"op_logic_and",
 	"op_inv",
 	"op_minus",
 	"op_not",
 	"op_push_int",
 	"op_push_id",
 	"op_push_true",
 	"op_push_false",
 };

