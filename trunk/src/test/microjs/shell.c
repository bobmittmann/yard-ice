/* 
 * File:	 usb-test.c
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(C) 2011 Bob Mittmann. All Rights Reserved.
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
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <sys/shell.h>
#include <sys/tty.h>
#include <sys/param.h>
#include <sys/dcclog.h>
#include <sys/delay.h>

#include <microjs.h>
#include <crc.h>

#include "board.h"
#include "flashfs.h"

extern const struct shell_cmd cmd_tab[];

int cmd_help(FILE *f, int argc, char ** argv)
{
	struct shell_cmd * cmd;

	if (argc > 2)
		return -1;

	if (argc > 1) {
		if ((cmd = cmd_lookup(argv[1], cmd_tab)) == NULL) {
			fprintf(f, " Not found: '%s'\r\n", argv[1]);
			return -1;
		}

		fprintf(f, "  %s, %s - %s\r\n", cmd->name, cmd->alias, cmd->desc);
		fprintf(f, "  usage: %s %s\r\n\r\n", argv[1], cmd->usage);

		return 0;
	}

	fprintf(f, "\r\n Command:   Alias:  Desciption: \r\n");
	for (cmd = (struct shell_cmd *)cmd_tab; cmd->callback != NULL; cmd++) {
		fprintf(f, "  %-10s %-4s   %s\r\n", cmd->name, cmd->alias, cmd->desc);
	}

	return 0;
}

int cmd_rx(FILE * f, int argc, char ** argv)
{
	if (argc < 2)
		return SHELL_ERR_ARG_MISSING;

	if (argc > 2)
		return SHELL_ERR_EXTRA_ARGS;


	fprintf(f, "RX waiting to receive.");
	if ((fs_xmodem_recv(f, argv[1])) < 0) {
		fprintf(f, "fs_xmodem_recv() failed!\r\n");
		return -1;
	}

	return 0;
}

int cmd_rm(FILE * f, int argc, char ** argv)
{
	struct fs_dirent entry;

	if (argc < 2)
		return SHELL_ERR_ARG_MISSING;

	if (argc > 2)
		return SHELL_ERR_EXTRA_ARGS;

	if (!fs_dirent_lookup(argv[1], &entry)) {
		fprintf(f, "invalid file: \"%s\"\r\n", argv[1]);
		return SHELL_ERR_ARG_INVALID;
	}

	fs_file_unlink(&entry);

	return 0;
}

int cmd_cat(FILE * f, int argc, char ** argv)
{
	struct fs_dirent entry;
	struct fs_file * fp;
	char * cp;
	int cnt;

	if (argc < 2)
		return SHELL_ERR_ARG_MISSING;

	if (argc > 2)
		return SHELL_ERR_EXTRA_ARGS;

	if (!fs_dirent_lookup(argv[1], &entry)) {
		fprintf(f, "invalid file: \"%s\"\r\n", argv[1]);
		return SHELL_ERR_ARG_INVALID;
	}

	fp = entry.fp;
	cp = (char *)fp->data;

	for (cnt = 0; cnt < fp->size; ) { 
		int n;
		n = strlen(cp);
		if (n > 0) {
			fprintf(f, "%s\r\n", cp);
		} else
			n = 1;

		cnt += n;
		cp += n;
	}

	return 0;
}

int cmd_ls(FILE * f, int argc, char ** argv)
{
	struct fs_dirent entry;

	if (argc > 1)
		return SHELL_ERR_EXTRA_ARGS;

	memset(&entry, 0, sizeof(entry));

	while (fs_dirent_get_next(&entry)) {
		fprintf(f, "0x%06x %6d 0x%04x %6d %s\r\n", 
				entry.blk_offs, entry.blk_size, 
				entry.fp->crc, entry.fp->size, entry.fp->name);
	}

	return 0;
}

int cmd_js(FILE * f, int argc, char ** argv)
{
	struct fs_dirent entry;
	uint16_t strbuf[128]; /*string buffer shuld be 16bits aligned */
	uint8_t code[512]; /* compiled code */
	uint32_t data[64]; /* data area */
	uint32_t symbuf[64]; /* symbol table buffer (can be shared with 
						  the data buffer) */
	struct microjs_compiler microjs; 
	struct microjs_vm vm; 
	struct symtab * symtab;
	char * script;
	int len;
	int n;

	if (argc < 2)
		return SHELL_ERR_ARG_MISSING;

	if (argc > 2)
		return SHELL_ERR_EXTRA_ARGS;

	/* initialize string buffer */
	strbuf_init(strbuf, sizeof(strbuf));
	/* initialize symbol table */
	symtab = symtab_init(symbuf, sizeof(symbuf));
	/* initialize compiler */
	microjs_compiler_init(&microjs, symtab, (int32_t *)data, sizeof(data));

	if (!fs_dirent_lookup(argv[1], &entry)) {
		fprintf(f, "invalid file: \"%s\"\r\n", argv[1]);
		return SHELL_ERR_ARG_INVALID;
	}

	fprintf(f, "\"%s\"\r\n", entry.fp->name);
	script = (char *)entry.fp->data;
	len = entry.fp->size;

	if ((n = microjs_compile(&microjs, code, script, len)) < 0)
		return 1;

	microjs_vm_init(&vm, (int32_t *)data, sizeof(data));
//	vm.env.ftrace = stderr;

	if (microjs_exec(&vm, code, n) < 0)
		return 1;

	fprintf(f, "\r\n");

	return 0;

}

int32_t isqrt(uint32_t x);

int cmd_prime(FILE * f, int argc, char ** argv)
{
	int32_t n, j;
	bool prime;

	srand(0);

	printf("----------------------\n");

	for (j = 0; j < 100; ) {
		n = rand();
		if (n <= 3) {
			prime = n > 1;
		} else {
			if (n % 2 == 0 || n % 3 == 0) {
				prime = false;
			} else {
				int32_t i;
				int32_t m;
				m = isqrt(n) + 1;
				prime = true;
				for (i = 5; (i < m) && (prime); i = i + 6) {
					if (n % i == 0 || n % (i + 2) == 0) {
						prime = false;
					}
				}
			}
		}
		if (prime) {
			j = j + 1;
			printf("%3d %12d\n", j, n);
		}
	}

	printf("----------------------\n");

	return 0;
}

const struct shell_cmd cmd_tab[] = {

	{ cmd_help, "help", "?", 
		"[COMMAND]", "show command usage (help [CMD])" },

	{ cmd_rx, "rx", "r", "FILENAME", "XMODEM file receive" },

	{ cmd_cat, "cat", "", "<filename>", "display file content" },

	{ cmd_ls, "ls", "", "<filename>", "list files" },

	{ cmd_rm, "rm", "", "<filename>", "remove files" },

	{ cmd_js, "js", "", "script", "javascript" },

	{ cmd_prime, "prime", "", "", "" },

	{ NULL, "", "", NULL, NULL }
};

