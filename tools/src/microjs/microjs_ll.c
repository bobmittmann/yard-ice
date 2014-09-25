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
	{  3, 37}, { 22, 37}, { 48, 38}, 
	/*  59 (exp) */
	{  6, 42}, { 13, 42}, { 21, 42}, { 37, 42}, { 44, 42}, { 48, 42}, 
	{ 49, 42}, { 50, 42}, { 51, 42}, { 53, 42}, 
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
	{  1, 28}, { 16, 25}, { 19, 27}, { 21, 26}, 
	/*  69 (function_call) */
	{ 21, 31}, 
	/*  70 (array_assign) */
	{  1, 30}, { 16, 29}, 
	/*  71 (arg_list_opt) */
	{  6, 33}, { 13, 33}, { 21, 33}, { 22, 32}, { 37, 33}, { 44, 33}, 
	{ 48, 33}, { 49, 33}, { 50, 33}, { 51, 33}, { 53, 33}, 
	/*  72 (arg_list) */
	{  6, 34}, { 13, 34}, { 21, 34}, { 37, 34}, { 44, 34}, { 48, 34}, 
	{ 49, 34}, { 50, 34}, { 51, 34}, { 53, 34}, 
	/*  73 (arg_list1) */
	{  2, 36}, { 22, 35}, 
	/*  74 (exp_lst) */
	{ 48, 39}, 
	/*  75 (exp_lst1) */
	{  2, 41}, {  3, 40}, { 22, 40}, 
	/*  76 (and_exp) */
	{  6, 47}, { 13, 47}, { 21, 47}, { 37, 47}, { 44, 47}, { 48, 47}, 
	{ 49, 47}, { 50, 47}, { 51, 47}, { 53, 47}, 
	/*  77 (or_exp) */
	{  2, 43}, {  3, 43}, { 11, 44}, { 12, 46}, { 20, 43}, { 22, 43}, 
	{ 31, 45}, 
	/*  78 (relational_exp) */
	{  6, 51}, { 13, 51}, { 21, 51}, { 37, 51}, { 44, 51}, { 48, 51}, 
	{ 49, 51}, { 50, 51}, { 51, 51}, { 53, 51}, 
	/*  79 (and_exp1) */
	{  2, 48}, {  3, 48}, { 10, 49}, { 11, 48}, { 12, 48}, { 20, 48}, 
	{ 22, 48}, { 31, 48}, { 32, 50}, 
	/*  80 (shift_exp) */
	{  6, 59}, { 13, 59}, { 21, 59}, { 37, 59}, { 44, 59}, { 48, 59}, 
	{ 49, 59}, { 50, 59}, { 51, 59}, { 53, 59}, 
	/*  81 (relational_exp1) */
	{  2, 52}, {  3, 52}, { 10, 52}, { 11, 52}, { 12, 52}, { 17, 53}, 
	{ 18, 54}, { 20, 52}, { 22, 52}, { 25, 57}, { 26, 58}, { 27, 55}, 
	{ 28, 56}, { 31, 52}, { 32, 52}, 
	/*  82 (additive_exp) */
	{  6, 63}, { 13, 63}, { 21, 63}, { 37, 63}, { 44, 63}, { 48, 63}, 
	{ 49, 63}, { 50, 63}, { 51, 63}, { 53, 63}, 
	/*  83 (shift_exp1) */
	{  2, 60}, {  3, 60}, { 10, 60}, { 11, 60}, { 12, 60}, { 17, 60}, 
	{ 18, 60}, { 20, 60}, { 22, 60}, { 25, 60}, { 26, 60}, { 27, 60}, 
	{ 28, 60}, { 29, 62}, { 30, 61}, { 31, 60}, { 32, 60}, 
	/*  84 (div_exp) */
	{  6, 67}, { 13, 67}, { 21, 67}, { 37, 67}, { 44, 67}, { 48, 67}, 
	{ 49, 67}, { 50, 67}, { 51, 67}, { 53, 67}, 
	/*  85 (additive_exp1) */
	{  2, 64}, {  3, 64}, {  5, 65}, {  6, 66}, { 10, 64}, { 11, 64}, 
	{ 12, 64}, { 17, 64}, { 18, 64}, { 20, 64}, { 22, 64}, { 25, 64}, 
	{ 26, 64}, { 27, 64}, { 28, 64}, { 29, 64}, { 30, 64}, { 31, 64}, 
	{ 32, 64}, 
	/*  86 (mult_exp) */
	{  6, 71}, { 13, 71}, { 21, 71}, { 37, 71}, { 44, 71}, { 48, 71}, 
	{ 49, 71}, { 50, 71}, { 51, 71}, { 53, 71}, 
	/*  87 (div_exp1) */
	{  2, 68}, {  3, 68}, {  5, 68}, {  6, 68}, {  8, 69}, {  9, 70}, 
	{ 10, 68}, { 11, 68}, { 12, 68}, { 17, 68}, { 18, 68}, { 20, 68}, 
	{ 22, 68}, { 25, 68}, { 26, 68}, { 27, 68}, { 28, 68}, { 29, 68}, 
	{ 30, 68}, { 31, 68}, { 32, 68}, 
	/*  88 (unary_exp) */
	{  6, 76}, { 13, 75}, { 21, 74}, { 37, 74}, { 44, 74}, { 48, 74}, 
	{ 49, 74}, { 50, 74}, { 51, 74}, { 53, 77}, 
	/*  89 (mult_exp1) */
	{  2, 72}, {  3, 72}, {  5, 72}, {  6, 72}, {  7, 73}, {  8, 72}, 
	{  9, 72}, { 10, 72}, { 11, 72}, { 12, 72}, { 17, 72}, { 18, 72}, 
	{ 20, 72}, { 22, 72}, { 25, 72}, { 26, 72}, { 27, 72}, { 28, 72}, 
	{ 29, 72}, { 30, 72}, { 31, 72}, { 32, 72}, 
	/*  90 (primary_exp) */
	{ 21, 78}, { 37, 83}, { 44, 82}, { 48, 84}, { 49, 79}, { 50, 80}, 
	{ 51, 81}, 
	/*  91 (id_eval) */
	{ 48, 85}, 
	/*  92 (id_eval1) */
	{  1, 89}, {  2, 86}, {  3, 86}, {  5, 86}, {  6, 86}, {  7, 86}, 
	{  8, 86}, {  9, 86}, { 10, 86}, { 11, 86}, { 12, 86}, { 17, 86}, 
	{ 18, 86}, { 19, 88}, { 20, 86}, { 21, 87}, { 22, 86}, { 25, 86}, 
	{ 26, 86}, { 27, 86}, { 28, 86}, { 29, 86}, { 30, 86}, { 31, 86}, 
	{ 32, 86}, 
	/*  93 (array_val) */
	{  1, 91}, {  2, 90}, {  3, 90}, {  5, 90}, {  6, 90}, {  7, 90}, 
	{  8, 90}, {  9, 90}, { 10, 90}, { 11, 90}, { 12, 90}, { 17, 90}, 
	{ 18, 90}, { 20, 90}, { 22, 90}, { 25, 90}, { 26, 90}, { 27, 90}, 
	{ 28, 90}, { 29, 90}, { 30, 90}, { 31, 90}, { 32, 90}, 
};
/* 678 bytes; */

static const struct {
	uint16_t off;
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
	{  69,  4},
	{  73,  1},
	{  74,  2},
	{  76, 11},
	{  87, 10},
	{  97,  2},
	{  99,  1},
	{ 100,  3},
	{ 103, 10},
	{ 113,  7},
	{ 120, 10},
	{ 130,  9},
	{ 139, 10},
	{ 149, 15},
	{ 164, 10},
	{ 174, 17},
	{ 191, 10},
	{ 201, 19},
	{ 220, 10},
	{ 230, 21},
	{ 251, 10},
	{ 261, 22},
	{ 283,  7},
	{ 290,  1},
	{ 291, 25},
	{ 316, 23},
};
/* 160 bytes */

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
	T_CATCH, A_OP_CATCH, A_OP_BLK_OPEN, T_LPAREN, T_ID, A_OP_VAR_DECL, A_OP_PUSH_TMP, A_OP_VAR_ASSIGN, T_RPAREN, T_LBRACE, N_STAT_LIST, T_RBRACE, A_OP_BLK_CLOSE, A_OP_CATCH_END, 
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
	T_EQUALS, N_EXP, A_OP_VAR_ASSIGN, 
	/* 24:assign_or_call(3) -> */
	T_ID, A_OP_PUSH_TMP, N_ASSIGN_OR_CALL1, 
	/* 25:assign_or_call1(3) -> */
	T_EQUALS, N_EXP, A_OP_VAR_ASSIGN, 
	/* 26:assign_or_call1(2) -> */
	N_FUNCTION_CALL, A_OP_RET_DISCARD, 
	/* 27:assign_or_call1(4) -> */
	T_LBRACKET, N_EXP, T_RBRACKET, N_ARRAY_ASSIGN, 
	/* 28:assign_or_call1(5) -> */
	T_DOT, T_ID, T_EQUALS, N_EXP, A_OP_ATTR_ASSIGN, 
	/* 29:array_assign(3) -> */
	T_EQUALS, N_EXP, A_OP_ARRAY_ASSIGN, 
	/* 30:array_assign(5) -> */
	T_DOT, T_ID, T_EQUALS, N_EXP, A_OP_ARRAY_ATTR_ASSIGN, 
	/* 31:function_call(5) -> */
	A_OP_FUNCTION_LOOKUP, T_LPAREN, N_ARG_LIST_OPT, T_RPAREN, A_OP_CALL, 
	/* 32:arg_list_opt(0) -> */
	
	/* 33:arg_list_opt(1) -> */
	N_ARG_LIST, 
	/* 34:arg_list(3) -> */
	N_EXP, A_OP_ARG, N_ARG_LIST1, 
	/* 35:arg_list1(0) -> */
	
	/* 36:arg_list1(2) -> */
	T_COMMA, N_ARG_LIST, 
	/* 37:exp_lst_opt(0) -> */
	
	/* 38:exp_lst_opt(1) -> */
	N_EXP_LST, 
	/* 39:exp_lst(2) -> */
	N_ASSIGN_OR_CALL, N_EXP_LST1, 
	/* 40:exp_lst1(0) -> */
	
	/* 41:exp_lst1(2) -> */
	T_COMMA, N_EXP_LST, 
	/* 42:exp(2) -> */
	N_AND_EXP, N_OR_EXP, 
	/* 43:or_exp(0) -> */
	
	/* 44:or_exp(3) -> */
	T_BAR, N_EXP, A_OP_OR, 
	/* 45:or_exp(3) -> */
	T_LOGICOR, N_EXP, A_OP_LOGIC_OR, 
	/* 46:or_exp(3) -> */
	T_CARET, N_EXP, A_OP_XOR, 
	/* 47:and_exp(2) -> */
	N_RELATIONAL_EXP, N_AND_EXP1, 
	/* 48:and_exp1(0) -> */
	
	/* 49:and_exp1(3) -> */
	T_AMPERSAND, N_AND_EXP, A_OP_AND, 
	/* 50:and_exp1(3) -> */
	T_LOGICAND, N_AND_EXP, A_OP_LOGIC_AND, 
	/* 51:relational_exp(2) -> */
	N_SHIFT_EXP, N_RELATIONAL_EXP1, 
	/* 52:relational_exp1(0) -> */
	
	/* 53:relational_exp1(3) -> */
	T_LESSTHEN, N_RELATIONAL_EXP, A_OP_LT, 
	/* 54:relational_exp1(3) -> */
	T_GREATTHEN, N_RELATIONAL_EXP, A_OP_GT, 
	/* 55:relational_exp1(3) -> */
	T_EQU, N_RELATIONAL_EXP, A_OP_EQU, 
	/* 56:relational_exp1(3) -> */
	T_NEQ, N_RELATIONAL_EXP, A_OP_NEQ, 
	/* 57:relational_exp1(3) -> */
	T_GTE, N_RELATIONAL_EXP, A_OP_GTE, 
	/* 58:relational_exp1(3) -> */
	T_LTE, N_RELATIONAL_EXP, A_OP_LTE, 
	/* 59:shift_exp(2) -> */
	N_ADDITIVE_EXP, N_SHIFT_EXP1, 
	/* 60:shift_exp1(0) -> */
	
	/* 61:shift_exp1(3) -> */
	T_SHL, N_SHIFT_EXP, A_OP_SHL, 
	/* 62:shift_exp1(3) -> */
	T_ASR, N_SHIFT_EXP, A_OP_ASR, 
	/* 63:additive_exp(2) -> */
	N_DIV_EXP, N_ADDITIVE_EXP1, 
	/* 64:additive_exp1(0) -> */
	
	/* 65:additive_exp1(3) -> */
	T_PLUS, N_ADDITIVE_EXP, A_OP_ADD, 
	/* 66:additive_exp1(3) -> */
	T_MINUS, N_ADDITIVE_EXP, A_OP_SUB, 
	/* 67:div_exp(2) -> */
	N_MULT_EXP, N_DIV_EXP1, 
	/* 68:div_exp1(0) -> */
	
	/* 69:div_exp1(3) -> */
	T_SLASH, N_DIV_EXP, A_OP_DIV, 
	/* 70:div_exp1(3) -> */
	T_PERCENT, N_DIV_EXP, A_OP_MOD, 
	/* 71:mult_exp(2) -> */
	N_UNARY_EXP, N_MULT_EXP1, 
	/* 72:mult_exp1(0) -> */
	
	/* 73:mult_exp1(3) -> */
	T_STAR, N_MULT_EXP, A_OP_MUL, 
	/* 74:unary_exp(1) -> */
	N_PRIMARY_EXP, 
	/* 75:unary_exp(3) -> */
	T_TILDE, N_UNARY_EXP, A_OP_INV, 
	/* 76:unary_exp(3) -> */
	T_MINUS, N_UNARY_EXP, A_OP_MINUS, 
	/* 77:unary_exp(3) -> */
	T_NOT, N_UNARY_EXP, A_OP_NOT, 
	/* 78:primary_exp(3) -> */
	T_LPAREN, N_EXP, T_RPAREN, 
	/* 79:primary_exp(2) -> */
	T_INT, A_OP_PUSH_INT, 
	/* 80:primary_exp(2) -> */
	T_CHAR, A_OP_PUSH_INT, 
	/* 81:primary_exp(2) -> */
	T_STRING, A_OP_PUSH_STRING, 
	/* 82:primary_exp(2) -> */
	T_TRUE, A_OP_PUSH_TRUE, 
	/* 83:primary_exp(2) -> */
	T_FALSE, A_OP_PUSH_FALSE, 
	/* 84:primary_exp(1) -> */
	N_ID_EVAL, 
	/* 85:id_eval(3) -> */
	T_ID, A_OP_PUSH_TMP, N_ID_EVAL1, 
	/* 86:id_eval1(1) -> */
	A_OP_VAR_EVAL, 
	/* 87:id_eval1(2) -> */
	N_FUNCTION_CALL, A_OP_CALL_RET, 
	/* 88:id_eval1(4) -> */
	T_LBRACKET, N_EXP, T_RBRACKET, N_ARRAY_VAL, 
	/* 89:id_eval1(3) -> */
	T_DOT, T_ID, A_OP_ATTR_EVAL, 
	/* 90:array_val(1) -> */
	A_OP_ARRAY_EVAL, 
	/* 91:array_val(3) -> */
	T_DOT, T_ID, A_OP_ARRAY_ATTR_EVAL, 
};
/* 233 bytes */

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
	{  91,  4},
	{  95,  5},
	{ 100,  3},
	{ 103,  5},
	{ 108,  5},
	{ 113,  0},
	{ 113,  1},
	{ 114,  3},
	{ 117,  0},
	{ 117,  2},
	{ 119,  0},
	{ 119,  1},
	{ 120,  2},
	{ 122,  0},
	{ 122,  2},
	{ 124,  2},
	{ 126,  0},
	{ 126,  3},
	{ 129,  3},
	{ 132,  3},
	{ 135,  2},
	{ 137,  0},
	{ 137,  3},
	{ 140,  3},
	{ 143,  2},
	{ 145,  0},
	{ 145,  3},
	{ 148,  3},
	{ 151,  3},
	{ 154,  3},
	{ 157,  3},
	{ 160,  3},
	{ 163,  2},
	{ 165,  0},
	{ 165,  3},
	{ 168,  3},
	{ 171,  2},
	{ 173,  0},
	{ 173,  3},
	{ 176,  3},
	{ 179,  2},
	{ 181,  0},
	{ 181,  3},
	{ 184,  3},
	{ 187,  2},
	{ 189,  0},
	{ 189,  3},
	{ 192,  1},
	{ 193,  3},
	{ 196,  3},
	{ 199,  3},
	{ 202,  3},
	{ 205,  2},
	{ 207,  2},
	{ 209,  2},
	{ 211,  2},
	{ 213,  2},
	{ 215,  1},
	{ 216,  3},
	{ 219,  1},
	{ 220,  2},
	{ 222,  4},
	{ 226,  3},
	{ 229,  1},
	{ 230,  3},
};
/* 184 bytes */

/* Total: 1255 bytes */


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
 	"array_assign",
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
 	"div_exp",
 	"additive_exp1",
 	"mult_exp",
 	"div_exp1",
 	"unary_exp",
 	"mult_exp1",
 	"primary_exp",
 	"id_eval",
 	"id_eval1",
 	"array_val",
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
 	"op_var_assign",
 	"op_catch_end",
 	"op_pop_tmp",
 	"op_ret_discard",
 	"op_attr_assign",
 	"op_array_assign",
 	"op_array_attr_assign",
 	"op_function_lookup",
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
 	"op_div",
 	"op_mod",
 	"op_mul",
 	"op_inv",
 	"op_minus",
 	"op_not",
 	"op_push_int",
 	"op_push_string",
 	"op_push_true",
 	"op_push_false",
 	"op_var_eval",
 	"op_call_ret",
 	"op_attr_eval",
 	"op_array_eval",
 	"op_array_attr_eval",
 };

