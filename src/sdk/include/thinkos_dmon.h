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

#ifdef CONFIG_H
#include "config.h"
#endif

#define __THINKOS_SYS__
#include <thinkos_sys.h>
#define __THINKOS_IRQ__
#include <thinkos_irq.h>

#include <sys/usb-dev.h>

enum dmon_ev_no {
	DMON_COMM_RCV = 0,
	DMON_COMM_EOT = 1,
	DMON_COMM_CTL = 2,

	DMON_RX_PIPE  = 3,
	DMON_TX_PIPE  = 4,
	DMON_ALARM    = 5,

	DMON_THREAD_STEP  = 8,
	DMON_THREAD_FAULT = 12,
	DMON_IDLE     = 29,
	DMON_EXCEPT   = 30,
	DMON_RESET    = 31
};

struct dmon_comm;

struct thinkos_dmon {
	struct dmon_comm * comm;
	uint32_t * ctx;
	volatile uint32_t mask;
	volatile uint32_t events;
	volatile uint32_t req;
	void (* task)(struct dmon_comm * comm);
};

extern struct thinkos_dmon thinkos_dmon_rt;

static inline void dmon_signal(int ev) {
	struct cm3_dcb * dcb = CM3_DCB;
	__bit_mem_wr((uint32_t *)&thinkos_dmon_rt.events, ev, 1);  
	dcb->demcr |= DCB_DEMCR_MON_PEND;
	asm volatile ("isb\n" :  :  : );
}

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

#ifdef __cplusplus
}
#endif

#endif /* __THINKOS_DMON_H__ */

