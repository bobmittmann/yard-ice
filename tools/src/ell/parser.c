/*
 * LL(1) Predictive Parser Table Generator and RDP Generator
 * 
 * This file is part of Ell.
 *
 * Ell is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/*! 
 * \file      parser.c
 * \brief     Ell grammar parser
 * \author    Ivo Mateljan
 * \copyright 2003-2014, Ivo Mateljan
 */ 

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>

#include "ell.h"

static FILE * infile;		/* the input file itself */

RULEPTR first_rule;
RULEPTR last_rule;
int	num_rules = 0;

SYMPTR first_sym = NULL;
SYMPTR last_sym = NULL;
int	lastsymnum = 0;

static int nextch = ' ';
static char lexeme[MAXLENGTH +1];  
static int lineno;

/* look up symbol s and return pointer to symbol table entry */
SYMPTR Lookup(char *s)
{
    SYMPTR sp;
    
	/* if exist return sp*/
	forall_symbols( sp ) {
		if (sp->symtext[0] != s[0]) continue;
		if (strcmp(sp->symtext, s) == 0)
			return( sp );
    }
	/* else insert */
    sp = (SYMPTR) calloc( 1, (unsigned)( sizeof(*sp)) );
	if (sp == NULL) {
		fprintf(stderr, "\nMemory error\n"); 
		exit(1);
	}
    sp->symtext = (char *)malloc(strlen(s)+1);
	if (sp->symtext == NULL) {
		fprintf(stderr, "\nMemory error\n"); 
		exit(1);
	}
	strcpy(sp->symtext,s);
    sp->next_sym = NULL;
    sp->kind = TERM;     /* assume: terminal */
	sp->rextype = SINGLE;
    sp->nullable = false;	/* assume: not nullable */
	sp->numalt = 0;
	sp->firstalt = NULL;    
    sp->sym_no = lastsymnum++;
    sp->first = sp->follow = NULL;
    if (first_sym == NULL)
		first_sym = sp;
    else
		last_sym->next_sym = sp;
    last_sym = sp;
    return( sp );
}

void SyntaxError(char * str)
{
	fprintf(stderr, "\nError: %s in line %d\n", str, lineno);
	exit(1);
}

static void scanComment(void)
{
	/* PRE: comment recognised */
    char prev = 0;	
	while(1) { /* eat */		
		nextch  = getc(infile);			
		if(nextch == EOF || nextch  == '\0') {
			SyntaxError("End of file in comment");
			return;
		}
		if (nextch == '\n') lineno++;
		if(nextch  == '*'  && prev == '/') 				
			SyntaxError("Nested comment");
		else if(nextch  == '/'  && prev == '*') 			
				break; /*  end of comment found */
		else 
			prev = nextch;
	}		
}

void expand_char(char * tok, int c)
{
	char * s;

	switch (c) {
	case '.':
		s = "DOT";
		break;
	case ',':
		s = "COMMA";
		break;
	case ';':
		s = "SEMICOLON";
		break;
	case ':':
		s = "COLON";
		break;
	case '*':
		s = "STAR";
		break;
	case '+':
		s = "PLUS";
		break;
	case '-':
		s = "MINUS";
		break;
	case '|':
		s = "BAR";
		break;
	case '?':
		s = "QUEST";
		break;
	case '!':
		s = "EXCLAM";
		break;
	case '=':
		s = "EQUALS";
		break; 
	case '/':
		s = "SLASH";
		break; 
	case '\\':
		s = "BACKSLASH";
		break; 
	case '%':
		s = "PERCENT";
		break; 
	case '&':
		s = "AMPERSAND";
		break; 
	case '^':
		s = "CARET";
		break; 
	case '~':
		s = "TILDE";
		break; 
	case '@':
		s = "AT";
		break; 
	case '#':
		s = "HASH";
		break; 
	case '(':
		s = "LPAREN";
		break; 
	case ')':
		s = "RPAREN";
		break; 
	case '[':
		s = "LBRACKET";
		break; 
	case ']':
		s = "RBRACKET";
		break; 
	case '{':
		s = "LBRACE";
		break; 
	case '}':
		s = "RBRACE";
		break; 
	case '<':
		s = "LESSTHEN";
		break; 
	case '>':
		s = "GREATTHEN";
		break; 
	default:
		tok[0] = c; 
		tok[1] = '\0';
		return;
	}

	strcpy(tok, s);
}

/*
   Returns next token with  string of token in lexeme[]   
   A token is: grammar-symbol or '|' or  '\n' ':' ';'  EOF		
*/

int GetToken(void) 
{
	/* skip white space and comment */
    while( isspace(nextch)  || ('/' == nextch)) {
		if (nextch == '\n') {			
			lineno++;
		}
		/* skip comment */
		if( nextch == '/') {			
		    nextch=getc(infile);
			if(nextch == '/')
			{ /* eat to end of line */
				do { nextch=getc(infile); 
					} while(nextch != '\n');
					lineno++;
			}
			else if(nextch == '*')
				scanComment();
			else  {				
				return('/');
			}
		}			
		nextch = getc(infile);
    }
		
    if (nextch == EOF) 	return( T_EOF);
   		
	if (isalpha (nextch) || nextch == '_' ) /*scan identifier */
	{	char *s = lexeme;        
		do {
				*s = nextch; s++;
				nextch = getc(infile);				
		} while( isalpha(nextch) || isdigit( nextch ) || (nextch == '_' ) );
		*s = '\0';
		return ID;
	}

	switch (nextch)
	{
		case  ':'  :  nextch = getc(infile); return COLON;
		case  ';'  :  nextch = getc(infile); return SEMI;
		case  '|'  :  nextch = getc(infile); return BAR;
		case  '('  :  nextch = getc(infile); return LPAREN;
		case  ')'  :  nextch = getc(infile); return RPAREN;		
		case  '*'  :  nextch = getc(infile); return STAR;		
		case  '?'  :  nextch = getc(infile); return QUEST;						
		case  '+'  :  nextch = getc(infile); return PLUS;						
		case  '{'  :  nextch = getc(infile); return LBRACE;
		case  '}'  :  nextch = getc(infile); return RBRACE;		
		case  ','  :  nextch = getc(infile); return COMMA;		
		case  '%'  :  nextch = getc(infile);
					  if (nextch == '%') {
						nextch = getc(infile);
						strcpy(lexeme, "%%" );
						return END_SPEC;
					  } else if(isalpha(nextch)) {
						char *s = lexeme;    			
						do {
							*s++ = nextch;
							nextch = getc(infile);				
						} while(isalpha(nextch));
						*s = '\0';			
						if (strcmp(lexeme, "token") == 0) {
							return TOKEN_SPEC;
						} else { 
							SyntaxError("Unknown directive");
							return( BAD_TOKEN );
						}
					  } else
					  return( BAD_TOKEN );
		
		case '\"': /*multichar token "token" or single char "c" */
			{
			char *s = lexeme;
			while (nextch != EOF) {
				nextch = toupper(getc(infile));				
				while (nextch <= ' ' )
					nextch = toupper(getc(infile));				
				if (nextch == '\"'){
					nextch = getc(infile); 
					break;
				}
				*s = nextch; s++;				
			};				
			*s = '\0';
			if (strlen(lexeme) == 1) { /*single char*/
				expand_char(lexeme, lexeme[1]);
				return CHR_LIT;
			}
			else
			  return ID;	
			} 
		/* single char token 'c'*/
		case '\'': 
			expand_char(lexeme, getc(infile));
			nextch = getc(infile);
			if (nextch != '\'') {
				SyntaxError("Single char token must be terminated with \'.");	
				return(BAD_TOKEN);
			} else {
				nextch = getc(infile);
				return CHR_LIT;
			}
	}						
    return BAD_TOKEN;
}


int lookahead;

void match(int t) 
{
	 if(lookahead == t) 
		 lookahead = GetToken();
	 else 
		 SyntaxError("bad token");
}

void RegisterRule(SYMPTR lhsp, int numrhs, SYMPTR rhs[], int first_alternate);
SYMPTR  ParseGroup(SYMPTR lhsp, int pos); 
int ParseAction(SYMPTR lhsp, SYMPTR rhs[], int pos);
void elemenlist(SYMPTR lhsp, int first_alternate); 
void productionrule(SYMPTR lhsp, int first_alternate); 
void grammar_rules();
void tokenlist();
void tokenspec() ;

void tokenlist() 
{
	SYMPTR lhsp;
	switch (lookahead) 
	{
	case END_SPEC: case CHR_LIT: case ID: 
		while (lookahead == CHR_LIT || lookahead == ID ) { 			
				lhsp = Lookup( lexeme );			
				if (lhsp != last_sym) {
					fprintf(stderr,"token %s declared twice\n", lexeme );
				}
				match(lookahead); 
		}
		break;
	default:
		SyntaxError("bad tokenlist");
	}
} /* end of tokenlist*/


void grammar_rules() 
{
	SYMPTR lhsp;
	int first_alternate;
	switch (lookahead) {
	case T_EOF: case ID: 
		while (lookahead == ID ) 
		{ 			
			lhsp = Lookup(lexeme);			
			if (lhsp->kind == NONTERM) {
				/* non-terminal is defined before*/
				first_alternate = false;
			} else {
				/* register non-terminal */
				lhsp->kind = NONTERM;				
				first_alternate = true;
			}
			match(ID); 
			match(COLON); 
			productionrule(lhsp,  first_alternate); 
			match(SEMI); 
		}
		break;
	default:
		SyntaxError("production must start with identifier");
	}
	
} /* end of grammar_rules*/


void productionrule(SYMPTR lhsp, int first_alternate) 
{
	switch (lookahead) {
	case CHR_LIT: 
	case ID: 
	case SEMI: 
	case BAR: 
	case LPAREN: 
	case RPAREN: 
	case LBRACE: 
		elemenlist(lhsp,  first_alternate); 
		while (lookahead == BAR ) { 
				match(BAR); 
				first_alternate = false;
				elemenlist(lhsp,  first_alternate); 
		}
		break;

	default:
		SyntaxError("productionrule");
	}
} /* end of productionrule*/


void elemenlist(SYMPTR lhsp, int first_alternate) 
{
	SYMPTR newsym,temprhs[128];
	int numrhs = 0;
	
	switch (lookahead) {
		case CHR_LIT: 
		case ID: 
		case SEMI: 
		case BAR: 
		case LPAREN: 
		case RPAREN:  
		case LBRACE:  
		while (lookahead == CHR_LIT || lookahead == ID || 
			   lookahead == LPAREN || lookahead == LBRACE) { 
			if (lookahead == CHR_LIT || lookahead == ID ) {
				temprhs[numrhs++] = Lookup(lexeme);
				match(lookahead); 
			} else if (lookahead == LBRACE) {
				numrhs += ParseAction(lhsp, &temprhs[numrhs], numrhs + 1); 
			} else if (lookahead == LPAREN) {
				newsym = ParseGroup(lhsp, numrhs + 1); 
				temprhs[numrhs++] = newsym;
			}
		}
		RegisterRule(lhsp, numrhs, temprhs, first_alternate);
		break;

	default:
		SyntaxError("bad rhs symbol ");
	}
} /* end of elemenlist*/

SYMPTR actionlist(SYMPTR lhsp)
{
	SYMPTR actp;

	actp = Lookup(lexeme);
	actp->kind = ACTION;				

	return actp;

} /* end of actionlist*/

int ParseAction(SYMPTR lhsp, SYMPTR rhs[], int pos) 
{
	int cnt = 0;

	match(LBRACE); 

	switch (lookahead) {
	case ID: 
		rhs[cnt++] = actionlist(lhsp);
		match(lookahead); 
		while (lookahead == COMMA) { 
			match(COMMA); 
			rhs[cnt++] = actionlist(lhsp);
			match(lookahead); 
		}
	case RBRACE: 
		break;
	default:
		SyntaxError("syntax action");
	}

	match(RBRACE); 

	return cnt;
} 

void RegisterRule(SYMPTR lhsp, int numrhs, SYMPTR rhs[], int first_alternate)
{
	
	/* get memory for numrhs symbols + one symbol that will be apennded later,
	   if regex group is repetition */
	RULEPTR rp = (RULEPTR)calloc( 1, (unsigned)( sizeof(*rp)+ (numrhs+1)*sizeof(rp->rhs[0]) ) );
		
	rp->rule_no = num_rules++;
	rp->lhs = lhsp;
	rp->nrhs = numrhs;
	rp->next_rule = NULL;
	rp->lineno = lineno;
	/* copy rhs */	
	while( numrhs-- > 0 )	rp->rhs[numrhs] = rhs[numrhs];
		
	/* put at end of rule list */
	if (last_rule == NULL)	
		first_rule = rp;
	else
		last_rule->next_rule = rp;
	last_rule = rp;
		
	/* register alternate productions */
	if (first_alternate) {
		lhsp->firstalt = rp;
		lhsp->numalt=1;
	}
	else
		lhsp->numalt++;
}


SYMPTR  ParseGroup(SYMPTR lhsp, int pos) 
{
	char *name, strpos[12];	
	SYMPTR newlhsp;
	RULEPTR rp;
	int has_empty;
	
	/* Algorithm:
		1. make name by adding (_$+rhs_pos+numalt) to name of lhs symbol
		2. generate new nonterminal - newlhsp with GROUP attribute
		3. Parse inside group
		4. if group ends with )? or )*.
		      1. change atribute to OPTION or REPEAT
	          2. generate sub rules for these regex
	*/
	
	sprintf(strpos, "%d%d",pos, lhsp->numalt);
	name = malloc(strlen(lhsp->symtext) + strlen(strpos)+ 3);
	strcpy(name, lhsp->symtext);
	strcat(name,"$");
	strcat(name,strpos);
	
	newlhsp = Lookup(name);
	if (newlhsp->kind == NONTERM) {
				/* non-terminal is defined before*/
				fprintf(stderr, "Do not use symbol with sufix _$");
				exit(1);
	}
	/* register non-terminal as GROUP */
	newlhsp->kind = NONTERM;				
	newlhsp->rextype = GROUP;

	/* start scanning inside parentheses */	
	/*******************************************************/
	match(LPAREN); 
	
	/* start scanning inside group with first alternative of newlhsp */
	
	productionrule(newlhsp, /*first_alternate*/  true); 
	
	match(RPAREN); 
	
	/* first check if the group has no an empty production */
	
	has_empty=false;
	
	forall_rules(rp) {
		if (rp->lhs->sym_no != newlhsp->sym_no) 
			continue;
		if (rp->nrhs == 0 ){
			has_empty = true; 
			break;
		}
	}		
	
	if (QUEST == lookahead) {
			match(QUEST); 
			newlhsp->rextype = OPTION;					
			/* we have name_$ ->  (group)?,  and we want to have: */
			/*      name_$ -> empty | group,  */
			/* so, add empty production  but only if it is not in the group*/			
			if(has_empty == false)
				RegisterRule(newlhsp, 0, 0, false);		
	} else if (STAR == lookahead ) {
			match(STAR); 
			newlhsp->rextype = REPEAT;					
			/* we have name_$ -> (group)*,  and we want to have */
			/*      name_$ -> empty | group name_$,     */
			/* 1. add production name_$ -> group name_$ to nonempty production */
			/* 2. add empty production,( but only if it is not in the group */
			/*    - this way everything is ok even if group has empty prod. ) */
			
			forall_rules(rp) {
				if (rp->lhs->sym_no != newlhsp->sym_no) 
					continue;			
				if(rp->nrhs != 0 ) /*not empty*/ 
				{	/* concat newlhs - we reserved place for it  before*/
					rp->rhs[rp->nrhs] = newlhsp;
					rp->nrhs++;
				}				
			}
			/* add empty production */
			if(has_empty == false)
				RegisterRule(newlhsp, 0, 0, false);		
	} else if (PLUS == lookahead) { /* repeat one or more times*/ 
			SYMPTR restsp;
			SYMPTR restRhs[1];
			newlhsp->rextype = REPEAT1;					
			match(PLUS); 

			/* we have name_$ -> (group)+,  and we want to have bnf
			        name_$ -> group name_$_rest, 
					name_$_rest -> empty | group name_$_rest, 
			   or: 
					name_$ -> group name_$_rest, 
					name_$_rest -> empty | name_$ , 			
			so we must:  */
			/* 1. make new symbol name_$_rest (NONTERM, GROUP)
			   2. append name_$_rest to rhs of name_$
			   3. add production, name_$_rest -> group name_$_rest, 
			      and set it as first alternative
			   4. add empty production name_$_rest ->  
				   (LL1 problem if group is empty)
			   5. make warning if group has empty
		    */
			name = malloc(strlen(newlhsp->symtext) + 5);
			strcpy(name, newlhsp->symtext);	strcat(name,"rest");
			restsp = Lookup(name);
			/* register non-terminal as REST - we do not generate  code for REST */
			restsp->kind = NONTERM;				
			restsp->rextype = REST;
			restRhs[0]=newlhsp;
			/* register  name_$ | empty */ 			
			RegisterRule(restsp, 1, restRhs, /*first_alt*/ true);		
			RegisterRule(restsp, 0, /*empty*/ 0, /*first_alt*/ false);		
			
			/* append name_$_rest to rhs of name_$ */
			forall_rules(rp) {
				if (rp->lhs->sym_no != newlhsp->sym_no) 
					continue;			
				if(rp->nrhs != 0 ) /*not empty*/ 
				{	/* concat newlhs - we reserved place for it  before*/
					rp->rhs[rp->nrhs] = restsp;
					rp->nrhs++;
				}				
			}
		}  
	
	return newlhsp;	
	
} /* end of group*/

void sort_symbols(void)
{
    SYMPTR sp;
    int sym_no = 0;
	
	forall_symbols(sp) {
		if (sp->kind == TERM)
			sp->sym_no = sym_no++;
	}

	forall_symbols(sp) {
		if (sp->kind == NONTERM)
			sp->sym_no = sym_no++;
	}
		
	forall_symbols(sp) {
		if (sp->kind == ACTION)
			sp->sym_no = sym_no++;
	}
}

int parse_grammar(const char * fname) 
{	
	if ((infile = fopen(fname, "rt")) == NULL) {
		fprintf(stderr, "cannot read %s\n", fname);
		return -1;
	} 

	lookahead = GetToken();
	switch (lookahead) {
	case T_EOF: 
	case TOKEN_SPEC: 
	case ID: 
		if (TOKEN_SPEC == lookahead) {
			match(TOKEN_SPEC); 
			tokenlist(); 
			match(END_SPEC); 
		}
		grammar_rules(); 
		match(T_EOF); 
		break;
	default:
		SyntaxError("start with grammar rule");
	}

	fclose(infile);

	sort_symbols();

	return 0;
} 

/* end of grammar*/

