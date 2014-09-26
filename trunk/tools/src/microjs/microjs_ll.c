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
	{  3, 38}, { 22, 38}, { 48, 39}, 
	/*  59 (exp) */
	{  6, 43}, { 13, 43}, { 21, 43}, { 37, 43}, { 44, 43}, { 48, 43}, 
	{ 49, 43}, { 50, 43}, { 51, 43}, { 53, 43}, 
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
	{ 21, 32}, 
	/*  70 (object_assign) */
	{  1, 30}, { 16, 29}, 
	/*  71 (attr_assign) */
	{ 48, 31}, 
	/*  72 (arg_list_opt) */
	{  6, 34}, { 13, 34}, { 21, 34}, { 22, 33}, { 37, 34}, { 44, 34}, 
	{ 48, 34}, { 49, 34}, { 50, 34}, { 51, 34}, { 53, 34}, 
	/*  73 (arg_list) */
	{  6, 35}, { 13, 35}, { 21, 35}, { 37, 35}, { 44, 35}, { 48, 35}, 
	{ 49, 35}, { 50, 35}, { 51, 35}, { 53, 35}, 
	/*  74 (arg_list1) */
	{  2, 37}, { 22, 36}, 
	/*  75 (exp_lst) */
	{ 48, 40}, 
	/*  76 (exp_lst1) */
	{  2, 42}, {  3, 41}, { 22, 41}, 
	/*  77 (and_exp) */
	{  6, 48}, { 13, 48}, { 21, 48}, { 37, 48}, { 44, 48}, { 48, 48}, 
	{ 49, 48}, { 50, 48}, { 51, 48}, { 53, 48}, 
	/*  78 (or_exp) */
	{  2, 44}, {  3, 44}, { 11, 45}, { 12, 47}, { 20, 44}, { 22, 44}, 
	{ 31, 46}, 
	/*  79 (relational_exp) */
	{  6, 52}, { 13, 52}, { 21, 52}, { 37, 52}, { 44, 52}, { 48, 52}, 
	{ 49, 52}, { 50, 52}, { 51, 52}, { 53, 52}, 
	/*  80 (and_exp1) */
	{  2, 49}, {  3, 49}, { 10, 50}, { 11, 49}, { 12, 49}, { 20, 49}, 
	{ 22, 49}, { 31, 49}, { 32, 51}, 
	/*  81 (shift_exp) */
	{  6, 60}, { 13, 60}, { 21, 60}, { 37, 60}, { 44, 60}, { 48, 60}, 
	{ 49, 60}, { 50, 60}, { 51, 60}, { 53, 60}, 
	/*  82 (relational_exp1) */
	{  2, 53}, {  3, 53}, { 10, 53}, { 11, 53}, { 12, 53}, { 17, 54}, 
	{ 18, 55}, { 20, 53}, { 22, 53}, { 25, 58}, { 26, 59}, { 27, 56}, 
	{ 28, 57}, { 31, 53}, { 32, 53}, 
	/*  83 (additive_exp) */
	{  6, 64}, { 13, 64}, { 21, 64}, { 37, 64}, { 44, 64}, { 48, 64}, 
	{ 49, 64}, { 50, 64}, { 51, 64}, { 53, 64}, 
	/*  84 (shift_exp1) */
	{  2, 61}, {  3, 61}, { 10, 61}, { 11, 61}, { 12, 61}, { 17, 61}, 
	{ 18, 61}, { 20, 61}, { 22, 61}, { 25, 61}, { 26, 61}, { 27, 61}, 
	{ 28, 61}, { 29, 63}, { 30, 62}, { 31, 61}, { 32, 61}, 
	/*  85 (div_exp) */
	{  6, 68}, { 13, 68}, { 21, 68}, { 37, 68}, { 44, 68}, { 48, 68}, 
	{ 49, 68}, { 50, 68}, { 51, 68}, { 53, 68}, 
	/*  86 (additive_exp1) */
	{  2, 65}, {  3, 65}, {  5, 66}, {  6, 67}, { 10, 65}, { 11, 65}, 
	{ 12, 65}, { 17, 65}, { 18, 65}, { 20, 65}, { 22, 65}, { 25, 65}, 
	{ 26, 65}, { 27, 65}, { 28, 65}, { 29, 65}, { 30, 65}, { 31, 65}, 
	{ 32, 65}, 
	/*  87 (mult_exp) */
	{  6, 72}, { 13, 72}, { 21, 72}, { 37, 72}, { 44, 72}, { 48, 72}, 
	{ 49, 72}, { 50, 72}, { 51, 72}, { 53, 72}, 
	/*  88 (div_exp1) */
	{  2, 69}, {  3, 69}, {  5, 69}, {  6, 69}, {  8, 70}, {  9, 71}, 
	{ 10, 69}, { 11, 69}, { 12, 69}, { 17, 69}, { 18, 69}, { 20, 69}, 
	{ 22, 69}, { 25, 69}, { 26, 69}, { 27, 69}, { 28, 69}, { 29, 69}, 
	{ 30, 69}, { 31, 69}, { 32, 69}, 
	/*  89 (unary_exp) */
	{  6, 77}, { 13, 76}, { 21, 75}, { 37, 75}, { 44, 75}, { 48, 75}, 
	{ 49, 75}, { 50, 75}, { 51, 75}, { 53, 78}, 
	/*  90 (mult_exp1) */
	{  2, 73}, {  3, 73}, {  5, 73}, {  6, 73}, {  7, 74}, {  8, 73}, 
	{  9, 73}, { 10, 73}, { 11, 73}, { 12, 73}, { 17, 73}, { 18, 73}, 
	{ 20, 73}, { 22, 73}, { 25, 73}, { 26, 73}, { 27, 73}, { 28, 73}, 
	{ 29, 73}, { 30, 73}, { 31, 73}, { 32, 73}, 
	/*  91 (primary_exp) */
	{ 21, 79}, { 37, 84}, { 44, 83}, { 48, 85}, { 49, 80}, { 50, 81}, 
	{ 51, 82}, 
	/*  92 (id_eval) */
	{ 48, 86}, 
	/*  93 (id_eval1) */
	{  1, 90}, {  2, 87}, {  3, 87}, {  5, 87}, {  6, 87}, {  7, 87}, 
	{  8, 87}, {  9, 87}, { 10, 87}, { 11, 87}, { 12, 87}, { 17, 87}, 
	{ 18, 87}, { 19, 89}, { 20, 87}, { 21, 88}, { 22, 87}, { 25, 87}, 
	{ 26, 87}, { 27, 87}, { 28, 87}, { 29, 87}, { 30, 87}, { 31, 87}, 
	{ 32, 87}, 
	/*  94 (object_eval) */
	{  1, 92}, {  2, 91}, {  3, 91}, {  5, 91}, {  6, 91}, {  7, 91}, 
	{  8, 91}, {  9, 91}, { 10, 91}, { 11, 91}, { 12, 91}, { 17, 91}, 
	{ 18, 91}, { 20, 91}, { 22, 91}, { 25, 91}, { 26, 91}, { 27, 91}, 
	{ 28, 91}, { 29, 91}, { 30, 91}, { 31, 91}, { 32, 91}, 
	/*  95 (attr_eval) */
	{ 48, 93}, 
};
/* 682 bytes; */

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
	{  76,  1},
	{  77, 11},
	{  88, 10},
	{  98,  2},
	{ 100,  1},
	{ 101,  3},
	{ 104, 10},
	{ 114,  7},
	{ 121, 10},
	{ 131,  9},
	{ 140, 10},
	{ 150, 15},
	{ 165, 10},
	{ 175, 17},
	{ 192, 10},
	{ 202, 19},
	{ 221, 10},
	{ 231, 21},
	{ 252, 10},
	{ 262, 22},
	{ 284,  7},
	{ 291,  1},
	{ 292, 25},
	{ 317, 23},
	{ 340,  1},
};
/* 168 bytes */

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
	/* 27:assign_or_call1(5) -> */
	T_LBRACKET, N_EXP, T_RBRACKET, A_OP_ARRAY_XLAT, N_OBJECT_ASSIGN, 
	/* 28:assign_or_call1(2) -> */
	T_DOT, N_ATTR_ASSIGN, 
	/* 29:object_assign(3) -> */
	T_EQUALS, N_EXP, A_OP_OBJECT_ASSIGN, 
	/* 30:object_assign(2) -> */
	T_DOT, N_ATTR_ASSIGN, 
	/* 31:attr_assign(5) -> */
	T_ID, A_OP_PUSH_TMP, T_EQUALS, N_EXP, A_OP_ATTR_ASSIGN, 
	/* 32:function_call(5) -> */
	A_OP_FUNCTION_LOOKUP, T_LPAREN, N_ARG_LIST_OPT, T_RPAREN, A_OP_CALL, 
	/* 33:arg_list_opt(0) -> */
	
	/* 34:arg_list_opt(1) -> */
	N_ARG_LIST, 
	/* 35:arg_list(3) -> */
	N_EXP, A_OP_ARG, N_ARG_LIST1, 
	/* 36:arg_list1(0) -> */
	
	/* 37:arg_list1(2) -> */
	T_COMMA, N_ARG_LIST, 
	/* 38:exp_lst_opt(0) -> */
	
	/* 39:exp_lst_opt(1) -> */
	N_EXP_LST, 
	/* 40:exp_lst(2) -> */
	N_ASSIGN_OR_CALL, N_EXP_LST1, 
	/* 41:exp_lst1(0) -> */
	
	/* 42:exp_lst1(2) -> */
	T_COMMA, N_EXP_LST, 
	/* 43:exp(2) -> */
	N_AND_EXP, N_OR_EXP, 
	/* 44:or_exp(0) -> */
	
	/* 45:or_exp(3) -> */
	T_BAR, N_EXP, A_OP_OR, 
	/* 46:or_exp(3) -> */
	T_LOGICOR, N_EXP, A_OP_LOGIC_OR, 
	/* 47:or_exp(3) -> */
	T_CARET, N_EXP, A_OP_XOR, 
	/* 48:and_exp(2) -> */
	N_RELATIONAL_EXP, N_AND_EXP1, 
	/* 49:and_exp1(0) -> */
	
	/* 50:and_exp1(3) -> */
	T_AMPERSAND, N_AND_EXP, A_OP_AND, 
	/* 51:and_exp1(3) -> */
	T_LOGICAND, N_AND_EXP, A_OP_LOGIC_AND, 
	/* 52:relational_exp(2) -> */
	N_SHIFT_EXP, N_RELATIONAL_EXP1, 
	/* 53:relational_exp1(0) -> */
	
	/* 54:relational_exp1(3) -> */
	T_LESSTHEN, N_RELATIONAL_EXP, A_OP_LT, 
	/* 55:relational_exp1(3) -> */
	T_GREATTHEN, N_RELATIONAL_EXP, A_OP_GT, 
	/* 56:relational_exp1(3) -> */
	T_EQU, N_RELATIONAL_EXP, A_OP_EQU, 
	/* 57:relational_exp1(3) -> */
	T_NEQ, N_RELATIONAL_EXP, A_OP_NEQ, 
	/* 58:relational_exp1(3) -> */
	T_GTE, N_RELATIONAL_EXP, A_OP_GTE, 
	/* 59:relational_exp1(3) -> */
	T_LTE, N_RELATIONAL_EXP, A_OP_LTE, 
	/* 60:shift_exp(2) -> */
	N_ADDITIVE_EXP, N_SHIFT_EXP1, 
	/* 61:shift_exp1(0) -> */
	
	/* 62:shift_exp1(3) -> */
	T_SHL, N_SHIFT_EXP, A_OP_SHL, 
	/* 63:shift_exp1(3) -> */
	T_ASR, N_SHIFT_EXP, A_OP_ASR, 
	/* 64:additive_exp(2) -> */
	N_DIV_EXP, N_ADDITIVE_EXP1, 
	/* 65:additive_exp1(0) -> */
	
	/* 66:additive_exp1(3) -> */
	T_PLUS, N_ADDITIVE_EXP, A_OP_ADD, 
	/* 67:additive_exp1(3) -> */
	T_MINUS, N_ADDITIVE_EXP, A_OP_SUB, 
	/* 68:div_exp(2) -> */
	N_MULT_EXP, N_DIV_EXP1, 
	/* 69:div_exp1(0) -> */
	
	/* 70:div_exp1(3) -> */
	T_SLASH, N_DIV_EXP, A_OP_DIV, 
	/* 71:div_exp1(3) -> */
	T_PERCENT, N_DIV_EXP, A_OP_MOD, 
	/* 72:mult_exp(2) -> */
	N_UNARY_EXP, N_MULT_EXP1, 
	/* 73:mult_exp1(0) -> */
	
	/* 74:mult_exp1(3) -> */
	T_STAR, N_MULT_EXP, A_OP_MUL, 
	/* 75:unary_exp(1) -> */
	N_PRIMARY_EXP, 
	/* 76:unary_exp(3) -> */
	T_TILDE, N_UNARY_EXP, A_OP_INV, 
	/* 77:unary_exp(3) -> */
	T_MINUS, N_UNARY_EXP, A_OP_MINUS, 
	/* 78:unary_exp(3) -> */
	T_NOT, N_UNARY_EXP, A_OP_NOT, 
	/* 79:primary_exp(3) -> */
	T_LPAREN, N_EXP, T_RPAREN, 
	/* 80:primary_exp(2) -> */
	T_INT, A_OP_PUSH_INT, 
	/* 81:primary_exp(2) -> */
	T_CHAR, A_OP_PUSH_INT, 
	/* 82:primary_exp(2) -> */
	T_STRING, A_OP_PUSH_STRING, 
	/* 83:primary_exp(2) -> */
	T_TRUE, A_OP_PUSH_TRUE, 
	/* 84:primary_exp(2) -> */
	T_FALSE, A_OP_PUSH_FALSE, 
	/* 85:primary_exp(1) -> */
	N_ID_EVAL, 
	/* 86:id_eval(3) -> */
	T_ID, A_OP_PUSH_TMP, N_ID_EVAL1, 
	/* 87:id_eval1(1) -> */
	A_OP_VAR_EVAL, 
	/* 88:id_eval1(2) -> */
	N_FUNCTION_CALL, A_OP_CALL_RET, 
	/* 89:id_eval1(5) -> */
	T_LBRACKET, N_EXP, T_RBRACKET, A_OP_ARRAY_XLAT, N_OBJECT_EVAL, 
	/* 90:id_eval1(2) -> */
	T_DOT, N_ATTR_EVAL, 
	/* 91:object_eval(1) -> */
	A_OP_OBJECT_EVAL, 
	/* 92:object_eval(2) -> */
	T_DOT, N_ATTR_EVAL, 
	/* 93:attr_eval(3) -> */
	T_ID, A_OP_PUSH_TMP, A_OP_ATTR_EVAL, 
};
/* 235 bytes */

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
	{  96,  2},
	{  98,  3},
	{ 101,  2},
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
	{ 222,  5},
	{ 227,  2},
	{ 229,  1},
	{ 230,  2},
	{ 232,  3},
};
/* 188 bytes */

/* Total: 1273 bytes */


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
 	"object_assign",
 	"attr_assign",
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
 	"object_eval",
 	"attr_eval",
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
 	"op_array_xlat",
 	"op_object_assign",
 	"op_attr_assign",
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
 	"op_object_eval",
 	"op_attr_eval",
 };

