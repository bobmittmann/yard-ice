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
	{  0,  0}, { 23,  3}, { 36,  1}, { 37,  1}, { 38,  2}, { 40,  1}, 
	{ 41,  2}, { 42,  1}, { 43,  2}, 
	/*  50 (stat) */
	{ 36, 88}, { 37, 90}, { 40, 91}, { 42, 89}, 
	/*  51 (semi_stat) */
	{ 38, 93}, { 41, 94}, { 43, 95}, 
	/*  52 (semi_list) */
	{  0,  4}, {  3,  5}, 
	/*  53 (compound_stat) */
	{ 23, 84}, 
	/*  54 (var_list) */
	{ 43,  6}, 
	/*  55 (var) */
	{ 43,  9}, 
	/*  56 (var_list1) */
	{  0,  7}, {  2,  8}, {  3,  7}, 
	/*  57 (var_assign_opt) */
	{  0, 10}, {  2, 10}, {  3, 10}, { 16, 11}, 
	/*  58 (exp) */
	{  6, 32}, { 13, 32}, { 21, 32}, { 35, 32}, { 39, 32}, { 43, 32}, 
	{ 44, 32}, { 45, 32}, { 46, 32}, { 48, 32}, 
	/*  59 (assign_or_call1) */
	{  1, 15}, { 16, 12}, { 19, 14}, { 21, 13}, 
	/*  60 (function_call) */
	{ 21, 21}, 
	/*  61 (attr_assign) */
	{ 43, 16}, 
	/*  62 (attr_assign2) */
	{ 16, 17}, { 19, 18}, { 21, 19}, 
	/*  63 (method_call) */
	{ 21, 20}, 
	/*  64 (arg_list_opt) */
	{  6, 23}, { 13, 23}, { 21, 23}, { 22, 22}, { 35, 23}, { 39, 23}, 
	{ 43, 23}, { 44, 23}, { 45, 23}, { 46, 23}, { 48, 23}, 
	/*  65 (arg_list) */
	{  6, 24}, { 13, 24}, { 21, 24}, { 35, 24}, { 39, 24}, { 43, 24}, 
	{ 44, 24}, { 45, 24}, { 46, 24}, { 48, 24}, 
	/*  66 (arg_list1) */
	{  2, 26}, { 22, 25}, 
	/*  67 (exp_lst_opt) */
	{  3, 27}, { 22, 27}, { 43, 28}, 
	/*  68 (exp_lst) */
	{ 43, 29}, 
	/*  69 (exp_lst1) */
	{  2, 31}, {  3, 30}, { 22, 30}, 
	/*  70 (and_exp) */
	{  6, 37}, { 13, 37}, { 21, 37}, { 35, 37}, { 39, 37}, { 43, 37}, 
	{ 44, 37}, { 45, 37}, { 46, 37}, { 48, 37}, 
	/*  71 (or_exp) */
	{  0, 33}, {  2, 33}, {  3, 33}, { 11, 34}, { 12, 36}, { 20, 33}, 
	{ 22, 33}, { 31, 35}, 
	/*  72 (relational_exp) */
	{  6, 41}, { 13, 41}, { 21, 41}, { 35, 41}, { 39, 41}, { 43, 41}, 
	{ 44, 41}, { 45, 41}, { 46, 41}, { 48, 41}, 
	/*  73 (and_exp1) */
	{  0, 38}, {  2, 38}, {  3, 38}, { 10, 39}, { 11, 38}, { 12, 38}, 
	{ 20, 38}, { 22, 38}, { 31, 38}, { 32, 40}, 
	/*  74 (shift_exp) */
	{  6, 49}, { 13, 49}, { 21, 49}, { 35, 49}, { 39, 49}, { 43, 49}, 
	{ 44, 49}, { 45, 49}, { 46, 49}, { 48, 49}, 
	/*  75 (relational_exp1) */
	{  0, 42}, {  2, 42}, {  3, 42}, { 10, 42}, { 11, 42}, { 12, 42}, 
	{ 17, 43}, { 18, 44}, { 20, 42}, { 22, 42}, { 25, 47}, { 26, 48}, 
	{ 27, 45}, { 28, 46}, { 31, 42}, { 32, 42}, 
	/*  76 (additive_exp) */
	{  6, 53}, { 13, 53}, { 21, 53}, { 35, 53}, { 39, 53}, { 43, 53}, 
	{ 44, 53}, { 45, 53}, { 46, 53}, { 48, 53}, 
	/*  77 (shift_exp1) */
	{  0, 50}, {  2, 50}, {  3, 50}, { 10, 50}, { 11, 50}, { 12, 50}, 
	{ 17, 50}, { 18, 50}, { 20, 50}, { 22, 50}, { 25, 50}, { 26, 50}, 
	{ 27, 50}, { 28, 50}, { 29, 52}, { 30, 51}, { 31, 50}, { 32, 50}, 
	/*  78 (div_exp) */
	{  6, 57}, { 13, 57}, { 21, 57}, { 35, 57}, { 39, 57}, { 43, 57}, 
	{ 44, 57}, { 45, 57}, { 46, 57}, { 48, 57}, 
	/*  79 (additive_exp1) */
	{  0, 54}, {  2, 54}, {  3, 54}, {  5, 55}, {  6, 56}, { 10, 54}, 
	{ 11, 54}, { 12, 54}, { 17, 54}, { 18, 54}, { 20, 54}, { 22, 54}, 
	{ 25, 54}, { 26, 54}, { 27, 54}, { 28, 54}, { 29, 54}, { 30, 54}, 
	{ 31, 54}, { 32, 54}, 
	/*  80 (mult_exp) */
	{  6, 61}, { 13, 61}, { 21, 61}, { 35, 61}, { 39, 61}, { 43, 61}, 
	{ 44, 61}, { 45, 61}, { 46, 61}, { 48, 61}, 
	/*  81 (div_exp1) */
	{  0, 58}, {  2, 58}, {  3, 58}, {  5, 58}, {  6, 58}, {  8, 59}, 
	{  9, 60}, { 10, 58}, { 11, 58}, { 12, 58}, { 17, 58}, { 18, 58}, 
	{ 20, 58}, { 22, 58}, { 25, 58}, { 26, 58}, { 27, 58}, { 28, 58}, 
	{ 29, 58}, { 30, 58}, { 31, 58}, { 32, 58}, 
	/*  82 (unary_exp) */
	{  6, 66}, { 13, 65}, { 21, 64}, { 35, 64}, { 39, 64}, { 43, 64}, 
	{ 44, 64}, { 45, 64}, { 46, 64}, { 48, 67}, 
	/*  83 (mult_exp1) */
	{  0, 62}, {  2, 62}, {  3, 62}, {  5, 62}, {  6, 62}, {  7, 63}, 
	{  8, 62}, {  9, 62}, { 10, 62}, { 11, 62}, { 12, 62}, { 17, 62}, 
	{ 18, 62}, { 20, 62}, { 22, 62}, { 25, 62}, { 26, 62}, { 27, 62}, 
	{ 28, 62}, { 29, 62}, { 30, 62}, { 31, 62}, { 32, 62}, 
	/*  84 (primary_exp) */
	{ 21, 68}, { 35, 73}, { 39, 72}, { 43, 74}, { 44, 69}, { 45, 70}, 
	{ 46, 71}, 
	/*  85 (id_eval) */
	{ 43, 75}, 
	/*  86 (id_eval1) */
	{  0, 76}, {  1, 79}, {  2, 76}, {  3, 76}, {  5, 76}, {  6, 76}, 
	{  7, 76}, {  8, 76}, {  9, 76}, { 10, 76}, { 11, 76}, { 12, 76}, 
	{ 17, 76}, { 18, 76}, { 19, 78}, { 20, 76}, { 21, 77}, { 22, 76}, 
	{ 25, 76}, { 26, 76}, { 27, 76}, { 28, 76}, { 29, 76}, { 30, 76}, 
	{ 31, 76}, { 32, 76}, 
	/*  87 (attr_eval) */
	{ 43, 80}, 
	/*  88 (attr_eval1) */
	{  0, 81}, {  2, 81}, {  3, 81}, {  5, 81}, {  6, 81}, {  7, 81}, 
	{  8, 81}, {  9, 81}, { 10, 81}, { 11, 81}, { 12, 81}, { 17, 81}, 
	{ 18, 81}, { 19, 83}, { 20, 81}, { 21, 82}, { 22, 81}, { 25, 81}, 
	{ 26, 81}, { 27, 81}, { 28, 81}, { 29, 81}, { 30, 81}, { 31, 81}, 
	{ 32, 81}, 
	/*  89 (stat_list) */
	{ 24, 85}, { 36, 86}, { 37, 86}, { 38, 87}, { 40, 86}, { 41, 87}, 
	{ 42, 86}, { 43, 87}, 
	/*  90 (condition) */
	{ 21, 92}, 
	/*  91 (else_opt) */
	{  0, 96}, { 23, 96}, { 24, 96}, { 34, 97}, { 36, 96}, { 37, 96}, 
	{ 38, 96}, { 40, 96}, { 41, 96}, { 42, 96}, { 43, 96}, 
	/*  92 (catch_opt) */
	{  0, 98}, { 23, 98}, { 24, 98}, { 33, 99}, { 36, 98}, { 37, 98}, 
	{ 38, 98}, { 40, 98}, { 41, 98}, { 42, 98}, { 43, 98}, 
};
/* 712 bytes; */

static const struct {
	uint16_t off;
	uint8_t cnt;
} predict_idx[] = {
	{   0,  9},
	{   9,  4},
	{  13,  3},
	{  16,  2},
	{  18,  1},
	{  19,  1},
	{  20,  1},
	{  21,  3},
	{  24,  4},
	{  28, 10},
	{  38,  4},
	{  42,  1},
	{  43,  1},
	{  44,  3},
	{  47,  1},
	{  48, 11},
	{  59, 10},
	{  69,  2},
	{  71,  3},
	{  74,  1},
	{  75,  3},
	{  78, 10},
	{  88,  8},
	{  96, 10},
	{ 106, 10},
	{ 116, 10},
	{ 126, 16},
	{ 142, 10},
	{ 152, 18},
	{ 170, 10},
	{ 180, 20},
	{ 200, 10},
	{ 210, 22},
	{ 232, 10},
	{ 242, 23},
	{ 265,  7},
	{ 272,  1},
	{ 273, 26},
	{ 299,  1},
	{ 300, 25},
	{ 325,  8},
	{ 333,  1},
	{ 334, 11},
	{ 345, 11},
};
/* 176 bytes */

/* Rules vectors table */
static const uint8_t rule_vec[] = {
	/*  0:program(1) -> */
	T_EOF, 
	/*  1:program(2) -> */
	N_STAT, N_PROGRAM, 
	/*  2:program(2) -> */
	N_SEMI_STAT, N_SEMI_LIST, 
	/*  3:program(2) -> */
	N_COMPOUND_STAT, N_PROGRAM, 
	/*  4:semi_list(1) -> */
	T_EOF, 
	/*  5:semi_list(2) -> */
	T_SEMICOLON, N_PROGRAM, 
	/*  6:var_list(2) -> */
	N_VAR, N_VAR_LIST1, 
	/*  7:var_list1(0) -> */
	
	/*  8:var_list1(2) -> */
	T_COMMA, N_VAR_LIST, 
	/*  9:var(4) -> */
	T_ID, A_OP_VAR_DECL, A_OP_PUSH_TMP, N_VAR_ASSIGN_OPT, 
	/* 10:var_assign_opt(1) -> */
	A_OP_POP_TMP, 
	/* 11:var_assign_opt(3) -> */
	T_EQUALS, N_EXP, A_OP_VAR_ASSIGN, 
	/* 12:assign_or_call1(3) -> */
	T_EQUALS, N_EXP, A_OP_VAR_ASSIGN, 
	/* 13:assign_or_call1(2) -> */
	N_FUNCTION_CALL, A_OP_RET_DISCARD, 
	/* 14:assign_or_call1(6) -> */
	T_LBRACKET, N_EXP, T_RBRACKET, A_OP_ARRAY_XLAT, T_DOT, N_ATTR_ASSIGN, 
	/* 15:assign_or_call1(3) -> */
	T_DOT, A_OP_OBJECT_GET, N_ATTR_ASSIGN, 
	/* 16:attr_assign(3) -> */
	T_ID, A_OP_PUSH_TMP, N_ATTR_ASSIGN2, 
	/* 17:attr_assign2(3) -> */
	T_EQUALS, N_EXP, A_OP_ATTR_ASSIGN, 
	/* 18:attr_assign2(6) -> */
	T_LBRACKET, N_EXP, T_RBRACKET, T_EQUALS, N_EXP, A_OP_ARRAY_ASSIGN, 
	/* 19:attr_assign2(2) -> */
	N_METHOD_CALL, A_OP_RET_DISCARD, 
	/* 20:method_call(5) -> */
	A_OP_METHOD_LOOKUP, T_LPAREN, N_ARG_LIST_OPT, T_RPAREN, A_OP_CALL, 
	/* 21:function_call(5) -> */
	A_OP_FUNCTION_LOOKUP, T_LPAREN, N_ARG_LIST_OPT, T_RPAREN, A_OP_CALL, 
	/* 22:arg_list_opt(0) -> */
	
	/* 23:arg_list_opt(1) -> */
	N_ARG_LIST, 
	/* 24:arg_list(3) -> */
	N_EXP, A_OP_ARG, N_ARG_LIST1, 
	/* 25:arg_list1(0) -> */
	
	/* 26:arg_list1(2) -> */
	T_COMMA, N_ARG_LIST, 
	/* 27:exp_lst_opt(0) -> */
	
	/* 28:exp_lst_opt(1) -> */
	N_EXP_LST, 
	/* 29:exp_lst(4) -> */
	T_ID, A_OP_PUSH_TMP, N_ASSIGN_OR_CALL1, N_EXP_LST1, 
	/* 30:exp_lst1(0) -> */
	
	/* 31:exp_lst1(2) -> */
	T_COMMA, N_EXP_LST, 
	/* 32:exp(2) -> */
	N_AND_EXP, N_OR_EXP, 
	/* 33:or_exp(0) -> */
	
	/* 34:or_exp(3) -> */
	T_BAR, N_EXP, A_OP_OR, 
	/* 35:or_exp(3) -> */
	T_LOGICOR, N_EXP, A_OP_LOGIC_OR, 
	/* 36:or_exp(3) -> */
	T_CARET, N_EXP, A_OP_XOR, 
	/* 37:and_exp(2) -> */
	N_RELATIONAL_EXP, N_AND_EXP1, 
	/* 38:and_exp1(0) -> */
	
	/* 39:and_exp1(3) -> */
	T_AMPERSAND, N_AND_EXP, A_OP_AND, 
	/* 40:and_exp1(3) -> */
	T_LOGICAND, N_AND_EXP, A_OP_LOGIC_AND, 
	/* 41:relational_exp(2) -> */
	N_SHIFT_EXP, N_RELATIONAL_EXP1, 
	/* 42:relational_exp1(0) -> */
	
	/* 43:relational_exp1(3) -> */
	T_LESSTHEN, N_RELATIONAL_EXP, A_OP_LT, 
	/* 44:relational_exp1(3) -> */
	T_GREATTHEN, N_RELATIONAL_EXP, A_OP_GT, 
	/* 45:relational_exp1(3) -> */
	T_EQU, N_RELATIONAL_EXP, A_OP_EQU, 
	/* 46:relational_exp1(3) -> */
	T_NEQ, N_RELATIONAL_EXP, A_OP_NEQ, 
	/* 47:relational_exp1(3) -> */
	T_GTE, N_RELATIONAL_EXP, A_OP_GTE, 
	/* 48:relational_exp1(3) -> */
	T_LTE, N_RELATIONAL_EXP, A_OP_LTE, 
	/* 49:shift_exp(2) -> */
	N_ADDITIVE_EXP, N_SHIFT_EXP1, 
	/* 50:shift_exp1(0) -> */
	
	/* 51:shift_exp1(3) -> */
	T_SHL, N_SHIFT_EXP, A_OP_SHL, 
	/* 52:shift_exp1(3) -> */
	T_ASR, N_SHIFT_EXP, A_OP_ASR, 
	/* 53:additive_exp(2) -> */
	N_DIV_EXP, N_ADDITIVE_EXP1, 
	/* 54:additive_exp1(0) -> */
	
	/* 55:additive_exp1(3) -> */
	T_PLUS, N_ADDITIVE_EXP, A_OP_ADD, 
	/* 56:additive_exp1(3) -> */
	T_MINUS, N_ADDITIVE_EXP, A_OP_SUB, 
	/* 57:div_exp(2) -> */
	N_MULT_EXP, N_DIV_EXP1, 
	/* 58:div_exp1(0) -> */
	
	/* 59:div_exp1(3) -> */
	T_SLASH, N_DIV_EXP, A_OP_DIV, 
	/* 60:div_exp1(3) -> */
	T_PERCENT, N_DIV_EXP, A_OP_MOD, 
	/* 61:mult_exp(2) -> */
	N_UNARY_EXP, N_MULT_EXP1, 
	/* 62:mult_exp1(0) -> */
	
	/* 63:mult_exp1(3) -> */
	T_STAR, N_MULT_EXP, A_OP_MUL, 
	/* 64:unary_exp(1) -> */
	N_PRIMARY_EXP, 
	/* 65:unary_exp(3) -> */
	T_TILDE, N_UNARY_EXP, A_OP_INV, 
	/* 66:unary_exp(3) -> */
	T_MINUS, N_UNARY_EXP, A_OP_MINUS, 
	/* 67:unary_exp(3) -> */
	T_NOT, N_UNARY_EXP, A_OP_NOT, 
	/* 68:primary_exp(3) -> */
	T_LPAREN, N_EXP, T_RPAREN, 
	/* 69:primary_exp(2) -> */
	T_INT, A_OP_PUSH_INT, 
	/* 70:primary_exp(2) -> */
	T_CHAR, A_OP_PUSH_INT, 
	/* 71:primary_exp(2) -> */
	T_STRING, A_OP_PUSH_STRING, 
	/* 72:primary_exp(2) -> */
	T_TRUE, A_OP_PUSH_TRUE, 
	/* 73:primary_exp(2) -> */
	T_FALSE, A_OP_PUSH_FALSE, 
	/* 74:primary_exp(1) -> */
	N_ID_EVAL, 
	/* 75:id_eval(3) -> */
	T_ID, A_OP_PUSH_TMP, N_ID_EVAL1, 
	/* 76:id_eval1(1) -> */
	A_OP_VAR_EVAL, 
	/* 77:id_eval1(2) -> */
	N_FUNCTION_CALL, A_OP_CALL_RET, 
	/* 78:id_eval1(6) -> */
	T_LBRACKET, N_EXP, T_RBRACKET, A_OP_ARRAY_XLAT, T_DOT, N_ATTR_EVAL, 
	/* 79:id_eval1(3) -> */
	T_DOT, A_OP_OBJECT_GET, N_ATTR_EVAL, 
	/* 80:attr_eval(3) -> */
	T_ID, A_OP_PUSH_TMP, N_ATTR_EVAL1, 
	/* 81:attr_eval1(1) -> */
	A_OP_ATTR_EVAL, 
	/* 82:attr_eval1(2) -> */
	N_METHOD_CALL, A_OP_CALL_RET, 
	/* 83:attr_eval1(4) -> */
	T_LBRACKET, N_EXP, T_RBRACKET, A_OP_ARRAY_EVAL, 
	/* 84:compound_stat(5) -> */
	T_LBRACE, A_OP_BLK_OPEN, N_STAT_LIST, T_RBRACE, A_OP_BLK_CLOSE, 
	/* 85:stat_list(0) -> */
	
	/* 86:stat_list(2) -> */
	N_STAT, N_STAT_LIST, 
	/* 87:stat_list(3) -> */
	N_SEMI_STAT, T_SEMICOLON, N_STAT_LIST, 
	/* 88:stat(13) -> */
	T_FOR, T_LPAREN, N_EXP_LST_OPT, T_SEMICOLON, A_OP_FOR_INIT, N_EXP, A_OP_FOR_COND, T_SEMICOLON, N_EXP_LST_OPT, A_OP_FOR_AFTER, T_RPAREN, N_COMPOUND_STAT, A_OP_FOR_END, 
	/* 89:stat(6) -> */
	T_WHILE, A_OP_WHILE_BEGIN, N_CONDITION, A_OP_WHILE_COND, N_COMPOUND_STAT, A_OP_WHILE_END, 
	/* 90:stat(6) -> */
	T_IF, N_CONDITION, A_OP_IF_COND, N_COMPOUND_STAT, N_ELSE_OPT, A_OP_IF_END, 
	/* 91:stat(4) -> */
	T_TRY, A_OP_TRY_BEGIN, N_COMPOUND_STAT, N_CATCH_OPT, 
	/* 92:condition(3) -> */
	T_LPAREN, N_EXP, T_RPAREN, 
	/* 93:semi_stat(3) -> */
	T_THROW, N_EXP, A_OP_THROW, 
	/* 94:semi_stat(2) -> */
	T_VAR, N_VAR_LIST, 
	/* 95:semi_stat(3) -> */
	T_ID, A_OP_PUSH_TMP, N_ASSIGN_OR_CALL1, 
	/* 96:else_opt(0) -> */
	
	/* 97:else_opt(3) -> */
	T_ELSE, A_OP_IF_ELSE, N_COMPOUND_STAT, 
	/* 98:catch_opt(1) -> */
	A_OP_TRY_END, 
	/* 99:catch_opt(14) -> */
	T_CATCH, A_OP_CATCH, A_OP_BLK_OPEN, T_LPAREN, T_ID, A_OP_VAR_DECL, A_OP_PUSH_TMP, A_OP_VAR_ASSIGN, T_RPAREN, T_LBRACE, N_STAT_LIST, T_RBRACE, A_OP_BLK_CLOSE, A_OP_CATCH_END, 
};
/* 258 bytes */

static const struct {
	uint8_t off;
	uint8_t cnt;
} rule_idx[] = {
	{   0,  1},
	{   1,  2},
	{   3,  2},
	{   5,  2},
	{   7,  1},
	{   8,  2},
	{  10,  2},
	{  12,  0},
	{  12,  2},
	{  14,  4},
	{  18,  1},
	{  19,  3},
	{  22,  3},
	{  25,  2},
	{  27,  6},
	{  33,  3},
	{  36,  3},
	{  39,  3},
	{  42,  6},
	{  48,  2},
	{  50,  5},
	{  55,  5},
	{  60,  0},
	{  60,  1},
	{  61,  3},
	{  64,  0},
	{  64,  2},
	{  66,  0},
	{  66,  1},
	{  67,  4},
	{  71,  0},
	{  71,  2},
	{  73,  2},
	{  75,  0},
	{  75,  3},
	{  78,  3},
	{  81,  3},
	{  84,  2},
	{  86,  0},
	{  86,  3},
	{  89,  3},
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
	{ 128,  2},
	{ 130,  0},
	{ 130,  3},
	{ 133,  3},
	{ 136,  2},
	{ 138,  0},
	{ 138,  3},
	{ 141,  1},
	{ 142,  3},
	{ 145,  3},
	{ 148,  3},
	{ 151,  3},
	{ 154,  2},
	{ 156,  2},
	{ 158,  2},
	{ 160,  2},
	{ 162,  2},
	{ 164,  1},
	{ 165,  3},
	{ 168,  1},
	{ 169,  2},
	{ 171,  6},
	{ 177,  3},
	{ 180,  3},
	{ 183,  1},
	{ 184,  2},
	{ 186,  4},
	{ 190,  5},
	{ 195,  0},
	{ 195,  2},
	{ 197,  3},
	{ 200, 13},
	{ 213,  6},
	{ 219,  6},
	{ 225,  4},
	{ 229,  3},
	{ 232,  3},
	{ 235,  2},
	{ 237,  3},
	{ 240,  0},
	{ 240,  3},
	{ 243,  1},
	{ 244, 14},
};
/* 200 bytes */

/* Total: 1346 bytes */


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

