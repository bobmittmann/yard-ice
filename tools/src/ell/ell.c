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
 * \file      ell.c
 * \brief     Ell application main
 * \author    Ivo Mateljan
 * \author    Robinson Mittmann <bobmittmann@gmail.com>
 * \copyright 2003-2014, Ivo Mateljan
 */ 

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <errno.h>
#include <libgen.h>

#include "ell.h"

int	conflict = NO_CONFLICT;
int verbose = 0;

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

void FindNullableSymbols(void) 
{ 
	RULEPTR rp;
	SYMPTR sp;
	int changed;
	int i;
	
	forall_symbols(sp) {	
		if (sp->kind == ACTION)
			sp->nullable = true;      
		else
			sp->nullable = false;      
	}
	
	/* this version assume that there is no epsilon symbol, but */
	/* pure null productions have rp->nrhs =0 */	
	
	forall_rules(rp) {	
		if (rp->nrhs == 0) {
			rp->lhs->nullable = true;
			continue;
		}

		rp->lhs->nullable = true;
		for (i = 0; i < rp->nrhs; i++) {
			sp = rp->rhs[i];
			if (sp->kind != ACTION) {
				rp->lhs->nullable = false;
				break;
			}
		}		
	}
	
	/* find all productions that generate empty string */
	do {
		changed = 0;		
		forall_rules(rp) {
			if (rp->lhs->nullable == true) 
				continue;
			
			for (i = 0; i < rp->nrhs; i++) {
				if (rp->rhs[i]->nullable == false) 
					break;
			}			
			if (i == rp->nrhs) {
				/* means that all are nullable - this also cover empty*/
				rp->lhs->nullable = true;
				changed = 1;
			}
		}		
	} while (changed);	
}


void ComputeFIRST(void) 
{
	int i;
	RULEPTR rp;
	SYMPTR sp;
	int changed;
	
	/* first set of terminal symbol contains only that terminal symbol */
    forall_symbols(sp) {
		sp->first = BitVecNew();
		if (sp->kind == TERM)
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

void ComputeFOLLOW(void) 
{
    int i, j, changed;
    RULEPTR rp;
    SYMPTR sp, tp;
	
    forall_symbols(sp) 
		sp->follow = BitVecNew();
    
	do {		
		changed = 0;
		forall_rules(rp) {					          
			for (i = 0; i < rp->nrhs; i++) {
				sp = rp->rhs[i];

				if (sp->kind != NONTERM) 
					continue;
				
				for (j = i + 1; j < rp->nrhs; j++) {
					tp = rp->rhs[j];

					if (tp->kind == ACTION)  
						continue;   

					changed += BitVecUnion(sp->follow, tp->first);

					if (!tp->nullable)
						break;
				}
				
				if (j == rp->nrhs) {
					/* means all nullable after i*/
					changed += BitVecUnion(sp->follow, rp->lhs->follow);
				}
			}		
		}
	} while (changed);
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
	
	forall_rules(rp) 
		rp->predict = BitVecNew();
	
	forall_rules(rp) {		
		all_nullable = true;
		
		for (i = 0; i < rp->nrhs; i++) {
			sp = rp->rhs[i];		   			

			BitVecUnion(rp->predict, sp->first);
			
			if (sp->nullable == false) {
				all_nullable = false;
				break;
			}
		}		
		if (all_nullable)		
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
	forall_rules(rp) {
		if (rp->nrhs > 1) {
			if (rp->lhs->sym_no == rp->rhs[0]->sym_no) {
				fprintf(fp,"\nLeft recursion in rule:%d, line:%d\n", 
						rp->rule_no, rp->lineno);
				conflict = conflict | LR_CONFLICT;
				rp->conflict = conflict;
			}
		}
	}

	/* for all nonterminal test LL(1) conflict */
	forall_symbols(sp) {

		if (sp->kind != NONTERM)  
			continue;   

		/* look for rules begining with nonterminal sp*/
		BitVecClear(set);			
		forall_rules(rp) {				
			if (rp->lhs->sym_no == sp->sym_no) {				
				/* 1st condition  - usually result of left recursion*/
				if (BitVecEmpty(rp->predict)) {
					fprintf(fp,"\nCan\'t predict rule:%d, line:%d\n", 
							rp->rule_no, rp->lineno);
					conflict = conflict | LR_CONFLICT;
					rp->conflict = conflict;
				}
				/* 2nd condition - result of common left prefixes*/
				if (BitVecHasIntersection(set, rp->predict)) {
					fprintf(fp,"\nConflict in rule:%d, line:%d\n", 
							rp->rule_no, rp->lineno);
					conflict = conflict | CLP_CONFLICT;
					rp->conflict = conflict;
				}
				BitVecUnion(set, rp->predict);					
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
		for (j = 0; j < rp->nrhs; j++ )  {
			sp = rp->rhs[j];		   			
			if (sp->kind == ACTION)  
				continue;   
			fprintf(fp, " %s", sp->symtext);		
		}
		fprintf(fp, "    Predict: { "); 
		forall_symbols( sp ) {
			if (BitVecTest(rp->predict, sp->sym_no) )
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
		sp->kind == TERM ? 'T' : 'N', sp->symtext );
	
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
	fprintf(f, "  -d     \tcreate debug structures\n");
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

	char prefix[256];

	char outname[256];

	FILE * cf = NULL;
	bool cgen = false;
	char cfname[256];

	FILE * hf = NULL;
	bool hgen = false;
	char hfname[256];

	bool tgen = false;
	char tfname[256];

	bool rdp = true; /* default, generate a RD parser */
	bool output_set = false;

	char * filename; /* name of grammar input file */

	int c;

	int	listing = false;
	int	dump_sets = false;
	int	debug = false;

	/* the prog name start just after the last lash */
	if ((prog = (char *)basename(argv[0])) == NULL)
		prog = argv[0];

	/* parse the command line options */
	while ((c = getopt(argc, argv, "V?vretchlsod:")) > 0) {
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

		case 'd':
			debug = true;
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
			strcpy(prefix, basename(outname));
			if ((cp = strrchr(prefix, '.')) != NULL)
				*cp = '\0';
			strcpy(hfname, prefix);
			strcat(hfname, ".h" );
		}
	} else { 
		char * cp;
		/* strip the file extension */
		strcpy(prefix, basename(filename));
		if ((cp = strrchr(prefix, '.')) != NULL)
			*cp = '\0';

		if (!cgen && !hgen) {
			cgen = true;
			hgen = true;
		}

		if (rdp) { 
			strcpy(cfname, prefix);
			strcat(cfname, "_par.c" );
			strcpy(hfname, prefix);
			strcat(hfname, "_par.h" );
		} else {
			strcpy(cfname, prefix);
			strcat(cfname, "_ll.c" );
			strcpy(hfname, prefix);
			strcat(hfname, "_ll.h" );
		}
		strcpy(tfname, prefix);
		strcat(tfname, ".tbl" );
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

	if (conflict & LR_CONFLICT)	{
		if (verbose)
			WriteGrammar(stderr);
		fprintf(stderr, "\nFinished with severe errors!\n");
		return 1;
	}

	if (conflict & CLP_CONFLICT) {	
		fprintf(stderr, "\nUsing default choice.\n");
	}

	if (tgen) {
		FILE * tf;

		if (verbose)
			printf(" - creating table file: \"%s\"\n", tfname);
		if ((tf = fopen(tfname, "w")) == NULL) {
			fprintf(stderr, "ERROR: creating file \"%s\": %s\n", 
					tfname, strerror(errno));  
			return 1;
		}

		/* create table file */
		WriteSymAndGrm(tf); 
		WriteLLTable(tf);

		fclose(tf);    
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
		if ((hf = fopen(hfname, "w")) == NULL) {
			fprintf(stderr, "ERROR: creating file \"%s\": %s\n", 
					hfname, strerror(errno));  
			return 1;
		}
	}

	if (rdp) {
		if (verbose)
			printf(" - recursive descent parser generation.\n");
	} else {
		if (verbose)
			printf(" - embedded predictive parser tebales generation.\n");
	}

	if (rdp) {
		WriteRecursiveParser(cf, cfname, hf, hfname);
	} else {
		if (hgen)
			write_compact_h(hf, prefix, hfname, debug); 

		if (cgen)
			write_compact_c(cf, prefix, hfname, debug);
	}

	if (hgen)
		fclose(hf);    

	if (cgen)
		fclose(cf);    

	if (conflict) {	
		fprintf(stderr, "\nFinished with errors!\n");
		return 1;
	}

	if (verbose)
		fprintf(stdout, " - finished without errors.\n");

	return 0;
}

