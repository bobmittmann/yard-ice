/**************************************************************
	LL(1) Predictive Parser Table Generator and RDP Generator
 **************************************************************

Author: Ivo Mateljan 

This is simple recursive descent parser generator. It also can be used to build
LL(1) predictive parsing tables. I have made it for my students.

Version 0.2.			1 September 2003.


USAGE:
  ell  [-l]  [-f] [-t]  [file]
	  
OPTIONS:
		
-t :	generate output of machine-readable parsing tables.
		this disable generation of recursive descent parser code,
		which is a default action

-l :	generate listing of terminals, nonterminals and grammar 
		on the standard output.
	  
-s :	generate listing of `first', 'follow' and 'predict' sets
		as computed for the grammar.	
		
			  
file :	the name of the file containing the grammar.  If this
		argument is absent, the grammar is read from the
		standard input.
			
RESULT:				  
	If the grammar is free of errors and if the `-t' flag is not
	given, ll(1) recursive descent parser code is generated in a file named
	`file.c', where `file' is the name of the input file holding
	the grammar. 
	If the grammar was read from the standard input, the tables are written 
	to a file named: `llparse.c'.

	If the grammar is free of errors and if the `-t' flag is given, 
	ll(1) parsing tables are generated in a file named
	`file.tbl', where `file' is the name of the input file holding
	the grammar. If the grammar was read from the standard input, the 
	tables are written to a file named: `llparse.tbl'.
					
	If the grammar contains errors, diagnostic information is given.
	
Gramar file format is extremly simple:
----------------------------------------------------------------------
%token  list_of_token
%%
list_of_production_rules

Every production start in a new line and have the form:

LHS : RHS1               or  LHS : RHS1 | RHS2| ...| RHSn ;
	| RHS2
	|  ....
    ;

LHS is single nonterminal. It is defined with alternate RHSi. One RHSi can be empty. 
RHSi contain series of tokens and nonterminals which can be grouped, 
as regular expressions:
    
	()  - group                              i.e. sign : ('+' | '-' |  ) 
	()? - group is optional                  i.e. sign : ('+' | '-')? 
	()* - group repeats zero or more times,  i.e. expr : term ( ('+' | '-') term )*
	()+ - group repeats one or more times,   i.e. expr : (statement )+

Rule is terminated with semicolon.

Nonternimals and tokens are designates with : (alpha|_)(alpha|digit|_)*
Tokens can also be are designated with single quoted char (i..e '+', '\n' )

Coments are in C++ style  
	 multiline comments  -  can not be nested
	// to end of line comment

To-do-list:
- proper error recovery
- semantic actions
- ebnf implementation, but retain possibility of predictive parser table generation
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>

#define FALSE	 0
#define TRUE     1
/* symbol -> kind */
#define TERM     0
#define NONTERM  1
#define ACTION 	 2
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


void WriteRecursiveParser(FILE *fp,char *pname, FILE *fh, char *hname);

typedef int *BITVEC;
int	bitvecsize;		    /* number of elements in a BITVEC */

typedef struct _symbol {	    /* Description of grammar symbol */
	struct  _symbol *next_sym;  /* a link */
	int 	sym_no;		        /* symbol number */
	int  	kind;		        /* terminal or non-terminal */
	int 	rextype;            /* regex type - GROUP, REPEAT, OPTION */
	int 	nullable;           /* if can generate empty string*/
	int 	numalt;
	struct  _rule *firstalt;    /* ptr to first of alternate rules */	
	BITVEC 	first;
	BITVEC 	follow;
	char 	*symtext;	        /* text of symbol */
	unsigned int r_offs;		/* offset in the rules table */
	unsigned int r_cnt;			/* number of rules */
} SYM, *SYMPTR; 

typedef struct _rule {	        /* Description of a production rule */
	struct _rule *next_rule;
	int	rule_no;	            /* production number */
	int	nrhs;		            /* number of symbols on RHS - empty pr. has nrhs=0*/		
	int 	conflict;
	int 	lineno;
	BITVEC	predict;            /* prediction set */
	SYMPTR	lhs;	            /* symbol on LHS */
	SYMPTR	rhs[1];		        /* symbols on right-hand side */
} RULE, *RULEPTR;

RULEPTR  first_rule, last_rule;
int	num_rules = 0;

SYMPTR	first_sym = NULL, last_sym = NULL;
int	lastsymnum = 0;

char	*filename;		/* name of grammar input file */
FILE	*infile;		/* the input file itself */

int	conflict = NO_CONFLICT;
int	listing = FALSE;
int	dump_sets = FALSE;
int	tables = FALSE;
int	code = FALSE;

int nextch = ' ';

/* Define looping constructs for the main lists in the program */

#define forall_rules(r)   for(r=first_rule;r!=NULL;r=r->next_rule)
#define forall_symbols(s) for(s=first_sym;s!=NULL;s=s->next_sym)

/* Package of bit vector routines */
BITVEC BitVecNew() 
{
    BITVEC vp = (BITVEC ) calloc( (unsigned)bitvecsize, sizeof(int) );
	if(vp == NULL) {fprintf(stderr, "\nMemory error\n"); exit(1);}
	/* bitvecsize = (lastsymnum + 31) >> 5; */
    return( vp );
}

void BitVecSetSize() 
{        
    bitvecsize = (lastsymnum + 31) >> 5;    
}

int BitVecUnion(BITVEC x,  BITVEC y)
{
    int i,old, changed=FALSE;
    for( i=0; i < bitvecsize; i++ )
	{ 
		old = x[i];
		x[i]  |=  y[i];
		if(!changed)
			changed = (x[i] != old);
	}
	return changed;
}

int BitVecEmpty(BITVEC x)
{
    int i;
    for( i=0; i < bitvecsize; i++ )
	{ 		
		if (x[i])
			return FALSE;
	}
	return TRUE;
}

int BitVecHasIntersection(BITVEC x,  BITVEC y)
{
    int i;
    for( i=0; i < bitvecsize; i++ )
	{ 		
		if (x[i]  & y[i])
			return TRUE;
	}
	return FALSE;
}

void BitVecClear(BITVEC x)
{
    int i;
    for( i=0; i < bitvecsize; i++ )	
		x[i] =0;
	
}

int BitVecTest(BITVEC x, int pos)
{
	return ( x[pos >> 5] & (1 << (pos & 31)) );
}

int BitVecNumEl(BITVEC x)
{
	int i,num;
	num=0;
	for( i=0; i < lastsymnum; i++ )	
	{
		if( BitVecTest(x, i) ) 
		  num++;
	}
	return num;
}

/* x >> 5 is iqual to x/32 */
/* 31 is equal to 0x1F */
/* return true if change */

int BitVecAdd(BITVEC x, int pos)
{
    int old = BitVecTest(x, pos);
	x[pos >> 5] |= (1 << (pos & 31));
	return old != BitVecTest(x, pos);
}

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
	if(sp == NULL) {fprintf(stderr, "\nMemory error\n"); exit(1);}
    sp->symtext = (char *)malloc(strlen(s)+1);
	if(sp->symtext == NULL) {fprintf(stderr, "\nMemory error\n"); exit(1);}
	strcpy(sp->symtext,s);
    sp->next_sym = NULL;
    sp->kind = TERM;     /* assume: terminal */
	sp->rextype = SINGLE;
    sp->nullable = FALSE;	/* assume: not nullable */
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
#define NL          '\n'

#define  MAXLENGTH   255

char lexeme[MAXLENGTH +1];  
int lineno = 1;
int parse_error = 0; /*unused */

void SyntaxError(char * str)
{
	fprintf(stderr, "\nError: %s in line %d\n", str, lineno);
	exit(1);
}

static void scanComment()
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

/*
   Returns next token with  string of token in lexeme[]   
   A token is: grammar-symbol or '|' or  '\n' ':' ';'  EOF		
*/

int GetToken() 
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

		case  '%'  :  nextch = getc(infile);
					  if(nextch == '%') {
						nextch = getc(infile);
						strcpy(lexeme, "%%" );
						return END_SPEC;
					  }
					  else if(isalpha(nextch)) {
						char *s = lexeme;    			
						do {
							*s++ = nextch;
							nextch = getc(infile);				
						} while(isalpha(nextch));
						*s = '\0';			
						if(strcmp(lexeme, "token") == 0)
							return TOKEN_SPEC;
						else { 
							SyntaxError("Unknown directive");
							return( BAD_TOKEN );
						}
					  } else
					  return( BAD_TOKEN );
		
		case '\"': /*multichar token "token" or single char "c" */
			{
			char *s = &lexeme[2];        			
			lexeme[0]='T'; lexeme[1]='_';
			while(nextch != EOF) {
				nextch = toupper(getc(infile));				
				while (nextch <= ' ' )
					nextch = toupper(getc(infile));				
				if(nextch == '\"'){
					nextch=getc(infile); 
					break;
				}
				*s = nextch; s++;				
			};				
			*s = '\0';
			if(strlen(lexeme) == 3) { /*single char*/
				lexeme[0]=lexeme[2]; lexeme[1]='\0';
				return CHR_LIT;
			}
			else
			  return ID;	
			} 
		/* single char token 'c'*/
		case '\'': 
			nextch = getc(infile);
			lexeme[0] = nextch; lexeme[1] = '\0';
			nextch = getc(infile);
			if(nextch != '\'') {
				SyntaxError("Single char token must be terminated with \'.");				
				return(BAD_TOKEN);
			}	
			else {
				nextch = getc(infile);
				return CHR_LIT;
			}
	}						
    return( BAD_TOKEN );
}


int lookahead;

void match(int t) 
{
	 if(lookahead == t) lookahead = GetToken();
	 else SyntaxError("bad token");
}

void RegisterRule(SYMPTR lhsp, int numrhs, SYMPTR rhs[], int first_alternate);
SYMPTR  ParseGroup(SYMPTR lhsp, int pos); 
void elemenlist (SYMPTR lhsp, int first_alternate); 
void productionrule (SYMPTR lhsp, int first_alternate); 
void grammar_rules();
void tokenlist();
void tokenspec() ;

void ParseGrammar() 
{	
	lookahead = GetToken();
	switch (lookahead) {
	case T_EOF: case TOKEN_SPEC: case ID: 
		if( TOKEN_SPEC == lookahead) {
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
} /* end of grammar*/

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
				first_alternate = FALSE;
			}
			else {
				/* register non-terminal */
				lhsp->kind = NONTERM;				
				first_alternate = TRUE;
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
	case CHR_LIT: case ID: case SEMI: case BAR: case LPAREN: case RPAREN: 
		elemenlist(lhsp,  first_alternate); 
		while (lookahead == BAR ) { 
				match(BAR); 
				first_alternate = FALSE;
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
		case CHR_LIT: case ID: case SEMI: case BAR: case LPAREN: case RPAREN:  
		while (lookahead == CHR_LIT || lookahead == ID || lookahead == LPAREN ) 
		{ 
			if (lookahead == CHR_LIT || lookahead == ID ) {
				temprhs[numrhs++] = Lookup(lexeme);
				match(lookahead); 
			}						
			else if(lookahead == LPAREN ) {
				newsym = ParseGroup(lhsp, numrhs+1); 
				temprhs[numrhs++] = newsym;
			}
		}
		RegisterRule(lhsp, numrhs, temprhs, first_alternate);
		break;

	default:
		SyntaxError("bad rhs symbol ");
	}
} /* end of elemenlist*/

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
	strcat(name,"$");strcat(name,strpos);
	
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
	
	productionrule(newlhsp, /*first_alternate*/  TRUE); 
	
	match(RPAREN); 
	
	/* first check if the group has no an empty production */
	
	has_empty=FALSE;
	
	forall_rules(rp)
	{
		if(rp->lhs->sym_no != newlhsp->sym_no) continue;
		if(rp->nrhs == 0 ){
			has_empty = TRUE; 
			break;
		}
	}		
	
	if (QUEST == lookahead) {
			match(QUEST); 
			newlhsp->rextype = OPTION;					
			/* we have name_$ ->  (group)?,  and we want to have: */
			/*      name_$ -> empty | group,  */
			/* so, add empty production  but only if it is not in the group*/			
			if(has_empty == FALSE)
				RegisterRule(newlhsp, 0, 0, FALSE);		
	}
	else if (STAR == lookahead ) {
			match(STAR); 
			newlhsp->rextype = REPEAT;					
			/* we have name_$ -> (group)*,  and we want to have */
			/*      name_$ -> empty | group name_$,     */
			/* 1. add production name_$ -> group name_$ to nonempty production */
			/* 2. add empty production,( but only if it is not in the group */
			/*    - this way everything is ok even if group has empty prod. ) */
			
			forall_rules(rp)
			{
				if(rp->lhs->sym_no != newlhsp->sym_no) continue;			
				if(rp->nrhs != 0 ) /*not empty*/ 
				{	/* concat newlhs - we reserved place for it  before*/
					rp->rhs[rp->nrhs] = newlhsp;
					rp->nrhs++;
				}				
			}
			/* add empty production */
			if(has_empty == FALSE)
				RegisterRule(newlhsp, 0, 0, FALSE);		
	}	
	else if (PLUS == lookahead) { /* repeat one or more times*/ 
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
			RegisterRule(restsp, 1, restRhs, /*first_alt*/ TRUE);		
			RegisterRule(restsp, 0, /*empty*/ 0, /*first_alt*/ FALSE);		
			
			/* append name_$_rest to rhs of name_$ */
			forall_rules(rp)
			{
				if(rp->lhs->sym_no != newlhsp->sym_no) continue;			
				if(rp->nrhs != 0 ) /*not empty*/ 
				{	/* concat newlhs - we reserved place for it  before*/
					rp->rhs[rp->nrhs] = restsp;
					rp->nrhs++;
				}				
			}
		}  
	
	return newlhsp;	
	
} /* end of group*/


void WriteGrammar(FILE *fp) 
{
    RULEPTR rp;
    SYMPTR sp;
    int j;
	
    puts( "Terminal Symbols:\n" ); 
	forall_symbols( sp ) {
		if (sp->kind == TERM)
			fprintf(fp, "%d: %s  ", sp->sym_no, sp->symtext );
	}
		
	puts( "\n\nNon-Terminal Symbols:\n" ); 
	forall_symbols( sp ) {
		if (sp->kind == NONTERM) {
			fprintf(fp, "%d: %s  ", sp->sym_no, sp->symtext );			
		}
	}
	puts( "\n\nProduction Rules:" );
	forall_rules( rp ) {			
		fprintf(fp,"\n%3d.\t%s :=", rp->rule_no, rp->lhs->symtext );
		for( j=0; j < rp->nrhs; j++ ) 
			fprintf(fp, " %s", rp->rhs[j]->symtext );		
	}
	fprintf(fp, "\n");
}

void FindNullableSymbols() 
{ 
	int i;
	RULEPTR rp;
	SYMPTR sp;
	int changed;
	
	forall_symbols( sp ) 	sp->nullable = FALSE;      
	
	/* this version assume that there is no epsilon symbol, but */
	/* pure null productions have rp->nrhs =0 */	
	
	forall_rules( rp ) 	if( rp->nrhs == 0) 	rp->lhs->nullable = TRUE;
	
	/* find all productions that generate empty string */
	do
	{
		changed = 0;		
		forall_rules( rp ) 
		{
			if( rp->lhs->nullable == TRUE) continue;
			
			for(i=0; i<rp->nrhs; i++)
			{
				if( rp->rhs[i]->nullable == FALSE) break;
			}			
			if( i==rp->nrhs ) /* means that all are nullable - this also cover empty*/
			{
				rp->lhs->nullable = TRUE;
				changed = 1;
			}
		}		
	} while( changed );	
}


void ComputeFIRST() 
{
	int i;
	RULEPTR rp;
	SYMPTR sp;
	int changed;
	
	/* first set of terminal symbol contains only that terminal symbol */
    forall_symbols( sp ) {
		sp->first = BitVecNew();
		if(sp->kind==TERM)
			BitVecAdd(sp->first, sp->sym_no );
    }
	
	/* Now compute all first sets for nonterminals*/
	do
	{
		SYM *s1, *s2;
		changed = 0;
		
		forall_rules( rp ) 
		{    
			s1 = rp->lhs; 
			for(i=0; i<rp->nrhs; i++)
			{
				s2 = rp->rhs[i]; 
				
				if( s2->kind == TERM )
				{          
					changed += BitVecAdd(s1->first, s2->sym_no);
					break;
				}
				else if( s1==s2 )
				{
					if( s1->nullable== FALSE) break;
				}
				else
				{ 
					changed += BitVecUnion(s1->first, s2->first);			  
					if( s2->nullable== FALSE ) break;
				}
			}
		}
	}while( changed );    
}

void ComputeFOLLOW() 
{
    int i, j, changed;
    RULEPTR rp;
    SYMPTR sp, tp;
	
    forall_symbols( sp ) sp->follow = BitVecNew();
    
	do 
	{		
		changed = 0;
		forall_rules( rp ) 
		{									             
			for( i = 0;   i<rp->nrhs; i++ ) 
			{
				sp = rp->rhs[i];
				if(sp->kind == TERM) continue;
				
				for( j=i+1; j < rp->nrhs; j++ ) 
				{
					tp = rp->rhs[j];
					
					if(tp->nullable == TRUE) {
						changed += BitVecUnion(sp->follow, tp->first);			  
						continue;
					}
					else {
						changed += BitVecUnion(sp->follow, tp->first);			  
						break;
					}
				}
				
				if(j == rp->nrhs) /* means all nullable after i*/
				{
					changed += BitVecUnion(sp->follow, rp->lhs->follow);			  
				}						
			}		
		}
		
	}while (changed);
}


void ComputePREDICT()
{
/* 
	Build Predictive set which is defined as:
	P(A->alpha) =  FIRST(alpha) U FOLLOW(A),   	if nullable(alpha)==true
              	 =  FIRST (alpha),                   	else.      
	nullable(alpha)==true means that if alpha = X1 X2 .. Xn, then all Xi are nullable
*/

	RULEPTR rp;
	SYMPTR sp; 
	int i, all_nullable;
	
	forall_rules( rp ) rp->predict = BitVecNew();
	
	forall_rules( rp ) 
	{		
		all_nullable = TRUE;
		
		for(i=0; i <rp->nrhs; i++)
		{
			sp = rp->rhs[i];		   			
			BitVecUnion(rp->predict, sp->first);
			
			if(sp->nullable == FALSE) {
				all_nullable = FALSE;
				break;
			}
		}		
		if(all_nullable)		
			BitVecUnion(rp->predict, rp->lhs->follow);
	}	
}

/*
For predictive parsing we must have 
1. no left recursion
2. no common left prefixes

Equivalently, using predictive set:
	1. For every production Predict(A->alpha) must not be empty
	2. For alternate production
		Predict(A->alpha) intesection Predict(A->beta) = empty set

Equivalently, using first and follow sets:
For every pair of productions A-> alpha | beta
	1. First(alpha) and First(beta) must be disjoint
	2. alpha and beta can be nullable but not both
	3. if nullable(beta) then First(alpha) and Follow(A) must be disjoint

*/

int TestLL1conflict(FILE *fp)
{
	
	RULEPTR rp;
	SYMPTR sp; 
	BITVEC  set = BitVecNew();
	
	conflict = NO_CONFLICT;	
	
	/* test imedite left recursion */
	forall_rules( rp ) 
	{
		if(rp->nrhs > 1)
		if(rp->lhs->sym_no == rp->rhs[0]->sym_no) {
				fprintf(fp,"\nLeft recursion in rule:%d, line:%d ", rp->rule_no, rp->lineno);
				conflict = conflict | LR_CONFLICT;
				rp->conflict = conflict;
		}
	}

	forall_symbols( sp ) /* for all nonterminal test LL(1) conflict */
	{
		if(sp->kind != NONTERM)  continue;   
		if(sp->kind == NONTERM)
		{
			/* look for rules begining with nonterminal sp*/
			BitVecClear(set);			
			forall_rules( rp ) 
			{				
				if(rp->lhs->sym_no == sp->sym_no)
				{				
					/* 1st condition  - usually result of left recursion*/
					if(BitVecEmpty(rp->predict)) 
					{
						fprintf(fp,"\nCan\'t predict rule:%d, line:%d", rp->rule_no, rp->lineno);
						conflict = conflict | LR_CONFLICT;
						rp->conflict = conflict;
					}
					/* 2nd condition - result of common left prefixes*/
					if (BitVecHasIntersection(set, rp->predict ))
					{
						fprintf(fp,"\nConflict in rule:%d, line:%d", rp->rule_no, rp->lineno);
						conflict = conflict | CLP_CONFLICT;
						rp->conflict = conflict;
					}
					BitVecUnion(set, rp->predict);					
				}
			}			
		}
	}
	
	free(set);	
	return conflict;
}

void WriteLLTable(FILE *fp)
{	
	RULEPTR rp;
	SYMPTR sp,tp; 
	BITVEC  set = BitVecNew();
	
	/* dont write table if conflict*/
	if(conflict) return;
	
	fprintf(fp, "\nLL(1) Table\n");
	
	forall_symbols( sp ) /* for all nonterminal write  */
	{
		if(sp->kind != NONTERM)  continue;   
		if(sp->kind == NONTERM)
		{
			fprintf(fp, "\n %d(%s)->",sp->sym_no,sp->symtext);
			/* look for rules begining with nonterminal sp*/
			/* and print predict set (terminal,prod.no)*/
			BitVecClear(set);
			forall_rules( rp ) 
			{
				if(rp->lhs->sym_no == sp->sym_no)
				{				
					forall_symbols( tp )
					{
						if(BitVecTest(rp->predict, tp->sym_no))						
							fprintf(fp,"(T:%d,R:%d) ", tp->sym_no, rp->rule_no);												
					}					
				}
			}
		}
	}
	
	free(set);
	fprintf(fp,"\n");
}


void WriteLLTableCode(FILE *fp)
{	
	RULEPTR rp;
	SYMPTR sp,tp; 
	BITVEC  set = BitVecNew();
	unsigned int pos = 0;
	unsigned int off = 0;
	unsigned int size = 0;

	#define SIZEOF_RULE 2
	#define SIZEOF_PROD 4

	/* dont write table if conflict*/
	if(conflict) return;
	
	fprintf(fp, "/* LL(1) Table */\n\n");

	fprintf(fp, "#include <stdint.h>\n\n");

	fprintf(fp, "struct rule {\n");
	fprintf(fp, "\tuint8_t t;\n");
	fprintf(fp, "\tuint8_t r;\n");
	fprintf(fp, "};\n\n");

	fprintf(fp, "struct prod {\n");
	fprintf(fp, "\tuint16_t off;\n");
	fprintf(fp, "\tuint8_t cnt;\n");
	fprintf(fp, "};\n\n");

	size = 0;
	fprintf(fp, "struct rule rule_lut[] = {");

	/* for all nonterminal write  */
	forall_symbols(sp) {
		unsigned int i;

		if (sp->kind != NONTERM)  
			continue;   

		off = pos;
		fprintf(fp, "\n\t/* %3d (%s) */", sp->sym_no, sp->symtext);
		/* look for rules begining with nonterminal sp*/
		/* and print predict set (terminal,prod.no)*/
		BitVecClear(set);
		i = 0;
		forall_rules( rp ) {
			if (rp->lhs->sym_no == sp->sym_no) {				
				forall_symbols( tp ) {
					if (BitVecTest(rp->predict, tp->sym_no)) {
//						fprintf(fp,"(T:%d,R:%d) ", 
//								tp->sym_no, rp->rule_no);
						if (i == 0)
							fprintf(fp,"\n\t");
						fprintf(fp,"{%3d,%3d}, ", 
								tp->sym_no, rp->rule_no);
						pos++;

						if (++i == 6)
							i = 0;
					}
				}					
			}
		}
		sp->r_offs = off;
		sp->r_cnt = pos - off;
		size += sp->r_cnt * SIZEOF_RULE;

//		fprintf(fp, "\n/* offs=%d cnt=%d */\n", off, pos - off);
	}

	fprintf(fp, "\n};\n\n");

	fprintf(fp, "/* %d bytes; */\n\n", size);

	size = 0;
	fprintf(fp, "struct prod prod_lut[] = {");
	/* for all nonterminal write  */
	forall_symbols(sp) {
		if (sp->kind != NONTERM)  
			continue;   
		fprintf(fp,"\n\t{%4d,%3d},", sp->r_offs, sp->r_cnt);
		size += SIZEOF_PROD;
	}

	fprintf(fp, "\n};\n\n");

	fprintf(fp, "/* %d bytes; */\n\n", size);

	free(set);
}

void upcase(char * dst, const char * s)
{
	char * cp;

	for (cp = (char *)s; *cp != '\0'; cp++)
		*dst++ = toupper(*cp);

	*dst = '\0';
}

void WriteSymAndGrmCode(FILE * fp) 
{    
	char s[256];
    int j;
    RULEPTR rp;
    SYMPTR sp;       
    
	fprintf(fp, "/* LL(1) Table */\n\n");
	fprintf(fp, "#include <stdint.h>\n\n");

    fprintf(fp, "/* Last symbol num:%d  Num rules:%d */\n\n", 
			lastsymnum, num_rules);	
    forall_symbols( sp ) {
		upcase(s, sp->symtext);
		fprintf(fp, "#define %c_%-20s %3d\n", 
		sp->kind==TERM? 'T':'N', s, sp->sym_no);
	}
	
	fprintf(fp, "\n\n");
    fprintf(fp, "/* Rules -- num:LHS name(num RHS symbols)-> RHS symbols */\n");	
    forall_rules( rp ) {		
		fprintf(fp, "/* %2d:%s(%d) ->", 
				rp->rule_no,rp->lhs->symtext, rp->nrhs ); 		
		for( j=0; j<rp->nrhs; j++ )
			fprintf(fp, " %s", rp->rhs[j]->symtext );		
		fprintf(fp, " */\n");
    }    

	fprintf(fp, "/*  */\n\n");
}






void WritePredictCases(FILE *fp, RULEPTR rp)
{
 /* used in WriteRecursiveParser(FILE *fp) for cases of predictng alternate productions*/

    SYMPTR sp; 			
	forall_symbols( sp ) 
	{
		if ( BitVecTest( rp->predict, sp->sym_no ) )
		{
			if(strlen(sp->symtext) == 1)
				fprintf(fp, "case \'%s\': ", sp->symtext );
			else
				fprintf(fp, "case %s: ", sp->symtext );									
		}
	}				
}

void WritePredictPredicates(FILE *fp, RULEPTR rp)
{
 /* used in WriteRecursiveParser(FILE *fp) for cases of predictng alternate productions*/

    SYMPTR sp; 		
	int first_time = 1;
	forall_symbols( sp ) 
	{
		if ( BitVecTest( rp->predict, sp->sym_no ) )
		{
			if(first_time)
			{
			if(strlen(sp->symtext) == 1) fprintf(fp, "lookahead == \'%s\' ", sp->symtext );
			else fprintf(fp, "lookahead == %s ", sp->symtext );
			first_time = 0;
			}
			else {
			if(strlen(sp->symtext) == 1) fprintf(fp, "|| lookahead == \'%s\' ", sp->symtext );
			else fprintf(fp, "|| lookahead == %s ", sp->symtext );
			}			
		}
	}				
}

void WriteFirstPredicates(FILE *fp, SYMPTR sym)
{
 /* used in WriteRecursiveParser(FILE *fp) for cases of predictng EBNF productions*/
    SYMPTR sp;
	int first_time = 1;
	forall_symbols( sp ) 
	{
		if ( BitVecTest( sym->first, sp->sym_no ) )
		{
			if(first_time)
			{
			if(strlen(sp->symtext) == 1) fprintf(fp, "lookahead == \'%s\' ", sp->symtext );
			else fprintf(fp, "lookahead == %s ", sp->symtext );
			first_time = 0;
			}
			else {
			if(strlen(sp->symtext) == 1) fprintf(fp, "|| lookahead == \'%s\' ", sp->symtext );
			else fprintf(fp, "|| lookahead == %s ", sp->symtext );
			}
		}		
	}				
}

void TabNL(FILE *fp, int tab)
{
  fprintf(fp, "\n"); while(tab--) fprintf(fp, "\t");
}

void DoExpandGroupingRules(SYMPTR sp, FILE *fp, int errorcode, int tab)
{
	int j,startalt, numalt, nrhs;	
	SYMPTR tp;
	RULEPTR rp;

	/* grouping assume that all terminals functions are defined*/
	/* so we dot need to check predict set for them */
	
	if(OPTION == sp->rextype) {
		TabNL(fp, tab);
		fprintf(fp, "if ("); WriteFirstPredicates(fp, sp); fprintf(fp, ") { ");
	}
	else if(REPEAT == sp->rextype)
	{	TabNL(fp, tab);
		fprintf(fp, "while (");  WriteFirstPredicates(fp, sp); fprintf(fp, ") { "); 
	}
	else if(REPEAT1 == sp->rextype)
	{	TabNL(fp, tab);
		fprintf(fp, "do  { "); ; 
	}
	
	/* write group */			
			startalt = 1;
			/*calc num alternate to print*/
			numalt = sp->numalt;
			
			if(REPEAT == sp->rextype || OPTION == sp->rextype) 
				numalt--; /*empty dont count in option and repeat*  */
			
			forall_rules( rp ) 		
			{
				if(rp->lhs != sp) continue;

				/* for nullable in repeat and option do nothing */
				if(rp->nrhs == 0 && sp->rextype != GROUP)  continue; 
				
				/* expand rhs */				
				if(REPEAT == sp->rextype || REPEAT1 == sp->rextype) 
					nrhs = rp->nrhs-1;
				else
					nrhs = rp->nrhs;				
				
				if(numalt > 1) {
					if (startalt){
						TabNL(fp, tab+1); 
						fprintf(fp, "if (");  
						WritePredictPredicates(fp, rp); 
						fprintf(fp, ") {"); 
						startalt = 0;
					} else {
						TabNL(fp, tab+1); 
						fprintf(fp, "else if (");  
						WritePredictPredicates(fp, rp); 
						fprintf(fp, ") {"); 
					}
				}
				for(j=0; j<nrhs; j++)
				{
					tp=rp->rhs[j];

					if(TERM == tp->kind )
					{   if(strlen(tp->symtext) == 1)
						{ TabNL(fp, tab+2);fprintf(fp, "match(\'%s\'); ",tp->symtext); }
						else
						{ TabNL(fp, tab+2); fprintf(fp, "match(%s); ",tp->symtext); }
					
					}
					if(NONTERM == tp->kind)
					{
						if(SINGLE == tp->rextype )
						{ TabNL(fp, tab+2); fprintf(fp, "%s(); ",tp->symtext);}
						else if(REST == tp->rextype)
							; /* we cant get here*/
						else
							DoExpandGroupingRules(tp, fp, errorcode, tab+1);						
					}
				}								
				if(numalt>1)
				{ TabNL(fp, tab+1); fprintf(fp,"}"); } /* end of alternative*/				
			}			
			
			if(sp->rextype == REPEAT1) {/* close repeat1 group */
				TabNL(fp, tab);
				fprintf(fp, "} while (");  WriteFirstPredicates(fp, sp); fprintf(fp, ");  "); 
			
			}
			else if(sp->rextype != GROUP) {/* close repeat or option group */
				TabNL(fp, tab); fprintf(fp, "}"); 
			}	
}

void WriteRecursiveParser(FILE *fp,char *pname, FILE *fh, char *hname)
{
	/* Generate code for recursive descent parser */    
	int j;
	RULEPTR rp;
	SYMPTR sp,rs; 
	int errorcode = 0;
	int tab = 1;
	
	fprintf(fp, "\n/***********  LL(1) Parser  ************/\n");
	
	/* define globals in header file */
	fprintf(fh, "#ifndef INC_PARSER_H\n");
	fprintf(fh, "#define INC_PARSER_H\n");

	fprintf(fh, "\n#ifdef USE_LEX\n");
	fprintf(fh, "\textern char *yytext;\n");
	fprintf(fh, "\tint yylex();\n");
	fprintf(fh, "\t#define getToken  yylex\n");
	fprintf(fh, "\t#define lexeme  yytext\n");
	fprintf(fh, "#else\n");
	fprintf(fh, "\tint getToken();\n");
	fprintf(fh, "\textern char *lexeme;\n");
	fprintf(fh, "#endif\n");
	fprintf(fh, "\n\nextern int lookahead;\n");
	
	/* #define token values in header file
	   Single char operators are treated with their ASCII code. We don't #define them.
	   To skip conflict with operator values add 255 to symbol-no
	*/
	
	forall_symbols( sp ) /* for all terminal*/
	{
		if(sp->kind == TERM)
		{
			if(strlen(sp->symtext) >1) /* skip single char operators*/
			fprintf(fh, "\n#define %s  %d", sp->symtext, sp->sym_no+256);		
		}
	}
	
	fprintf(fh, "\n");

	fprintf(fp, "\n#include <stdio.h>\n");
	fprintf(fp, "\n#include \"%s\"\n", hname);

	fprintf(fp, "\n\nint lookahead;\n");	

	fprintf(fp, "\nvoid SyntaxError(int code) \n{");
	fprintf(fp, "\n\tprintf(\"Error:%%d\\n\",code);");
	fprintf(fp, "\n\texit(1); \n}\n");
	fprintf(fp, "\nvoid match(int t) \n{");
	fprintf(fp, "\n\t if(lookahead == t) lookahead = getToken();");
	fprintf(fp, "\n\t else SyntaxError(-1);\n}\n");

	/* RDP functions code */
		
	forall_symbols( sp ) /* for all nonterminal write parse procedure */
	{
		if( sp->kind != NONTERM )  continue;   
		if( (NONTERM == sp->kind)  && (SINGLE == sp->rextype) )
		{			
			/* header func protos*/
			fprintf(fh, "\nvoid %s();",sp->symtext);
			/* start func defs */
			fprintf(fp, "\nvoid %s() \n{",sp->symtext);
			
			/* look for rules begining with nonterminal sp*/
			/* expand rules */
			/* and use predict set for alternative */			
            
			TabNL(fp, tab); fprintf(fp, "switch (lookahead) {");			
			
			//for all rules with lhs = sp)
			forall_rules( rp ) 		
			{
				if(rp->lhs != sp) continue;

				TabNL(fp, tab); WritePredictCases(fp, rp);
				/* expand rhs */				
				for(j=0; j<rp->nrhs; j++)
				{
					rs=rp->rhs[j];

					if(TERM  == rs->kind)
					{   if(strlen(rs->symtext) == 1){ 
							TabNL(fp, tab+1); fprintf(fp, "match(\'%s\'); ",rs->symtext);
						}
						else {
							TabNL(fp, tab+1); fprintf(fp, "match(%s); ",rs->symtext);
						}					
					}
					if( NONTERM == rs->kind )
					{
						if(rs->rextype == SINGLE) {
							TabNL(fp, tab+1); fprintf(fp, "%s(); ",rs->symtext); 
						}
						else {
							DoExpandGroupingRules(rs, fp, errorcode, tab+1);
						}
					}										
				}
				/* close case block*/
				TabNL(fp, tab+1); fprintf(fp, "break;\n");			
			}

            /*close switch*/
			TabNL(fp, tab); 
			fprintf(fp, "default:");
			TabNL(fp, tab+1); 
			fprintf(fp, "Error(%d);",errorcode++ );
			TabNL(fp, tab); fprintf(fp, "}");
			/* close procedure */
			fprintf(fp, "\n} /* end of %s*/\n", sp->symtext);
		}		
	}
	fprintf(fp,"\n");
	fprintf(fh,"\n\n#endif\n");
}


void DumpFFSets(FILE *fp) 
{
	SYMPTR sp, tp;
	
	fprintf(fp, "\nList of First, Follow sets:\n" );
	
	forall_symbols( sp ) 
	{
		if(sp->kind != NONTERM) continue;

		fprintf(fp, "\nSymbol %s    (%s)", sp->symtext,
			sp->nullable ? "nullable" : "non-nullable" );
		
		fprintf(fp, "\nFirst: ");		
		forall_symbols( tp ) {
			if ( BitVecTest( sp->first, tp->sym_no ) )
				fprintf(fp,"   %s", tp->symtext );
		}
		
		fprintf(fp, "\nFollow: ");		
		forall_symbols( tp ) {
			if ( BitVecTest( sp->follow, tp->sym_no ) )
				fprintf(fp, "  %s", tp->symtext );
		}
		fprintf(fp, "\n");
	}
}

void DumpPredictSets(FILE *fp) 
{
	SYMPTR sp;
	RULEPTR rp;
	int j;
	
	fprintf(fp, "\nList of productions and Predict sets:\n" );
		
	forall_rules( rp ) 	{		
		fprintf(fp, "%3d.\t%s ->", rp->rule_no, rp->lhs->symtext );
		for( j=0; j < rp->nrhs; j++ ) 
			fprintf(fp, " %s", rp->rhs[j]->symtext );		
		fprintf(fp, "    Predict: { "); 
		forall_symbols( sp ) {
			if ( BitVecTest( rp->predict, sp->sym_no ) )
				fprintf(fp, " %s", sp->symtext );
		}
		fprintf(fp, " } \n" );		
	}
}


void WriteSymAndGrm(FILE *tbl) 
{    
    int j;
    RULEPTR rp;
    SYMPTR sp;       
    
    fprintf(tbl, "Last symbol num:%d  Num rules:%d \n", lastsymnum, num_rules);	
    forall_symbols( sp )
		fprintf(tbl, "%d\t %c\t %s\n", sp->sym_no, 
		sp->kind==TERM? 'T':'N', sp->symtext );
	
    fprintf(tbl, "Rules -- num:LHS name(num RHS symbols)-> RHS symbols \n");	
    forall_rules( rp ) {		
		fprintf(tbl, "%d:%s(%d) ->", rp->rule_no,rp->lhs->symtext, rp->nrhs ); 		
		for( j=0; j<rp->nrhs; j++ )
			fprintf(tbl, " %s", rp->rhs[j]->symtext );		
		fprintf(tbl, "\n");
    }    
}

void PrintUsage()
{
	fprintf(stdout,"Usage: ell  [-l]  [-f] [-t]  <grammar-file>\n");
	fprintf(stdout,"Options:  -l  : print symbols and BNF grammar rules to stdout\n");
	fprintf(stdout,"          -s  : print first, follow and predict sets to stdout\n");
	fprintf(stdout,"          -t  : print predictive table to stdout\n");
	fprintf(stdout,"          -c  : generate tables as compact C code\n");
	exit(1);

}

int main(int argc,  char **argv)
{
   int i;
   char *cp;
   FILE *fout;
   FILE *fhd = NULL;
   char outname[128];
   char outheader[128];


	for( i=1; i<argc; i++ ) { /* get command line arguments */
		cp = argv[i];
		if (*cp == '-') {	/* an option */
			cp++;
			if (*cp == 's') {dump_sets = TRUE; continue;   }
			if (*cp == 'l') {listing = TRUE;   continue;   }
			if (*cp == 't') {tables = TRUE;    continue;   }
			if (*cp == 'c') {code = TRUE;    continue;   }
			fprintf(stderr, "unknown option (-%s)\n", cp);	continue;
		}
		if (filename != NULL) {
			fprintf(stderr, "only one file argument permitted\n");
			continue;
		}
		filename = cp;
	}
	
	if (filename != NULL) {
		infile = fopen( filename, "rt" );
		if (infile == NULL) {
			fprintf(stderr,"cannot read %s\n", filename);
			exit(1);
		}
	}
	else {
		PrintUsage();
	}
    
	if (tables) { /* predictive LL(1) parse table generation */
		if (filename != NULL) {
			strcpy( outname, filename );
			strcat( outname, ".tbl" );
		} else
			strcpy( outname, "llparser.tbl" );
	} else if (code) { 
		/* predictive LL(1) parse table driven code generation */
		if (filename != NULL) {
			strcpy(outname, filename );
			strcpy(outheader, filename );
			strcat(outname, "_tab.c" );
			strcat(outheader, "_tab.h" );
		} else {
			strcpy( outname, "ll_tab.c" );
			strcpy( outname, "ll_tab.h" );
		}
	} else {   /* recursive descent parser generation */
		if (filename != NULL) {
			strcpy( outname, filename );
			strcpy( outheader, filename );
			strcat( outname, "_par.c" );
			strcat( outheader, "_par.h" );
		} else {
			strcpy( outname, "llparser.c" );
		    strcpy( outname, "llparser.h" );
		}
	}

	fout = fopen( outname, "w");    /*open output file */
	if (fout == NULL) {
		fprintf(stderr, "cannot create %s\n", outname);  
		exit(1);		
	}
	
	if (!tables) {
		fhd = fopen( outheader, "w");    /*open output file */
		if (fhd == NULL) {
			fprintf(stderr, "cannot create %s\n", outheader);  
			exit(-1);		
		}	
	}
	
	ParseGrammar();
	fclose(infile);
	
	BitVecSetSize();
	FindNullableSymbols();
	ComputeFIRST();
	ComputeFOLLOW();
	ComputePREDICT();
    
	if (listing) 
		WriteGrammar(stdout);
	if (dump_sets) {
		DumpFFSets(stdout);
		DumpPredictSets(stdout);
	}
    
	TestLL1conflict(fout);
	
	if(conflict)	{		
		DumpPredictSets(fout);

		if (!(conflict & LR_CONFLICT))
			WriteRecursiveParser(fout,outname, fhd, outheader);		
	} else {  
		if (tables) {
			WriteSymAndGrm(fout); 
			WriteLLTable(fout);
		} else if (code) {
			WriteSymAndGrmCode(fhd); 
			WriteLLTableCode(fout);
		} else
			WriteRecursiveParser(fout,outname, fhd, outheader);
	}
	
	if (!tables) 
		fclose(fhd);    

	fclose( fout );    

	if(conflict & LR_CONFLICT)	
		fprintf(stderr, "\nFinished with severe errors - see %s\n", outname);
	
	else if(conflict & CLP_CONFLICT) {	
		fprintf(stderr, "\nFinished with errors!\n");
		fprintf(stderr, "\nUsing default choice - see %s\n", outname);
	}
	else
		fprintf(stderr, "\nFinished without errors - see %s\n", outname);
	exit( conflict );
    return 0;
}
