/* LL(1) Embeddedd Nonrecursive Predictive Parser */

#include "microjs_ll.h"

/* Token-Rule pair (predict set) */
struct tr_pair {
	uint8_t t;
	uint8_t r;
};

/* Predict sets */
static const struct tr_pair predict_vec[] = {
	/*  54 (program) */
	{  0,  0}, { 23,  2}, { 39,  1}, { 41,  1}, { 43,  1}, { 45,  1}, 
	{ 46,  1}, { 47,  1}, { 48,  1}, 
	/*  55 (stat) */
	{ 39,  6}, { 41,  8}, { 43, 10}, { 45,  9}, { 46, 11}, { 47,  7}, 
	{ 48, 12}, 
	/*  56 (compound_stat) */
	{ 23,  3}, 
	/*  57 (stat_list) */
	{ 24,  4}, { 39,  5}, { 41,  5}, { 43,  5}, { 45,  5}, { 46,  5}, 
	{ 47,  5}, { 48,  5}, 
	/*  58 (exp_lst_opt) */
	{  3, 33}, { 22, 33}, { 48, 34}, 
	/*  59 (exp) */
	{  6, 38}, { 13, 38}, { 21, 38}, { 37, 38}, { 44, 38}, { 48, 38}, 
	{ 49, 38}, { 50, 38}, { 51, 38}, { 53, 38}, 
	/*  60 (condition) */
	{ 21, 17}, 
	/*  61 (else_opt) */
	{  0, 13}, { 23, 13}, { 24, 13}, { 36, 14}, { 39, 13}, { 41, 13}, 
	{ 43, 13}, { 45, 13}, { 46, 13}, { 47, 13}, { 48, 13}, 
	/*  62 (catch_opt) */
	{  0, 15}, { 23, 15}, { 24, 15}, { 34, 16}, { 39, 15}, { 41, 15}, 
	{ 43, 15}, { 45, 15}, { 46, 15}, { 47, 15}, { 48, 15}, 
	/*  63 (var_list) */
	{ 48, 18}, 
	/*  64 (assign_or_call) */
	{ 48, 24}, 
	/*  65 (var) */
	{ 48, 21}, 
	/*  66 (var_list1) */
	{  2, 20}, {  3, 19}, 
	/*  67 (var_assign_opt) */
	{  2, 22}, {  3, 22}, { 16, 23}, 
	/*  68 (assign_or_call1) */
	{ 16, 25}, { 21, 26}, 
	/*  69 (function_call) */
	{ 21, 27}, 
	/*  70 (arg_list_opt) */
	{  6, 29}, { 13, 29}, { 21, 29}, { 22, 28}, { 37, 29}, { 44, 29}, 
	{ 48, 29}, { 49, 29}, { 50, 29}, { 51, 29}, { 53, 29}, 
	/*  71 (arg_list) */
	{  6, 30}, { 13, 30}, { 21, 30}, { 37, 30}, { 44, 30}, { 48, 30}, 
	{ 49, 30}, { 50, 30}, { 51, 30}, { 53, 30}, 
	/*  72 (arg_list1) */
	{  2, 32}, { 22, 31}, 
	/*  73 (exp_lst) */
	{ 48, 35}, 
	/*  74 (exp_lst1) */
	{  2, 37}, {  3, 36}, { 22, 36}, 
	/*  75 (and_exp) */
	{  6, 43}, { 13, 43}, { 21, 43}, { 37, 43}, { 44, 43}, { 48, 43}, 
	{ 49, 43}, { 50, 43}, { 51, 43}, { 53, 43}, 
	/*  76 (or_exp) */
	{  2, 39}, {  3, 39}, { 11, 40}, { 12, 42}, { 22, 39}, { 31, 41}, 
	/*  77 (relational_exp) */
	{  6, 47}, { 13, 47}, { 21, 47}, { 37, 47}, { 44, 47}, { 48, 47}, 
	{ 49, 47}, { 50, 47}, { 51, 47}, { 53, 47}, 
	/*  78 (and_exp1) */
	{  2, 44}, {  3, 44}, { 10, 45}, { 11, 44}, { 12, 44}, { 22, 44}, 
	{ 31, 44}, { 32, 46}, 
	/*  79 (shift_exp) */
	{  6, 55}, { 13, 55}, { 21, 55}, { 37, 55}, { 44, 55}, { 48, 55}, 
	{ 49, 55}, { 50, 55}, { 51, 55}, { 53, 55}, 
	/*  80 (relational_exp1) */
	{  2, 48}, {  3, 48}, { 10, 48}, { 11, 48}, { 12, 48}, { 17, 49}, 
	{ 18, 50}, { 22, 48}, { 25, 53}, { 26, 54}, { 27, 51}, { 28, 52}, 
	{ 31, 48}, { 32, 48}, 
	/*  81 (additive_exp) */
	{  6, 59}, { 13, 59}, { 21, 59}, { 37, 59}, { 44, 59}, { 48, 59}, 
	{ 49, 59}, { 50, 59}, { 51, 59}, { 53, 59}, 
	/*  82 (shift_exp1) */
	{  2, 56}, {  3, 56}, { 10, 56}, { 11, 56}, { 12, 56}, { 17, 56}, 
	{ 18, 56}, { 22, 56}, { 25, 56}, { 26, 56}, { 27, 56}, { 28, 56}, 
	{ 29, 58}, { 30, 57}, { 31, 56}, { 32, 56}, 
	/*  83 (mult_exp) */
	{  6, 63}, { 13, 63}, { 21, 63}, { 37, 63}, { 44, 63}, { 48, 63}, 
	{ 49, 63}, { 50, 63}, { 51, 63}, { 53, 63}, 
	/*  84 (additive_exp1) */
	{  2, 60}, {  3, 60}, {  5, 61}, {  6, 62}, { 10, 60}, { 11, 60}, 
	{ 12, 60}, { 17, 60}, { 18, 60}, { 22, 60}, { 25, 60}, { 26, 60}, 
	{ 27, 60}, { 28, 60}, { 29, 60}, { 30, 60}, { 31, 60}, { 32, 60}, 
	/*  85 (unary_exp) */
	{  6, 70}, { 13, 69}, { 21, 68}, { 37, 68}, { 44, 68}, { 48, 68}, 
	{ 49, 68}, { 50, 68}, { 51, 68}, { 53, 71}, 
	/*  86 (mult_exp1) */
	{  2, 64}, {  3, 64}, {  5, 64}, {  6, 64}, {  7, 65}, {  8, 66}, 
	{  9, 67}, { 10, 64}, { 11, 64}, { 12, 64}, { 17, 64}, { 18, 64}, 
	{ 22, 64}, { 25, 64}, { 26, 64}, { 27, 64}, { 28, 64}, { 29, 64}, 
	{ 30, 64}, { 31, 64}, { 32, 64}, 
	/*  87 (primary_exp) */
	{ 21, 72}, { 37, 77}, { 44, 76}, { 48, 78}, { 49, 73}, { 50, 74}, 
	{ 51, 75}, 
	/*  88 (meth_or_attr) */
	{ 48, 79}, 
	/*  89 (meth_or_attr1) */
	{  2, 80}, {  3, 80}, {  5, 80}, {  6, 80}, {  7, 80}, {  8, 80}, 
	{  9, 80}, { 10, 80}, { 11, 80}, { 12, 80}, { 17, 80}, { 18, 80}, 
	{ 21, 81}, { 22, 80}, { 25, 80}, { 26, 80}, { 27, 80}, { 28, 80}, 
	{ 29, 80}, { 30, 80}, { 31, 80}, { 32, 80}, 
};
/* 544 bytes; */

static const struct {
	uint8_t off;
	uint8_t cnt;
} predict_idx[] = {
	{   0,  9},
	{   9,  7},
	{  16,  1},
	{  17,  8},
	{  25,  3},
	{  28, 10},
	{  38,  1},
	{  39, 11},
	{  50, 11},
	{  61,  1},
	{  62,  1},
	{  63,  1},
	{  64,  2},
	{  66,  3},
	{  69,  2},
	{  71,  1},
	{  72, 11},
	{  83, 10},
	{  93,  2},
	{  95,  1},
	{  96,  3},
	{  99, 10},
	{ 109,  6},
	{ 115, 10},
	{ 125,  8},
	{ 133, 10},
	{ 143, 14},
	{ 157, 10},
	{ 167, 16},
	{ 183, 10},
	{ 193, 18},
	{ 211, 10},
	{ 221, 21},
	{ 242,  7},
	{ 249,  1},
	{ 250, 22},
};
/* 72 bytes */

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
	/* 15:catch_opt(1) -> */
	A_OP_TRY_END, 
	/* 16:catch_opt(14) -> */
	T_CATCH, A_OP_CATCH, A_OP_BLK_OPEN, T_LPAREN, T_ID, A_OP_VAR_DECL, A_OP_PUSH_TMP, A_OP_ASSIGN, T_RPAREN, T_LBRACE, N_STAT_LIST, T_RBRACE, A_OP_BLK_CLOSE, A_OP_CATCH_END, 
	/* 17:condition(3) -> */
	T_LPAREN, N_EXP, T_RPAREN, 
	/* 18:var_list(2) -> */
	N_VAR, N_VAR_LIST1, 
	/* 19:var_list1(0) -> */
	
	/* 20:var_list1(2) -> */
	T_COMMA, N_VAR_LIST, 
	/* 21:var(4) -> */
	T_ID, A_OP_VAR_DECL, A_OP_PUSH_TMP, N_VAR_ASSIGN_OPT, 
	/* 22:var_assign_opt(1) -> */
	A_OP_POP_TMP, 
	/* 23:var_assign_opt(3) -> */
	T_EQUALS, N_EXP, A_OP_ASSIGN, 
	/* 24:assign_or_call(3) -> */
	T_ID, A_OP_PUSH_TMP, N_ASSIGN_OR_CALL1, 
	/* 25:assign_or_call1(3) -> */
	T_EQUALS, N_EXP, A_OP_ASSIGN, 
	/* 26:assign_or_call1(2) -> */
	N_FUNCTION_CALL, A_OP_RET_DISCARD, 
	/* 27:function_call(5) -> */
	A_OP_METHOD, T_LPAREN, N_ARG_LIST_OPT, T_RPAREN, A_OP_CALL, 
	/* 28:arg_list_opt(0) -> */
	
	/* 29:arg_list_opt(1) -> */
	N_ARG_LIST, 
	/* 30:arg_list(3) -> */
	N_EXP, A_OP_ARG, N_ARG_LIST1, 
	/* 31:arg_list1(0) -> */
	
	/* 32:arg_list1(2) -> */
	T_COMMA, N_ARG_LIST, 
	/* 33:exp_lst_opt(0) -> */
	
	/* 34:exp_lst_opt(1) -> */
	N_EXP_LST, 
	/* 35:exp_lst(2) -> */
	N_ASSIGN_OR_CALL, N_EXP_LST1, 
	/* 36:exp_lst1(0) -> */
	
	/* 37:exp_lst1(2) -> */
	T_COMMA, N_EXP_LST, 
	/* 38:exp(2) -> */
	N_AND_EXP, N_OR_EXP, 
	/* 39:or_exp(0) -> */
	
	/* 40:or_exp(3) -> */
	T_BAR, N_EXP, A_OP_OR, 
	/* 41:or_exp(3) -> */
	T_LOGICOR, N_EXP, A_OP_LOGIC_OR, 
	/* 42:or_exp(3) -> */
	T_CARET, N_EXP, A_OP_XOR, 
	/* 43:and_exp(2) -> */
	N_RELATIONAL_EXP, N_AND_EXP1, 
	/* 44:and_exp1(0) -> */
	
	/* 45:and_exp1(3) -> */
	T_AMPERSAND, N_AND_EXP, A_OP_AND, 
	/* 46:and_exp1(3) -> */
	T_LOGICAND, N_AND_EXP, A_OP_LOGIC_AND, 
	/* 47:relational_exp(2) -> */
	N_SHIFT_EXP, N_RELATIONAL_EXP1, 
	/* 48:relational_exp1(0) -> */
	
	/* 49:relational_exp1(3) -> */
	T_LESSTHEN, N_RELATIONAL_EXP, A_OP_LT, 
	/* 50:relational_exp1(3) -> */
	T_GREATTHEN, N_RELATIONAL_EXP, A_OP_GT, 
	/* 51:relational_exp1(3) -> */
	T_EQU, N_RELATIONAL_EXP, A_OP_EQU, 
	/* 52:relational_exp1(3) -> */
	T_NEQ, N_RELATIONAL_EXP, A_OP_NEQ, 
	/* 53:relational_exp1(3) -> */
	T_GTE, N_RELATIONAL_EXP, A_OP_GTE, 
	/* 54:relational_exp1(3) -> */
	T_LTE, N_RELATIONAL_EXP, A_OP_LTE, 
	/* 55:shift_exp(2) -> */
	N_ADDITIVE_EXP, N_SHIFT_EXP1, 
	/* 56:shift_exp1(0) -> */
	
	/* 57:shift_exp1(3) -> */
	T_SHL, N_SHIFT_EXP, A_OP_SHL, 
	/* 58:shift_exp1(3) -> */
	T_ASR, N_SHIFT_EXP, A_OP_ASR, 
	/* 59:additive_exp(2) -> */
	N_MULT_EXP, N_ADDITIVE_EXP1, 
	/* 60:additive_exp1(0) -> */
	
	/* 61:additive_exp1(3) -> */
	T_PLUS, N_ADDITIVE_EXP, A_OP_ADD, 
	/* 62:additive_exp1(3) -> */
	T_MINUS, N_ADDITIVE_EXP, A_OP_SUB, 
	/* 63:mult_exp(2) -> */
	N_UNARY_EXP, N_MULT_EXP1, 
	/* 64:mult_exp1(0) -> */
	
	/* 65:mult_exp1(3) -> */
	T_STAR, N_MULT_EXP, A_OP_MUL, 
	/* 66:mult_exp1(3) -> */
	T_SLASH, N_MULT_EXP, A_OP_DIV, 
	/* 67:mult_exp1(3) -> */
	T_PERCENT, N_MULT_EXP, A_OP_MOD, 
	/* 68:unary_exp(1) -> */
	N_PRIMARY_EXP, 
	/* 69:unary_exp(3) -> */
	T_TILDE, N_UNARY_EXP, A_OP_INV, 
	/* 70:unary_exp(3) -> */
	T_MINUS, N_UNARY_EXP, A_OP_MINUS, 
	/* 71:unary_exp(3) -> */
	T_NOT, N_UNARY_EXP, A_OP_NOT, 
	/* 72:primary_exp(3) -> */
	T_LPAREN, N_EXP, T_RPAREN, 
	/* 73:primary_exp(2) -> */
	T_INT, A_OP_PUSH_INT, 
	/* 74:primary_exp(2) -> */
	T_CHAR, A_OP_PUSH_INT, 
	/* 75:primary_exp(2) -> */
	T_STRING, A_OP_PUSH_STRING, 
	/* 76:primary_exp(2) -> */
	T_TRUE, A_OP_PUSH_TRUE, 
	/* 77:primary_exp(2) -> */
	T_FALSE, A_OP_PUSH_FALSE, 
	/* 78:primary_exp(1) -> */
	N_METH_OR_ATTR, 
	/* 79:meth_or_attr(3) -> */
	T_ID, A_OP_PUSH_TMP, N_METH_OR_ATTR1, 
	/* 80:meth_or_attr1(1) -> */
	A_OP_ATTR, 
	/* 81:meth_or_attr1(2) -> */
	N_FUNCTION_CALL, A_OP_CALL_RET, 
};
/* 203 bytes */

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
	{  53,  1},
	{  54, 14},
	{  68,  3},
	{  71,  2},
	{  73,  0},
	{  73,  2},
	{  75,  4},
	{  79,  1},
	{  80,  3},
	{  83,  3},
	{  86,  3},
	{  89,  2},
	{  91,  5},
	{  96,  0},
	{  96,  1},
	{  97,  3},
	{ 100,  0},
	{ 100,  2},
	{ 102,  0},
	{ 102,  1},
	{ 103,  2},
	{ 105,  0},
	{ 105,  2},
	{ 107,  2},
	{ 109,  0},
	{ 109,  3},
	{ 112,  3},
	{ 115,  3},
	{ 118,  2},
	{ 120,  0},
	{ 120,  3},
	{ 123,  3},
	{ 126,  2},
	{ 128,  0},
	{ 128,  3},
	{ 131,  3},
	{ 134,  3},
	{ 137,  3},
	{ 140,  3},
	{ 143,  3},
	{ 146,  2},
	{ 148,  0},
	{ 148,  3},
	{ 151,  3},
	{ 154,  2},
	{ 156,  0},
	{ 156,  3},
	{ 159,  3},
	{ 162,  2},
	{ 164,  0},
	{ 164,  3},
	{ 167,  3},
	{ 170,  3},
	{ 173,  1},
	{ 174,  3},
	{ 177,  3},
	{ 180,  3},
	{ 183,  3},
	{ 186,  2},
	{ 188,  2},
	{ 190,  2},
	{ 192,  2},
	{ 194,  2},
	{ 196,  1},
	{ 197,  3},
	{ 200,  1},
	{ 201,  2},
};
/* 164 bytes */

/* Total: 983 bytes */


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
 	"PLUS",
 	"MINUS",
 	"STAR",
 	"SLASH",
 	"PERCENT",
 	"AMPERSAND",
 	"BAR",
 	"CARET",
 	"TILDE",
 	"EXCLAM",
 	"QUEST",
 	"EQUALS",
 	"LESSTHEN",
 	"GREATTHEN",
 	"LBRACKET",
 	"RBRACKET",
 	"LPAREN",
 	"RPAREN",
 	"LBRACE",
 	"RBRACE",
 	"GTE",
 	"LTE",
 	"EQU",
 	"NEQ",
 	"ASR",
 	"SHL",
 	"LOGICOR",
 	"LOGICAND",
 	"BREAK",
 	"CATCH",
 	"CONTINUE",
 	"ELSE",
 	"FALSE",
 	"FINALLY",
 	"FOR",
 	"FUNCTION",
 	"IF",
 	"RETURN",
 	"THROW",
 	"TRUE",
 	"TRY",
 	"VAR",
 	"WHILE",
 	"ID",
 	"INT",
 	"CHAR",
 	"STRING",
 	"ERR",
 	"NOT",
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
 	"op_lt",
 	"op_gt",
 	"op_equ",
 	"op_neq",
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
 	"op_push_string",
 	"op_push_true",
 	"op_push_false",
 	"op_attr",
 	"op_call_ret",
 };

