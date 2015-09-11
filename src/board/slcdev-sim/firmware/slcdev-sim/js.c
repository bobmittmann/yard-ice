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

#include <thinkos.h>
#include <microjs.h>

#include "profclk.h"
#include "slcdev.h"
#include "slcdev-lib.h"

#include <sys/dcclog.h>

uint32_t slcdev_symbuf[64]; /* symbol table buffer */

int js(FILE * f, char * script, unsigned int len)
{
	struct symtab * symtab = (struct symtab *)slcdev_symbuf; /* symbols */
	uint8_t code[512]; /* compiled code */
	uint32_t sdtbuf[64]; /* compiler buffer */
	int32_t stack[16]; /* exec stack */
	struct microjs_sdt * microjs; 
	struct microjs_rt * rt;
	struct microjs_vm vm; 
	struct symstat symstat;
	uint32_t start_clk;
	uint32_t stop_clk;
	int code_sz;
	int ret;

	profclk_init();

	/* initialize compiler */
	if ((microjs = microjs_sdt_init(sdtbuf, sizeof(sdtbuf), 
									symtab, &slcdev_lib)) == NULL) {
		fprintf(f, "# internal error\n");
		return -1;
	}

	symstat = symtab_state_save(symtab);

	/* start the syntax direct translation */
	microjs_sdt_begin(microjs, code, sizeof(code));

	start_clk = profclk_get();
	/* compile */
	microjs_sdt_begin(microjs, code, sizeof(code));
	if ((ret = microjs_compile(microjs, script, len)) < 0) {
		symtab_state_rollback(symtab, symstat);
		fprintf(f, "# compile error: %d\n", -ret);
		DCC_LOG1(LOG_WARNING, "compile error: %d", ret);
		microjs_sdt_error(f, microjs, ret);
		DCC_LOG(LOG_TRACE, "...");
		return -1;
	}

	/* end the syntax direct translation */
	if ((ret = microjs_sdt_end(microjs)) < 0) {
		symtab_state_rollback(symtab, symstat);
		fprintf(f, "# translation error: %d\n", -ret);
		DCC_LOG1(LOG_WARNING, "translation error: %d", ret);
		microjs_sdt_error(f, microjs, ret);
		DCC_LOG(LOG_TRACE, "...");
		return -1;
	}

	stop_clk = profclk_get();

	/* get the compiled code size */
	code_sz = ret;
	rt = symtab_rt_get(symtab);

	if (SLCDEV_VERBOSE()) {
		fprintf(f, " - Compile time: %d us.\n", 
				profclk_us(stop_clk - start_clk));
		fprintf(f, " - code: %d\n", code_sz);
		fprintf(f, " - data: %d of %d\n", rt->data_sz, sizeof(slcdev_vm_data));
		fprintf(f, " - stack: %d of %d\n", rt->stack_sz, sizeof(stack));
	}

	if (rt->data_sz > sizeof(slcdev_vm_data)) {
		fprintf(f, "# data overflow. %d bytes required\n", rt->data_sz);
		return -1;
	}

	if (rt->stack_sz > sizeof(stack)) {
		fprintf(f, "# stack overflow. %d bytes required\n", rt->stack_sz);
		return -1;
	}

#if MICROJS_TRACE_ENABLED
	microjs_vm_tracef = f;
#endif

	/* initialize virtual machine instance */
	microjs_vm_init(&vm, rt, NULL, slcdev_vm_data, stack);

	start_clk = profclk_get();
	if ((ret = microjs_exec(&vm, code)) != 0){
		fprintf(f, "# exec error: %d\n", ret);
		return -1;
	}

	if (SLCDEV_VERBOSE()) {
		stop_clk = profclk_get();
		fprintf(f, "Exec time: %d us.\n", profclk_us(stop_clk - start_clk));
	}

	return 0;
}

