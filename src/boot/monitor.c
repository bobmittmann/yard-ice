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
const struct magic_blk app_magic = {
	.hdr = {
		.pos = 0,
		.cnt = 3
	},
	.rec = {
		{  0xffffffff, 0x08000041 },
		{  0xffffffff, 0x20002000 },
		{  0xffffffff, 0x00004000 }
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

#if (THINKOS_ENABLE_ERROR_TRAP)
#define MONITOR_FAULT_ENABLE 1
#else
#define MONITOR_FAULT_ENABLE 0
#endif

#ifndef MONITOR_UPGRADE_ENABLE
  #if DEBUG
    #define MONITOR_UPGRADE_ENABLE     0
  #else
    #define MONITOR_UPGRADE_ENABLE     1
  #endif
#endif

#ifndef MONITOR_APPRESTART_ENABLE
#define MONITOR_APPRESTART_ENABLE      1
#endif

#ifndef MONITOR_OSINFO_ENABLE 
#define MONITOR_OSINFO_ENABLE          1
#endif

#ifndef MONITOR_PAUSE_ENABLE
#define MONITOR_PAUSE_ENABLE           0
#endif

#ifndef MONITOR_LOCKINFO_ENABLE
#define MONITOR_LOCKINFO_ENABLE        1
#endif

#ifndef MONITOR_UPLOAD_CONFIG_ENABLE
#define MONITOR_UPLOAD_CONFIG_ENABLE   1
#endif

#ifndef MONITOR_APP_EXEC
#define MONITOR_APP_EXEC               0
#endif

#define BOOTLOADER_BLOCK_OFFS 0x00000000
#define BOOTLOADER_BLOCK_SIZE (16 * 1024)
#define BOOTLOADER_START_ADDR (0x08000000 + BOOTLOADER_BLOCK_OFFS)

#define CONFIG_BLOCK_OFFS 0x00008000
#define CONFIG_BLOCK_SIZE (16 * 1024)
#define CONFIG_START_ADDR (0x08000000 + CONFIG_BLOCK_OFFS)

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
#if (MONITOR_UPLOAD_CONFIG_ENABLE)
" ^F - Upload Config\r\n"
#endif
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


void print_percent(uint32_t val, const struct monitor_comm * comm)
{
	monitor_comm_send_uint(val / 10, 4, comm);
	monitor_putc('.', comm);
	monitor_comm_send_uint(val % 10, 1, comm);
}

static void print_osinfo(const struct monitor_comm * comm, uint32_t cycref[])
{
	struct thinkos_rt * rt = &thinkos_rt;
#if (THINKOS_ENABLE_PROFILING)
	uint32_t cyc[THINKOS_CTX_CNT];
	uint32_t cycdiv;
	uint32_t busy;
	uint32_t cycsum = 0;
	uint32_t cycbusy;
	uint32_t idle;
	uint32_t dif;
#endif
	const char * tag;
	int i;

	monitor_puts(s_hr, comm);
#if (THINKOS_ENABLE_PROFILING)
	cycsum = 0;
	for (i = 0; i < THINKOS_CTX_CNT; ++i) {
		uint32_t cnt = rt->cyccnt[i];
		uint32_t ref = cycref[i];

		cycref[i] = cnt;
		dif = cnt - ref; 
		cycsum += dif;
		cyc[i] = dif;
	}

	cycbusy = cycsum - dif;

	cycdiv = (cycsum + 500) / 1000;

	busy = cycbusy / cycdiv;
	if (busy > 1000)
		busy  = 1000;

	idle = 1000 - busy;
	(void) idle;

	monitor_puts("CPU: ", comm);
	print_percent(busy, comm);
	monitor_puts("% busy, ", comm);
	print_percent(idle, comm);
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
			tag = __thinkos_thread_tag_get(i);
			monitor_comm_send_str(tag, 8, comm);
			monitor_comm_send_blanks(1, comm);
			monitor_comm_send_hex(__thinkos_thread_sp_get(i), 8, comm);
			monitor_comm_send_blanks(1, comm);
			monitor_comm_send_hex(__thinkos_thread_lr_get(i), 8, comm);
			monitor_comm_send_blanks(1, comm);
			monitor_comm_send_hex(__thinkos_thread_pc_get(i), 8, comm);
			monitor_comm_send_uint(__thinkos_thread_wq_get(i), 4, comm);
			monitor_comm_send_str(__thinkos_thread_tmw_get(i) ? 
								  "Yes" : " No", 4, comm);

#if (THINKOS_ENABLE_PROFILING)
			busy = cyc[i] / cycdiv;
			if (busy > 1000)
				busy  = 1000;
			print_percent(busy, comm);
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
		monitor_signal(MONITOR_USER_EVENT4);
		break;
#endif
	case CTRL_V:
		monitor_puts(s_hr, comm);
		monitor_puts(s_version, comm);
		monitor_puts(s_help, comm);
		break;

#if (MONITOR_PAUSE_ENABLE)
	case CTRL_P:
		pause_all();
		break;
#endif

#if (MONITOR_UPGRADE_ENABLE)
	case CTRL_Y:
		monitor_puts(s_confirm, comm);
		if (monitor_getc(comm) == 'y') {
			monitor_req_app_upload(); 
		}

		break;

	case CTRL_R:
		monitor_puts(s_confirm, comm);
		if (monitor_getc(comm) == 'y') {
			monitor_soft_reset();
			monitor_signal(MONITOR_USER_EVENT1);
		}
		break;

#if (MONITOR_UPLOAD_CONFIG_ENABLE)
	case CTRL_F:
		monitor_puts(s_confirm, comm);
		if (monitor_getc(comm) == 'y') {
			monitor_soft_reset();
			monitor_signal(MONITOR_USER_EVENT3);
		}
		break;
#endif

#endif

#if (MONITOR_APPRESTART_ENABLE)
	case CTRL_Z:
#if (MONITOR_APP_EXEC)
		monitor_req_app_exec(); 
#else
		thinkos_krn_sysrst();
#endif
		break;
#endif
	default:
		return false;
	}

	return true;
}

#if (MONITOR_APP_EXEC)

static void __main_thread_exec(int (* func)(void *), void * arg)
{
	int thread_id = 0;
	struct thinkos_context * ctx;

	DCC_LOG2(LOG_INFO, "__thinkos_thread_ctx_init(func=%p arg=%p)", func, arg);
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

	DCC_LOG(LOG_INFO, "__thinkos_defer_sched()");
	__thinkos_defer_sched();
}

static bool __monitor_app_exec(uintptr_t addr)
{
	uint32_t * signature = (uint32_t *)addr;
	int i;

	for (i = app_magic.hdr.cnt - 1; i >= 0; --i) {
		if (signature[i] != app_magic.rec[i].comp) {
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
#else

#endif

/* Default Monitor Task */
void __attribute__((noreturn)) monitor_task(const struct monitor_comm * comm, 
                                            void * param)
{
#if (MONITOR_OSINFO_ENABLE)
	uint32_t cycref[THINKOS_CTX_CNT];
#endif
	uint32_t sigmask = 0;
	uint8_t buf[4];
	uint8_t * ptr;
	uint32_t sig;
	int cnt;
	bool connected;
	int status;
#if (MONITOR_APP_EXEC)
	struct monitor monitor;

	DCC_LOG(LOG_INFO, "starting monitor...");

	monitor.flags = (uintptr_t)param;
#endif

	/* unmask events */
	sigmask |= (1 << MONITOR_SOFTRST);
#if (MONITOR_FAULT_ENABLE)
	sigmask |= (1 << MONITOR_THREAD_FAULT);
#endif
#if (MONITOR_EXCEPTION_ENABLE)
	sigmask |= (1 << MONITOR_KRN_ABORT);
	sigmask |= (1 << MONITOR_KRN_FAULT);
#endif
	sigmask |= (1 << MONITOR_COMM_RCV);
	sigmask |= (1 << MONITOR_COMM_CTL);
	sigmask |= (1 << MONITOR_TX_PIPE);
	sigmask |= (1 << MONITOR_APP_EXEC);
	sigmask |= (1 << MONITOR_APP_UPLOAD);
	sigmask |= (1 << MONITOR_USER_EVENT1);
	sigmask |= (1 << MONITOR_USER_EVENT2);
	sigmask |= (1 << MONITOR_USER_EVENT3);
#if (MONITOR_OSINFO_ENABLE)
	sigmask |= (1 << MONITOR_USER_EVENT4);
#endif

#if (MONITOR_APP_EXEC)
	if (!(monitor.flags & MONITOR_AUTOBOOT)) {
		monitor_puts(s_hr, comm);
		monitor_puts(s_version, comm);
	}

	if (monitor.flags & MONITOR_AUTOBOOT) {
		monitor_req_app_exec(); 
	}
#endif

	for(;;) {
		switch ((sig = monitor_select(sigmask))) {

		case MONITOR_SOFTRST:
			monitor_clear(MONITOR_SOFTRST);
			board_reset();
			monitor_puts("\r\n", comm);
			goto is_connected;

#if (MONITOR_FAULT_ENABLE)
		case MONITOR_THREAD_FAULT:
			{
			int32_t errno;
			int32_t thread;
			
			DCC_LOG(LOG_ERROR, "THREAD_FAULT!!!");

			monitor_clear(MONITOR_THREAD_FAULT);
			thread = monitor_thread_break_get(&errno);

			monitor_puts("!ERR: thread=", comm);
			monitor_comm_send_uint(thread, 5, comm);
			monitor_puts(" rrno=", comm);
			monitor_comm_send_uint(errno, 5, comm);
#if (MONITOR_OSINFO_ENABLE)
			monitor_signal(MONITOR_USER_EVENT4);
#endif
			}
			break;
#endif
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
			yflash(BOOTLOADER_BLOCK_OFFS, BOOTLOADER_BLOCK_SIZE, 
				   &bootloader_magic);
			break;


#if (MONITOR_UPLOAD_CONFIG_ENABLE)
		case MONITOR_USER_EVENT3:
			monitor_clear(MONITOR_USER_EVENT3);
			yflash(CONFIG_BLOCK_OFFS, CONFIG_BLOCK_SIZE, NULL);
			break;
#endif
#endif

#if (MONITOR_OSINFO_ENABLE)
		case MONITOR_USER_EVENT4:
			monitor_clear(MONITOR_USER_EVENT4);
			print_osinfo(comm, cycref);
			break;
#endif

#if (MONITOR_APP_EXEC)
		case MONITOR_APP_EXEC:
			monitor_clear(MONITOR_APP_EXEC);
			if (!__monitor_app_exec(APPLICATION_START_ADDR)) {
				monitor_puts("!ERR: app\r\n", comm);
			}
			break;
#endif

		case MONITOR_COMM_RCV:

			DCC_LOG(LOG_INFO, "COMM_RCV...");

			/* receive from the COMM driver one byte at the time */
			if ((cnt = monitor_comm_recv(comm, buf, 1)) > 0) {
				int c = buf[0];

				DCC_LOG1(LOG_INFO, "COMM_RCV: c=0x%02x", c);
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

			} else {
				DCC_LOG1(LOG_INFO, "monitor_comm_recv() = %d", cnt);
			}
			break;
#if 0
		case MONITOR_COMM_BRK:
			monitor_comm_break_ack(comm);
			break;
#endif
		case MONITOR_COMM_CTL:
			monitor_clear(MONITOR_COMM_CTL);

is_connected:
			status = monitor_comm_status_get(comm);
			if (status & COMM_ST_CONNECTED) {
				DCC_LOG(LOG_INFO, "connected....");
			}
			connected = (status & COMM_ST_CONNECTED) ? true : false;
			thinkos_krn_console_connect_set(connected);

			sigmask &= ~((1 << MONITOR_COMM_EOT) | 
						 (1 << MONITOR_COMM_RCV) |
						 (1 << MONITOR_RX_PIPE));
			sigmask |= (1 << MONITOR_TX_PIPE);

			if (connected) {
				sigmask |= ((1 << MONITOR_COMM_EOT) |
							(1 << MONITOR_COMM_RCV));
			}


			DCC_LOG1(LOG_MSG, "sigmask=%08x", sigmask);
			break;

		case MONITOR_COMM_EOT:
			/* FALLTHROUGH */
		case MONITOR_TX_PIPE:
			if ((cnt = thinkos_console_tx_pipe_ptr(&ptr)) > 0) {
				int n;
				DCC_LOG1(LOG_INFO, "TX Pipe: cnt=%d, send...", cnt);
				if ((n = monitor_comm_send(comm, ptr, cnt)) > 0) {
					thinkos_console_tx_pipe_commit(n);
					if (n == cnt) {
						/* Wait for TX_PIPE */
						sigmask |= (1 << MONITOR_TX_PIPE);
						sigmask &= ~(1 << MONITOR_COMM_EOT);
					} else {
						/* Wait for COMM_EOT */
						sigmask |= (1 << MONITOR_COMM_EOT);
						sigmask &= ~(1 << MONITOR_TX_PIPE);
					}
				} else {
					/* Wait for COMM_EOT */
					sigmask |= (1 << MONITOR_COMM_EOT);
					sigmask &=  ~(1 << MONITOR_TX_PIPE);
				}
			} else {
				/* Wait for TX_PIPE */
				DCC_LOG1(LOG_INFO, "TX Pipe: cnt=%d, wait....", cnt);
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
					if (n == cnt) {
						/* Wait for RX_PIPE */
						DCC_LOG(LOG_INFO, 
								"RX_PIPE: Wait for RX_PIPE && COMM_RECV");
						sigmask |= (1 << MONITOR_COMM_RCV);
						sigmask &=  ~(1 << MONITOR_RX_PIPE);
					} else {
						DCC_LOG(LOG_INFO, "RX_PIPE: Wait for COMM_RECV");
						/* Wait for COMM_RECV */
						sigmask |= (1 << MONITOR_COMM_RCV);
						sigmask &=  ~(1 << MONITOR_RX_PIPE);
					}
				} else {
					/* Wait for COMM_RECV */
					DCC_LOG(LOG_ERROR, "RX_PIPE: Wait for COMM_RECV");
					sigmask |= (1 << MONITOR_COMM_RCV);
					sigmask &=  ~(1 << MONITOR_RX_PIPE);
				}
			} else {
				DCC_LOG1(LOG_ERROR, "RX_PIPE: RX, cnt=%d", cnt);
				/* Wait for RX_PIPE */
				sigmask &= ~(1 << MONITOR_COMM_RCV);
				sigmask |= (1 << MONITOR_RX_PIPE);
			}
			break;

		}
	}
}
