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
 * @file arm9.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <stdlib.h>
#include <string.h>

#include "jtag.h"
#include "jtag_arm.h"
#include "armice.h"
#include "arm_opc.h"

#include "dbglog.h"


/**************************************************************
 * ARM9 Macros (Layer II)
 **************************************************************/

int arm9_wr8(jtag_tap_t * tap, uint32_t addr, uint8_t data)
{
	jtag_arm9_step(tap, 1, ARM_LDM(ARM_R1_R2, ARM_R0));
	jtag_arm9_step(tap, 2, ARM_NOP);
	jtag_arm9_step_wr(tap, 1, data);
	jtag_arm9_step_wr(tap, 1, addr);

	/* run at system speed  */
	return jtag_arm9_step_sys(tap, ARM_STRB(ARM_R0, ARM_R1, 0));
}

int arm9_rd8(jtag_tap_t * tap, uint32_t addr, uint8_t * data)
{
	int ret;
	unsigned int val;

	jtag_arm9_step(tap, 1, ARM_LDR(ARM_R2, ARM_R0, 0));
	jtag_arm9_step(tap, 2, ARM_NOP);
	jtag_arm9_step_wr(tap, 1, addr);

	/* run at system speed and */
	ret = jtag_arm9_step_sys(tap, ARM_LDRB(ARM_R1, ARM_R2, 0));

	jtag_arm9_step(tap, 1, ARM_STR(ARM_R1, ARM_R0, 0));
	jtag_arm9_step(tap, 2, ARM_NOP);
	jtag_arm9_step_rd(tap, 1, &val);
	*data = val;

	return ret;
}

int arm9_wr16(jtag_tap_t * tap, uint32_t addr, uint16_t data)
{
	jtag_arm9_step(tap, 1, ARM_LDM(ARM_R1_R2, ARM_R0));
	jtag_arm9_step(tap, 2, ARM_NOP);
	jtag_arm9_step_wr(tap, 1, data);
	jtag_arm9_step_wr(tap, 1, addr);
	
	/* run at system speed  */
	return jtag_arm9_step_sys(tap, ARM_STRH(ARM_R0, ARM_R1, 0));
}

int arm9_rd16(jtag_tap_t * tap, uint32_t addr, uint16_t * data)
{
	int ret;
	unsigned int val;

	jtag_arm9_step(tap, 1, ARM_LDR(ARM_R1_R2, ARM_R0, 0));
	jtag_arm9_step(tap, 2, ARM_NOP);
	jtag_arm9_step_wr(tap, 1, addr);

	/* run at system speed */
	ret = jtag_arm9_step_sys(tap, ARM_LDRH(ARM_R1, ARM_R2, 0));

	jtag_arm9_step(tap, 1, ARM_STR(ARM_R1, ARM_R0, 0));
	jtag_arm9_step(tap, 2, ARM_NOP);
	jtag_arm9_step_rd(tap, 1, &val);
	*data = val;

	return ret;
}

int arm9_wr32(jtag_tap_t * tap, uint32_t addr, uint32_t data)
{
	int ret;

	DCC_LOG2(LOG_INFO, "[%08x] <- %08x", addr, data);

	jtag_arm9_step(tap, 1, ARM_LDM(ARM_R1_R2, ARM_R0));
	jtag_arm9_step(tap, 2, ARM_NOP);
	jtag_arm9_step_wr(tap, 1, data);
	jtag_arm9_step_wr(tap, 1, addr);
	
	/* run at system speed  */
	ret = jtag_arm9_step_sys(tap, ARM_STR(ARM_R1, ARM_R2, 0));

	return ret;
}

int arm9_rd32(jtag_tap_t * tap, uint32_t addr, uint32_t * data)
{
	int ret;

	jtag_arm9_step(tap, 1, ARM_LDR(ARM_R2, ARM_R0, 0));
	jtag_arm9_step(tap, 2, ARM_NOP);
	jtag_arm9_step_wr(tap, 1, addr);

	/* run at system speed */
	ret = jtag_arm9_step_sys(tap, ARM_LDR(ARM_R1, ARM_R2, 0));

	jtag_arm9_step(tap, 1, ARM_STR(ARM_R1, ARM_R0, 0));
	jtag_arm9_step(tap, 2, ARM_NOP);
	jtag_arm9_step_rd(tap, 1, data);

	DCC_LOG2(LOG_INFO, "[%08x] -> %08x", addr, *data);

	return ret;
}

void arm9_cpsr_get(jtag_tap_t * tap, uint32_t * cpsr)
{
	DCC_LOG(LOG_INFO, "saving cpsr");
	jtag_arm9_step(tap, 1, ARM_MRS_CPSR(ARM_R0));
	jtag_arm9_step(tap, 4, ARM_NOP);
	jtag_arm9_step(tap, 1, ARM_STR(ARM_R0, ARM_PC, 0));
	jtag_arm9_step(tap, 2, ARM_NOP);
	jtag_arm9_step_rd(tap, 1, cpsr);
}

void arm9_cpsr_set(jtag_tap_t * tap, uint32_t cpsr)
{
	DCC_LOG1(LOG_TRACE, "restoring cpsr=0x%08x", cpsr);
	jtag_arm9_step(tap, 1, ARM_MSR_CPSR_IM(cpsr & 0xff, 0, 1));
	jtag_arm9_step(tap, 4, ARM_NOP);
	jtag_arm9_step(tap, 1, ARM_MSR_CPSR_IM((cpsr >> 8) & 0xff, 12, 2));
	jtag_arm9_step(tap, 4, ARM_NOP);
	jtag_arm9_step(tap, 1, ARM_MSR_CPSR_IM((cpsr >> 16) & 0xff, 8, 4));
	jtag_arm9_step(tap, 4, ARM_NOP);
	jtag_arm9_step(tap, 1, ARM_MSR_CPSR_IM((cpsr >> 24) & 0xff, 4, 8));
	jtag_arm9_step(tap, 2, ARM_NOP);
}

void arm9_spsr_get(jtag_tap_t * tap, uint32_t * cpsr)
{
	DCC_LOG(LOG_INFO, "saving cpsr");
	jtag_arm9_step(tap, 1, ARM_MRS_CPSR(ARM_R0));
	jtag_arm9_step(tap, 4, ARM_NOP);
	jtag_arm9_step(tap, 1, ARM_STR(ARM_R0, ARM_PC, 0));
	jtag_arm9_step(tap, 2, ARM_NOP);
	jtag_arm9_step_rd(tap, 1, cpsr);
}

void arm9_spsr_set(jtag_tap_t * tap, uint32_t cpsr)
{
	DCC_LOG1(LOG_TRACE, "restoring cpsr=0x%08x", cpsr);
	jtag_arm9_step(tap, 1, ARM_MSR_SPSR_IM(cpsr & 0xff, 0, 1));
	jtag_arm9_step(tap, 4, ARM_NOP);
	jtag_arm9_step(tap, 1, ARM_MSR_SPSR_IM((cpsr >> 8) & 0xff, 12, 2));
	jtag_arm9_step(tap, 4, ARM_NOP);
	jtag_arm9_step(tap, 1, ARM_MSR_SPSR_IM((cpsr >> 16) & 0xff, 8, 4));
	jtag_arm9_step(tap, 4, ARM_NOP);
	jtag_arm9_step(tap, 1, ARM_MSR_SPSR_IM((cpsr >> 24) & 0xff, 4, 8));
	jtag_arm9_step(tap, 2, ARM_NOP);
}


void arm9_context_save(jtag_tap_t * tap, unsigned int flags,
							struct armice_context * ct)
{
	uint32_t cpsr;
	int mode;
	int i;

	DCC_LOG(LOG_INFO, "saving registers");
	cpsr = ct->cpsr;

	/* save the registers 
	 Uses PC as index as the lower bits [1,0] are 0 */
	jtag_arm9_step(tap, 1, ARM_STM(ARM_R0_PC, ARM_PC));
	jtag_arm9_step(tap, 1, ARM_MRS_CPSR(ARM_R0));
	/* ensure R0 is word aligned */
	jtag_arm9_step(tap, 1, ARM_NOP);
	
	for (i = 0; i < 16; i++) {
		jtag_arm9_step_rd(tap, 1, &ct->r[i]);
	}

	jtag_arm9_step(tap, 1, ARM_STR(ARM_R0, ARM_R0, 0));
	jtag_arm9_step(tap, 2, ARM_EOR(ARM_R0, ARM_R0, ARM_R0));
	jtag_arm9_step_rd(tap, 2, &ct->cpsr);

	/* adjust the program counter */
	mode = ct->cpsr & ~ARM_MODE_MASK;
	if ((mode != (cpsr & ~ARM_MODE_MASK)) && (mode == ARM_MODE_ABORT)) {
		/* exception */
		ct->pc -= (3 * 4);
	} else {
		if (flags & ARMICE_DBGRQ)
			ct->pc -= (3 * 4);
		else
			/* normal execution */
			ct->pc -= (5 * 4);
	}
}

void arm9_context_restore(jtag_tap_t * tap, armice_context_t * ct)
{
	int i;

	DCC_LOG1(LOG_INFO, "restoring cpsr=0x%08x", ct->cpsr);
	jtag_arm9_step(tap, 1, ARM_MSR_CPSR_IM(ct->cpsr & 0xff, 0, 1));
	jtag_arm9_step(tap, 5, ARM_MSR_CPSR_IM((ct->cpsr >> 8) & 0xff, 12, 2));
	jtag_arm9_step(tap, 5, ARM_MSR_CPSR_IM((ct->cpsr >> 16) & 0xff, 8, 4));
	jtag_arm9_step(tap, 5, ARM_MSR_CPSR_IM((ct->cpsr >> 24) & 0xff, 4, 8));

	DCC_LOG(LOG_INFO, "restoring registers");

	jtag_arm9_step(tap, 1, ARM_LDM(ARM_R0_LR, ARM_R0));
	jtag_arm9_step(tap, 2, ARM_NOP);
	jtag_arm9_step_wr(tap, 1, ct->r[0]);
	for (i = 1; i < 15; i++) {
		jtag_arm9_step_wr(tap, 1, ct->r[i]);
	}
	jtag_arm9_step(tap, 1, ARM_LDM((1 << ARM_PC), ARM_R0));
	jtag_arm9_step(tap, 2, ARM_NOP);
	jtag_arm9_step_wr(tap, 4, ct->pc);
}

int arm9_insn_fetch(jtag_tap_t * tap, uint32_t addr, 
					   int thumb, uint32_t * insn)
{
	uint32_t tmp;

	DCC_LOG2(LOG_INFO, "addr: 0x%08x (%s)", addr, thumb ? "THUMB" : "ARM");

	/* fetching opcode */
	jtag_arm9_step(tap, 1, ARM_LDR(ARM_R2, ARM_R0, 0));
	jtag_arm9_step(tap, 2, ARM_NOP);
	jtag_arm9_step_wr(tap, 1, addr);

	/* fetching opcode */
	if (thumb) {
		jtag_arm9_step_sys(tap, ARM_LDRH(ARM_R1, ARM_R2, 2));
		jtag_arm9_step(tap, 1, ARM_STR(ARM_R1, ARM_R0, 0));
		jtag_arm9_step(tap, 2, ARM_NOP);
		jtag_arm9_step_rd(tap, 1, &tmp);
		*insn = tmp & 0xffff;

		if ((tmp & 0xf800) != 0xf000)
			return 2;

		/* the thumb bl instruction requires two words */
		DCC_LOG(LOG_INFO, "bl instruction");
		jtag_arm9_step_sys(tap, ARM_LDRH(ARM_R1, ARM_R2, 0));
		jtag_arm9_step(tap, 1, ARM_STR(ARM_R1, ARM_R0, 0));
		jtag_arm9_step(tap, 2, ARM_NOP);
		jtag_arm9_step_rd(tap, 1, &tmp);
		*insn |= tmp << 16;
	} else {
		jtag_arm9_step_sys(tap, ARM_LDR(ARM_R1, ARM_R2, 0));
		jtag_arm9_step(tap, 1, ARM_STR(ARM_R1, ARM_R0, 0));
		jtag_arm9_step(tap, 2, ARM_NOP);
		jtag_arm9_step_rd(tap, 1, insn);
	}

	return 4;
}

int arm9_insn_step(jtag_tap_t * tap, struct armice_context * ct)
{
	int ret;
	int i;
#if 0
	DCC_LOG1(LOG_INFO, "restoring cpsr=0x%08x", ct->cpsr);
	jtag_arm9_step(tap, 1, ARM_MSR_CPSR_IM(ct->cpsr & 0xff, 0, 1));
	jtag_arm9_step(tap, 5, ARM_MSR_CPSR_IM((ct->cpsr >> 8) & 0xff, 12, 2));
	jtag_arm9_step(tap, 5, ARM_MSR_CPSR_IM((ct->cpsr >> 16) & 0xff, 8, 4));
	jtag_arm9_step(tap, 5, ARM_MSR_CPSR_IM((ct->cpsr >> 24) & 0xff, 4, 8));

	DCC_LOG(LOG_INFO, "restoring registers");

	jtag_arm9_step(tap, 1, ARM_LDM(ARM_R0_LR, ARM_R0));
	jtag_arm9_step(tap, 2, ARM_NOP);
	jtag_arm9_step_wr(tap, 1, ct->r[0]);
	for (i = 1; i < 15; i++) {
		jtag_arm9_step_wr(tap, 1, ct->r[i]);
	}
	jtag_arm9_step(tap, 1, ARM_LDM((1 << ARM_PC), ARM_R0));
	jtag_arm9_step(tap, 2, ARM_NOP);
	jtag_arm9_step_wr(tap, 5, ct->pc);

#endif

	jtag_arm9_step(tap, 1, ARM_NOP);


	/* resume */
	jtag_arm9_step(tap, 1, ARM_B(-(2 * 4)));
	jtag_arm9_step_sync(tap, ARM_NOP);

	/* set the single step breakpoint */
	DCC_LOG1(LOG_TRACE, "single step BP @ 0x%08x...", ct->pc - (2 * 4));
	jtag_arm_sstp_bp_set(tap, ct->pc - (2 * 4));

	/* */
	if ((ret = jtag_arm_restart(tap)) != JTAG_OK) {
		DCC_LOG(LOG_ERROR, "jtag_arm_restart()");
	}




#if 0
	/* set the single step breakpoint */

#endif

	/* resume */
//	jtag_arm9_step(tap, 1, ARM_B(-4 * 2));
//	jtag_arm9_step_sync(tap, ARM_NOP);



//	ret = jtag_arm9_step_sys(tap, ARM_B(-(5 * 4)));

	DCC_LOG(LOG_TRACE, "disabling single step ...");
	jtag_arm_sstp_disable(tap);

	return ret;
}

int arm9_exec(jtag_tap_t * tap, int thumb)
{
	/* exec */
	if (thumb) {
		jtag_arm9_step(tap, 1, THUMB_B(-5 * 2));
		return jtag_arm9_step_sync(tap, THUMB_NOP);
	}

	jtag_arm9_step(tap, 1, ARM_B(-(5 * 4)));
	return jtag_arm9_step_sync(tap, ARM_NOP);
}

void arm9_code_exec(jtag_tap_t * tap, uint32_t addr, 
					uint32_t * regv, int regc)
{
	int regs;
	int i;
	
	/* set registers */
	regs = 0;
	for (i = 0; i < regc; i++) {
		regs |= (1 << i);
	}

	DCC_LOG1(LOG_INFO, "regs=0x%04x", regs);
	
	jtag_arm9_step(tap, 1, ARM_LDM(regs | (1 << ARM_PC), ARM_R0));
	jtag_arm9_step(tap, 2, ARM_NOP);
	for (i = 0; i < regc; i++) {
		DCC_LOG2(LOG_INFO, "r%d=0x%08x", i, regv[i]);
		jtag_arm9_step_wr(tap, 1, regv[i]);
	}
	/* set PC */
	DCC_LOG1(LOG_INFO, "pc=0x%08x", addr);
	jtag_arm9_step_wr(tap, 2, addr);

	/* resume */
	jtag_arm9_step(tap, 1, ARM_B(-(2 * 4)));
	jtag_arm9_step_sync(tap, ARM_NOP);
}

/**************************************************************
 * THUMB Macros
 **************************************************************/

void arm9_thumb_context_save(jtag_tap_t * tap, unsigned int flags,
							 armice_context_t * ct)
{
	int i;

	DCC_LOG(LOG_INFO, "changing to ARM");

	/* save R0 and PC */
	jtag_arm9_step(tap, 1, THUMB_STR(ARM_R0, ARM_R0));
	jtag_arm9_step(tap, 2, THUMB_NOP);
	jtag_arm9_step_thumb_rd(tap, 2, &ct->r[0]);
	jtag_arm9_step(tap, 1, THUMB_MOV(ARM_R0, ARM_PC));
	jtag_arm9_step(tap, 1, THUMB_STR(ARM_R0, ARM_R0));
	jtag_arm9_step(tap, 2, THUMB_NOP);
	jtag_arm9_step_thumb_rd(tap, 2, &ct->pc);
	jtag_arm9_step(tap, 1, THUMB_BX(ARM_PC));
	jtag_arm9_step(tap, 2, THUMB_NOP);

	/* adjust PC */
	if (flags & ARMICE_DBGRQ)
		ct->pc -= (3 * 2);
	else
		ct->pc -= (7 * 2);

	DCC_LOG(LOG_INFO, "saving registers");

	/* save remaining registers */
	jtag_arm9_step(tap, 1, ARM_STM(ARM_R1_R14, ARM_PC));
	jtag_arm9_step(tap, 2, ARM_NOP);
	for (i = 1; i < 15; i++) {
		jtag_arm9_step_rd(tap, 1, &ct->r[i]);
	}

	/* save CPSR into R0 */
	jtag_arm9_step(tap, 1, ARM_MRS_CPSR(ARM_R0));
	jtag_arm9_step(tap, 2, ARM_NOP);
	/* ensure R0 is word aligned */
	/* store R0 (CPSR) */
	jtag_arm9_step(tap, 1, ARM_STR(ARM_R0, ARM_PC, 0));
	jtag_arm9_step(tap, 2, ARM_EOR(ARM_R0, ARM_R0, ARM_R0));
	jtag_arm9_step_rd(tap, 2, &ct->cpsr);

	ct->cpsr |= ARM_STATE_THUMB;
}

void arm9_thumb_context_restore(jtag_tap_t * tap, armice_context_t * ct)
{
	int i;
	uint32_t cpsr;

	DCC_LOG1(LOG_INFO, "restoring cpsr=0x%08x", ct->cpsr);

	/* restore CPSR with the THUMB flag cleared */
	cpsr = ct->cpsr & ~(ARM_STATE_THUMB); 

	jtag_arm9_step(tap, 1, ARM_MSR_CPSR_IM(cpsr & 0xff, 0, 1));
	jtag_arm9_step(tap, 5, ARM_MSR_CPSR_IM((cpsr >> 8) & 0xff, 12, 2));
	jtag_arm9_step(tap, 5, ARM_MSR_CPSR_IM((cpsr >> 16) & 0xff, 8, 4));
	jtag_arm9_step(tap, 5, ARM_MSR_CPSR_IM((cpsr >> 24) & 0xff, 4, 8));
	jtag_arm9_step(tap, 2, ARM_NOP);

	DCC_LOG(LOG_INFO, "restoring registers");

	jtag_arm9_step(tap, 1, ARM_LDM(ARM_R0_LR, ARM_PC));
	jtag_arm9_step(tap, 2, ARM_NOP);
	/* R0 = PC */
	jtag_arm9_step_wr(tap, 1, (ct->pc - (3 * 2)) | 1);
	for (i = 1; i < 15; i++) {
		jtag_arm9_step_wr(tap, 1, ct->r[i]);
	}

	jtag_arm9_step(tap, 2, ARM_NOP);

	/* thumb state switch (6 cycles) */
	jtag_arm9_step(tap, 1, ARM_BX(ARM_R0));
	jtag_arm9_step(tap, 2, ARM_NOP);

	/* thumb state (1 insn - 3 fetches) */
	/* restore r0-r1 */
	jtag_arm9_step(tap, 1, THUMB_LDMIA((1 << ARM_R0), ARM_R0));
	jtag_arm9_step(tap, 2, THUMB_NOP);
	jtag_arm9_step_thumb_wr(tap, 1, ct->r[0]);
}

int arm9_thumb_insn_step(jtag_tap_t * tap, 
						 struct armice_context * ct)
{
	int ret;
	int i;

	/* restore the context */
	jtag_arm9_step(tap, 1, ARM_LDR(ARM_R0, ARM_R0, 0));
	jtag_arm9_step(tap, 2, ARM_NOP);
	/* restore cpsr with the Thumb flag cleared */
	jtag_arm9_step_wr(tap, 1, ct->cpsr & ~(ARM_STATE_THUMB)); 

	jtag_arm9_step(tap, 1, ARM_LDM(ARM_R1_R14, ARM_SP));
	jtag_arm9_step(tap, 2, ARM_MSR_CPSR(ARM_R0));
	/* restore registers r2 to r14 */
	for (i = 1; i < 14; i++) {
		jtag_arm9_step_wr(tap, 1, ct->r[i]);
	}
	/* register 14 and LDM stuffing, MSR stuffing */
	jtag_arm9_step_wr(tap, 4, ct->lr);

	jtag_arm9_step(tap, 1, ARM_LDR(ARM_R0, ARM_SP, 0));
	/* thumb state switch  */
	jtag_arm9_step(tap, 2, ARM_BX(ARM_R0));
	/* load pc */
	jtag_arm9_step_wr(tap, 2, (ct->pc - (6 * 2)) | 1);

	/* restore r0 */
	jtag_arm9_step(tap, 1, THUMB_LDR_SP(ARM_R0, 0));
	jtag_arm9_step(tap, 2, THUMB_NOP);
	jtag_arm9_step_thumb_wr(tap, 1, ct->r[0]);

	/* resume */
	jtag_arm9_step(tap, 1, THUMB_B(-2 * 2));
	jtag_arm9_step_sync(tap, THUMB_NOP);

	/* set the single step breakpoint */
	jtag_arm_sstp_bp_set(tap, ct->pc - (2 * 2));

	/* */
	if ((ret = jtag_arm_restart(tap)) != JTAG_OK) {
		DCC_LOG(LOG_ERROR, "jtag_arm_restart()");
	}

	ret = 0;
	return ret;
}

/**************************************************************
 * ARM7 Bulk memory operations
 **************************************************************/

int arm9_mem_read(jtag_tap_t * tap, uint32_t base, 
			   uint32_t offs, void * p, int len)
{
	uint32_t addr;
	uint32_t data;
	unsigned char * cp;
	int cnt;
	int j;
	int n;

	DCC_LOG3(LOG_INFO, "base=0x%08x offs=0x%08x len=%d,", base, offs, len);

	if (len == 0)
		return 0;

	addr = base + offs;

	/* uses R12 as index */
	jtag_arm9_step(tap, 1, ARM_LDR(ARM_R13, ARM_R0, 0));
	jtag_arm9_step(tap, 2, ARM_NOP);
	jtag_arm9_step_wr(tap, 1, addr);

	cp = (unsigned char *)p;
	cnt = 0;

	/* get the first bytes, if the address is not half-word (16bits) aligned */
	if(addr & 0x01) {
		/* run at system speed and increment the base register by one */
		jtag_arm9_step_sys(tap, ARM_LDRB(ARM_R1, ARM_R13, 1));
		jtag_arm9_step(tap, 1, ARM_STR(ARM_R1, ARM_R0, 0));
		jtag_arm9_step(tap, 2, ARM_NOP);
		jtag_arm9_step_rd(tap, 1, &data);
		*cp++ = data;
		len--;
		addr++;
		cnt++;
	}
	/* get the next word, if the address is not word (32bits) aligned */
	if ((len >= 2) && (addr & 0x02)) {
		/* run at system speed and increment the base register by two */
		jtag_arm9_step_sys(tap, ARM_LDRH(ARM_R1, ARM_R13, 2));
		jtag_arm9_step(tap, 1, ARM_STR(ARM_R1, ARM_R0, 0));
		jtag_arm9_step(tap, 2, ARM_NOP);
		jtag_arm9_step_rd(tap, 1, &data);
		*cp++ = data;
		*cp++ = data >> 8;
		len -= 2;
		cnt += 2;
	}

	while (len >= 4) {
		/* transfer up to 12 words */
		n = (len / 4) < 12 ? (len / 4) : 12;

		if (n == 1) {
			/* run at system speed and increment the base register by 4 */
			jtag_arm9_step_sys(tap, ARM_LDR(ARM_R1, ARM_R13, 4));
			jtag_arm9_step(tap, 1, ARM_STR(ARM_R1, ARM_R0, 0));
			jtag_arm9_step(tap, 2, ARM_NOP);
			jtag_arm9_step_rd(tap, 1, &data);
			*cp++ = data;
			*cp++ = data >> 8;
			*cp++ = data >> 16;
			*cp++ = data >> 24;
		} else {
			/* run at system speed and increment the base register by (4 * n) */
			jtag_arm9_step_sys(tap, ARM_LDMIA((2 << n) - 2, ARM_R13));
			jtag_arm9_step(tap, 1, ARM_STM((2 << n) - 2, ARM_R0));
			jtag_arm9_step(tap, 2, ARM_NOP);
			for (j = 0; j < n; j++) {
				jtag_arm9_step_rd(tap, 1, &data);
				*cp++ = data;
				*cp++ = data >> 8;
				*cp++ = data >> 16;
				*cp++ = data >> 24;
			}
		}
		len -= 4 * n;
		cnt += 4 * n; 
	}

	/* get the last word, if the address is not word (32bits) aligned */
	if (len & 0x02) {
		/* run at system speed and increment the base register by two */
		jtag_arm9_step_sys(tap, ARM_LDRH(ARM_R1, ARM_R13, 2));
		jtag_arm9_step(tap, 1, ARM_STR(ARM_R1, ARM_R0, 0));
		jtag_arm9_step(tap, 2, ARM_NOP);
		jtag_arm9_step_rd(tap, 1, &data);
		*cp++ = data;
		*cp++ = data >> 8;
		cnt += 2;
	}
	/* get the last bytes, if the address is not half-word (16bits) aligned */
	if(len & 0x01) {
		/* run at system speed */
		jtag_arm9_step_sys(tap, ARM_LDRB(ARM_R1, ARM_R13, 0));
		jtag_arm9_step(tap, 1, ARM_STR(ARM_R1, ARM_R0, 0));
		jtag_arm9_step(tap, 2, ARM_NOP);
		jtag_arm9_step_rd(tap, 1, &data);
		*cp = data;
		cnt ++;
	}

	return cnt;
}

int arm9_mem_write(jtag_tap_t * tap, uint32_t base, 
				   uint32_t offs, const void * p, int len)
{
	uint32_t addr;
	uint32_t data;
	unsigned char * cp;
	int cnt;
	int j;
	int n;

	DCC_LOG3(LOG_INFO, "base=0x%08x offs=0x%08x len=%d,", base, offs, len);

	if (len == 0)
		return 0;

	addr = base + offs;
	cnt = 0;
	cp = (unsigned char *)p;

	jtag_arm9_step(tap, 1, ARM_LDR(ARM_R13, ARM_R0, 0));
	jtag_arm9_step(tap, 2, ARM_NOP);
	jtag_arm9_step_wr(tap, 1, addr);

	/* get the first bytes, if the address is not half-word (16bits) aligned */
	if (addr & 0x01) {
		data = *cp++;
		jtag_arm9_step(tap, 1, ARM_LDR(ARM_R1, ARM_R0, 0));
		jtag_arm9_step(tap, 2, ARM_NOP);
		jtag_arm9_step_wr(tap, 1, data);
		/* run at system speed and increment the base register by one */
		jtag_arm9_step_sys(tap, ARM_STRB(ARM_R1, ARM_R13, 1));
		len--;
		addr++;
		cnt++;
	}
	/* get the next word, if the address is not word (32bits) aligned */
	if ((len >= 2) && (addr & 0x02)) {
		data = *cp++;
		data += *cp++ << 8;
		jtag_arm9_step(tap, 1, ARM_LDR(ARM_R1, ARM_R0, 0));
		jtag_arm9_step(tap, 2, ARM_NOP);
		jtag_arm9_step_wr(tap, 1, data);
		/* run at system speed and increment the base register by two */
		jtag_arm9_step_sys(tap, ARM_STRH(ARM_R1, ARM_R13, 2));
		len -= 2;
		cnt += 2;
	}

	while (len >= 4) {
		/* transfer up to 12 words */
		n = (len / 4) < 12 ? (len / 4) : 12;

		if (n == 1) {
			/* run at system speed  */
			data = *cp++;
			data += *cp++ << 8;
			data += *cp++ << 16;
			data += *cp++ << 24;
			jtag_arm9_step(tap, 1, ARM_LDR(ARM_R1, ARM_R0, 0));
			jtag_arm9_step(tap, 2, ARM_NOP);
			jtag_arm9_step_wr(tap, 1, data);
			/* run at system speed and increment the base register by 4 */
			jtag_arm9_step_sys(tap, ARM_STR(ARM_R1, ARM_R13, 4));
		} else {
			jtag_arm9_step(tap, 1, ARM_LDM((2 << n) - 2, ARM_R0));
			jtag_arm9_step(tap, 2, ARM_NOP);
			for (j = 0; j < n; j++) {
				data = *cp++;
				data += *cp++ << 8;
				data += *cp++ << 16;
				data += *cp++ << 24;
				DCC_LOG1(LOG_INFO, "data=0x%08x", data);
				jtag_arm9_step_wr(tap, 1, data);
			}
			/* run at system speed and increment the base register by (4 * n) */
			jtag_arm9_step_sys(tap, ARM_STMIA((2 << n) - 2, ARM_R13));
		}
		len -= 4 * n;
		cnt += 4 * n; 
	}

	/* get the last word, if the address is not word (32bits) aligned */
	if (len & 0x02) {
		data = *cp++;
		data += *cp++ << 8;
		jtag_arm9_step(tap, 1, ARM_LDR(ARM_R1, ARM_R0, 0));
		jtag_arm9_step(tap, 2, ARM_NOP);
		jtag_arm9_step_wr(tap, 1, data);
		/* run at system speed and increment the base register by two */
		jtag_arm9_step_sys(tap, ARM_STRH(ARM_R1, ARM_R13, 2));
		len -= 2;
		cnt += 2;
	}
	/* get the last bytes, if the address is not half-word (16bits) aligned */
	if (len) {
		data = *cp;
		jtag_arm9_step(tap, 1, ARM_LDR(ARM_R1, ARM_R0, 0));
		jtag_arm9_step(tap, 2, ARM_NOP);
		jtag_arm9_step_wr(tap, 2, data);
		/* run at system speed */
		jtag_arm9_step_sys(tap, ARM_STRB(ARM_R1, ARM_R13, 0));
		cnt++;
	}

	return cnt;
}

int arm9_mem_set(jtag_tap_t * tap, uint32_t base, 
				 uint32_t offs, int val, int len)
{
	uint32_t addr;
	uint32_t data;
	int cnt;

	DCC_LOG3(LOG_TRACE, "base=0x%08x offs=0x%08x len=%d,", base, offs, len);

	if (len == 0)
		return 0;

	addr = base + offs;
	val &= 0xff;
	data = val | (val << 8) | (val << 16) | (val << 24);
	cnt = 0;

	jtag_arm9_step(tap, 1, ARM_LDM(ARM_R1_R2, ARM_R0));
	jtag_arm9_step(tap, 2, ARM_NOP);
	jtag_arm9_step_wr(tap, 1, data);	
	jtag_arm9_step_wr(tap, 1, addr);

	/* write the first bytes, 
	   if the address is not half-word (16bits) aligned */
	if (addr & 0x01) {
		/* run at system speed and increment the base register by one */
		jtag_arm9_step_sys(tap, ARM_STRB(ARM_R1, ARM_R2, 1));
		len--;
		addr++;
		cnt++;
	}
	/* write the next word, if the address is not word (32bits) aligned */
	if ((len >= 2) && (addr & 0x02)) {
		/* run at system speed and increment the base register by two */
		jtag_arm9_step_sys(tap, ARM_STRH(ARM_R1, ARM_R2, 2));
		len -= 2;
		cnt += 2;
	}

	while (len >= 4) {
		/* run at system speed and increment the base register by four */
		jtag_arm9_step_sys(tap, ARM_STR(ARM_R1, ARM_R2, 4));
		len -= 4;
		cnt += 4;
	}
	/* write the last word, if the address is not word (32bits) aligned */
	if (len & 0x02) {
		/* run at system speed and increment the base register by two */
		jtag_arm9_step_sys(tap, ARM_STRH(ARM_R1, ARM_R2, 2));
		len -= 2;
		cnt += 2;
	}
	/* write the last bytes, if the address is not half-word (16bits) aligned */
	if (len) {
		/* run at system speed and increment the base register by one */
		jtag_arm9_step_sys(tap, ARM_STRB(ARM_R1, ARM_R2, 1));
		cnt++;
	}

	return cnt;
}


