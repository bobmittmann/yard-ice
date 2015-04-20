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

enum dmon_ev_no {
	DMON_COMM_RCV = 0,
	DMON_COMM_EOT = 1,
	DMON_COMM_CTL = 2,
	DMON_THREAD_FAULT = 16,
	DMON_BUSFAULT = 30,
	DMON_START    = 31
};

struct dmon_comm;

struct thinkos_dmon {
	struct dmon_comm * comm;
	uint32_t * ctx;
	volatile uint32_t events;
	volatile uint32_t mask;
	void (* task)(struct thinkos_dmon * dmon, struct dmon_comm * comm);
	struct {	
		struct thinkos_context ctx;
		uint32_t ret;
		uint32_t sp;
		int thread;
	} except;
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

void thinkos_dmon_init(void * comm, void (* task)(struct thinkos_dmon * , 
												  struct dmon_comm * ));

int dmon_comm_send(void * comm, const void * buf, unsigned int len);

int dmon_comm_recv(void * comm, void * buf, unsigned int len);

int dmon_comm_connect(void * drv);

void dmon_unmask(int event);

void dmon_mask(int event);

uint32_t dmon_select(uint32_t watch);

int dmon_wait(int event);

#ifdef __cplusplus
}
#endif

#endif /* __THINKOS_DMON_H__ */

