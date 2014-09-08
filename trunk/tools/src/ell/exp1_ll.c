/* LL(1) Table */

#include "exp1_ll.h"

struct tr_pair {
	uint8_t t;
	uint8_t r;
};

/* Productions vectors table */
const struct tr_pair prod_vec[] = {
	/*   8 (prog) */
	{  5,  0}, {  7,  0}, 
	/*   9 (exp) */
	{  5,  1}, {  7,  1}, 
	/*  10 (mult_exp) */
	{  5,  5}, {  7,  5}, 
	/*  11 (exp1) */
	{  0,  2}, {  1,  3}, {  2,  4}, {  6,  2}, 
	/*  12 (factor) */
	{  5,  9}, {  7, 10}, 
	/*  13 (mult_exp1) */
	{  0,  6}, {  1,  6}, {  2,  6}, {  3,  7}, {  4,  8}, {  6,  6}, 
};
/* 36 bytes; */

static const struct {
	uint16_t off;
	uint8_t cnt;
} prod_idx[] = {
	{   0,  2},
	{   2,  2},
	{   4,  2},
	{   6,  4},
	{  10,  2},
	{  12,  6},
};
/* 24 bytes */

/* Rules vectors table */
const uint8_t rule_vec[] = {
	/*  0:prog(2) -> */
	N_EXP, T_EOF, 
	/*  1:exp(2) -> */
	N_MULT_EXP, N_EXP1, 
	/*  2:exp1(0) -> */
	
	/*  3:exp1(3) -> */
	T_PLUS, N_EXP, A_ADD, 
	/*  4:exp1(3) -> */
	T_MINUS, N_EXP, A_SUB, 
	/*  5:mult_exp(2) -> */
	N_FACTOR, N_MULT_EXP1, 
	/*  6:mult_exp1(0) -> */
	
	/*  7:mult_exp1(2) -> */
	T_MUL, N_MULT_EXP, 
	/*  8:mult_exp1(2) -> */
	T_DIV, N_MULT_EXP, 
	/*  9:factor(3) -> */
	T_LPAREN, N_EXP, T_RPAREN, 
	/* 10:factor(1) -> */
	T_INT, 
};
/* 20 bytes */

static const struct {
	uint8_t off;
	uint8_t cnt;
} rule_idx[] = {
	{   0,  2},
	{   2,  2},
	{   4,  0},
	{   4,  3},
	{   7,  3},
	{  10,  2},
	{  12,  0},
	{  12,  2},
	{  14,  2},
	{  16,  3},
	{  19,  1},
};
/* 22 bytes */

/* Total: 102 bytes */


#define NONTERM_BASE 8
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

