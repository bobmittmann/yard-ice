/* LL(1) Embeddedd Nonrecursive Predictive Parser */

#include "calc_ll.h"

/* Token-Rule pair (predict set) */
struct tr_pair {
	uint8_t t;
	uint8_t r;
};

/* Predict sets */
const struct tr_pair predict_vec[] = {
	/*  44 (program) */
	{  0,  0}, {  3,  1}, { 35,  1}, { 36,  1}, { 39,  1}, { 40,  1}, 
	{ 41,  1}, 
	/*  45 (stat) */
	{  3,  5}, { 35,  6}, { 36,  8}, { 39,  9}, { 40,  7}, { 41, 10}, 
	/*  46 (compound_stat) */
	{  9,  2}, 
	/*  47 (stat_list) */
	{  3,  4}, { 10,  3}, { 35,  4}, { 36,  4}, { 39,  4}, { 40,  4}, 
	{ 41,  4}, 
	/*  48 (exp_lst_opt) */
	{  3, 28}, {  8, 28}, { 41, 29}, 
	/*  49 (exp) */
	{  7, 33}, { 20, 33}, { 29, 33}, { 30, 33}, { 34, 33}, { 38, 33}, 
	{ 41, 33}, { 42, 33}, 
	/*  50 (condition) */
	{  7, 13}, 
	/*  51 (else_opt) */
	{  0, 11}, {  3, 11}, { 10, 11}, { 33, 12}, { 35, 11}, { 36, 11}, 
	{ 39, 11}, { 40, 11}, { 41, 11}, 
	/*  52 (var_list) */
	{ 41, 14}, 
	/*  53 (assign_or_call) */
	{ 41, 20}, 
	/*  54 (var) */
	{ 41, 17}, 
	/*  55 (var_list1) */
	{  2, 16}, {  3, 15}, 
	/*  56 (var_assign_opt) */
	{  2, 18}, {  3, 18}, { 32, 19}, 
	/*  57 (assign_or_call1) */
	{  7, 22}, { 32, 21}, 
	/*  58 (arg_list_opt) */
	{  7, 24}, {  8, 23}, { 20, 24}, { 29, 24}, { 30, 24}, { 34, 24}, 
	{ 38, 24}, { 41, 24}, { 42, 24}, 
	/*  59 (arg_list) */
	{  7, 25}, { 20, 25}, { 29, 25}, { 30, 25}, { 34, 25}, { 38, 25}, 
	{ 41, 25}, { 42, 25}, 
	/*  60 (arg_list1) */
	{  2, 27}, {  8, 26}, 
	/*  61 (exp_lst) */
	{ 41, 30}, 
	/*  62 (exp_lst1) */
	{  2, 32}, {  3, 31}, {  8, 31}, 
	/*  63 (shift_exp) */
	{  7, 41}, { 20, 41}, { 29, 41}, { 30, 41}, { 34, 41}, { 38, 41}, 
	{ 41, 41}, { 42, 41}, 
	/*  64 (relational_exp) */
	{  2, 34}, {  3, 34}, {  8, 34}, { 13, 40}, { 14, 37}, { 15, 39}, 
	{ 16, 38}, { 17, 35}, { 18, 36}, 
	/*  65 (additive_exp) */
	{  7, 45}, { 20, 45}, { 29, 45}, { 30, 45}, { 34, 45}, { 38, 45}, 
	{ 41, 45}, { 42, 45}, 
	/*  66 (shift_exp1) */
	{  2, 42}, {  3, 42}, {  8, 42}, { 11, 44}, { 12, 43}, { 13, 42}, 
	{ 14, 42}, { 15, 42}, { 16, 42}, { 17, 42}, { 18, 42}, 
	/*  67 (mult_exp) */
	{  7, 52}, { 20, 52}, { 29, 52}, { 30, 52}, { 34, 52}, { 38, 52}, 
	{ 41, 52}, { 42, 52}, 
	/*  68 (additive_exp1) */
	{  2, 46}, {  3, 46}, {  8, 46}, { 11, 46}, { 12, 46}, { 13, 46}, 
	{ 14, 46}, { 15, 46}, { 16, 46}, { 17, 46}, { 18, 46}, { 19, 47}, 
	{ 20, 48}, { 24, 50}, { 25, 51}, { 28, 49}, 
	/*  69 (unary_exp) */
	{  7, 59}, { 20, 61}, { 29, 62}, { 30, 60}, { 34, 59}, { 38, 59}, 
	{ 41, 59}, { 42, 59}, 
	/*  70 (mult_exp1) */
	{  2, 53}, {  3, 53}, {  8, 53}, { 11, 53}, { 12, 53}, { 13, 53}, 
	{ 14, 53}, { 15, 53}, { 16, 53}, { 17, 53}, { 18, 53}, { 19, 53}, 
	{ 20, 53}, { 21, 54}, { 22, 55}, { 23, 56}, { 24, 53}, { 25, 53}, 
	{ 26, 57}, { 27, 58}, { 28, 53}, 
	/*  71 (primary_exp) */
	{  7, 63}, { 34, 66}, { 38, 65}, { 41, 67}, { 42, 64}, 
	/*  72 (meth_or_attr) */
	{ 41, 68}, 
	/*  73 (meth_or_attr1) */
	{  2, 69}, {  3, 69}, {  7, 70}, {  8, 69}, { 11, 69}, { 12, 69}, 
	{ 13, 69}, { 14, 69}, { 15, 69}, { 16, 69}, { 17, 69}, { 18, 69}, 
	{ 19, 69}, { 20, 69}, { 21, 69}, { 22, 69}, { 23, 69}, { 24, 69}, 
	{ 25, 69}, { 26, 69}, { 27, 69}, { 28, 69}, 
};
/* 384 bytes; */

static const struct {
	uint8_t off;
	uint8_t cnt;
} predict_idx[] = {
	{   0,  7},
	{   7,  6},
	{  13,  1},
	{  14,  7},
	{  21,  3},
	{  24,  8},
	{  32,  1},
	{  33,  9},
	{  42,  1},
	{  43,  1},
	{  44,  1},
	{  45,  2},
	{  47,  3},
	{  50,  2},
	{  52,  9},
	{  61,  8},
	{  69,  2},
	{  71,  1},
	{  72,  3},
	{  75,  8},
	{  83,  9},
	{  92,  8},
	{ 100, 11},
	{ 111,  8},
	{ 119, 16},
	{ 135,  8},
	{ 143, 21},
	{ 164,  5},
	{ 169,  1},
	{ 170, 22},
};
/* 60 bytes */

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
	T_ID, A_OP_VAR_DECL, A_OP_PUSH_SYM, N_VAR_ASSIGN_OPT, A_OP_POP_SYM, 
	/* 18:var_assign_opt(0) -> */
	
	/* 19:var_assign_opt(3) -> */
	T_ASSIGN, N_EXP, A_OP_ASSIGN, 
	/* 20:assign_or_call(4) -> */
	T_ID, A_OP_PUSH_SYM, N_ASSIGN_OR_CALL1, A_OP_POP_SYM, 
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
	N_SHIFT_EXP, N_RELATIONAL_EXP, 
	/* 34:relational_exp(0) -> */
	
	/* 35:relational_exp(3) -> */
	T_EQU, N_EXP, A_OP_EQU, 
	/* 36:relational_exp(3) -> */
	T_NEQ, N_EXP, A_OP_NEQ, 
	/* 37:relational_exp(3) -> */
	T_LT, N_EXP, A_OP_LT, 
	/* 38:relational_exp(3) -> */
	T_GT, N_EXP, A_OP_GT, 
	/* 39:relational_exp(3) -> */
	T_GTE, N_EXP, A_OP_GTE, 
	/* 40:relational_exp(3) -> */
	T_LTE, N_EXP, A_OP_LTE, 
	/* 41:shift_exp(2) -> */
	N_ADDITIVE_EXP, N_SHIFT_EXP1, 
	/* 42:shift_exp1(0) -> */
	
	/* 43:shift_exp1(3) -> */
	T_SHL, N_SHIFT_EXP, A_OP_SHL, 
	/* 44:shift_exp1(3) -> */
	T_ASR, N_SHIFT_EXP, A_OP_ASR, 
	/* 45:additive_exp(2) -> */
	N_MULT_EXP, N_ADDITIVE_EXP1, 
	/* 46:additive_exp1(0) -> */
	
	/* 47:additive_exp1(3) -> */
	T_PLUS, N_ADDITIVE_EXP, A_OP_ADD, 
	/* 48:additive_exp1(3) -> */
	T_MINUS, N_ADDITIVE_EXP, A_OP_SUB, 
	/* 49:additive_exp1(3) -> */
	T_XOR, N_ADDITIVE_EXP, A_OP_XOR, 
	/* 50:additive_exp1(3) -> */
	T_OR, N_ADDITIVE_EXP, A_OP_OR, 
	/* 51:additive_exp1(3) -> */
	T_LOR, N_ADDITIVE_EXP, A_OP_LOGIC_OR, 
	/* 52:mult_exp(2) -> */
	N_UNARY_EXP, N_MULT_EXP1, 
	/* 53:mult_exp1(0) -> */
	
	/* 54:mult_exp1(3) -> */
	T_MUL, N_MULT_EXP, A_OP_MUL, 
	/* 55:mult_exp1(3) -> */
	T_DIV, N_MULT_EXP, A_OP_DIV, 
	/* 56:mult_exp1(3) -> */
	T_MOD, N_MULT_EXP, A_OP_MOD, 
	/* 57:mult_exp1(3) -> */
	T_AND, N_MULT_EXP, A_OP_AND, 
	/* 58:mult_exp1(3) -> */
	T_LAND, N_MULT_EXP, A_OP_LOGIC_AND, 
	/* 59:unary_exp(1) -> */
	N_PRIMARY_EXP, 
	/* 60:unary_exp(3) -> */
	T_INV, N_UNARY_EXP, A_OP_INV, 
	/* 61:unary_exp(3) -> */
	T_MINUS, N_UNARY_EXP, A_OP_MINUS, 
	/* 62:unary_exp(3) -> */
	T_NOT, N_UNARY_EXP, A_OP_NOT, 
	/* 63:primary_exp(3) -> */
	T_LPAREN, N_EXP, T_RPAREN, 
	/* 64:primary_exp(2) -> */
	T_INT, A_OP_PUSH_INT, 
	/* 65:primary_exp(2) -> */
	T_TRUE, A_OP_PUSH_TRUE, 
	/* 66:primary_exp(2) -> */
	T_FALSE, A_OP_PUSH_FALSE, 
	/* 67:primary_exp(3) -> */
	N_METH_OR_ATTR, A_OP_METH_OR_ATTR, A_OP_POP_SYM, 
	/* 68:meth_or_attr(3) -> */
	T_ID, A_OP_PUSH_SYM, N_METH_OR_ATTR1, 
	/* 69:meth_or_attr1(0) -> */
	
	/* 70:meth_or_attr1(4) -> */
	T_LPAREN, A_OP_METHOD, N_ARG_LIST_OPT, T_RPAREN, 
};
/* 172 bytes */

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
	{   9, 13},
	{  22,  6},
	{  28,  6},
	{  34,  3},
	{  37,  2},
	{  39,  0},
	{  39,  3},
	{  42,  3},
	{  45,  2},
	{  47,  0},
	{  47,  2},
	{  49,  5},
	{  54,  0},
	{  54,  3},
	{  57,  4},
	{  61,  3},
	{  64,  6},
	{  70,  0},
	{  70,  1},
	{  71,  3},
	{  74,  0},
	{  74,  2},
	{  76,  0},
	{  76,  1},
	{  77,  2},
	{  79,  0},
	{  79,  2},
	{  81,  2},
	{  83,  0},
	{  83,  3},
	{  86,  3},
	{  89,  3},
	{  92,  3},
	{  95,  3},
	{  98,  3},
	{ 101,  2},
	{ 103,  0},
	{ 103,  3},
	{ 106,  3},
	{ 109,  2},
	{ 111,  0},
	{ 111,  3},
	{ 114,  3},
	{ 117,  3},
	{ 120,  3},
	{ 123,  3},
	{ 126,  2},
	{ 128,  0},
	{ 128,  3},
	{ 131,  3},
	{ 134,  3},
	{ 137,  3},
	{ 140,  3},
	{ 143,  1},
	{ 144,  3},
	{ 147,  3},
	{ 150,  3},
	{ 153,  3},
	{ 156,  2},
	{ 158,  2},
	{ 160,  2},
	{ 162,  3},
	{ 165,  3},
	{ 168,  0},
	{ 168,  4},
};
/* 142 bytes */

/* Total: 758 bytes */


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
 	"IF",
 	"PRINT",
 	"TRUE",
 	"VAR",
 	"WHILE",
 	"ID",
 	"INT",
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
 	"shift_exp",
 	"relational_exp",
 	"additive_exp",
 	"shift_exp1",
 	"mult_exp",
 	"additive_exp1",
 	"unary_exp",
 	"mult_exp1",
 	"primary_exp",
 	"meth_or_attr",
 	"meth_or_attr1",
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
 	"op_push_sym",
 	"op_pop_sym",
 	"op_assign",
 	"op_method",
 	"op_meth_or_attr",
 	"op_ret_discard",
 	"op_arg",
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
 	"op_push_true",
 	"op_push_false",
 };

