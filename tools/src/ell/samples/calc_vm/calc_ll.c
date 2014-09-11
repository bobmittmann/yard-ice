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
	{  0,  0}, {  3,  1}, { 35,  1}, { 36,  1}, { 37,  1}, { 39,  1}, 
	{ 40,  1}, { 41,  1}, 
	/*  45 (stat) */
	{  3,  5}, { 35,  6}, { 36,  8}, { 37,  9}, { 39, 10}, { 40,  7}, 
	{ 41, 11}, 
	/*  46 (compound_stat) */
	{  9,  2}, 
	/*  47 (stat_list) */
	{  3,  4}, { 10,  3}, { 35,  4}, { 36,  4}, { 37,  4}, { 39,  4}, 
	{ 40,  4}, { 41,  4}, 
	/*  48 (exp_lst_opt) */
	{  3, 34}, {  8, 34}, { 41, 35}, 
	/*  49 (exp) */
	{  7, 39}, { 20, 39}, { 29, 39}, { 30, 39}, { 34, 39}, { 38, 39}, 
	{ 41, 39}, { 42, 39}, 
	/*  50 (condition) */
	{  7, 19}, 
	/*  51 (else_opt) */
	{  0, 17}, {  3, 17}, { 10, 17}, { 33, 18}, { 35, 17}, { 36, 17}, 
	{ 37, 17}, { 39, 17}, { 40, 17}, { 41, 17}, 
	/*  52 (print_list_opt) */
	{  3, 12}, {  7, 13}, { 20, 13}, { 29, 13}, { 30, 13}, { 34, 13}, 
	{ 38, 13}, { 41, 13}, { 42, 13}, 
	/*  53 (var_list) */
	{ 41, 20}, 
	/*  54 (assign_or_call) */
	{ 41, 26}, 
	/*  55 (print_list) */
	{  7, 14}, { 20, 14}, { 29, 14}, { 30, 14}, { 34, 14}, { 38, 14}, 
	{ 41, 14}, { 42, 14}, 
	/*  56 (print_list1) */
	{  2, 16}, {  3, 15}, 
	/*  57 (var) */
	{ 41, 23}, 
	/*  58 (var_list1) */
	{  2, 22}, {  3, 21}, 
	/*  59 (var_assign_opt) */
	{  2, 24}, {  3, 24}, { 32, 25}, 
	/*  60 (assign_or_call1) */
	{  7, 28}, { 32, 27}, 
	/*  61 (arg_list_opt) */
	{  7, 30}, {  8, 29}, { 20, 30}, { 29, 30}, { 30, 30}, { 34, 30}, 
	{ 38, 30}, { 41, 30}, { 42, 30}, 
	/*  62 (arg_list) */
	{  7, 31}, { 20, 31}, { 29, 31}, { 30, 31}, { 34, 31}, { 38, 31}, 
	{ 41, 31}, { 42, 31}, 
	/*  63 (arg_list1) */
	{  2, 33}, {  8, 32}, 
	/*  64 (exp_lst) */
	{ 41, 36}, 
	/*  65 (exp_lst1) */
	{  2, 38}, {  3, 37}, {  8, 37}, 
	/*  66 (shift_exp) */
	{  7, 47}, { 20, 47}, { 29, 47}, { 30, 47}, { 34, 47}, { 38, 47}, 
	{ 41, 47}, { 42, 47}, 
	/*  67 (relational_exp) */
	{  2, 40}, {  3, 40}, {  8, 40}, { 13, 46}, { 14, 43}, { 15, 45}, 
	{ 16, 44}, { 17, 41}, { 18, 42}, 
	/*  68 (additive_exp) */
	{  7, 51}, { 20, 51}, { 29, 51}, { 30, 51}, { 34, 51}, { 38, 51}, 
	{ 41, 51}, { 42, 51}, 
	/*  69 (shift_exp1) */
	{  2, 48}, {  3, 48}, {  8, 48}, { 11, 50}, { 12, 49}, { 13, 48}, 
	{ 14, 48}, { 15, 48}, { 16, 48}, { 17, 48}, { 18, 48}, 
	/*  70 (mult_exp) */
	{  7, 58}, { 20, 58}, { 29, 58}, { 30, 58}, { 34, 58}, { 38, 58}, 
	{ 41, 58}, { 42, 58}, 
	/*  71 (additive_exp1) */
	{  2, 52}, {  3, 52}, {  8, 52}, { 11, 52}, { 12, 52}, { 13, 52}, 
	{ 14, 52}, { 15, 52}, { 16, 52}, { 17, 52}, { 18, 52}, { 19, 53}, 
	{ 20, 54}, { 24, 56}, { 25, 57}, { 28, 55}, 
	/*  72 (unary_exp) */
	{  7, 65}, { 20, 67}, { 29, 68}, { 30, 66}, { 34, 65}, { 38, 65}, 
	{ 41, 65}, { 42, 65}, 
	/*  73 (mult_exp1) */
	{  2, 59}, {  3, 59}, {  8, 59}, { 11, 59}, { 12, 59}, { 13, 59}, 
	{ 14, 59}, { 15, 59}, { 16, 59}, { 17, 59}, { 18, 59}, { 19, 59}, 
	{ 20, 59}, { 21, 60}, { 22, 61}, { 23, 62}, { 24, 59}, { 25, 59}, 
	{ 26, 63}, { 27, 64}, { 28, 59}, 
	/*  74 (primary_exp) */
	{  7, 69}, { 34, 72}, { 38, 71}, { 41, 73}, { 42, 70}, 
	/*  75 (meth_or_attr) */
	{ 41, 74}, 
	/*  76 (meth_or_attr1) */
	{  2, 75}, {  3, 75}, {  7, 76}, {  8, 75}, { 11, 75}, { 12, 75}, 
	{ 13, 75}, { 14, 75}, { 15, 75}, { 16, 75}, { 17, 75}, { 18, 75}, 
	{ 19, 75}, { 20, 75}, { 21, 75}, { 22, 75}, { 23, 75}, { 24, 75}, 
	{ 25, 75}, { 26, 75}, { 27, 75}, { 28, 75}, 
};
/* 430 bytes; */

static const struct {
	uint8_t off;
	uint8_t cnt;
} predict_idx[] = {
	{   0,  8},
	{   8,  7},
	{  15,  1},
	{  16,  8},
	{  24,  3},
	{  27,  8},
	{  35,  1},
	{  36, 10},
	{  46,  9},
	{  55,  1},
	{  56,  1},
	{  57,  8},
	{  65,  2},
	{  67,  1},
	{  68,  2},
	{  70,  3},
	{  73,  2},
	{  75,  9},
	{  84,  8},
	{  92,  2},
	{  94,  1},
	{  95,  3},
	{  98,  8},
	{ 106,  9},
	{ 115,  8},
	{ 123, 11},
	{ 134,  8},
	{ 142, 16},
	{ 158,  8},
	{ 166, 21},
	{ 187,  5},
	{ 192,  1},
	{ 193, 22},
};
/* 66 bytes */

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
	/*  9:stat(4) -> */
	T_PRINT, N_PRINT_LIST_OPT, T_SEMICOLON, A_OP_PRINT_END, 
	/* 10:stat(3) -> */
	T_VAR, N_VAR_LIST, T_SEMICOLON, 
	/* 11:stat(2) -> */
	N_ASSIGN_OR_CALL, T_SEMICOLON, 
	/* 12:print_list_opt(0) -> */
	
	/* 13:print_list_opt(1) -> */
	N_PRINT_LIST, 
	/* 14:print_list(3) -> */
	N_EXP, A_OP_PRINT_INT, N_PRINT_LIST1, 
	/* 15:print_list1(0) -> */
	
	/* 16:print_list1(3) -> */
	T_COMMA, A_OP_PRINT_COMMA, N_PRINT_LIST, 
	/* 17:else_opt(0) -> */
	
	/* 18:else_opt(3) -> */
	T_ELSE, A_OP_IF_ELSE, N_COMPOUND_STAT, 
	/* 19:condition(3) -> */
	T_LPAREN, N_EXP, T_RPAREN, 
	/* 20:var_list(2) -> */
	N_VAR, N_VAR_LIST1, 
	/* 21:var_list1(0) -> */
	
	/* 22:var_list1(2) -> */
	T_COMMA, N_VAR_LIST, 
	/* 23:var(5) -> */
	T_ID, A_OP_VAR_DECL, A_OP_PUSH_SYM, N_VAR_ASSIGN_OPT, A_OP_POP_SYM, 
	/* 24:var_assign_opt(0) -> */
	
	/* 25:var_assign_opt(3) -> */
	T_ASSIGN, N_EXP, A_OP_ASSIGN, 
	/* 26:assign_or_call(4) -> */
	T_ID, A_OP_PUSH_SYM, N_ASSIGN_OR_CALL1, A_OP_POP_SYM, 
	/* 27:assign_or_call1(3) -> */
	T_ASSIGN, N_EXP, A_OP_ASSIGN, 
	/* 28:assign_or_call1(6) -> */
	T_LPAREN, A_OP_METHOD, N_ARG_LIST_OPT, T_RPAREN, A_OP_METH_OR_ATTR, A_OP_RET_DISCARD, 
	/* 29:arg_list_opt(0) -> */
	
	/* 30:arg_list_opt(1) -> */
	N_ARG_LIST, 
	/* 31:arg_list(3) -> */
	N_EXP, A_OP_ARG, N_ARG_LIST1, 
	/* 32:arg_list1(0) -> */
	
	/* 33:arg_list1(2) -> */
	T_COMMA, N_ARG_LIST, 
	/* 34:exp_lst_opt(0) -> */
	
	/* 35:exp_lst_opt(1) -> */
	N_EXP_LST, 
	/* 36:exp_lst(2) -> */
	N_ASSIGN_OR_CALL, N_EXP_LST1, 
	/* 37:exp_lst1(0) -> */
	
	/* 38:exp_lst1(2) -> */
	T_COMMA, N_EXP_LST, 
	/* 39:exp(2) -> */
	N_SHIFT_EXP, N_RELATIONAL_EXP, 
	/* 40:relational_exp(0) -> */
	
	/* 41:relational_exp(3) -> */
	T_EQU, N_EXP, A_OP_EQU, 
	/* 42:relational_exp(3) -> */
	T_NEQ, N_EXP, A_OP_NEQ, 
	/* 43:relational_exp(3) -> */
	T_LT, N_EXP, A_OP_LT, 
	/* 44:relational_exp(3) -> */
	T_GT, N_EXP, A_OP_GT, 
	/* 45:relational_exp(3) -> */
	T_GTE, N_EXP, A_OP_GTE, 
	/* 46:relational_exp(3) -> */
	T_LTE, N_EXP, A_OP_LTE, 
	/* 47:shift_exp(2) -> */
	N_ADDITIVE_EXP, N_SHIFT_EXP1, 
	/* 48:shift_exp1(0) -> */
	
	/* 49:shift_exp1(3) -> */
	T_SHL, N_SHIFT_EXP, A_OP_SHL, 
	/* 50:shift_exp1(3) -> */
	T_ASR, N_SHIFT_EXP, A_OP_ASR, 
	/* 51:additive_exp(2) -> */
	N_MULT_EXP, N_ADDITIVE_EXP1, 
	/* 52:additive_exp1(0) -> */
	
	/* 53:additive_exp1(3) -> */
	T_PLUS, N_ADDITIVE_EXP, A_OP_ADD, 
	/* 54:additive_exp1(3) -> */
	T_MINUS, N_ADDITIVE_EXP, A_OP_SUB, 
	/* 55:additive_exp1(3) -> */
	T_XOR, N_ADDITIVE_EXP, A_OP_XOR, 
	/* 56:additive_exp1(3) -> */
	T_OR, N_ADDITIVE_EXP, A_OP_OR, 
	/* 57:additive_exp1(3) -> */
	T_LOR, N_ADDITIVE_EXP, A_OP_LOGIC_OR, 
	/* 58:mult_exp(2) -> */
	N_UNARY_EXP, N_MULT_EXP1, 
	/* 59:mult_exp1(0) -> */
	
	/* 60:mult_exp1(3) -> */
	T_MUL, N_MULT_EXP, A_OP_MUL, 
	/* 61:mult_exp1(3) -> */
	T_DIV, N_MULT_EXP, A_OP_DIV, 
	/* 62:mult_exp1(3) -> */
	T_MOD, N_MULT_EXP, A_OP_MOD, 
	/* 63:mult_exp1(3) -> */
	T_AND, N_MULT_EXP, A_OP_AND, 
	/* 64:mult_exp1(3) -> */
	T_LAND, N_MULT_EXP, A_OP_LOGIC_AND, 
	/* 65:unary_exp(1) -> */
	N_PRIMARY_EXP, 
	/* 66:unary_exp(3) -> */
	T_INV, N_UNARY_EXP, A_OP_INV, 
	/* 67:unary_exp(3) -> */
	T_MINUS, N_UNARY_EXP, A_OP_MINUS, 
	/* 68:unary_exp(3) -> */
	T_NOT, N_UNARY_EXP, A_OP_NOT, 
	/* 69:primary_exp(3) -> */
	T_LPAREN, N_EXP, T_RPAREN, 
	/* 70:primary_exp(2) -> */
	T_INT, A_OP_PUSH_INT, 
	/* 71:primary_exp(2) -> */
	T_TRUE, A_OP_PUSH_TRUE, 
	/* 72:primary_exp(2) -> */
	T_FALSE, A_OP_PUSH_FALSE, 
	/* 73:primary_exp(3) -> */
	N_METH_OR_ATTR, A_OP_METH_OR_ATTR, A_OP_POP_SYM, 
	/* 74:meth_or_attr(3) -> */
	T_ID, A_OP_PUSH_SYM, N_METH_OR_ATTR1, 
	/* 75:meth_or_attr1(0) -> */
	
	/* 76:meth_or_attr1(4) -> */
	T_LPAREN, A_OP_METHOD, N_ARG_LIST_OPT, T_RPAREN, 
};
/* 183 bytes */

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
	{  34,  4},
	{  38,  3},
	{  41,  2},
	{  43,  0},
	{  43,  1},
	{  44,  3},
	{  47,  0},
	{  47,  3},
	{  50,  0},
	{  50,  3},
	{  53,  3},
	{  56,  2},
	{  58,  0},
	{  58,  2},
	{  60,  5},
	{  65,  0},
	{  65,  3},
	{  68,  4},
	{  72,  3},
	{  75,  6},
	{  81,  0},
	{  81,  1},
	{  82,  3},
	{  85,  0},
	{  85,  2},
	{  87,  0},
	{  87,  1},
	{  88,  2},
	{  90,  0},
	{  90,  2},
	{  92,  2},
	{  94,  0},
	{  94,  3},
	{  97,  3},
	{ 100,  3},
	{ 103,  3},
	{ 106,  3},
	{ 109,  3},
	{ 112,  2},
	{ 114,  0},
	{ 114,  3},
	{ 117,  3},
	{ 120,  2},
	{ 122,  0},
	{ 122,  3},
	{ 125,  3},
	{ 128,  3},
	{ 131,  3},
	{ 134,  3},
	{ 137,  2},
	{ 139,  0},
	{ 139,  3},
	{ 142,  3},
	{ 145,  3},
	{ 148,  3},
	{ 151,  3},
	{ 154,  1},
	{ 155,  3},
	{ 158,  3},
	{ 161,  3},
	{ 164,  3},
	{ 167,  2},
	{ 169,  2},
	{ 171,  2},
	{ 173,  3},
	{ 176,  3},
	{ 179,  0},
	{ 179,  4},
};
/* 154 bytes */

/* Total: 833 bytes */


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
 	"print_list_opt",
 	"var_list",
 	"assign_or_call",
 	"print_list",
 	"print_list1",
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
 	"op_print_end",
 	"op_print_int",
 	"op_print_comma",
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

