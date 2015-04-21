/* 
 * File:	 usb-cdc.c
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

#include <sys/stm32f.h>
#include <arch/cortex-m3.h>
#include <sys/param.h>
#include <sys/serial.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/usb-cdc.h>

#include <sys/dcclog.h>

#include "monitor.h"

const char monitor_banner[] = "\r\n\r\n"
"-------------------------------------------------------------------------\r\n"
"- ThinkOS Monitor\r\n"
"-------------------------------------------------------------------------\r\n"
"\r\n";

char comm_buf[129];

int cprintf(struct dmon_comm * comm, const char *fmt, ... )
{
	char * s = comm_buf;
	va_list ap;
	int n;

	va_start(ap, fmt);
	n = vsnprintf(s, 129, fmt, ap);
	va_end(ap);

	n = MIN(n, 128);

	return dmon_comm_send(comm, s, n);
}

int osinfo(struct dmon_comm * comm);

void show_context(struct dmon_comm * comm, 
				  const struct thinkos_context * ctx, 
				  uint32_t sp)
{
	uint32_t psr = ctx->xpsr;
	char * s = comm_buf;
	int n;

	n = sprintf(s, " xpsr=%08x [N=%c Z=%c C=%c V=%c Q=%c "
				"ICI/IT=%02x XCP=%02x]\r\n", 
				psr,
				((psr >> 31) & 0x01) + '0',
				((psr >> 30) & 0x01) + '0',
				((psr >> 29) & 0x01) + '0',
				((psr >> 28) & 0x01) + '0',
				((psr >> 27) & 0x01) + '0',
				((psr >> 19) & 0xc0) | ((psr >> 10) & 0x3f),
				psr & 0xff);
	dmon_comm_send(comm, s, n);
	n = sprintf(s, "   r0=%08x   r4=%08x   r8=%08x  r12=%08x\r\n",
				ctx->r0, ctx->r4, ctx->r8, ctx->r12);
	dmon_comm_send(comm, s, n);
	n = sprintf(s, "   r1=%08x   r5=%08x   r9=%08x   sp=%08x\r\n", 
				ctx->r1, ctx->r5, ctx->r9, sp);
	dmon_comm_send(comm, s, n);
	n = sprintf(s, "   r2=%08x   r6=%08x  r10=%08x   lr=%08x\r\n", 
				ctx->r2, ctx->r6, ctx->r10, ctx->lr);
	dmon_comm_send(comm, s, n);
	n = sprintf(s, "   r3=%08x   r7=%08x  r11=%08x   pc=%08x\r\n",  
				ctx->r3, ctx->r7, ctx->r11, ctx->pc);
	dmon_comm_send(comm, s, n);
}

void monitor_on_fault(struct dmon_comm * comm)
{
	const struct thinkos_except * xcpt = &thinkos_except_buf;
	char * s = comm_buf;
	int n;

	DCC_LOG(LOG_TRACE, "DMON_THREAD_FAULT.");
	n = sprintf(s, "Fault at thread %d\r\n", xcpt->thread);
	dmon_comm_send(comm, s, n);
	show_context(comm, &xcpt->ctx, xcpt->sp);
}

const char monitor_menu[] = "\r\n\r\n"
	 "ThinkOS Monitor Demo\r\n"
	 "  [r] - restart monitor\r\n"
	 "  [x] - show exception\r\n"
	 "  [t] - comm test\r\n"
	 "  [i] - ThinkOS info\r\n"
	 "> ";

void show_menu(struct dmon_comm * comm)
{
	dmon_comm_send(comm, monitor_menu, sizeof(monitor_menu) - 1);
}

void test(struct dmon_comm * comm)
{
			cprintf(comm, "\r\n");
			cprintf(comm, 
	"0123456789abcdef0123456789ABCDEF0123456789abcdef0123456789ABCDEF"
	"0123456789abcdef0123456789ABCDEF0123456789abcdef0123456789ABCDEF");
			cprintf(comm, "\r\n");
//			cprintf(comm, 
//	"0123456789abcdef0123456789ABCDEF0123456789abcdef0123456789ABCDEF"
//	"0123456789abcdef0123456789ABCDEF0123456789abcdef0123456789ABCDEF");
//			cprintf(comm, "\r\n");
//			cprintf(comm, 
//	"0123456789abcdef0123456789ABCDEF0123456789abcdef0123456789ABCDEF"
//	"0123456789abcdef0123456789ABCDEF0123456789abcdef0123456789ABCDEF");
}


void __attribute__((noreturn)) monitor_task(struct thinkos_dmon * mon,
											struct dmon_comm * comm)
{
	uint32_t sigmask;
	uint32_t sigset;
	char buf[80];
	int n;
	int c;

	DCC_LOG(LOG_TRACE, "Monitor start...");
	dmon_comm_connect(comm);

	DCC_LOG(LOG_TRACE, "Comm connected..");

	dmon_comm_send(comm, monitor_banner, 
					  sizeof(monitor_banner) - 1);

	sigmask = (1 << DMON_THREAD_FAULT);
	sigmask |= (1 << DMON_COMM_RCV);
	for(;;) {
		
		sigset = dmon_select(sigmask);
		if (sigset & (1 << DMON_THREAD_FAULT)) {
			DCC_LOG(LOG_TRACE, "Thread fault.");
			monitor_on_fault(comm);
			dmon_clear(DMON_THREAD_FAULT);
		}

		if (sigset & (1 << DMON_COMM_RCV)) {
			DCC_LOG(LOG_INFO, "Comm RX.");
			n = dmon_comm_recv(comm, buf, 64);
			DCC_LOG1(LOG_INFO, "n=%d...", n);
			c = buf[0];
//			buf[1] = '\r';
//			buf[2] = '\n';
//			dmon_comm_send(comm, buf, 3);
			switch (c) {
			case 'r':
				dmon_signal(DMON_START);
				break;
			case 'x':
				monitor_on_fault(comm);
				break;
			case 't':
				test(comm);
				break;
			case 'i':
				osinfo(comm);
				break;
			default:
				show_menu(comm);
				break;
			}
		}
	}
}


void monitor_init(void)
{
	void * comm;
	
	comm = usb_mon_init(&stm32f_otg_fs_dev, 
						cdc_acm_def_str, 
						cdc_acm_def_strcnt);

	thinkos_dmon_init(comm, monitor_task);
}


static const char obj_type_name[][8] = {
	"Ready",
	"Sched",
	"Cancl",
	"Pausd",
	"Clock",
	"Mutex",
	"Cond",
	"Sem",
	"Event",
	"Flag",
	"Join",
	"Inv"
};

#if THINKOS_ENABLE_THREAD_ALLOC | THINKOS_ENABLE_MUTEX_ALLOC | \
	THINKOS_ENABLE_COND_ALLOC | THINKOS_ENABLE_SEM_ALLOC | \
	THINKOS_ENABLE_EVENT_ALLOC | THINKOS_ENABLE_FLAG_ALLOC
static int bmp_bit_cnt(uint32_t bmp[], int bits)
{
	int cnt = 0;
	int j;
	int i;

	for (j = 0; j < (bits / 32); ++j)
		for (i = 0; i < 32; ++i)
			cnt += ((1 << i) & bmp[j]) ? 1: 0;

	for (i = 0; i < bits % 32; ++i)
		cnt += ((1 << i) & bmp[j]) ? 1: 0;

	return cnt;
}
#endif

static void os_alloc_dump(struct dmon_comm * comm, struct thinkos_rt * rt)
{
	cprintf(comm, "     ");
#if THINKOS_ENABLE_MUTEX_ALLOC
	cprintf(comm, "   Thread");
#endif
#if THINKOS_ENABLE_MUTEX_ALLOC
	cprintf(comm, "    Mutex");
#endif
#if THINKOS_ENABLE_COND_ALLOC
	cprintf(comm, "     Cond");
#endif
#if THINKOS_ENABLE_SEM_ALLOC
	cprintf(comm, "  Semaphr");
#endif
#if THINKOS_ENABLE_EVENT_ALLOC
	cprintf(comm, "    Event");
#endif
#if THINKOS_ENABLE_FLAG_ALLOC
	cprintf(comm, "     Flag");
#endif
	cprintf(comm, "\r\n");

	cprintf(comm, " Cnt:");
#if THINKOS_ENABLE_THREAD_ALLOC
	cprintf(comm, "%6d/%-2d", bmp_bit_cnt(rt->th_alloc, THINKOS_THREADS_MAX), 
			THINKOS_THREADS_MAX);
#endif
#if THINKOS_ENABLE_MUTEX_ALLOC
	cprintf(comm, "%6d/%-2d", bmp_bit_cnt(rt->mutex_alloc, THINKOS_MUTEX_MAX), 
			THINKOS_MUTEX_MAX);
#endif
#if THINKOS_ENABLE_COND_ALLOC
	cprintf(comm, "%6d/%-2d", bmp_bit_cnt(rt->cond_alloc, THINKOS_COND_MAX),
			THINKOS_COND_MAX);
#endif
#if THINKOS_ENABLE_SEM_ALLOC
	cprintf(comm, "%6d/%-2d", bmp_bit_cnt(rt->sem_alloc, THINKOS_SEMAPHORE_MAX), 
			THINKOS_SEMAPHORE_MAX);
#endif
#if THINKOS_ENABLE_EVENT_ALLOC
	cprintf(comm, "%6d/%-2d", bmp_bit_cnt(rt->ev_alloc, THINKOS_EVENT_MAX),
			THINKOS_EVENT_MAX);
#endif
#if THINKOS_ENABLE_FLAG_ALLOC
	cprintf(comm, "%6d/%-2d", bmp_bit_cnt(rt->flag_alloc, THINKOS_FLAG_MAX),
			THINKOS_FLAG_MAX);
#endif
	cprintf(comm, "\r\n");

}

int osinfo(struct dmon_comm * comm)
{
	struct thinkos_rt * rt = &thinkos_rt;
	uint32_t * wq;
	int i;
#if THINKOS_ENABLE_PROFILING
	uint32_t cycdiv;
	uint32_t busy;
#endif
#if THINKOS_MUTEX_MAX > 0
	int j;
#endif


	cprintf(comm, "\r\n");
#if THINKOS_ENABLE_CLOCK
	cprintf(comm, "[ Ticks = %d ]", rt->ticks);
#endif

#if THINKOS_ENABLE_PROFILING
	{
		int self = thinkos_rt.active;
		uint32_t cyccnt = CM3_DWT->cyccnt;
		int32_t delta = cyccnt - thinkos_rt.cycref;
		/* update the reference */
		thinkos_rt.cycref = cyccnt;
		/* update thread's cycle counter */
		thinkos_rt.cyccnt[self] += delta; 
	}
	{
		uint32_t cycsum = 0;
		uint32_t cycbusy;
		uint32_t idle;
//		uint32_t sys;

		cycsum = 0;
		for (i = 0; i < THINKOS_THREADS_MAX; ++i)
			cycsum += rt->cyccnt[i];
		cycbusy = cycsum;
		cycsum += rt->cyccnt[THINKOS_CYCCNT_IDLE];
//		cycsum += rt->cyccnt[THINKOS_CYCCNT_SYS];

		cycdiv = (cycsum + 500) / 1000;
		busy = (cycbusy + cycdiv / 2) / cycdiv;
		idle = 1000 - busy;
		cprintf(comm, " [ %u cycles | %d.%d%% busy | %d.%d%% idle ]", 
				cycsum, busy / 10, busy % 10, idle / 10, idle % 10);
	}

#endif

	cprintf(comm, "\r\n");

	cprintf(comm, " Th"); 
#if THINKOS_ENABLE_THREAD_INFO
	cprintf(comm, " |     Tag"); 
	cprintf(comm, " |    Stack"); 
#endif
	cprintf(comm, " |  Context"); 
#if THINKOS_ENABLE_THREAD_STAT
	cprintf(comm, " |  WQ | TmW"); 
#endif
#if THINKOS_ENABLE_TIMESHARE
	cprintf(comm, " |  Val |  Pri"); 
#endif
#if THINKOS_ENABLE_CLOCK
	cprintf(comm, " | Clock (ms)"); 
#endif
#if THINKOS_ENABLE_PROFILING
	cprintf(comm, " | CPU %%"); 
#endif

#if THINKOS_MUTEX_MAX > 0
	cprintf(comm, " | Locks\r\n"); 
#else
	cprintf(comm, " |\r\n");
#endif

	for (i = 0; i < THINKOS_THREADS_MAX; ++i) {
		if (rt->ctx[i] != NULL) {
			cprintf(comm, "%3d", i);
#if THINKOS_ENABLE_THREAD_INFO
			if (rt->th_inf[i] != NULL) {
				cprintf(comm, " | %7s", rt->th_inf[i]->tag); 
				cprintf(comm, " | %08x", (uint32_t)rt->th_inf[i]->stack_ptr); 
			} else {
				cprintf(comm, " |     ..."); 
				cprintf(comm, " |      ..."); 
			}
#endif
			cprintf(comm, " | %08x", (uint32_t)rt->ctx[i]); 
#if THINKOS_ENABLE_THREAD_STAT
			cprintf(comm, " | %3d | %s", rt->th_stat[i] >> 1, 
					rt->th_stat[i] & 1 ? "Yes" : " No"); 
#endif
#if THINKOS_ENABLE_TIMESHARE
			cprintf(comm, " | %4d | %4d", rt->sched_val[i], rt->sched_pri[i]); 
#endif
#if THINKOS_ENABLE_CLOCK
			{
				int32_t dt = (int32_t)(rt->clock[i] - rt->ticks);
				if (dt < 0)
					cprintf(comm, " | <timedout>"); 
				else
					cprintf(comm, " | %10d", dt); 
			}
#endif
#if THINKOS_ENABLE_PROFILING
			busy = (rt->cyccnt[i] + cycdiv / 2) / cycdiv;
			cprintf(comm, " | %3d.%d", busy / 10, busy % 10);
#endif
			cprintf(comm, " |");
#if THINKOS_MUTEX_MAX > 0
			for (j = 0; j < THINKOS_MUTEX_MAX ; ++j) {
				if (rt->lock[j] == i)
					cprintf(comm, " %d", j + THINKOS_MUTEX_BASE);
			}
#endif 
			cprintf(comm, "\r\n");
		}
#if THINKOS_ENABLE_PROFILING
		/* Reset cycle counters */
		rt->cyccnt[i] = 0; 
#endif
	}

	for (wq = rt->wq_lst; wq != rt->wq_end; ++wq) {
		int oid;
		int type;
		if (*wq) { 
			oid = wq - rt->wq_lst;
			type = thinkos_obj_type_get(oid);
			cprintf(comm, "%3d %5s:", oid, obj_type_name[type]);
			for (i = 0; i < THINKOS_THREADS_MAX; ++i) {
				if (*wq & (1 << i)) 
					cprintf(comm, " %d", i);
			}
#if THINKOS_MUTEX_MAX > 0
			if (type == THINKOS_OBJ_MUTEX)
				cprintf(comm, " [lock=%d]", rt->lock[oid - THINKOS_MUTEX_BASE]);
#endif 
			cprintf(comm, "\r\n");
		}
	}

	os_alloc_dump(comm, rt);

	return 0;
}

#if 0
int cmd_thread(struct dmon_comm * comm)
{
	uint8_t lst[THINKOS_THREADS_MAX];
	struct thinkos_context * ctx;
	struct thinkos_rt rt;
	unsigned int th;
	int oid;
	int type;
	int cnt;
#if THINKOS_MUTEX_MAX > 0
	int j;
#endif
	int i;

	thinkos_rt_snapshot(&rt);

	if (argc == 1) {
		// no arguments (dump the current thread)
		lst[0] = thinkos_thread_self();
		cnt = 1;
	} else {
		// read all arguments from command line
		cnt = (argc - 1);
		for (i = 0; i < cnt; ++i) {
			lst[i] = strtoul(argv[i + 1], NULL, 0);
		}
	}

	for (i = 0; i < cnt; ++i) {
		th = lst[i];
		if ((th >= THINKOS_THREADS_MAX) || (rt->ctx[th] == NULL)) {
			cprintf(comm, "Thread %d is invalid!\n", th);
			return SHELL_ERR_ARG_INVALID;
		}

		oid = rt->th_stat[th] >> 1;
		type = thinkos_obj_type_get(oid);

		cprintf(comm, " - Id: %d", th); 

#if THINKOS_ENABLE_THREAD_INFO
		if (rt->th_inf[th] != NULL)
			cprintf(comm, ", %s", rt->th_inf[th]->tag); 
		else
			cprintf(comm, ", %s", "..."); 
#endif
		cprintf(comm, "\n"); 

#if THINKOS_ENABLE_THREAD_STAT
		cprintf(comm, " - Waiting on queue: %3d %5s (time wait: %s)\n", 
				oid, obj_type_name[type], rt->th_stat[th] & 1 ? "Yes" : " No"); 
#endif

#if THINKOS_ENABLE_TIMESHARE
		cprintf(comm, " - Scheduler: val=%d pri=%4d\n", 
				rt->sched_val[th], rt->sched_pri[th]); 
#endif
#if THINKOS_ENABLE_CLOCK
		cprintf(comm, " - Clock: val=%d time=%d\n", rt->clock[th],
				(int32_t)(rt->clock[th] - rt->ticks)); 
#endif

#if THINKOS_MUTEX_MAX > 0
		cprintf(comm, " - Mutex Locks: ");
		for (j = 0; j < THINKOS_MUTEX_MAX ; ++j) {
			if (rt->lock[j] == th)
				cprintf(comm, " %d", j + THINKOS_MUTEX_BASE);
		}
		cprintf(comm, "\n");
#endif 

		ctx = rt->ctx[th];

		cprintf(comm, " - Context: 0x%08x\n", (uint32_t)ctx); 

		cprintf(comm, "     r0=%08x  r1=%08x  r2=%08x  r3=%08x\n", 
				ctx->r0, ctx->r1, ctx->r2, ctx->r3);
		cprintf(comm, "     r4=%08x  r5=%08x  r6=%08x  r7=%08x\n", 
				ctx->r4, ctx->r7, ctx->r6, ctx->r7);
		cprintf(comm, "     r8=%08x  r9=%08x r10=%08x r11=%08x\n", 
				ctx->r8, ctx->r9, ctx->r10, ctx->r11);
		cprintf(comm, "    r12=%08x  sp=%08x  lr=%08x  pc=%08x\n", 
				ctx->r12, (uint32_t)ctx, ctx->lr, ctx->pc);
		cprintf(comm, "   xpsr=%08x\n", ctx->xpsr);

		cprintf(comm, "\n");
	}

	return 0;
}
#endif

#if 0
int scan_stack(FILE * f, uint32_t * ptr, unsigned int size)
{
	int i;

	for (i = 0; i < size / 4; ++i) {
		if (ptr[i] != 0xdeadbeef)
			break;
	}
	cprintf(comm, " | %6d ", i * 4);

	return 0;
}

int cmd_oscheck(FILE * f, int argc, char ** argv)
{
	struct thinkos_rt rt;
	int i;

	if (argc > 1)
		return SHELL_ERR_EXTRA_ARGS;

	if (thinkos_rt_snapshot(&rt) < 0) {
		cprintf(comm, "#ERROR: Thinkos RT_DEBUG not enabled!\n");
		return SHELL_ERR_GENERAL;
	}

	cprintf(comm, "\n");

	cprintf(comm, " Th"); 
#if THINKOS_ENABLE_THREAD_INFO
	cprintf(comm, " |     Tag"); 
	cprintf(comm, " |    Stack"); 
#endif
	cprintf(comm, " |  Context"); 
#if THINKOS_ENABLE_THREAD_INFO
	cprintf(comm, " |   Size"); 
	cprintf(comm, " |   Free"); 
#endif
	cprintf(comm, "\n");

	for (i = 0; i < THINKOS_THREADS_MAX; ++i) {
		if (rt->ctx[i] != NULL) {
			cprintf(comm, "%3d", i);
#if THINKOS_ENABLE_THREAD_INFO
			if (rt->th_inf[i] != NULL) {
				cprintf(comm, " | %7s", rt->th_inf[i]->tag); 
				cprintf(comm, " | %08x", (uint32_t)rt->th_inf[i]->stack_ptr); 
			} else {
				cprintf(comm, " |     ..."); 
				cprintf(comm, " |      ..."); 
			}
#endif
			cprintf(comm, " | %08x", (uint32_t)rt->ctx[i]); 
#if THINKOS_ENABLE_THREAD_INFO
			cprintf(comm, " | %6d", rt->th_inf[i]->stack_size); 
			scan_stack(f, rt->th_inf[i]->stack_ptr, rt->th_inf[i]->stack_size);
#endif
			cprintf(comm, "\n");
		}
	}

	return 0;
}
#endif

