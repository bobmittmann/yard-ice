/* 
 * Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
 * 
 * This file is part of the YARD-ICE.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You can receive a copy of the GNU Lesser General Public License from 
 * http://www.gnu.org/
 */

/** 
 * @file arch/arm.h
 * @brief ARM CPU macros and definitions 
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */

#ifndef __ARM_H__
#define __ARM_H__

#define ARM_MODE_USER           0x10
#define ARM_MODE_SUPERVISOR     0x13
#define ARM_MODE_ABORT          0x17
#define ARM_MODE_UNDEF          0x1b
#define ARM_MODE_IRQ            0x12
#define ARM_MODE_FIQ            0x11
#define ARM_MODE_SYSTEM         0x1f
#define ARM_MODE_MASK           (0xffffffe0)

#define ARM_IRQ_BIT             7
#define ARM_DISABLE_IRQ         (1 << ARM_IRQ_BIT)

#define ARM_FIQ_BIT             6
#define ARM_DISABLE_FIQ         (1 << ARM_FIQ_BIT)

#define ARM_THUMB_BIT           5
#define ARM_STATE_THUMB         (1 << ARM_THUMB_BIT)

#define ARM_ENABLE_THUMB        ARM_STATE_THUMB
#define ARM_THUMB               ARM_STATE_THUMB

#define ARM_UNDEFINED_INSTRUCTION 1
#define ARM_SOFTWARE_INTERRUPT 2
#define ARM_PREFETCH_ABORT 3
#define ARM_DATA_ABORT 4
#define ARM_INVALID_INSTRUCTION 5
#define ARM_INTERRUPT_REQUEST 6
#define ARM_FAST_INTERRUPT_REQUEST 7

#define ARM_CONTEXT_SL (10 * 4)
#define ARM_CONTEXT_FP (11 * 4)
#define ARM_CONTEXT_IP (12 * 4)
#define ARM_CONTEXT_SP (13 * 4)
#define ARM_CONTEXT_LR (14 * 4)
#define ARM_CONTEXT_PC (15 * 4)
#define ARM_CONTEXT_CPSR (16 * 4)


/* mmu off/on */
#define C1_MMU          (1<<0)		
/* alignment faults off/on */
#define C1_ALIGN        (1<<1)		
/* dcache off/on */
#define C1_DC           (1<<2)		
/* big endian off/on */
#define C1_BIG_ENDIAN   (1<<7)		
/* system protection */
#define C1_SYS_PROT     (1<<8)		
/* ROM protection */
#define C1_ROM_PROT     (1<<9)		
/* icache off/on */
#define C1_IC           (1<<12)		
/* location of vectors: low/high addresses */
#define C1_HIGH_VECTORS (1<<13)		


#ifndef __ASSEMBLER__

#include <stdint.h>

#define __GET_SP(SP) __asm__ __volatile__ ("mov %0, sp\n": "=r" (SP) : )
#define __GET_LR(LR) __asm__ __volatile__ ("mov %0, lr\n": "=r" (LR) : )
#define __GET_PC(PC) __asm__ __volatile__ ("mov %0, pc\n": "=r" (PC) : )
#define __GET_FP(PC) __asm__ __volatile__ ("mov %0, fp\n": "=r" (PC) : )

#ifdef __thumb__
#define __GET_CPSR(CPSR) __asm__ __volatile__ (".code  16\n"\
	".thumb_func\n"\
	".align 0\n"\
	"bx     pc\n"\
	"nop\n"\
	".code  32\n"\
	"mrs    %0, cpsr\n"\
	"orr    %0, %0, #0x20\n"\
	"add    r1, pc, #1\n"\
	"bx     r1\n"\
	".code  16\n"\
	".align 1\n"\
	: "=r" (CPSR): : "r1" )
#else
#define __GET_CPSR(CPSR) __asm__ __volatile__ ("mrs %0, cpsr\n": "=r" (CPSR) : )
#endif

#ifdef __thumb__
#define __GET_SPSR(SPSR) __asm__ __volatile__ (".code  16\n"\
	".thumb_func\n"\
	".align 0\n"\
	"bx     pc\n"\
	"nop\n"\
	".code  32\n"\
	"mrs    %0, spsr\n"\
	"orr    %0, %0, #0x20\n"\
	"add    r1, pc, #1\n"\
	"bx     r1\n"\
	".code  16\n"\
	".align 1\n"\
	: "=r" (SPSR): : "r1" )
#else
#define __GET_SPSR(SPSR) __asm__ __volatile__ ("mrs %0, spsr\n": "=r" (SPSR) : )
#endif

#ifdef __thumb__
#define __PREFETCH_ABORT() __asm__ __volatile__ (\
	"mov r0, #1\nror r0, r0\n"\
	"mov pc, r0\n": : : "r0" )
#else
#define __PREFETCH_ABORT() __asm__ __volatile__ (\
	"mov r0, #0x80000000\n"\
	"mov pc, r0\n": : : "r0" )
#endif

#ifdef __thumb__
#define __DATA_ABORT() __asm__ __volatile__ (\
	"mov r0, #1\nror r0, r0\n"\
	"ldr r0, [r0]\n": : : "r0" )
#else
#define __DATA_ABORT() __asm__ __volatile__ (\
	"mov r0, #0x80000000\n"\
	"ldr r0, [r0]\n": : : "r0" )
#endif

#ifdef __thumb__
#define __UNDEF() __asm__ __volatile__ (".thumb\n\t.word 0xe800e800\n" : : )
#else
#define __UNDEF() __asm__ __volatile__ (".arm\n\t.word 0xe6000010\n" : : )
#endif

/* A prosaic nop */
#ifdef __thumb__
#define __NOP() __asm__ __volatile__ (".thumb\n\tnop\n" : : )
#else
#define __NOP() __asm__ __volatile__ (".arm\n\tnop\n" : : )
#endif

/* This macro define the software interrupts in a distinct way
   for ARM and THUMB code. The ARM swi number is shifted to the
   upper 16bits. As the THUMB swi is limitted to 8bits this is not 
   a great deal. The advantage of this declaration is that 
   the following simple sequence can be used to request a system call:
  
   stmdb   sp!, {r12, lr} ; save the lr and an auxiliar register
   ldrb    r12, [lr, #-2] ; load the swi number
   mov     lr, #SYS_CALL_TAB ; get the syscall table pointer
   ldr     r12, [lr, r12, lsl #2] ; get the syscall handler
   mov     lr, pc ; save the return adress
   bx      r12 ; call the function (interwork)
   ldmia   sp!, {r12, pc}^ ; restore and return
*/

#ifdef __thumb__
#define __SWI(N) __asm__ __volatile__ (".thumb\n\tswi " #N "\n": : )
#else
#define __SWI(N) __asm__ __volatile__ (".arm\n\tswi " #N " << 16\n": : )
#endif

extern inline uint32_t __sp(void) {
	register uint32_t __ret;
	__GET_SP(__ret);
	return __ret;
}

extern inline uint32_t __lr(void) {
	register uint32_t __ret;
	__GET_LR(__ret);
	return __ret;
}

extern inline uint32_t __pc(void) {
	register uint32_t __ret;
	__GET_PC(__ret);
	return __ret;
}

extern inline uint32_t __fp(void) {
	register uint32_t __ret;
	__GET_FP(__ret);
	return __ret;
}

extern inline uint32_t __cpsr(void) {
	register uint32_t __ret;
	__GET_CPSR(__ret);
	return __ret;
}

extern inline uint32_t __spsr(void) {
	register uint32_t __ret;
	__GET_SPSR(__ret);
	return __ret;
}

extern inline void __nop(void) {
	__NOP();
}

/* disable FIQ interrupts, It returns the CPSR. The return value may be 
   saved to call the __set_cpsr() function to restore it. */
extern inline uint32_t __dfiq(void) {
	register uint32_t __res;
	register uint32_t __tmp;
	__asm__ __volatile__ (
#ifdef __thumb__
	".thumb\n"
	".align 0,0x45\n"
	"bx     pc\n"
	"nop\n"
	".arm\n"
#endif
	"mrs    %0, CPSR\n"
    "orr    %1, %0, #0x40\n"
	"msr    CPSR_c, %1\n"
#ifdef __thumb__
	"add    %1, pc, #1\n"
	"bx     %1\n"
	".thumb\n"
	".align 1\n"
#endif
	: "=r"(__res), "=r"(__tmp): );
	return __res;
}

/* disable IRQ interrupts */
extern inline uint32_t __dint(void) {
	register uint32_t __res;
	register uint32_t __tmp;
	__asm__ __volatile__ (
#ifdef __thumb__
	".thumb\n"
	".align 0,0x45\n"
	"bx     pc\n"
	"nop\n"
	".arm\n"
#endif
	"mrs    %0, CPSR\n"
    "orr    %1, %0, #0x80\n"
	"msr    CPSR_c, %1\n"
#ifdef __thumb__
	"add    %1, pc, #1\n"
	"bx     %1\n"
	".thumb\n"
	".align 1\n"
#endif
	: "=r"(__res), "=r"(__tmp): );
	return __res;
}
		
/* enable IRQ interrupts */
extern inline uint32_t __eint(void) {
	register uint32_t __res;
	register uint32_t __tmp;
	__asm__ __volatile__ (
#ifdef __thumb__
	".thumb\n"
	".align 0,0x45\n"
	"bx     pc\n"
	"nop\n"
	".arm\n"
#endif
	"mrs    %0, CPSR \n"
    "bic    %1, %0, #0xc0\n"
	"msr    CPSR_c, %1\n"
#ifdef __thumb__
	"add    %1, pc, #1\n"
	"bx     %1\n"
	".thumb\n"
	".align 1\n"
#endif
	: "=r"(__res), "=r"(__tmp): );
	return __res;
}

/* configure the CPSR */
extern inline void __set_cpsr(uint32_t __cpsr) {
	uint32_t tmp;
	__asm__ __volatile__ (
#ifdef __thumb__
	".thumb\n"
	".align 0,0x45\n"
	"bx     pc\n"
	"nop\n"
	".arm\n"
#endif
	"msr    CPSR_c, %0\n"
#ifdef __thumb__
	"add    %0, pc, #1\n"
	"bx     %0\n"
	".code  16\n"
	".align 1\n"
#endif
	: "=r"(tmp): "0" (__cpsr) );
}

struct arm_context {
	uint32_t r[10];
	uint32_t sl;
	uint32_t fp;
	uint32_t ip;
	uint32_t sp;
	uint32_t lr;
	uint32_t pc;
	uint32_t cpsr;
};

#ifdef __cplusplus
extern "C" {
#endif

uint32_t eint(void);

uint32_t dint(void);

void set_cpsr(uint32_t);

void arm_show_context(struct arm_context * context);

uint32_t arm_cp15_ctrl_set(uint32_t);

uint32_t arm_cp15_ctrl_clr(uint32_t);

void arm_icache_disable(void);
void arm_dcache_disable(void);
void arm_icache_enable(void);
void arm_dcache_enable(void);

#ifdef __cplusplus
}
#endif

#endif /* __ASSEMBLER__ */

#endif /* __ARM_H__ */

