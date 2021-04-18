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
 * \file      rdp.c
 * \brief     Recursive descent parser generator
 * \author    Ivo Mateljan
 * \copyright 2003-2014, Ivo Mateljan
 */ 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>

#include "ell.h"

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


