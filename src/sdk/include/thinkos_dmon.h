/* 
 * File:	 board.h
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(c) 2003-2006 BORESTE (www.boreste.com). All Rights Reserved.
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

#ifndef __THINKOS_DMON_H__
#define __THINKOS_DMON_H__

#ifndef __THINKOS_DMON__
#error "Never use <thinkos_dmon.h> directly; include <thinkos.h> instead."
#endif 

#define __THINKOS_SYS__
#include <thinkos_sys.h>
#define __THINKOS_IRQ__
#include <thinkos_irq.h>

#include <sys/usb-dev.h>

enum dbgmon_event {
	DMON_COMM_RCV     = 0,
	DMON_COMM_EOT     = 1,
	DMON_COMM_CTL     = 2,

	DMON_RX_PIPE      = 3,
	DMON_TX_PIPE      = 4,
	DMON_ALARM        = 5,

	DMON_THREAD_STEP  = 8,
	DMON_THREAD_FAULT = 12,
	DMON_BREAKPOINT   = 16,
	DMON_IDLE         = 29,
	DMON_EXCEPT       = 30,
	DMON_RESET        = 31
};

struct dmon_comm;

#define NVIC_IRQ_REGS ((THINKOS_IRQ_MAX + 31) / 32)

struct thinkos_dmon {
	struct dmon_comm * comm;
	uint32_t * ctx;           /* monitor context */
	volatile uint32_t mask;   /* events mask */
	volatile uint32_t events; /* events bitmap */
	uint32_t nvic_ie[NVIC_IRQ_REGS]; /* interrupt state */
	void (* task)(struct dmon_comm * comm);
};

extern struct thinkos_dmon thinkos_dmon_rt;

static inline void dmon_signal(int ev) {
	struct cm3_dcb * dcb = CM3_DCB;
	__bit_mem_wr((uint32_t *)&thinkos_dmon_rt.events, ev, 1);  
	dcb->demcr |= DCB_DEMCR_MON_PEND;
	asm volatile ("isb\n" :  :  : );
}


#define SZ_128   7
#define SZ_256   8
#define SZ_1K   10
#define SZ_2K   11
#define SZ_4K   12
#define SZ_8K   13
#define SZ_16K  14
#define SZ_32K  15
#define SZ_64K  16
#define SZ_128K 17
#define SZ_256K 18
#define SZ_512K 19
#define SZ_1M   20
#define SZ_2M   21
#define SZ_4M   22
#define SZ_8M   23
#define SZ_16M  24
#define SZ_32M  25
#define SZ_64M  26
#define SZ_128M 27
#define SZ_256M 28
#define SZ_512M 29
#define SZ_1G   30
#define SZ_2G   31

#define BLK_RW  (0 << 7)
#define BLK_RO  (1 << 7)

struct blk_desc {
	uint32_t ref;
	uint8_t  opt;
	uint8_t  siz;
	uint16_t cnt;
};

struct mem_desc {
	char name[8];
	struct blk_desc blk[];
};

struct thinkos_board {
	char name[18];

	struct {
		uint8_t minor;
		uint8_t major;
	} hw_ver;

	struct {
		uint8_t minor;
		uint8_t major;
		uint16_t build;
	} sw_ver;

	struct {
		const struct mem_desc * ram;
		const struct mem_desc * flash;
	} memory;

	struct {
		uint32_t start_addr;
		uint32_t block_size;
	} application;

	bool (* init)(void);
	void (* softreset)(void);
	bool (* autoboot)(unsigned int tick);
	bool (* configure)(struct dmon_comm *);
	void (* upgrade)(struct dmon_comm *);
	void (* on_appload)(void);
	void (* comm_irqen)(void);
};

extern const struct thinkos_board this_board;

#ifdef __cplusplus
extern "C" {
#endif

void thinkos_dmon_init(void * comm, void (* task)(struct dmon_comm * ));

void dmon_reset(void);

void dmon_exec(void (* task)(struct dmon_comm *));

void dmon_unmask(int sig);

void dmon_mask(int sig);

void dmon_clear(int sig);

uint32_t dmon_select(uint32_t watch);

int dmon_wait(int sig);

int dmon_expect(int sig);

int dmon_sleep(unsigned int ms);

void dmon_alarm(unsigned int ms);

void dmon_alarm_stop(void);

int dmon_wait_idle(void);


int dmon_thread_step(unsigned int id, bool block);


int dmon_comm_send(struct dmon_comm * comm, 
				   const void * buf, unsigned int len);

int dmon_comm_recv(struct dmon_comm * comm, void * buf, unsigned int len);

int dmon_comm_connect(struct dmon_comm * comm);

bool dmon_comm_isconnected(struct dmon_comm * comm);

void dmon_comm_rxflowctrl(struct dmon_comm * comm, bool en);


struct dmon_comm * usb_comm_init(const usb_dev_t * usb);

void console_task(struct dmon_comm * comm);

int dmprintf(struct dmon_comm * comm, const char *fmt, ... );

void dmon_print_thread(struct dmon_comm * comm, int id);

void dmon_print_context(struct dmon_comm * comm, 
						const struct thinkos_context * ctx, 
						uint32_t sp);

void dmon_print_exception(struct dmon_comm * comm, 
						  struct thinkos_except * xcpt);

int dmon_print_osinfo(struct dmon_comm * comm);

void dmon_print_alloc(struct dmon_comm * comm);

void dmon_print_stack_usage(struct dmon_comm * comm);

bool dmon_app_exec(uint32_t addr, bool paused);

bool dmon_app_erase(struct dmon_comm * comm, 
					uint32_t addr, unsigned int size);

int dmon_ymodem_flash(struct dmon_comm * comm,
					  uint32_t addr, unsigned int size);

bool dmon_app_suspend(void);

bool dmon_app_continue(void);

void dmon_soft_reset(struct dmon_comm * comm);

bool dmon_breakpoint_set(uint32_t addr, uint32_t size);

bool dmon_breakpoint_clear(uint32_t addr, uint32_t size);

void dmon_breakpoint_clear_all(void);

bool dmon_watchpoint_set(uint32_t addr, uint32_t size, int access);

bool dmon_watchpoint_clear(uint32_t addr, uint32_t size);

void dmon_watchpoint_clear_all(void);


int dmputc(int c, struct dmon_comm * comm);

int dmputs(char * s, struct dmon_comm * comm);

int dmgets(char * s, int size, struct dmon_comm * comm);

int dmscanf(struct dmon_comm * comm, const char *fmt, ... );

#ifdef __cplusplus
}
#endif

#endif /* __THINKOS_DMON_H__ */

