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

struct microjs_vm slcdev_vm; 
uint32_t slcdev_symbuf[64]; /* symbol table buffer */
uint8_t slcdev_jscode[512]; /* compiled code */

uint32_t __attribute__((aligned(8))) js_runtime_stack[512];

void simrpc_test(void);

#undef SLCDEV_VERBOSE 
#define SLCDEV_VERBOSE() (1)

void __attribute__((noreturn)) js_runtime_task(void)
{
	struct symtab * symtab = (struct symtab *)slcdev_symbuf; /* symbols */
	struct microjs_rt * rt;
	uint32_t start_clk;
	uint32_t stop_clk;
	int32_t stack[16]; /* exec stack */
	int ret;

//	simrpc_test();

	for (;;) {
		thinkos_flag_take(JSRUNTIME_FLAG); 

		thinkos_mutex_lock(JSRUNTIME_MUTEX); 

		rt = symtab_rt_get(symtab);
		fprintf(stderr, " - stack: %d of %d\n", rt->stack_sz, sizeof(stack));

		if (rt->stack_sz > sizeof(stack)) {
			fprintf(stderr, "# stack overflow. %d bytes required\n", 
					rt->stack_sz);
		} else {

#if MICROJS_TRACE_ENABLED
			microjs_vm_tracef = stderr;
#endif

			/* initialize virtual machine instance */
			microjs_vm_init(&slcdev_vm, rt, NULL, slcdev_vm_data, stack);

			start_clk = profclk_get();
			if ((ret = microjs_exec(&slcdev_vm, slcdev_jscode)) != 0){
				fprintf(stderr, "# exec error: %d\n", ret);
			} else {
				if (SLCDEV_VERBOSE()) {
					stop_clk = profclk_get();
					fprintf(stderr, "Exec time: %d us.\n", 
							profclk_us(stop_clk - start_clk));
				}
			}
		}

		thinkos_mutex_unlock(JSRUNTIME_MUTEX); 
	}
}

void js_runtime_init(void)
{
	/* create a thread to run microjs scripts */
	thinkos_thread_create((void *)js_runtime_task, (void *)NULL,
						  js_runtime_stack, sizeof(js_runtime_stack) | 
						  THINKOS_OPT_PRIORITY(3) | THINKOS_OPT_ID(3));
}


int js(char * script, unsigned int len)
{
	struct symtab * symtab = (struct symtab *)slcdev_symbuf; /* symbols */
	uint32_t sdtbuf[64]; /* compiler buffer */
	struct microjs_sdt * microjs; 
	struct microjs_rt * rt;
	struct symstat symstat;
	uint32_t start_clk;
	uint32_t stop_clk;
	int code_sz;
	int ret;

	profclk_init();

	/* initialize compiler */
	if ((microjs = microjs_sdt_init(sdtbuf, sizeof(sdtbuf), 
									symtab, &slcdev_lib)) == NULL) {
		fprintf(stderr, "# internal error\n");
		return -1;
	}

	/* foce MicroJS VM to stop */
	microjs_abort(&slcdev_vm);

	thinkos_mutex_lock(JSRUNTIME_MUTEX); 

	symstat = symtab_state_save(symtab);

	/* start the syntax direct translation */
	microjs_sdt_begin(microjs, slcdev_jscode, sizeof(slcdev_jscode));

	start_clk = profclk_get();
	/* compile */
	microjs_sdt_begin(microjs, slcdev_jscode, sizeof(slcdev_jscode));
	if ((ret = microjs_compile(microjs, script, len)) < 0) {
		symtab_state_rollback(symtab, symstat);
		fprintf(stderr, "# compile error: %d\n", -ret);
		DCC_LOG1(LOG_WARNING, "compile error: %d", ret);
		microjs_sdt_error(stderr, microjs, ret);
		DCC_LOG(LOG_TRACE, "...");
		ret = -1;
	} else if ((ret = microjs_sdt_end(microjs)) < 0) {
		/* end the syntax direct translation */
		symtab_state_rollback(symtab, symstat);
		fprintf(stderr, "# translation error: %d\n", -ret);
		DCC_LOG1(LOG_WARNING, "translation error: %d", ret);
		microjs_sdt_error(stderr, microjs, ret);
		DCC_LOG(LOG_TRACE, "...");
		ret = -1;
	} else {
		stop_clk = profclk_get();
		/* get the compiled code size */
		code_sz = ret;
		rt = symtab_rt_get(symtab);

//		if (SLCDEV_VERBOSE()) {
			fprintf(stderr, " - Compile time: %d us.\n", 
					profclk_us(stop_clk - start_clk));
			fprintf(stderr, " - code: %d\n", code_sz);
			fprintf(stderr, " - data: %d of %d\n", rt->data_sz, 
					sizeof(slcdev_vm_data));
//		}

		if (rt->data_sz > sizeof(slcdev_vm_data)) {
			symtab_state_rollback(symtab, symstat);
			fprintf(stderr, "# data overflow. %d bytes required\n", 
					rt->data_sz);
			DCC_LOG1(LOG_WARNING, "data overflow. %d bytes required\n", 
					 rt->data_sz);
			ret = -1;
		} else {
			DCC_LOG(LOG_TRACE, "signaling JS runtime.");
			thinkos_flag_give(JSRUNTIME_FLAG); 
			ret = 0;
		}
	}

	thinkos_mutex_unlock(JSRUNTIME_MUTEX); 

	return ret;
}

int js_pause(void)
{
	struct microjs_vm * vm = &slcdev_vm; 

	if (thinkos_mutex_trylock(JSRUNTIME_MUTEX) < 0) {
		/* foce MicroJS VM to stop */
		microjs_abort(vm);
		thinkos_mutex_lock(JSRUNTIME_MUTEX);

		printf("\nMicroJS: SP=0x%04x XP=0x%04x PC=0x%04x\n", 
			   (int)(vm->sp * sizeof(int32_t)),
			   (int)(vm->xp * sizeof(int32_t)),
			   (int)(vm->pc));

	} else {
		printf("MicroJS VM is not running!\n");
	}

	thinkos_mutex_unlock(JSRUNTIME_MUTEX); 

	return 0;
}

int js_stop(void)
{
	return js_pause();
}

int js_play(void)
{
	if (thinkos_mutex_trylock(JSRUNTIME_MUTEX) < 0) {
		printf("MicroJS VM already running!\n");
		return 0;
	}

	printf("MicroJS VM play.\n");

	thinkos_flag_give(JSRUNTIME_FLAG); 

	thinkos_mutex_unlock(JSRUNTIME_MUTEX); 

	return 0;
}

