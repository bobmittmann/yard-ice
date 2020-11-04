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

#define __THINKOS_BOOTLDR__
#include <thinkos/bootldr.h>

#include <sys/dcclog.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
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
#pragma GCC diagnostic pop

extern int __heap_end;
const void * heap_end = &__heap_end; 
extern uint32_t _stack;
extern const struct thinkos_thread_inf thinkos_main_inf;

extern const uint8_t otg_xflash_pic[];
extern const unsigned int sizeof_otg_xflash_pic;

void board_reset(void);

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
#define MONITOR_UPGRADE_ENABLE     1
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
#define MONITOR_PAUSE_ENABLE       0
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

#if (MONITOR_UPGRADE_ENABLE)
/* Receies a file using YMODEM protocol and writes into Flash. */
static int yflash(uint32_t blk_offs, uint32_t blk_size,
		   const struct magic_blk * magic)
{
	uintptr_t yflash_code = (uintptr_t)(0x20001000);
	int (* yflash_ram)(uint32_t, uint32_t, const struct magic_blk *);
	uintptr_t thumb;
	int ret;

	cm3_primask_set(1);
	__thinkos_memcpy((void *)yflash_code, otg_xflash_pic, 
					 sizeof_otg_xflash_pic);

    thumb = yflash_code | 0x00000001; /* thumb call */
	yflash_ram = (int (*)(uint32_t, uint32_t, const struct magic_blk *))thumb;
	ret = yflash_ram(blk_offs, blk_size, magic);

	return ret;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
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
#pragma GCC diagnostic pop

#endif

#if (MONITOR_OSINFO_ENABLE)

void monitor_puthex(uint32_t val, unsigned int witdh, 
				   const struct monitor_comm * comm);

void monitor_putuint(uint32_t val, unsigned int witdh, 
					const struct monitor_comm * comm);

void monitor_putint(int32_t val, unsigned int witdh, 
				   const struct monitor_comm * comm);

#define CYCCNT_MAX THINKOS_THREADS_MAX

int __scan_stack(void * stack, unsigned int size);

static void print_osinfo(const struct monitor_comm * comm)
{
	struct thinkos_rt * rt = &thinkos_rt;
#if (THINKOS_ENABLE_PROFILING)
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

#if (THINKOS_ENABLE_PROFILING)
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

	monitor_puts(s_hr, comm);
#if (THINKOS_ENABLE_PROFILING)
	cycsum = 0;
	for (i = 0; i < THINKOS_THREADS_MAX; ++i)
		cycsum += cycbuf[i];
	cycbusy = cycsum;

	cycdiv = (cycsum + 500) / 1000;
	busy = (cycbusy + cycdiv / 2) / cycdiv;
	idle = 1000 - busy;

	monitor_puts("CPU: ", comm);
	monitor_comm_send_uint(busy / 10, 3, comm);
	monitor_putc('.', comm);
	monitor_comm_send_uint(busy % 10, 1, comm);
	monitor_puts("% busy, ", comm);
	monitor_comm_send_uint(idle / 10, 3, comm);
	monitor_putc('.', comm);
	monitor_comm_send_uint(idle % 10, 1, comm);
	monitor_puts("% idle\r\n", comm);
#endif

	monitor_puts( " Th     Tag       SP       LR       PC  WQ TmW", comm);
#if (THINKOS_ENABLE_PROFILING)
	monitor_puts(" CPU % ", comm);
#endif
#if (MONITOR_LOCKINFO_ENABLE)
	monitor_puts(" Locks", comm);
#endif
	monitor_puts("\r\n", comm);

	for (i = 0; i < THINKOS_THREADS_MAX; ++i) {
		if (__thinkos_thread_ctx_is_valid(i)) {
#if (MONITOR_LOCKINFO_ENABLE)
			int j;
#endif
			monitor_comm_send_uint(i + 1, 3, comm);
			/* Internal thread ids start form 0 whereas user
			   thread numbers start form one ... */
			tag = (rt->th_inf[i] != NULL) ? rt->th_inf[i]->tag : "...";
			monitor_comm_send_str(tag, 8, comm);
			monitor_comm_send_blanks(1, comm);
			monitor_comm_send_hex(__thread_sp_get(rt, i), 8, comm);
			monitor_comm_send_blanks(1, comm);
			monitor_comm_send_hex(__thread_lr_get(rt, i), 8, comm);
			monitor_comm_send_blanks(1, comm);
			monitor_comm_send_hex(__thread_pc_get(rt, i), 8, comm);
			monitor_comm_send_uint(__thread_wq_get(rt, i), 4, comm);
			monitor_comm_send_str(__thread_tmw_get(rt, i) ? 
								  "Yes" : " No", 4, comm);

#if (THINKOS_ENABLE_PROFILING)
			busy = (cycbuf[i] + cycdiv / 2) / cycdiv;
			monitor_comm_send_uint(busy / 10, 3, comm);
			monitor_putc('.', comm);
			monitor_comm_send_uint(busy % 10, 1, comm);
#endif


#if (MONITOR_LOCKINFO_ENABLE)
			for (j = 0; j < THINKOS_MUTEX_MAX ; ++j) {
				if (rt->lock[j] == i)
					monitor_comm_send_uint(j + THINKOS_MUTEX_BASE, 3, comm);
			}
#endif
			monitor_puts("\r\n", comm);
		}
	}
}

#endif /* MONITOR_OSINFO_ENABLE */

#if (MONITOR_PAUSE_ENABLE)
static void pause_all(void)
{
	unsigned int wq;
	unsigned int irq;


	/* clear all bits on all queues */
	for (wq = 0; wq < THINKOS_WQ_CNT; ++wq) 
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

bool monitor_process_input(const struct monitor_comm * comm, int c)
{
	switch (c) {
#if (MONITOR_UPGRADE_ENABLE)
	case CTRL_FS:
		monitor_puts(s_confirm, comm);
		if (monitor_getc(comm) == 'y') {
			monitor_soft_reset();
			monitor_signal(MONITOR_USER_EVENT2);
		}
		break;
#endif
#if (MONITOR_OSINFO_ENABLE)
	case CTRL_O:
		print_osinfo(comm);
		break;
#endif
	case CTRL_V:
		monitor_puts(s_hr, comm);
		monitor_puts(s_version, comm);
		monitor_puts(s_help, comm);
		break;

#if (MONITOR_PAUSE_ENABLE)
	case CTRL_P:
		monitor_puts("^P\r\n", comm);
		pause_all();
		break;
#endif

#if (MONITOR_UPGRADE_ENABLE)
	case CTRL_Y:
		monitor_puts(s_confirm, comm);
		if (monitor_getc(comm) == 'y') {
			monitor_req_app_upload(); 
		} else {
			monitor_puts("\r\n", comm);
		}

		break;

	case CTRL_R:
		monitor_puts(s_confirm, comm);
		if (monitor_getc(comm) == 'y') {
			monitor_soft_reset();
			monitor_signal(MONITOR_USER_EVENT1);
		}
		break;
#endif

#if (MONITOR_APPRESTART_ENABLE)
	case CTRL_Z:
		monitor_puts("^Z\r\n", comm);
		monitor_req_app_exec(); 
		break;
#endif
	default:
		return false;
	}

	return true;
}

static void __main_thread_exec(int (* func)(void *), void * arg)
{
	int thread_id = 0;
	struct thinkos_context * ctx;

	DCC_LOG(LOG_TRACE, "__thinkos_thread_abort()");
	__thinkos_thread_abort(thread_id);

	DCC_LOG2(LOG_TRACE, "__thinkos_thread_ctx_init(func=%p arg=%p)", func, arg);
	ctx = __thinkos_thread_ctx_init(thread_id, (uintptr_t)&_stack, 
									(uintptr_t)func, (uintptr_t)arg);

#if (THINKOS_ENABLE_THREAD_INFO)
	__thinkos_thread_inf_set(thread_id, &thinkos_main_inf);
#endif
	
#if (THINKOS_ENABLE_STACK_LIMIT)
	__thinkos_thread_sl_set(thread_id, 0);
#endif

	/* commit the context to the kernel */ 
	__thinkos_thread_ctx_set(thread_id, ctx, CONTROL_SPSEL | CONTROL_nPRIV);

	DCC_LOG1(LOG_INFO, "thread=%d [ready]", thread_id);
	__bit_mem_wr(&thinkos_rt.wq_ready, thread_id, 1);

	DCC_LOG(LOG_TRACE, "__thinkos_defer_sched()");
	__thinkos_defer_sched();
}

static bool __monitor_app_exec(struct monitor * monitor) 
{
	uint32_t * signature = (uint32_t *)APPLICATION_START_ADDR;
	int i;

	for (i = app_magic.hdr.cnt - 1; i >= 0; --i) {
		if (signature[i] != app_magic.rec[i].comp) {
			monitor->flags |= MONITOR_SHELL;
			return false;
		}
	}

	if (i < 0) {
		uintptr_t thumb;
		int (* app)(void *);

		thumb = ((uintptr_t)signature) + 1;
		app = (int (*)(void *))thumb;
		__main_thread_exec(app, NULL);
		return true;
	}

	return false;
}

/* Default Monitor Task */
void __attribute__((noreturn)) monitor_task(const struct monitor_comm * comm, 
                                            void * param)
{
	uint32_t sigmask = 0;
	uint8_t buf[1];
	uint8_t * ptr;
	uint32_t sig;
	int cnt;
#if (THINKOS_ENABLE_CONSOLE_RAW)
	bool raw_mode = false;
#endif
	bool connected = false;
	struct monitor monitor;

	monitor.flags = (uint32_t)param;

	/* unmask events */
	sigmask |= (1 << MONITOR_SOFTRST);
	sigmask |= (1 << MONITOR_STARTUP);
	sigmask |= (1 << MONITOR_COMM_RCV);
	sigmask |= (1 << MONITOR_COMM_CTL);
	sigmask |= (1 << MONITOR_TX_PIPE);
	sigmask |= (1 << MONITOR_APP_STOP);
	sigmask |= (1 << MONITOR_APP_EXEC);
	sigmask |= (1 << MONITOR_APP_UPLOAD);
	sigmask |= (1 << MONITOR_APP_ERASE);
	sigmask |= (1 << MONITOR_USER_EVENT1);
	sigmask |= (1 << MONITOR_USER_EVENT2);

	if (!(monitor.flags & MONITOR_AUTOBOOT)) {
		monitor_puts(s_hr, comm);
		monitor_puts(s_version, comm);
	}

	if (monitor.flags & MONITOR_AUTOBOOT) {
		monitor_req_app_exec(); 
	}

	for(;;) {
		switch ((sig = monitor_select(sigmask))) {

		case MONITOR_STARTUP:
			monitor_clear(MONITOR_STARTUP);
			break;

		case MONITOR_SOFTRST:
			monitor_clear(MONITOR_SOFTRST);
			board_reset();
			monitor_puts("+RST\r\n", comm);
			goto is_connected;

#if (MONITOR_UPGRADE_ENABLE)
		case MONITOR_APP_UPLOAD:
			monitor_clear(MONITOR_APP_UPLOAD);
			yflash(APPLICATION_BLOCK_OFFS, APPLICATION_BLOCK_SIZE, 
				   &app_magic);
			break;

		case MONITOR_USER_EVENT1:
			monitor_clear(MONITOR_USER_EVENT1);
			yflash(RBF_BLOCK_OFFS, RBF_BLOCK_SIZE, NULL);
			break;

		case MONITOR_USER_EVENT2:
			monitor_clear(MONITOR_USER_EVENT2);
			yflash(0, 32768, &bootloader_magic);
			break;
#endif

		case MONITOR_APP_EXEC:
			monitor_clear(MONITOR_APP_EXEC);
			if (!__monitor_app_exec(&monitor)) {
				monitor_puts("!ERR: app\r\n", comm);
			}
			break;

		case MONITOR_COMM_RCV:
#if (THINKOS_ENABLE_CONSOLE_RAW)
			raw_mode = thinkos_console_is_raw_mode();
			if (raw_mode) {
				goto raw_mode_recv;
			}
#endif
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

#if (THINKOS_ENABLE_CONSOLE_RAW)
raw_mode_recv:
			/* get a pointer to the head of the pipe.
			   thinkos_console_rx_pipe_ptr() will return the number of 
			   consecutive spaces in the buffer. */
			if ((cnt = thinkos_console_rx_pipe_ptr(&ptr)) > 0) {
				int n;
				/* receive from the COMM driver */
				if ((n = monitor_comm_recv(comm, ptr, cnt)) > 0) {
					/* commit the fifo head */
					thinkos_console_rx_pipe_commit(n);
				}
				/* Wait for COMM_RECV */
				sigmask |= (1 << MONITOR_COMM_RCV);
				sigmask &=  ~(1 << MONITOR_RX_PIPE);
			} else {
				/* Wait for RX_PIPE */
				sigmask &= ~(1 << MONITOR_COMM_RCV);
				sigmask |= (1 << MONITOR_RX_PIPE);
			}
#endif
			break;

		case MONITOR_COMM_CTL:
			monitor_clear(MONITOR_COMM_CTL);
is_connected:
			{
				connected = monitor_comm_isconnected(comm);
				thinkos_krn_console_connect_set(connected);
#if (THINKOS_ENABLE_CONSOLE_RAW)
				raw_mode = thinkos_console_is_raw_mode();
#endif
				sigmask &= ~((1 << MONITOR_COMM_EOT) | 
							 (1 << MONITOR_COMM_RCV) |
							 (1 << MONITOR_RX_PIPE));
				if (connected) {
					sigmask |= ((1 << MONITOR_COMM_EOT) |
							(1 << MONITOR_COMM_RCV));
				}

				sigmask |= (1 << MONITOR_TX_PIPE);
			}
			break;

		case MONITOR_COMM_EOT:
			/* FALLTHROUGH */
		case MONITOR_TX_PIPE:
			if ((cnt = thinkos_console_tx_pipe_ptr(&ptr)) > 0) {
				int n;
				/* send to the COMM driver */
				if ((n = monitor_comm_send(comm, ptr, cnt)) > 0) {
					thinkos_console_tx_pipe_commit(n);
				} 
				/* Wait for COMM_EOT */
				sigmask |= (1 << MONITOR_COMM_EOT);
				sigmask &= ~(1 << MONITOR_TX_PIPE);
			} else {
				/* Wait for TX_PIPE */
				sigmask |= (1 << MONITOR_TX_PIPE);
				sigmask &= ~(1 << MONITOR_COMM_EOT);
			}
			break;

		case MONITOR_RX_PIPE:
			/* get a pointer to the head of the pipe.
			   thinkos_console_rx_pipe_ptr() will return the number of 
			   consecutive spaces in the buffer. */
			if ((cnt = thinkos_console_rx_pipe_ptr(&ptr)) > 0) {
				int n;
				/* receive from the COMM driver */
				if ((n = monitor_comm_recv(comm, ptr, cnt)) > 0) {
					/* commit the fifo head */
					thinkos_console_rx_pipe_commit(n);
				}
				/* Wait for COMM_RECV */
				sigmask |= (1 << MONITOR_COMM_RCV);
				sigmask &= ~(1 << MONITOR_RX_PIPE);
			} else {
				/* Wait for RX_PIPE */
				sigmask &= ~(1 << MONITOR_COMM_RCV);
				sigmask |= (1 << MONITOR_RX_PIPE);
			}
			break;
		}
	}
}

