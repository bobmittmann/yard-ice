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
#include <inttypes.h>
#include <stdbool.h>
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include "config.h"
#include "calc.h"

#if defined(WIN32)
  #include <io.h>
  #include <fcntl.h>
#else
  #include <assert.h>
#endif

int load_script(const char * pathname, char ** cpp, unsigned int * lp)
{
	char * cp;
	FILE * f;
	int ret;
	int len;

	if ((pathname== NULL) || (cpp == NULL) || (lp == NULL)) {
		fprintf(stderr, "ERROR: %s: invalid arguments.\n", __func__);
		return -EINVAL;
	}

	if ((f = fopen(pathname, "r")) == NULL) {
		fprintf(stderr, "ERROR: %s: open(): %s.\n",
				__func__, strerror(errno));
		return -1;
	}

//	fprintf(stderr, "%s: 1.\n", __func__);
//	fflush(stderr);

	if ((ret = fseek(f, 0, SEEK_END)) < 0) {
		fprintf(stderr, "ERROR: %s: fseek(): %s.\n",
				__func__, strerror(errno));
		fclose(f);
		return ret;
	}

//	fprintf(stderr, "%s: 2.\n", __func__);
//	fflush(stderr);

	if ((len = ftell(f)) < 0) {
		fprintf(stderr, "ERROR: %s: ftell(): %s.\n",
				__func__, strerror(errno));
		fclose(f);
		return len;
	}

//	fprintf(stderr, "%s: 3. len=%d\n", __func__, len);
//	fflush(stderr);

	if ((ret = fseek(f, 0, SEEK_SET)) < 0) {
		fprintf(stderr, "ERROR: %s: fseek(): %s.\n",
				__func__, strerror(errno));
		fclose(f);
		return ret;
	}

	if ((cp = realloc(*cpp, *lp + len)) == NULL) {
		fprintf(stderr, "ERROR: %s: frealloc(): %s.\n",
				__func__, strerror(errno));
		fclose(f);
		return -1;
	}

	cp += *lp;

	if ((ret = fread(cp, len, 1, f)) != 1) {
		fprintf(stderr, "ERROR: %s: fread(): %s.\n",
				__func__, strerror(errno));
		fclose(f);
		return ret;
	}

	fclose(f);

	*cpp = cp;
	*lp += len;

	return len;
}

void usage(char * prog)
{
	fprintf(stderr, "Usage: %s [OPTION...] [SCRIPT...]\n", prog);
	fprintf(stderr, "Parse a script....\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "  -?     \tShow this help message\n");
	fprintf(stderr, "  -o FILE\toutput\n");
	fprintf(stderr, "  -v[v]  \tVerbosity level\n");
	fprintf(stderr, "  -V     \tPrint version\n");
	fprintf(stderr, "\n");
	exit(0);
}

void version(char * prog)
{
	fprintf(stderr, "%s\n", PACKAGE_STRING);
	fprintf(stderr, "(C)Copyright Bob Mittmann (bobmittmann@gmail.com)\n");
	exit(1);
}

void parse_err(char * prog, char * opt)
{
	fprintf(stderr, "%s: invalid option %s\n", prog, opt);
	exit(1);
}

void cleanup(void)
{
}

void sig_quit(int signo)
{
	cleanup();
	exit(3);
}

/*
int compile(const char * txt, unsigned int len)
{
	uint8_t tok_buf[8192];
	uint8_t code_buf[8192];
	struct microjs_parser jp;
	int err;

	microjs_init(&jp, tok_buf, sizeof(tok_buf));

	microjs_open(&jp, txt, len);

	if ((err = microjs_scan(&jp)) != MICROJS_OK) {
		fprintf(stderr, "Lexical analisys failed.\n");
		microjs_print_err(stderr, &jp, err);
		return err;
	}

	microjs_tok_dump(stdout, &jp);

	if ((err = microjs_parse(&jp, code_buf, sizeof(code_buf))) != MICROJS_OK) {
		fprintf(stderr, "Syntax analisys failed.\n");
		return err;
	}

	return 0;
}
*/

int do_test(const char * txt, unsigned int len)
{
	struct lexer lex;
	struct token tok;

	dump_src(txt, len);
	lexer_open(&lex, txt, len);

	for (;;) {
		tok = lexer_scan(&lex);

		printf("'%s' ", tok2str(tok));
		fflush(stdout);

		if (tok.typ == TOK_ERR) {
			fprintf(stderr, "\nLexical analisys failed.\n");
			lexer_print_err(stderr, &lex, tok.qlf);
			return 0;
		}

		if (tok.typ == TOK_EOF) {
			return 0;
		}
	}

	return 0;
}

int compile(const char * txt, unsigned int len)
{
	struct parser calc; 
	uint8_t code_buf[8192];
	int err;

	calc_open(&calc, txt, len);

	err = calc_parse(&calc, code_buf, sizeof(code_buf));

	if (err != OK) {
		lexer_print_err(stderr, &calc.lex, err);
	}

	return 0;
}


int main(int argc, char *argv[])
{
	extern char *optarg;	/* getopt */
	extern int optind;	/* getopt */
	bool output_set = false;
	char * script = NULL;
	unsigned int len = 0;
	char output[1024];
	int verbose = 0;
	int test = 0;
	char * prog;
	FILE * f;
	int c;

	/* the prog name start just after the last lash */
	if ((prog = (char *)strrchr(argv[0], '/')) == NULL)
		prog = argv[0];
	else
		prog++;

	/* parse the command line options */
	while ((c = getopt(argc, argv, "V?vto:")) > 0) {
		switch (c) {
			case 'V':
				version(prog);
				break;

			case '?':
				usage(prog);
				break;

			case 'v':
				verbose++;
				break;

			case 'o':
				strcpy(output, optarg);
				output_set = true;
				break;

			case 't':
				test++;
				break;

			default:
				parse_err(prog, optarg);
		}
	}

	if (optind >= argc) {
		fprintf(stderr, "%s: missing input file.\n\n", prog);
		usage(prog);
	}
	
	while (optind < argc) {
		char * input = argv[optind];

		printf(" - script: %s\n", input);
		fflush(stdout);

		if (load_script(input, &script, &len) < 0) {
			return 1;
		}

		optind++;
	}

#if defined(WIN32)
#else
	signal(SIGINT, sig_quit);
	signal(SIGTERM, sig_quit);
	signal(SIGQUIT, sig_quit);
	signal(SIGABRT, sig_quit);
#endif

	if (output_set) {
		printf(" - output: %s\n", output);
		printf("\n");
		if ((f = fopen(output, "+w")) == NULL) { 
			fprintf(stderr, "%s: fopen() failed!\n", prog);
			return 5;
		}
	} else {
		f = stdout;
	}

	if (test)
		do_test(script, len);
	else
		compile(script, len);

	return 0;
}

