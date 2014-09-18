/* LL(1) Embeddedd Nonrecursive Predictive Parser */

#include "microjs_ll.h"

/* Token-Rule pair (predict set) */
struct tr_pair {
	uint8_t t;
	uint8_t r;
};

/* Predict sets */
static const struct tr_pair predict_vec[] = {
	/*  52 (program) */
	{  0,  0}, {  9,  2}, { 35,  1}, { 37,  1}, { 39,  1}, { 40,  1}, 
	{ 41,  1}, { 43,  1}, { 48,  1}, 
	/*  53 (stat) */
	{ 35,  6}, { 37,  8}, { 39, 11}, { 40,  7}, { 41,  9}, { 43, 10}, 
	{ 48, 12}, 
	/*  54 (compound_stat) */
	{  9,  3}, 
	/*  55 (stat_list) */
	{ 10,  4}, { 35,  5}, { 37,  5}, { 39,  5}, { 40,  5}, { 41,  5}, 
	{ 43,  5}, { 48,  5}, 
	/*  56 (exp_lst_opt) */
	{  3, 35}, {  8, 35}, { 48, 36}, 
	/*  57 (exp) */
	{  7, 40}, { 20, 40}, { 29, 40}, { 30, 40}, { 34, 40}, { 38, 40}, 
	{ 48, 40}, { 49, 40}, { 50, 40}, 
	/*  58 (condition) */
	{  7, 19}, 
	/*  59 (else_opt) */
	{  0, 13}, {  9, 13}, { 10, 13}, { 33, 14}, { 35, 13}, { 37, 13}, 
	{ 39, 13}, { 40, 13}, { 41, 13}, { 43, 13}, { 48, 13}, 
	/*  60 (catch_opt) */
	{  0, 15}, {  9, 15}, { 10, 15}, { 35, 15}, { 37, 15}, { 39, 15}, 
	{ 40, 15}, { 41, 15}, { 42, 16}, { 43, 15}, { 44, 15}, { 48, 15}, 
	/*  61 (var_list) */
	{ 48, 20}, 
	/*  62 (assign_or_call) */
	{ 48, 26}, 
	/*  63 (finally_opt) */
	{  0, 17}, {  9, 17}, { 10, 17}, { 35, 17}, { 37, 17}, { 39, 17}, 
	{ 40, 17}, { 41, 17}, { 43, 17}, { 44, 18}, { 48, 17}, 
	/*  64 (var) */
	{ 48, 23}, 
	/*  65 (var_list1) */
	{  2, 22}, {  3, 21}, 
	/*  66 (var_assign_opt) */
	{  2, 24}, {  3, 24}, { 32, 25}, 
	/*  67 (assign_or_call1) */
	{  7, 28}, { 32, 27}, 
	/*  68 (function_call) */
	{  7, 29}, 
	/*  69 (arg_list_opt) */
	{  7, 31}, {  8, 30}, { 20, 31}, { 29, 31}, { 30, 31}, { 34, 31}, 
	{ 38, 31}, { 48, 31}, { 49, 31}, { 50, 31}, 
	/*  70 (arg_list) */
	{  7, 32}, { 20, 32}, { 29, 32}, { 30, 32}, { 34, 32}, { 38, 32}, 
	{ 48, 32}, { 49, 32}, { 50, 32}, 
	/*  71 (arg_list1) */
	{  2, 34}, {  8, 33}, 
	/*  72 (exp_lst) */
	{ 48, 37}, 
	/*  73 (exp_lst1) */
	{  2, 39}, {  3, 38}, {  8, 38}, 
	/*  74 (and_exp) */
	{  7, 45}, { 20, 45}, { 29, 45}, { 30, 45}, { 34, 45}, { 38, 45}, 
	{ 48, 45}, { 49, 45}, { 50, 45}, 
	/*  75 (or_exp) */
	{  2, 41}, {  3, 41}, {  8, 41}, { 24, 42}, { 25, 43}, { 28, 44}, 
	/*  76 (relational_exp) */
	{  7, 49}, { 20, 49}, { 29, 49}, { 30, 49}, { 34, 49}, { 38, 49}, 
	{ 48, 49}, { 49, 49}, { 50, 49}, 
	/*  77 (and_exp1) */
	{  2, 46}, {  3, 46}, {  8, 46}, { 24, 46}, { 25, 46}, { 26, 47}, 
	{ 27, 48}, { 28, 46}, 
	/*  78 (shift_exp) */
	{  7, 57}, { 20, 57}, { 29, 57}, { 30, 57}, { 34, 57}, { 38, 57}, 
	{ 48, 57}, { 49, 57}, { 50, 57}, 
	/*  79 (relational_exp1) */
	{  2, 50}, {  3, 50}, {  8, 50}, { 13, 56}, { 14, 53}, { 15, 55}, 
	{ 16, 54}, { 17, 51}, { 18, 52}, { 24, 50}, { 25, 50}, { 26, 50}, 
	{ 27, 50}, { 28, 50}, 
	/*  80 (additive_exp) */
	{  7, 61}, { 20, 61}, { 29, 61}, { 30, 61}, { 34, 61}, { 38, 61}, 
	{ 48, 61}, { 49, 61}, { 50, 61}, 
	/*  81 (shift_exp1) */
	{  2, 58}, {  3, 58}, {  8, 58}, { 11, 60}, { 12, 59}, { 13, 58}, 
	{ 14, 58}, { 15, 58}, { 16, 58}, { 17, 58}, { 18, 58}, { 24, 58}, 
	{ 25, 58}, { 26, 58}, { 27, 58}, { 28, 58}, 
	/*  82 (mult_exp) */
	{  7, 65}, { 20, 65}, { 29, 65}, { 30, 65}, { 34, 65}, { 38, 65}, 
	{ 48, 65}, { 49, 65}, { 50, 65}, 
	/*  83 (additive_exp1) */
	{  2, 62}, {  3, 62}, {  8, 62}, { 11, 62}, { 12, 62}, { 13, 62}, 
	{ 14, 62}, { 15, 62}, { 16, 62}, { 17, 62}, { 18, 62}, { 19, 63}, 
	{ 20, 64}, { 24, 62}, { 25, 62}, { 26, 62}, { 27, 62}, { 28, 62}, 
	/*  84 (unary_exp) */
	{  7, 70}, { 20, 72}, { 29, 73}, { 30, 71}, { 34, 70}, { 38, 70}, 
	{ 48, 70}, { 49, 70}, { 50, 70}, 
	/*  85 (mult_exp1) */
	{  2, 66}, {  3, 66}, {  8, 66}, { 11, 66}, { 12, 66}, { 13, 66}, 
	{ 14, 66}, { 15, 66}, { 16, 66}, { 17, 66}, { 18, 66}, { 19, 66}, 
	{ 20, 66}, { 21, 67}, { 22, 68}, { 23, 69}, { 24, 66}, { 25, 66}, 
	{ 26, 66}, { 27, 66}, { 28, 66}, 
	/*  86 (primary_exp) */
	{  7, 74}, { 34, 77}, { 38, 76}, { 48, 79}, { 49, 75}, { 50, 78}, 
	/*  87 (meth_or_attr) */
	{ 48, 80}, 
	/*  88 (meth_or_attr1) */
	{  2, 81}, {  3, 81}, {  7, 82}, {  8, 81}, { 11, 81}, { 12, 81}, 
	{ 13, 81}, { 14, 81}, { 15, 81}, { 16, 81}, { 17, 81}, { 18, 81}, 
	{ 19, 81}, { 20, 81}, { 21, 81}, { 22, 81}, { 23, 81}, { 24, 81}, 
	{ 25, 81}, { 26, 81}, { 27, 81}, { 28, 81}, 
};
/* 548 bytes; */

static const struct {
	uint16_t off;
	uint8_t cnt;
} predict_idx[] = {
	{   0,  9},
	{   9,  7},
	{  16,  1},
	{  17,  8},
	{  25,  3},
	{  28,  9},
	{  37,  1},
	{  38, 11},
	{  49, 12},
	{  61,  1},
	{  62,  1},
	{  63, 11},
	{  74,  1},
	{  75,  2},
	{  77,  3},
	{  80,  2},
	{  82,  1},
	{  83, 10},
	{  93,  9},
	{ 102,  2},
	{ 104,  1},
	{ 105,  3},
	{ 108,  9},
	{ 117,  6},
	{ 123,  9},
	{ 132,  8},
	{ 140,  9},
	{ 149, 14},
	{ 163,  9},
	{ 172, 16},
	{ 188,  9},
	{ 197, 18},
	{ 215,  9},
	{ 224, 21},
	{ 245,  6},
	{ 251,  1},
	{ 252, 22},
};
/* 148 bytes */

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
	/*  9:stat(4) -> */
	T_TRY, A_OP_TRY_BEGIN, N_COMPOUND_STAT, N_CATCH_OPT, 
	/* 10:stat(4) -> */
	T_THROW, N_EXP, A_OP_THROW, T_SEMICOLON, 
	/* 11:stat(3) -> */
	T_VAR, N_VAR_LIST, T_SEMICOLON, 
	/* 12:stat(2) -> */
	N_ASSIGN_OR_CALL, T_SEMICOLON, 
	/* 13:else_opt(0) -> */
	
	/* 14:else_opt(3) -> */
	T_ELSE, A_OP_IF_ELSE, N_COMPOUND_STAT, 
	/* 15:catch_opt(2) -> */
	A_OP_TRY_END, N_FINALLY_OPT, 
	/* 16:catch_opt(15) -> */
	T_CATCH, A_OP_CATCH, A_OP_BLK_OPEN, T_LPAREN, T_ID, A_OP_VAR_DECL, A_OP_PUSH_TMP, A_OP_ASSIGN, T_RPAREN, T_LBRACE, N_STAT_LIST, T_RBRACE, A_OP_BLK_CLOSE, A_OP_CATCH_END, N_FINALLY_OPT, 
	/* 17:finally_opt(0) -> */
	
	/* 18:finally_opt(3) -> */
	T_FINALLY, A_OP_FINALLY, N_COMPOUND_STAT, 
	/* 19:condition(3) -> */
	T_LPAREN, N_EXP, T_RPAREN, 
	/* 20:var_list(2) -> */
	N_VAR, N_VAR_LIST1, 
	/* 21:var_list1(0) -> */
	
	/* 22:var_list1(2) -> */
	T_COMMA, N_VAR_LIST, 
	/* 23:var(4) -> */
	T_ID, A_OP_VAR_DECL, A_OP_PUSH_TMP, N_VAR_ASSIGN_OPT, 
	/* 24:var_assign_opt(1) -> */
	A_OP_POP_TMP, 
	/* 25:var_assign_opt(3) -> */
	T_ASSIGN, N_EXP, A_OP_ASSIGN, 
	/* 26:assign_or_call(3) -> */
	T_ID, A_OP_PUSH_TMP, N_ASSIGN_OR_CALL1, 
	/* 27:assign_or_call1(3) -> */
	T_ASSIGN, N_EXP, A_OP_ASSIGN, 
	/* 28:assign_or_call1(2) -> */
	N_FUNCTION_CALL, A_OP_RET_DISCARD, 
	/* 29:function_call(5) -> */
	A_OP_METHOD, T_LPAREN, N_ARG_LIST_OPT, T_RPAREN, A_OP_CALL, 
	/* 30:arg_list_opt(0) -> */
	
	/* 31:arg_list_opt(1) -> */
	N_ARG_LIST, 
	/* 32:arg_list(3) -> */
	N_EXP, A_OP_ARG, N_ARG_LIST1, 
	/* 33:arg_list1(0) -> */
	
	/* 34:arg_list1(2) -> */
	T_COMMA, N_ARG_LIST, 
	/* 35:exp_lst_opt(0) -> */
	
	/* 36:exp_lst_opt(1) -> */
	N_EXP_LST, 
	/* 37:exp_lst(2) -> */
	N_ASSIGN_OR_CALL, N_EXP_LST1, 
	/* 38:exp_lst1(0) -> */
	
	/* 39:exp_lst1(2) -> */
	T_COMMA, N_EXP_LST, 
	/* 40:exp(2) -> */
	N_AND_EXP, N_OR_EXP, 
	/* 41:or_exp(0) -> */
	
	/* 42:or_exp(3) -> */
	T_OR, N_EXP, A_OP_OR, 
	/* 43:or_exp(3) -> */
	T_LOR, N_EXP, A_OP_LOGIC_OR, 
	/* 44:or_exp(3) -> */
	T_XOR, N_EXP, A_OP_XOR, 
	/* 45:and_exp(2) -> */
	N_RELATIONAL_EXP, N_AND_EXP1, 
	/* 46:and_exp1(0) -> */
	
	/* 47:and_exp1(3) -> */
	T_AND, N_AND_EXP, A_OP_AND, 
	/* 48:and_exp1(3) -> */
	T_LAND, N_AND_EXP, A_OP_LOGIC_AND, 
	/* 49:relational_exp(2) -> */
	N_SHIFT_EXP, N_RELATIONAL_EXP1, 
	/* 50:relational_exp1(0) -> */
	
	/* 51:relational_exp1(3) -> */
	T_EQU, N_RELATIONAL_EXP, A_OP_EQU, 
	/* 52:relational_exp1(3) -> */
	T_NEQ, N_RELATIONAL_EXP, A_OP_NEQ, 
	/* 53:relational_exp1(3) -> */
	T_LT, N_RELATIONAL_EXP, A_OP_LT, 
	/* 54:relational_exp1(3) -> */
	T_GT, N_RELATIONAL_EXP, A_OP_GT, 
	/* 55:relational_exp1(3) -> */
	T_GTE, N_RELATIONAL_EXP, A_OP_GTE, 
	/* 56:relational_exp1(3) -> */
	T_LTE, N_RELATIONAL_EXP, A_OP_LTE, 
	/* 57:shift_exp(2) -> */
	N_ADDITIVE_EXP, N_SHIFT_EXP1, 
	/* 58:shift_exp1(0) -> */
	
	/* 59:shift_exp1(3) -> */
	T_SHL, N_SHIFT_EXP, A_OP_SHL, 
	/* 60:shift_exp1(3) -> */
	T_ASR, N_SHIFT_EXP, A_OP_ASR, 
	/* 61:additive_exp(2) -> */
	N_MULT_EXP, N_ADDITIVE_EXP1, 
	/* 62:additive_exp1(0) -> */
	
	/* 63:additive_exp1(3) -> */
	T_PLUS, N_ADDITIVE_EXP, A_OP_ADD, 
	/* 64:additive_exp1(3) -> */
	T_MINUS, N_ADDITIVE_EXP, A_OP_SUB, 
	/* 65:mult_exp(2) -> */
	N_UNARY_EXP, N_MULT_EXP1, 
	/* 66:mult_exp1(0) -> */
	
	/* 67:mult_exp1(3) -> */
	T_MUL, N_MULT_EXP, A_OP_MUL, 
	/* 68:mult_exp1(3) -> */
	T_DIV, N_MULT_EXP, A_OP_DIV, 
	/* 69:mult_exp1(3) -> */
	T_MOD, N_MULT_EXP, A_OP_MOD, 
	/* 70:unary_exp(1) -> */
	N_PRIMARY_EXP, 
	/* 71:unary_exp(3) -> */
	T_INV, N_UNARY_EXP, A_OP_INV, 
	/* 72:unary_exp(3) -> */
	T_MINUS, N_UNARY_EXP, A_OP_MINUS, 
	/* 73:unary_exp(3) -> */
	T_NOT, N_UNARY_EXP, A_OP_NOT, 
	/* 74:primary_exp(3) -> */
	T_LPAREN, N_EXP, T_RPAREN, 
	/* 75:primary_exp(2) -> */
	T_INT, A_OP_PUSH_INT, 
	/* 76:primary_exp(2) -> */
	T_TRUE, A_OP_PUSH_TRUE, 
	/* 77:primary_exp(2) -> */
	T_FALSE, A_OP_PUSH_FALSE, 
	/* 78:primary_exp(2) -> */
	T_STRING, A_OP_PUSH_STRING, 
	/* 79:primary_exp(1) -> */
	N_METH_OR_ATTR, 
	/* 80:meth_or_attr(3) -> */
	T_ID, A_OP_PUSH_TMP, N_METH_OR_ATTR1, 
	/* 81:meth_or_attr1(1) -> */
	A_OP_ATTR, 
	/* 82:meth_or_attr1(1) -> */
	N_FUNCTION_CALL, 
};
/* 205 bytes */

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
	{  37,  4},
	{  41,  4},
	{  45,  3},
	{  48,  2},
	{  50,  0},
	{  50,  3},
	{  53,  2},
	{  55, 15},
	{  70,  0},
	{  70,  3},
	{  73,  3},
	{  76,  2},
	{  78,  0},
	{  78,  2},
	{  80,  4},
	{  84,  1},
	{  85,  3},
	{  88,  3},
	{  91,  3},
	{  94,  2},
	{  96,  5},
	{ 101,  0},
	{ 101,  1},
	{ 102,  3},
	{ 105,  0},
	{ 105,  2},
	{ 107,  0},
	{ 107,  1},
	{ 108,  2},
	{ 110,  0},
	{ 110,  2},
	{ 112,  2},
	{ 114,  0},
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
	{ 139,  3},
	{ 142,  3},
	{ 145,  3},
	{ 148,  3},
	{ 151,  2},
	{ 153,  0},
	{ 153,  3},
	{ 156,  3},
	{ 159,  2},
	{ 161,  0},
	{ 161,  3},
	{ 164,  3},
	{ 167,  2},
	{ 169,  0},
	{ 169,  3},
	{ 172,  3},
	{ 175,  3},
	{ 178,  1},
	{ 179,  3},
	{ 182,  3},
	{ 185,  3},
	{ 188,  3},
	{ 191,  2},
	{ 193,  2},
	{ 195,  2},
	{ 197,  2},
	{ 199,  1},
	{ 200,  3},
	{ 203,  1},
	{ 204,  1},
};
/* 166 bytes */

/* Total: 1067 bytes */


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
 	"TRY",
 	"CATCH",
 	"THROW",
 	"FINALLY",
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
 	"catch_opt",
 	"var_list",
 	"assign_or_call",
 	"finally_opt",
 	"var",
 	"var_list1",
 	"var_assign_opt",
 	"assign_or_call1",
 	"function_call",
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
 	"op_try_begin",
 	"op_throw",
 	"op_if_else",
 	"op_try_end",
 	"op_catch",
 	"op_var_decl",
 	"op_push_tmp",
 	"op_assign",
 	"op_catch_end",
 	"op_finally",
 	"op_pop_tmp",
 	"op_ret_discard",
 	"op_method",
 	"op_call",
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
 	"op_attr",
 };

