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

#define __THINKOS_DMON__
#include <thinkos_dmon.h>
#include <thinkos.h>
#include <sys/dcclog.h>

#include <gdb.h>

#ifndef MONITOR_CONFIGURE_ENABLE
#define MONITOR_CONFIGURE_ENABLE  1
#endif

#ifndef MONITOR_DUMPMEM_ENABLE
#define MONITOR_DUMPMEM_ENABLE    1
#endif

#ifndef MONITOR_UPGRADE_ENABLE
#define MONITOR_UPGRADE_ENABLE    1
#endif

#ifndef MONITOR_STACKUSAGE_ENABLE
#define MONITOR_STACKUSAGE_ENABLE 1
#endif

#ifndef MONITOR_THREADINFO_ENABLE
#define MONITOR_THREADINFO_ENABLE 1
#endif

#ifndef MONITOR_APPWIPE_ENABLE
#define MONITOR_APPWIPE_ENABLE    1
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

#if (MONITOR_THREADINFO_ENABLE)
#define MONITOR_STARTUP_MAGIC -111
int8_t monitor_thread_id = MONITOR_STARTUP_MAGIC;
#endif

static const char monitor_menu[] = 
"- ThinkOS Monitor Commands:\r\n"
" Ctrl+C - Stop app\r\n"
#if (MONITOR_CONFIGURE_ENABLE)
" Ctrl+K - Configure Board\r\n"
#endif
#if (MONITOR_UPGRADE_ENABLE)
" Ctrl+L - Upload ThinkOS\r\n"
#endif
#if (MONITOR_THREADINFO_ENABLE)
" Ctrl+N - Select Next Thread\r\n"
" Ctrl+O - ThinkOS info\r\n"
" Ctrl+P - Pause all threads\r\n"
" Ctrl+Q - Restart monitor\r\n"
" Ctrl+R - Resume all threads\r\n"
#endif
#if (MONITOR_DUMPMEM_ENABLE)
" Ctrl+S - Show memory\r\n"
#endif
#if (MONITOR_THREADINFO_ENABLE)
" Ctrl+T - Thread info\r\n"
#endif
#if (MONITOR_STACKUSAGE_ENABLE)
" Ctrl+U - Stack usage info\r\n"
#endif
" Ctrl+V - Help\r\n"
#if (MONITOR_APPWIPE_ENABLE)
" Ctrl+W - Wipe application\r\n"
#endif
#if (THINKOS_ENABLE_EXCEPTIONS)
" Ctrl+X - Exception info\r\n"
#endif
" Ctrl+Y - YMODEM app upload\r\n"
" Ctrl+Z - Restart app\r\n";

static const char __hr__[] = 
"--------------------------------------------------------------\r\n";

static void monitor_show_help(struct dmon_comm * comm)
{
	dmon_comm_send(comm, __hr__, sizeof(__hr__) - 1);
	dmon_comm_send(comm, monitor_menu, sizeof(monitor_menu) - 1);
	dmon_comm_send(comm, __hr__, sizeof(__hr__) - 1);
}

#if (THINKOS_ENABLE_EXCEPTIONS)
static void monitor_print_fault(struct dmon_comm * comm)
{
	struct thinkos_except * xcpt = &thinkos_except_buf;

	switch (xcpt->type == 0) {
		dmprintf(comm, "No fault!");
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
		dmprintf(comm, __hr__);
		dmon_print_exception(comm, xcpt);
		dmprintf(comm, __hr__);
	}
}
#endif

#if (MONITOR_THREADINFO_ENABLE)
static void monitor_pause_all(struct dmon_comm * comm)
{
	dmprintf(comm, "\r\nPausing all threads...\r\n");
	DCC_LOG(LOG_WARNING, "__thinkos_pause_all()");
	__thinkos_pause_all();
	if (dmon_wait_idle() < 0) {
		DCC_LOG(LOG_WARNING, "dmon_wait_idle() failed!");
	}
}

static void monitor_resume_all(struct dmon_comm * comm)
{
	dmprintf(comm, "\r\nResuming all threads...\r\n");
	__thinkos_resume_all();
	dmprintf(comm, "Restarting...\r\n");
}
#endif

static const char ymodem_rcv_msg[] = 
"\r\nYMODEM receive (^X to cancel) ... ";
static const char ymodem_err_msg[] = 
"\r\n#ERROR: YMODEM failed!\r\n";
static const char app_invalid_msg[] = 
"\r\n#ERROR: Invalid app!\r\n";

static void monitor_ymodem_recv(struct dmon_comm * comm, 
								uint32_t addr, unsigned int size)
{
	dmon_comm_send(comm, ymodem_rcv_msg, sizeof(ymodem_rcv_msg) - 1);
	dmon_soft_reset(comm);
	if (dmon_ymodem_flash(comm, addr, size) < 0) {
		dmon_comm_send(comm, ymodem_err_msg, sizeof(ymodem_err_msg) - 1);
		return;
	}	

	if (dmon_app_exec(addr, false) < 0) {
		dmon_comm_send(comm, app_invalid_msg, sizeof(app_invalid_msg) - 1);
		return;
	}
}

#if (MONITOR_APPWIPE_ENABLE)
static void monitor_app_erase(struct dmon_comm * comm, 
							  uint32_t addr, unsigned int size)
{
	dmprintf(comm, "\r\nErasing application block ... ");
	dmon_soft_reset(comm);
	if (dmon_app_erase(comm, addr, size))
		dmprintf(comm, "done.\r\n");
	else	
		dmprintf(comm, "failed!\r\n");
}
#endif

#if (MONITOR_DUMPMEM_ENABLE)
int long2hex_le(char * s, unsigned long val);
int char2hex(char * s, int c);

static void monitor_dump_mem(struct dmon_comm * comm, 
							  uint32_t addr, unsigned int size)
{
	char buf[14 + 16 * 3];
	unsigned int rem = size;
	uint8_t * cmp = (uint8_t *)-1;
	bool eq = false;

	while (rem) {
		int n = rem < 16 ? rem : 16;
		uint8_t * src = (uint8_t *)addr;
		char * cp = buf;
		unsigned int i;
	
		if (cmp != (uint8_t *)-1) {
			for (i = 0; i < n; ++i) {
				if (src[i] != cmp[i]) {
					eq = false;
					goto dump_line;
				}
			}

			if (!eq) {
				dmon_comm_send(comm, " ...\r\n", 6);
				eq = true;
			}
		} else {	

dump_line:
			cp += long2hex_le(cp, addr);
			*cp++ = ':';

			for (i = 0; i < n; ++i) {
				*cp++ = ' ';
				cp += char2hex(cp, src[i]);
			}

			*cp++ = '\r';
			*cp++ = '\n';

			dmon_comm_send(comm, buf, cp - buf);
		}

		addr += n;
		rem -= n;
		cmp = src;
	}
}
#endif

static void monitor_exec(struct dmon_comm * comm, unsigned int addr)
{
	if (dmon_app_exec(addr, false) < 0) {
		dmon_comm_send(comm, app_invalid_msg, sizeof(app_invalid_msg) - 1);
		return;
	}
}

//void gdb_task(struct dmon_comm *) __attribute__((weak, alias("monitor_task")));
void monitor_task(struct dmon_comm *);

int monitor_process_input(struct dmon_comm * comm, char * buf, int len)
{
	int i;
	int j;
	int c;

	for (i = 0; i < len; ++i) {
		c = buf[i];
		switch (c) {
		case CTRL_C:
			dmon_comm_send(comm, "^C\r\n", 4);
			dmon_soft_reset(comm);
			break;
#if (BOOT_ENABLE_GDB)
		case '+':
			dmon_exec(gdb_task);
			break;
#endif
#if (MONITOR_CONFIGURE_ENABLE)
		case CTRL_K:
			dmprintf(comm, "^K\r\n");
			dmon_soft_reset(comm);
			this_board.configure(comm);
			break;
#endif
#if (MONITOR_UPGRADE_ENABLE)
		case CTRL_L:
			dmon_soft_reset(comm);
			dmprintf(comm, "^L\r\n");
			dmprintf(comm, "Confirm (yes/no)? ");
			dmscanf(comm, "yes%n", &i);
			if (i == 3)
				this_board.upgrade(comm);
			break;
#endif
#if (MONITOR_THREADINFO_ENABLE)
		case CTRL_N:
			monitor_thread_id = __thinkos_thread_getnext(monitor_thread_id);
			if (monitor_thread_id == - 1)
				monitor_thread_id = __thinkos_thread_getnext(monitor_thread_id);
			dmprintf(comm, "Thread = %d\r\n", monitor_thread_id);
			dmon_print_thread(comm, monitor_thread_id);
			break;
		case CTRL_O:
			dmon_print_osinfo(comm);
			break;
		case CTRL_P:
			dmprintf(comm, "^P\r\n");
			monitor_pause_all(comm);
			break;
		case CTRL_Q:
			dmprintf(comm, "^Q\r\n");
			dmon_exec(monitor_task);
			break;
		case CTRL_R:
			dmprintf(comm, "^R\r\n");
			monitor_resume_all(comm);
			break;
#endif
#if (MONITOR_DUMPMEM_ENABLE)
		case CTRL_S:
			dmprintf(comm, "^S\r\n");
			monitor_dump_mem(comm, this_board.application.start_addr, 
							 this_board.application.block_size);
			break;
#endif
#if (MONITOR_THREADINFO_ENABLE)
		case CTRL_T:
			dmon_print_thread(comm, monitor_thread_id);
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
#if (THINKOS_ENABLE_EXCEPTIONS)
		case CTRL_X:
			monitor_print_fault(comm);
			break;
#endif
		case CTRL_Y:
			dmon_comm_send(comm, "^Y\r\n", 4);
			monitor_ymodem_recv(comm, this_board.application.start_addr, 
								this_board.application.block_size);
			break;
#if (MONITOR_APPWIPE_ENABLE)
		case CTRL_W:
			dmprintf(comm, "^W\r\n");
			monitor_app_erase(comm, this_board.application.start_addr, 
							  this_board.application.block_size);
			break;
#endif
		case CTRL_Z:
			dmon_comm_send(comm, "^Z\r\n", 4);
			dmon_soft_reset(comm);
			monitor_exec(comm, this_board.application.start_addr);
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

/*
   Dafault Monitor Task
 */

void __attribute__((noreturn)) monitor_task(struct dmon_comm * comm)
{
	uint32_t sigmask = 0;
	uint32_t sigset;
#if THINKOS_ENABLE_CONSOLE
	bool connected;
	uint8_t * ptr;
	int cnt;
#endif
	int tick_cnt = 0;
	char buf[64];
	int len;

//	DCC_LOG(LOG_TRACE, "Monitor start...");
//	dmon_comm_connect(comm);
//	DCC_LOG(LOG_TRACE, "Comm connected.");

//	dmon_sleep(100);

#if 0
	dmprintf(comm, "\r\n\r\n");
	dmprintf(comm, __hr__);
	dmprintf(comm, " ThikOS Debug Monitor (Ctrl+V for Help)\r\n");
	dmprintf(comm, __hr__);
#endif

#if (THINKOS_ENABLE_EXCEPTIONS)
	sigmask |= (1 << DMON_THREAD_FAULT);
	sigmask |= (1 << DMON_EXCEPT);
#endif
	sigmask |= (1 << DMON_COMM_RCV);
#if THINKOS_ENABLE_CONSOLE
	sigmask |= (1 << DMON_COMM_CTL);
	sigmask |= (1 << DMON_TX_PIPE);
	sigmask |= (1 << DMON_RX_PIPE);
#endif

#if (MONITOR_THREADINFO_ENABLE)
	if (monitor_thread_id == MONITOR_STARTUP_MAGIC) {
#endif
		/* first time we run the monitor, start a timer to call the 
		   board_tick() periodically */
		sigmask |= (1 << DMON_ALARM);
		dmon_alarm(125);
#if (MONITOR_THREADINFO_ENABLE)
		monitor_thread_id = -1;
	}
#endif

#if THINKOS_ENABLE_CONSOLE
	connected = dmon_comm_isconnected(comm);
#endif

	for(;;) {
		sigset = dmon_select(sigmask);
		DCC_LOG1(LOG_INFO, "sigset=%08x", sigset);

#if THINKOS_ENABLE_CONSOLE
		if (sigset & (1 << DMON_COMM_CTL)) {
			DCC_LOG(LOG_INFO, "Comm Ctl.");
			dmon_clear(DMON_COMM_CTL);
			connected = dmon_comm_isconnected(comm);
		}
#endif

		if (sigset & (1 << DMON_COMM_RCV)) {
#if THINKOS_ENABLE_CONSOLE
			if ((cnt = __console_rx_pipe_ptr(&ptr)) > 0) {
				DCC_LOG1(LOG_INFO, "Comm recv. rx_pipe.free=%d", cnt);
				if ((len = dmon_comm_recv(comm, ptr, cnt)) > 0) {
					len = monitor_process_input(comm, (char *)ptr, len);
					__console_rx_pipe_commit(len); 
				} else {
					DCC_LOG(LOG_WARNING, "dmon_comm_recv() failed, "
							"masking DMON_COMM_RCV!");
					sigmask &= ~(1 << DMON_COMM_RCV);
				}
			} else {
				DCC_LOG(LOG_TRACE, "Comm recv. rx pipe full!");
				if ((len = dmon_comm_recv(comm, buf, sizeof(buf))) > 0) {
					monitor_process_input(comm, buf, len);
				} else {
					DCC_LOG(LOG_WARNING, "dmon_comm_recv() failed, "
							"masking DMON_COMM_RCV!");
					sigmask &= ~(1 << DMON_COMM_RCV);
				}
			}
#else
			if ((len = dmon_comm_recv(comm, buf, sizeof(buf))) > 0) {
				monitor_process_input(comm, buf, len);
			}
#endif
		}

#if THINKOS_ENABLE_CONSOLE
		if (sigset & (1 << DMON_RX_PIPE)) {
			if ((cnt = __console_rx_pipe_ptr(&ptr)) > 0) {
				DCC_LOG1(LOG_TRACE, "RX Pipe. rx_pipe.free=%d. "
						 "Unmaksing DMON_COMM_RCV!", cnt);
				sigmask |= (1 << DMON_COMM_RCV);
			} else {
				DCC_LOG(LOG_TRACE, "RX Pipe empty!!!");
			}
			dmon_clear(DMON_RX_PIPE);
		}


		if (sigset & (1 << DMON_TX_PIPE)) {
			DCC_LOG(LOG_INFO, "TX Pipe.");
			if ((cnt = __console_tx_pipe_ptr(&ptr)) > 0) {
				DCC_LOG1(LOG_INFO, "TX Pipe, %d pending chars.", cnt);
				if (connected) 
					len = dmon_comm_send(comm, ptr, cnt);
				else
					len = cnt;
				__console_tx_pipe_commit(len); 
			} else {
				DCC_LOG(LOG_INFO, "TX Pipe empty!!!");
				dmon_clear(DMON_TX_PIPE);
			}
		}
#endif

		if (sigset & (1 << DMON_ALARM)) {
			dmon_clear(DMON_ALARM);
			if (this_board.autoboot(tick_cnt++) && 
				dmon_app_exec(this_board.application.start_addr, false)) {
				sigmask &= ~(1 << DMON_ALARM);
				this_board.on_appload();
			} else {
				/* reastart the alarm timer */
				dmon_alarm(125);
			}  
		}

#if (THINKOS_ENABLE_EXCEPTIONS)
		if (sigset & (1 << DMON_THREAD_FAULT)) {
			DCC_LOG(LOG_TRACE, "Thread fault.");
			monitor_on_fault(comm);
			dmon_clear(DMON_THREAD_FAULT);
		}

		if (sigset & (1 << DMON_EXCEPT)) {
			DCC_LOG(LOG_TRACE, "System exception.");
			monitor_on_fault(comm);
			dmon_clear(DMON_EXCEPT);
		}
#endif
	}
}

