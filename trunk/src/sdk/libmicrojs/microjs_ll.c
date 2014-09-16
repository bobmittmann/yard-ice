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
	{  0,  0}, {  3,  1}, { 35,  1}, { 36,  1}, { 37,  1}, { 39,  1}, 
	{ 40,  1}, { 41,  1}, { 42,  1}, { 43,  1}, { 44,  1}, 
	/*  49 (stat) */
	{  3,  5}, { 35,  6}, { 36, 14}, { 37,  8}, { 39,  9}, { 40,  7}, 
	{ 41, 11}, { 42, 12}, { 43, 13}, { 44, 10}, 
	/*  50 (compound_stat) */
	{  9,  2}, 
	/*  51 (stat_list) */
	{  3,  4}, { 10,  3}, { 35,  4}, { 36,  4}, { 37,  4}, { 39,  4}, 
	{ 40,  4}, { 41,  4}, { 42,  4}, { 43,  4}, { 44,  4}, 
	/*  52 (exp_lst_opt) */
	{  3, 32}, {  8, 32}, { 44, 33}, 
	/*  53 (exp) */
	{  7, 39}, { 20, 39}, { 29, 39}, { 30, 39}, { 34, 39}, { 38, 39}, 
	{ 44, 39}, { 45, 39}, { 46, 39}, 
	/*  54 (condition) */
	{  7, 17}, 
	/*  55 (else_opt) */
	{  0, 15}, {  3, 15}, { 10, 15}, { 33, 16}, { 35, 15}, { 36, 15}, 
	{ 37, 15}, { 39, 15}, { 40, 15}, { 41, 15}, { 42, 15}, { 43, 15}, 
	{ 44, 15}, 
	/*  56 (var_list) */
	{ 44, 18}, 
	/*  57 (assign_or_call) */
	{ 44, 24}, 
	/*  58 (exp_opt) */
	{  3, 37}, {  7, 38}, { 20, 38}, { 29, 38}, { 30, 38}, { 34, 38}, 
	{ 38, 38}, { 44, 38}, { 45, 38}, { 46, 38}, 
	/*  59 (param_list_opt) */
	{  8, 82}, { 44, 83}, 
	/*  60 (var) */
	{ 44, 21}, 
	/*  61 (var_list1) */
	{  2, 20}, {  3, 19}, 
	/*  62 (var_assign_opt) */
	{  2, 22}, {  3, 22}, { 32, 23}, 
	/*  63 (assign_or_call1) */
	{  7, 26}, { 32, 25}, 
	/*  64 (arg_list_opt) */
	{  7, 28}, {  8, 27}, { 20, 28}, { 29, 28}, { 30, 28}, { 34, 28}, 
	{ 38, 28}, { 44, 28}, { 45, 28}, { 46, 28}, 
	/*  65 (arg_list) */
	{  7, 29}, { 20, 29}, { 29, 29}, { 30, 29}, { 34, 29}, { 38, 29}, 
	{ 44, 29}, { 45, 29}, { 46, 29}, 
	/*  66 (arg_list1) */
	{  2, 31}, {  8, 30}, 
	/*  67 (exp_lst) */
	{ 44, 34}, 
	/*  68 (exp_lst1) */
	{  2, 36}, {  3, 35}, {  8, 35}, 
	/*  69 (and_exp) */
	{  7, 44}, { 20, 44}, { 29, 44}, { 30, 44}, { 34, 44}, { 38, 44}, 
	{ 44, 44}, { 45, 44}, { 46, 44}, 
	/*  70 (or_exp) */
	{  2, 40}, {  3, 40}, {  8, 40}, { 24, 41}, { 25, 42}, { 28, 43}, 
	/*  71 (relational_exp) */
	{  7, 48}, { 20, 48}, { 29, 48}, { 30, 48}, { 34, 48}, { 38, 48}, 
	{ 44, 48}, { 45, 48}, { 46, 48}, 
	/*  72 (and_exp1) */
	{  2, 45}, {  3, 45}, {  8, 45}, { 24, 45}, { 25, 45}, { 26, 46}, 
	{ 27, 47}, { 28, 45}, 
	/*  73 (shift_exp) */
	{  7, 56}, { 20, 56}, { 29, 56}, { 30, 56}, { 34, 56}, { 38, 56}, 
	{ 44, 56}, { 45, 56}, { 46, 56}, 
	/*  74 (relational_exp1) */
	{  2, 49}, {  3, 49}, {  8, 49}, { 13, 55}, { 14, 52}, { 15, 54}, 
	{ 16, 53}, { 17, 50}, { 18, 51}, { 24, 49}, { 25, 49}, { 26, 49}, 
	{ 27, 49}, { 28, 49}, 
	/*  75 (additive_exp) */
	{  7, 60}, { 20, 60}, { 29, 60}, { 30, 60}, { 34, 60}, { 38, 60}, 
	{ 44, 60}, { 45, 60}, { 46, 60}, 
	/*  76 (shift_exp1) */
	{  2, 57}, {  3, 57}, {  8, 57}, { 11, 59}, { 12, 58}, { 13, 57}, 
	{ 14, 57}, { 15, 57}, { 16, 57}, { 17, 57}, { 18, 57}, { 24, 57}, 
	{ 25, 57}, { 26, 57}, { 27, 57}, { 28, 57}, 
	/*  77 (mult_exp) */
	{  7, 64}, { 20, 64}, { 29, 64}, { 30, 64}, { 34, 64}, { 38, 64}, 
	{ 44, 64}, { 45, 64}, { 46, 64}, 
	/*  78 (additive_exp1) */
	{  2, 61}, {  3, 61}, {  8, 61}, { 11, 61}, { 12, 61}, { 13, 61}, 
	{ 14, 61}, { 15, 61}, { 16, 61}, { 17, 61}, { 18, 61}, { 19, 62}, 
	{ 20, 63}, { 24, 61}, { 25, 61}, { 26, 61}, { 27, 61}, { 28, 61}, 
	/*  79 (unary_exp) */
	{  7, 69}, { 20, 71}, { 29, 72}, { 30, 70}, { 34, 69}, { 38, 69}, 
	{ 44, 69}, { 45, 69}, { 46, 69}, 
	/*  80 (mult_exp1) */
	{  2, 65}, {  3, 65}, {  8, 65}, { 11, 65}, { 12, 65}, { 13, 65}, 
	{ 14, 65}, { 15, 65}, { 16, 65}, { 17, 65}, { 18, 65}, { 19, 65}, 
	{ 20, 65}, { 21, 66}, { 22, 67}, { 23, 68}, { 24, 65}, { 25, 65}, 
	{ 26, 65}, { 27, 65}, { 28, 65}, 
	/*  81 (primary_exp) */
	{  7, 73}, { 34, 76}, { 38, 75}, { 44, 78}, { 45, 74}, { 46, 77}, 
	/*  82 (meth_or_attr) */
	{ 44, 79}, 
	/*  83 (meth_or_attr1) */
	{  2, 80}, {  3, 80}, {  7, 81}, {  8, 80}, { 11, 80}, { 12, 80}, 
	{ 13, 80}, { 14, 80}, { 15, 80}, { 16, 80}, { 17, 80}, { 18, 80}, 
	{ 19, 80}, { 20, 80}, { 21, 80}, { 22, 80}, { 23, 80}, { 24, 80}, 
	{ 25, 80}, { 26, 80}, { 27, 80}, { 28, 80}, 
	/*  84 (param_list) */
	{ 44, 84}, 
	/*  85 (param_list1) */
	{  2, 86}, {  8, 85}, 
};
/* 550 bytes; */

static const struct {
	uint16_t off;
	uint8_t cnt;
} predict_idx[] = {
	{   0, 11},
	{  11, 10},
	{  21,  1},
	{  22, 11},
	{  33,  3},
	{  36,  9},
	{  45,  1},
	{  46, 13},
	{  59,  1},
	{  60,  1},
	{  61, 10},
	{  71,  2},
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
	{ 272,  1},
	{ 273,  2},
};
/* 152 bytes */

/* Rules vectors table */
static const uint8_t rule_vec[] = {
	/*  0:program(1) -> */
	T_EOF, 
	/*  1:program(2) -> */
	N_STAT, N_PROGRAM, 
	/*  2:compound_stat(5) -> */
	T_LBRACE, A_OP_BLK_OPEN, N_STAT_LIST, T_RBRACE, A_OP_BLK_CLOSE, 
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
	/* 11:stat(2) -> */
	T_CONTINUE, T_SEMICOLON, 
	/* 12:stat(2) -> */
	T_BREAK, T_SEMICOLON, 
	/* 13:stat(3) -> */
	T_RETURN, N_EXP_OPT, T_SEMICOLON, 
	/* 14:stat(6) -> */
	T_FUNCTION, T_ID, T_LPAREN, N_PARAM_LIST_OPT, T_RPAREN, N_COMPOUND_STAT, 
	/* 15:else_opt(0) -> */
	
	/* 16:else_opt(3) -> */
	T_ELSE, A_OP_IF_ELSE, N_COMPOUND_STAT, 
	/* 17:condition(3) -> */
	T_LPAREN, N_EXP, T_RPAREN, 
	/* 18:var_list(2) -> */
	N_VAR, N_VAR_LIST1, 
	/* 19:var_list1(0) -> */
	
	/* 20:var_list1(2) -> */
	T_COMMA, N_VAR_LIST, 
	/* 21:var(5) -> */
	T_ID, A_OP_VAR_DECL, A_OP_PUSH_TMP, N_VAR_ASSIGN_OPT, A_OP_POP_TMP, 
	/* 22:var_assign_opt(0) -> */
	
	/* 23:var_assign_opt(3) -> */
	T_ASSIGN, N_EXP, A_OP_ASSIGN, 
	/* 24:assign_or_call(4) -> */
	T_ID, A_OP_PUSH_TMP, N_ASSIGN_OR_CALL1, A_OP_POP_TMP, 
	/* 25:assign_or_call1(3) -> */
	T_ASSIGN, N_EXP, A_OP_ASSIGN, 
	/* 26:assign_or_call1(6) -> */
	T_LPAREN, A_OP_METHOD, N_ARG_LIST_OPT, T_RPAREN, A_OP_METH_OR_ATTR, A_OP_RET_DISCARD, 
	/* 27:arg_list_opt(0) -> */
	
	/* 28:arg_list_opt(1) -> */
	N_ARG_LIST, 
	/* 29:arg_list(3) -> */
	N_EXP, A_OP_ARG, N_ARG_LIST1, 
	/* 30:arg_list1(0) -> */
	
	/* 31:arg_list1(2) -> */
	T_COMMA, N_ARG_LIST, 
	/* 32:exp_lst_opt(0) -> */
	
	/* 33:exp_lst_opt(1) -> */
	N_EXP_LST, 
	/* 34:exp_lst(2) -> */
	N_ASSIGN_OR_CALL, N_EXP_LST1, 
	/* 35:exp_lst1(0) -> */
	
	/* 36:exp_lst1(2) -> */
	T_COMMA, N_EXP_LST, 
	/* 37:exp_opt(0) -> */
	
	/* 38:exp_opt(1) -> */
	N_EXP, 
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
	/* 82:param_list_opt(0) -> */
	
	/* 83:param_list_opt(1) -> */
	N_PARAM_LIST, 
	/* 84:param_list(2) -> */
	T_ID, N_PARAM_LIST1, 
	/* 85:param_list1(0) -> */
	
	/* 86:param_list1(2) -> */
	T_COMMA, N_PARAM_LIST, 
};
/* 199 bytes */

static const struct {
	uint8_t off;
	uint8_t cnt;
} rule_idx[] = {
	{   0,  1},
	{   1,  2},
	{   3,  5},
	{   8,  0},
	{   8,  2},
	{  10,  1},
	{  11, 13},
	{  24,  6},
	{  30,  6},
	{  36,  3},
	{  39,  2},
	{  41,  2},
	{  43,  2},
	{  45,  3},
	{  48,  6},
	{  54,  0},
	{  54,  3},
	{  57,  3},
	{  60,  2},
	{  62,  0},
	{  62,  2},
	{  64,  5},
	{  69,  0},
	{  69,  3},
	{  72,  4},
	{  76,  3},
	{  79,  6},
	{  85,  0},
	{  85,  1},
	{  86,  3},
	{  89,  0},
	{  89,  2},
	{  91,  0},
	{  91,  1},
	{  92,  2},
	{  94,  0},
	{  94,  2},
	{  96,  0},
	{  96,  1},
	{  97,  2},
	{  99,  0},
	{  99,  3},
	{ 102,  3},
	{ 105,  3},
	{ 108,  2},
	{ 110,  0},
	{ 110,  3},
	{ 113,  3},
	{ 116,  2},
	{ 118,  0},
	{ 118,  3},
	{ 121,  3},
	{ 124,  3},
	{ 127,  3},
	{ 130,  3},
	{ 133,  3},
	{ 136,  2},
	{ 138,  0},
	{ 138,  3},
	{ 141,  3},
	{ 144,  2},
	{ 146,  0},
	{ 146,  3},
	{ 149,  3},
	{ 152,  2},
	{ 154,  0},
	{ 154,  3},
	{ 157,  3},
	{ 160,  3},
	{ 163,  1},
	{ 164,  3},
	{ 167,  3},
	{ 170,  3},
	{ 173,  3},
	{ 176,  2},
	{ 178,  2},
	{ 180,  2},
	{ 182,  2},
	{ 184,  3},
	{ 187,  3},
	{ 190,  0},
	{ 190,  4},
	{ 194,  0},
	{ 194,  1},
	{ 195,  2},
	{ 197,  0},
	{ 197,  2},
};
/* 174 bytes */

/* Total: 1075 bytes */


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

