/* LL(1) Embeddedd Nonrecursive Predictive Parser */

#include "microjs_ll.h"

/* Token-Rule pair (predict set) */
struct tr_pair {
	uint8_t t;
	uint8_t r;
};

/* Predict sets */
static const struct tr_pair predict_vec[] = {
	/*  48 (program) */
	{  0,  0}, {  9,  2}, { 35,  1}, { 37,  1}, { 39,  1}, { 40,  1}, 
	{ 44,  1}, 
	/*  49 (stat) */
	{ 35,  6}, { 37,  8}, { 39,  9}, { 40,  7}, { 44, 10}, 
	/*  50 (compound_stat) */
	{  9,  3}, 
	/*  51 (stat_list) */
	{ 10,  4}, { 35,  5}, { 37,  5}, { 39,  5}, { 40,  5}, { 44,  5}, 
	/*  52 (exp_lst_opt) */
	{  3, 28}, {  8, 28}, { 44, 29}, 
	/*  53 (exp) */
	{  7, 33}, { 20, 33}, { 29, 33}, { 30, 33}, { 34, 33}, { 38, 33}, 
	{ 44, 33}, { 45, 33}, { 46, 33}, 
	/*  54 (condition) */
	{  7, 13}, 
	/*  55 (else_opt) */
	{  0, 11}, {  9, 11}, { 10, 11}, { 33, 12}, { 35, 11}, { 37, 11}, 
	{ 39, 11}, { 40, 11}, { 44, 11}, 
	/*  56 (var_list) */
	{ 44, 14}, 
	/*  57 (assign_or_call) */
	{ 44, 20}, 
	/*  58 (var) */
	{ 44, 17}, 
	/*  59 (var_list1) */
	{  2, 16}, {  3, 15}, 
	/*  60 (var_assign_opt) */
	{  2, 18}, {  3, 18}, { 32, 19}, 
	/*  61 (assign_or_call1) */
	{  7, 22}, { 32, 21}, 
	/*  62 (arg_list_opt) */
	{  7, 24}, {  8, 23}, { 20, 24}, { 29, 24}, { 30, 24}, { 34, 24}, 
	{ 38, 24}, { 44, 24}, { 45, 24}, { 46, 24}, 
	/*  63 (arg_list) */
	{  7, 25}, { 20, 25}, { 29, 25}, { 30, 25}, { 34, 25}, { 38, 25}, 
	{ 44, 25}, { 45, 25}, { 46, 25}, 
	/*  64 (arg_list1) */
	{  2, 27}, {  8, 26}, 
	/*  65 (exp_lst) */
	{ 44, 30}, 
	/*  66 (exp_lst1) */
	{  2, 32}, {  3, 31}, {  8, 31}, 
	/*  67 (and_exp) */
	{  7, 38}, { 20, 38}, { 29, 38}, { 30, 38}, { 34, 38}, { 38, 38}, 
	{ 44, 38}, { 45, 38}, { 46, 38}, 
	/*  68 (or_exp) */
	{  2, 34}, {  3, 34}, {  8, 34}, { 24, 35}, { 25, 36}, { 28, 37}, 
	/*  69 (relational_exp) */
	{  7, 42}, { 20, 42}, { 29, 42}, { 30, 42}, { 34, 42}, { 38, 42}, 
	{ 44, 42}, { 45, 42}, { 46, 42}, 
	/*  70 (and_exp1) */
	{  2, 39}, {  3, 39}, {  8, 39}, { 24, 39}, { 25, 39}, { 26, 40}, 
	{ 27, 41}, { 28, 39}, 
	/*  71 (shift_exp) */
	{  7, 50}, { 20, 50}, { 29, 50}, { 30, 50}, { 34, 50}, { 38, 50}, 
	{ 44, 50}, { 45, 50}, { 46, 50}, 
	/*  72 (relational_exp1) */
	{  2, 43}, {  3, 43}, {  8, 43}, { 13, 49}, { 14, 46}, { 15, 48}, 
	{ 16, 47}, { 17, 44}, { 18, 45}, { 24, 43}, { 25, 43}, { 26, 43}, 
	{ 27, 43}, { 28, 43}, 
	/*  73 (additive_exp) */
	{  7, 54}, { 20, 54}, { 29, 54}, { 30, 54}, { 34, 54}, { 38, 54}, 
	{ 44, 54}, { 45, 54}, { 46, 54}, 
	/*  74 (shift_exp1) */
	{  2, 51}, {  3, 51}, {  8, 51}, { 11, 53}, { 12, 52}, { 13, 51}, 
	{ 14, 51}, { 15, 51}, { 16, 51}, { 17, 51}, { 18, 51}, { 24, 51}, 
	{ 25, 51}, { 26, 51}, { 27, 51}, { 28, 51}, 
	/*  75 (mult_exp) */
	{  7, 58}, { 20, 58}, { 29, 58}, { 30, 58}, { 34, 58}, { 38, 58}, 
	{ 44, 58}, { 45, 58}, { 46, 58}, 
	/*  76 (additive_exp1) */
	{  2, 55}, {  3, 55}, {  8, 55}, { 11, 55}, { 12, 55}, { 13, 55}, 
	{ 14, 55}, { 15, 55}, { 16, 55}, { 17, 55}, { 18, 55}, { 19, 56}, 
	{ 20, 57}, { 24, 55}, { 25, 55}, { 26, 55}, { 27, 55}, { 28, 55}, 
	/*  77 (unary_exp) */
	{  7, 63}, { 20, 65}, { 29, 66}, { 30, 64}, { 34, 63}, { 38, 63}, 
	{ 44, 63}, { 45, 63}, { 46, 63}, 
	/*  78 (mult_exp1) */
	{  2, 59}, {  3, 59}, {  8, 59}, { 11, 59}, { 12, 59}, { 13, 59}, 
	{ 14, 59}, { 15, 59}, { 16, 59}, { 17, 59}, { 18, 59}, { 19, 59}, 
	{ 20, 59}, { 21, 60}, { 22, 61}, { 23, 62}, { 24, 59}, { 25, 59}, 
	{ 26, 59}, { 27, 59}, { 28, 59}, 
	/*  79 (primary_exp) */
	{  7, 67}, { 34, 70}, { 38, 69}, { 44, 72}, { 45, 68}, { 46, 71}, 
	/*  80 (meth_or_attr) */
	{ 44, 73}, 
	/*  81 (meth_or_attr1) */
	{  2, 74}, {  3, 74}, {  7, 75}, {  8, 74}, { 11, 74}, { 12, 74}, 
	{ 13, 74}, { 14, 74}, { 15, 74}, { 16, 74}, { 17, 74}, { 18, 74}, 
	{ 19, 74}, { 20, 74}, { 21, 74}, { 22, 74}, { 23, 74}, { 24, 74}, 
	{ 25, 74}, { 26, 74}, { 27, 74}, { 28, 74}, 
};
/* 484 bytes; */

static const struct {
	uint8_t off;
	uint8_t cnt;
} predict_idx[] = {
	{   0,  7},
	{   7,  5},
	{  12,  1},
	{  13,  6},
	{  19,  3},
	{  22,  9},
	{  31,  1},
	{  32,  9},
	{  41,  1},
	{  42,  1},
	{  43,  1},
	{  44,  2},
	{  46,  3},
	{  49,  2},
	{  51, 10},
	{  61,  9},
	{  70,  2},
	{  72,  1},
	{  73,  3},
	{  76,  9},
	{  85,  6},
	{  91,  9},
	{ 100,  8},
	{ 108,  9},
	{ 117, 14},
	{ 131,  9},
	{ 140, 16},
	{ 156,  9},
	{ 165, 18},
	{ 183,  9},
	{ 192, 21},
	{ 213,  6},
	{ 219,  1},
	{ 220, 22},
};
/* 68 bytes */

/* Rules vectors table */
static const uint8_t rule_vec[] = {
	/*  0:program(1) -> */
	T_EOF, 
	/*  1:program(2) -> */
	N_STAT, N_PROGRAM, 
	/*  2:program(2) -> */
	N_COMPOUND_STAT, N_PROGRAM, 
	/*  3:compound_stat(5) -> */
	T_LBRACE, A_OP_BLK_OPEN, N_STAT_LIST, T_RBRACE, A_OP_BLK_CLOSE, 
	/*  4:stat_list(0) -> */
	
	/*  5:stat_list(2) -> */
	N_STAT, N_STAT_LIST, 
	/*  6:stat(13) -> */
	T_FOR, T_LPAREN, N_EXP_LST_OPT, T_SEMICOLON, A_OP_FOR_INIT, N_EXP, A_OP_FOR_COND, T_SEMICOLON, N_EXP_LST_OPT, A_OP_FOR_AFTER, T_RPAREN, N_COMPOUND_STAT, A_OP_FOR_END, 
	/*  7:stat(6) -> */
	T_WHILE, A_OP_WHILE_BEGIN, N_CONDITION, A_OP_WHILE_COND, N_COMPOUND_STAT, A_OP_WHILE_END, 
	/*  8:stat(6) -> */
	T_IF, N_CONDITION, A_OP_IF_COND, N_COMPOUND_STAT, N_ELSE_OPT, A_OP_IF_END, 
	/*  9:stat(3) -> */
	T_VAR, N_VAR_LIST, T_SEMICOLON, 
	/* 10:stat(2) -> */
	N_ASSIGN_OR_CALL, T_SEMICOLON, 
	/* 11:else_opt(0) -> */
	
	/* 12:else_opt(3) -> */
	T_ELSE, A_OP_IF_ELSE, N_COMPOUND_STAT, 
	/* 13:condition(3) -> */
	T_LPAREN, N_EXP, T_RPAREN, 
	/* 14:var_list(2) -> */
	N_VAR, N_VAR_LIST1, 
	/* 15:var_list1(0) -> */
	
	/* 16:var_list1(2) -> */
	T_COMMA, N_VAR_LIST, 
	/* 17:var(5) -> */
	T_ID, A_OP_VAR_DECL, A_OP_PUSH_TMP, N_VAR_ASSIGN_OPT, A_OP_POP_TMP, 
	/* 18:var_assign_opt(0) -> */
	
	/* 19:var_assign_opt(3) -> */
	T_ASSIGN, N_EXP, A_OP_ASSIGN, 
	/* 20:assign_or_call(4) -> */
	T_ID, A_OP_PUSH_TMP, N_ASSIGN_OR_CALL1, A_OP_POP_TMP, 
	/* 21:assign_or_call1(3) -> */
	T_ASSIGN, N_EXP, A_OP_ASSIGN, 
	/* 22:assign_or_call1(6) -> */
	T_LPAREN, A_OP_METHOD, N_ARG_LIST_OPT, T_RPAREN, A_OP_METH_OR_ATTR, A_OP_RET_DISCARD, 
	/* 23:arg_list_opt(0) -> */
	
	/* 24:arg_list_opt(1) -> */
	N_ARG_LIST, 
	/* 25:arg_list(3) -> */
	N_EXP, A_OP_ARG, N_ARG_LIST1, 
	/* 26:arg_list1(0) -> */
	
	/* 27:arg_list1(2) -> */
	T_COMMA, N_ARG_LIST, 
	/* 28:exp_lst_opt(0) -> */
	
	/* 29:exp_lst_opt(1) -> */
	N_EXP_LST, 
	/* 30:exp_lst(2) -> */
	N_ASSIGN_OR_CALL, N_EXP_LST1, 
	/* 31:exp_lst1(0) -> */
	
	/* 32:exp_lst1(2) -> */
	T_COMMA, N_EXP_LST, 
	/* 33:exp(2) -> */
	N_AND_EXP, N_OR_EXP, 
	/* 34:or_exp(0) -> */
	
	/* 35:or_exp(3) -> */
	T_OR, N_EXP, A_OP_OR, 
	/* 36:or_exp(3) -> */
	T_LOR, N_EXP, A_OP_LOGIC_OR, 
	/* 37:or_exp(3) -> */
	T_XOR, N_EXP, A_OP_XOR, 
	/* 38:and_exp(2) -> */
	N_RELATIONAL_EXP, N_AND_EXP1, 
	/* 39:and_exp1(0) -> */
	
	/* 40:and_exp1(3) -> */
	T_AND, N_AND_EXP, A_OP_AND, 
	/* 41:and_exp1(3) -> */
	T_LAND, N_AND_EXP, A_OP_LOGIC_AND, 
	/* 42:relational_exp(2) -> */
	N_SHIFT_EXP, N_RELATIONAL_EXP1, 
	/* 43:relational_exp1(0) -> */
	
	/* 44:relational_exp1(3) -> */
	T_EQU, N_RELATIONAL_EXP, A_OP_EQU, 
	/* 45:relational_exp1(3) -> */
	T_NEQ, N_RELATIONAL_EXP, A_OP_NEQ, 
	/* 46:relational_exp1(3) -> */
	T_LT, N_RELATIONAL_EXP, A_OP_LT, 
	/* 47:relational_exp1(3) -> */
	T_GT, N_RELATIONAL_EXP, A_OP_GT, 
	/* 48:relational_exp1(3) -> */
	T_GTE, N_RELATIONAL_EXP, A_OP_GTE, 
	/* 49:relational_exp1(3) -> */
	T_LTE, N_RELATIONAL_EXP, A_OP_LTE, 
	/* 50:shift_exp(2) -> */
	N_ADDITIVE_EXP, N_SHIFT_EXP1, 
	/* 51:shift_exp1(0) -> */
	
	/* 52:shift_exp1(3) -> */
	T_SHL, N_SHIFT_EXP, A_OP_SHL, 
	/* 53:shift_exp1(3) -> */
	T_ASR, N_SHIFT_EXP, A_OP_ASR, 
	/* 54:additive_exp(2) -> */
	N_MULT_EXP, N_ADDITIVE_EXP1, 
	/* 55:additive_exp1(0) -> */
	
	/* 56:additive_exp1(3) -> */
	T_PLUS, N_ADDITIVE_EXP, A_OP_ADD, 
	/* 57:additive_exp1(3) -> */
	T_MINUS, N_ADDITIVE_EXP, A_OP_SUB, 
	/* 58:mult_exp(2) -> */
	N_UNARY_EXP, N_MULT_EXP1, 
	/* 59:mult_exp1(0) -> */
	
	/* 60:mult_exp1(3) -> */
	T_MUL, N_MULT_EXP, A_OP_MUL, 
	/* 61:mult_exp1(3) -> */
	T_DIV, N_MULT_EXP, A_OP_DIV, 
	/* 62:mult_exp1(3) -> */
	T_MOD, N_MULT_EXP, A_OP_MOD, 
	/* 63:unary_exp(1) -> */
	N_PRIMARY_EXP, 
	/* 64:unary_exp(3) -> */
	T_INV, N_UNARY_EXP, A_OP_INV, 
	/* 65:unary_exp(3) -> */
	T_MINUS, N_UNARY_EXP, A_OP_MINUS, 
	/* 66:unary_exp(3) -> */
	T_NOT, N_UNARY_EXP, A_OP_NOT, 
	/* 67:primary_exp(3) -> */
	T_LPAREN, N_EXP, T_RPAREN, 
	/* 68:primary_exp(2) -> */
	T_INT, A_OP_PUSH_INT, 
	/* 69:primary_exp(2) -> */
	T_TRUE, A_OP_PUSH_TRUE, 
	/* 70:primary_exp(2) -> */
	T_FALSE, A_OP_PUSH_FALSE, 
	/* 71:primary_exp(2) -> */
	T_STRING, A_OP_PUSH_STRING, 
	/* 72:primary_exp(3) -> */
	N_METH_OR_ATTR, A_OP_METH_OR_ATTR, A_OP_POP_TMP, 
	/* 73:meth_or_attr(3) -> */
	T_ID, A_OP_PUSH_TMP, N_METH_OR_ATTR1, 
	/* 74:meth_or_attr1(0) -> */
	
	/* 75:meth_or_attr1(4) -> */
	T_LPAREN, A_OP_METHOD, N_ARG_LIST_OPT, T_RPAREN, 
};
/* 181 bytes */

static const struct {
	uint8_t off;
	uint8_t cnt;
} rule_idx[] = {
	{   0,  1},
	{   1,  2},
	{   3,  2},
	{   5,  5},
	{  10,  0},
	{  10,  2},
	{  12, 13},
	{  25,  6},
	{  31,  6},
	{  37,  3},
	{  40,  2},
	{  42,  0},
	{  42,  3},
	{  45,  3},
	{  48,  2},
	{  50,  0},
	{  50,  2},
	{  52,  5},
	{  57,  0},
	{  57,  3},
	{  60,  4},
	{  64,  3},
	{  67,  6},
	{  73,  0},
	{  73,  1},
	{  74,  3},
	{  77,  0},
	{  77,  2},
	{  79,  0},
	{  79,  1},
	{  80,  2},
	{  82,  0},
	{  82,  2},
	{  84,  2},
	{  86,  0},
	{  86,  3},
	{  89,  3},
	{  92,  3},
	{  95,  2},
	{  97,  0},
	{  97,  3},
	{ 100,  3},
	{ 103,  2},
	{ 105,  0},
	{ 105,  3},
	{ 108,  3},
	{ 111,  3},
	{ 114,  3},
	{ 117,  3},
	{ 120,  3},
	{ 123,  2},
	{ 125,  0},
	{ 125,  3},
	{ 128,  3},
	{ 131,  2},
	{ 133,  0},
	{ 133,  3},
	{ 136,  3},
	{ 139,  2},
	{ 141,  0},
	{ 141,  3},
	{ 144,  3},
	{ 147,  3},
	{ 150,  1},
	{ 151,  3},
	{ 154,  3},
	{ 157,  3},
	{ 160,  3},
	{ 163,  2},
	{ 165,  2},
	{ 167,  2},
	{ 169,  2},
	{ 171,  3},
	{ 174,  3},
	{ 177,  0},
	{ 177,  4},
};
/* 152 bytes */

/* Total: 885 bytes */


int microjs_ll_push(uint8_t * sp, unsigned int sym, unsigned int tok)
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

int microjs_ll_start(uint8_t * sp)
{
	sp[-1] = T_EOF;
	sp[-2] = N_PROGRAM;

	return 2;
}

const const char * const microjs_ll_sym[] = {
	"EOF",
 	"DOT",
 	"COMMA",
 	"SEMICOLON",
 	"COLON",
 	"LBRACKET",
 	"RBRACKET",
 	"LPAREN",
 	"RPAREN",
 	"LBRACE",
 	"RBRACE",
 	"ASR",
 	"SHL",
 	"LTE",
 	"LT",
 	"GTE",
 	"GT",
 	"EQU",
 	"NEQ",
 	"PLUS",
 	"MINUS",
 	"MUL",
 	"DIV",
 	"MOD",
 	"OR",
 	"LOR",
 	"AND",
 	"LAND",
 	"XOR",
 	"NOT",
 	"INV",
 	"QUEST",
 	"ASSIGN",
 	"ELSE",
 	"FALSE",
 	"FOR",
 	"FUNCTION",
 	"IF",
 	"TRUE",
 	"VAR",
 	"WHILE",
 	"CONTINUE",
 	"BREAK",
 	"RETURN",
 	"ID",
 	"INT",
 	"STRING",
 	"ERR",
 	"program",
 	"stat",
 	"compound_stat",
 	"stat_list",
 	"exp_lst_opt",
 	"exp",
 	"condition",
 	"else_opt",
 	"var_list",
 	"assign_or_call",
 	"var",
 	"var_list1",
 	"var_assign_opt",
 	"assign_or_call1",
 	"arg_list_opt",
 	"arg_list",
 	"arg_list1",
 	"exp_lst",
 	"exp_lst1",
 	"and_exp",
 	"or_exp",
 	"relational_exp",
 	"and_exp1",
 	"shift_exp",
 	"relational_exp1",
 	"additive_exp",
 	"shift_exp1",
 	"mult_exp",
 	"additive_exp1",
 	"unary_exp",
 	"mult_exp1",
 	"primary_exp",
 	"meth_or_attr",
 	"meth_or_attr1",
 	"op_blk_open",
 	"op_blk_close",
 	"op_for_init",
 	"op_for_cond",
 	"op_for_after",
 	"op_for_end",
 	"op_while_begin",
 	"op_while_cond",
 	"op_while_end",
 	"op_if_cond",
 	"op_if_end",
 	"op_if_else",
 	"op_var_decl",
 	"op_push_tmp",
 	"op_pop_tmp",
 	"op_assign",
 	"op_method",
 	"op_meth_or_attr",
 	"op_ret_discard",
 	"op_arg",
 	"op_or",
 	"op_logic_or",
 	"op_xor",
 	"op_and",
 	"op_logic_and",
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
 	"op_mul",
 	"op_div",
 	"op_mod",
 	"op_inv",
 	"op_minus",
 	"op_not",
 	"op_push_int",
 	"op_push_true",
 	"op_push_false",
 	"op_push_string",
 };

