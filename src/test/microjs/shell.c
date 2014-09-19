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

uint8_t  vm_code[256];  /* compiled code */
uint32_t vm_data[64];   /* data area */
uint16_t vm_strbuf[64]; /*string buffer shuld be 16bits aligned */

int cmd_js(FILE * f, int argc, char ** argv)
{
	struct fs_dirent entry;
	uint32_t sdtbuf[64]; /* compiler buffer */
	struct microjs_sdt * microjs; 
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
	strbuf_init(vm_strbuf, sizeof(vm_strbuf));
	/* initialize symbol table */
	symtab = symtab_init(vm_data, sizeof(vm_data), &externals);
	/* initialize compiler */
	microjs = microjs_sdt_init(sdtbuf, sizeof(sdtbuf), symtab, 
							  vm_code, sizeof(vm_code), sizeof(vm_data));


	if (!fs_dirent_lookup(argv[1], &entry)) {
		fprintf(f, "invalid file: \"%s\"\n", argv[1]);
		return SHELL_ERR_ARG_INVALID;
	}

	fprintf(f, "\"%s\"\n", entry.fp->name);
	script = (char *)entry.fp->data;
	len = entry.fp->size;

	/* compile */
	if ((n = microjs_compile(microjs, script, len)) < 0) {
		fprintf(f, "# compile error: %d\n", -n);
		return -1;
	}

	if ((n = microjs_sdt_done(microjs)) < 0) {
		fprintf(f, "# compile error: %d\n", -n);
		return -1;
	}

	fprintf(f, "Code: %d bytes.\n", n);
	fprintf(f, "Data: %d bytes.\n", microjs_tgt_heap(microjs));
	microjs_vm_init(&vm, (int32_t *)vm_data, sizeof(vm_data));

	if ((n = microjs_exec(&vm, vm_code, n)) < 0){
		fprintf(f, "# exec error: %d\n", -n);
		return -1;
	}

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

	{ NULL, "", "", NULL, NULL }
};

