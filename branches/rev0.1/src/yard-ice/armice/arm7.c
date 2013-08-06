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
 * @file arm7.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <stdlib.h>
#include <string.h>

#include "jtag_arm.h"
#include "armice.h"
#include "arm_opc.h"

#include "dbglog.h"


/**************************************************************
 * ARM7 Macros (Layer II)
 **************************************************************/

int arm7_wr8(jtag_tap_t * tap, unsigned int addr, unsigned char data)

{
	jtag_arm7_step(tap, 1, ARM_LDM(ARM_R1_R2, ARM_PC), NULL);
	jtag_arm7_step(tap, 2, ARM_NOP, NULL);
	jtag_arm7_step(tap, 1, data, NULL);
	jtag_arm7_step(tap, 2, addr, NULL);

	/* run at system speed  */
	jtag_arm7_step(tap, 1, ARM_NOP, NULL);
	return jtag_arm7_step_sys(tap, ARM_NOP, ARM_STRB(ARM_R1, ARM_R2, 0));
}

int arm7_rd8(jtag_tap_t * tap, unsigned int addr, unsigned char * data)
{
	int ret;
	unsigned int val;

	jtag_arm7_step(tap, 1, ARM_LDR(ARM_R2, ARM_PC, 0), NULL);
	jtag_arm7_step(tap, 2, ARM_NOP, NULL);
	jtag_arm7_step(tap, 2, addr, NULL);

	/* run at system speed and */
	jtag_arm7_step(tap, 1, ARM_NOP, NULL);
	ret = jtag_arm7_step_sys(tap, ARM_NOP, ARM_LDRB(ARM_R1, ARM_R2, 0));

	jtag_arm7_step(tap, 1, ARM_STR(ARM_R1, ARM_PC, 0), NULL);
	jtag_arm7_step(tap, 2, ARM_NOP, NULL);

	jtag_arm7_step(tap, 1, 0, &val);

	*data = val;

	return ret;
}

int arm7_wr16(jtag_tap_t * tap, unsigned int addr, 
			  unsigned short data)
{
	jtag_arm7_step(tap, 1, ARM_LDM(ARM_R1_R2, ARM_PC), NULL);
	jtag_arm7_step(tap, 2, ARM_NOP, NULL);
	jtag_arm7_step(tap, 1, data, NULL);
	jtag_arm7_step(tap, 2, addr, NULL);

	/* run at system speed  */
	jtag_arm7_step(tap, 1, ARM_NOP, NULL);
	return jtag_arm7_step_sys(tap, ARM_NOP, ARM_STRH(ARM_R1, ARM_R2, 0));
}

int arm7_rd16(jtag_tap_t * tap, unsigned int addr, 
			  unsigned short * data)
{
	int ret;
	unsigned int val;

	jtag_arm7_step(tap, 1, ARM_LDR(ARM_R2, ARM_PC, 0), NULL);
	jtag_arm7_step(tap, 2, ARM_NOP, NULL);
	jtag_arm7_step(tap, 2, addr, NULL);

	/* run at system speed */
	jtag_arm7_step(tap, 1, ARM_NOP, NULL);
	ret = jtag_arm7_step_sys(tap, ARM_NOP, ARM_LDRH(ARM_R1, ARM_R2, 0));

	jtag_arm7_step(tap, 1, ARM_STR(ARM_R1, ARM_PC, 0), NULL);
	jtag_arm7_step(tap, 2, ARM_NOP, NULL);

	jtag_arm7_step(tap, 1, 0, &val);
	*data = val;

	return ret;
}

int arm7_wr32(jtag_tap_t * tap, uint32_t addr, uint32_t data)
{
	jtag_arm7_step(tap, 1, ARM_LDM(ARM_R1_R2, ARM_PC), NULL);
	jtag_arm7_step(tap, 2, ARM_NOP, NULL);
	jtag_arm7_step(tap, 1, data, NULL);
	jtag_arm7_step(tap, 2, addr, NULL);

	/* run at system speed  */
	jtag_arm7_step(tap, 1, ARM_NOP, NULL);
	return jtag_arm7_step_sys(tap, ARM_NOP, ARM_STR(ARM_R1, ARM_R2, 0));
}

int arm7_rd32(jtag_tap_t * tap, uint32_t addr, uint32_t * data)
{
	int ret;

	jtag_arm7_step(tap, 1, ARM_LDR(ARM_R2, ARM_PC, 0), NULL);
	jtag_arm7_step(tap, 2, ARM_NOP, NULL);
	jtag_arm7_step(tap, 2, addr, NULL);

	/* run at system speed */
	jtag_arm7_step(tap, 1, ARM_NOP, NULL);
	ret = jtag_arm7_step_sys(tap, ARM_NOP, ARM_LDR(ARM_R1, ARM_R2, 0));

	jtag_arm7_step(tap, 1, ARM_STR(ARM_R1, ARM_PC, 0), NULL);
	jtag_arm7_step(tap, 2, ARM_NOP, NULL);

	jtag_arm7_step(tap, 1, 0, data);

	return ret;
}

void arm7_context_save(jtag_tap_t * tap, unsigned int flags,
					   struct armice_context * ct)
{
	uint32_t cpsr;
	int mode;
	int i;

	DCC_LOG(LOG_INFO, "saving registers");
	cpsr = ct->cpsr;

	/* save the registers */
	jtag_arm7_step(tap, 1, ARM_STM(ARM_R0_PC, ARM_R0), NULL);
	jtag_arm7_step(tap, 1, ARM_MRS_CPSR(ARM_R0), NULL);
	jtag_arm7_step(tap, 1, ARM_NOP, NULL);

	for (i = 0; i < 15; i++) {
		jtag_arm7_step(tap, 1, 0, &ct->r[i]);
	}
	jtag_arm7_step(tap, 2, 0, &ct->pc);

	jtag_arm7_step(tap, 1, ARM_STR(ARM_R0, ARM_R0, 0), NULL);
	/* clear PC */
	jtag_arm7_step(tap, 2, ARM_EOR(ARM_PC, ARM_PC, ARM_PC), NULL);
	jtag_arm7_step(tap, 3, ARM_NOP, &ct->cpsr);

	cpsr = ARM_DISABLE_IRQ | ARM_DISABLE_FIQ | ARM_MODE_SUPERVISOR;
	if ((ct->cpsr & 0xff) != cpsr) {
		DCC_LOG(LOG_TRACE, "changing to Supervisor mode");
		cpsr = ARM_DISABLE_IRQ | ARM_DISABLE_FIQ | ARM_MODE_SUPERVISOR;
		jtag_arm7_step(tap, 1, ARM_MSR_CPSR_IM(cpsr & 0xff, 0, 1), NULL);
		jtag_arm7_step(tap, 2, ARM_NOP, NULL);
	}

	/* adjust the program counter */
	mode = ct->cpsr & ~ARM_MODE_MASK;
	if ((mode != (cpsr & ~ARM_MODE_MASK)) && (mode == ARM_MODE_ABORT)) {
		/* exception */
		ct->pc -= (4 * 4);
	} else {
		if (flags & ARMICE_DBGRQ)
			ct->pc -= (3 * 4);
		else
			/* normal execution */
			ct->pc -= (4 * 4);
	}

}

void arm7_context_restore(jtag_tap_t * tap, armice_context_t * ct)
{
	int i;

	DCC_LOG1(LOG_TRACE, "ct=0x%p", ct);

	if (ct->cpsr & ARM_STATE_THUMB) {
		DCC_LOG(LOG_ERROR, "THUMB flag set???");
	}

	/* restore the context */
	jtag_arm7_step(tap, 1, ARM_LDR(ARM_R1, ARM_PC, 0), NULL);
	jtag_arm7_step(tap, 1, ARM_MSR_CPSR(ARM_R1), NULL);
	jtag_arm7_step(tap, 1, ARM_LDM(ARM_R0_PC, ARM_PC), NULL);
	jtag_arm7_step(tap, 2, ct->cpsr, NULL); 
	jtag_arm7_step(tap, 2, ARM_NOP, NULL);
	for (i = 0; i < 15; i++) {
		jtag_arm7_step(tap, 1, ct->r[i], NULL);
	}
	jtag_arm7_step(tap, 1, ct->pc, NULL);
	jtag_arm7_step(tap, 2, ARM_NOP, NULL);
}

int arm7_insn_step(jtag_tap_t * tap, armice_context_t * ct)
{
	int ret;
#if 0
	int i;

	/* restore the context */
	jtag_arm7_step(tap, 1, ARM_LDR(ARM_R1, ARM_PC, 0), NULL);
	jtag_arm7_step(tap, 1, ARM_MSR_CPSR(ARM_R1), NULL);
	jtag_arm7_step(tap, 1, ARM_LDM(ARM_R0_PC, ARM_PC), NULL);
	jtag_arm7_step(tap, 2, ct->cpsr, NULL); 
	jtag_arm7_step(tap, 2, ARM_NOP, NULL);
	for (i = 0; i < 15; i++) {
		jtag_arm7_step(tap, 1, ct->r[i], NULL);
	}
	jtag_arm7_step(tap, 1, ct->pc, NULL);
	jtag_arm7_step(tap, 2, ARM_NOP, NULL);
#else
	arm7_context_restore(tap, ct);
#endif
	/* set the single step breakpoint */
	DCC_LOG1(LOG_TRACE, "single step BP @ 0x%08x...", ct->pc - (2 * 4));
	arm_sstp_bp_set(tap, ct->pc - (2 * 4));

	ret = jtag_arm7_step_sys(tap, ARM_NOP, ARM_B(-(6 * 4)));

	DCC_LOG(LOG_TRACE, "disabling single step ...");
	arm_sstp_disable(tap);

	return ret;
}

int arm7_insn_fetch(jtag_tap_t * tap, uint32_t addr, 
					int thumb, uint32_t * insn)
{
	uint32_t tmp;

	DCC_LOG2(LOG_INFO, "addr: 0x%08x (%s)", addr, thumb ? "THUMB" : "ARM");

	jtag_arm7_step(tap, 1, ARM_LDR(ARM_R2, ARM_PC, 0), NULL);
	jtag_arm7_step(tap, 2, ARM_NOP, NULL);
	jtag_arm7_step(tap, 2, addr, NULL);
	jtag_arm7_step(tap, 1, ARM_NOP, NULL);

	/* fetching opcode */
	if (thumb) {
		/* run at system speed */
		jtag_arm7_step_sys(tap, ARM_NOP, ARM_LDRH(ARM_R1, ARM_R2, 2));
		jtag_arm7_step(tap, 1, ARM_STR(ARM_R1, ARM_PC, 0), NULL);
		jtag_arm7_step(tap, 2, ARM_NOP, NULL);
		jtag_arm7_step(tap, 2, ARM_NOP, &tmp);

		*insn = tmp & 0xffff;

		if ((tmp & 0xf800) != 0xf000)
			return 2;

		/* the thumb bl instruction requires two words */
		DCC_LOG(LOG_INFO, "bl instruction");
		jtag_arm7_step_sys(tap, ARM_NOP, ARM_LDRH(ARM_R1, ARM_R2, 0));
		jtag_arm7_step(tap, 1, ARM_STR(ARM_R1, ARM_PC, 0), NULL);
		jtag_arm7_step(tap, 2, ARM_NOP, NULL);
		jtag_arm7_step(tap, 1, 0, &tmp);
		*insn |= tmp << 16;

	} else {
		jtag_arm7_step_sys(tap, ARM_NOP, ARM_LDR(ARM_R1, ARM_R2, 0));
		jtag_arm7_step(tap, 1, ARM_STR(ARM_R1, ARM_PC, 0), NULL);
		jtag_arm7_step(tap, 2, ARM_NOP, NULL);
		jtag_arm7_step(tap, 1, 0, insn);
	}

	return 4;
}

void arm7_spsr_get(jtag_tap_t * tap, unsigned int * val)
{
	jtag_arm7_step(tap, 1, ARM_MRS_SPSR(ARM_R1), NULL);
	jtag_arm7_step(tap, 1, ARM_STR(ARM_R1, ARM_R0, 0), NULL);
	jtag_arm7_step(tap, 2, ARM_NOP, NULL);
	jtag_arm7_step(tap, 1, 0, val);
}

void arm7_spsr_set(jtag_tap_t * tap, unsigned int val)
{
	jtag_arm7_step(tap, 1, ARM_LDR(ARM_R1, ARM_R0, 0), NULL);
	jtag_arm7_step(tap, 1, ARM_MSR_SPSR(ARM_R1), NULL);
	jtag_arm7_step(tap, 1, ARM_NOP, NULL);
	jtag_arm7_step(tap, 2, val, NULL); 
	jtag_arm7_step(tap, 1, ARM_NOP, NULL);
}

int arm7_exec(jtag_tap_t * tap, int thumb)
{
	/* exec */
	if (thumb) {
		jtag_arm7_step(tap, 1, THUMB_NOP, NULL);
		jtag_arm7_step_brk(tap, 1, THUMB_NOP);
		return jtag_arm7_step(tap, 1, THUMB_B(-7 * 2), NULL);
	}
	jtag_arm7_step(tap, 1, ARM_NOP, NULL);
	jtag_arm7_step_brk(tap, 1, ARM_NOP);
	return jtag_arm7_step(tap, 1, ARM_B(-(7 * 4)), NULL);
}

void arm7_code_exec(jtag_tap_t * tap, uint32_t addr, 
					uint32_t * regv, int regc)
{
	int regs;
	int i;

	/* set registers */
	regs = 0;
	for (i = 0; i < regc; i++) {
		regs |= (1 << i);
	}

	jtag_arm7_step(tap, 1, ARM_LDM(regs | (1 << ARM_PC), ARM_R0), NULL);
	jtag_arm7_step(tap, 2, ARM_NOP, NULL);
	for (i = 0; i < regc; i++) {
		jtag_arm7_step(tap, 1, regv[i], NULL);
	}
	/* set PC */
	jtag_arm7_step(tap, 1, addr, NULL);
	jtag_arm7_step(tap, 2, ARM_NOP, NULL);

	/* resume */
	jtag_arm7_step_brk(tap, 1, ARM_NOP);
	jtag_arm7_step(tap, 1, ARM_B(-(4 * 4)), NULL);
}


/**************************************************************
 * THUMB Macros
 **************************************************************/

void arm7_thumb_context_save(jtag_tap_t * tap, unsigned int flags,
							 struct armice_context * ct)
{
	int i;
	uint32_t cpsr;

	DCC_LOG(LOG_TRACE, "changing to ARM");

	/* save R0 and PC */
	jtag_arm7_step(tap, 1, THUMB_STR(ARM_R0, ARM_R0), NULL);
	jtag_arm7_step(tap, 1, THUMB_MOV(ARM_R0, ARM_PC), NULL);
	jtag_arm7_step(tap, 1, THUMB_STR(ARM_R0, ARM_R0), NULL);
	jtag_arm7_step(tap, 1, 0, &ct->r[0]);
	jtag_arm7_step(tap, 2, THUMB_BX(ARM_PC), NULL);
	jtag_arm7_step(tap, 2, THUMB_NOP, &ct->pc);

	DCC_LOG(LOG_INFO, "saving registers..");

	/* save remaining registers */
	jtag_arm7_step(tap, 1, ARM_STM(ARM_R1_R14, ARM_R0), NULL);
	/* save cpsr into R0 */
	jtag_arm7_step(tap, 1, ARM_MRS_CPSR(ARM_R1), NULL);
	/* store CPSR (R0) */
	jtag_arm7_step(tap, 1, ARM_STR(ARM_R1, ARM_R0, 0), NULL);
	for (i = 1; i < 15; i++) {
		jtag_arm7_step(tap, 1, 0, &ct->r[i]);
	}

	/* clear PC */
	jtag_arm7_step(tap, 2, ARM_EOR(ARM_PC, ARM_PC, ARM_PC), NULL);
	jtag_arm7_step(tap, 3, ARM_NOP, &ct->cpsr);

	cpsr = ARM_DISABLE_IRQ | ARM_DISABLE_FIQ | ARM_MODE_SUPERVISOR;
	if ((ct->cpsr & 0xff) != cpsr) {
		DCC_LOG(LOG_TRACE, "changing to Supervisor mode");
		cpsr = ARM_DISABLE_IRQ | ARM_DISABLE_FIQ | ARM_MODE_SUPERVISOR;
		jtag_arm7_step(tap, 1, ARM_MSR_CPSR_IM(cpsr & 0xff, 0, 1), NULL);
		jtag_arm7_step(tap, 2, ARM_NOP, NULL);
	}

	/* adjust CPSR */
	ct->cpsr |= ARM_STATE_THUMB;

	/* adjust PC */
	if (flags & ARMICE_DBGRQ)
		ct->pc -= (3 * 2);
	else
		ct->pc -= (4 * 2);
}

	/* ensure R0 is word aligned */
//	jtag_arm7_step(tap, 1, ARM_EOR(ARM_R0, ARM_R0, ARM_R0), NULL);
void arm7_thumb_context_restore(jtag_tap_t * tap, armice_context_t * ct)
{
	int i;

	DCC_LOG(LOG_TRACE, "restoring registers...");

	if ((ct->cpsr & ARM_STATE_THUMB) == 0) {
		DCC_LOG(LOG_ERROR, "THUMB flag clear???");
	}

	/* restore the context */
	jtag_arm7_step(tap, 1, ARM_LDM(ARM_R1_R2, ARM_PC), NULL);
	jtag_arm7_step(tap, 1, ARM_EOR(ARM_R0, ARM_R0, ARM_R0), NULL);
	jtag_arm7_step(tap, 1, ARM_MSR_CPSR(ARM_R2), NULL);
	/* pc must take into account the 2 instructions needed to 
	 * thumb state switch bellow */
	jtag_arm7_step(tap, 1, (ct->pc - (2 * 2)) | 1, NULL);
	/* restore cpsr with the Thumb flag cleared */
	jtag_arm7_step(tap, 2, ct->cpsr & ~(ARM_STATE_THUMB), NULL); 

	jtag_arm7_step(tap, 1, ARM_LDM(ARM_R2_R14, ARM_R0), NULL);
	jtag_arm7_step(tap, 2, ARM_NOP, NULL);

	/* restore registers r2 to r14 */
	for (i = 2; i < 14; i++) {
		jtag_arm7_step(tap, 1, ct->r[i], NULL);
	}
	/* register 14 and LDM stuffing */
	jtag_arm7_step(tap, 2, ct->lr, NULL);

	/* thumb state switch (6 cycles) */
	jtag_arm7_step(tap, 1, ARM_BX(ARM_R1), NULL);
	jtag_arm7_step(tap, 2, ARM_NOP, NULL);

	DCC_LOG(LOG_TRACE, "back to thumb");

	/* thumb state (1 insn - 3 fetches) */
	/* restore r0-r1 */
	jtag_arm7_step(tap, 1, THUMB_LDMIA(ARM_R0_R1, ARM_R0), NULL);
	jtag_arm7_step(tap, 2, THUMB_NOP, NULL);
	jtag_arm7_step(tap, 1, ct->r[0], NULL);
	jtag_arm7_step(tap, 2, ct->r[1], NULL);
}

int arm7_thumb_insn_step(jtag_tap_t * tap, armice_context_t * ct)
{
	int ret;
#if 0
	int i;

	DCC_LOG1(LOG_INFO, "ct=0x%p", ct);

	/* restore the context */
	jtag_arm7_step(tap, 1, ARM_LDM(ARM_R1_R2, ARM_PC), NULL);
	jtag_arm7_step(tap, 1, ARM_EOR(ARM_R0, ARM_R0, ARM_R0), NULL);
	jtag_arm7_step(tap, 1, ARM_MSR_CPSR(ARM_R2), NULL);
	/* pc must take into account the 2 instructions needed to 
	 * thumb state switch bellow */
	jtag_arm7_step(tap, 1, (ct->pc - (2 * 2)) | 1, NULL);
	/* restore cpsr with the Thumb flag cleared */
	jtag_arm7_step(tap, 2, ct->cpsr & ~(ARM_STATE_THUMB), NULL); 

	jtag_arm7_step(tap, 1, ARM_LDM(ARM_R2_R14, ARM_R0), NULL);
	jtag_arm7_step(tap, 2, ARM_NOP, NULL);

	/* restore registers r2 to r14 */
	for (i = 2; i < 14; i++) {
		jtag_arm7_step(tap, 1, ct->r[i], NULL);
	}
	/* register 14 and LDM stuffing */
	jtag_arm7_step(tap, 2, ct->lr, NULL);

	jtag_arm7_step(tap, 1, ARM_BX(ARM_R1), NULL);
	jtag_arm7_step(tap, 2, ARM_NOP, NULL);

	DCC_LOG(LOG_TRACE, "back to thumb");

	/* thumb state switch (5 instructions) */
	/* restore r0-r1 */
	jtag_arm7_step(tap, 1, THUMB_LDMIA(ARM_R0_R1, ARM_R0), NULL);
	jtag_arm7_step(tap, 2, THUMB_NOP, NULL);
	jtag_arm7_step(tap, 1, ct->r[0], NULL);
	jtag_arm7_step(tap, 2, ct->r[1], NULL);
#else
	arm7_thumb_context_restore(tap, ct);
#endif

	jtag_arm7_step(tap, 1, THUMB_NOP, NULL);

	/* set the single step breakpoint */
	DCC_LOG1(LOG_TRACE, "single step BP @ 0x%08x...", ct->pc - (2 * 2));
	arm_sstp_bp_set(tap, ct->pc - (2 * 2));

	ret = jtag_arm7_step_sys(tap, THUMB_NOP, THUMB_B(-(7 * 2)));

	DCC_LOG(LOG_TRACE, "disabling single step ...");
	arm_sstp_disable(tap);

	return ret;
}

/**************************************************************
 * ARM7 Bulk memory operations
 **************************************************************/

int arm7_mem_read(jtag_tap_t * tap, uint32_t base, 
				  uint32_t offs, void * p, int len)
{
	uint32_t addr;
	uint32_t data;
	unsigned char * cp;
	int cnt;
	int n;
	int j;

	DCC_LOG3(LOG_INFO, "base=0x%08x offs=0x%08x len=%d,", base, offs, len);

	if (len == 0)
		return 0;

	addr = base + offs;

	/* uses R12 as index */
	jtag_arm7_step(tap, 1, ARM_LDR(ARM_R14, ARM_PC, 0), NULL);
	jtag_arm7_step(tap, 2, ARM_NOP, NULL);
	jtag_arm7_step(tap, 2, addr, NULL);

	cp = (unsigned char *)p;
	cnt = 0;

	/* get the first bytes, if the address is not half-word (16bits) aligned */
	if(addr & 0x01) {
		/* run at system speed and increment the base register by one */
		jtag_arm7_step(tap, 1, ARM_NOP, NULL);
		jtag_arm7_step_sys(tap, ARM_NOP, ARM_LDRB(ARM_R0, ARM_R14, 1));

		jtag_arm7_step(tap, 1, ARM_STR(ARM_R0, ARM_PC, 0), NULL);
		jtag_arm7_step(tap, 2, ARM_NOP, NULL);
		jtag_arm7_step(tap, 1, 0, &data);
		*cp++ = data;
		len--;
		addr++;
		cnt++;
	}
	/* get the next word, if the address is not word (32bits) aligned */
	if ((len >= 2) && (addr & 0x02)) {
		/* run at system speed and increment the base register by two */
		jtag_arm7_step(tap, 1, ARM_NOP, NULL);
		jtag_arm7_step_sys(tap, ARM_NOP, ARM_LDRH(ARM_R0, ARM_R14, 2));
		jtag_arm7_step(tap, 1, ARM_STR(ARM_R0, ARM_PC, 0), NULL);
		jtag_arm7_step(tap, 2, ARM_NOP, NULL);
		jtag_arm7_step(tap, 1, 0, &data);
		*cp++ = data;
		*cp++ = data >> 8;
		len -= 2;
		cnt += 2;
	}

	while (len >= 4) {
		/* transfer up to 14 words */
		n = (len / 4) < 14 ? (len / 4) : 14;
		jtag_arm7_step(tap, 1, ARM_NOP, NULL);
		jtag_arm7_step_sys(tap, ARM_NOP, ARM_LDMIA((1 << n) - 1, ARM_R14));
		/* run at system speed and increment the base register by (4 * n) */
		jtag_arm7_step(tap, 1, ARM_STM((1 << n) - 1, ARM_PC), NULL);
		jtag_arm7_step(tap, 2, ARM_NOP, NULL);
		for (j = 0; j < n; j++) {
			jtag_arm7_step(tap, 1, 0, &data);
			*cp++ = data;
			*cp++ = data >> 8;
			*cp++ = data >> 16;
			*cp++ = data >> 24;
		}
		len -= 4 * n;
		cnt += 4 * n; 
	}

	/* get the last word, if the address is not word (32bits) aligned */
	if (len & 0x02) {
		/* run at system speed and increment the base register by two */
		jtag_arm7_step(tap, 1, ARM_NOP, NULL);
		jtag_arm7_step_sys(tap, ARM_NOP, ARM_LDRH(ARM_R0, ARM_R14, 2));
		jtag_arm7_step(tap, 1, ARM_STR(ARM_R0, ARM_PC, 0), NULL);
		jtag_arm7_step(tap, 2, ARM_NOP, NULL);
		jtag_arm7_step(tap, 1, 0, &data);
		*cp++ = data;
		*cp++ = data >> 8;
		cnt += 2;
	}
	/* get the last bytes, if the address is not half-word (16bits) aligned */
	if(len & 0x01) {
		/* run at system speed and increment the base register by one */
		jtag_arm7_step(tap, 1, ARM_NOP, NULL);
		jtag_arm7_step_sys(tap, ARM_NOP, ARM_LDRB(ARM_R0, ARM_R14, 1));
		jtag_arm7_step(tap, 1, ARM_STR(ARM_R0, ARM_PC, 0), NULL);
		jtag_arm7_step(tap, 2, ARM_NOP, NULL);
		jtag_arm7_step(tap, 1, 0, &data);
		*cp = data;
		cnt++;
	}

	return cnt;
}

int arm7_mem_write(jtag_tap_t * tap, uint32_t base, 
				   uint32_t offs, const void * p, int len)
{
	uint32_t addr;
	uint32_t data;
	unsigned char * cp;
	int cnt;
	int n;
	int j;

	DCC_LOG3(LOG_INFO, "base=0x%08x offs=0x%08x len=%d,", base, offs, len);

	if (len == 0)
		return 0;

	addr = base + offs;
	cnt = 0;
	cp = (unsigned char *)p;

	jtag_arm7_step(tap, 1, ARM_LDR(ARM_R14, ARM_PC, 0), NULL);
	jtag_arm7_step(tap, 2, ARM_NOP, NULL);
	jtag_arm7_step(tap, 2, addr, NULL);

	/* get the first bytes, if the address is not half-word (16bits) aligned */
	if (addr & 0x01) {
		data = *cp++;
		jtag_arm7_step(tap, 1, ARM_LDR(ARM_R1, ARM_PC, 0), NULL);
		jtag_arm7_step(tap, 2, ARM_NOP, NULL);
		jtag_arm7_step(tap, 2, data, NULL);	
		/* run at system speed and increment the base register by one */
		jtag_arm7_step(tap, 1, ARM_NOP, NULL);
		jtag_arm7_step_sys(tap, ARM_NOP, ARM_STRB(ARM_R1, ARM_R14, 1));
		len--;
		addr++;
		cnt++;
	}
	/* get the next word, if the address is not word (32bits) aligned */
	if ((len >= 2) && (addr & 0x02)) {
		data = *cp++;
		data += *cp++ << 8;
		jtag_arm7_step(tap, 1, ARM_LDR(ARM_R1, ARM_PC, 0), NULL);
		jtag_arm7_step(tap, 2, ARM_NOP, NULL);
		jtag_arm7_step(tap, 2, data, NULL);	
		/* run at system speed and increment the base register by two */
		jtag_arm7_step(tap, 1, ARM_NOP, NULL);
		jtag_arm7_step_sys(tap, ARM_NOP, ARM_STRH(ARM_R1, ARM_R14, 2));
		len -= 2;
		cnt += 2;
	}

	while (len >= 4) {
		/* transfer up to 14 words */
		n = (len / 4) < 14 ? (len / 4) : 14;

		jtag_arm7_step(tap, 1, ARM_LDM((1 << n) - 1, ARM_PC), NULL);
		jtag_arm7_step(tap, 2, ARM_NOP, NULL);
		for (j = 0; j < n; j++) {
			data = *cp++;
			data += *cp++ << 8;
			data += *cp++ << 16;
			data += *cp++ << 24;
			DCC_LOG1(LOG_INFO, "data=0x%08x", data);
			jtag_arm7_step(tap, 1, data, NULL);
		}

		jtag_arm7_step(tap, 2, ARM_NOP, NULL);
		jtag_arm7_step_sys(tap, ARM_NOP, ARM_STMIA((1 << n) - 1, ARM_R14));

		/* run at system speed and increment the base register by (4 * n) */
		len -= 4 * n;
		cnt += 4 * n; 
	}

	/* get the last word, if the address is not word (32bits) aligned */
	if (len & 0x02) {
		data = *cp++;
		data += *cp++ << 8;
		jtag_arm7_step(tap, 1, ARM_LDR(ARM_R1, ARM_PC, 0), NULL);
		jtag_arm7_step(tap, 2, ARM_NOP, NULL);
		jtag_arm7_step(tap, 2, data, NULL);	
		/* run at system speed and increment the base register by two */
		jtag_arm7_step(tap, 1, ARM_NOP, NULL);
		jtag_arm7_step_sys(tap, ARM_NOP, ARM_STRH(ARM_R1, ARM_R14, 2));
		len -= 2;
		cnt += 2;
	}
	/* get the last bytes, if the address is not half-word (16bits) aligned */
	if (len) {
		data = *cp;
		jtag_arm7_step(tap, 1, ARM_LDR(ARM_R1, ARM_PC, 0), NULL);
		jtag_arm7_step(tap, 2, ARM_NOP, NULL);
		jtag_arm7_step(tap, 2, data, NULL);	
		/* run at system speed and increment the base register by one */
		jtag_arm7_step(tap, 1, ARM_NOP, NULL);
		jtag_arm7_step_sys(tap, ARM_NOP, ARM_STRB(ARM_R1, ARM_R14, 1));
		cnt++;
	}

	return cnt;
}

int arm7_mem_set(jtag_tap_t * tap, uint32_t base, 
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

	jtag_arm7_step(tap, 1, ARM_LDM(ARM_R1_R2, ARM_PC), NULL);
	jtag_arm7_step(tap, 2, ARM_NOP, NULL);
	jtag_arm7_step(tap, 1, data, NULL);
	jtag_arm7_step(tap, 2, addr, NULL);


	/* write the first bytes, 
	   if the address is not half-word (16bits) aligned */
	if (addr & 0x01) {
		/* run at system speed and increment the base register by one */
		jtag_arm7_step(tap, 1, ARM_NOP, NULL);
		jtag_arm7_step_sys(tap, ARM_NOP, ARM_STRB(ARM_R1, ARM_R2, 1));
		len--;
		addr++;
		cnt++;
	}
	/* write the next word, if the address is not word (32bits) aligned */
	if ((len >= 2) && (addr & 0x02)) {
		/* run at system speed and increment the base register by two */
		jtag_arm7_step(tap, 1, ARM_NOP, NULL);
		jtag_arm7_step_sys(tap, ARM_NOP, ARM_STRH(ARM_R1, ARM_R2, 2));
		len -= 2;
		cnt += 2;
	}
	while (len >= 4) {
		/* run at system speed and increment the base register by four */
		jtag_arm7_step(tap, 1, ARM_NOP, NULL);
		jtag_arm7_step_sys(tap, ARM_NOP, ARM_STR(ARM_R1, ARM_R2, 4));
		len -= 4;
		cnt += 4;
	}
	/* write the last word, if the address is not word (32bits) aligned */
	if (len & 0x02) {
		/* run at system speed and increment the base register by two */
		jtag_arm7_step(tap, 1, ARM_NOP, NULL);
		jtag_arm7_step_sys(tap, ARM_NOP, ARM_STRH(ARM_R1, ARM_R2, 2));
		len -= 2;
		cnt += 2;
	}
	/* write the last bytes, if the address is not half-word (16bits) aligned */
	if (len) {
		/* run at system speed and increment the base register by one */
		jtag_arm7_step(tap, 1, ARM_NOP, NULL);
		jtag_arm7_step_sys(tap, ARM_NOP, ARM_STRB(ARM_R1, ARM_R2, 1));
		cnt++;
	}

	return cnt;
}

