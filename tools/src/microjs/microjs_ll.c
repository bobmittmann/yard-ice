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
	{  3, 34}, {  8, 34}, { 48, 35}, 
	/*  57 (exp) */
	{  7, 39}, { 20, 39}, { 29, 39}, { 30, 39}, { 34, 39}, { 38, 39}, 
	{ 48, 39}, { 49, 39}, { 50, 39}, 
	/*  58 (condition) */
	{  7, 19}, 
	/*  59 (else_opt) */
	{  0, 13}, {  9, 13}, { 10, 13}, { 33, 14}, { 35, 13}, { 37, 13}, 
	{ 39, 13}, { 40, 13}, { 41, 13}, { 43, 13}, { 48, 13}, 
	/*  60 (catch_opt) */
	{  0, 15}, {  9, 15}, { 10, 15}, { 35, 15}, { 37, 15}, { 39, 15}, 
	{ 40, 15}, { 41, 15}, { 42, 16}, { 43, 15}, { 48, 15}, 
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
	/*  68 (arg_list_opt) */
	{  7, 30}, {  8, 29}, { 20, 30}, { 29, 30}, { 30, 30}, { 34, 30}, 
	{ 38, 30}, { 48, 30}, { 49, 30}, { 50, 30}, 
	/*  69 (arg_list) */
	{  7, 31}, { 20, 31}, { 29, 31}, { 30, 31}, { 34, 31}, { 38, 31}, 
	{ 48, 31}, { 49, 31}, { 50, 31}, 
	/*  70 (arg_list1) */
	{  2, 33}, {  8, 32}, 
	/*  71 (exp_lst) */
	{ 48, 36}, 
	/*  72 (exp_lst1) */
	{  2, 38}, {  3, 37}, {  8, 37}, 
	/*  73 (and_exp) */
	{  7, 44}, { 20, 44}, { 29, 44}, { 30, 44}, { 34, 44}, { 38, 44}, 
	{ 48, 44}, { 49, 44}, { 50, 44}, 
	/*  74 (or_exp) */
	{  2, 40}, {  3, 40}, {  8, 40}, { 24, 41}, { 25, 42}, { 28, 43}, 
	/*  75 (relational_exp) */
	{  7, 48}, { 20, 48}, { 29, 48}, { 30, 48}, { 34, 48}, { 38, 48}, 
	{ 48, 48}, { 49, 48}, { 50, 48}, 
	/*  76 (and_exp1) */
	{  2, 45}, {  3, 45}, {  8, 45}, { 24, 45}, { 25, 45}, { 26, 46}, 
	{ 27, 47}, { 28, 45}, 
	/*  77 (shift_exp) */
	{  7, 56}, { 20, 56}, { 29, 56}, { 30, 56}, { 34, 56}, { 38, 56}, 
	{ 48, 56}, { 49, 56}, { 50, 56}, 
	/*  78 (relational_exp1) */
	{  2, 49}, {  3, 49}, {  8, 49}, { 13, 55}, { 14, 52}, { 15, 54}, 
	{ 16, 53}, { 17, 50}, { 18, 51}, { 24, 49}, { 25, 49}, { 26, 49}, 
	{ 27, 49}, { 28, 49}, 
	/*  79 (additive_exp) */
	{  7, 60}, { 20, 60}, { 29, 60}, { 30, 60}, { 34, 60}, { 38, 60}, 
	{ 48, 60}, { 49, 60}, { 50, 60}, 
	/*  80 (shift_exp1) */
	{  2, 57}, {  3, 57}, {  8, 57}, { 11, 59}, { 12, 58}, { 13, 57}, 
	{ 14, 57}, { 15, 57}, { 16, 57}, { 17, 57}, { 18, 57}, { 24, 57}, 
	{ 25, 57}, { 26, 57}, { 27, 57}, { 28, 57}, 
	/*  81 (mult_exp) */
	{  7, 64}, { 20, 64}, { 29, 64}, { 30, 64}, { 34, 64}, { 38, 64}, 
	{ 48, 64}, { 49, 64}, { 50, 64}, 
	/*  82 (additive_exp1) */
	{  2, 61}, {  3, 61}, {  8, 61}, { 11, 61}, { 12, 61}, { 13, 61}, 
	{ 14, 61}, { 15, 61}, { 16, 61}, { 17, 61}, { 18, 61}, { 19, 62}, 
	{ 20, 63}, { 24, 61}, { 25, 61}, { 26, 61}, { 27, 61}, { 28, 61}, 
	/*  83 (unary_exp) */
	{  7, 69}, { 20, 71}, { 29, 72}, { 30, 70}, { 34, 69}, { 38, 69}, 
	{ 48, 69}, { 49, 69}, { 50, 69}, 
	/*  84 (mult_exp1) */
	{  2, 65}, {  3, 65}, {  8, 65}, { 11, 65}, { 12, 65}, { 13, 65}, 
	{ 14, 65}, { 15, 65}, { 16, 65}, { 17, 65}, { 18, 65}, { 19, 65}, 
	{ 20, 65}, { 21, 66}, { 22, 67}, { 23, 68}, { 24, 65}, { 25, 65}, 
	{ 26, 65}, { 27, 65}, { 28, 65}, 
	/*  85 (primary_exp) */
	{  7, 73}, { 34, 76}, { 38, 75}, { 48, 78}, { 49, 74}, { 50, 77}, 
	/*  86 (meth_or_attr) */
	{ 48, 79}, 
	/*  87 (meth_or_attr1) */
	{  2, 80}, {  3, 80}, {  7, 81}, {  8, 80}, { 11, 80}, { 12, 80}, 
	{ 13, 80}, { 14, 80}, { 15, 80}, { 16, 80}, { 17, 80}, { 18, 80}, 
	{ 19, 80}, { 20, 80}, { 21, 80}, { 22, 80}, { 23, 80}, { 24, 80}, 
	{ 25, 80}, { 26, 80}, { 27, 80}, { 28, 80}, 
};
/* 544 bytes; */

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
	{  49, 11},
	{  60,  1},
	{  61,  1},
	{  62, 11},
	{  73,  1},
	{  74,  2},
	{  76,  3},
	{  79,  2},
	{  81, 10},
	{  91,  9},
	{ 100,  2},
	{ 102,  1},
	{ 103,  3},
	{ 106,  9},
	{ 115,  6},
	{ 121,  9},
	{ 130,  8},
	{ 138,  9},
	{ 147, 14},
	{ 161,  9},
	{ 170, 16},
	{ 186,  9},
	{ 195, 18},
	{ 213,  9},
	{ 222, 21},
	{ 243,  6},
	{ 249,  1},
	{ 250, 22},
};
/* 144 bytes */

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
	/*  9:stat(5) -> */
	T_TRY, A_OP_TRY_BEGIN, N_COMPOUND_STAT, N_CATCH_OPT, A_OP_TRY_END, 
	/* 10:stat(4) -> */
	T_THROW, N_EXP, A_OP_THROW, T_SEMICOLON, 
	/* 11:stat(3) -> */
	T_VAR, N_VAR_LIST, T_SEMICOLON, 
	/* 12:stat(2) -> */
	N_ASSIGN_OR_CALL, T_SEMICOLON, 
	/* 13:else_opt(0) -> */
	
	/* 14:else_opt(3) -> */
	T_ELSE, A_OP_IF_ELSE, N_COMPOUND_STAT, 
	/* 15:catch_opt(0) -> */
	
	/* 16:catch_opt(7) -> */
	T_CATCH, T_LPAREN, T_ID, T_RPAREN, A_OP_CATCH, N_COMPOUND_STAT, N_FINALLY_OPT, 
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
	/* 23:var(5) -> */
	T_ID, A_OP_VAR_DECL, A_OP_PUSH_TMP, N_VAR_ASSIGN_OPT, A_OP_POP_TMP, 
	/* 24:var_assign_opt(0) -> */
	
	/* 25:var_assign_opt(3) -> */
	T_ASSIGN, N_EXP, A_OP_ASSIGN, 
	/* 26:assign_or_call(4) -> */
	T_ID, A_OP_PUSH_TMP, N_ASSIGN_OR_CALL1, A_OP_POP_TMP, 
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
	N_AND_EXP, N_OR_EXP, 
	/* 40:or_exp(0) -> */
	
	/* 41:or_exp(3) -> */
	T_OR, N_EXP, A_OP_OR, 
	/* 42:or_exp(3) -> */
	T_LOR, N_EXP, A_OP_LOGIC_OR, 
	/* 43:or_exp(3) -> */
	T_XOR, N_EXP, A_OP_XOR, 
	/* 44:and_exp(2) -> */
	N_RELATIONAL_EXP, N_AND_EXP1, 
	/* 45:and_exp1(0) -> */
	
	/* 46:and_exp1(3) -> */
	T_AND, N_AND_EXP, A_OP_AND, 
	/* 47:and_exp1(3) -> */
	T_LAND, N_AND_EXP, A_OP_LOGIC_AND, 
	/* 48:relational_exp(2) -> */
	N_SHIFT_EXP, N_RELATIONAL_EXP1, 
	/* 49:relational_exp1(0) -> */
	
	/* 50:relational_exp1(3) -> */
	T_EQU, N_RELATIONAL_EXP, A_OP_EQU, 
	/* 51:relational_exp1(3) -> */
	T_NEQ, N_RELATIONAL_EXP, A_OP_NEQ, 
	/* 52:relational_exp1(3) -> */
	T_LT, N_RELATIONAL_EXP, A_OP_LT, 
	/* 53:relational_exp1(3) -> */
	T_GT, N_RELATIONAL_EXP, A_OP_GT, 
	/* 54:relational_exp1(3) -> */
	T_GTE, N_RELATIONAL_EXP, A_OP_GTE, 
	/* 55:relational_exp1(3) -> */
	T_LTE, N_RELATIONAL_EXP, A_OP_LTE, 
	/* 56:shift_exp(2) -> */
	N_ADDITIVE_EXP, N_SHIFT_EXP1, 
	/* 57:shift_exp1(0) -> */
	
	/* 58:shift_exp1(3) -> */
	T_SHL, N_SHIFT_EXP, A_OP_SHL, 
	/* 59:shift_exp1(3) -> */
	T_ASR, N_SHIFT_EXP, A_OP_ASR, 
	/* 60:additive_exp(2) -> */
	N_MULT_EXP, N_ADDITIVE_EXP1, 
	/* 61:additive_exp1(0) -> */
	
	/* 62:additive_exp1(3) -> */
	T_PLUS, N_ADDITIVE_EXP, A_OP_ADD, 
	/* 63:additive_exp1(3) -> */
	T_MINUS, N_ADDITIVE_EXP, A_OP_SUB, 
	/* 64:mult_exp(2) -> */
	N_UNARY_EXP, N_MULT_EXP1, 
	/* 65:mult_exp1(0) -> */
	
	/* 66:mult_exp1(3) -> */
	T_MUL, N_MULT_EXP, A_OP_MUL, 
	/* 67:mult_exp1(3) -> */
	T_DIV, N_MULT_EXP, A_OP_DIV, 
	/* 68:mult_exp1(3) -> */
	T_MOD, N_MULT_EXP, A_OP_MOD, 
	/* 69:unary_exp(1) -> */
	N_PRIMARY_EXP, 
	/* 70:unary_exp(3) -> */
	T_INV, N_UNARY_EXP, A_OP_INV, 
	/* 71:unary_exp(3) -> */
	T_MINUS, N_UNARY_EXP, A_OP_MINUS, 
	/* 72:unary_exp(3) -> */
	T_NOT, N_UNARY_EXP, A_OP_NOT, 
	/* 73:primary_exp(3) -> */
	T_LPAREN, N_EXP, T_RPAREN, 
	/* 74:primary_exp(2) -> */
	T_INT, A_OP_PUSH_INT, 
	/* 75:primary_exp(2) -> */
	T_TRUE, A_OP_PUSH_TRUE, 
	/* 76:primary_exp(2) -> */
	T_FALSE, A_OP_PUSH_FALSE, 
	/* 77:primary_exp(2) -> */
	T_STRING, A_OP_PUSH_STRING, 
	/* 78:primary_exp(3) -> */
	N_METH_OR_ATTR, A_OP_METH_OR_ATTR, A_OP_POP_TMP, 
	/* 79:meth_or_attr(3) -> */
	T_ID, A_OP_PUSH_TMP, N_METH_OR_ATTR1, 
	/* 80:meth_or_attr1(0) -> */
	
	/* 81:meth_or_attr1(4) -> */
	T_LPAREN, A_OP_METHOD, N_ARG_LIST_OPT, T_RPAREN, 
};
/* 200 bytes */

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
	{  37,  5},
	{  42,  4},
	{  46,  3},
	{  49,  2},
	{  51,  0},
	{  51,  3},
	{  54,  0},
	{  54,  7},
	{  61,  0},
	{  61,  3},
	{  64,  3},
	{  67,  2},
	{  69,  0},
	{  69,  2},
	{  71,  5},
	{  76,  0},
	{  76,  3},
	{  79,  4},
	{  83,  3},
	{  86,  6},
	{  92,  0},
	{  92,  1},
	{  93,  3},
	{  96,  0},
	{  96,  2},
	{  98,  0},
	{  98,  1},
	{  99,  2},
	{ 101,  0},
	{ 101,  2},
	{ 103,  2},
	{ 105,  0},
	{ 105,  3},
	{ 108,  3},
	{ 111,  3},
	{ 114,  2},
	{ 116,  0},
	{ 116,  3},
	{ 119,  3},
	{ 122,  2},
	{ 124,  0},
	{ 124,  3},
	{ 127,  3},
	{ 130,  3},
	{ 133,  3},
	{ 136,  3},
	{ 139,  3},
	{ 142,  2},
	{ 144,  0},
	{ 144,  3},
	{ 147,  3},
	{ 150,  2},
	{ 152,  0},
	{ 152,  3},
	{ 155,  3},
	{ 158,  2},
	{ 160,  0},
	{ 160,  3},
	{ 163,  3},
	{ 166,  3},
	{ 169,  1},
	{ 170,  3},
	{ 173,  3},
	{ 176,  3},
	{ 179,  3},
	{ 182,  2},
	{ 184,  2},
	{ 186,  2},
	{ 188,  2},
	{ 190,  3},
	{ 193,  3},
	{ 196,  0},
	{ 196,  4},
};
/* 164 bytes */

/* Total: 1052 bytes */


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
 	"op_try_end",
 	"op_throw",
 	"op_if_else",
 	"op_catch",
 	"op_finally",
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

