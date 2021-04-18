/* LL(1) Embeddedd Nonrecursive Predictive Parser */

#include "types_ll.h"

/* Token-Rule pair (predict set) */
struct tr_pair {
	uint8_t t;
	uint8_t r;
};

/* Predict sets */
static const struct tr_pair predict_vec[] = {
	/*  66 (program) */
	{  0,  0}, { 23,  2}, { 39,  1}, { 41,  1}, { 43,  1}, { 45,  1}, 
	{ 47,  1}, { 53,  1}, { 54,  1}, { 55,  1}, 
	/*  67 (stat) */
	{ 39,  6}, { 41,  8}, { 43, 10}, { 45,  9}, { 47,  7}, { 53, 11}, 
	{ 54, 12}, { 55, 13}, 
	/*  68 (compound_stat) */
	{ 23,  3}, 
	/*  69 (stat_list) */
	{ 24,  4}, { 39,  5}, { 41,  5}, { 43,  5}, { 45,  5}, { 47,  5}, 
	{ 53,  5}, { 54,  5}, { 55,  5}, 
	/*  70 (for_lst_opt) */
	{  3, 32}, { 22, 32}, { 56, 33}, { 57, 33}, { 58, 33}, { 59, 33}, 
	{ 60, 33}, { 61, 33}, 
	/*  71 (int_exp) */
	{ 13, 68}, { 21, 68}, { 49, 68}, { 50, 68}, { 56, 68}, { 57, 68}, 
	{ 63, 68}, { 64, 68}, 
	/*  72 (condition) */
	{ 21, 25}, 
	/*  73 (else_opt) */
	{  0, 21}, { 23, 21}, { 24, 21}, { 36, 22}, { 39, 21}, { 41, 21}, 
	{ 43, 21}, { 45, 21}, { 47, 21}, { 53, 21}, { 54, 21}, { 55, 21}, 
	/*  74 (catch_opt) */
	{  0, 23}, { 23, 23}, { 24, 23}, { 34, 24}, { 39, 23}, { 41, 23}, 
	{ 43, 23}, { 45, 23}, { 47, 23}, { 53, 23}, { 54, 23}, { 55, 23}, 
	/*  75 (bool_var_list) */
	{ 48, 37}, 
	/*  76 (int_var_list) */
	{ 48, 55}, 
	/*  77 (fix_var_list) */
	{ 48, 97}, 
	/*  78 (assign_or_call) */
	{ 56, 14}, { 57, 15}, { 58, 16}, { 59, 17}, { 60, 18}, { 61, 19}, 
	/*  79 (function_call) */
	{ 21, 20}, 
	/*  80 (fix_exp) */
	{  6,110}, { 21,110}, { 49,110}, { 50,110}, { 56,110}, { 57,110}, 
	{ 58,110}, { 59,110}, { 65,110}, 
	/*  81 (bool_exp) */
	{ 21, 43}, { 37, 43}, { 44, 43}, { 62, 43}, 
	/*  82 (arg_list_opt) */
	{ 13, 27}, { 21, 27}, { 22, 26}, { 49, 27}, { 50, 27}, { 56, 27}, 
	{ 57, 27}, { 63, 27}, { 64, 27}, 
	/*  83 (arg_list) */
	{ 13, 28}, { 21, 28}, { 49, 28}, { 50, 28}, { 56, 28}, { 57, 28}, 
	{ 63, 28}, { 64, 28}, 
	/*  84 (exp) */
	{ 13, 31}, { 21, 31}, { 49, 31}, { 50, 31}, { 56, 31}, { 57, 31}, 
	{ 63, 31}, { 64, 31}, 
	/*  85 (arg_list1) */
	{  2, 30}, { 22, 29}, 
	/*  86 (for_lst) */
	{ 56, 34}, { 57, 34}, { 58, 34}, { 59, 34}, { 60, 34}, { 61, 34}, 
	/*  87 (for_lst1) */
	{  2, 36}, {  3, 35}, { 22, 35}, 
	/*  88 (bool_var) */
	{ 48, 40}, 
	/*  89 (bool_var_list1) */
	{  2, 39}, {  3, 38}, 
	/*  90 (bool_var_assign_opt) */
	{  2, 41}, {  3, 41}, { 16, 42}, 
	/*  91 (bool_and_exp) */
	{ 21, 46}, { 37, 46}, { 44, 46}, { 62, 46}, 
	/*  92 (bool_or_exp) */
	{  2, 44}, {  3, 44}, { 22, 44}, { 31, 45}, 
	/*  93 (bool_unary_exp) */
	{ 21, 49}, { 37, 49}, { 44, 49}, { 62, 50}, 
	/*  94 (bool_and_exp1) */
	{  2, 47}, {  3, 47}, { 22, 47}, { 31, 47}, { 32, 48}, 
	/*  95 (bool_pri_exp) */
	{ 21, 53}, { 37, 52}, { 44, 51}, 
	/*  96 (rel_exp) */
	{ 13, 54}, { 21, 54}, { 49, 54}, { 50, 54}, { 56, 54}, { 57, 54}, 
	{ 63, 54}, { 64, 54}, 
	/*  97 (int_rel_exp) */
	{ 13, 61}, { 21, 61}, { 49, 61}, { 50, 61}, { 56, 61}, { 57, 61}, 
	{ 63, 61}, { 64, 61}, 
	/*  98 (int_var) */
	{ 48, 58}, 
	/*  99 (int_var_list1) */
	{  2, 57}, {  3, 56}, 
	/* 100 (int_var_assign_opt) */
	{  2, 59}, {  3, 59}, { 16, 60}, 
	/* 101 (int_rel_exp1) */
	{ 17, 62}, { 18, 63}, { 25, 66}, { 26, 67}, { 27, 64}, { 28, 65}, 
	/* 102 (and_exp) */
	{ 13, 72}, { 21, 72}, { 49, 72}, { 50, 72}, { 56, 72}, { 57, 72}, 
	{ 63, 72}, { 64, 72}, 
	/* 103 (or_exp) */
	{  2, 69}, {  3, 69}, { 11, 70}, { 12, 71}, { 17, 69}, { 18, 69}, 
	{ 22, 69}, { 25, 69}, { 26, 69}, { 27, 69}, { 28, 69}, 
	/* 104 (shift_exp) */
	{ 13, 75}, { 21, 75}, { 49, 75}, { 50, 75}, { 56, 75}, { 57, 75}, 
	{ 63, 75}, { 64, 75}, 
	/* 105 (and_exp1) */
	{  2, 73}, {  3, 73}, { 10, 74}, { 11, 73}, { 12, 73}, { 17, 73}, 
	{ 18, 73}, { 22, 73}, { 25, 73}, { 26, 73}, { 27, 73}, { 28, 73}, 
	/* 106 (int_add_exp) */
	{ 13, 79}, { 21, 79}, { 49, 79}, { 50, 79}, { 56, 79}, { 57, 79}, 
	{ 63, 79}, { 64, 79}, 
	/* 107 (shift_exp1) */
	{  2, 76}, {  3, 76}, { 10, 76}, { 11, 76}, { 12, 76}, { 17, 76}, 
	{ 18, 76}, { 22, 76}, { 25, 76}, { 26, 76}, { 27, 76}, { 28, 76}, 
	{ 29, 78}, { 30, 77}, 
	/* 108 (int_mult_exp) */
	{ 13, 83}, { 21, 83}, { 49, 83}, { 50, 83}, { 56, 83}, { 57, 83}, 
	{ 63, 83}, { 64, 83}, 
	/* 109 (int_add_exp1) */
	{  2, 80}, {  3, 80}, {  5, 81}, {  6, 82}, { 10, 80}, { 11, 80}, 
	{ 12, 80}, { 17, 80}, { 18, 80}, { 22, 80}, { 25, 80}, { 26, 80}, 
	{ 27, 80}, { 28, 80}, { 29, 80}, { 30, 80}, 
	/* 110 (int_unary_exp) */
	{ 13, 89}, { 21, 88}, { 49, 88}, { 50, 88}, { 56, 88}, { 57, 88}, 
	{ 63, 88}, { 64, 88}, 
	/* 111 (int_mult_exp1) */
	{  2, 84}, {  3, 84}, {  5, 84}, {  6, 84}, {  7, 87}, {  8, 85}, 
	{  9, 86}, { 10, 84}, { 11, 84}, { 12, 84}, { 17, 84}, { 18, 84}, 
	{ 22, 84}, { 25, 84}, { 26, 84}, { 27, 84}, { 28, 84}, { 29, 84}, 
	{ 30, 84}, 
	/* 112 (int_primary_exp) */
	{ 21, 96}, { 49, 90}, { 50, 91}, { 56, 92}, { 57, 93}, { 63, 94}, 
	{ 64, 95}, 
	/* 113 (fix_var) */
	{ 48,100}, 
	/* 114 (fix_var_list1) */
	{  2, 99}, {  3, 98}, 
	/* 115 (fix_var_assign_opt) */
	{  2,101}, {  3,101}, { 16,102}, 
	/* 116 (fix_rel_exp) */
	{  6,103}, { 21,103}, { 49,103}, { 50,103}, { 56,103}, { 57,103}, 
	{ 58,103}, { 59,103}, { 65,103}, 
	/* 117 (fix_rel_exp1) */
	{ 17,104}, { 18,105}, { 25,108}, { 26,109}, { 27,106}, { 28,107}, 
	/* 118 (fix_add_exp) */
	{  6,111}, { 21,111}, { 49,111}, { 50,111}, { 56,111}, { 57,111}, 
	{ 58,111}, { 59,111}, { 65,111}, 
	/* 119 (fix_mult_exp) */
	{  6,115}, { 21,115}, { 49,115}, { 50,115}, { 56,115}, { 57,115}, 
	{ 58,115}, { 59,115}, { 65,115}, 
	/* 120 (fix_add_exp1) */
	{  2,112}, {  3,112}, {  5,113}, {  6,114}, { 17,112}, { 18,112}, 
	{ 22,112}, { 25,112}, { 26,112}, { 27,112}, { 28,112}, 
	/* 121 (fix_unary_exp) */
	{  6,120}, { 21,119}, { 49,119}, { 50,119}, { 56,119}, { 57,119}, 
	{ 58,119}, { 59,119}, { 65,119}, 
	/* 122 (fix_mult_exp1) */
	{  2,116}, {  3,116}, {  5,116}, {  6,116}, {  7,118}, {  8,117}, 
	{ 17,116}, { 18,116}, { 22,116}, { 25,116}, { 26,116}, { 27,116}, 
	{ 28,116}, 
	/* 123 (fix_primary_exp) */
	{ 21,128}, { 49,124}, { 50,125}, { 56,126}, { 57,127}, { 58,122}, 
	{ 59,123}, { 65,121}, 
};
/* 762 bytes; */

static const struct {
	uint16_t off;
	uint8_t cnt;
} predict_idx[] = {
	{   0, 10},
	{  10,  8},
	{  18,  1},
	{  19,  9},
	{  28,  8},
	{  36,  8},
	{  44,  1},
	{  45, 12},
	{  57, 12},
	{  69,  1},
	{  70,  1},
	{  71,  1},
	{  72,  6},
	{  78,  1},
	{  79,  9},
	{  88,  4},
	{  92,  9},
	{ 101,  8},
	{ 109,  8},
	{ 117,  2},
	{ 119,  6},
	{ 125,  3},
	{ 128,  1},
	{ 129,  2},
	{ 131,  3},
	{ 134,  4},
	{ 138,  4},
	{ 142,  4},
	{ 146,  5},
	{ 151,  3},
	{ 154,  8},
	{ 162,  8},
	{ 170,  1},
	{ 171,  2},
	{ 173,  3},
	{ 176,  6},
	{ 182,  8},
	{ 190, 11},
	{ 201,  8},
	{ 209, 12},
	{ 221,  8},
	{ 229, 14},
	{ 243,  8},
	{ 251, 16},
	{ 267,  8},
	{ 275, 19},
	{ 294,  7},
	{ 301,  1},
	{ 302,  2},
	{ 304,  3},
	{ 307,  9},
	{ 316,  6},
	{ 322,  9},
	{ 331,  9},
	{ 340, 11},
	{ 351,  9},
	{ 360, 13},
	{ 373,  8},
};
/* 232 bytes */

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
	T_FOR, T_LPAREN, N_FOR_LST_OPT, T_SEMICOLON, A_OP_FOR_INIT, N_INT_EXP, A_OP_FOR_COND, T_SEMICOLON, N_FOR_LST_OPT, A_OP_FOR_AFTER, T_RPAREN, N_COMPOUND_STAT, A_OP_FOR_END, 
	/*  7:stat(6) -> */
	T_WHILE, A_OP_WHILE_BEGIN, N_CONDITION, A_OP_WHILE_COND, N_COMPOUND_STAT, A_OP_WHILE_END, 
	/*  8:stat(6) -> */
	T_IF, N_CONDITION, A_OP_IF_COND, N_COMPOUND_STAT, N_ELSE_OPT, A_OP_IF_END, 
	/*  9:stat(4) -> */
	T_TRY, A_OP_TRY_BEGIN, N_COMPOUND_STAT, N_CATCH_OPT, 
	/* 10:stat(4) -> */
	T_THROW, N_INT_EXP, A_OP_THROW, T_SEMICOLON, 
	/* 11:stat(3) -> */
	T_BOOL, N_BOOL_VAR_LIST, T_SEMICOLON, 
	/* 12:stat(3) -> */
	T_INT, N_INT_VAR_LIST, T_SEMICOLON, 
	/* 13:stat(3) -> */
	T_FIX, N_FIX_VAR_LIST, T_SEMICOLON, 
	/* 14:assign_or_call(5) -> */
	T_INT_VAR, T_EQUALS, N_INT_EXP, A_OP_INT_ASSIGN, T_SEMICOLON, 
	/* 15:assign_or_call(4) -> */
	T_INT_FUN, N_FUNCTION_CALL, A_OP_RET_DISCARD, T_SEMICOLON, 
	/* 16:assign_or_call(5) -> */
	T_FIX_VAR, T_EQUALS, N_FIX_EXP, A_OP_FIX_ASSIGN, T_SEMICOLON, 
	/* 17:assign_or_call(4) -> */
	T_FIX_FUN, N_FUNCTION_CALL, A_OP_RET_DISCARD, T_SEMICOLON, 
	/* 18:assign_or_call(5) -> */
	T_BOOL_VAR, T_EQUALS, N_BOOL_EXP, A_OP_BOOL_ASSIGN, T_SEMICOLON, 
	/* 19:assign_or_call(4) -> */
	T_BOOL_FUN, N_FUNCTION_CALL, A_OP_RET_DISCARD, T_SEMICOLON, 
	/* 20:function_call(5) -> */
	A_OP_METHOD, T_LPAREN, N_ARG_LIST_OPT, T_RPAREN, A_OP_CALL, 
	/* 21:else_opt(0) -> */
	
	/* 22:else_opt(3) -> */
	T_ELSE, A_OP_IF_ELSE, N_COMPOUND_STAT, 
	/* 23:catch_opt(1) -> */
	A_OP_TRY_END, 
	/* 24:catch_opt(14) -> */
	T_CATCH, A_OP_CATCH, A_OP_BLK_OPEN, T_LPAREN, T_ID, A_OP_INT_VAR_DECL, A_OP_PUSH_TMP, A_OP_ASSIGN, T_RPAREN, T_LBRACE, N_STAT_LIST, T_RBRACE, A_OP_BLK_CLOSE, A_OP_CATCH_END, 
	/* 25:condition(3) -> */
	T_LPAREN, N_INT_EXP, T_RPAREN, 
	/* 26:arg_list_opt(0) -> */
	
	/* 27:arg_list_opt(1) -> */
	N_ARG_LIST, 
	/* 28:arg_list(3) -> */
	N_EXP, A_OP_ARG, N_ARG_LIST1, 
	/* 29:arg_list1(0) -> */
	
	/* 30:arg_list1(2) -> */
	T_COMMA, N_ARG_LIST, 
	/* 31:exp(1) -> */
	N_INT_EXP, 
	/* 32:for_lst_opt(0) -> */
	
	/* 33:for_lst_opt(1) -> */
	N_FOR_LST, 
	/* 34:for_lst(2) -> */
	N_ASSIGN_OR_CALL, N_FOR_LST1, 
	/* 35:for_lst1(0) -> */
	
	/* 36:for_lst1(2) -> */
	T_COMMA, N_FOR_LST, 
	/* 37:bool_var_list(2) -> */
	N_BOOL_VAR, N_BOOL_VAR_LIST1, 
	/* 38:bool_var_list1(0) -> */
	
	/* 39:bool_var_list1(2) -> */
	T_COMMA, N_BOOL_VAR_LIST, 
	/* 40:bool_var(4) -> */
	T_ID, A_OP_VAR_DECL, A_OP_PUSH_TMP, N_BOOL_VAR_ASSIGN_OPT, 
	/* 41:bool_var_assign_opt(1) -> */
	A_OP_POP_TMP, 
	/* 42:bool_var_assign_opt(3) -> */
	T_EQUALS, N_BOOL_EXP, A_OP_BOOL_ASSIGN, 
	/* 43:bool_exp(2) -> */
	N_BOOL_AND_EXP, N_BOOL_OR_EXP, 
	/* 44:bool_or_exp(0) -> */
	
	/* 45:bool_or_exp(3) -> */
	T_LOGICOR, N_BOOL_EXP, A_OP_LOGIC_OR, 
	/* 46:bool_and_exp(2) -> */
	N_BOOL_UNARY_EXP, N_BOOL_AND_EXP1, 
	/* 47:bool_and_exp1(0) -> */
	
	/* 48:bool_and_exp1(3) -> */
	T_LOGICAND, N_BOOL_AND_EXP, A_OP_LOGIC_AND, 
	/* 49:bool_unary_exp(1) -> */
	N_BOOL_PRI_EXP, 
	/* 50:bool_unary_exp(3) -> */
	T_NOT, N_BOOL_UNARY_EXP, A_OP_NOT, 
	/* 51:bool_pri_exp(2) -> */
	T_TRUE, A_OP_PUSH_TRUE, 
	/* 52:bool_pri_exp(2) -> */
	T_FALSE, A_OP_PUSH_FALSE, 
	/* 53:bool_pri_exp(3) -> */
	T_LPAREN, N_BOOL_EXP, T_RPAREN, 
	/* 54:rel_exp(1) -> */
	N_INT_REL_EXP, 
	/* 55:int_var_list(2) -> */
	N_INT_VAR, N_INT_VAR_LIST1, 
	/* 56:int_var_list1(0) -> */
	
	/* 57:int_var_list1(2) -> */
	T_COMMA, N_INT_VAR_LIST, 
	/* 58:int_var(4) -> */
	T_ID, A_OP_VAR_DECL, A_OP_PUSH_TMP, N_INT_VAR_ASSIGN_OPT, 
	/* 59:int_var_assign_opt(1) -> */
	A_OP_POP_TMP, 
	/* 60:int_var_assign_opt(3) -> */
	T_EQUALS, N_INT_EXP, A_OP_INT_ASSIGN, 
	/* 61:int_rel_exp(2) -> */
	N_INT_EXP, N_INT_REL_EXP1, 
	/* 62:int_rel_exp1(3) -> */
	T_LESSTHEN, N_INT_EXP, A_OP_LT, 
	/* 63:int_rel_exp1(3) -> */
	T_GREATTHEN, N_INT_EXP, A_OP_GT, 
	/* 64:int_rel_exp1(3) -> */
	T_EQU, N_INT_EXP, A_OP_EQU, 
	/* 65:int_rel_exp1(3) -> */
	T_NEQ, N_INT_EXP, A_OP_NEQ, 
	/* 66:int_rel_exp1(3) -> */
	T_GTE, N_INT_EXP, A_OP_GTE, 
	/* 67:int_rel_exp1(3) -> */
	T_LTE, N_INT_EXP, A_OP_LTE, 
	/* 68:int_exp(2) -> */
	N_AND_EXP, N_OR_EXP, 
	/* 69:or_exp(0) -> */
	
	/* 70:or_exp(3) -> */
	T_BAR, N_INT_EXP, A_OP_OR, 
	/* 71:or_exp(3) -> */
	T_CARET, N_INT_EXP, A_OP_XOR, 
	/* 72:and_exp(2) -> */
	N_SHIFT_EXP, N_AND_EXP1, 
	/* 73:and_exp1(0) -> */
	
	/* 74:and_exp1(3) -> */
	T_AMPERSAND, N_AND_EXP, A_OP_AND, 
	/* 75:shift_exp(2) -> */
	N_INT_ADD_EXP, N_SHIFT_EXP1, 
	/* 76:shift_exp1(0) -> */
	
	/* 77:shift_exp1(3) -> */
	T_SHL, N_SHIFT_EXP, A_OP_SHL, 
	/* 78:shift_exp1(3) -> */
	T_ASR, N_SHIFT_EXP, A_OP_ASR, 
	/* 79:int_add_exp(2) -> */
	N_INT_MULT_EXP, N_INT_ADD_EXP1, 
	/* 80:int_add_exp1(0) -> */
	
	/* 81:int_add_exp1(3) -> */
	T_PLUS, N_INT_ADD_EXP, A_OP_ADD, 
	/* 82:int_add_exp1(3) -> */
	T_MINUS, N_INT_ADD_EXP, A_OP_SUB, 
	/* 83:int_mult_exp(2) -> */
	N_INT_UNARY_EXP, N_INT_MULT_EXP1, 
	/* 84:int_mult_exp1(0) -> */
	
	/* 85:int_mult_exp1(3) -> */
	T_SLASH, N_INT_MULT_EXP, A_OP_DIV, 
	/* 86:int_mult_exp1(3) -> */
	T_PERCENT, N_INT_MULT_EXP, A_OP_MOD, 
	/* 87:int_mult_exp1(3) -> */
	T_STAR, N_INT_MULT_EXP, A_OP_MUL, 
	/* 88:int_unary_exp(1) -> */
	N_INT_PRIMARY_EXP, 
	/* 89:int_unary_exp(3) -> */
	T_TILDE, N_INT_PRIMARY_EXP, A_OP_INV, 
	/* 90:int_primary_exp(2) -> */
	T_INT_VAL, A_OP_PUSH_INT, 
	/* 91:int_primary_exp(2) -> */
	T_CHAR, A_OP_PUSH_INT, 
	/* 92:int_primary_exp(1) -> */
	T_INT_VAR, 
	/* 93:int_primary_exp(3) -> */
	T_INT_FUN, N_FUNCTION_CALL, A_OP_CALL_INT_RET, 
	/* 94:int_primary_exp(4) -> */
	T_FIX2INT, T_LPAREN, N_FIX_EXP, T_RPAREN, 
	/* 95:int_primary_exp(4) -> */
	T_BOOL2INT, T_LPAREN, N_FIX_EXP, T_RPAREN, 
	/* 96:int_primary_exp(3) -> */
	T_LPAREN, N_INT_EXP, T_RPAREN, 
	/* 97:fix_var_list(2) -> */
	N_FIX_VAR, N_FIX_VAR_LIST1, 
	/* 98:fix_var_list1(0) -> */
	
	/* 99:fix_var_list1(2) -> */
	T_COMMA, N_FIX_VAR_LIST, 
	/* 100:fix_var(4) -> */
	T_ID, A_OP_VAR_DECL, A_OP_PUSH_TMP, N_FIX_VAR_ASSIGN_OPT, 
	/* 101:fix_var_assign_opt(1) -> */
	A_OP_POP_TMP, 
	/* 102:fix_var_assign_opt(3) -> */
	T_EQUALS, N_FIX_EXP, A_OP_FIX_ASSIGN, 
	/* 103:fix_rel_exp(2) -> */
	N_FIX_EXP, N_FIX_REL_EXP1, 
	/* 104:fix_rel_exp1(3) -> */
	T_LESSTHEN, N_FIX_EXP, A_OP_LT, 
	/* 105:fix_rel_exp1(3) -> */
	T_GREATTHEN, N_FIX_EXP, A_OP_GT, 
	/* 106:fix_rel_exp1(3) -> */
	T_EQU, N_FIX_EXP, A_OP_EQU, 
	/* 107:fix_rel_exp1(3) -> */
	T_NEQ, N_FIX_EXP, A_OP_NEQ, 
	/* 108:fix_rel_exp1(3) -> */
	T_GTE, N_FIX_EXP, A_OP_GTE, 
	/* 109:fix_rel_exp1(3) -> */
	T_LTE, N_FIX_EXP, A_OP_LTE, 
	/* 110:fix_exp(1) -> */
	N_FIX_ADD_EXP, 
	/* 111:fix_add_exp(2) -> */
	N_FIX_MULT_EXP, N_FIX_ADD_EXP1, 
	/* 112:fix_add_exp1(0) -> */
	
	/* 113:fix_add_exp1(3) -> */
	T_PLUS, N_FIX_ADD_EXP, A_OP_ADD_FIX, 
	/* 114:fix_add_exp1(3) -> */
	T_MINUS, N_FIX_ADD_EXP, A_OP_SUB_FIX, 
	/* 115:fix_mult_exp(2) -> */
	N_FIX_UNARY_EXP, N_FIX_MULT_EXP1, 
	/* 116:fix_mult_exp1(0) -> */
	
	/* 117:fix_mult_exp1(3) -> */
	T_SLASH, N_FIX_MULT_EXP, A_OP_DIV_FIX, 
	/* 118:fix_mult_exp1(3) -> */
	T_STAR, N_FIX_MULT_EXP, A_OP_MUL_FIX, 
	/* 119:fix_unary_exp(1) -> */
	N_FIX_PRIMARY_EXP, 
	/* 120:fix_unary_exp(3) -> */
	T_MINUS, N_FIX_PRIMARY_EXP, A_OP_MINUS_FIX, 
	/* 121:fix_primary_exp(2) -> */
	T_FIX_VAL, A_OP_PUSH_FIX, 
	/* 122:fix_primary_exp(1) -> */
	T_FIX_VAR, 
	/* 123:fix_primary_exp(3) -> */
	T_FIX_FUN, N_FUNCTION_CALL, A_OP_CALL_FIX_RET, 
	/* 124:fix_primary_exp(3) -> */
	T_INT_VAL, A_OP_PUSH_INT, A_OP_INT2FIX, 
	/* 125:fix_primary_exp(3) -> */
	T_CHAR, A_OP_PUSH_INT, A_OP_INT2FIX, 
	/* 126:fix_primary_exp(2) -> */
	T_INT_VAR, A_OP_INT2FIX, 
	/* 127:fix_primary_exp(4) -> */
	T_INT_FUN, N_FUNCTION_CALL, A_OP_CALL_INT_RET, A_OP_INT2FIX, 
	/* 128:fix_primary_exp(3) -> */
	T_LPAREN, N_FIX_EXP, T_RPAREN, 
};
/* 322 bytes */

static const struct {
	uint16_t off;
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
	{  48,  3},
	{  51,  3},
	{  54,  5},
	{  59,  4},
	{  63,  5},
	{  68,  4},
	{  72,  5},
	{  77,  4},
	{  81,  5},
	{  86,  0},
	{  86,  3},
	{  89,  1},
	{  90, 14},
	{ 104,  3},
	{ 107,  0},
	{ 107,  1},
	{ 108,  3},
	{ 111,  0},
	{ 111,  2},
	{ 113,  1},
	{ 114,  0},
	{ 114,  1},
	{ 115,  2},
	{ 117,  0},
	{ 117,  2},
	{ 119,  2},
	{ 121,  0},
	{ 121,  2},
	{ 123,  4},
	{ 127,  1},
	{ 128,  3},
	{ 131,  2},
	{ 133,  0},
	{ 133,  3},
	{ 136,  2},
	{ 138,  0},
	{ 138,  3},
	{ 141,  1},
	{ 142,  3},
	{ 145,  2},
	{ 147,  2},
	{ 149,  3},
	{ 152,  1},
	{ 153,  2},
	{ 155,  0},
	{ 155,  2},
	{ 157,  4},
	{ 161,  1},
	{ 162,  3},
	{ 165,  2},
	{ 167,  3},
	{ 170,  3},
	{ 173,  3},
	{ 176,  3},
	{ 179,  3},
	{ 182,  3},
	{ 185,  2},
	{ 187,  0},
	{ 187,  3},
	{ 190,  3},
	{ 193,  2},
	{ 195,  0},
	{ 195,  3},
	{ 198,  2},
	{ 200,  0},
	{ 200,  3},
	{ 203,  3},
	{ 206,  2},
	{ 208,  0},
	{ 208,  3},
	{ 211,  3},
	{ 214,  2},
	{ 216,  0},
	{ 216,  3},
	{ 219,  3},
	{ 222,  3},
	{ 225,  1},
	{ 226,  3},
	{ 229,  2},
	{ 231,  2},
	{ 233,  1},
	{ 234,  3},
	{ 237,  4},
	{ 241,  4},
	{ 245,  3},
	{ 248,  2},
	{ 250,  0},
	{ 250,  2},
	{ 252,  4},
	{ 256,  1},
	{ 257,  3},
	{ 260,  2},
	{ 262,  3},
	{ 265,  3},
	{ 268,  3},
	{ 271,  3},
	{ 274,  3},
	{ 277,  3},
	{ 280,  1},
	{ 281,  2},
	{ 283,  0},
	{ 283,  3},
	{ 286,  3},
	{ 289,  2},
	{ 291,  0},
	{ 291,  3},
	{ 294,  3},
	{ 297,  1},
	{ 298,  3},
	{ 301,  2},
	{ 303,  1},
	{ 304,  3},
	{ 307,  3},
	{ 310,  3},
	{ 313,  2},
	{ 315,  4},
	{ 319,  3},
};
/* 516 bytes */

/* Total: 1832 bytes */


int types_ll_push(uint8_t * sp, unsigned int sym, unsigned int tok)
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

int types_ll_start(uint8_t * sp)
{
	sp[-1] = T_EOF;
	sp[-2] = N_PROGRAM;

	return 2;
}

const const char * const types_ll_sym[] = {
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
 	"INT_VAL",
 	"CHAR",
 	"STRING",
 	"ERR",
 	"BOOL",
 	"INT",
 	"FIX",
 	"INT_VAR",
 	"INT_FUN",
 	"FIX_VAR",
 	"FIX_FUN",
 	"BOOL_VAR",
 	"BOOL_FUN",
 	"NOT",
 	"FIX2INT",
 	"BOOL2INT",
 	"FIX_VAL",
 	"program",
 	"stat",
 	"compound_stat",
 	"stat_list",
 	"for_lst_opt",
 	"int_exp",
 	"condition",
 	"else_opt",
 	"catch_opt",
 	"bool_var_list",
 	"int_var_list",
 	"fix_var_list",
 	"assign_or_call",
 	"function_call",
 	"fix_exp",
 	"bool_exp",
 	"arg_list_opt",
 	"arg_list",
 	"exp",
 	"arg_list1",
 	"for_lst",
 	"for_lst1",
 	"bool_var",
 	"bool_var_list1",
 	"bool_var_assign_opt",
 	"bool_and_exp",
 	"bool_or_exp",
 	"bool_unary_exp",
 	"bool_and_exp1",
 	"bool_pri_exp",
 	"rel_exp",
 	"int_rel_exp",
 	"int_var",
 	"int_var_list1",
 	"int_var_assign_opt",
 	"int_rel_exp1",
 	"and_exp",
 	"or_exp",
 	"shift_exp",
 	"and_exp1",
 	"int_add_exp",
 	"shift_exp1",
 	"int_mult_exp",
 	"int_add_exp1",
 	"int_unary_exp",
 	"int_mult_exp1",
 	"int_primary_exp",
 	"fix_var",
 	"fix_var_list1",
 	"fix_var_assign_opt",
 	"fix_rel_exp",
 	"fix_rel_exp1",
 	"fix_add_exp",
 	"fix_mult_exp",
 	"fix_add_exp1",
 	"fix_unary_exp",
 	"fix_mult_exp1",
 	"fix_primary_exp",
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
 	"op_int_assign",
 	"op_ret_discard",
 	"op_fix_assign",
 	"op_bool_assign",
 	"op_method",
 	"op_call",
 	"op_if_else",
 	"op_try_end",
 	"op_catch",
 	"op_int_var_decl",
 	"op_push_tmp",
 	"op_assign",
 	"op_catch_end",
 	"op_arg",
 	"op_var_decl",
 	"op_pop_tmp",
 	"op_logic_or",
 	"op_logic_and",
 	"op_not",
 	"op_push_true",
 	"op_push_false",
 	"op_lt",
 	"op_gt",
 	"op_equ",
 	"op_neq",
 	"op_gte",
 	"op_lte",
 	"op_or",
 	"op_xor",
 	"op_and",
 	"op_shl",
 	"op_asr",
 	"op_add",
 	"op_sub",
 	"op_div",
 	"op_mod",
 	"op_mul",
 	"op_inv",
 	"op_push_int",
 	"op_call_int_ret",
 	"op_add_fix",
 	"op_sub_fix",
 	"op_div_fix",
 	"op_mul_fix",
 	"op_minus_fix",
 	"op_push_fix",
 	"op_call_fix_ret",
 	"op_int2fix",
 };

