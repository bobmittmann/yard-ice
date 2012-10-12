/*
 * File:     cm3rtos_svc.h
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(C) 2012 Bob Mittmann. All Rights Reserved.
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

#ifndef __CM3RTOS_SVC_H__
#define __CM3RTOS_SVC_H__

/* -------------------------------------------------------------------------- 
 * Service numbers
 * --------------------------------------------------------------------------*/

#define CM3RTOS_THREAD_ID 0
#define CM3RTOS_YIELD 1

#define CM3RTOS_THREAD_CREATE 2
#define CM3RTOS_PAUSE 3
#define CM3RTOS_RESUME 4
#define CM3RTOS_CANCEL 5
#define CM3RTOS_JOIN 6

#define CM3RTOS_SLEEP 7

#define CM3RTOS_MUTEX_ALLOC 8
#define CM3RTOS_MUTEX_FREE 9
#define CM3RTOS_MUTEX_LOCK 10
#define CM3RTOS_MUTEX_UNLOCK 11

#define CM3RTOS_WAIT 12
#define CM3RTOS_SIGNAL 13

#ifndef __ASSEMBLER__

/* -------------------------------------------------------------------------- 
 * C service call macros 
 * --------------------------------------------------------------------------*/

#define __SVC_CALL(N) ( { register int ret asm("r0"); \
	asm volatile ("svc " #N "\n" : "=r"(ret) : : ); \
	ret; } )

#define __SVC_CALL1(N, A1) ( { register int ret asm("r0"); \
	register int r0 asm("r0") = (int)A1; \
	asm volatile ("svc " #N "\n" : "=r"(ret) : "0"(r0) : ); \
	ret; } )

#define __SVC_CALL2(N, A1, A2) ( { register int ret asm("r0"); \
	register int r0 asm("r0") = (int)A1; \
	register int r1 asm("r1") = (int)A2; \
	asm volatile ("svc " #N "\n" : "=r"(ret) : \
				  "0"(r0), "r"(r1) : ); \
	ret; } )

#define __SVC_CALL3(N, A1, A2, A3) ( { register int ret0 asm("r0"); \
	register int r0 asm("r0") = (int)A1; \
	register int r1 asm("r1") = (int)A2; \
	register int r2 asm("r2") = (int)A3; \
	asm volatile ("svc " #N "\n" : "=r"(ret) : \
				  "0"(r0), "r"(r1), "r"(r2) : ); \
	ret; } )

#define __SVC_CALL4(N, A1, A2, A3, A4) ( { register int ret asm("r0"); \
	register int r0 asm("r0") = (int)A1; \
	register int r1 asm("r1") = (int)A2; \
	register int r2 asm("r2") = (int)A3; \
	register int r3 asm("r3") = (int)A4; \
	asm volatile ("svc " #N "\n" : "=r"(ret) : \
				  "0"(r0), "r"(r1), "r"(r2), "r"(r3) : ); \
	ret; } )

#define __SVC_CALL5(N, A1, A2, A3, A4, A5) ( { register int ret asm("r0"); \
	register int r0 asm("r0") = (int)A1; \
	register int r1 asm("r1") = (int)A2; \
	register int r2 asm("r2") = (int)A3; \
	register int r3 asm("r3") = (int)A4; \
	register int r12 asm("r12") = (int)A5; \
	asm volatile ("svc " #N "\n" : "=r"(ret) : \
				  "0"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r12) : ); \
	ret; } )


#define CM3RTOS_SVC(N) __SVC_CALL(N)

#define CM3RTOS_SVC1(N, A1) __SVC_CALL1(N, (A1))

#define CM3RTOS_SVC2(N, A1, A2) __SVC_CALL2(N, (A1), (A2))

#define CM3RTOS_SVC3(N, A1, A2, A3) __SVC_CALL3(N, (A1), (A2), (A3))

#define CM3RTOS_SVC4(N, A1, A2, A3, A4) __SVC_CALL4(N, (A1), (A2), (A3), (A4))

#define CM3RTOS_SVC5(N, A1, A2, A3, A4, A5) __SVC_CALL5(N, (A1), (A2), \
														(A3), (A4), (A5))


#ifdef __cplusplus
extern "C" {
#endif

static inline int cm3rtos_thread_id(void)  {
	return CM3RTOS_SVC(CM3RTOS_THREAD_ID);
}

static inline int cm3rtos_thread_create(int (* task)(void *), 
										void * arg, void * stack_ptr,
										unsigned int stack_size,
										unsigned int opt) {
	return CM3RTOS_SVC5(CM3RTOS_THREAD_CREATE, task, arg, 
						stack_ptr, stack_size, opt);
}

static inline int cm3rtos_cancel(unsigned int id, int code)  {
	return CM3RTOS_SVC2(CM3RTOS_CANCEL, id, code);
}

static inline void cm3rtos_sleep(unsigned int ms)  {
	CM3RTOS_SVC1(CM3RTOS_SLEEP, ms);
}

static inline int cm3rtos_join(unsigned int id)  {
	return CM3RTOS_SVC1(CM3RTOS_JOIN, id);
}

static inline int cm3rtos_pause(unsigned int id)  {
	return CM3RTOS_SVC1(CM3RTOS_PAUSE, id);
}

static inline int cm3rtos_resume(unsigned int id)  {
	return CM3RTOS_SVC1(CM3RTOS_RESUME, id);
}

#ifdef __cplusplus
}
#endif

#endif /* __ASSEMBLER__ */

#endif /* __CM3RTOS_SVC_H__ */

