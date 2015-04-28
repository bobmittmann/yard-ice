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
#include <dis-asm.h>
#include <sys/dcclog.h>

#include "monitor.h"

int osinfo(struct dmon_comm * comm);
void show_thread_info(struct dmon_comm * comm, int id);
int target_print_insn(struct dmon_comm * comm, uint32_t addr);

char comm_buf[129];

int dmprintf(struct dmon_comm * comm, const char *fmt, ... )
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

void show_context(struct dmon_comm * comm, 
				  const struct thinkos_context * ctx, 
				  uint32_t sp)
{
	uint32_t psr = ctx->xpsr;

	dmprintf(comm, " xpsr=%08x [N=%c Z=%c C=%c V=%c Q=%c "
				"ICI/IT=%02x XCP=%02x]\r\n", 
				psr,
				((psr >> 31) & 0x01) + '0',
				((psr >> 30) & 0x01) + '0',
				((psr >> 29) & 0x01) + '0',
				((psr >> 28) & 0x01) + '0',
				((psr >> 27) & 0x01) + '0',
				((psr >> 19) & 0xc0) | ((psr >> 10) & 0x3f),
				psr & 0xff);
	dmprintf(comm, "   r0=%08x   r4=%08x   r8=%08x  r12=%08x\r\n",
				ctx->r0, ctx->r4, ctx->r8, ctx->r12);
	dmprintf(comm, "   r1=%08x   r5=%08x   r9=%08x   sp=%08x\r\n", 
				ctx->r1, ctx->r5, ctx->r9, sp);
	dmprintf(comm, "   r2=%08x   r6=%08x  r10=%08x   lr=%08x\r\n", 
				ctx->r2, ctx->r6, ctx->r10, ctx->lr);
	dmprintf(comm, "   r3=%08x   r7=%08x  r11=%08x   pc=%08x\r\n",  
				ctx->r3, ctx->r7, ctx->r11, ctx->pc);
}

void monitor_on_fault(struct dmon_comm * comm)
{
	const struct thinkos_except * xcpt = &thinkos_except_buf;

	DCC_LOG(LOG_TRACE, "DMON_THREAD_FAULT.");
	dmprintf(comm, "Fault at thread %d\r\n", xcpt->thread);
	show_context(comm, &xcpt->ctx, xcpt->sp);
}

int thread_id = -1;

void monitor_on_step(struct dmon_comm * comm)
{
	struct thinkos_thread st;
	struct thinkos_context * ctx;
	int id = thinkos_rt.step_id;
	int type;

	if (__thinkos_thread_get(&thinkos_rt, &st, id) < 0) {
		dmprintf(comm, "Thread %d is invalid!\r\n", id);
		return;
	}

	type = thinkos_obj_type_get(st.wq);

	if (st.th_inf != NULL)
		dmprintf(comm, " <%2d> '%s': ", id, st.th_inf->tag); 
	else
		dmprintf(comm, " <%2d> '...': ", id); 

	if (THINKOS_OBJ_READY == type) {
		dmprintf(comm, " %s.\r\n", thinkos_type_name_lut[type]); 
	} else {
		dmprintf(comm, " %swait on %s(%3d)\r\n", 
				 st.tmw ? "time" : "", thinkos_type_name_lut[type], st.wq ); 
	}

	ctx = &st.ctx;

	dmprintf(comm, " %08x:  ", ctx->pc & ~1);
	target_print_insn(comm, ctx->pc);
	dmprintf(comm, "\r\n");
}


void monitor_step_sync(struct dmon_comm * comm)
{
	DCC_LOG1(LOG_TRACE, "Step %d", thread_id);
	if (dmon_thread_step(thread_id, true) < 0) {
		dmprintf(comm, "dmon_thread_step() failed!\r\n");
		return;
	}
	monitor_on_step(comm);
}


void monitor_step_assync(struct dmon_comm * comm)
{
	DCC_LOG1(LOG_TRACE, "Step %d", thread_id);

	if (dmon_thread_step(thread_id, false) < 0) {
		dmprintf(comm, "dmon_thread_step() failed!\r\n");
	}
}

void monitor_dump(struct dmon_comm * comm)
{
	show_thread_info(comm, thread_id);
}

const char monitor_banner[] = "\r\n\r\n"
"-------------------------------------------------------------------------\r\n"
"- ThinkOS Monitor\r\n"
"-------------------------------------------------------------------------\r\n"
"\r\n";

const char monitor_menu[] = "\r\n"
	 "-- ThinkOS Monitor Commands ---------\r\n"
	 " Ctrl+Q - Restart monitor\r\n"
	 " Ctrl+R - Resume all threads\r\n"
	 " Ctrl+P - Pause all threads\r\n"
	 " Ctrl+X - Show exception\r\n"
	 " Ctrl+S - Thread Step\r\n"
	 " Ctrl+T - Comm test\r\n"
	 " Ctrl+I - ThinkOS info\r\n"
	 " Ctrl+N - Select Next Thread\r\n"
	 "-------------------------------------\r\n\r\n";

void show_help(struct dmon_comm * comm)
{
	dmon_comm_send(comm, monitor_menu, sizeof(monitor_menu) - 1);
}

void test(struct dmon_comm * comm)
{
			dmprintf(comm, "\r\n");
			dmprintf(comm, 
	"0123456789abcdef0123456789ABCDEF0123456789abcdef0123456789ABCDEF"
	"0123456789abcdef0123456789ABCDEF0123456789abcdef0123456789ABCDEF");
			dmprintf(comm, "\r\n");
		
	dmon_sleep(100);
	dmprintf(comm, "1.\r\n");

	dmon_sleep(100);
	dmprintf(comm, "2.\r\n");

//			dmprintf(comm, 
//	"0123456789abcdef0123456789ABCDEF0123456789abcdef0123456789ABCDEF"
//	"0123456789abcdef0123456789ABCDEF0123456789abcdef0123456789ABCDEF");
//			dmprintf(comm, "\r\n");
//			dmprintf(comm, 
//	"0123456789abcdef0123456789ABCDEF0123456789abcdef0123456789ABCDEF"
//	"0123456789abcdef0123456789ABCDEF0123456789abcdef0123456789ABCDEF");
}

#if (!THINKOS_ENABLE_PAUSE)
#error "Need THINKOS_ENABLE_PAUSE!"
#endif

void monitor_pause_all(struct dmon_comm * comm)
{
	dmprintf(comm, "\r\nPausing all threads...\r\n");
	__thinkos_pause_all();
	dmon_wait_idle();
	dmprintf(comm, "[Idle]\r\n");
}

void monitor_resume_all(struct dmon_comm * comm)
{
	dmprintf(comm, "\r\nResuming all threads...\r\n");
	__thinkos_resume_all();
	dmprintf(comm, "Restarting...\r\n");
}

#define CTRL_B 0x02
#define CTRL_C 0x03
#define CTRL_D 0x04
#define CTRL_E 0x05
#define CTRL_F 0x06
#define CTRL_G 0x07
#define CTRL_H 0x08
#define CTRL_I 0x09
#define CTRL_J 0x0a
#define CTRL_K 0x0b
#define CTRL_L 0x0c
#define CTRL_M 0x0d /* CR */
#define CTRL_N 0x0e
#define CTRL_O 0x0f
#define CTRL_P 0x10
#define CTRL_Q 0x11
#define CTRL_R 0x12
#define CTRL_S 0x13
#define CTRL_T 0x14
#define CTRL_U 0x15
#define CTRL_V 0x16
#define CTRL_W 0x17
#define CTRL_X 0x18
#define CTRL_Y 0x19
#define CTRL_Z 0x1a

void gdb_task(struct dmon_comm * comm);

static int process_input(struct dmon_comm * comm, char * buf, int len)
{
	int i;
	int j;
	int c;

	for (i = 0; i < len; ++i) {
		c = buf[i];
		switch (c) {
		case CTRL_Q:
			dmon_reset();
			break;
		case CTRL_G:
			dmprintf(comm, "GDB\r\n");
		case '+':
			dmon_exec(gdb_task);
			break;
		case CTRL_P:
			monitor_pause_all(comm);
			break;
		case CTRL_R:
			monitor_resume_all(comm);
			break;
		case CTRL_T:
			test(comm);
			break;
		case CTRL_I:
			osinfo(comm);
			break;
		case CTRL_N:
			thread_id = __thinkos_thread_getnext(thread_id);
			if (thread_id == - 1)
				thread_id = __thinkos_thread_getnext(thread_id);
			dmprintf(comm, "Current thread = %d\r\n", thread_id);
			show_thread_info(comm, thread_id);
			break;
		case CTRL_X:
			monitor_on_fault(comm);
			break;
		case CTRL_S:
			monitor_step_assync(comm);
			break;
		case CTRL_D:
			monitor_dump(comm);
			break;
		case CTRL_H:
			show_help(comm);
			break;
		default:
			continue;
		}
		/* shift the input to remove the consumed character */
		for (j = i; j < len - 1; ++j)
			buf[j] = buf[j + 1];
		len--;
	}

	return len;
}


void __attribute__((noreturn)) monitor_task(struct dmon_comm * comm)
{
	uint32_t sigmask;
	uint32_t sigset;
	uint8_t * ptr;
	int cnt;
	int len;

	DCC_LOG(LOG_TRACE, "Monitor start...");
	dmon_comm_connect(comm);

	DCC_LOG(LOG_TRACE, "Comm connected..");
//	dmon_sleep(500);
//	dmon_comm_send(comm, monitor_banner, sizeof(monitor_banner) - 1);
//	dmon_comm_send(comm, monitor_menu, sizeof(monitor_menu) - 1);

	thread_id = __thinkos_thread_getnext(-1);

	sigmask = (1 << DMON_THREAD_FAULT);
	sigmask |= (1 << DMON_THREAD_STEP);
	sigmask |= (1 << DMON_COMM_RCV);
	sigmask |= (1 << DMON_COMM_CTL);
	sigmask |= (1 << DMON_TX_PIPE);
	sigmask |= (1 << DMON_RX_PIPE);

	for(;;) {
		
		sigset = dmon_select(sigmask);

		if (sigset & (1 << DMON_THREAD_FAULT)) {
			DCC_LOG(LOG_TRACE, "Thread fault.");
			monitor_on_fault(comm);
			dmon_clear(DMON_THREAD_FAULT);
		}

		if (sigset & (1 << DMON_THREAD_STEP)) {
			DCC_LOG(LOG_TRACE, "Thread step.");
			monitor_on_step(comm);
			dmon_clear(DMON_THREAD_STEP);
		}

		if (sigset & (1 << DMON_COMM_CTL)) {
			DCC_LOG(LOG_TRACE, "Comm Ctl.");
			dmon_clear(DMON_COMM_CTL);
			if (!dmon_comm_isconnected(comm))	
				dmon_reset();
		}

		if (sigset & (1 << DMON_RX_PIPE)) {
			if ((cnt = __console_rx_pipe_ptr(&ptr)) > 0) {
				DCC_LOG1(LOG_INFO, "RX Pipe. rx_pipe.free=%d. "
						 "Unmaksing DMON_COMM_RCV!", cnt);
				sigmask |= (1 << DMON_COMM_RCV);
			} else {
				DCC_LOG(LOG_INFO, "RX Pipe empty!!!");
			}
			dmon_clear(DMON_RX_PIPE);
		}

		if (sigset & (1 << DMON_TX_PIPE)) {
			DCC_LOG(LOG_INFO, "TX Pipe.");
			if ((cnt = __console_tx_pipe_ptr(&ptr)) > 0) {
				len = dmon_comm_send(comm, ptr, cnt);
				__console_tx_pipe_commit(len); 
			} else {
				DCC_LOG(LOG_INFO, "TX Pipe empty!!!");
				dmon_clear(DMON_TX_PIPE);
			}
		}

		if (sigset & (1 << DMON_COMM_RCV)) {
			if ((cnt = __console_rx_pipe_ptr(&ptr)) > 0) {
				DCC_LOG1(LOG_INFO, "Comm recv. rx_pipe.free=%d", cnt);
				if ((len = dmon_comm_recv(comm, ptr, cnt)) > 0) {
					len = process_input(comm, (char *)ptr, len);
					__console_rx_pipe_commit(len); 
				}
			} else {
				DCC_LOG(LOG_INFO, "Comm recv. Masking DMON_COMM_RCV!");
				sigmask &= ~(1 << DMON_COMM_RCV);
			}
		}
	}
}

void console_task(struct dmon_comm * comm);

void monitor_init(void)
{
	struct dmon_comm * comm;
	
	comm = usb_comm_init(&stm32f_otg_fs_dev, 
						 cdc_acm_def_str, 
						 cdc_acm_def_strcnt);

	thinkos_dmon_init(comm, monitor_task);
//	thinkos_dmon_init(comm, console_task);
}


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
	dmprintf(comm, "     ");
#if THINKOS_ENABLE_MUTEX_ALLOC
	dmprintf(comm, "   Thread");
#endif
#if THINKOS_ENABLE_MUTEX_ALLOC
	dmprintf(comm, "    Mutex");
#endif
#if THINKOS_ENABLE_COND_ALLOC
	dmprintf(comm, "     Cond");
#endif
#if THINKOS_ENABLE_SEM_ALLOC
	dmprintf(comm, "  Semaphr");
#endif
#if THINKOS_ENABLE_EVENT_ALLOC
	dmprintf(comm, "    Event");
#endif
#if THINKOS_ENABLE_FLAG_ALLOC
	dmprintf(comm, "     Flag");
#endif
	dmprintf(comm, "\r\n");

	dmprintf(comm, " Cnt:");
#if THINKOS_ENABLE_THREAD_ALLOC
	dmprintf(comm, "%6d/%-2d", bmp_bit_cnt(rt->th_alloc, THINKOS_THREADS_MAX), 
			THINKOS_THREADS_MAX);
#endif
#if THINKOS_ENABLE_MUTEX_ALLOC
	dmprintf(comm, "%6d/%-2d", bmp_bit_cnt(rt->mutex_alloc, THINKOS_MUTEX_MAX), 
			THINKOS_MUTEX_MAX);
#endif
#if THINKOS_ENABLE_COND_ALLOC
	dmprintf(comm, "%6d/%-2d", bmp_bit_cnt(rt->cond_alloc, THINKOS_COND_MAX),
			THINKOS_COND_MAX);
#endif
#if THINKOS_ENABLE_SEM_ALLOC
	dmprintf(comm, "%6d/%-2d", bmp_bit_cnt(rt->sem_alloc, THINKOS_SEMAPHORE_MAX), 
			THINKOS_SEMAPHORE_MAX);
#endif
#if THINKOS_ENABLE_EVENT_ALLOC
	dmprintf(comm, "%6d/%-2d", bmp_bit_cnt(rt->ev_alloc, THINKOS_EVENT_MAX),
			THINKOS_EVENT_MAX);
#endif
#if THINKOS_ENABLE_FLAG_ALLOC
	dmprintf(comm, "%6d/%-2d", bmp_bit_cnt(rt->flag_alloc, THINKOS_FLAG_MAX),
			THINKOS_FLAG_MAX);
#endif
	dmprintf(comm, "\r\n");

}

struct thinkos_rt rt_buf;

int osinfo(struct dmon_comm * comm)
{
//	struct thinkos_rt * rt = &thinkos_rt;
	struct thinkos_rt * rt = &rt_buf;
	uint32_t * wq;
	int i;
#if THINKOS_ENABLE_PROFILING
	uint32_t cyccnt;
	int32_t delta;
	uint32_t cycdiv;
	uint32_t busy;
#endif
#if THINKOS_MUTEX_MAX > 0
	int j;
#endif

#if THINKOS_ENABLE_PROFILING
	cyccnt = CM3_DWT->cyccnt;
	delta = cyccnt - thinkos_rt.cycref;
	/* update the reference */
	thinkos_rt.cycref = cyccnt;
	/* update active thread's cycle counter */
	thinkos_rt.cyccnt[thinkos_rt.active] += delta; 
#endif

	memcpy(rt, &thinkos_rt, sizeof(struct thinkos_rt));

#if THINKOS_ENABLE_PROFILING
	/* Reset cycle counters */
	for (i = 0; i <= THINKOS_THREADS_MAX; ++i)
		thinkos_rt.cyccnt[i] = 0; 
#endif

	dmprintf(comm, "\r\n------------------------------------------------"
			 "---------------------\r\n");
#if THINKOS_ENABLE_CLOCK
	dmprintf(comm, "[ Ticks = %d ]", rt->ticks);
#endif

#if THINKOS_ENABLE_PROFILING
	{
		uint32_t cycsum = 0;
		uint32_t cycbusy;
		uint32_t idle;

		cycsum = 0;
		for (i = 0; i < THINKOS_THREADS_MAX; ++i)
			cycsum += rt->cyccnt[i];
		cycbusy = cycsum;
		cycsum += rt->cyccnt[THINKOS_CYCCNT_IDLE];

		cycdiv = (cycsum + 500) / 1000;
		busy = (cycbusy + cycdiv / 2) / cycdiv;
		idle = 1000 - busy;
		dmprintf(comm, " [ %u cycles | %d.%d%% busy | %d.%d%% idle ]", 
				cycsum, busy / 10, busy % 10, idle / 10, idle % 10);
	}

#endif

	dmprintf(comm, "\r\n");

	dmprintf(comm, " Th"); 
#if THINKOS_ENABLE_THREAD_INFO
	dmprintf(comm, " |     Tag"); 
	dmprintf(comm, " |    Stack"); 
#endif
	dmprintf(comm, " |  Context"); 
#if THINKOS_ENABLE_THREAD_STAT
	dmprintf(comm, " |  WQ | TmW"); 
#endif
#if THINKOS_ENABLE_TIMESHARE
	dmprintf(comm, " |  Val |  Pri"); 
#endif
#if THINKOS_ENABLE_CLOCK
	dmprintf(comm, " | Clock (ms)"); 
#endif
#if THINKOS_ENABLE_PROFILING
	dmprintf(comm, " | CPU %%"); 
#endif

#if THINKOS_MUTEX_MAX > 0
	dmprintf(comm, " | Locks\r\n"); 
#else
	dmprintf(comm, " |\r\n");
#endif

	for (i = 0; i < THINKOS_THREADS_MAX; ++i) {
		if (rt->ctx[i] != NULL) {
			dmprintf(comm, "%3d", i);
#if THINKOS_ENABLE_THREAD_INFO
			if (rt->th_inf[i] != NULL) {
				dmprintf(comm, " | %7s", rt->th_inf[i]->tag); 
				dmprintf(comm, " | %08x", (uint32_t)rt->th_inf[i]->stack_ptr); 
			} else {
				dmprintf(comm, " |     ..."); 
				dmprintf(comm, " |      ..."); 
			}
#endif
			dmprintf(comm, " | %08x", (uint32_t)rt->ctx[i]); 
#if THINKOS_ENABLE_THREAD_STAT
			dmprintf(comm, " | %3d | %s", rt->th_stat[i] >> 1, 
					rt->th_stat[i] & 1 ? "Yes" : " No"); 
#endif
#if THINKOS_ENABLE_TIMESHARE
			dmprintf(comm, " | %4d | %4d", rt->sched_val[i], rt->sched_pri[i]); 
#endif
#if THINKOS_ENABLE_CLOCK
			{
				int32_t dt = (int32_t)(rt->clock[i] - rt->ticks);
				if (dt < 0)
					dmprintf(comm, " | <timedout>"); 
				else
					dmprintf(comm, " | %10d", dt); 
			}
#endif
#if THINKOS_ENABLE_PROFILING
			busy = (rt->cyccnt[i] + cycdiv / 2) / cycdiv;
			dmprintf(comm, " | %3d.%d", busy / 10, busy % 10);
#endif
			dmprintf(comm, " |");
#if THINKOS_MUTEX_MAX > 0
			for (j = 0; j < THINKOS_MUTEX_MAX ; ++j) {
				if (rt->lock[j] == i)
					dmprintf(comm, " %d", j + THINKOS_MUTEX_BASE);
			}
#endif 
			dmprintf(comm, "\r\n");
		}
	}

	for (wq = rt->wq_lst; wq != rt->wq_end; ++wq) {
		int oid;
		int type;
		if (*wq) { 
			oid = wq - rt->wq_lst;
			type = thinkos_obj_type_get(oid);
			dmprintf(comm, "%3d %5s:", oid, thinkos_type_name_lut[type]);
			for (i = 0; i < THINKOS_THREADS_MAX; ++i) {
				if (*wq & (1 << i)) 
					dmprintf(comm, " %d", i);
			}
#if THINKOS_MUTEX_MAX > 0
			if (type == THINKOS_OBJ_MUTEX)
				dmprintf(comm, " [lock=%d]", rt->lock[oid - THINKOS_MUTEX_BASE]);
#endif 
			dmprintf(comm, "\r\n");
		}
	}

	os_alloc_dump(comm, rt);

	return 0;
}

void show_thread_info(struct dmon_comm * comm, int id)
{
	struct thinkos_thread st;
	struct thinkos_context * ctx;
	int type;

	if (__thinkos_thread_get(&thinkos_rt, &st, id) < 0) {
		dmprintf(comm, "Thread %d is invalid!\r\n", id);
		return;
	}

	type = thinkos_obj_type_get(st.wq);

	dmprintf(comm, " - Id: %d", id); 
	if (st.th_inf != NULL)
		dmprintf(comm, ", '%s'", st.th_inf->tag); 
	else
		dmprintf(comm, ", '%s'", "..."); 

	if (THINKOS_OBJ_READY == type) {
		dmprintf(comm, " %s.\r\n", thinkos_type_name_lut[type]); 
	} else {
		dmprintf(comm, " %swait on %s(%3d)\r\n", 
				 st.tmw ? "time" : "", thinkos_type_name_lut[type], st.wq ); 
	}

	dmprintf(comm, " - Scheduler: val=%3d pri=%3d - ", 
			 st.sched_val, st.sched_pri); 
	dmprintf(comm, " timeout=%8d ms", st.timeout); 
	dmprintf(comm, " - cycles=%u\r\n", st.cyccnt); 

	ctx = &st.ctx;

	dmprintf(comm, "     r0=%08x  r1=%08x  r2=%08x  r3=%08x\r\n", 
			 ctx->r0, ctx->r1, ctx->r2, ctx->r3);
	dmprintf(comm, "     r4=%08x  r5=%08x  r6=%08x  r7=%08x\r\n", 
			 ctx->r4, ctx->r7, ctx->r6, ctx->r7);
	dmprintf(comm, "     r8=%08x  r9=%08x r10=%08x r11=%08x\r\n", 
			 ctx->r8, ctx->r9, ctx->r10, ctx->r11);
	dmprintf(comm, "    r12=%08x  sp=%08x  lr=%08x  pc=%08x\r\n", 
			 ctx->r12, st.sp, ctx->lr, ctx->pc);
	dmprintf(comm, "   xpsr=%08x\r\n", ctx->xpsr);

	dmprintf(comm, "%08x:  ", ctx->pc & ~1);

	target_print_insn(comm, ctx->pc);

	dmprintf(comm, "\r\n");
}

struct mem_range {
	uint32_t base;
	uint32_t size;
};

static struct mem_range target_mem_map[] = {
	{ .base = 0x08000000, .size = 512 * 1024 },
	{ .base = 0x20000000, .size = 112 * 1024 },
	{ .base = 0x2001c000, .size = 16 * 1024 },
	{ .base = 0x10000000, .size = 64 * 1024 }
};
	/*
	flash (rx) : ORIGIN = 0x08000000, LENGTH = 512K
	sram0 (rw) : ORIGIN = 0x20000000, LENGTH = 112K
	sram1 (rw) : ORIGIN = 0x2001c000, LENGTH = 16K
	sram2 (rw) : ORIGIN = 0x10000000, LENGTH = 64K
	*/

static bool is_addr_valid(uint32_t addr) 
{
	int i;
	int n = sizeof(target_mem_map) / sizeof(struct mem_range);

	for (i = 0; i < n; ++i) {
		struct mem_range * mem = &target_mem_map[i];
		if ((addr >= mem->base) && (addr < (mem->base + mem->size))) {
			return true;
		}
	}
	return false;
}

static int read_memory(bfd_vma addr, uint8_t * buf, unsigned int len, 
				struct disassemble_info *dinfo)
{
	uint8_t * dst = (uint8_t *)buf;
	uint8_t * src = (uint8_t *)addr;;
	int i;
	
	DCC_LOG2(LOG_INFO, "addr=0x%08x len=%d", addr, len);

	if (len == 0)
		return 0;

	if (!is_addr_valid(addr))
		return -1;

	for (i = 0; i < len; ++i)
		dst[i] = src[i];

	return 0;
}

static void memory_error(int status, bfd_vma addr, 
						 struct disassemble_info *dinfo)
{
	dinfo->fprintf_func(dinfo->stream, "ERROR: Memory Read @ %08x\n", addr);
}

static void print_address(bfd_vma addr, struct disassemble_info *dinfo)
{
	dinfo->fprintf_func(dinfo->stream, "%08x", addr);
}

static int symbol_at_address(bfd_vma addr, struct disassemble_info *dinfo)
{
	return 0;
}

int target_print_insn(struct dmon_comm * comm, uint32_t addr)
{
	struct disassemble_info dinfo;

	memset(&dinfo, 0, sizeof(struct disassemble_info));
	dinfo.fprintf_func = (fprintf_ftype)dmprintf;
	dinfo.stream = comm;
	dinfo.read_memory_func = read_memory;
	dinfo.memory_error_func = memory_error;
	dinfo.print_address_func = print_address;
	dinfo.symbol_at_address_func = symbol_at_address;
	dinfo.endian_code = BFD_ENDIAN_LITTLE;
	dinfo.mach = bfd_mach_arm_unknown;
	dinfo.flags = USER_SPECIFIED_MACHINE_TYPE;
	dinfo.disassembler_options = "force-thumb";
	dinfo.application_data = NULL;

	return print_insn_little_arm(addr, &dinfo);
}
