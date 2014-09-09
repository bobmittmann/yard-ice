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
	{ 12, 13}, { 19, 13}, { 23, 13}, { 30, 13}, { 31, 13}, { 32, 13}, 
	{ 33, 13}, { 37, 13}, 
	/*  43 (condition) */
	{ 23, 12}, 
	/*  44 (else_opt) */
	{ 35, 11}, 
	/*  45 (shift_exp) */
	{ 12, 21}, { 19, 21}, { 23, 21}, { 30, 21}, { 31, 21}, { 32, 21}, 
	{ 33, 21}, { 37, 21}, 
	/*  46 (relational_exp) */
	{  1, 15}, {  2, 16}, {  3, 17}, {  4, 18}, {  5, 19}, {  6, 20}, 
	{ 22, 14}, { 24, 14}, 
	/*  47 (additive_exp) */
	{ 12, 25}, { 19, 25}, { 23, 25}, { 30, 25}, { 31, 25}, { 32, 25}, 
	{ 33, 25}, { 37, 25}, 
	/*  48 (shift_exp1) */
	{  1, 22}, {  2, 22}, {  3, 22}, {  4, 22}, {  5, 22}, {  6, 22}, 
	{  9, 24}, { 10, 23}, { 22, 22}, { 24, 22}, 
	/*  49 (mult_exp) */
	{ 12, 32}, { 19, 32}, { 23, 32}, { 30, 32}, { 31, 32}, { 32, 32}, 
	{ 33, 32}, { 37, 32}, 
	/*  50 (additive_exp1) */
	{  1, 26}, {  2, 26}, {  3, 26}, {  4, 26}, {  5, 26}, {  6, 26}, 
	{  7, 31}, {  9, 26}, { 10, 26}, { 11, 27}, { 12, 28}, { 16, 30}, 
	{ 18, 29}, { 22, 26}, { 24, 26}, 
	/*  51 (unary_exp) */
	{ 12, 41}, { 19, 40}, { 23, 39}, { 30, 39}, { 31, 39}, { 32, 39}, 
	{ 33, 39}, { 37, 42}, 
	/*  52 (mult_exp1) */
	{  1, 33}, {  2, 33}, {  3, 33}, {  4, 33}, {  5, 33}, {  6, 33}, 
	{  7, 33}, {  8, 38}, {  9, 33}, { 10, 33}, { 11, 33}, { 12, 33}, 
	{ 13, 34}, { 14, 35}, { 15, 36}, { 16, 33}, { 17, 37}, { 18, 33}, 
	{ 22, 33}, { 24, 33}, 
	/*  53 (primary_exp) */
	{ 23, 43}, { 30, 44}, { 31, 45}, { 32, 46}, { 33, 47}, 
};
/* 242 bytes; */

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
	{  30,  1},
	{  31,  8},
	{  39,  8},
	{  47,  8},
	{  55, 10},
	{  65,  8},
	{  73, 15},
	{  88,  8},
	{  96, 20},
	{ 116,  5},
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
	/* 10:stat(7) -> */
	T_IF, A_OP_IF_BEGIN, N_CONDITION, A_OP_IF_COND, N_COMPOUND_STAT, N_ELSE_OPT, A_OP_IF_END, 
	/* 11:else_opt(3) -> */
	T_ELSE, A_OP_IF_ELSE, N_COMPOUND_STAT, 
	/* 12:condition(3) -> */
	T_LPAREN, N_EXP, T_RPAREN, 
	/* 13:exp(2) -> */
	N_SHIFT_EXP, N_RELATIONAL_EXP, 
	/* 14:relational_exp(0) -> */
	
	/* 15:relational_exp(3) -> */
	T_EQU, N_EXP, A_OP_EQU, 
	/* 16:relational_exp(3) -> */
	T_NEQ, N_EXP, A_OP_NEQ, 
	/* 17:relational_exp(3) -> */
	T_LT, N_EXP, A_OP_LT, 
	/* 18:relational_exp(3) -> */
	T_GT, N_EXP, A_OP_GT, 
	/* 19:relational_exp(3) -> */
	T_GTE, N_EXP, A_OP_GTE, 
	/* 20:relational_exp(3) -> */
	T_LTE, N_EXP, A_OP_LTE, 
	/* 21:shift_exp(2) -> */
	N_ADDITIVE_EXP, N_SHIFT_EXP1, 
	/* 22:shift_exp1(0) -> */
	
	/* 23:shift_exp1(3) -> */
	T_SHL, N_SHIFT_EXP, A_OP_SHL, 
	/* 24:shift_exp1(3) -> */
	T_ASR, N_SHIFT_EXP, A_OP_ASR, 
	/* 25:additive_exp(2) -> */
	N_MULT_EXP, N_ADDITIVE_EXP1, 
	/* 26:additive_exp1(0) -> */
	
	/* 27:additive_exp1(3) -> */
	T_PLUS, N_ADDITIVE_EXP, A_OP_ADD, 
	/* 28:additive_exp1(3) -> */
	T_MINUS, N_ADDITIVE_EXP, A_OP_SUB, 
	/* 29:additive_exp1(3) -> */
	T_XOR, N_ADDITIVE_EXP, A_OP_XOR, 
	/* 30:additive_exp1(3) -> */
	T_OR, N_ADDITIVE_EXP, A_OP_OR, 
	/* 31:additive_exp1(3) -> */
	T_LOR, N_ADDITIVE_EXP, A_OP_LOGIC_OR, 
	/* 32:mult_exp(2) -> */
	N_UNARY_EXP, N_MULT_EXP1, 
	/* 33:mult_exp1(0) -> */
	
	/* 34:mult_exp1(3) -> */
	T_MUL, N_MULT_EXP, A_OP_MUL, 
	/* 35:mult_exp1(3) -> */
	T_DIV, N_MULT_EXP, A_OP_DIV, 
	/* 36:mult_exp1(3) -> */
	T_MOD, N_MULT_EXP, A_OP_MOD, 
	/* 37:mult_exp1(3) -> */
	T_AND, N_MULT_EXP, A_OP_AND, 
	/* 38:mult_exp1(3) -> */
	T_LAND, N_MULT_EXP, A_OP_LOGIC_AND, 
	/* 39:unary_exp(1) -> */
	N_PRIMARY_EXP, 
	/* 40:unary_exp(3) -> */
	T_INV, N_UNARY_EXP, A_OP_INV, 
	/* 41:unary_exp(3) -> */
	T_MINUS, N_UNARY_EXP, A_OP_MINUS, 
	/* 42:unary_exp(3) -> */
	T_NOT, N_UNARY_EXP, A_OP_NOT, 
	/* 43:primary_exp(3) -> */
	T_LPAREN, N_EXP, T_RPAREN, 
	/* 44:primary_exp(2) -> */
	T_INT, A_OP_PUSH_INT, 
	/* 45:primary_exp(3) -> */
	T_ID, A_OP_LOOKUP_ID, A_OP_PUSH_ID, 
	/* 46:primary_exp(2) -> */
	T_TRUE, A_OP_PUSH_TRUE, 
	/* 47:primary_exp(2) -> */
	T_FALSE, A_OP_PUSH_FALSE, 
};
/* 126 bytes */

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
	{  29,  7},
	{  36,  3},
	{  39,  3},
	{  42,  2},
	{  44,  0},
	{  44,  3},
	{  47,  3},
	{  50,  3},
	{  53,  3},
	{  56,  3},
	{  59,  3},
	{  62,  2},
	{  64,  0},
	{  64,  3},
	{  67,  3},
	{  70,  2},
	{  72,  0},
	{  72,  3},
	{  75,  3},
	{  78,  3},
	{  81,  3},
	{  84,  3},
	{  87,  2},
	{  89,  0},
	{  89,  3},
	{  92,  3},
	{  95,  3},
	{  98,  3},
	{ 101,  3},
	{ 104,  1},
	{ 105,  3},
	{ 108,  3},
	{ 111,  3},
	{ 114,  3},
	{ 117,  2},
	{ 119,  3},
	{ 122,  2},
	{ 124,  2},
};
/* 96 bytes */

/* Total: 496 bytes */


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
 	"op_if_begin",
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

