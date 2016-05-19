/* 
 * Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
 * 
 * This file is part of the YARD-ICE.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You can receive a copy of the GNU Lesser General Public License from 
 * http://www.gnu.org/
 */

/** 
 * @file ell.h
 * @brief 
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#ifndef __ELL_H__
#define __ELL_H__

#include "config.h"
#include <stdint.h>
#include <stdbool.h>

/* symbol -> kind */
enum {
	TERM = 0,
	NONTERM,
	ACTION,
};

/* symbol -> regex ( generate using regular expresion) */
#define SINGLE	 0
#define GROUP	 1
#define OPTION 	 2
#define REPEAT	 4
#define REPEAT1	 8
#define REST	 16

/* for LL1 conflicts */
#define NO_CONFLICT    0
#define LR_CONFLICT    1
#define CLP_CONFLICT   2

typedef int *BITVEC;

typedef struct _symbol {	    /* Description of grammar symbol */
	struct  _symbol *next_sym;  /* a link */
	int 	sym_no;		        /* symbol number */
	int  	kind;		        /* terminal, non-terminal or action */
	int 	rextype;            /* regex type - GROUP, REPEAT, OPTION */
	int 	nullable;           /* if can generate empty string*/
	int 	numalt;
	struct  _rule *firstalt;    /* ptr to first of alternate rules */	
	BITVEC 	first;
	BITVEC 	follow;
	char 	*symtext;	        /* text of symbol */
	unsigned int r_offs;		/* offset in the production's list */
	unsigned int r_cnt;			/* number of rules */
} SYM, *SYMPTR; 

typedef struct _rule {	        /* Description of a production rule */
	struct _rule *next_rule;
	int	rule_no;	            /* production number */
	int	nrhs;		            /* number of symbols on RHS - empty pr. has nrhs=0*/		
	int 	conflict;
	int 	lineno;
	unsigned int offs;			/* offset in the rule's list */
	BITVEC	predict;            /* prediction set */
	SYMPTR	lhs;	            /* symbol on LHS */
	SYMPTR	rhs[1];		        /* symbols on right-hand side */
} RULE, *RULEPTR;

static inline char kind_prefix(int k) {
	return (k == ACTION) ? 'A' : (k == NONTERM) ? 'N' : 'T';
}

/* the scanner */

#define BAD_TOKEN   -1
#define T_EOF         0
#define ID          256
#define TOKEN_SPEC  257
#define END_SPEC    258
#define CHR_LIT     259
#define STR_LIT     260
#define STAR         '*'
#define QUEST        '?'
#define PLUS         '+'
#define LPAREN       '('
#define RPAREN       ')'
#define SEMI         ';'
#define COLON        ':'
#define BAR          '|'
#define COMMA        ','
#define LBRACE       '{'
#define RBRACE       '}'

#define NL          '\n'

#define  MAXLENGTH   255

extern RULEPTR first_rule;
extern SYMPTR first_sym;
extern int num_rules;
extern int lastsymnum;

/* Define looping constructs for the main lists in the program */
#define forall_rules(r)   for(r = first_rule; r != NULL; r = r->next_rule)
#define forall_symbols(s) for(s = first_sym; s != NULL; s = s->next_sym)

#ifdef __cplusplus
extern "C" {
#endif

int parse_grammar(const char * fname);

/* Package of bit vector routines */
BITVEC BitVecNew();
void BitVecSetSize(); 
int BitVecUnion(BITVEC x,  BITVEC y);
int BitVecEmpty(BITVEC x);
int BitVecHasIntersection(BITVEC x,  BITVEC y);
void BitVecClear(BITVEC x);
int BitVecTest(BITVEC x, int pos);
int BitVecNumEl(BITVEC x);
int BitVecAdd(BITVEC x, int pos);

void WriteRecursiveParser(FILE *fp, char * pname, FILE * fh, char *hname);

void write_compact_c(FILE * fp, char * prefix, char * hname, bool debug);

void write_compact_h(FILE * fp, char * prefix, char * hname, bool debug); 


#ifdef __cplusplus
}
#endif

#endif /* __CALC_H__ */

