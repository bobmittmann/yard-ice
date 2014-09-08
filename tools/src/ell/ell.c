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

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <errno.h>

#include "ell.h"

int	conflict = NO_CONFLICT;
int verbose = 0;

void WriteRecursiveParser(FILE *fp, char * pname, FILE * fh, char *hname);

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
	
	forall_symbols( sp ) 	sp->nullable = false;      
	
	/* this version assume that there is no epsilon symbol, but */
	/* pure null productions have rp->nrhs =0 */	
	
	forall_rules( rp ) 	if( rp->nrhs == 0) 	rp->lhs->nullable = true;
	
	/* find all productions that generate empty string */
	do
	{
		changed = 0;		
		forall_rules( rp ) 
		{
			if( rp->lhs->nullable == true) continue;
			
			for(i=0; i<rp->nrhs; i++)
			{
				if( rp->rhs[i]->nullable == false) break;
			}			
			if( i==rp->nrhs ) /* means that all are nullable - this also cover empty*/
			{
				rp->lhs->nullable = true;
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
    forall_symbols(sp) {
		sp->first = BitVecNew();
		if(sp->kind == TERM)
			BitVecAdd(sp->first, sp->sym_no );
    }
	
	/* Now compute all first sets for nonterminals*/
	do {
		SYM *s1, *s2;
		changed = 0;
		
		forall_rules(rp) {
			s1 = rp->lhs; 
			for(i = 0; i < rp->nrhs; i++) {
				s2 = rp->rhs[i]; 
				
				if (s2->kind == TERM) {          
					changed += BitVecAdd(s1->first, s2->sym_no);
					break;
				} else if (s1 == s2) {
					if (s1->nullable == false) 
						break;
				} else { 
					changed += BitVecUnion(s1->first, s2->first);			  
					if (s2->nullable == false) 
						break;
				}
			}
		}
	} while(changed);    
}

void ComputeFOLLOW() 
{
    int i, j, changed;
    RULEPTR rp;
    SYMPTR sp, tp;
	
    forall_symbols( sp ) sp->follow = BitVecNew();
    
	do {		
		changed = 0;
		forall_rules( rp ) {									             
			for( i = 0;   i<rp->nrhs; i++ ) {
				sp = rp->rhs[i];

				if(sp->kind != NONTERM) 
					continue;
				
				for( j=i+1; j < rp->nrhs; j++ ) 
				{
					tp = rp->rhs[j];
					
					if(tp->nullable == true) {
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
		all_nullable = true;
		
		for(i=0; i <rp->nrhs; i++)
		{
			sp = rp->rhs[i];		   			
			BitVecUnion(rp->predict, sp->first);
			
			if(sp->nullable == false) {
				all_nullable = false;
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
				fprintf(fp,"\nLeft recursion in rule:%d, line:%d ", 
						rp->rule_no, rp->lineno);
				conflict = conflict | LR_CONFLICT;
				rp->conflict = conflict;
		}
	}

	forall_symbols( sp ) /* for all nonterminal test LL(1) conflict */
	{
		if(sp->kind != NONTERM)  
			continue;   
		if(sp->kind == NONTERM) {
			/* look for rules begining with nonterminal sp*/
			BitVecClear(set);			
			forall_rules( rp ) 
			{				
				if(rp->lhs->sym_no == sp->sym_no)
				{				
					/* 1st condition  - usually result of left recursion*/
					if(BitVecEmpty(rp->predict)) 
					{
						fprintf(fp,"\nCan\'t predict rule:%d, line:%d", 
								rp->rule_no, rp->lineno);
						conflict = conflict | LR_CONFLICT;
						rp->conflict = conflict;
					}
					/* 2nd condition - result of common left prefixes*/
					if (BitVecHasIntersection(set, rp->predict)) {
						fprintf(fp,"\nConflict in rule:%d, line:%d", 
								rp->rule_no, rp->lineno);
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

void WritePredictCases(FILE *fp, RULEPTR rp)
{
 /* used in WriteRecursiveParser(FILE *fp) for cases of predictng alternate productions*/

    SYMPTR sp; 			
	forall_symbols(sp) {
		if (BitVecTest( rp->predict, sp->sym_no )) {
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
	
	/* for all terminal*/
	forall_symbols(sp) {
		if(sp->kind == TERM) {
			if (strlen(sp->symtext) > 1) /* skip single char operators*/
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

void usage(FILE * f, char * prog)
{
	fprintf(f, "Usage: %s [OPTION...] grammar\n", prog);
	fprintf(f, "LL(1) Predictive Parser Generator.\n");
	fprintf(f, "\n");
	fprintf(f, "  -?     \tShow this help message\n");
	fprintf(f, "  -o FILE\toutput\n");
	fprintf(f, "  -v[v]  \tVerbosity level\n");
	fprintf(f, "  -V     \tPrint version\n");
	fprintf(f, "  -l     \tprint symbols and BNF grammar rules to stdout\n");
	fprintf(f, "  -s     \tprint first, follow and predict sets to stdout\n");
	fprintf(f, "  -t     \twrite predictive table\n");
	fprintf(f, "  -c     \twrite .c file\n");
	fprintf(f, "  -h     \twrite .h file\n");
	fprintf(f, "  -r     \trecursive descent parser\n");
	fprintf(f, "  -e     \tembedded predictive parser tables\n");
	fprintf(f, "\n");
}

void version(char * prog)
{
	fprintf(stderr, "%s\n", PACKAGE_STRING);
	fprintf(stderr, "(C)Copyright, Ivo Mateljan.\n");
	exit(1);
}


int main(int argc,  char **argv)
{
	extern char *optarg;	/* getopt */
	extern int optind;	/* getopt */
	char * prog;

	char basename[256];
	char outname[256];

	FILE * cf = NULL;
	bool cgen = false;
	char cfname[256];

	FILE * hf = NULL;
	bool hgen = false;
	char hfname[256];

	FILE * tf = NULL;
	bool tgen = false;
	char tfname[256];

	bool rdp = true; /* default, generate a RD parser */
	bool output_set = false;

	char * filename; /* name of grammar input file */

	int c;

	int	listing = false;
	int	dump_sets = false;

	/* the prog name start just after the last lash */
	if ((prog = (char *)strrchr(argv[0], '/')) == NULL)
		prog = argv[0];
	else
		prog++;

	/* parse the command line options */
	while ((c = getopt(argc, argv, "V?vretchlso:")) > 0) {
		switch (c) {
		case 'V':
			version(prog);
			break;

		case '?':
			usage(stdout, prog);
			return 0;

		case 'v':
			verbose++;
			break;

		case 'r':
			rdp = true;
			break;

		case 'e':
			rdp = false;
			break;

		case 't':
			tgen = true;
			break;

		case 'c':
			cgen = true;
			break;

		case 'h':
			hgen = true;
			break;

		case 'l':
			listing = true;
			break;

		case 's':
			dump_sets = true;
			break;

		case 'o':
			strcpy(outname, optarg);
			output_set = true;
			break;


		default:
			fprintf(stderr, "%s: invalid option %s\n", prog, optarg);
			return 1;
		}
	}

	if (optind != (argc - 1)) {
		fprintf(stderr, "%s: missing grammar.\n\n", prog);
		usage(stderr, prog);
		return 2;
	}

	filename = argv[optind];

	if (output_set) {
		if (cgen && hgen) {
			fprintf(stderr, "incompatible options: -o -c -h\n");
			return 1;
		}

		if (cgen && tgen) {
			fprintf(stderr, "incompatible options: -o -c -t\n");
			return 1;
		}

		if (hgen && tgen) {
			fprintf(stderr, "incompatible options: -o -h -t\n");
			return 1;
		}

		if (tgen) 
			strcpy(tfname, outname);

		if (hgen) 
			strcpy(hfname, outname);

		if (cgen)  {
			char * cp;
			strcpy(cfname, outname);
			/* strip the file extension */
			strcpy(basename, outname);
			if ((cp = strrchr(basename, '.')) != NULL)
				*cp = '\0';
			strcpy(hfname, basename);
			strcat(hfname, ".h" );
		}
	} else { 
		char * cp;
		/* strip the file extension */
		strcpy(basename, filename);
		if ((cp = strrchr(basename, '.')) != NULL)
			*cp = '\0';

		if (rdp) { 
			strcpy(cfname, basename);
			strcat(cfname, "_par.c" );
			strcpy(hfname, basename);
			strcat(hfname, "_par.h" );
		} else {
			strcpy(cfname, basename);
			strcat(cfname, "_ll.c" );
			strcpy(hfname, basename);
			strcat(hfname, "_ll.h" );
		}
		strcpy(tfname, basename);
		strcat(tfname, ".tbl" );
	}

	if (rdp) {
		if (verbose)
			printf("%s: recursive descent parser generation.\n", prog);
		cgen = true;
		hgen = true;
	} else {
		if (verbose)
			printf("%s: embedded predictive parser tebales generation.\n", 
				   prog);
	}

	if (tgen) {
		if (verbose)
			printf(" - creating table file: \"%s\"\n", tfname);
		if ((tf = fopen(tfname, "w")) == NULL) {
			fprintf(stderr, "ERROR: creating file \"%s\": %s\n", 
					tfname, strerror(errno));  
			return 1;
		}
	}

	if (cgen) {
		if (verbose)
			printf(" - creating C file: \"%s\"\n", cfname);
		if ((cf = fopen(cfname, "w")) == NULL) {
			fprintf(stderr, "ERROR: creating file \"%s\": %s\n", 
					cfname, strerror(errno));  
			return 1;
		}
	}

	if (hgen) {
		if (verbose)
			printf(" - creating C header file: \"%s\"\n", hfname);
		if ((cf = fopen(hfname, "w")) == NULL) {
			fprintf(stderr, "ERROR: creating file \"%s\": %s\n", 
					hfname, strerror(errno));  
			return 1;
		}
	}
	
	parse_grammar(filename);

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

	TestLL1conflict(stderr);

	if (conflict)	{		
		DumpPredictSets(stderr);
		if (!(conflict & LR_CONFLICT) && rdp) {
			WriteRecursiveParser(cf, cfname, hf, hfname);
		}

		if (conflict & LR_CONFLICT)	{
			fprintf(stderr, "\nFinished with severe errors - see %s\n", 
					outname);
		} else if(conflict & CLP_CONFLICT) {	
			fprintf(stderr, "\nFinished with errors!\n");
//			fprintf(stderr, "\nUsing default choice - see %s\n", outname);
		} 
	} else {  
		if (verbose)
			fprintf(stdout, " - finished without errors.\n");

		if (tgen) {
			/* create table file */
			WriteSymAndGrm(tf); 
			WriteLLTable(tf);
		} 

		if (rdp) {
			WriteRecursiveParser(cf, cfname, hf, hfname);
		} else {
			if (hgen)
				write_compact_h(hf, hfname); 

			if (cgen)
				write_compact_c(cf, hfname);
		}
	}

	if (tgen)
		fclose(tf);    

	if (hgen)
		fclose(hf);    

	if (cgen)
		fclose(cf);    

	return 0;
}

