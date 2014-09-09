/* LL(1) Embeddedd Nonrecursive Predictive Parser */

#include "calc_ll.h"

/* Token-Rule pair (predict set) */
struct tr_pair {
	uint8_t t;
	uint8_t r;
};

/* Predict sets */
const struct tr_pair predict_vec[] = {
	/*  38 (program) */
	{  0,  0}, { 22,  1}, { 27,  1}, { 28,  1}, { 29,  1}, { 31,  1}, 
	{ 34,  1}, 
	/*  39 (stat) */
	{ 22,  5}, { 27,  6}, { 28,  8}, { 29,  9}, { 31,  7}, { 34, 10}, 
	/*  40 (compound_stat) */
	{ 25,  2}, 
	/*  41 (stat_list) */
	{ 22,  4}, { 26,  3}, { 27,  4}, { 28,  4}, { 29,  4}, { 31,  4}, 
	{ 34,  4}, 
	/*  42 (exp) */
	{ 12, 14}, { 19, 14}, { 23, 14}, { 30, 14}, { 31, 14}, { 32, 14}, 
	{ 33, 14}, { 37, 14}, 
	/*  43 (condition) */
	{ 23, 13}, 
	/*  44 (else_opt) */
	{  0, 11}, { 22, 11}, { 26, 11}, { 27, 11}, { 28, 11}, { 29, 11}, 
	{ 31, 11}, { 34, 11}, { 35, 12}, 
	/*  45 (shift_exp) */
	{ 12, 22}, { 19, 22}, { 23, 22}, { 30, 22}, { 31, 22}, { 32, 22}, 
	{ 33, 22}, { 37, 22}, 
	/*  46 (relational_exp) */
	{  1, 16}, {  2, 17}, {  3, 18}, {  4, 19}, {  5, 20}, {  6, 21}, 
	{ 22, 15}, { 24, 15}, 
	/*  47 (additive_exp) */
	{ 12, 26}, { 19, 26}, { 23, 26}, { 30, 26}, { 31, 26}, { 32, 26}, 
	{ 33, 26}, { 37, 26}, 
	/*  48 (shift_exp1) */
	{  1, 23}, {  2, 23}, {  3, 23}, {  4, 23}, {  5, 23}, {  6, 23}, 
	{  9, 25}, { 10, 24}, { 22, 23}, { 24, 23}, 
	/*  49 (mult_exp) */
	{ 12, 33}, { 19, 33}, { 23, 33}, { 30, 33}, { 31, 33}, { 32, 33}, 
	{ 33, 33}, { 37, 33}, 
	/*  50 (additive_exp1) */
	{  1, 27}, {  2, 27}, {  3, 27}, {  4, 27}, {  5, 27}, {  6, 27}, 
	{  7, 32}, {  9, 27}, { 10, 27}, { 11, 28}, { 12, 29}, { 16, 31}, 
	{ 18, 30}, { 22, 27}, { 24, 27}, 
	/*  51 (unary_exp) */
	{ 12, 42}, { 19, 41}, { 23, 40}, { 30, 40}, { 31, 40}, { 32, 40}, 
	{ 33, 40}, { 37, 43}, 
	/*  52 (mult_exp1) */
	{  1, 34}, {  2, 34}, {  3, 34}, {  4, 34}, {  5, 34}, {  6, 34}, 
	{  7, 34}, {  8, 39}, {  9, 34}, { 10, 34}, { 11, 34}, { 12, 34}, 
	{ 13, 35}, { 14, 36}, { 15, 37}, { 16, 34}, { 17, 38}, { 18, 34}, 
	{ 22, 34}, { 24, 34}, 
	/*  53 (primary_exp) */
	{ 23, 44}, { 30, 45}, { 31, 46}, { 32, 47}, { 33, 48}, 
};
/* 258 bytes; */

static const struct {
	uint8_t off;
	uint8_t cnt;
} predict_idx[] = {
	{   0,  7},
	{   7,  6},
	{  13,  1},
	{  14,  7},
	{  21,  8},
	{  29,  1},
	{  30,  9},
	{  39,  8},
	{  47,  8},
	{  55,  8},
	{  63, 10},
	{  73,  8},
	{  81, 15},
	{  96,  8},
	{ 104, 20},
	{ 124,  5},
};
/* 32 bytes */

/* Rules vectors table */
const uint8_t rule_vec[] = {
	/*  0:program(1) -> */
	T_EOF, 
	/*  1:program(2) -> */
	N_STAT, N_PROGRAM, 
	/*  2:compound_stat(3) -> */
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
	T_WHILE, A_OP_WHILE_BEGIN, N_CONDITION, A_OP_WHILE_COND, N_COMPOUND_STAT, A_OP_WHILE_END, 
	/* 10:stat(6) -> */
	T_IF, N_CONDITION, A_OP_IF_COND, N_COMPOUND_STAT, N_ELSE_OPT, A_OP_IF_END, 
	/* 11:else_opt(0) -> */
	
	/* 12:else_opt(3) -> */
	T_ELSE, A_OP_IF_ELSE, N_COMPOUND_STAT, 
	/* 13:condition(3) -> */
	T_LPAREN, N_EXP, T_RPAREN, 
	/* 14:exp(2) -> */
	N_SHIFT_EXP, N_RELATIONAL_EXP, 
	/* 15:relational_exp(0) -> */
	
	/* 16:relational_exp(3) -> */
	T_EQU, N_EXP, A_OP_EQU, 
	/* 17:relational_exp(3) -> */
	T_NEQ, N_EXP, A_OP_NEQ, 
	/* 18:relational_exp(3) -> */
	T_LT, N_EXP, A_OP_LT, 
	/* 19:relational_exp(3) -> */
	T_GT, N_EXP, A_OP_GT, 
	/* 20:relational_exp(3) -> */
	T_GTE, N_EXP, A_OP_GTE, 
	/* 21:relational_exp(3) -> */
	T_LTE, N_EXP, A_OP_LTE, 
	/* 22:shift_exp(2) -> */
	N_ADDITIVE_EXP, N_SHIFT_EXP1, 
	/* 23:shift_exp1(0) -> */
	
	/* 24:shift_exp1(3) -> */
	T_SHL, N_SHIFT_EXP, A_OP_SHL, 
	/* 25:shift_exp1(3) -> */
	T_ASR, N_SHIFT_EXP, A_OP_ASR, 
	/* 26:additive_exp(2) -> */
	N_MULT_EXP, N_ADDITIVE_EXP1, 
	/* 27:additive_exp1(0) -> */
	
	/* 28:additive_exp1(3) -> */
	T_PLUS, N_ADDITIVE_EXP, A_OP_ADD, 
	/* 29:additive_exp1(3) -> */
	T_MINUS, N_ADDITIVE_EXP, A_OP_SUB, 
	/* 30:additive_exp1(3) -> */
	T_XOR, N_ADDITIVE_EXP, A_OP_XOR, 
	/* 31:additive_exp1(3) -> */
	T_OR, N_ADDITIVE_EXP, A_OP_OR, 
	/* 32:additive_exp1(3) -> */
	T_LOR, N_ADDITIVE_EXP, A_OP_LOGIC_OR, 
	/* 33:mult_exp(2) -> */
	N_UNARY_EXP, N_MULT_EXP1, 
	/* 34:mult_exp1(0) -> */
	
	/* 35:mult_exp1(3) -> */
	T_MUL, N_MULT_EXP, A_OP_MUL, 
	/* 36:mult_exp1(3) -> */
	T_DIV, N_MULT_EXP, A_OP_DIV, 
	/* 37:mult_exp1(3) -> */
	T_MOD, N_MULT_EXP, A_OP_MOD, 
	/* 38:mult_exp1(3) -> */
	T_AND, N_MULT_EXP, A_OP_AND, 
	/* 39:mult_exp1(3) -> */
	T_LAND, N_MULT_EXP, A_OP_LOGIC_AND, 
	/* 40:unary_exp(1) -> */
	N_PRIMARY_EXP, 
	/* 41:unary_exp(3) -> */
	T_INV, N_UNARY_EXP, A_OP_INV, 
	/* 42:unary_exp(3) -> */
	T_MINUS, N_UNARY_EXP, A_OP_MINUS, 
	/* 43:unary_exp(3) -> */
	T_NOT, N_UNARY_EXP, A_OP_NOT, 
	/* 44:primary_exp(3) -> */
	T_LPAREN, N_EXP, T_RPAREN, 
	/* 45:primary_exp(2) -> */
	T_INT, A_OP_PUSH_INT, 
	/* 46:primary_exp(3) -> */
	T_ID, A_OP_LOOKUP_ID, A_OP_PUSH_ID, 
	/* 47:primary_exp(2) -> */
	T_TRUE, A_OP_PUSH_TRUE, 
	/* 48:primary_exp(2) -> */
	T_FALSE, A_OP_PUSH_FALSE, 
};
/* 125 bytes */

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
	{  29,  6},
	{  35,  0},
	{  35,  3},
	{  38,  3},
	{  41,  2},
	{  43,  0},
	{  43,  3},
	{  46,  3},
	{  49,  3},
	{  52,  3},
	{  55,  3},
	{  58,  3},
	{  61,  2},
	{  63,  0},
	{  63,  3},
	{  66,  3},
	{  69,  2},
	{  71,  0},
	{  71,  3},
	{  74,  3},
	{  77,  3},
	{  80,  3},
	{  83,  3},
	{  86,  2},
	{  88,  0},
	{  88,  3},
	{  91,  3},
	{  94,  3},
	{  97,  3},
	{ 100,  3},
	{ 103,  1},
	{ 104,  3},
	{ 107,  3},
	{ 110,  3},
	{ 113,  3},
	{ 116,  2},
	{ 118,  3},
	{ 121,  2},
	{ 123,  2},
};
/* 98 bytes */

/* Total: 513 bytes */


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
 	"IF",
 	"ELSE",
 	"ERR",
 	"NOT",
 	"program",
 	"stat",
 	"compound_stat",
 	"stat_list",
 	"exp",
 	"condition",
 	"else_opt",
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
 	"op_if_cond",
 	"op_if_end",
 	"op_if_else",
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

