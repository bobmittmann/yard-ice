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
#include "profclk.h"

#define __TEST_LIB_DEF__
#include "test_lib.h"

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

uint8_t  vm_code[256];  /* compiled code */
uint16_t vm_strbuf[128]; /*string buffer shuld be 16bits aligned */
int32_t vm_data[32];   /* data area */
int32_t vm_stack[32];   /* data area */
uint32_t js_symbuf[64]; /*string buffer shuld be 16bits aligned */
uint32_t js_sdtbuf[64]; /* compiler buffer */

struct microjs_rt rt;
struct microjs_vm vm; 

void vm_reset(void) 
{
	struct symtab * symtab;

	/* initialize string buffer */
	strbuf_init(vm_strbuf, sizeof(vm_strbuf));
	/* initialize virtual machine */
	microjs_vm_init(&vm, &rt, NULL, vm_data, vm_stack);
	/* clear data */
	microjs_vm_clr_data(&vm, &rt);
	/* initialize symbol table */
	symtab = symtab_init(js_symbuf, sizeof(js_symbuf), &test_lib);
	/* initialize compiler */
	microjs_sdt_init(js_sdtbuf, sizeof(js_sdtbuf), symtab);
}

int cmd_vm(FILE * f, int argc, char ** argv)
{
	if (argc != 1)
		return SHELL_ERR_EXTRA_ARGS;

	vm_reset();
	return 0;
}

int cmd_js(FILE * f, int argc, char ** argv)
{
	struct microjs_sdt * microjs = (struct microjs_sdt *)js_sdtbuf;
	struct symtab * symtab = (struct symtab *)js_symbuf;
	struct fs_dirent entry;
	struct microjs_vm vm; 
	char * script;
	uint32_t start_clk;
	uint32_t stop_clk;
	struct symstat symstat;
	int code_sz;
	int len;
	int err;
	int n;

	if (argc < 2)
		return SHELL_ERR_ARG_MISSING;

	if (argc > 2)
		return SHELL_ERR_EXTRA_ARGS;

	profclk_init();

	/* initialize virtual machine */
	microjs_vm_init(&vm, &rt, NULL, vm_data, vm_stack);

	symstat = symtab_state_save(symtab);

	microjs_sdt_begin(microjs, vm_code, sizeof(vm_code));

	if (!fs_dirent_lookup(argv[1], &entry)) {
		fprintf(f, "invalid file: \"%s\"\n", argv[1]);
		return SHELL_ERR_ARG_INVALID;
	}

	fprintf(f, "\"%s\"\n", entry.fp->name);
	script = (char *)entry.fp->data;
	len = entry.fp->size;

	/* compile */
	start_clk = profclk_get();
	if ((n = microjs_compile(microjs, script, len)) < 0) {
		symtab_state_rollback(symtab, symstat);
		fprintf(f, "# compile error: %d\n", -n);
		microjs_sdt_error(stderr, microjs, n);
		return -1;
	}

	if ((code_sz = microjs_sdt_end(microjs, &rt)) < 0) {
		symtab_state_rollback(symtab, symstat);
		fprintf(f, "# compile error: %d\n", -code_sz);
		microjs_sdt_error(stderr, microjs, code_sz);
		return -1;
	}
	stop_clk = profclk_get();

	printf(" - Comile time: %d us.\n", profclk_us(stop_clk - start_clk ));
	printf(" -   Code size: %d\n", code_sz);
	printf(" -   Data size: %d\n", rt.data_sz);
	printf(" -  Stack size: %d\n", rt.stack_sz);


	start_clk = profclk_get();
	if ((err = microjs_exec(&vm, vm_code, code_sz)) != 0) {
		fprintf(f, "\n#exec error: code %d!\n", err);
		return -1;
	}
	stop_clk = profclk_get();
	fprintf(f, "Exec time: %d us.\n", profclk_us(stop_clk - start_clk ));

	return 0;

}

int32_t isqrt(uint32_t x);

int cmd_prime(FILE * f, int argc, char ** argv)
{
	uint32_t start_clk;
	uint32_t stop_clk;
	int32_t n, j, x;
	int cnt;
	bool prime;

	profclk_init();
	start_clk = profclk_get();

	srand(0);
	cnt = 0;

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
		cnt = cnt + 1;
	}

	printf("----------------------\n");

	x = (j * 10000) / cnt;

	printf("%d out of %d are prime, %d.%d %%.\n",
		   j, cnt, x / 100, x % 100);

	printf("---\n\n");

	stop_clk = profclk_get();
	fprintf(f, "Run time: %d us.\n", profclk_us(stop_clk - start_clk ));

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

	{ cmd_vm, "vm", "", "", "reset virtual machine" },
   
	{ cmd_prime, "prime", "", "", "" },

	{ NULL, "", "", NULL, NULL }
};

