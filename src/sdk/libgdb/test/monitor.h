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

#ifndef __MONITOR_H__
#define __MONITOR_H__

#define __THINKOS_SYS__
#include <thinkos_sys.h>
#define __THINKOS_IRQ__
#include <thinkos_irq.h>

enum monitor_ev_no {
	MON_COMM_RCV = 0,
	MON_COMM_EOT
};

#define MON_RX_BUF_LEN 64

struct thinkos_monitor {
	uint32_t events;
	struct {
		uint8_t rx_buf[MON_RX_BUF_LEN];
		void * drv;
	} comm;
} monitor_rt;

static inline void monitor_signal(int ev) {
	struct cm3_dcb * dcb = CM3_DCB;
	__bit_mem_wr(&monitor_rt.events, ev, 1);  
	dcb->demcr |= DCB_DEMCR_MON_PEND;
	asm volatile ("isb\n" :  :  : );
}

#ifdef __cplusplus
extern "C" {
#endif

void monitor_init(void);

int monitor_comm_send(void * drv, const void * buf, unsigned int len);

int monitor_comm_recv(void * drv, void * buf, unsigned int len);

#ifdef __cplusplus
}
#endif

#endif /* __MONITOR_H__ */

