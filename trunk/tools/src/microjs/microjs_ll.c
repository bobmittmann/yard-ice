/* LL(1) Embeddedd Nonrecursive Predictive Parser */

#include "microjs_ll.h"

/* Token-Rule pair (predict set) */
struct tr_pair {
	uint8_t t;
	uint8_t r;
};

/* Predict sets */
static const struct tr_pair predict_vec[] = {
	/*  49 (program) */
	{  0,  0}, {  3,  1}, { 35,  1}, { 36,  1}, { 37,  1}, { 39,  1}, 
	{ 40,  1}, { 41,  1}, { 42,  1}, { 43,  1}, { 44,  1}, 
	/*  50 (stat) */
	{  3,  5}, { 35,  6}, { 36, 14}, { 37,  8}, { 39,  9}, { 40,  7}, 
	{ 41, 11}, { 42, 12}, { 43, 13}, { 44, 10}, 
	/*  51 (compound_stat) */
	{  9,  2}, 
	/*  52 (stat_list) */
	{  3,  4}, { 10,  3}, { 35,  4}, { 36,  4}, { 37,  4}, { 39,  4}, 
	{ 40,  4}, { 41,  4}, { 42,  4}, { 43,  4}, { 44,  4}, 
	/*  53 (exp_lst_opt) */
	{  3, 34}, {  8, 34}, { 44, 35}, 
	/*  54 (exp) */
	{  7, 41}, { 20, 41}, { 29, 41}, { 30, 41}, { 34, 41}, { 38, 41}, 
	{ 44, 41}, { 45, 41}, { 46, 41}, 
	/*  55 (condition) */
	{  7, 17}, 
	/*  56 (else_opt) */
	{  0, 15}, {  3, 15}, { 10, 15}, { 33, 16}, { 35, 15}, { 36, 15}, 
	{ 37, 15}, { 39, 15}, { 40, 15}, { 41, 15}, { 42, 15}, { 43, 15}, 
	{ 44, 15}, 
	/*  57 (var_list) */
	{ 44, 18}, 
	/*  58 (assign_or_call) */
	{ 44, 24}, 
	/*  59 (exp_opt) */
	{  3, 39}, {  7, 40}, { 20, 40}, { 29, 40}, { 30, 40}, { 34, 40}, 
	{ 38, 40}, { 44, 40}, { 45, 40}, { 46, 40}, 
	/*  60 (param_list_opt) */
	{  8, 84}, { 44, 85}, 
	/*  61 (var) */
	{ 44, 21}, 
	/*  62 (var_list1) */
	{  2, 20}, {  3, 19}, 
	/*  63 (var_assign_opt) */
	{  2, 22}, {  3, 22}, { 32, 23}, 
	/*  64 (assign_or_call1) */
	{  7, 26}, { 32, 25}, { 48, 27}, 
	/*  65 (arg_list_opt) */
	{  7, 29}, {  8, 28}, { 20, 29}, { 29, 29}, { 30, 29}, { 34, 29}, 
	{ 38, 29}, { 44, 29}, { 45, 29}, { 46, 29}, 
	/*  66 (cmd_arg_opt) */
	{  7, 30}, { 34, 30}, { 38, 30}, { 44, 30}, { 45, 30}, { 46, 30}, 
	/*  67 (arg_list) */
	{  7, 31}, { 20, 31}, { 29, 31}, { 30, 31}, { 34, 31}, { 38, 31}, 
	{ 44, 31}, { 45, 31}, { 46, 31}, 
	/*  68 (primary_exp) */
	{  7, 75}, { 34, 78}, { 38, 77}, { 44, 80}, { 45, 76}, { 46, 79}, 
	/*  69 (arg_list1) */
	{  2, 33}, {  8, 32}, 
	/*  70 (exp_lst) */
	{ 44, 36}, 
	/*  71 (exp_lst1) */
	{  2, 38}, {  3, 37}, {  8, 37}, 
	/*  72 (and_exp) */
	{  7, 46}, { 20, 46}, { 29, 46}, { 30, 46}, { 34, 46}, { 38, 46}, 
	{ 44, 46}, { 45, 46}, { 46, 46}, 
	/*  73 (or_exp) */
	{  2, 42}, {  3, 42}, {  8, 42}, { 24, 43}, { 25, 44}, { 28, 45}, 
	/*  74 (relational_exp) */
	{  7, 50}, { 20, 50}, { 29, 50}, { 30, 50}, { 34, 50}, { 38, 50}, 
	{ 44, 50}, { 45, 50}, { 46, 50}, 
	/*  75 (and_exp1) */
	{  2, 47}, {  3, 47}, {  8, 47}, { 24, 47}, { 25, 47}, { 26, 48}, 
	{ 27, 49}, { 28, 47}, 
	/*  76 (shift_exp) */
	{  7, 58}, { 20, 58}, { 29, 58}, { 30, 58}, { 34, 58}, { 38, 58}, 
	{ 44, 58}, { 45, 58}, { 46, 58}, 
	/*  77 (relational_exp1) */
	{  2, 51}, {  3, 51}, {  8, 51}, { 13, 57}, { 14, 54}, { 15, 56}, 
	{ 16, 55}, { 17, 52}, { 18, 53}, { 24, 51}, { 25, 51}, { 26, 51}, 
	{ 27, 51}, { 28, 51}, 
	/*  78 (additive_exp) */
	{  7, 62}, { 20, 62}, { 29, 62}, { 30, 62}, { 34, 62}, { 38, 62}, 
	{ 44, 62}, { 45, 62}, { 46, 62}, 
	/*  79 (shift_exp1) */
	{  2, 59}, {  3, 59}, {  8, 59}, { 11, 61}, { 12, 60}, { 13, 59}, 
	{ 14, 59}, { 15, 59}, { 16, 59}, { 17, 59}, { 18, 59}, { 24, 59}, 
	{ 25, 59}, { 26, 59}, { 27, 59}, { 28, 59}, 
	/*  80 (mult_exp) */
	{  7, 66}, { 20, 66}, { 29, 66}, { 30, 66}, { 34, 66}, { 38, 66}, 
	{ 44, 66}, { 45, 66}, { 46, 66}, 
	/*  81 (additive_exp1) */
	{  2, 63}, {  3, 63}, {  8, 63}, { 11, 63}, { 12, 63}, { 13, 63}, 
	{ 14, 63}, { 15, 63}, { 16, 63}, { 17, 63}, { 18, 63}, { 19, 64}, 
	{ 20, 65}, { 24, 63}, { 25, 63}, { 26, 63}, { 27, 63}, { 28, 63}, 
	/*  82 (unary_exp) */
	{  7, 71}, { 20, 73}, { 29, 74}, { 30, 72}, { 34, 71}, { 38, 71}, 
	{ 44, 71}, { 45, 71}, { 46, 71}, 
	/*  83 (mult_exp1) */
	{  2, 67}, {  3, 67}, {  8, 67}, { 11, 67}, { 12, 67}, { 13, 67}, 
	{ 14, 67}, { 15, 67}, { 16, 67}, { 17, 67}, { 18, 67}, { 19, 67}, 
	{ 20, 67}, { 21, 68}, { 22, 69}, { 23, 70}, { 24, 67}, { 25, 67}, 
	{ 26, 67}, { 27, 67}, { 28, 67}, 
	/*  84 (meth_or_attr) */
	{ 44, 81}, 
	/*  85 (meth_or_attr1) */
	{  2, 82}, {  3, 82}, {  7, 82}, {  7, 83}, {  8, 82}, { 11, 82}, 
	{ 12, 82}, { 13, 82}, { 14, 82}, { 15, 82}, { 16, 82}, { 17, 82}, 
	{ 18, 82}, { 19, 82}, { 20, 82}, { 21, 82}, { 22, 82}, { 23, 82}, 
	{ 24, 82}, { 25, 82}, { 26, 82}, { 27, 82}, { 28, 82}, { 34, 82}, 
	{ 38, 82}, { 44, 82}, { 45, 82}, { 46, 82}, 
	/*  86 (param_list) */
	{ 44, 86}, 
	/*  87 (param_list1) */
	{  2, 88}, {  8, 87}, 
};
/* 576 bytes; */

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
	{  79,  3},
	{  82, 10},
	{  92,  6},
	{  98,  9},
	{ 107,  6},
	{ 113,  2},
	{ 115,  1},
	{ 116,  3},
	{ 119,  9},
	{ 128,  6},
	{ 134,  9},
	{ 143,  8},
	{ 151,  9},
	{ 160, 14},
	{ 174,  9},
	{ 183, 16},
	{ 199,  9},
	{ 208, 18},
	{ 226,  9},
	{ 235, 21},
	{ 256,  1},
	{ 257, 28},
	{ 285,  1},
	{ 286,  2},
};
/* 156 bytes */

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
	/* 27:assign_or_call1(2) -> */
	T_CMD, N_CMD_ARG_OPT, 
	/* 28:arg_list_opt(0) -> */
	
	/* 29:arg_list_opt(1) -> */
	N_ARG_LIST, 
	/* 30:cmd_arg_opt(2) -> */
	N_PRIMARY_EXP, N_PRIMARY_EXP, 
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
	/* 39:exp_opt(0) -> */
	
	/* 40:exp_opt(1) -> */
	N_EXP, 
	/* 41:exp(2) -> */
	N_AND_EXP, N_OR_EXP, 
	/* 42:or_exp(0) -> */
	
	/* 43:or_exp(3) -> */
	T_OR, N_EXP, A_OP_OR, 
	/* 44:or_exp(3) -> */
	T_LOR, N_EXP, A_OP_LOGIC_OR, 
	/* 45:or_exp(3) -> */
	T_XOR, N_EXP, A_OP_XOR, 
	/* 46:and_exp(2) -> */
	N_RELATIONAL_EXP, N_AND_EXP1, 
	/* 47:and_exp1(0) -> */
	
	/* 48:and_exp1(3) -> */
	T_AND, N_AND_EXP, A_OP_AND, 
	/* 49:and_exp1(3) -> */
	T_LAND, N_AND_EXP, A_OP_LOGIC_AND, 
	/* 50:relational_exp(2) -> */
	N_SHIFT_EXP, N_RELATIONAL_EXP1, 
	/* 51:relational_exp1(0) -> */
	
	/* 52:relational_exp1(3) -> */
	T_EQU, N_RELATIONAL_EXP, A_OP_EQU, 
	/* 53:relational_exp1(3) -> */
	T_NEQ, N_RELATIONAL_EXP, A_OP_NEQ, 
	/* 54:relational_exp1(3) -> */
	T_LT, N_RELATIONAL_EXP, A_OP_LT, 
	/* 55:relational_exp1(3) -> */
	T_GT, N_RELATIONAL_EXP, A_OP_GT, 
	/* 56:relational_exp1(3) -> */
	T_GTE, N_RELATIONAL_EXP, A_OP_GTE, 
	/* 57:relational_exp1(3) -> */
	T_LTE, N_RELATIONAL_EXP, A_OP_LTE, 
	/* 58:shift_exp(2) -> */
	N_ADDITIVE_EXP, N_SHIFT_EXP1, 
	/* 59:shift_exp1(0) -> */
	
	/* 60:shift_exp1(3) -> */
	T_SHL, N_SHIFT_EXP, A_OP_SHL, 
	/* 61:shift_exp1(3) -> */
	T_ASR, N_SHIFT_EXP, A_OP_ASR, 
	/* 62:additive_exp(2) -> */
	N_MULT_EXP, N_ADDITIVE_EXP1, 
	/* 63:additive_exp1(0) -> */
	
	/* 64:additive_exp1(3) -> */
	T_PLUS, N_ADDITIVE_EXP, A_OP_ADD, 
	/* 65:additive_exp1(3) -> */
	T_MINUS, N_ADDITIVE_EXP, A_OP_SUB, 
	/* 66:mult_exp(2) -> */
	N_UNARY_EXP, N_MULT_EXP1, 
	/* 67:mult_exp1(0) -> */
	
	/* 68:mult_exp1(3) -> */
	T_MUL, N_MULT_EXP, A_OP_MUL, 
	/* 69:mult_exp1(3) -> */
	T_DIV, N_MULT_EXP, A_OP_DIV, 
	/* 70:mult_exp1(3) -> */
	T_MOD, N_MULT_EXP, A_OP_MOD, 
	/* 71:unary_exp(1) -> */
	N_PRIMARY_EXP, 
	/* 72:unary_exp(3) -> */
	T_INV, N_UNARY_EXP, A_OP_INV, 
	/* 73:unary_exp(3) -> */
	T_MINUS, N_UNARY_EXP, A_OP_MINUS, 
	/* 74:unary_exp(3) -> */
	T_NOT, N_UNARY_EXP, A_OP_NOT, 
	/* 75:primary_exp(3) -> */
	T_LPAREN, N_EXP, T_RPAREN, 
	/* 76:primary_exp(2) -> */
	T_INT, A_OP_PUSH_INT, 
	/* 77:primary_exp(2) -> */
	T_TRUE, A_OP_PUSH_TRUE, 
	/* 78:primary_exp(2) -> */
	T_FALSE, A_OP_PUSH_FALSE, 
	/* 79:primary_exp(2) -> */
	T_STRING, A_OP_PUSH_STRING, 
	/* 80:primary_exp(3) -> */
	N_METH_OR_ATTR, A_OP_METH_OR_ATTR, A_OP_POP_TMP, 
	/* 81:meth_or_attr(3) -> */
	T_ID, A_OP_PUSH_TMP, N_METH_OR_ATTR1, 
	/* 82:meth_or_attr1(0) -> */
	
	/* 83:meth_or_attr1(4) -> */
	T_LPAREN, A_OP_METHOD, N_ARG_LIST_OPT, T_RPAREN, 
	/* 84:param_list_opt(0) -> */
	
	/* 85:param_list_opt(1) -> */
	N_PARAM_LIST, 
	/* 86:param_list(2) -> */
	T_ID, N_PARAM_LIST1, 
	/* 87:param_list1(0) -> */
	
	/* 88:param_list1(2) -> */
	T_COMMA, N_PARAM_LIST, 
};
/* 203 bytes */

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
	{  85,  2},
	{  87,  0},
	{  87,  1},
	{  88,  2},
	{  90,  3},
	{  93,  0},
	{  93,  2},
	{  95,  0},
	{  95,  1},
	{  96,  2},
	{  98,  0},
	{  98,  2},
	{ 100,  0},
	{ 100,  1},
	{ 101,  2},
	{ 103,  0},
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
	{ 137,  3},
	{ 140,  2},
	{ 142,  0},
	{ 142,  3},
	{ 145,  3},
	{ 148,  2},
	{ 150,  0},
	{ 150,  3},
	{ 153,  3},
	{ 156,  2},
	{ 158,  0},
	{ 158,  3},
	{ 161,  3},
	{ 164,  3},
	{ 167,  1},
	{ 168,  3},
	{ 171,  3},
	{ 174,  3},
	{ 177,  3},
	{ 180,  2},
	{ 182,  2},
	{ 184,  2},
	{ 186,  2},
	{ 188,  3},
	{ 191,  3},
	{ 194,  0},
	{ 194,  4},
	{ 198,  0},
	{ 198,  1},
	{ 199,  2},
	{ 201,  0},
	{ 201,  2},
};
/* 178 bytes */

/* Total: 1113 bytes */


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
 	"CMD",
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
 	"exp_opt",
 	"param_list_opt",
 	"var",
 	"var_list1",
 	"var_assign_opt",
 	"assign_or_call1",
 	"arg_list_opt",
 	"cmd_arg_opt",
 	"arg_list",
 	"primary_exp",
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
 	"meth_or_attr",
 	"meth_or_attr1",
 	"param_list",
 	"param_list1",
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

