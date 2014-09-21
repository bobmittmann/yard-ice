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

/* --------------------------------------------------------------------------
   External symbols
   -------------------------------------------------------------------------- */

struct ext_libdef externals = {
	.name = "lib",
	.fncnt = 9,
	.fndef = {
		[EXT_RAND] = { .nm = "rand", .argmin = 0, .argmax = 0, .ret = 1 },
		[EXT_SRAND] = { .nm = "srand", .argmin = 1, .argmax = 1, .ret = 0 },
		[EXT_TIME] = { .nm = "time", .argmin = 0, .argmax = 0, .ret = 1 },
		[EXT_SQRT] = { .nm = "sqrt", .argmin = 1, .argmax = 1, .ret = 1 },
		[EXT_LOG2] = { .nm = "log2", .argmin = 1, .argmax = 1, .ret = 1 },
		[EXT_WRITE] = { .nm = "write", .argmin = 0, .argmax = 32, .ret = 0 },
		[EXT_PRINT] = { .nm = "print", .argmin = 0, .argmax = 32, .ret = 0 },
		[EXT_PRINTF] = { .nm = "printf", .argmin = 1, .argmax = 32, .ret = 0 },
		[EXT_MEMRD] = { .nm = "memrd", .argmin = 1, .argmax = 1, .ret = 1 },
	}
};

int main(int argc,  char **argv)
{
	uint8_t code[512]; /* compiled code */
	int32_t data[64]; /* data area */

	uint16_t strbuf[128]; /*string buffer shuld be 16bits aligned */
	uint32_t symbuf[64]; /* symbol table buffer (can be shared with 
						  the data buffer) */
	uint32_t sdtbuf[64]; /* compiler buffer */

	struct microjs_sdt * microjs; 
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
	int i = 1;
	int err;
	int n;
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
	symtab = symtab_init(symbuf, sizeof(symbuf), &externals);
	/* initialize compiler */
	microjs = microjs_sdt_init(sdtbuf, sizeof(sdtbuf), symtab, 
							  code, sizeof(code), sizeof(data));

	/* load all scripts */
	for (i = optind; i < argc; ++i) {
		if (load_script(argv[i], &script, &len) < 0)
			return 1;
	}

	/* compile */
	if ((err = microjs_compile(microjs, script, len)) < 0) {
		microjs_sdt_error(stderr, microjs, err);
		return 1;
	}

	/* insert an ABT opcode at the end of the code */
	if ((n = microjs_sdt_done(microjs)) < 0)
		return 1;

	/* initialize virtual machine */
	microjs_vm_init(&vm, data, sizeof(data));
	/* initialize run time environment */
	vm.env.ftrace = ftrace;
	/* run */
	if ((n = microjs_exec(&vm, code, n)) != 0) {
		fprintf(stderr, "\n#ERROR: Script failed with code %d!\n", n);
		return 1;
	}

	return 0;

}
