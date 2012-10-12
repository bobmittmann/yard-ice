/*
 * File:     cm3rtos.h
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

#ifndef __CM3RTOS_H__
#define __CM3RTOS_H__

#include <cm3rtos_svc.h>

#define CM3RTOS_ERROR -1
#define CM3RTOS_TIMEDOUT -2

#define CM3RTOS_OPT_PRIORITY(VAL) ((VAL) & 0xff)
#define CM3RTOS_OPT_ID(VAL) (((VAL) & 0x07f) << 8)
#define CM3RTOS_OPT_PAUSED (1 << 16)

#ifndef __ASSEMBLER__

#ifdef __cplusplus
extern "C" {
#endif

static inline void cm3rtos_yield(void)  {
	CM3_SCB->icsr = SCB_ICSR_PENDSVSET; /* PendSV rise */
	asm volatile ("dsb\n"); /* Data synchronization barrier */
}


int cm3rtos_init(unsigned int opt);

int cm3rtos_thread_create(int (* task)(void *), 
						  void * arg, void * stack_ptr,
						  unsigned int stack_size,
						  unsigned int opt);

int cm3rtos_cancel(unsigned int thread_id, int code);

int cm3rtos_join(unsigned int thread_id);

int cm3rtos_pause(unsigned int thread_id);

int cm3rtos_resume(unsigned int thread_id);

void cm3rtos_sleep(unsigned int ms);

int cm3rtos_thread_id(void);

void cm3rtos_yield(void);

uint32_t cm3rtos_idle_val(void);


#ifdef __cplusplus
}
#endif

#endif /* __ASSEMBLER__ */

#endif /* __CM3RTOS_H__ */

