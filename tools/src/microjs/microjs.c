/* 
 * Copyright(C) 2014 Robinson Mittmann. All Rights Reserved.
 * 
 * This file is part of the MicroJs
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
 * @file microjs.c
 * @brief Simple command line interpreter
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <libgen.h>

#include "config.h"

#include <microjs.h>
#define __MICROJS_LIB_DEF__
#include <microjs-stdlib.h>

#if defined(WIN32)
  #include <io.h>
  #include <fcntl.h>
#else
  #include <assert.h>
#endif

int load_script(const char * pathname, char ** cpp, unsigned int * lenp)
{
	char * cp;
	FILE * f;
	int ret;
	int len;

	if ((f = fopen(pathname, "r")) == NULL) {
		fprintf(stderr, "ERROR: %s: open(): %s.\n",
				__func__, strerror(errno));
		return -1;
	}

	fseek(f, 0, SEEK_END);
	len = ftell(f);

	if ((cp = realloc(*cpp, *lenp + len)) == NULL) {
		fprintf(stderr, "ERROR: %s: frealloc(): %s.\n",
				__func__, strerror(errno));
		fclose(f);
		return -1;
	}
	*cpp = cp;

	cp += *lenp;

	fseek(f, 0, SEEK_SET);

	if ((ret = fread(cp, len, 1, f)) != 1) {
		fprintf(stderr, "ERROR: %s: fread(): %s.\n",
				__func__, strerror(errno));
		fclose(f);
		return ret;
	}

	fclose(f);

	*lenp += len;

	return len;
}

void usage(FILE * f, char * prog)
{
	fprintf(f, "Usage: %s [OPTION...] [SCRIPT...]\n", prog);
	fprintf(f, "Parse a script....\n");
	fprintf(f, "\n");
	fprintf(f, "  -?     \tShow this help message\n");
	fprintf(f, "  -o FILE\toutput\n");
	fprintf(f, "  -v[v]  \tVerbosity level\n");
	fprintf(f, "  -V     \tPrint version\n");
	fprintf(f, "  -t     \tVirtual machine trace\n");
	fprintf(f, "\n");
}

void version(char * prog)
{
	fprintf(stderr, "%s\n", PACKAGE_STRING);
	fprintf(stderr, "(C)Copyright Bob Mittmann (bobmittmann@gmail.com)\n");
	exit(1);
}

extern FILE * microjs_vm_tracef;

int main(int argc,  char **argv)
{
	uint8_t vm_code[1024]; /* compiled code */
	int32_t vm_data[64]; /* data area */
	int32_t vm_stack[64]; /* stack area */

	uint16_t strbuf[128]; /*string buffer shuld be 16bits aligned */
	uint32_t js_symbuf[64]; /* symbol table buffer (can be shared with 
						  the data buffer) */
	uint32_t js_sdtbuf[64]; /* compiler buffer */

	struct microjs_sdt * microjs; 
	struct microjs_rt * rt;

	struct microjs_vm vm; 

	struct symtab * symtab;

	char outfname[256];
	bool trace = false;
	bool outset = false;
	FILE * ftrace = NULL;
	char * prog;
	char * script = NULL;
	unsigned int len = 0;
	int verbose = 0;
	int code_sz;
	int i = 1;
	int err;
	int c;

	/* the prog name start just after the last lash */
	if ((prog = (char *)basename(argv[0])) == NULL)
		prog = argv[0];

	/* parse the command line options */
	while ((c = getopt(argc, argv, "V?vto:")) > 0) {
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

		case 't':
			trace = true;
			break;

		case 'o':
			strcpy(outfname, optarg);
			outset = true;
			break;


		default:
			fprintf(stderr, "%s: invalid option %s\n", prog, optarg);
			return 1;
		}
	}

	if (optind == argc) {
		fprintf(stderr, "%s: missing javascript source file.\n\n", prog);
		usage(stderr, prog);
		return 2;
	}

	if (trace) {
		if (outset) {
			if ((ftrace = fopen(outfname, "w")) == NULL) {
				fprintf(stderr, "ERROR: creating file \"%s\": %s\n", 
						outfname, strerror(errno));  
				return 1;
			}
		} else
			ftrace = stdout;
	}

	/* initialize string buffer */
	strbuf_init(strbuf, sizeof(strbuf));
	/* initialize symbol table */
	symtab = symtab_init(js_symbuf, sizeof(js_symbuf));

	/* initialize compiler */
	microjs = microjs_sdt_init(js_sdtbuf, sizeof(js_sdtbuf), 
							   symtab, &microjs_lib);

	microjs_sdt_begin(microjs, vm_code, sizeof(vm_code));

	/* load all scripts */
	for (i = optind; i < argc; ++i) {
		if (load_script(argv[i], &script, &len) < 0)
			return 1;
	}

	/* compile */
	if ((err = microjs_compile(microjs, script, len)) < 0) {
		microjs_sdt_error(stderr, microjs, err);
		symtab_dump(stderr, symtab);
		return 1;
	}


	/* insert an ABT opcode at the end of the code */
	if ((code_sz = microjs_sdt_end(microjs)) < 0)
		return 1;

	printf("\nSymbol table:\n");
	symtab_dump(stdout, symtab);

	rt = symtab_rt_get(symtab);

	if (verbose) {
		printf(" -  Code size = %d!\n", code_sz);
		printf(" -  Data size = %d!\n", rt->data_sz);
		printf(" - Stack size = %d!\n", rt->stack_sz);
	}

	if (rt->data_sz >= sizeof(vm_data)) {
		fprintf(stderr, "\n#ERROR: data overflow!\n");
		return 1;
	}

	if (rt->stack_sz >= sizeof(vm_stack)) {
		fprintf(stderr, "\n#ERROR: stack overflow!\n");
		return 1;
	}

	microjs_vm_tracef = ftrace;
	/* initialize virtual machine */
	microjs_vm_init(&vm, rt, NULL, vm_data, vm_stack);

	/* run */
	if ((err = microjs_exec(&vm, vm_code)) != 0) {
		fprintf(stderr, "\n#ERROR: Script failed with code %d!\n", err);
		return 1;
	}

	return 0;

}

