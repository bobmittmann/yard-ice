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
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#define __THINKOS_DMON__
#include <thinkos_dmon.h>
#include <thinkos.h>
#include <sys/dcclog.h>

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

extern int thread_id;

static const char monitor_menu[] = "\r\n"
"-- ThinkOS Monitor Commands ---------\r\n"
" Ctrl+Q - Restart monitor\r\n"
" Ctrl+R - Resume all threads\r\n"
" Ctrl+P - Pause all threads\r\n"
" Ctrl+X - Show exception\r\n"
" Ctrl+S - Thread Step\r\n"
" Ctrl+I - ThinkOS info\r\n"
" Ctrl+N - Select Next Thread\r\n"
"-------------------------------------\r\n\r\n";

static void monitor_show_help(struct dmon_comm * comm)
{
	dmon_comm_send(comm, monitor_menu, sizeof(monitor_menu) - 1);
}

static void monitor_on_fault(struct dmon_comm * comm)
{
	const struct thinkos_except * xcpt = &thinkos_except_buf;

	DCC_LOG(LOG_TRACE, "DMON_THREAD_FAULT.");
	dmprintf(comm, "Fault at thread %d\r\n", xcpt->thread);
//	dmon_print_context(comm, &xcpt->ctx, xcpt->sp);
}

static void monitor_pause_all(struct dmon_comm * comm)
{
	dmprintf(comm, "\r\nPausing all threads...\r\n");
	DCC_LOG(LOG_WARNING, "__thinkos_pause_all()");
	__thinkos_pause_all();
	if (dmon_wait_idle() < 0) {
		DCC_LOG(LOG_WARNING, "dmon_wait_idle() failed!");
	}
	dmprintf(comm, "[Idle]\r\n");
}

static void monitor_resume_all(struct dmon_comm * comm)
{
	dmprintf(comm, "\r\nResuming all threads...\r\n");
	__thinkos_resume_all();
	dmprintf(comm, "Restarting...\r\n");
}

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
		case CTRL_I:
			dmon_print_osinfo(comm);
			break;
		case CTRL_N:
			thread_id = __thinkos_thread_getnext(thread_id);
			if (thread_id == - 1)
				thread_id = __thinkos_thread_getnext(thread_id);
			dmprintf(comm, "Current thread = %d\r\n", thread_id);
			dmon_print_thread(comm, thread_id);
			break;
		case CTRL_X:
			monitor_on_fault(comm);
			break;
		case CTRL_D:
			dmon_print_thread(comm, thread_id);
			break;
		case CTRL_Y:
			dmon_print_stack_usage(comm);
			break;
		case CTRL_C:
			dmon_soft_reset(comm);
			break;
		case CTRL_H:
			monitor_show_help(comm);
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

	dmon_comm_connect(comm);

	thread_id = __thinkos_thread_getnext(-1);

	sigmask = (1 << DMON_THREAD_FAULT);
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

		if (sigset & (1 << DMON_COMM_CTL)) {
			DCC_LOG(LOG_TRACE, "Comm Ctl.");
			dmon_clear(DMON_COMM_CTL);
			if (!dmon_comm_isconnected(comm))	
				dmon_reset();
		}

		if (sigset & (1 << DMON_COMM_RCV)) {
			if ((cnt = __console_rx_pipe_ptr(&ptr)) > 0) {
				DCC_LOG1(LOG_TRACE, "Comm recv. rx_pipe.free=%d", cnt);
				if ((len = dmon_comm_recv(comm, ptr, cnt)) > 0) {
					len = process_input(comm, (char *)ptr, len);
					__console_rx_pipe_commit(len); 
				}
			} else {
				DCC_LOG(LOG_TRACE, "Comm recv. Masking DMON_COMM_RCV!");
				sigmask &= ~(1 << DMON_COMM_RCV);
			}
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
	}
}

