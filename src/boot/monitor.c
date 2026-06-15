/* 
 * File:	 monitor.c
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

/** 
 * @file monitor.c
 * @brief YARD-ICE debug monitor
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include "monitor-i.h"
#include "version.h"

#include <sys/stm32f.h>
#include <arch/cortex-m3.h>
#include <sys/param.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <ascii.h>

#define __THINKOS_BOOTLDR__
#include <thinkos/bootldr.h>

#include <sys/dcclog.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
const struct magic_blk board_app_magic = {
	.hdr = {
		.pos = 0,
		.cnt = 3
	},
	.rec = {
		{ 0xffffffff, 0x08020041 },
		{ 0xffffffff, 0x20020000 },
		{ 0xffffffff, 0x00004000 },
/*		{ 0xfff00000, 0x000fffff },
		{ 0xffffffff, 0x6e696854 },
        { 0xffffffff, 0x00534f6b },
        { 0xffffffff, 0x64726159 },
        { 0xffffffff, 0x00656349 } */
	}
};

const struct magic_blk bootloader_magic = {
	.hdr = {
		.pos = 0,
		.cnt = 2
	},
	.rec = {
		{  0xfffc0000, 0x20000000 },
		{  0xffff0000, 0x08000000 },
	}
};
#pragma GCC diagnostic pop

extern const uint8_t otg_xflash_pic[];
extern const unsigned int sizeof_otg_xflash_pic;

void board_reset(void);

#if (THINKOS_ENABLE_ERROR_TRAP)
#define MONITOR_FAULT_ENABLE 1
#else
#define MONITOR_FAULT_ENABLE 0
#endif

#ifndef MONITOR_OSINFO_ENABLE 
#define MONITOR_OSINFO_ENABLE          1
#endif

static const char s_version[] = "ThinkOS " VERSION_NUM "\r\n";

static const char s_help[] = 
"\tCtrl+C - Restart\r\n"
"\tCtrl+F - Upload FPGA\r\n"
#if (MONITOR_OSINFO_ENABLE)
"\tCtrl+O - OS Info\r\n"
#endif
"\tCtrl+V - Help\r\n"
"\tCtrl+T - Upload ThinkOS\r\n"
"\tCtrl+Y - Upload YARD-ICE\r\n"
;

static const char s_hr[] = 
"\r\n----\r\n";

//static const char s_error[] = "Error!\r\n";

static const char s_confirm[] = "Confirm [y]?";

/* Receies a file using YMODEM protocol and writes into Flash. */
static int yflash(uint32_t blk_offs, uint32_t blk_size,
		   const struct magic_blk * magic)
{
	uintptr_t yflash_code = (uintptr_t)(0x20001000);
	int (* yflash_ram)(uint32_t, uint32_t, const struct magic_blk *);
	uintptr_t thumb;
	int ret;

	/* disable interrupts */
	cm3_cpsid_i();
	__thinkos_memcpy32((void *)yflash_code, otg_xflash_pic, 
					 sizeof_otg_xflash_pic);

    thumb = yflash_code | 0x00000001; /* thumb call */
	yflash_ram = (int (*)(uint32_t, uint32_t, const struct magic_blk *))thumb;
	ret = yflash_ram(blk_offs, blk_size, magic);

	return ret;
}

#define REQ_SHOW_OSINFO     MONITOR_USER_EVENT0
#define REQ_FPGA_UPLOAD     MONITOR_USER_EVENT1
#define REQ_BOOT_UPLOAD     MONITOR_USER_EVENT2
#define	REQ_CONFIG_UPLOAD   MONITOR_USER_EVENT3
#define REQ_SHOW_MENU       MONITOR_USER_EVENT4
#define REQ_SHOW_STACK_USG  MONITOR_USER_EVENT5

bool monitor_process_input(const struct monitor_comm * comm, int c)
{
	switch (c) {
	case CTRL_C:
		monitor_core_rst_and_signal(MONITOR_APP_EXEC);
		break;

#if (MONITOR_OSINFO_ENABLE)
	case CTRL_O:
		monitor_signal(REQ_SHOW_OSINFO);
		break;
#endif
    case CTRL_U:
		monitor_signal(REQ_SHOW_STACK_USG);
        break;

	case CTRL_V:
		monitor_signal(REQ_SHOW_MENU);
		break;
 
	case CTRL_Y:
		monitor_puts(s_confirm, comm);
		if (monitor_getc(comm) == 'y') {
			monitor_core_rst_and_signal(MONITOR_APP_UPLOAD);
		}

		break;

	case CTRL_FS:
		monitor_puts(s_confirm, comm);
		if (monitor_getc(comm) == 'y') {
			monitor_core_rst_and_signal(REQ_FPGA_UPLOAD);
		}
		break;

	case CTRL_F:
		monitor_puts(s_confirm, comm);
		if (monitor_getc(comm) == 'y') {
			monitor_core_rst_and_signal(REQ_FPGA_UPLOAD);
		}
		break;
	default:
		return false;
	}

	return true;
}

/* Default Monitor Task */
void __attribute__((noreturn)) monitor_task(const struct monitor_comm * comm, 
											void * param, 
											struct thinkos_rt * krn)
{
#if (MONITOR_OSINFO_ENABLE)
	uint32_t cycref[THINKOS_THREAD_LAST + 1];
#endif
	uint32_t sigmask = 0;
	uint8_t buf[4];
	uint8_t * ptr;
	int sig;
	int cnt;

	DCC_LOG(LOG_TRACE, "starting monitor...");

	/* unmask events */
	sigmask |= (1 << MONITOR_TASK_INIT);
	sigmask |= (1 << MONITOR_ON_CORE_RST);
#if (MONITOR_FAULT_ENABLE)
	sigmask |= (1 << MONITOR_THREAD_FAULT);
#endif
	sigmask |= (1 << MONITOR_COMM_EOT);
	sigmask |= (1 << MONITOR_COMM_RCV);
	sigmask |= (1 << MONITOR_COMM_CTL);
	sigmask |= (1 << MONITOR_COMM_BRK);
	sigmask |= (1 << MONITOR_TX_PIPE);
	sigmask |= (1 << MONITOR_RX_PIPE);

	sigmask |= (1 << MONITOR_APP_UPLOAD);
	sigmask |= (1 << MONITOR_APP_EXEC);
	sigmask |= (1 << REQ_FPGA_UPLOAD);
	sigmask |= (1 << REQ_BOOT_UPLOAD);
	sigmask |= (1 << REQ_SHOW_MENU);
	sigmask |= (1 << REQ_SHOW_STACK_USG);

#if (MONITOR_OSINFO_ENABLE)
	sigmask |= (1 << REQ_SHOW_OSINFO);
#endif

//	monitor_alarm(1000);

	monitor_unmask(MONITOR_COMM_CTL);
	monitor_unmask(MONITOR_COMM_BRK);

	for(;;) {
		switch ((sig = monitor_select(sigmask))) {
#if 0
		case MONITOR_ALARM:
			monitor_clear(MONITOR_ALARM);
			DCC_LOG(LOG_TRACE, "Alarm");
			monitor_alarm(5000);
			break;
#endif
		case MONITOR_COMM_BRK:
			monitor_clear(MONITOR_COMM_BRK);
			DCC_LOG(LOG_TRACE, "Line break received");
//			monitor_comm_break_ack(comm);
			monitor_core_rst_and_signal(REQ_SHOW_MENU);
			break;

		case MONITOR_ON_CORE_RST:
			DCC_LOG(LOG_TRACE, "Core reset received");
			monitor_clear(MONITOR_ON_CORE_RST);
			monitor_signal(MONITOR_APP_EXEC);
//			thinkos_krn_thread_init(krn, 1, &shell_thread_init);
			break;

		case MONITOR_APP_EXEC:
			DCC_LOG(LOG_TRACE, "APP_EXEC...");
			monitor_clear(MONITOR_APP_EXEC);
			board_reset();
			monitor_app_exec(APPLICATION_START_ADDR);
			break;

		case REQ_SHOW_STACK_USG:
			monitor_clear(REQ_SHOW_STACK_USG);
			monitor_print_stack_usage(comm);
			break;

#if (MONITOR_FAULT_ENABLE)
		case MONITOR_THREAD_FAULT:
			{
				int32_t errno;
				int32_t thread;

				monitor_clear(MONITOR_THREAD_FAULT);
				/* get the last thread known to be at fault */
				thread = monitor_thread_break_get(&errno);
				(void)thread;

				if ((errno >= THINKOS_ERR_APP_INVALID) && 
					(errno <= THINKOS_ERR_APP_BSS_INVALID)) {
					DCC_LOG(LOG_ERROR, "Invalid application!");
					monitor_thread_break_clr();
				} else if (errno == THINKOS_ERR_SYSCALL_INVALID) {
					struct thinkos_context * ctx = monitor_thread_ctx_get(thread);
					uint8_t * pc = (uint8_t *)ctx->pc;
					(void)pc;
					DCC_LOG1(LOG_ERROR, "Invalid System Call %d!", pc[-2]);
					monitor_thread_break_clr();
				} else {
					DCC_LOG2(LOG_ERROR, "<%d> error %d !!", thread, errno);
				}

				monitor_puts("!ERR: thread=", comm);
				monitor_comm_send_uint(thread, 3, comm);
				monitor_puts(", error code=", comm);
				monitor_comm_send_uint(errno, 3, comm);
			}
			break;
#endif

#if (MONITOR_OSINFO_ENABLE)
		case REQ_SHOW_OSINFO:
			monitor_clear(REQ_SHOW_OSINFO);
			monitor_print_osinfo(comm, cycref);
			break;
#endif

		case MONITOR_APP_UPLOAD:
			monitor_clear(MONITOR_APP_UPLOAD);
			yflash(APPLICATION_BLOCK_OFFS, APPLICATION_BLOCK_SIZE, 
				   &board_app_magic);
			break;

		case REQ_FPGA_UPLOAD:
			monitor_clear(REQ_FPGA_UPLOAD);
			yflash(RBF_BLOCK_OFFS, RBF_BLOCK_SIZE, NULL);
			break;

		case REQ_BOOT_UPLOAD:
			monitor_clear(REQ_BOOT_UPLOAD);
			yflash(BOOTLOADER_BLOCK_OFFS, BOOTLOADER_BLOCK_SIZE, 
				   &bootloader_magic);
			break;

		case REQ_SHOW_MENU:
			monitor_clear(REQ_SHOW_MENU);
			board_reset();
			if (monitor_comm_isconnected(comm)) {
				monitor_puts(s_hr, comm);
				monitor_puts(s_version, comm);
				monitor_puts(s_help, comm);
			}
			break;

		case MONITOR_COMM_RCV:
			/* receive from the COMM driver one byte at the time */
			if ((cnt = monitor_comm_recv(comm, buf, 1)) > 0) {
				int c = buf[0];
				/* process the input character */
				if (!monitor_process_input(comm, c)) {
					int n;
					/* if the character was not consumed by the monitor 
					   insert into the console pipe */
					/* get a pointer to the head of the pipe.
					 thinkos_console_rx_pipe_ptr() will return the number of 
					 consecutive spaces in the buffer. We need only one. */
					if ((n = thinkos_console_rx_pipe_ptr(&ptr)) > 0) {
						/* copy the character into the RX fifo */
						ptr[0] = c;
						/* commit the fifo head */
						thinkos_console_rx_pipe_commit(1);
					} else {
						/* discard */
					}
				}
			}
			break;

		case MONITOR_RX_PIPE:
			sigmask = monitor_on_rx_pipe(comm, sigmask);
			break;

		case MONITOR_COMM_CTL:
			DCC_LOG1(LOG_MSG, "comm=%08x", comm);
			sigmask = monitor_on_comm_ctl(comm, sigmask);
			break;

		case MONITOR_COMM_EOT:
			/* FALLTHROUGH */
		case MONITOR_TX_PIPE:
			sigmask = monitor_on_tx_pipe(comm, sigmask);
			break;

		default:
			monitor_clear(sig);
			DCC_LOG1(LOG_ERROR, "Unhandled signal %d", sig);
		}
	}
}
