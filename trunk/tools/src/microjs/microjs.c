/* $Id: dcclog.c,v 1.10 2006/09/28 19:31:45 bob Exp $ 
 *
 * File:	dcclog.c
 * Module:
 * Project:	ARM-DCC logger expander
 * Author:	Robinson Mittmann (bob@boreste.com, bob@methafora.com.br)
 * Target:
 * Comment:
 * Copyright(C) 2005 Robinson Mittmann. All Rights Reserved.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
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


int main(int argc,  char **argv)
{
	int32_t data[128];
	uint8_t code[512];
	struct microjs_compiler microjs; 
	struct microjs_vm vm; 
	struct sym_tab sym_tab;

	char outfname[256];
	bool trace = false;
	bool outset = false;
	FILE * ftrace = NULL;
	char * prog;
	char * script = NULL;
	unsigned int len = 0;
	int verbose = 0;
	int i = 1;
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

	sym_tab_init(&sym_tab);
	microjs_compiler_init(&microjs, &sym_tab, data, sizeof(data));

	for (i = optind; i < argc; ++i) {
		if (load_script(argv[i], &script, &len) < 0)
			return 1;
	}

	if ((n = microjs_compile(&microjs, code, script, len)) < 0)
		return 1;

	microjs_vm_init(&vm, data, sizeof(data));
	vm.ftrace = ftrace;

	if (microjs_exec(&vm, code, n) < 0)
		return 1;

	return 0;

}

