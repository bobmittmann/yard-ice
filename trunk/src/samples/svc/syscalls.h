/*
 * File:	asm/cm3/syscalls.h
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

#ifndef __ASM_SYSCALLS_H__
#define __ASM_SYSCALLS_H__

#ifndef __ASSEMBLER__

#define __SVC_CALL(N) ( { register int ret asm("r0"); \
	asm volatile ("svc " #N "\n" : "=r"(ret) : : "r12" ); \
	ret; } )

#define __SVC_CALL1(N, A1) ( { register int ret asm("r0"); \
	register int r0 asm("r0") = (int)A1; \
	asm volatile ("svc " #N "\n" : "=r"(ret) : "0"(r0) : "r12" ); \
	ret; } )

#define __SVC_CALL2(N, A1, A2) ( { register int ret0 asm("r0"); \
	register int ret1 asm("r1"); \
	register int r0 asm("r0") = (int)A1; \
	register int r1 asm("r1") = (int)A2; \
	asm volatile ("svc " #N "\n" : "=r"(ret0), "=r"(ret1) : \
				  "0"(r0), "1"(r1) : "r12" ); \
	ret0; } )

#define __SVC_CALL3(N, A1, A2, A3) ( { register int ret0 asm("r0"); \
	register int ret1 asm("r1"); \
	register int ret2 asm("r2"); \
	register int r0 asm("r0") = (int)A1; \
	register int r1 asm("r1") = (int)A2; \
	register int r2 asm("r2") = (int)A3; \
	asm volatile ("svc " #N "\n" : "=r"(ret0), "=r"(ret1), "=r"(ret2) : \
				  "0"(r0), "1"(r1), "2"(r2) : "r12" ); \
	ret0; } )

#define __SVC_CALL4(N, A1, A2, A3, A4) ( { register int ret0 asm("r0"); \
	register int ret1 asm("r1"); \
	register int ret2 asm("r2"); \
	register int r0 asm("r0") = (int)A1; \
	register int r1 asm("r1") = (int)A2; \
	register int r2 asm("r2") = (int)A3; \
	register int r3 asm("r3") = (int)A4; \
	asm volatile ("svc " #N "\n" : "=r"(ret0), "=r"(ret1), "=r"(ret2) : \
				  "0"(r0), "1"(r1), "2"(r2), "3"(r3) : "r12" ); \
	ret0; } )


#define __SVC(N) __SVC_CALL(N)
#define __SVC1(N, A1) __SVC_CALL1(N, (A1))
#define __SVC2(N, A1, A2) __SVC_CALL2(N, (A1), (A2))
#define __SVC3(N, A1, A2, A3) __SVC_CALL3(N, (A1), (A2), (A3))
#define __SVC4(N, A1, A2, A3, A4) __SVC_CALL3(N, (A1), (A2), (A3), (A4))

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* __ASSEMBLER__ */

#endif /* __ASM_SYSCALLS_H__ */

