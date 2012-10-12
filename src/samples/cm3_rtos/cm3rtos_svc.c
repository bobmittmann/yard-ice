/* 
 * File:	 dac-test.c
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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arch/cortex-m3.h>
#include <sys/serial.h>
#include <sys/delay.h>

#include <cm3rtos_svc.h>
#define __CM3RTOS_SYS__
#include <cm3rtos_sys.h>

int cm3rtos_join_svc(unsigned int idx);

int cm3rtos_cancel_svc(unsigned int idx, int code);

int cm3rtos_resume_svc(unsigned int idx);

int cm3rtos_pause_svc(unsigned int idx);

int cm3rtos_thread_create_svc(struct cm3rtos_thread_init * init);

int cm3rtos_thread_id_svc(void);

void cm3rtos_sleep_svc(unsigned int ms);

int cm3rtos_mutex_alloc_svc(void);

int cm3rtos_mutex_free_svc(int mutex);

int cm3rtos_mutex_lock_svc(int mutex);

int cm3rtos_mutex_unlock_svc(int mutex);

void cm3_svc_handler(void)
{
	uint32_t * arg;
	uint8_t * pc;
	int svc;

	/* get a pointer to the process' stack */
	arg = (uint32_t * )cm3_psp_get();
	/* get PC value */
	pc = (uint8_t *)arg[6];
	/* get the immediate data from instruction */
	svc = pc[-2];

	switch (svc) {
	case CM3RTOS_THREAD_ID:
		arg[0] = cm3rtos_thread_id_svc();
		return;

	case CM3RTOS_YIELD:
		__cm3rtos_defer_sched();
		return;

	case CM3RTOS_THREAD_CREATE:
		arg[0] = cm3rtos_thread_create_svc((struct cm3rtos_thread_init *)arg);
		return;

#if CM3RTOS_ENABLE_PAUSE
	case CM3RTOS_PAUSE:
		arg[0] = cm3rtos_pause_svc(arg[0]);
		return;

	case CM3RTOS_RESUME:
		arg[0] = cm3rtos_resume_svc(arg[0]);
		return;
#endif

#if CM3RTOS_ENABLE_CANCEL
	case CM3RTOS_CANCEL:
		arg[0] = cm3rtos_cancel_svc(arg[0], arg[1]);
		return;
#endif

#if CM3RTOS_ENABLE_JOIN
	case CM3RTOS_JOIN:
		arg[0] = cm3rtos_join_svc(arg[0]);
		return;
#endif

	case CM3RTOS_SLEEP:
		cm3rtos_sleep_svc(arg[0]);
		break;

#if CM3RTOS_MUTEX_MAX > 0
#if CM3RTOS_ENABLE_MUTEX_ALLOC
	case CM3RTOS_MUTEX_ALLOC:
		arg[0] = cm3rtos_mutex_alloc_svc();
		break;

	case CM3RTOS_MUTEX_FREE:
		cm3rtos_mutex_free_svc(arg[0]);
		break;
#endif /* CM3RTOS_MUTEX_ALLOC */

	case CM3RTOS_MUTEX_LOCK:
		cm3rtos_mutex_lock_svc(arg[0]);
		break;

	case CM3RTOS_MUTEX_UNLOCK:
		cm3rtos_mutex_unlock_svc(arg[0]);
		break;
#endif /* CM3RTOS_MUTEX_MAX > 0 */

	case CM3RTOS_WAIT:
		/* insert into the wait queue */
//		bmp_bit_set(&cm3rtos_rt.wq[arg[0]], cm3rtos_rt.active->idx);  
//		bmp_bit_set(&cm3rtos_rt.wq[arg[0]], cm3rtos_rt.active >> 1);  
//		cm3rtos_defer_sched();
		break;

//	case CM3RTOS_SIGNAL:
//		break;
	default:
		arg[0] = -1;
	}
}

