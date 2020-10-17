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
 * \file      embedded.c
 * \brief     Compact predictive parser tables generator
 * \author    Robinson Mittmann <bobmittmann@gmail.com>
 * \copyright 2003-2014, Ivo Mateljan
 */ 

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "ell.h"

struct tr_pair {
	int sym_no;
	int rule_no;
};

char * strupper(char * dst, const char * src)
{
	char * s = dst;
	char * cp;

	for (cp = (char *)src; *cp != '\0'; cp++)
		*dst++ = toupper(*cp);

	*dst = '\0';
	return s;
}


int tr_pair_cmp(const void * p1, const void * p2)
{
	return ((struct tr_pair *)p1)->sym_no - ((struct tr_pair *)p2)->sym_no;
}

void write_sym_tab(FILE * fp, char * prefix) 
{    
    SYMPTR sp;       

	fprintf(fp, "const const char * const %s_ll_sym[] = {\n", prefix);
	forall_symbols(sp) {
		if (sp->kind == TERM)
			fprintf(fp, "\t\"%s\",\n ", sp->symtext);
	}

	forall_symbols(sp) {
		if (sp->kind == NONTERM)
			fprintf(fp, "\t\"%s\",\n ", sp->symtext);
	}

	forall_symbols(sp) {
		if (sp->kind == ACTION)
			fprintf(fp, "\t\"%s\",\n ", sp->symtext);
	}
	
	fprintf(fp, "};\n\n");
}

void write_compact_c(FILE * fp, char * prefix, char * hname, bool debug)
{	
	RULEPTR rp;
	SYMPTR sp,tp; 
	BITVEC  set = BitVecNew();
	unsigned int pos;
	unsigned int off;
	unsigned int size;
	unsigned int cnt;
	unsigned int sum = 0;
	unsigned int idx_sz;
	struct tr_pair tr_vec[1024]; /* FIXME: dynamic allocation */

	fprintf(fp, "/* LL(1) Embeddedd Nonrecursive Predictive Parser */\n\n");

	fprintf(fp, "#include \"%s\"\n\n", hname);

    fprintf(fp, "/* Token-Rule pair (predict set) */\n");	
	fprintf(fp, "struct tr_pair {\n");
	fprintf(fp, "\tuint8_t t;\n");
	fprintf(fp, "\tuint8_t r;\n");
	fprintf(fp, "};\n\n");

	#define SIZEOF_TR_PAIR 2

	cnt = 0;
	pos = 0;
	size = 0;
    fprintf(fp, "/* Predict sets */\n");	
	fprintf(fp, "static const struct tr_pair predict_vec[] = {");
	/* for all nonterminal write  */
	forall_symbols(sp) {
		unsigned int i;

		if (sp->kind != NONTERM)  
			continue;   

		off = pos;
		cnt = 0;
		/* look for rules begining with nonterminal sp */
		/* and print predict set (terminal,prod.no)*/
		BitVecClear(set);
		forall_rules(rp) {
			if (rp->lhs->sym_no == sp->sym_no) {				
				forall_symbols(tp) {
					if (BitVecTest(rp->predict, tp->sym_no)) {
						tr_vec[cnt].sym_no = tp->sym_no;
						tr_vec[cnt].rule_no = rp->rule_no;
						cnt++;
					}
				}					
			}
		}

		qsort(tr_vec, cnt, sizeof(struct tr_pair), tr_pair_cmp);

		fprintf(fp, "\n\t/* %3d (%s) */", sp->sym_no, sp->symtext);
		for (i = 0; i < cnt; ++i) {
			if ((i % 6) == 0)
				fprintf(fp,"\n\t");
			fprintf(fp,"{%3d,%3d}, ", tr_vec[i].sym_no, tr_vec[i].rule_no);
		}

		pos += cnt;
		sp->r_offs = off;
		sp->r_cnt = pos - off;
		size += sp->r_cnt * SIZEOF_TR_PAIR;
	}

	fprintf(fp, "\n};\n");
	fprintf(fp, "/* %d bytes; */\n\n", size);
	sum += size;

	size = 0;
	fprintf(fp, "static const struct {\n");
	if ((pos - cnt) >= 256) {
		idx_sz = 4;
		fprintf(fp, "\tuint16_t off;\n");
	} else {
		idx_sz = 2;
		fprintf(fp, "\tuint8_t off;\n");
	}
	fprintf(fp, "\tuint8_t cnt;\n");
	fprintf(fp, "} predict_idx[] = {");
	/* for all nonterminal write  */
	forall_symbols(sp) {
		if (sp->kind != NONTERM)  
			continue;   
		fprintf(fp,"\n\t{%4d,%3d},", sp->r_offs, sp->r_cnt);
		size += idx_sz; 
	}
	fprintf(fp, "\n};\n");
	fprintf(fp, "/* %d bytes */\n\n", size);
	sum += size;


	cnt = 0;
	pos = 0;
	size = 0;
    fprintf(fp, "/* Rules vectors table */\n");	
	fprintf(fp, "static const uint8_t rule_vec[] = {");
    forall_rules(rp) {		
    	int j;

		off = pos;
		fprintf(fp, "\n\t/* %2d:%s(%d) -> */\n\t", rp->rule_no, 
				rp->lhs->symtext, rp->nrhs); 		
		cnt = rp->nrhs;
		for(j = 0; j < cnt; ++j) {
			char s[256];
			sp = rp->rhs[j];
			fprintf(fp, "%c_%s, ", kind_prefix(sp->kind), 
					strupper(s, sp->symtext));
			pos++;
			size += 1;
		}
		rp->offs = off;
    }    
	fprintf(fp, "\n};\n");
	fprintf(fp, "/* %d bytes */\n\n", size);
	sum += size;

	size = 0;
	fprintf(fp, "static const struct {\n");
	if ((pos - cnt) >= 256) {
		idx_sz = 4;
		fprintf(fp, "\tuint16_t off;\n");
	} else {
		idx_sz = 2;
		fprintf(fp, "\tuint8_t off;\n");
	}
	fprintf(fp, "\tuint8_t cnt;\n");
	fprintf(fp, "} rule_idx[] = {");
    forall_rules(rp) {		
		fprintf(fp,"\n\t{%4d,%3d},", rp->offs, rp->nrhs);
		size += idx_sz; 
    }    
	fprintf(fp, "\n};\n");
	fprintf(fp, "/* %d bytes */\n\n", size);
	sum += size;

	fprintf(fp, "/* Total: %d bytes */\n\n", sum);
	fprintf(fp, "\n");

	fprintf(fp, "int %s_ll_push(uint8_t * sp, unsigned int sym,"
			" unsigned int tok)\n", prefix);
	fprintf(fp, "{\n");
	fprintf(fp, "\tconst struct tr_pair * vec;\n");
	fprintf(fp, "\tuint8_t * p;\n");
	fprintf(fp, "\tint imax;\n");
	fprintf(fp, "\tint imin;\n");
	fprintf(fp, "\tint n;\n");
	fprintf(fp, "\tint i;\n\n");
	fprintf(fp, "\t/* Get the index for the symbol at the stack's head */\n");
	fprintf(fp, "\ti = sym - NONTERM_BASE;\n");
	fprintf(fp, "\tif (i < 0) /* Shuld be nonterminal */\n");
	fprintf(fp, "\t\treturn -1;\n");
	fprintf(fp, "\tvec = &predict_vec[predict_idx[i].off];\n");
	fprintf(fp, "\timax = predict_idx[i].cnt;\n");
	fprintf(fp, "\timin = 1;\n\n");
	fprintf(fp, "\t/* Is the production list empty ? !!! \n");
	fprintf(fp, "\tif (imax < 0)\n");
	fprintf(fp, "\t\treturn -1;\n");
	fprintf(fp, "\t*/\n\n");
	fprintf(fp, "\t/* Binary search for the matching rule */\n");
	fprintf(fp, "\twhile (imin < imax) {\n");
	fprintf(fp, "\t\tint imid = (imin + imax) / 2;\n");
	fprintf(fp, "\t\tif (vec[imid - 1].t < tok) \n");
	fprintf(fp, "\t\t\timin = imid + 1;\n");
	fprintf(fp, "\t\telse\n");
	fprintf(fp, "\t\t\timax = imid;\n");
	fprintf(fp, "\t}\n\n");
	fprintf(fp, "\tif (vec[imin - 1].t != tok) {\n");
	fprintf(fp, "\t\t/* No matching rule, return error. */\n");
	fprintf(fp, "\t\treturn -1;\n");
	fprintf(fp, "\t}\n\n");
	fprintf(fp, "\ti = vec[imin - 1].r;\n");
	fprintf(fp, "\tn = rule_idx[i].cnt;\n");
	fprintf(fp, "\tp = (uint8_t *)&rule_vec[rule_idx[i].off];\n");
	fprintf(fp, "\tsp -= n;\n\n");
	fprintf(fp, "\t/* Push the production into the stack */\n");
	fprintf(fp, "\tfor (i = 0; i < n; ++i)\n");
	fprintf(fp, "\t\tsp[i] = p[i];\n\n");
	fprintf(fp, "\t/* Return the lenght of the production */\n");
	fprintf(fp, "\treturn n;\n");
	fprintf(fp, "}\n\n");

	if ((rp = first_rule) != NULL) {
		char s[256];
		int j;

		fprintf(fp, "int %s_ll_start(uint8_t * sp)\n", prefix);
		fprintf(fp, "{\n");

		for(j = 0; j < rp->nrhs; ++j) {
			sp = rp->rhs[j];
			fprintf(fp, "\tsp[-%d] = ", j + 1);
			fprintf(fp, "%c_%s;\n", kind_prefix(sp->kind), 
					strupper(s, sp->symtext));
		}

		fprintf(fp, "\tsp[-%d] = ", j + 1);
		fprintf(fp, "%c_%s;\n", kind_prefix(rp->lhs->kind), 
					strupper(s, rp->lhs->symtext));

		fprintf(fp, "\n");
		fprintf(fp, "\treturn %d;\n", j + 1);
		fprintf(fp, "}\n\n");
    }    

	if (debug)
		write_sym_tab(fp, prefix);

	free(set);
}

void write_compact_h(FILE * fp, char * prefix, char * hname, bool debug) 
{    
	char s[256];
	char h[256];
    SYMPTR sp;       
	char * cp;
	int n;
    
	strupper(h, hname);
	for (cp = h; *cp != '\0'; ++cp) {
		if (!isalnum(*cp))
			*cp = '_';
	}

	fprintf(fp, "#ifndef __%s__\n", h);
	fprintf(fp, "#define __%s__\n\n", h);
	fprintf(fp, "#include <stdint.h>\n\n");
    fprintf(fp, "/* Last symbol num:%d  Num rules:%d */\n\n", 
			lastsymnum, num_rules);	
    forall_symbols( sp ) {
	}

	n = 0;
	forall_symbols(sp) {
		if (sp->kind == TERM) {
			fprintf(fp, "#define %c_%-20s %3d\n", kind_prefix(sp->kind), 
					strupper(s, sp->symtext), sp->sym_no);
		}
	}

	fprintf(fp, "\n");

	n = 0;
	forall_symbols(sp) {
		if (sp->kind == NONTERM) {
			if (n++ == 0)
				fprintf(fp, "#define %-23s %d\n", "NONTERM_BASE", sp->sym_no);
			fprintf(fp, "#define %c_%-20s %3d\n", kind_prefix(sp->kind), 
					strupper(s, sp->symtext), sp->sym_no);
		}
	}

	fprintf(fp, "\n");

	n = 0;
	forall_symbols(sp) {
		if (sp->kind == ACTION) {
			if (n++ == 0)
				fprintf(fp, "#define %-23s %d\n", "ACTION_BASE", sp->sym_no);
			fprintf(fp, "#define %c_%-20s %3d\n", kind_prefix(sp->kind), 
					strupper(s, sp->symtext), sp->sym_no);
		}
	}
	
	forall_symbols(sp) {
		if (sp->kind == NONTERM)  
			break;
	}

	fprintf(fp, "\n");
	fprintf(fp, "#define IS_A_TERMINAL(_X) ((_X) < NONTERM_BASE)\n");
	fprintf(fp, "#ifdef ACTION_BASE\n");
	fprintf(fp, "#define IS_A_NONTERMINAL(_X) ((_X) >= NONTERM_BASE && "
			"(_X) < ACTION_BASE)\n");
	fprintf(fp, "#define IS_AN_ACTION(_X) ((_X) >= ACTION_BASE)\n");
	fprintf(fp, "#define ACTION(_X) ((_X) - ACTION_BASE)\n");
	fprintf(fp, "#else\n");
	fprintf(fp, "#define IS_A_NONTERMINAL(_X) ((_X) >= NONTERM_BASE)\n");
	fprintf(fp, "#define IS_AN_ACTION(_X) (0)\n");
	fprintf(fp, "#define ACTION(_X) (0)\n");
	fprintf(fp, "#endif\n\n");

	if (debug)
		fprintf(fp, "extern const const char * const %s_ll_sym[];\n\n", 
				prefix);

	fprintf(fp, "#ifdef __cplusplus\nextern \"C\" {\n#endif\n\n");

	fprintf(fp, "int %s_ll_push(uint8_t * sp, unsigned int sym,"
			" unsigned int tok);\n", prefix);

	fprintf(fp, "int %s_ll_start(uint8_t * sp);\n", prefix);

	fprintf(fp, "\n#ifdef __cplusplus\n}\n#endif\n\n");

	fprintf(fp, "#endif /* __%s__ */\n", h);
	fprintf(fp, "\n\n");
}

