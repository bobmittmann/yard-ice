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

void write_sym_tab(FILE * fp) 
{    
    SYMPTR sp;       

	fprintf(fp, "const const char * const ll_sym_tab[] = {\n");
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

void write_compact_c(FILE * fp, char * hname)
{	
	RULEPTR rp;
	SYMPTR sp,tp; 
	BITVEC  set = BitVecNew();

	unsigned int pos = 0;
	unsigned int off = 0;
	unsigned int size = 0;
	unsigned int cnt = 0;
	unsigned int sum = 0;
	struct tr_pair tr_vec[1024]; 

	#define SIZEOF_TR_PAIR 2
	#define SIZEOF_PROD_IDX_ENTRY 4
	#define SIZEOF_RULE_IDX_ENTRY 2

	fprintf(fp, "/* LL(1) Table */\n\n");

	fprintf(fp, "#include \"%s\"\n\n", hname);

	fprintf(fp, "struct tr_pair {\n");
	fprintf(fp, "\tuint8_t t;\n");
	fprintf(fp, "\tuint8_t r;\n");
	fprintf(fp, "};\n\n");

    fprintf(fp, "/* Productions vectors table */\n");	
	size = 0;
	fprintf(fp, "const struct tr_pair prod_vec[] = {");
	/* for all nonterminal write  */
	forall_symbols(sp) {
		unsigned int i;

		if (sp->kind != NONTERM)  
			continue;   

		off = pos;
		cnt = 0;
		/* look for rules begining with nonterminal sp*/
		/* and print predict set (terminal,prod.no)*/
		BitVecClear(set);
		forall_rules( rp ) {
			if (rp->lhs->sym_no == sp->sym_no) {				
				forall_symbols( tp ) {
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
	fprintf(fp, "\tuint16_t off;\n");
	fprintf(fp, "\tuint8_t cnt;\n");
	fprintf(fp, "} prod_idx[] = {");
	/* for all nonterminal write  */
	forall_symbols(sp) {
		if (sp->kind != NONTERM)  
			continue;   
		fprintf(fp,"\n\t{%4d,%3d},", sp->r_offs, sp->r_cnt);
		size += SIZEOF_PROD_IDX_ENTRY; 
	}
	fprintf(fp, "\n};\n");
	fprintf(fp, "/* %d bytes */\n\n", size);
	sum += size;


    fprintf(fp, "/* Rules vectors table */\n");	
	pos = 0;
	size = 0;
	fprintf(fp, "const uint8_t rule_vec[] = {");
    forall_rules(rp) {		
    	int j;

		off = pos;
		fprintf(fp, "\n\t/* %2d:%s(%d) -> */\n\t", rp->rule_no, 
				rp->lhs->symtext, rp->nrhs); 		
		for(j = 0; j < rp->nrhs; ++j) {
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
	fprintf(fp, "\tuint8_t off;\n");
	fprintf(fp, "\tuint8_t cnt;\n");
	fprintf(fp, "} rule_idx[] = {");
    forall_rules(rp) {		
		fprintf(fp,"\n\t{%4d,%3d},", rp->offs, rp->nrhs);
		size += SIZEOF_RULE_IDX_ENTRY; 
    }    
	fprintf(fp, "\n};\n");
	fprintf(fp, "/* %d bytes */\n\n", size);
	sum += size;

	fprintf(fp, "/* Total: %d bytes */\n\n", sum);
	fprintf(fp, "\n");

	fprintf(fp, "int ll_rule_push(uint8_t * sp, unsigned int sym,"
			" unsigned int tok)\n");
	fprintf(fp, "{\n");
	fprintf(fp, "\tstruct tr_pair * vec;\n");
	fprintf(fp, "\tuint8_t * p;\n");
	fprintf(fp, "\tint imax;\n");
	fprintf(fp, "\tint imin;\n");
	fprintf(fp, "\tint n;\n");
	fprintf(fp, "\tint i;\n\n");
	fprintf(fp, "\t/* Get the index for the symbol at the stack's head */\n");
	fprintf(fp, "\ti = sym - NONTERM_BASE;\n");
	fprintf(fp, "\tif (i < 0) /* Shuld be nonterminal */\n");
	fprintf(fp, "\t\treturn -1;\n");
	fprintf(fp, "\tvec = (struct tr_pair *)&prod_vec[prod_idx[i].off];\n");
	fprintf(fp, "\timax = prod_idx[i].cnt;\n");
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

	write_sym_tab(fp);

	free(set);
}

void write_compact_h(FILE * fp, char * hname) 
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

	fprintf(fp, "extern const const char * const ll_sym_tab[];\n\n");

	fprintf(fp, "#ifdef __cplusplus\nextern \"C\" {\n#endif\n\n");

	fprintf(fp, "int ll_rule_push(uint8_t * sp, unsigned int sym,"
			" unsigned int tok);\n");

	fprintf(fp, "int ll_start(uint8_t * sp);\n");

	fprintf(fp, "\n#ifdef __cplusplus\n}\n#endif\n\n");

	fprintf(fp, "#endif /* __%s__ */\n", h);
	fprintf(fp, "\n\n");
}

