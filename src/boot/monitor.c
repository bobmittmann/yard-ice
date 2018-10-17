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

/** 
 * @file monitor.c
 * @brief YARD-ICE debug monitor
 * @author Robinson Mittmann <bobmittmann@gmail.com>
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
#include <thinkos/dbgmon.h>
#define __THINKOS_BOOTLDR__
#include <thinkos/bootldr.h>
#include <thinkos.h>
#include <sys/dcclog.h>
#include <sys/delay.h>
#include "version.h"

static const struct magic_blk app_magic = {
	.hdr = {
		.pos = 0,
		.cnt = 3
	},
	.rec = {
		{  0xffffffff, 0x0a0de004 },
		{  0xffffffff, 0x6e696854 },
		{  0xffffffff, 0x00534f6b }
	}
};

extern int __heap_end;
const void * heap_end = &__heap_end; 
extern uint32_t _stack;
extern const struct thinkos_thread_inf thinkos_main_inf;

void board_init(void);

extern const uint8_t otg_xflash_pic[];
extern const unsigned int sizeof_otg_xflash_pic;

struct magic_hdr {
	uint16_t pos;
	uint16_t cnt;
};

struct magic_rec {
	uint32_t mask;
	uint32_t comp;
};

struct magic {
	struct magic_hdr hdr;
	struct magic_rec rec[];
};

#ifndef MONITOR_UPGRADE_ENABLE
#if DEBUG
#define MONITOR_UPGRADE_ENABLE     0
#else
#define MONITOR_UPGRADE_ENABLE     1
#endif
#endif

#ifndef MONITOR_APPRESTART_ENABLE
#define MONITOR_APPRESTART_ENABLE  1
#endif

#ifndef MONITOR_OSINFO_ENABLE 
#define MONITOR_OSINFO_ENABLE      1
#endif

#ifndef MONITOR_PAUSE_ENABLE
#define MONITOR_PAUSE_ENABLE       1
#endif

#ifndef MONITOR_LOCKINFO_ENABLE
#define MONITOR_LOCKINFO_ENABLE    1
#endif

#define RBF_BLOCK_OFFS 0x00010000
#define RBF_BLOCK_SIZE (64 * 1024)
#define RBF_START_ADDR (0x08000000 + RBF_BLOCK_OFFS)

#define APPLICATION_BLOCK_OFFS 0x00020000
#define APPLICATION_BLOCK_SIZE (384 * 1024)
#define APPLICATION_START_ADDR (0x08000000 + APPLICATION_BLOCK_OFFS)

/* ASCII Keyboard codes */

#define _NULL_  0x00 /* Null (Ctrl+@) */
#define CTRL_A  0x01 /* SOH */
#define CTRL_B  0x02 /* STX */
#define CTRL_C  0x03 /* ETX */
#define CTRL_D  0x04 /* EOT */
#define CTRL_E  0x05 /* ENQ */
#define CTRL_F  0x06 /* ACK */
#define CTRL_G  0x07 /* BEL */
#define CTRL_H  0x08 /* BS */
#define CTRL_I  0x09 /* TAB */
#define CTRL_J  0x0a /* LF */
#define CTRL_K  0x0b /* VT */
#define CTRL_L  0x0c /* FF */
#define CTRL_M  0x0d /* CR */
#define CTRL_N  0x0e /* SO */
#define CTRL_O  0x0f /* SI */
#define CTRL_P  0x10 /* DLE */
#define CTRL_Q  0x11 /* DC1 */
#define CTRL_R  0x12 /* DC2 */
#define CTRL_S  0x13 /* DC3 */
#define CTRL_T  0x14 /* DC4 */
#define CTRL_U  0x15 /* NAK */
#define CTRL_V  0x16 /* SYN */
#define CTRL_W  0x17 /* ETB */
#define CTRL_X  0x18 /* CAN */
#define CTRL_Y  0x19 /* EM */
#define CTRL_Z  0x1a /* SUB */
#define _ESC_   0x1b /* ESC (Ctrl+[) */
#define CTRL_FS 0x1c /* FS  (Ctrl+\) */
#define CTRL_GS 0x1d /* GS  (Ctrl+]) */
#define CTRL_RS 0x1e /* RS  (Ctrl+^) */
#define CTRL_US 0x1f /* US  (Ctrl+_) */

static const char s_version[] = "ThinkOS " VERSION_NUM "\r\n";

static const char s_help[] = 
#if (MONITOR_OSINFO_ENABLE)
" ^O - OS Info\r\n"
#endif
#if (MONITOR_PAUSE_ENABLE)
" ^P - Pause app\r\n"
#endif
#if (MONITOR_UPGRADE_ENABLE)
" ^R - Upload FPGA\r\n"
#endif
" ^V - Help\r\n"
#if (MONITOR_APPWIPE_ENABLE)
" ^W - Wipe App\r\n"
#endif
#if (MONITOR_UPGRADE_ENABLE)
" ^Y - Upload YARD-ICE\r\n"
#endif
#if (MONITOR_APPRESTART_ENABLE)
" ^Z - Restart\r\n"
#endif
#if (MONITOR_UPGRADE_ENABLE)
" ^\\ - Upload ThinkOS\r\n"
#endif
;

static const char s_hr[] = 
"\r\n----\r\n";

//static const char s_error[] = "Error!\r\n";

#if (MONITOR_UPGRADE_ENABLE)
static const char s_confirm[] = "Confirm [y]?";
#endif

#if (MONITOR_OSINFO_ENABLE)
#define PUTS(S) dbgmon_printf(comm, S) 
#else
#define PUTS(S) dmputs(S, comm) 
#endif

#if (MONITOR_UPGRADE_ENABLE)
static int yflash(uint32_t blk_offs, uint32_t blk_size,
		   const struct magic_blk * magic)
{
	uint32_t * yflash_code = (uint32_t *)(0x20001000);
	int (* yflash_ram)(uint32_t, uint32_t, const struct magic_blk *) = 
		((void *)yflash_code) + 1;
//	unsigned int pri;
	int ret;

//	pri = cm3_primask_get();
	cm3_primask_set(1);

	__thinkos_memcpy(yflash_code, otg_xflash_pic, sizeof_otg_xflash_pic);
	ret = yflash_ram(blk_offs, blk_size, magic);

//	cm3_primask_set(pri);

	return ret;
}

static const struct magic_blk bootloader_magic = {
	.hdr = {
		.pos = 0,
		.cnt = 2
	},
	.rec = {
		{  0xfffc0000, 0x20000000 },
		{  0xffff0000, 0x08000000 },
	}
};

#endif

#if (MONITOR_OSINFO_ENABLE)

#if THINKOS_ENABLE_THREAD_VOID
#define CYCCNT_MAX ((THINKOS_THREADS_MAX) + 2) /* extra slot for void thread */
#else
#define CYCCNT_MAX ((THINKOS_THREADS_MAX) + 1)
#endif

int __scan_stack(void * stack, unsigned int size);

static void print_osinfo(struct dbgmon_comm * comm)
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

	dbgmon_printf(comm, s_hr);
#if THINKOS_ENABLE_PROFILING
	cycsum = 0;
	for (i = 0; i < THINKOS_THREADS_MAX; ++i)
		cycsum += cycbuf[i];
	cycbusy = cycsum;
	cycsum += cycbuf[THINKOS_CYCCNT_IDLE];

	cycdiv = (cycsum + 500) / 1000;
	busy = (cycbusy + cycdiv / 2) / cycdiv;
	idle = 1000 - busy;
	dbgmon_printf(comm, "CPU: %d.%d%% busy, %d.%d%% idle\r\n", 
			 busy / 10, busy % 10, idle / 10, idle % 10);
#endif

	dbgmon_printf(comm, " Th     Tag       SP       LR       PC  WQ TmW");
#if THINKOS_ENABLE_PROFILING
	dbgmon_printf(comm, " CPU %% ");
#endif
#if (MONITOR_LOCKINFO_ENABLE)
	dbgmon_printf(comm, " Locks");
#endif
	dbgmon_printf(comm, "\r\n");

	for (i = 0; i < THINKOS_THREADS_MAX; ++i) {
		if (rt->ctx[i] != NULL) {
#if (MONITOR_LOCKINFO_ENABLE)
			int j;
#endif
			/* Internal thread ids start form 0 whereas user
			   thread numbers start form one ... */
			tag = (rt->th_inf[i] != NULL) ? rt->th_inf[i]->tag : "...";
			dbgmon_printf(comm, "%3d %7s %08x %08x %08x %3d %s", i + 1, tag,
					 (uint32_t)rt->ctx[i], rt->ctx[i]->lr, rt->ctx[i]->pc, 
					 rt->th_stat[i] >> 1, rt->th_stat[i] & 1 ? "Yes" : " No");

#if THINKOS_ENABLE_PROFILING
			busy = (cycbuf[i] + cycdiv / 2) / cycdiv;
			dbgmon_printf(comm, " %3d.%d", busy / 10, busy % 10);
#endif


#if (MONITOR_LOCKINFO_ENABLE)
			for (j = 0; j < THINKOS_MUTEX_MAX ; ++j) {
				if (rt->lock[j] == i)
					dbgmon_printf(comm, " %d", j + THINKOS_MUTEX_BASE);
			}
#endif
			dbgmon_printf(comm, "\r\n");
		}
	}
}

#endif /* MONITOR_OSINFO_ENABLE */

static void __main_thread_exec(void (* func)(int))
{
	int thread_id = 0;

	DCC_LOG(LOG_TRACE, "__thinkos_thread_abort()");
	__thinkos_thread_abort(thread_id);

	DCC_LOG(LOG_TRACE, "__thinkos_thread_init()");
	__thinkos_thread_init(thread_id, (uintptr_t)&_stack, func, (void *)NULL);

#if THINKOS_ENABLE_THREAD_INFO
	__thinkos_thread_inf_set(thread_id, &thinkos_main_inf);
#endif

	DCC_LOG(LOG_TRACE, "__thinkos_thread_resume()");
	__thinkos_thread_resume(thread_id);

	DCC_LOG(LOG_TRACE, "__thinkos_defer_sched()");
	__thinkos_defer_sched();
}

#if (MONITOR_PAUSE_ENABLE)
static void pause_all(void)
{
	unsigned int wq;
	unsigned int irq;


	/* clear all bits on all queues */
	for (wq = 0; wq < THINKOS_WQ_LST_END; ++wq) 
		thinkos_rt.wq_lst[wq] = 0;

#if ((THINKOS_THREADS_MAX) < 32) 
	thinkos_rt.wq_ready = 1 << (THINKOS_THREADS_MAX);
#endif

	for (irq = 0; irq < THINKOS_IRQ_MAX; ++irq) {
		if (thinkos_rt.irq_th[irq] != THINKOS_THREAD_IDLE)
			cm3_irq_disable(irq);
	}

	__thinkos_defer_sched();
}
#endif

static bool monitor_process_input(struct dbgmon_comm * comm, int c)
{
	switch (c) {
#if (MONITOR_UPGRADE_ENABLE)
	case CTRL_FS:
		PUTS(s_confirm);
		if (dbgmon_getc(comm) == 'y') {
			dbgmon_soft_reset();
			dbgmon_signal(DBGMON_USER_EVENT2);
		}
		break;
#endif
#if (MONITOR_OSINFO_ENABLE)
	case CTRL_O:
		print_osinfo(comm);
		break;
#endif
	case CTRL_V:
		PUTS(s_hr);
		PUTS(s_version);
		PUTS(s_help);
		break;

#if (MONITOR_PAUSE_ENABLE)
	case CTRL_P:
		PUTS("^P\r\n");
		pause_all();
		break;
#endif

#if (MONITOR_UPGRADE_ENABLE)
	case CTRL_Y:
		PUTS(s_confirm);
		if (dbgmon_getc(comm) == 'y') {
			dbgmon_req_app_upload(); 
		} else {
			PUTS("\r\n");
		}

		break;

	case CTRL_R:
		PUTS(s_confirm);
		if (dbgmon_getc(comm) == 'y') {
			dbgmon_soft_reset();
			dbgmon_signal(DBGMON_USER_EVENT1);
		}
		break;
#endif


#if (MONITOR_APPRESTART_ENABLE)
	case CTRL_Z:
		PUTS("^Z\r\n");
		dbgmon_req_app_exec(); 
		break;
#endif
	default:
		return false;
	}

	return true;
}

/* Default Monitor Task */
#define MONITOR_AUTOBOOT 1
#define MONITOR_SHELL 2

void __attribute__((noreturn)) monitor_task(struct dbgmon_comm * comm, void * param)
{
	uint32_t sigmask;
	uint32_t sig;
	uint8_t buf[1];
	uint8_t * ptr;
	int cnt;
	int len;
	uint32_t flags = (uint32_t)param;

	/* unmask events */
	sigmask = (1 << DBGMON_SOFTRST) | 
		(1 << DBGMON_COMM_RCV) | (1 << DBGMON_TX_PIPE) | 
		(1 << DBGMON_RX_PIPE) | (1 << DBGMON_APP_EXEC) |
		(1 << DBGMON_APP_UPLOAD) | (1 << DBGMON_USER_EVENT1) | 
		(1 << DBGMON_USER_EVENT2);

#if DEBUG
	sigmask |= (1 << DBGMON_COMM_CTL);
#endif

	if (!(flags & MONITOR_AUTOBOOT)) {
		PUTS(s_hr);
		PUTS(s_version);
	}

	if (flags & MONITOR_AUTOBOOT) {
		dbgmon_req_app_exec(); 
	}

	for(;;) {
		switch ((sig = dbgmon_select(sigmask))) {

		case DBGMON_STARTUP:
			dbgmon_clear(DBGMON_STARTUP);
			break;

		case DBGMON_SOFTRST:
			dbgmon_clear(DBGMON_SOFTRST);
			board_init();
			PUTS("+RST\r\n");
			break;

		case DBGMON_APP_UPLOAD:
			dbgmon_clear(DBGMON_APP_UPLOAD);
			yflash(APPLICATION_BLOCK_OFFS, APPLICATION_BLOCK_SIZE, &app_magic);
			break;

		case DBGMON_USER_EVENT1:
			dbgmon_clear(DBGMON_USER_EVENT1);
			yflash(RBF_BLOCK_OFFS, RBF_BLOCK_SIZE, NULL);
			break;

		case DBGMON_USER_EVENT2:
			dbgmon_clear(DBGMON_USER_EVENT2);
			yflash(0, 32768, &bootloader_magic);
			break;

#if DEBUG
		if (sigset & (1 << DBGMON_COMM_CTL)) {
			DCC_LOG1(LOG_TRACE, "Comm Ctl, sigset=%08x", sigset);
			dbgmon_clear(DBGMON_COMM_CTL);
			if (!dbgmon_comm_isconnected(comm))	
				dbgmon_reset();
		}
#endif
		case DBGMON_APP_EXEC: {
			uint32_t * signature = (uint32_t *)APPLICATION_START_ADDR;
			int i = app_magic.hdr.cnt;

			dbgmon_clear(DBGMON_APP_EXEC);

			for (i = app_magic.hdr.cnt - 1; i >= 0; --i) {
				if (signature[i] != app_magic.rec[i].comp) {
					PUTS("!ERR: magic\r\n");
					flags |= MONITOR_SHELL;
					break;
				}
			}

			if (i < 0) {
				__main_thread_exec((void *)(APPLICATION_START_ADDR | 1));
			}
		}
			break;
#if 0
		case DBGMON_APP_EXEC:
			dbgmon_clear(DBGMON_APP_EXEC);
			if (!dbgmon_app_exec(app, false)) {
				PUTS("!ERR: app\r\n");
			}
			break;
#endif

		case DBGMON_COMM_RCV:
			if (flags & MONITOR_SHELL) { 
				/* receive from the COMM driver one bye at the time */
				if (dbgmon_comm_recv(comm, buf, 1) > 0) {
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
			} else {
				if ((cnt = __console_rx_pipe_ptr(&ptr)) > 0) {
					if ((len = dbgmon_comm_recv(comm, ptr, cnt)) > 0)
						__console_rx_pipe_commit(len); 
				} else {
					sigmask &= ~(1 << DBGMON_COMM_RCV);
				}
			}
			break;

		case DBGMON_RX_PIPE:
			if ((cnt = __console_rx_pipe_ptr(&ptr)) > 0) {
				sigmask |= (1 << DBGMON_COMM_RCV);
			} else {
				sigmask &= ~(1 << DBGMON_RX_PIPE);
			}
//			dbgmon_clear(DBGMON_RX_PIPE);
			break;


		case DBGMON_TX_PIPE:
			if ((cnt = __console_tx_pipe_ptr(&ptr)) > 0) {
				cnt = dbgmon_comm_send(comm, ptr, cnt);
				__console_tx_pipe_commit(cnt); 
			} else {
				sigmask |= (1 << DBGMON_TX_PIPE);
				//dbgmon_clear(DBGMON_TX_PIPE);
			}
		}
	}
}

