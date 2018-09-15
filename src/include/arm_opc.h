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
 * @file arm_opc.h
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#ifndef __ARM_OPC_H__
#define __ARM_OPC_H__


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

/* CP15 register fields */

#define C0_ISIZE(CTYPE)  ((CTYPE) & 0xfff) 
#define C0_DSIZE(CTYPE)  (((CTYPE) >> 12) & 0xfff)

#define CACHE_SIZE(XSIZE) (512 << ((XSIZE >> 6) & 0x0f))

#define C0_TCM_DTCM(TCM)  (((TCM) >> 16) & 1) /* DTCM present */
#define C0_TCM_ITCM(TCM)  (((TCM) >> 0) & 1) /* ITCM present */

/* CP15 C1 Control Register */

/* mmu off/on */
#define C1_MMU          (1<<0)		
/* alignment faults off/on */
#define C1_ALIGN        (1<<1)		
/* dcache off/on */
#define C1_DCACHE       (1<<2)		
/* big endian off/on */
#define C1_BIG_ENDIAN   (1<<7)		
/* system protection */
#define C1_SYS_PROT     (1<<8)		
/* ROM protection */
#define C1_ROM_PROT     (1<<9)		
/* icache off/on */
#define C1_ICACHE       (1<<12)		
/* location of vectors: low/high addresses */
#define C1_HIGH_VECTORS (1<<13)		
/* Round-robin replacement strategy */
#define C1_RR           (1 << 14) 
/* ARMv4 T bit behavior */
#define C1_L4           (1 << 15)
/* Should be one bits */
#define C1_SBO          ((1 << 18) | (1 << 16))

/* ARM flags */
#define ARM_FLAG_N (0x8 << 28)
#define ARM_FLAG_Z (0x4 << 28)
#define ARM_FLAG_C (0x2 << 28)
#define ARM_FLAG_V (0x1 << 28)

/* ARM-state condition execution */
#define ARM_COND_EQ 0x0
#define ARM_COND_NE 0x1
#define ARM_COND_CS 0x2
#define ARM_COND_CC 0x3
#define ARM_COND_MI 0x4
#define ARM_COND_PL 0x5
#define ARM_COND_VS 0x6
#define ARM_COND_VC 0x7
#define ARM_COND_HI 0x8
#define ARM_COND_LS 0x9
#define ARM_COND_GE 0xa
#define ARM_COND_LT 0xb
#define ARM_COND_GT 0xc
#define ARM_COND_LE 0xd
#define ARM_COND_AL 0xe
#define ARM_COND_NV 0xf

/* ARM-state registers definitions and program counter */

#define ARM_R0         0
#define ARM_R1         1
#define ARM_R2         2
#define ARM_R3         3
#define ARM_R4         4
#define ARM_R5         5
#define ARM_R6         6
#define ARM_R7         7
#define ARM_R8         8
#define ARM_R9         9
#define ARM_R10        10
#define ARM_R11        11
#define ARM_R12        12
#define ARM_R13        13
#define ARM_R14        14
#define ARM_R15        15
#define ARM_SP         13
#define ARM_LR         14
#define ARM_PC         15

#define ARM_R8_FIQ     16
#define ARM_R9_FIQ     17
#define ARM_R10_FIQ    18
#define ARM_R11_FIQ    19
#define ARM_R12_FIQ    20
#define ARM_13_FIQ     21
#define ARM_14_FIQ     22
#define ARM_SP_FIQ     21
#define ARM_LR_FIQ     22

#define ARM_13_SVC     23
#define ARM_14_SVC     24
#define ARM_SP_SVC     23
#define ARM_LR_SVC     24

#define ARM_13_SVC     23
#define ARM_14_SVC     24
#define ARM_SP_ABT     25
#define ARM_LR_ABT     26

#define ARM_13_SVC     23
#define ARM_14_SVC     24
#define ARM_SP_IRQ     27
#define ARM_LR_IRQ     28

#define ARM_13_SVC     23
#define ARM_14_SVC     24
#define ARM_SP_UND     29
#define ARM_LR_UND     30

/* ARM-state registers range */
#define ARM_R0_R0      0x0001
#define ARM_R0_R1      0x0003
#define ARM_R0_R2      0x0007
#define ARM_R0_R3      0x000f
#define ARM_R0_R4      0x001f
#define ARM_R0_R5      0x003f
#define ARM_R0_R6      0x007f
#define ARM_R0_R7      0x00ff
#define ARM_R0_R8      0x01ff
#define ARM_R0_R9      0x03ff
#define ARM_R0_R10     0x07ff
#define ARM_R0_R11     0x0fff
#define ARM_R0_R12     0x1fff
#define ARM_R0_R13     0x3fff
#define ARM_R0_R14     0x7fff
#define ARM_R0_LR      0x7fff
#define ARM_R0_PC      0xffff
#define ARM_R1_R14     0x7ffe
#define ARM_R2_R14     0x7ffc
#define ARM_R1_R2      0x0006

/* ARM-state program status registers */
#define ARM_CPSR       16 
#define ARM_SPSR       17

/* ARM opcodes */

#define ARM_NOP 0xe1a08008

#define ARM_SWI(N) (0xef000000 | (N & 0x00ffffff))

//#define ARM_UNDEF 0x60000010

#define ARM_UNDEF ARM_SWI(0x00)
#define ARM_STUFF 0x00000000

#define ARM_MOV_PC_0 0xe3a0f000

#define ARM_MOV(RD, RM) (0xe1a00000 | (((RD) & 0x0f) << 12) | ((RM) & 0x0f))

#define ARM_ADD(RD, RN, IMM) (0xe2800000 | (((RN) & 0x0f) << 16) | \
			(((RD) & 0x0f) << 12) | ((IMM) & 0x00ff))

#define ARM_SUB(RD, RN, IMM) (0xe2400000 | (((RN) & 0x0f) << 16) | \
							  (((RD) & 0x0f) << 12) | ((IMM) & 0x00ff))

#define ARM_AND(RD, RN, RM) (0xe0000000 | (((RN) & 0x0f) << 16) | \
							 (((RD) & 0x0f) << 12) | ((RM) & 0x0f))

#define ARM_EOR(RD, RN, RM) (0xe0400000 | (((RN) & 0x0f) << 16) | \
							 (((RD) & 0x0f) << 12) | ((RM) & 0x0f))

#define ARM_ORR(RD, RN, RM) (0xe1800000 | (((RN) & 0x0f) << 16) | \
							 (((RD) & 0x0f) << 12) | ((RM) & 0x0f))

#define ARM_BX(RN) (0xe12fff10 | ((RN) & 0x0f))

#define ARM_LDR(RD, RN, OFFS) (((OFFS) ? 0xe4900000 : 0xe5900000) | \
			(((RN) & 0x0f) << 16) | (((RD) & 0x0f) << 12) | ((OFFS) & 0x0fff))

#define ARM_STR(RD, RN, OFFS) (((OFFS) ? 0xe4800000 : 0xe5800000) | \
			(((RN) & 0x0f) << 16) | (((RD) & 0x0f) << 12) | ((OFFS) & 0x0fff))

#define ARM_LDRB(RD, RN, OFFS) (0xe4d00000 | (((RN) & 0x0f) << 16) | \
			(((RD) & 0x0f) << 12) | ((OFFS) & 0x0fff))

#define ARM_STRB(RD, RN, OFFS) (0xe4c00000 | (((RN) & 0x0f) << 16) | \
			(((RD) & 0x0f) << 12) | ((OFFS) & 0x0fff))

#define ARM_LDRH(RD, RN, OFFS) (0xe0d000b0 | (((RN) & 0x0f) << 16) | \
			(((RD) & 0x0f) << 12) | (((OFFS) & 0x0f0) << 4) | ((OFFS) & 0x0f))

#define ARM_STRH(RD, RN, OFFS) (0xe0c000b0 | (((RN) & 0x0f) << 16) | \
			(((RD) & 0x0f) << 12) | (((OFFS) & 0x0f0) << 4) | ((OFFS) & 0x0f))

#define ARM_LDM(RGS, BASE) (0xe8900000 | (((BASE) & 0x0f) << 16) | \
			((RGS) & 0xffff))

#define ARM_LDMIA(RGS, BASE) (0xe8b00000 | (((BASE) & 0x0f) << 16) | \
			((RGS) & 0xffff))

#define ARM_STM(RGS, BASE) (0xe8800000 | (((BASE) & 0x0f) << 16) | \
			((RGS) & 0xffff))

#define ARM_STMIA(RGS, BASE) (0xe8a00000 | (((BASE) & 0x0f) << 16) | \
			((RGS) & 0xffff))

#define ARM_B(OFFS) (0xea000000 | (0x00ffffff & ((OFFS) >> 2)))

#define ARM_MRS_CPSR(DST) (0xe10f0000 | (((DST) & 0x0f) << 12))

#define ARM_MRS_SPSR(DST) (0xe14f0000 | (((DST) & 0x0f) << 12))

#define ARM_MSR_CPSR(SRC) (0xe12ff000 | ((SRC) & 0x0f))

#define ARM_MSR_SPSR(SRC) (0xe16ff000 | ((SRC) & 0x0f))

#define ARM_MSR_CPSR_IM(IM, ROTATE, FIELD) \
	    (0xe320f000 | (IM)  | ((ROTATE) << 8) | ((FIELD) << 16))

#define ARM_MSR_SPSR_IM(IM, ROTATE, FIELD) \
	    (0xe360f000 | (IM)  | ((ROTATE) << 8) | ((FIELD) << 16))


#define ARM_MSR_C(SRC) (0xe121f000 | ((SRC) & 0x0f))

#define ARM_MSR_F(SRC) (0xe128f000 | ((SRC) & 0x0f))


/* THUMB opcodes */

#define THUMB_NOP	0x46c046c0

#define THUMB_SWI(N) (0xdf00df00 | (N & 0x00ff) | ((N & 0x00ff) << 16))

#define THUMB_UNDEF THUMB_SWI(0x00)

#define THUMB_STR(DST, BASE) (0x60006000 | \
			((((BASE) & 0x07) << 3) | (DST & 0x07)) | \
			(((((BASE) & 0x07) << 3) | (DST & 0x07)) << 16))

#define THUMB_LDR(SRC, BASE) (0x68006800 | \
			((((BASE) & 0x07) << 3) | ((SRC) & 0x07)) | \
			(((((BASE) & 0x07) << 3) | ((SRC) & 0x07)) << 16))

#define THUMB_LDR_SP(RD, IMM) (0x98009800 | \
			((((RD) & 0x07) << 8) | (((IMM) / 4) & 0xff)) | \
			(((((RD) & 0x07) << 8) | (((IMM) / 4) & 0xff)) << 16))

#define THUMB_LDMIA(RGS, RN) (0xc800c800 | \
			((((RN) & 0x7) << 8) | ((RGS) & 0xff)) | \
			(((((RN) & 0x7) << 8) | ((RGS) & 0xff)) << 16))

#define THUMB_MOV(RD, RS) (0x46004600 | \
			(((RS) & 0x0f) << 3) | ((RD) & 0x07) | (((RD) & 0x08) << 4) | \
			(((((RS) & 0x0f) << 3) | ((RD) & 0x07) | (((RD) & 0x08) << 4)) << 16))

#define THUMB_SUB(RD, IMM) (0x38003800 | \
			((((RD) & 0x07) << 8) | ((IMM) & 0xff)) | \
			(((((RD) & 0x07) << 8) | ((IMM) & 0xff)) << 16))

#define THUMB_EOR(RD, RS) (0x40404040 | \
			(((RS) & 0x07) << 3) | ((RD) & 0x07) | \
			(((((RS) & 0x07) << 3) | ((RD) & 0x07)) << 16))

#define THUMB_BX(DST) (0x47004700 | \
			((DST & 0x0f) << 3) | \
			((DST & 0x0f) << 19))

#define THUMB_PUSH(RGS, LR) (0xb400b400 | \
							 (((RGS) & 0x0f) | (((LR) ? 1 : 0) << 8)) | \
							 ((((RGS) & 0x0f) | (((LR) ? 1 : 0) << 8)) << 16))

#define THUMB_POP(RGS, PC) (0xbc00bc00 | \
							(((RGS) & 0x0f) | (((PC) ? 1 : 0) << 8)) | \
							((((RGS) & 0x0f) | (((PC) ? 1 : 0) << 8)) << 16))

#define THUMB_B(OFFS) (0xe000e000 | ((0x00fff & (OFFS)) >> 1) | \
					   ( ((0x00fff & (OFFS)) >> 1) << 16 ) )

#define THUMB_STUFF 0x0000


#ifdef __ASSEMBLER__


#else /* __ASSEMBLER__ */

#include <stdint.h>
#include <stdio.h>

struct arm_opcode {
    uint32_t value, mask;	/* recognise instruction if (op&mask)==value */
    const char *assembler;		/* how to disassemble this instruction */
};

typedef struct arm_opcode arm_opcode_t;

struct thumb_opcode {
    unsigned short value, mask;	/* recognise instruction if (op&mask)==value */
    const char * assembler;		/* how to disassemble this instruction */
};

typedef struct thumb_opcode thumb_opcode_t;

extern const struct arm_opcode arm_opcodes[];

extern const struct thumb_opcode thumb_opcodes[];

extern const struct ice_drv arm7ice_drv;

extern const char * const arm_regnames[];

#ifdef __cplusplus
extern "C" {
#endif

int arm_lookup_reg(char * name);

const char * arm_reg_name(int r);

int arm_show_cpsr(FILE * f, uint32_t cpsr);

int arm_show_regs(FILE * f, uint32_t * reg);

int arm_show_asm(FILE * f, uint32_t addr, uint32_t opc);

int thumb_show_asm(FILE * f, uint32_t addr, uint32_t opc);

int arm_fmt_insn(char * s, unsigned int pc, unsigned int opc);

int thumb_fmt_insn(char * s, unsigned int pc, unsigned int opc);

#ifdef __cplusplus
}
#endif

#endif /* __ASSEMBLER__ */

#endif /* __ARM_OPC_H__ */

