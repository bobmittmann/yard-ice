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
	{  3, 42}, { 22, 42}, { 48, 43}, 
	/*  59 (exp) */
	{  6, 47}, { 13, 47}, { 21, 47}, { 37, 47}, { 44, 47}, { 48, 47}, 
	{ 49, 47}, { 50, 47}, { 51, 47}, { 53, 47}, 
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
	{ 21, 36}, 
	/*  70 (object_assign) */
	{  1, 30}, { 16, 29}, 
	/*  71 (attr_assign) */
	{ 48, 31}, 
	/*  72 (attr_assign2) */
	{ 16, 32}, { 19, 33}, { 21, 34}, 
	/*  73 (method_call) */
	{ 21, 35}, 
	/*  74 (arg_list_opt) */
	{  6, 38}, { 13, 38}, { 21, 38}, { 22, 37}, { 37, 38}, { 44, 38}, 
	{ 48, 38}, { 49, 38}, { 50, 38}, { 51, 38}, { 53, 38}, 
	/*  75 (arg_list) */
	{  6, 39}, { 13, 39}, { 21, 39}, { 37, 39}, { 44, 39}, { 48, 39}, 
	{ 49, 39}, { 50, 39}, { 51, 39}, { 53, 39}, 
	/*  76 (arg_list1) */
	{  2, 41}, { 22, 40}, 
	/*  77 (exp_lst) */
	{ 48, 44}, 
	/*  78 (exp_lst1) */
	{  2, 46}, {  3, 45}, { 22, 45}, 
	/*  79 (and_exp) */
	{  6, 52}, { 13, 52}, { 21, 52}, { 37, 52}, { 44, 52}, { 48, 52}, 
	{ 49, 52}, { 50, 52}, { 51, 52}, { 53, 52}, 
	/*  80 (or_exp) */
	{  2, 48}, {  3, 48}, { 11, 49}, { 12, 51}, { 20, 48}, { 22, 48}, 
	{ 31, 50}, 
	/*  81 (relational_exp) */
	{  6, 56}, { 13, 56}, { 21, 56}, { 37, 56}, { 44, 56}, { 48, 56}, 
	{ 49, 56}, { 50, 56}, { 51, 56}, { 53, 56}, 
	/*  82 (and_exp1) */
	{  2, 53}, {  3, 53}, { 10, 54}, { 11, 53}, { 12, 53}, { 20, 53}, 
	{ 22, 53}, { 31, 53}, { 32, 55}, 
	/*  83 (shift_exp) */
	{  6, 64}, { 13, 64}, { 21, 64}, { 37, 64}, { 44, 64}, { 48, 64}, 
	{ 49, 64}, { 50, 64}, { 51, 64}, { 53, 64}, 
	/*  84 (relational_exp1) */
	{  2, 57}, {  3, 57}, { 10, 57}, { 11, 57}, { 12, 57}, { 17, 58}, 
	{ 18, 59}, { 20, 57}, { 22, 57}, { 25, 62}, { 26, 63}, { 27, 60}, 
	{ 28, 61}, { 31, 57}, { 32, 57}, 
	/*  85 (additive_exp) */
	{  6, 68}, { 13, 68}, { 21, 68}, { 37, 68}, { 44, 68}, { 48, 68}, 
	{ 49, 68}, { 50, 68}, { 51, 68}, { 53, 68}, 
	/*  86 (shift_exp1) */
	{  2, 65}, {  3, 65}, { 10, 65}, { 11, 65}, { 12, 65}, { 17, 65}, 
	{ 18, 65}, { 20, 65}, { 22, 65}, { 25, 65}, { 26, 65}, { 27, 65}, 
	{ 28, 65}, { 29, 67}, { 30, 66}, { 31, 65}, { 32, 65}, 
	/*  87 (div_exp) */
	{  6, 72}, { 13, 72}, { 21, 72}, { 37, 72}, { 44, 72}, { 48, 72}, 
	{ 49, 72}, { 50, 72}, { 51, 72}, { 53, 72}, 
	/*  88 (additive_exp1) */
	{  2, 69}, {  3, 69}, {  5, 70}, {  6, 71}, { 10, 69}, { 11, 69}, 
	{ 12, 69}, { 17, 69}, { 18, 69}, { 20, 69}, { 22, 69}, { 25, 69}, 
	{ 26, 69}, { 27, 69}, { 28, 69}, { 29, 69}, { 30, 69}, { 31, 69}, 
	{ 32, 69}, 
	/*  89 (mult_exp) */
	{  6, 76}, { 13, 76}, { 21, 76}, { 37, 76}, { 44, 76}, { 48, 76}, 
	{ 49, 76}, { 50, 76}, { 51, 76}, { 53, 76}, 
	/*  90 (div_exp1) */
	{  2, 73}, {  3, 73}, {  5, 73}, {  6, 73}, {  8, 74}, {  9, 75}, 
	{ 10, 73}, { 11, 73}, { 12, 73}, { 17, 73}, { 18, 73}, { 20, 73}, 
	{ 22, 73}, { 25, 73}, { 26, 73}, { 27, 73}, { 28, 73}, { 29, 73}, 
	{ 30, 73}, { 31, 73}, { 32, 73}, 
	/*  91 (unary_exp) */
	{  6, 81}, { 13, 80}, { 21, 79}, { 37, 79}, { 44, 79}, { 48, 79}, 
	{ 49, 79}, { 50, 79}, { 51, 79}, { 53, 82}, 
	/*  92 (mult_exp1) */
	{  2, 77}, {  3, 77}, {  5, 77}, {  6, 77}, {  7, 78}, {  8, 77}, 
	{  9, 77}, { 10, 77}, { 11, 77}, { 12, 77}, { 17, 77}, { 18, 77}, 
	{ 20, 77}, { 22, 77}, { 25, 77}, { 26, 77}, { 27, 77}, { 28, 77}, 
	{ 29, 77}, { 30, 77}, { 31, 77}, { 32, 77}, 
	/*  93 (primary_exp) */
	{ 21, 83}, { 37, 88}, { 44, 87}, { 48, 89}, { 49, 84}, { 50, 85}, 
	{ 51, 86}, 
	/*  94 (id_eval) */
	{ 48, 90}, 
	/*  95 (id_eval1) */
	{  1, 94}, {  2, 91}, {  3, 91}, {  5, 91}, {  6, 91}, {  7, 91}, 
	{  8, 91}, {  9, 91}, { 10, 91}, { 11, 91}, { 12, 91}, { 17, 91}, 
	{ 18, 91}, { 19, 93}, { 20, 91}, { 21, 92}, { 22, 91}, { 25, 91}, 
	{ 26, 91}, { 27, 91}, { 28, 91}, { 29, 91}, { 30, 91}, { 31, 91}, 
	{ 32, 91}, 
	/*  96 (object_eval) */
	{  1, 96}, {  2, 95}, {  3, 95}, {  5, 95}, {  6, 95}, {  7, 95}, 
	{  8, 95}, {  9, 95}, { 10, 95}, { 11, 95}, { 12, 95}, { 17, 95}, 
	{ 18, 95}, { 20, 95}, { 22, 95}, { 25, 95}, { 26, 95}, { 27, 95}, 
	{ 28, 95}, { 29, 95}, { 30, 95}, { 31, 95}, { 32, 95}, 
	/*  97 (attr_eval) */
	{ 48, 97}, 
	/*  98 (attr_eval1) */
	{  2, 98}, {  3, 98}, {  5, 98}, {  6, 98}, {  7, 98}, {  8, 98}, 
	{  9, 98}, { 10, 98}, { 11, 98}, { 12, 98}, { 17, 98}, { 18, 98}, 
	{ 19,100}, { 20, 98}, { 21, 99}, { 22, 98}, { 25, 98}, { 26, 98}, 
	{ 27, 98}, { 28, 98}, { 29, 98}, { 30, 98}, { 31, 98}, { 32, 98}, 
};
/* 738 bytes; */

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
	{  77,  3},
	{  80,  1},
	{  81, 11},
	{  92, 10},
	{ 102,  2},
	{ 104,  1},
	{ 105,  3},
	{ 108, 10},
	{ 118,  7},
	{ 125, 10},
	{ 135,  9},
	{ 144, 10},
	{ 154, 15},
	{ 169, 10},
	{ 179, 17},
	{ 196, 10},
	{ 206, 19},
	{ 225, 10},
	{ 235, 21},
	{ 256, 10},
	{ 266, 22},
	{ 288,  7},
	{ 295,  1},
	{ 296, 25},
	{ 321, 23},
	{ 344,  1},
	{ 345, 24},
};
/* 180 bytes */

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
	/* 31:attr_assign(3) -> */
	T_ID, A_OP_PUSH_TMP, N_ATTR_ASSIGN2, 
	/* 32:attr_assign2(3) -> */
	T_EQUALS, N_EXP, A_OP_ATTR_ASSIGN, 
	/* 33:attr_assign2(6) -> */
	T_LBRACKET, N_EXP, T_RBRACKET, T_EQUALS, N_EXP, A_OP_ARRAY_ASSIGN, 
	/* 34:attr_assign2(2) -> */
	N_METHOD_CALL, A_OP_RET_DISCARD, 
	/* 35:method_call(5) -> */
	A_OP_METHOD_LOOKUP, T_LPAREN, N_ARG_LIST_OPT, T_RPAREN, A_OP_CALL, 
	/* 36:function_call(5) -> */
	A_OP_FUNCTION_LOOKUP, T_LPAREN, N_ARG_LIST_OPT, T_RPAREN, A_OP_CALL, 
	/* 37:arg_list_opt(0) -> */
	
	/* 38:arg_list_opt(1) -> */
	N_ARG_LIST, 
	/* 39:arg_list(3) -> */
	N_EXP, A_OP_ARG, N_ARG_LIST1, 
	/* 40:arg_list1(0) -> */
	
	/* 41:arg_list1(2) -> */
	T_COMMA, N_ARG_LIST, 
	/* 42:exp_lst_opt(0) -> */
	
	/* 43:exp_lst_opt(1) -> */
	N_EXP_LST, 
	/* 44:exp_lst(2) -> */
	N_ASSIGN_OR_CALL, N_EXP_LST1, 
	/* 45:exp_lst1(0) -> */
	
	/* 46:exp_lst1(2) -> */
	T_COMMA, N_EXP_LST, 
	/* 47:exp(2) -> */
	N_AND_EXP, N_OR_EXP, 
	/* 48:or_exp(0) -> */
	
	/* 49:or_exp(3) -> */
	T_BAR, N_EXP, A_OP_OR, 
	/* 50:or_exp(3) -> */
	T_LOGICOR, N_EXP, A_OP_LOGIC_OR, 
	/* 51:or_exp(3) -> */
	T_CARET, N_EXP, A_OP_XOR, 
	/* 52:and_exp(2) -> */
	N_RELATIONAL_EXP, N_AND_EXP1, 
	/* 53:and_exp1(0) -> */
	
	/* 54:and_exp1(3) -> */
	T_AMPERSAND, N_AND_EXP, A_OP_AND, 
	/* 55:and_exp1(3) -> */
	T_LOGICAND, N_AND_EXP, A_OP_LOGIC_AND, 
	/* 56:relational_exp(2) -> */
	N_SHIFT_EXP, N_RELATIONAL_EXP1, 
	/* 57:relational_exp1(0) -> */
	
	/* 58:relational_exp1(3) -> */
	T_LESSTHEN, N_RELATIONAL_EXP, A_OP_LT, 
	/* 59:relational_exp1(3) -> */
	T_GREATTHEN, N_RELATIONAL_EXP, A_OP_GT, 
	/* 60:relational_exp1(3) -> */
	T_EQU, N_RELATIONAL_EXP, A_OP_EQU, 
	/* 61:relational_exp1(3) -> */
	T_NEQ, N_RELATIONAL_EXP, A_OP_NEQ, 
	/* 62:relational_exp1(3) -> */
	T_GTE, N_RELATIONAL_EXP, A_OP_GTE, 
	/* 63:relational_exp1(3) -> */
	T_LTE, N_RELATIONAL_EXP, A_OP_LTE, 
	/* 64:shift_exp(2) -> */
	N_ADDITIVE_EXP, N_SHIFT_EXP1, 
	/* 65:shift_exp1(0) -> */
	
	/* 66:shift_exp1(3) -> */
	T_SHL, N_SHIFT_EXP, A_OP_SHL, 
	/* 67:shift_exp1(3) -> */
	T_ASR, N_SHIFT_EXP, A_OP_ASR, 
	/* 68:additive_exp(2) -> */
	N_DIV_EXP, N_ADDITIVE_EXP1, 
	/* 69:additive_exp1(0) -> */
	
	/* 70:additive_exp1(3) -> */
	T_PLUS, N_ADDITIVE_EXP, A_OP_ADD, 
	/* 71:additive_exp1(3) -> */
	T_MINUS, N_ADDITIVE_EXP, A_OP_SUB, 
	/* 72:div_exp(2) -> */
	N_MULT_EXP, N_DIV_EXP1, 
	/* 73:div_exp1(0) -> */
	
	/* 74:div_exp1(3) -> */
	T_SLASH, N_DIV_EXP, A_OP_DIV, 
	/* 75:div_exp1(3) -> */
	T_PERCENT, N_DIV_EXP, A_OP_MOD, 
	/* 76:mult_exp(2) -> */
	N_UNARY_EXP, N_MULT_EXP1, 
	/* 77:mult_exp1(0) -> */
	
	/* 78:mult_exp1(3) -> */
	T_STAR, N_MULT_EXP, A_OP_MUL, 
	/* 79:unary_exp(1) -> */
	N_PRIMARY_EXP, 
	/* 80:unary_exp(3) -> */
	T_TILDE, N_UNARY_EXP, A_OP_INV, 
	/* 81:unary_exp(3) -> */
	T_MINUS, N_UNARY_EXP, A_OP_MINUS, 
	/* 82:unary_exp(3) -> */
	T_NOT, N_UNARY_EXP, A_OP_NOT, 
	/* 83:primary_exp(3) -> */
	T_LPAREN, N_EXP, T_RPAREN, 
	/* 84:primary_exp(2) -> */
	T_INT, A_OP_PUSH_INT, 
	/* 85:primary_exp(2) -> */
	T_CHAR, A_OP_PUSH_INT, 
	/* 86:primary_exp(2) -> */
	T_STRING, A_OP_PUSH_STRING, 
	/* 87:primary_exp(2) -> */
	T_TRUE, A_OP_PUSH_TRUE, 
	/* 88:primary_exp(2) -> */
	T_FALSE, A_OP_PUSH_FALSE, 
	/* 89:primary_exp(1) -> */
	N_ID_EVAL, 
	/* 90:id_eval(3) -> */
	T_ID, A_OP_PUSH_TMP, N_ID_EVAL1, 
	/* 91:id_eval1(1) -> */
	A_OP_VAR_EVAL, 
	/* 92:id_eval1(2) -> */
	N_FUNCTION_CALL, A_OP_CALL_RET, 
	/* 93:id_eval1(5) -> */
	T_LBRACKET, N_EXP, T_RBRACKET, A_OP_ARRAY_XLAT, N_OBJECT_EVAL, 
	/* 94:id_eval1(2) -> */
	T_DOT, N_ATTR_EVAL, 
	/* 95:object_eval(1) -> */
	A_OP_OBJECT_EVAL, 
	/* 96:object_eval(2) -> */
	T_DOT, N_ATTR_EVAL, 
	/* 97:attr_eval(3) -> */
	T_ID, A_OP_PUSH_TMP, N_ATTR_EVAL1, 
	/* 98:attr_eval1(1) -> */
	A_OP_ATTR_EVAL, 
	/* 99:attr_eval1(2) -> */
	N_METHOD_CALL, A_OP_CALL_RET, 
	/* 100:attr_eval1(4) -> */
	T_LBRACKET, N_EXP, T_RBRACKET, A_OP_ARRAY_EVAL, 
};
/* 256 bytes */

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
	{ 103,  3},
	{ 106,  3},
	{ 109,  6},
	{ 115,  2},
	{ 117,  5},
	{ 122,  5},
	{ 127,  0},
	{ 127,  1},
	{ 128,  3},
	{ 131,  0},
	{ 131,  2},
	{ 133,  0},
	{ 133,  1},
	{ 134,  2},
	{ 136,  0},
	{ 136,  2},
	{ 138,  2},
	{ 140,  0},
	{ 140,  3},
	{ 143,  3},
	{ 146,  3},
	{ 149,  2},
	{ 151,  0},
	{ 151,  3},
	{ 154,  3},
	{ 157,  2},
	{ 159,  0},
	{ 159,  3},
	{ 162,  3},
	{ 165,  3},
	{ 168,  3},
	{ 171,  3},
	{ 174,  3},
	{ 177,  2},
	{ 179,  0},
	{ 179,  3},
	{ 182,  3},
	{ 185,  2},
	{ 187,  0},
	{ 187,  3},
	{ 190,  3},
	{ 193,  2},
	{ 195,  0},
	{ 195,  3},
	{ 198,  3},
	{ 201,  2},
	{ 203,  0},
	{ 203,  3},
	{ 206,  1},
	{ 207,  3},
	{ 210,  3},
	{ 213,  3},
	{ 216,  3},
	{ 219,  2},
	{ 221,  2},
	{ 223,  2},
	{ 225,  2},
	{ 227,  2},
	{ 229,  1},
	{ 230,  3},
	{ 233,  1},
	{ 234,  2},
	{ 236,  5},
	{ 241,  2},
	{ 243,  1},
	{ 244,  2},
	{ 246,  3},
	{ 249,  1},
	{ 250,  2},
	{ 252,  4},
};
/* 202 bytes */

/* Total: 1376 bytes */


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

