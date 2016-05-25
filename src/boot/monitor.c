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

#ifdef CONFIG_H
#include "config.h"
#endif

#include <sys/stm32f.h>
#include <arch/cortex-m3.h>
#include <sys/param.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#define __THINKOS_DBGMON__
#include <thinkos_dmon.h>
#include <thinkos.h>
#include <sys/dcclog.h>
#include "version.h"

#ifndef MONITOR_UPGRADE_ENABLE
#define MONITOR_UPGRADE_ENABLE     1
#endif

#ifndef MONITOR_APPWIPE_ENABLE
#define MONITOR_APPWIPE_ENABLE     1
#endif

#ifndef MONITOR_APPTERM_ENABLE
#define MONITOR_APPTERM_ENABLE     0
#endif

#ifndef MONITOR_APPRESTART_ENABLE
#define MONITOR_APPRESTART_ENABLE  1
#endif

#ifndef MONITOR_FAULT_ENABLE
#define MONITOR_FAULT_ENABLE       0
#endif

#define MONITOR_OSINFO_ENABLE      0

#ifndef MONITOR_STACKUSAGE_ENABLE
#define MONITOR_STACKUSAGE_ENABLE  0
#endif


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

static const char monitor_menu[] = 
"ThinkOS " VERSION_NUM "\r\n"
#if (MONITOR_APPTERM_ENABLE)
" ^C - Stop app\r\n"
#endif
#if (MONITOR_UPGRADE_ENABLE)
" ^L - Upload ThinkOS\r\n"
#endif
#if (MONITOR_OSINFO_ENABLE)
" ^O - OS Info\r\n"
#endif
#if (MONITOR_STACKUSAGE_ENABLE)
" Ctrl+U - Stack usage info\r\n"
#endif
" ^V - Help\r\n"
#if (MONITOR_APPWIPE_ENABLE)
" ^W - Wipe App\r\n"
#endif
#if (MONITOR_FAULT_ENABLE)
" Ctrl+X - Exception info\r\n"
#endif
" ^Y - Upload App\r\n"
#if (MONITOR_APPRESTART_ENABLE)
" ^Z - Restart\r\n"
#endif
;

static const char __hr__[] = 
"----\r\n";

#if (MONITOR_OSINFO_ENABLE)
#define PUTS(S) dmprintf(comm, S) 
#else
#define PUTS(S) dmputs(S, comm) 
#endif

static void monitor_show_help(struct dmon_comm * comm)
{
	PUTS(__hr__);
	PUTS(monitor_menu);
}

extern int __heap_end;
const void * heap_end = &__heap_end; 
extern uint32_t _stack;
extern const struct thinkos_thread_inf thinkos_main_inf;

static void board_exec(void (* func)(int), int mode)
{
	int thread_id = 0;

	DCC_LOG(LOG_TRACE, "__thinkos_thread_abort()");
	__thinkos_thread_abort(thread_id);

	DCC_LOG1(LOG_TRACE, "__thinkos_thread_init(mode=%d)", mode);
	__thinkos_thread_init(thread_id, (uintptr_t)&_stack, func, (void *)mode);

#if THINKOS_ENABLE_THREAD_INFO
	__thinkos_thread_inf_set(thread_id, &thinkos_main_inf);
#endif

	DCC_LOG(LOG_TRACE, "__thinkos_thread_resume()");
	__thinkos_thread_resume(thread_id);

	DCC_LOG(LOG_TRACE, "__thinkos_defer_sched()");
	__thinkos_defer_sched();
}

bool monitor_app_exec(uint32_t addr, int mode)
{
	uint32_t * signature = (uint32_t *)addr;

	if ((signature[0] != 0x0a0de004) ||
		(signature[1] != 0x6e696854) ||
		(signature[2] != 0x00534f6b)) {
		DCC_LOG1(LOG_WARNING, "invalid application signature, addr=%p!", 
				 signature);

		return false;
	}

	board_exec((void *)(addr | 1), mode);

	return true;
}

#if (MONITOR_FAULT_ENABLE)
static void monitor_print_fault(struct dmon_comm * comm)
{
	struct thinkos_except * xcpt = &thinkos_except_buf;

	switch (xcpt->type == 0) {
		PUTS("No fault.\r\n");
		return;
	}

	dmon_print_exception(comm, xcpt);
}

static void monitor_on_fault(struct dmon_comm * comm)
{
	struct thinkos_except * xcpt = &thinkos_except_buf;

	if (dmon_wait_idle() < 0) {
		DCC_LOG(LOG_WARNING, "dmon_wait_idle() failed!");
	}
	if (dmon_comm_isconnected(comm)) {
		PUTS(__hr__);
		dmon_print_exception(comm, xcpt);
	}
}
#endif

static const char s_error[] = "Error!\r\n";

int board_yflash(uint32_t addr, unsigned int size);

static void monitor_ymodem_recv(struct dmon_comm * comm, 
								uint32_t addr, unsigned int size)
{
	dbgmon_soft_reset();

//	PUTS("\r\nYmodem...");
//	if (dmon_ymodem_flash(comm, addr, size) < 0) {
	if (board_yflash(addr, size) < 0) {
		PUTS(s_error);
		return;
	}	

	if (monitor_app_exec(addr, 0) < 0) {
		PUTS(s_error);
		return;
	}
}


#if (MONITOR_APPWIPE_ENABLE)
static void monitor_app_erase(struct dmon_comm * comm, 
							  uint32_t addr, unsigned int size)
{
	PUTS("\r\nErasing...");
	dbgmon_soft_reset();
	if (dmon_app_erase(comm, addr, size))
		PUTS("Ok.\r\n");
	else	
		PUTS(s_error);
}
#endif

#if (MONITOR_OSINFO_ENABLE)

#if THINKOS_ENABLE_THREAD_VOID
#define CYCCNT_MAX ((THINKOS_THREADS_MAX) + 2) /* extra slot for void thread */
#else
#define CYCCNT_MAX ((THINKOS_THREADS_MAX) + 1)
#endif

void monitor_print_osinfo(struct dmon_comm * comm)
{
	struct thinkos_rt * rt = &thinkos_rt;
#if THINKOS_ENABLE_PROFILING
	uint32_t cycbuf[CYCCNT_MAX];
	uint32_t cyccnt;
	int32_t delta;
	uint32_t cycdiv;
	uint32_t busy;
	uint32_t cycsum = 0;
	uint32_t cycbusy;
	uint32_t idle;
#endif
	const char * tag;
	int i;

#if THINKOS_ENABLE_PROFILING
	cyccnt = CM3_DWT->cyccnt;
	delta = cyccnt - thinkos_rt.cycref;
	/* update the reference */
	thinkos_rt.cycref = cyccnt;
	/* update active thread's cycle counter */
	thinkos_rt.cyccnt[thinkos_rt.active] += delta; 
	/* copy the thread counters to a buffer */
	__thinkos_memcpy32(cycbuf, rt->cyccnt, sizeof(cycbuf));
	/* reset cycle counters */
	__thinkos_memset32(rt->cyccnt, 0, sizeof(cycbuf));
#endif

	dmprintf(comm, __hr__);
#if THINKOS_ENABLE_PROFILING
	cycsum = 0;
	for (i = 0; i < THINKOS_THREADS_MAX; ++i)
		cycsum += cycbuf[i];
	cycbusy = cycsum;
	cycsum += cycbuf[THINKOS_CYCCNT_IDLE];

	cycdiv = (cycsum + 500) / 1000;
	busy = (cycbusy + cycdiv / 2) / cycdiv;
	idle = 1000 - busy;
	dmprintf(comm, "CPU: %d.%d%% busy, %d.%d%% idle\r\n", 
			 busy / 10, busy % 10, idle / 10, idle % 10);
#endif

	dmprintf(comm, " Th     Tag       SP       LR       PC  WQ TmW CPU %%\r\n");

	for (i = 0; i < THINKOS_THREADS_MAX; ++i) {
		if (rt->ctx[i] != NULL) {
			/* Internal thread ids start form 0 whereas user
			   thread numbers start form one ... */
			tag = (rt->th_inf[i] != NULL) ? rt->th_inf[i]->tag : "...";
			busy = (cycbuf[i] + cycdiv / 2) / cycdiv;
			dmprintf(comm, "%3d %7s %08x %08x %08x %3d %s %3d.%d\r\n",
					 i + 1, tag,
					 (uint32_t)rt->ctx[i], rt->ctx[i]->lr, rt->ctx[i]->pc, 
					 rt->th_stat[i] >> 1, rt->th_stat[i] & 1 ? "Yes" : " No",
					 busy / 10, busy % 10);
		}
	}
}

#endif /* MONITOR_OSINFO_ENABLE */

#if (MONITOR_APPRESTART_ENABLE)
static void monitor_exec(struct dmon_comm * comm, unsigned int addr)
{
	if (!dmon_app_exec(addr, false)) {
		PUTS(s_error);
		return;
	}
}
#endif

void monitor_task(struct dmon_comm *);

static bool monitor_process_input(struct dmon_comm * comm, int c)
{
	switch (c) {
#if (MONITOR_APPTERM_ENABLE)
	case CTRL_C:
		PUTS("^C\r\n");
		dbgmon_soft_reset();
		break;
#endif
#if (MONITOR_UPGRADE_ENABLE)
	case CTRL_L:
		dbgmon_soft_reset();
		this_board.upgrade(comm);
		break;
#endif
#if (MONITOR_OSINFO_ENABLE)
	case CTRL_O:
		monitor_print_osinfo(comm);
		break;
#endif
#if (MONITOR_STACKUSAGE_ENABLE)
	case CTRL_U:
		dmon_print_stack_usage(comm);
		break;
#endif
	case CTRL_V:
		monitor_show_help(comm);
		break;
#if (MONITOR_FAULT_ENABLE)
	case CTRL_X:
		monitor_print_fault(comm);
		break;
#endif
	case CTRL_Y:
		monitor_ymodem_recv(comm, this_board.application.start_addr, 
							this_board.application.block_size);
		break;
#if (MONITOR_APPWIPE_ENABLE)
	case CTRL_W:
		monitor_app_erase(comm, this_board.application.start_addr, 
						  this_board.application.block_size);
		break;
#endif
#if (MONITOR_APPRESTART_ENABLE)
	case CTRL_Z:
		PUTS("^Z\r\n");
		DCC_LOG(LOG_TRACE, "dbgmon_soft_reset()...");
		dbgmon_soft_reset();
		DCC_LOG2(LOG_TRACE, "dbgmon_wait_idle()... active=%d ready=0x%08x",
				 thinkos_rt.active, thinkos_rt.wq_ready);
		DCC_LOG3(LOG_TRACE, "msp=%08x psp=%08x ctrl=%02x", 
				 cm3_msp_get(), cm3_psp_get(), cm3_control_get());
		dbgmon_wait_idle();
		DCC_LOG(LOG_TRACE, "monitor_exec()...");
		monitor_exec(comm, this_board.application.start_addr);
		break;
#endif
	default:
		return false;
	}

	return true;
}

/*
   Dafault Monitor Task
 */

void __attribute__((noreturn)) monitor_task(struct dmon_comm * comm)
{
	uint32_t sigmask;
	uint32_t sigset;
	uint8_t buf[1];
	uint8_t * ptr;
	int cnt;

	dbgmon_wait_idle();

	if (this_board.autoboot(0)) {
		DCC_LOG(LOG_TRACE, "Autoboot...");
		monitor_app_exec(this_board.application.start_addr, 0);
	} else {
		DCC_LOG(LOG_WARNING, "No autoboot!!");
	}

	sigmask = 0;
#if (MONITOR_FAULT_ENABLE)
	sigmask |= (1 << DBGMON_THREAD_FAULT);
	sigmask |= (1 << DBGMON_EXCEPT);
#endif
	sigmask |= (1 << DBGMON_COMM_RCV);
	sigmask |= (1 << DBGMON_TX_PIPE);
#if DEBUG
	sigmask |= (1 << DBGMON_COMM_CTL);
#endif

	for(;;) {
		sigset = dbgmon_select(sigmask);
		DCC_LOG1(LOG_MSG, "sigset=%08x", sigset);

		if (sigset & (1 << DBGMON_TX_PIPE)) {
			DCC_LOG(LOG_MSG, "TX Pipe.");
			if ((cnt = __console_tx_pipe_ptr(&ptr)) > 0) {
				DCC_LOG1(LOG_INFO, "TX Pipe, %d pending chars.", cnt);
				cnt = dmon_comm_send(comm, ptr, cnt);
				__console_tx_pipe_commit(cnt); 
			} else {
				DCC_LOG(LOG_INFO, "TX Pipe empty!!!");
				dbgmon_clear(DBGMON_TX_PIPE);
			}
		}

		if (sigset & (1 << DBGMON_COMM_RCV)) {
			/* receive from the COMM driver one bye at the time */
			if (dmon_comm_recv(comm, buf, 1) > 0) {
				int c = buf[0];
				/* process the input character */
				if (!monitor_process_input(comm, c)) {
					/* if the character was not consumed by the monitor 
					   insert into the console pipe */
					/* get a pointer to the head of the pipe.
					 __console_rx_pipe_ptr() will return the number of 
					 consecutive spaces in the buffer. We need only one. */
					if (__console_rx_pipe_ptr(&ptr) > 0) {
						/* copy the character into the RX fifo */
						ptr[0] = c;
						/* commit the fifo head */
						__console_rx_pipe_commit(1);
					}
				}
			}
		}

#if DEBUG
		if (sigset & (1 << DBGMON_COMM_CTL)) {
			DCC_LOG1(LOG_TRACE, "Comm Ctl, sigset=%08x", sigset);
			dbgmon_clear(DBGMON_COMM_CTL);
		}
#endif

#if (MONITOR_FAULT_ENABLE)
		if (sigset & (1 << DBGMON_THREAD_FAULT)) {
			DCC_LOG(LOG_TRACE, "Thread fault.");
			monitor_on_fault(comm);
			dbgmon_clear(DBGMON_THREAD_FAULT);
		}

		if (sigset & (1 << DBGMON_EXCEPT)) {
			DCC_LOG(LOG_TRACE, "System exception.");
			monitor_on_fault(comm);
			dbgmon_clear(DBGMON_EXCEPT);
		}
#endif
	}
}

