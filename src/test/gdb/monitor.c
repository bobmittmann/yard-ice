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
#include <dis-asm.h>
#include <sys/dcclog.h>
#include <gdb.h>

#include "monitor.h"

void monitor_on_fault(struct dmon_comm * comm);
void monitor_dump(struct dmon_comm * comm);
void monitor_pause_all(struct dmon_comm * comm);
void monitor_resume_all(struct dmon_comm * comm);
int monitor_print_insn(struct dmon_comm * comm, uint32_t addr);

void monitor_ymodem_recv(struct dmon_comm * comm)
{
	dmon_soft_reset(comm);
	if (dmon_app_load_ymodem(comm) < 0) {
		dmprintf(comm, "YMODEM fail!\r\n");
		return;
	}	

	dmon_app_exec(false);
}

static int thread_id = -1;

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
	monitor_print_insn(comm, ctx->pc);
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

const char monitor_banner[] = "\r\n\r\n"
"-------------------------------------------------------------------------\r\n"
"- ThinkOS Monitor\r\n"
"-------------------------------------------------------------------------\r\n"
"\r\n";

static const char monitor_menu[] = "\r\n"
	 "-- ThinkOS Monitor Commands ---------\r\n"
	 " Ctrl+V - Show help\r\n"
	 " Ctrl+Q - Restart monitor\r\n"
	 " Ctrl+R - Resume all threads\r\n"
	 " Ctrl+P - Pause all threads\r\n"
	 " Ctrl+X - Show exception\r\n"
	 " Ctrl+S - Thread Step\r\n"
	 " Ctrl+T - Comm test\r\n"
	 " Ctrl+I - ThinkOS info\r\n"
	 " Ctrl+N - Select Next Thread\r\n"
	 "-------------------------------------\r\n\r\n";

static void show_help(struct dmon_comm * comm)
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
			dmon_print_osinfo(comm);
			break;
		case CTRL_N:
		case 'n':
			thread_id = __thinkos_thread_getnext(thread_id);
			if (thread_id == - 1)
				thread_id = __thinkos_thread_getnext(thread_id);
			dmprintf(comm, "Current thread = %d\r\n", thread_id);
			dmon_print_thread(comm, thread_id);
			break;
		case CTRL_X:
			monitor_on_fault(comm);
			break;
		case CTRL_S:
		case 's':
			monitor_step_assync(comm);
			break;
		case CTRL_D:
			monitor_dump(comm);
			break;
		case CTRL_V:
			show_help(comm);
			break;
		case 'i':
			thinkos_debug_step_i(4);
			break;
		case CTRL_U:
			dmon_print_stack_usage(comm);
			break;
		case CTRL_Y:
			monitor_ymodem_recv(comm);
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

void monitor_init(void)
{
	struct dmon_comm * comm;
	
	DCC_LOG(LOG_TRACE, "..... !!!!! ......");

	comm = usb_comm_init(&stm32f_otg_fs_dev);

	thinkos_console_init();

//	gdb_init(monitor_task);

//	thinkos_dmon_init(comm, console_task);

	thinkos_dmon_init(comm, monitor_task);
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

int monitor_print_insn(struct dmon_comm * comm, uint32_t addr)
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
