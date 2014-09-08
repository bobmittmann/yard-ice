/* LL(1) Table */

#include "calc_ll.h"

struct tr_pair {
	uint8_t t;
	uint8_t r;
};

/* Productions vectors table */
const struct tr_pair prod_vec[] = {
	/*  48 (program) */
	{  0,  0}, { 24,  1}, { 30,  1}, { 32,  1}, { 33,  1}, { 36,  1}, 
	{ 37,  1}, { 38,  1}, { 40,  1}, { 42,  1}, { 43,  1}, { 45,  1}, 
	/*  49 (element) */
	{ 24,  3}, { 30,  3}, { 32,  3}, { 33,  3}, { 36,  3}, { 37,  2}, 
	{ 38,  3}, { 40,  3}, { 42,  3}, { 43,  3}, { 45,  3}, 
	/*  50 (param_list_opt) */
	{ 29,  4}, { 45,  5}, 
	/*  51 (compound_stat) */
	{ 30, 21}, 
	/*  52 (stat) */
	{ 24,  9}, { 30, 17}, { 32, 13}, { 33, 14}, { 36, 10}, { 38, 11}, 
	{ 40, 15}, { 42, 16}, { 43, 12}, { 45, 18}, 
	/*  53 (param_list) */
	{ 45,  6}, 
	/*  54 (param_list1) */
	{ 23,  8}, { 29,  7}, 
	/*  55 (exp_lst_opt) */
	{ 24, 40}, { 29, 40}, { 45, 41}, 
	/*  56 (exp) */
	{  9, 47}, { 10, 47}, { 19, 47}, { 20, 47}, { 26, 48}, { 28, 47}, 
	{ 35, 47}, { 39, 47}, { 41, 47}, { 44, 47}, { 45, 47}, 
	/*  57 (condition) */
	{ 28, 24}, 
	/*  58 (else_opt) */
	{  0, 19}, { 24, 19}, { 30, 19}, { 31, 19}, { 32, 19}, { 33, 19}, 
	{ 34, 20}, { 36, 19}, { 37, 19}, { 38, 19}, { 40, 19}, { 42, 19}, 
	{ 43, 19}, { 45, 19}, 
	/*  59 (exp_opt) */
	{  9, 46}, { 10, 46}, { 19, 46}, { 20, 46}, { 24, 45}, { 26, 46}, 
	{ 28, 46}, { 35, 46}, { 39, 46}, { 41, 46}, { 44, 46}, { 45, 46}, 
	/*  60 (var_list) */
	{ 45, 25}, 
	/*  61 (assign_or_call) */
	{ 45, 31}, 
	/*  62 (stat_list) */
	{ 24, 23}, { 30, 23}, { 31, 22}, { 32, 23}, { 33, 23}, { 36, 23}, 
	{ 38, 23}, { 40, 23}, { 42, 23}, { 43, 23}, { 45, 23}, 
	/*  63 (var) */
	{ 45, 28}, 
	/*  64 (var_list1) */
	{ 23, 27}, { 24, 26}, 
	/*  65 (var1) */
	{ 21, 30}, { 23, 29}, { 24, 29}, 
	/*  66 (assign_or_call1) */
	{ 21, 32}, { 22, 33}, { 26, 34}, { 28, 35}, 
	/*  67 (assign_or_call2) */
	{ 21, 36}, { 22, 37}, 
	/*  68 (arg_list_opt) */
	{  9, 93}, { 10, 93}, { 19, 93}, { 20, 93}, { 26, 93}, { 27, 92}, 
	{ 28, 93}, { 29, 92}, { 35, 93}, { 39, 93}, { 41, 93}, { 44, 93}, 
	{ 45, 93}, 
	/*  69 (assign_or_call3) */
	{ 22, 39}, { 23, 38}, { 24, 38}, { 29, 38}, 
	/*  70 (exp_lst) */
	{ 45, 42}, 
	/*  71 (exp_lst1) */
	{ 23, 44}, { 24, 43}, { 29, 43}, 
	/*  72 (shift_exp) */
	{  9, 56}, { 10, 56}, { 19, 56}, { 20, 56}, { 28, 56}, { 35, 56}, 
	{ 39, 56}, { 41, 56}, { 44, 56}, { 45, 56}, 
	/*  73 (exp1) */
	{  3, 52}, {  4, 50}, {  5, 53}, {  6, 51}, {  7, 54}, {  8, 55}, 
	{ 23, 49}, { 24, 49}, { 27, 49}, { 29, 49}, 
	/*  74 (additive_exp) */
	{  9, 60}, { 10, 60}, { 19, 60}, { 20, 60}, { 28, 60}, { 35, 60}, 
	{ 39, 60}, { 41, 60}, { 44, 60}, { 45, 60}, 
	/*  75 (shift_exp1) */
	{  1, 59}, {  2, 58}, {  3, 57}, {  4, 57}, {  5, 57}, {  6, 57}, 
	{  7, 57}, {  8, 57}, { 23, 57}, { 24, 57}, { 27, 57}, { 29, 57}, 
	/*  76 (mult_exp) */
	{  9, 67}, { 10, 67}, { 19, 67}, { 20, 67}, { 28, 67}, { 35, 67}, 
	{ 39, 67}, { 41, 67}, { 44, 67}, { 45, 67}, 
	/*  77 (additive_exp1) */
	{  1, 61}, {  2, 61}, {  3, 61}, {  4, 61}, {  5, 61}, {  6, 61}, 
	{  7, 61}, {  8, 61}, {  9, 62}, { 10, 63}, { 14, 66}, { 15, 64}, 
	{ 18, 65}, { 23, 61}, { 24, 61}, { 27, 61}, { 29, 61}, 
	/*  78 (unary_exp) */
	{  9, 77}, { 10, 78}, { 19, 76}, { 20, 75}, { 28, 74}, { 35, 74}, 
	{ 39, 74}, { 41, 74}, { 44, 74}, { 45, 74}, 
	/*  79 (mult_exp1) */
	{  1, 68}, {  2, 68}, {  3, 68}, {  4, 68}, {  5, 68}, {  6, 68}, 
	{  7, 68}, {  8, 68}, {  9, 68}, { 10, 68}, { 11, 69}, { 12, 70}, 
	{ 13, 71}, { 14, 68}, { 15, 68}, { 16, 73}, { 17, 72}, { 18, 68}, 
	{ 23, 68}, { 24, 68}, { 27, 68}, { 29, 68}, 
	/*  80 (primary_exp) */
	{ 28, 79}, { 35, 81}, { 39, 83}, { 41, 82}, { 44, 80}, { 45, 84}, 
	/*  81 (met_or_attr) */
	{ 45, 85}, 
	/*  82 (met_or_attr1) */
	{  1, 86}, {  2, 86}, {  3, 86}, {  4, 86}, {  5, 86}, {  6, 86}, 
	{  7, 86}, {  8, 86}, {  9, 86}, { 10, 86}, { 11, 86}, { 12, 86}, 
	{ 13, 86}, { 14, 86}, { 15, 86}, { 16, 86}, { 17, 86}, { 18, 86}, 
	{ 22, 87}, { 23, 86}, { 24, 86}, { 26, 88}, { 27, 86}, { 28, 89}, 
	{ 29, 86}, 
	/*  83 (met_or_attr2) */
	{  1, 90}, {  2, 90}, {  3, 90}, {  4, 90}, {  5, 90}, {  6, 90}, 
	{  7, 90}, {  8, 90}, {  9, 90}, { 10, 90}, { 11, 90}, { 12, 90}, 
	{ 13, 90}, { 14, 90}, { 15, 90}, { 16, 90}, { 17, 90}, { 18, 90}, 
	{ 22, 91}, { 23, 90}, { 24, 90}, { 27, 90}, { 29, 90}, 
	/*  84 (arg_list) */
	{  9, 94}, { 10, 94}, { 19, 94}, { 20, 94}, { 26, 94}, { 28, 94}, 
	{ 35, 94}, { 39, 94}, { 41, 94}, { 44, 94}, { 45, 94}, 
	/*  85 (arg_list1) */
	{ 23, 96}, { 27, 95}, { 29, 95}, 
};
/* 592 bytes; */

static const struct {
	uint16_t off;
	uint8_t cnt;
} prod_idx[] = {
	{   0, 12},
	{  12, 11},
	{  23,  2},
	{  25,  1},
	{  26, 10},
	{  36,  1},
	{  37,  2},
	{  39,  3},
	{  42, 11},
	{  53,  1},
	{  54, 14},
	{  68, 12},
	{  80,  1},
	{  81,  1},
	{  82, 11},
	{  93,  1},
	{  94,  2},
	{  96,  3},
	{  99,  4},
	{ 103,  2},
	{ 105, 13},
	{ 118,  4},
	{ 122,  1},
	{ 123,  3},
	{ 126, 10},
	{ 136, 10},
	{ 146, 10},
	{ 156, 12},
	{ 168, 10},
	{ 178, 17},
	{ 195, 10},
	{ 205, 22},
	{ 227,  6},
	{ 233,  1},
	{ 234, 25},
	{ 259, 23},
	{ 282, 11},
	{ 293,  3},
};
/* 152 bytes */

/* Rules vectors table */
const uint8_t rule_vec[] = {
	/*  0:program(1) -> */
	T_EOF, 
	/*  1:program(2) -> */
	N_ELEMENT, N_PROGRAM, 
	/*  2:element(6) -> */
	T_FUNCTION, T_ID, T_LPAREN, N_PARAM_LIST_OPT, T_RPAREN, N_COMPOUND_STAT, 
	/*  3:element(1) -> */
	N_STAT, 
	/*  4:param_list_opt(0) -> */
	
	/*  5:param_list_opt(1) -> */
	N_PARAM_LIST, 
	/*  6:param_list(2) -> */
	T_ID, N_PARAM_LIST1, 
	/*  7:param_list1(0) -> */
	
	/*  8:param_list1(2) -> */
	T_COMMA, N_PARAM_LIST, 
	/*  9:stat(1) -> */
	T_SEMICOLON, 
	/* 10:stat(9) -> */
	T_FOR, T_LPAREN, N_EXP_LST_OPT, T_SEMICOLON, N_EXP, T_SEMICOLON, N_EXP_LST_OPT, T_RPAREN, N_COMPOUND_STAT, 
	/* 11:stat(4) -> */
	T_IF, N_CONDITION, N_COMPOUND_STAT, N_ELSE_OPT, 
	/* 12:stat(3) -> */
	T_WHILE, N_CONDITION, N_COMPOUND_STAT, 
	/* 13:stat(2) -> */
	T_BREAK, T_SEMICOLON, 
	/* 14:stat(2) -> */
	T_CONTINUE, T_SEMICOLON, 
	/* 15:stat(3) -> */
	T_RETURN, N_EXP_OPT, T_SEMICOLON, 
	/* 16:stat(3) -> */
	T_VAR, N_VAR_LIST, T_SEMICOLON, 
	/* 17:stat(1) -> */
	N_COMPOUND_STAT, 
	/* 18:stat(2) -> */
	N_ASSIGN_OR_CALL, T_SEMICOLON, 
	/* 19:else_opt(0) -> */
	
	/* 20:else_opt(2) -> */
	T_ELSE, N_COMPOUND_STAT, 
	/* 21:compound_stat(3) -> */
	T_LBRACE, N_STAT_LIST, T_RBRACE, 
	/* 22:stat_list(0) -> */
	
	/* 23:stat_list(2) -> */
	N_STAT, N_STAT_LIST, 
	/* 24:condition(3) -> */
	T_LPAREN, N_EXP, T_RPAREN, 
	/* 25:var_list(2) -> */
	N_VAR, N_VAR_LIST1, 
	/* 26:var_list1(0) -> */
	
	/* 27:var_list1(2) -> */
	T_COMMA, N_VAR_LIST, 
	/* 28:var(2) -> */
	T_ID, N_VAR1, 
	/* 29:var1(0) -> */
	
	/* 30:var1(2) -> */
	T_ASSIGN, N_EXP, 
	/* 31:assign_or_call(2) -> */
	T_ID, N_ASSIGN_OR_CALL1, 
	/* 32:assign_or_call1(2) -> */
	T_ASSIGN, N_EXP, 
	/* 33:assign_or_call1(2) -> */
	T_DOT, N_ASSIGN_OR_CALL, 
	/* 34:assign_or_call1(4) -> */
	T_LBKT, N_EXP, T_RBKT, N_ASSIGN_OR_CALL2, 
	/* 35:assign_or_call1(4) -> */
	T_LPAREN, N_ARG_LIST_OPT, T_RPAREN, N_ASSIGN_OR_CALL3, 
	/* 36:assign_or_call2(2) -> */
	T_ASSIGN, N_EXP, 
	/* 37:assign_or_call2(2) -> */
	T_DOT, N_ASSIGN_OR_CALL, 
	/* 38:assign_or_call3(0) -> */
	
	/* 39:assign_or_call3(2) -> */
	T_DOT, N_ASSIGN_OR_CALL, 
	/* 40:exp_lst_opt(0) -> */
	
	/* 41:exp_lst_opt(1) -> */
	N_EXP_LST, 
	/* 42:exp_lst(2) -> */
	N_ASSIGN_OR_CALL, N_EXP_LST1, 
	/* 43:exp_lst1(0) -> */
	
	/* 44:exp_lst1(2) -> */
	T_COMMA, N_EXP_LST, 
	/* 45:exp_opt(0) -> */
	
	/* 46:exp_opt(1) -> */
	N_EXP, 
	/* 47:exp(2) -> */
	N_SHIFT_EXP, N_EXP1, 
	/* 48:exp(3) -> */
	T_LBKT, N_ARG_LIST_OPT, T_RBKT, 
	/* 49:exp1(0) -> */
	
	/* 50:exp1(2) -> */
	T_LT, N_EXP, 
	/* 51:exp1(2) -> */
	T_GT, N_EXP, 
	/* 52:exp1(2) -> */
	T_LTE, N_EXP, 
	/* 53:exp1(2) -> */
	T_GTE, N_EXP, 
	/* 54:exp1(2) -> */
	T_EQ, N_EXP, 
	/* 55:exp1(2) -> */
	T_NEQ, N_EXP, 
	/* 56:shift_exp(2) -> */
	N_ADDITIVE_EXP, N_SHIFT_EXP1, 
	/* 57:shift_exp1(0) -> */
	
	/* 58:shift_exp1(2) -> */
	T_SHL, N_SHIFT_EXP, 
	/* 59:shift_exp1(2) -> */
	T_ASR, N_SHIFT_EXP, 
	/* 60:additive_exp(2) -> */
	N_MULT_EXP, N_ADDITIVE_EXP1, 
	/* 61:additive_exp1(0) -> */
	
	/* 62:additive_exp1(3) -> */
	T_PLUS, N_ADDITIVE_EXP, A_OP_ADD, 
	/* 63:additive_exp1(3) -> */
	T_MINUS, N_ADDITIVE_EXP, A_OP_SUB, 
	/* 64:additive_exp1(3) -> */
	T_BITOR, N_ADDITIVE_EXP, A_OP_OR, 
	/* 65:additive_exp1(3) -> */
	T_XOR, N_ADDITIVE_EXP, A_OP_XOR, 
	/* 66:additive_exp1(3) -> */
	T_OR, N_ADDITIVE_EXP, A_OP_OR, 
	/* 67:mult_exp(2) -> */
	N_UNARY_EXP, N_MULT_EXP1, 
	/* 68:mult_exp1(0) -> */
	
	/* 69:mult_exp1(3) -> */
	T_MUL, N_MULT_EXP, A_OP_MULT, 
	/* 70:mult_exp1(3) -> */
	T_DIV, N_MULT_EXP, A_OP_DIV, 
	/* 71:mult_exp1(3) -> */
	T_MOD, N_MULT_EXP, A_OP_MOD, 
	/* 72:mult_exp1(3) -> */
	T_BITAND, N_MULT_EXP, A_OP_AND, 
	/* 73:mult_exp1(3) -> */
	T_AND, N_MULT_EXP, A_OP_AND, 
	/* 74:unary_exp(1) -> */
	N_PRIMARY_EXP, 
	/* 75:unary_exp(3) -> */
	T_INV, N_UNARY_EXP, A_OP_INV, 
	/* 76:unary_exp(3) -> */
	T_NOT, N_UNARY_EXP, A_OP_NOT, 
	/* 77:unary_exp(2) -> */
	T_PLUS, N_UNARY_EXP, 
	/* 78:unary_exp(3) -> */
	T_MINUS, N_UNARY_EXP, A_OP_MINUS, 
	/* 79:primary_exp(3) -> */
	T_LPAREN, N_EXP, T_RPAREN, 
	/* 80:primary_exp(1) -> */
	T_INT, 
	/* 81:primary_exp(1) -> */
	T_FALSE, 
	/* 82:primary_exp(1) -> */
	T_TRUE, 
	/* 83:primary_exp(1) -> */
	T_NULL, 
	/* 84:primary_exp(1) -> */
	N_MET_OR_ATTR, 
	/* 85:met_or_attr(2) -> */
	T_ID, N_MET_OR_ATTR1, 
	/* 86:met_or_attr1(0) -> */
	
	/* 87:met_or_attr1(2) -> */
	T_DOT, N_MET_OR_ATTR, 
	/* 88:met_or_attr1(4) -> */
	T_LBKT, N_EXP, T_RBKT, N_MET_OR_ATTR2, 
	/* 89:met_or_attr1(4) -> */
	T_LPAREN, N_ARG_LIST_OPT, T_RPAREN, N_MET_OR_ATTR2, 
	/* 90:met_or_attr2(0) -> */
	
	/* 91:met_or_attr2(2) -> */
	T_DOT, N_MET_OR_ATTR, 
	/* 92:arg_list_opt(0) -> */
	
	/* 93:arg_list_opt(1) -> */
	N_ARG_LIST, 
	/* 94:arg_list(2) -> */
	N_EXP, N_ARG_LIST1, 
	/* 95:arg_list1(0) -> */
	
	/* 96:arg_list1(2) -> */
	T_COMMA, N_ARG_LIST, 
};
/* 185 bytes */

static const struct {
	uint8_t off;
	uint8_t cnt;
} rule_idx[] = {
	{   0,  1},
	{   1,  2},
	{   3,  6},
	{   9,  1},
	{  10,  0},
	{  10,  1},
	{  11,  2},
	{  13,  0},
	{  13,  2},
	{  15,  1},
	{  16,  9},
	{  25,  4},
	{  29,  3},
	{  32,  2},
	{  34,  2},
	{  36,  3},
	{  39,  3},
	{  42,  1},
	{  43,  2},
	{  45,  0},
	{  45,  2},
	{  47,  3},
	{  50,  0},
	{  50,  2},
	{  52,  3},
	{  55,  2},
	{  57,  0},
	{  57,  2},
	{  59,  2},
	{  61,  0},
	{  61,  2},
	{  63,  2},
	{  65,  2},
	{  67,  2},
	{  69,  4},
	{  73,  4},
	{  77,  2},
	{  79,  2},
	{  81,  0},
	{  81,  2},
	{  83,  0},
	{  83,  1},
	{  84,  2},
	{  86,  0},
	{  86,  2},
	{  88,  0},
	{  88,  1},
	{  89,  2},
	{  91,  3},
	{  94,  0},
	{  94,  2},
	{  96,  2},
	{  98,  2},
	{ 100,  2},
	{ 102,  2},
	{ 104,  2},
	{ 106,  2},
	{ 108,  0},
	{ 108,  2},
	{ 110,  2},
	{ 112,  2},
	{ 114,  0},
	{ 114,  3},
	{ 117,  3},
	{ 120,  3},
	{ 123,  3},
	{ 126,  3},
	{ 129,  2},
	{ 131,  0},
	{ 131,  3},
	{ 134,  3},
	{ 137,  3},
	{ 140,  3},
	{ 143,  3},
	{ 146,  1},
	{ 147,  3},
	{ 150,  3},
	{ 153,  2},
	{ 155,  3},
	{ 158,  3},
	{ 161,  1},
	{ 162,  1},
	{ 163,  1},
	{ 164,  1},
	{ 165,  1},
	{ 166,  2},
	{ 168,  0},
	{ 168,  2},
	{ 170,  4},
	{ 174,  4},
	{ 178,  0},
	{ 178,  2},
	{ 180,  0},
	{ 180,  1},
	{ 181,  2},
	{ 183,  0},
	{ 183,  2},
};
/* 194 bytes */

/* Total: 1123 bytes */


int ll_rule_push(uint8_t * sp, unsigned int sym, unsigned int tok)
{
	struct tr_pair * vec;
	uint8_t * p;
	int imax;
	int imin;
	int n;
	int i;

	/* Get the index for the symbol at the stack's head */
	i = sym - NONTERM_BASE;
	if (i < 0) /* Shuld be nonterminal */
		return -1;
	vec = (struct tr_pair *)&prod_vec[prod_idx[i].off];
	imax = prod_idx[i].cnt;
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

const const char * const ll_sym_tab[] = {
	"EOF",
 	"ASR",
 	"SHL",
 	"LTE",
 	"LT",
 	"GTE",
 	"GT",
 	"EQ",
 	"NEQ",
 	"PLUS",
 	"MINUS",
 	"MUL",
 	"DIV",
 	"MOD",
 	"OR",
 	"BITOR",
 	"AND",
 	"BITAND",
 	"XOR",
 	"NOT",
 	"INV",
 	"ASSIGN",
 	"DOT",
 	"COMMA",
 	"SEMICOLON",
 	"COLON",
 	"LBKT",
 	"RBKT",
 	"LPAREN",
 	"RPAREN",
 	"LBRACE",
 	"RBRACE",
 	"BREAK",
 	"CONTINUE",
 	"ELSE",
 	"FALSE",
 	"FOR",
 	"FUNCTION",
 	"IF",
 	"NULL",
 	"RETURN",
 	"TRUE",
 	"VAR",
 	"WHILE",
 	"INT",
 	"ID",
 	"STRING",
 	"ERR",
 	"program",
 	"element",
 	"param_list_opt",
 	"compound_stat",
 	"stat",
 	"param_list",
 	"param_list1",
 	"exp_lst_opt",
 	"exp",
 	"condition",
 	"else_opt",
 	"exp_opt",
 	"var_list",
 	"assign_or_call",
 	"stat_list",
 	"var",
 	"var_list1",
 	"var1",
 	"assign_or_call1",
 	"assign_or_call2",
 	"arg_list_opt",
 	"assign_or_call3",
 	"exp_lst",
 	"exp_lst1",
 	"shift_exp",
 	"exp1",
 	"additive_exp",
 	"shift_exp1",
 	"mult_exp",
 	"additive_exp1",
 	"unary_exp",
 	"mult_exp1",
 	"primary_exp",
 	"met_or_attr",
 	"met_or_attr1",
 	"met_or_attr2",
 	"arg_list",
 	"arg_list1",
 	"op_add",
 	"op_sub",
 	"op_or",
 	"op_xor",
 	"op_mult",
 	"op_div",
 	"op_mod",
 	"op_and",
 	"op_inv",
 	"op_not",
 	"op_minus",
 };

