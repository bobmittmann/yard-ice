/* 
 *
 * File:	arm7.h
 * Module:  armice	
 * Project:
 * Author:	Robinson Mittmann (bob@boreste.com)
 * Target:
 * Comment:	
 * Copyright(c) 2003-2006 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/clock.h>
#include <arch/at91x40.h>

#include "ice_drv.h"
#include "arm7ice.h"
#include "arm_opc.h"

#ifdef ARM7ICE_DEBUG
#ifndef DEBUG
#define DEBUG
#endif
#endif
#include <debug.h>

#include <sys/dcclog.h>

/**************************************************************
 * ARM7 Macros (Layer II)
 **************************************************************/

int arm7_wr8(struct armice_ctrl * ctrl, unsigned int addr, unsigned char data);

int arm7_rd8(struct armice_ctrl * ctrl, unsigned int addr, 
			unsigned char * data);

int arm7_wr16(struct armice_ctrl * ctrl, unsigned int addr, 
			 unsigned short data);

int arm7_rd16(struct armice_ctrl * ctrl, unsigned int addr, 
			 unsigned short * data);
int arm7_wr32(struct armice_ctrl * ctrl, unsigned int addr, unsigned int data);

int arm7_rd32(struct armice_ctrl * ctrl, unsigned int addr, 
			 unsigned int * data);

void arm7_context_save(struct armice_ctrl * ctrl, struct armice_context * ct);

int arm7_insn_step(struct armice_ctrl * ctrl, struct armice_context * ct)

void arm7_context_restore(struct armice_ctrl * ctrl, 
								const struct armice_context * ct)
void arm7_exec(struct armice_ctrl * ctrl)

void arm7_spsr_get(struct armice_ctrl * ctrl, unsigned int * val)

void arm7_spsr_set(struct armice_ctrl * ctrl, unsigned int val)

/**************************************************************
 * THUMB Macros
 **************************************************************/

void arm7_thumb_context_save(struct armice_ctrl * ctrl, 
							   struct armice_context * ct)
{
	uint32_t insn;
	int i;

	DCC_LOG(LOG_INFO, "changing to ARM");

	/* save r0 */
	jtag_arm7_step(1, THUMB_PUSH((1 << ARM_R0), 0), NULL);
	jtag_arm7_step(1, THUMB_MOV(ARM_R0, ARM_PC), NULL);
	jtag_arm7_step(1, THUMB_PUSH((1 << ARM_R0), 0), NULL);
	jtag_arm7_step(1, 0, &ct->r[0]);
	jtag_arm7_step(2, THUMB_BX(ARM_PC), NULL);
	jtag_arm7_step(2, THUMB_NOP, &ct->pc);

	/* adjust PC */
	if (ctrl->ice_flags & ARM7ICE_DBGRQ)
		ct->pc -= (3 * 2);
	else
		ct->pc -= (4 * 2);

	DCC_LOG(LOG_INFO, "saving registers");

	/* save remaining registers */
	jtag_arm7_step(1, ARM_STM(ARM_R1_R14, ARM_SP), NULL);
	/* adjust fetch addr */
	jtag_arm7_step(1, ARM_SUB(ARM_R1, ARM_PC, 28), NULL);
	/* save cpsr */
	jtag_arm7_step(1, ARM_MRS(ARM_R3), NULL);
	for (i = 1; i < 15; i++) {
		jtag_arm7_step(1, 0, &ct->r[i]);
	}
	/* adjust sp */
	ct->sp += 8;

	jtag_arm7_step(1, ARM_STR(ARM_R3, ARM_SP, 0), NULL);
	/* clear r0 */
	jtag_arm7_step(2, ARM_EOR(ARM_SP, ARM_SP, ARM_SP), NULL);
	jtag_arm7_step(2, ARM_NOP, &ct->cpsr);
	/* fetching opcode */
	jtag_arm7_step_brk(1, ARM_NOP);
	jtag_arm7_step_sync(1, ARM_LDRH(ARM_R2, ARM_R1, 2));

	jtag_arm7_step(1, ARM_STR(ARM_R2, ARM_SP, 0), NULL);
	jtag_arm7_step(2, ARM_NOP, NULL);
	jtag_arm7_step(1, 0, &insn);
	ct->insn = insn & 0xffff;

	if ((insn & 0xf800) == 0xf000) {
		/* the thumb bl instruction requires two words */
		DCC_LOG(LOG_INFO, "bl instruction");
		jtag_arm7_step(1, ARM_NOP, NULL);
		jtag_arm7_step_brk(1, ARM_NOP);
		jtag_arm7_step_sync(1, ARM_LDRH(ARM_R2, ARM_R1, 0));
		jtag_arm7_step(1, ARM_STR(ARM_R2, ARM_SP, 0), NULL);
		jtag_arm7_step(2, ARM_NOP, NULL);
		jtag_arm7_step(1, 0, &insn);
		ct->insn |= insn << 16;
	}

	ct->cpsr |= ARM_STATE_THUMB;
}

int arm7_thumb_insn_step(struct armice_ctrl * ctrl, 
						 struct armice_context * ct)
{
	struct thumb_opcode * opc;
	unsigned int insn;
	unsigned int status;
	int ret;
	int n;
	int i;

	/* restore the context */
	jtag_arm7_step(1, ARM_LDR(ARM_R0, ARM_SP, 0), NULL);
	jtag_arm7_step(1, ARM_MSR(ARM_R0), NULL);
	jtag_arm7_step(1, ARM_LDM(ARM_R1_R14, ARM_SP), NULL);
	/* restore cpsr with the Thumb flag cleared */
	jtag_arm7_step(2, ct->cpsr & ~(ARM_STATE_THUMB), NULL); 
	jtag_arm7_step(2, ARM_NOP, NULL);
	/* restore registers r2 to r14 */
	for (i = 1; i < 14; i++) {
		jtag_arm7_step(1, ct->r[i], NULL);
	}
	/* register 14 and LDM stuffing */
	jtag_arm7_step(2, ct->lr, NULL);

	insn = ct->insn & 0xffff;

	jtag_arm7_step(1, ARM_LDR(ARM_R0, ARM_SP, 0), NULL);
	jtag_arm7_step(1, ARM_BX(ARM_R0), NULL);
	jtag_arm7_step(1, ARM_NOP, NULL);

	for (opc = (struct thumb_opcode *)thumb_opcodes; 
		 (insn & opc->mask) != opc->value; opc++) {
		if (opc->mask == 0x0000) {
			DCC_LOG(LOG_WARNING, "??");
			return -1;
		}
	}

	if (opc->system) {
		DCC_LOG(LOG_TRACE, "system");
		/* system exec */
		jtag_arm7_step(2, (ct->pc - (7 * 2)) | 1, NULL);
		jtag_arm7_step(1, ARM_NOP, NULL);
		/* thumb state switch */
		/* restore r0 */
		jtag_arm7_step(1, THUMB_LDR_SP(ARM_R0, 0), NULL);
		jtag_arm7_step(2, THUMB_NOP, NULL);
		jtag_arm7_step(1, ct->r[0], NULL);

		/* adjust stack pointer */
		/* exec */
		jtag_arm7_step(2, THUMB_NOP, NULL);
		jtag_arm7_step_brk(1, THUMB_NOP);
		/* copy the thumb code to both halves of the bus */
		if (jtag_arm7_step_sync(1, insn | (insn << 16)) != ARMICE_OK) {
			DCC_LOG(LOG_WARNING, "jtag_arm7_step_sync() fail.");
			return -1;
		}

		return 0;
	};

	DCC_LOG2(LOG_TRACE, "core %d: insn=%04x", opc->internal, insn);

	/* core exec */
	jtag_arm7_step(2, (ct->pc - (5 * 2)) | 1, NULL);
	jtag_arm7_step(1, ARM_NOP, NULL);
	/* thumb state switch */
	/* restore r0 */
	jtag_arm7_step(1, THUMB_LDR_SP(ARM_R0, 0), NULL);
	jtag_arm7_step(2, THUMB_NOP, NULL);
	jtag_arm7_step(2, ct->r[0], NULL);

	/* exec */
	jtag_arm7_step(1, insn | (insn << 16), NULL);

	if ((insn & 0xf800) == 0xf000) {
		DCC_LOG(LOG_TRACE, "bl");
		insn = ct->insn >> 16;
		jtag_arm7_step(1, insn | (insn << 16), NULL);
		jtag_arm7_step(2, THUMB_NOP, NULL);
		jtag_arm7_step_brk(1, THUMB_NOP);
		if (jtag_arm7_step_sync(1, THUMB_B(-6)) != ARMICE_OK) {
			DCC_LOG(LOG_WARNING, "jtag_arm7_step_sync() fail.");
			return -1;
		}

	}

	if ((insn & 0xff00) == 0xdf00) {
		DCC_LOG(LOG_TRACE, "swi");
		jtag_arm7_step(2, THUMB_NOP, NULL);
		/* arm switch */
		jtag_arm7_step(1, ARM_NOP, NULL);
		jtag_arm7_step_brk(1, ARM_NOP);
		if (jtag_arm7_step_sync(1, ARM_B(-16)) != ARMICE_OK) {
			DCC_LOG(LOG_WARNING, "jtag_arm7_step_sync() fail.");
			return -1;
		}
	}

	if ((insn & 0xff80) == 0x4700) {
		DCC_LOG(LOG_TRACE, "bx");
		jtag_arm7_step(2, THUMB_NOP, NULL);

		do { 
			if ((ret = arm7ice_get_status(&status)) != ARMICE_OK) {
				DCC_LOG1(LOG_ERROR, "arm7ice_get_status() err: %d", ret);
				return -1;
			}
		} while (((status & (ARMICE_ST_DBGACK | ARMICE_ST_NMREQ)) ^ 
				  (ARMICE_ST_DBGACK | ARMICE_ST_NMREQ)) != 0);

		/* force a system sync */
		if (status & ARMICE_ST_TBIT) {
			jtag_arm7_step(1, THUMB_NOP, NULL);
			jtag_arm7_step_brk(1, THUMB_NOP);
			jtag_arm7_step_sync(1, THUMB_B(-8));
		} else {
			/* arm switch */
			jtag_arm7_step(1, ARM_NOP, NULL);
			jtag_arm7_step_brk(1, ARM_NOP);
			jtag_arm7_step_sync(1, ARM_B(-16));
		}

		return 0;
	}

	n = opc->internal;

	if (((insn & 0xffc0) == 0x4100) || ((insn & 0xffc0) == 0x41c0) ||
		((insn & 0xffc0) == 0x40c0) || ((insn & 0xffc0) == 0x4080)) {
		DCC_LOG(LOG_TRACE, "register shift");
		n++;
	}

	jtag_arm7_step(n, THUMB_B(-2), NULL); /* force a pipeline flushing */
	jtag_arm7_step(1, THUMB_NOP, NULL);
	jtag_arm7_step_brk(1, THUMB_NOP);
	if (jtag_arm7_step_sync(1, THUMB_B(-8)) != ARMICE_OK) {
		DCC_LOG(LOG_WARNING, "jtag_arm7_step_sync() fail.");
		return -1;
	}

	return 0;
}

void arm7_thumb_context_restore(struct armice_ctrl * ctrl, 
								const struct armice_context * ct)
{
	int i;

	DCC_LOG1(LOG_INFO, "ct=0x%p", ct);

	/* restore the context */
	jtag_arm7_step(1, ARM_LDR(ARM_R0, ARM_SP, 0), NULL);
	jtag_arm7_step(1, ARM_MSR(ARM_R0), NULL);
	jtag_arm7_step(1, ARM_LDM(ARM_R1_R14, ARM_SP), NULL);
	/* restore cpsr with the Thumb flag cleared */
	jtag_arm7_step(2, ct->cpsr & ~(ARM_STATE_THUMB), NULL); 
	jtag_arm7_step(2, ARM_NOP, NULL);

	/* restore registers r2 to r14 */
	for (i = 1; i < 14; i++) {
		jtag_arm7_step(1, ct->r[i], NULL);
	}
	/* register 14 and LDM stuffing */
	jtag_arm7_step(2, ct->lr, NULL);
	jtag_arm7_step(1, ARM_LDR(ARM_R0, ARM_SP, 0), NULL);
	jtag_arm7_step(1, ARM_BX(ARM_R0), NULL);
	jtag_arm7_step(1, ARM_NOP, NULL);
	/* pc must take in account the 4 instructions needed to 
	 * thumb state switch bellow */
	jtag_arm7_step(2, (ct->pc - (3 * 2)) | 1, NULL);
	jtag_arm7_step(1, ARM_NOP, NULL);

	DCC_LOG(LOG_TRACE, "back to thumb");

	/* thumb state switch (4 instructions) */
	/* restore r0 */
	jtag_arm7_step(1, THUMB_LDR_SP(ARM_R0, 0), NULL);
	jtag_arm7_step(2, THUMB_NOP, NULL);
	jtag_arm7_step(1, ct->r[0], NULL);

	jtag_arm7_step(2, THUMB_NOP, NULL);
}

void  arm7_thumb_exec(struct armice_ctrl * ctrl)
{
	/* exec */
	jtag_arm7_step_brk(1, THUMB_NOP);
	jtag_arm7_step(1, THUMB_B(-6 * 2), NULL);
}

/**************************************************************
 * ARM7 Bulk memory operations
 **************************************************************/

int arm7_mem_read(const ice_drv_t * ice, uint32_t base, 
			   uint32_t offs, void * p, int len)
{
	uint32_t addr;
	uint32_t data;
	unsigned char * cp;
	int n;

	DCC_LOG3(LOG_INFO, "base=0x%08x offs=0x%08x len=%d,", base, offs, len);

	if (len == 0)
		return 0;

	addr = base + offs;

	/* uses R12 as index */
	jtag_arm7_step(1, ARM_LDR(ARM_R12, ARM_SP, 0), NULL);
	jtag_arm7_step(2, ARM_NOP, NULL);
	jtag_arm7_step(2, addr, NULL);

	cp = (unsigned char *)p;
	n = 0;

	/* get the first bytes, if the address is not half-word (16bits) aligned */
	if(addr & 0x01) {
		/* run at system speed and increment the base register by one */
		jtag_arm7_step(1, ARM_NOP, NULL);
		jtag_arm7_step_brk(1, ARM_NOP);
		jtag_arm7_step_sync(1, ARM_LDRB(ARM_R1, ARM_R12, 1));

		jtag_arm7_step(1, ARM_STR(ARM_R1, ARM_SP, 0), NULL);
		jtag_arm7_step(2, ARM_NOP, NULL);
		jtag_arm7_step(1, 0, &data);
		*cp++ = data;
		len--;
		addr++;
		n++;
	}
	/* get the next word, if the address is not word (32bits) aligned */
	if ((len >= 2) && (addr & 0x02)) {
		/* run at system speed and increment the base register by two */
		jtag_arm7_step(1, ARM_NOP, NULL);
		jtag_arm7_step_brk(1, ARM_NOP);
		jtag_arm7_step_sync(1, ARM_LDRH(ARM_R1, ARM_R12, 2));
		jtag_arm7_step(1, ARM_STR(ARM_R1, ARM_SP, 0), NULL);
		jtag_arm7_step(2, ARM_NOP, NULL);
		jtag_arm7_step(1, 0, &data);
		*cp++ = data;
		*cp++ = data >> 8;
		len -= 2;
		n += 2;
	}
	for (; len >= 4; len -= 4) {
		/* run at system speed and increment the base register by four */
		jtag_arm7_step(1, ARM_NOP, NULL);
		jtag_arm7_step_brk(1, ARM_NOP);
		jtag_arm7_step_sync(1, ARM_LDR(ARM_R1, ARM_R12, 4));
		jtag_arm7_step(1, ARM_STR(ARM_R1, ARM_SP, 0), NULL);
		jtag_arm7_step(2, ARM_NOP, NULL);
		jtag_arm7_step(1, 0, &data);
		*cp++ = data;
		*cp++ = data >> 8;
		*cp++ = data >> 16;
		*cp++ = data >> 24;
		n += 4;
	}
	/* get the last word, if the address is not word (32bits) aligned */
	if (len & 0x02) {
		/* run at system speed and increment the base register by two */
		jtag_arm7_step(1, ARM_NOP, NULL);
		jtag_arm7_step_brk(1, ARM_NOP);
		jtag_arm7_step_sync(1, ARM_LDRH(ARM_R1, ARM_R12, 2));
		jtag_arm7_step(1, ARM_STR(ARM_R1, ARM_SP, 0), NULL);
		jtag_arm7_step(2, ARM_NOP, NULL);
		jtag_arm7_step(1, 0, &data);
		*cp++ = data;
		*cp++ = data >> 8;
		n += 2;
	}
	/* get the last bytes, if the address is not half-word (16bits) aligned */
	if(len & 0x01) {
		/* run at system speed and increment the base register by one */
		jtag_arm7_step(1, ARM_NOP, NULL);
		jtag_arm7_step_brk(1, ARM_NOP);
		jtag_arm7_step_sync(1, ARM_LDRB(ARM_R1, ARM_R12, 1));
		jtag_arm7_step(1, ARM_STR(ARM_R1, ARM_SP, 0), NULL);
		jtag_arm7_step(2, ARM_NOP, NULL);
		jtag_arm7_step(1, 0, &data);
		*cp = data;
		n ++;
	}

	return n;
}

int arm7_mem_write(const ice_drv_t * ice, uint32_t base, 
				   uint32_t offs, const void * p, int len)
{
	uint32_t addr;
	uint32_t data;
	unsigned char * cp;
	int n;

	DCC_LOG3(LOG_INFO, "base=0x%08x offs=0x%08x len=%d,", base, offs, len);

	if (len == 0)
		return 0;

	addr = base + offs;
	n = 0;
	cp = (unsigned char *)p;

	jtag_arm7_step(1, ARM_LDR(ARM_R12, ARM_SP, 0), NULL);
	jtag_arm7_step(2, ARM_NOP, NULL);
	jtag_arm7_step(2, addr, NULL);

	/* get the first bytes, if the address is not half-word (16bits) aligned */
	if (addr & 0x01) {
		data = *cp++;
		jtag_arm7_step(1, ARM_LDR(ARM_R1, ARM_SP, 0), NULL);
		jtag_arm7_step(2, ARM_NOP, NULL);
		jtag_arm7_step(2, data, NULL);	
		/* run at system speed and increment the base register by one */
		jtag_arm7_step(1, ARM_NOP, NULL);
		jtag_arm7_step_brk(1, ARM_NOP);
		jtag_arm7_step_sync(1, ARM_STRB(ARM_R1, ARM_R12, 1));
		len--;
		addr++;
		n++;
	}
	/* get the next word, if the address is not word (32bits) aligned */
	if ((len >= 2) && (addr & 0x02)) {
		data = *cp++;
		data += *cp++ << 8;
		jtag_arm7_step(1, ARM_LDR(ARM_R1, ARM_SP, 0), NULL);
		jtag_arm7_step(2, ARM_NOP, NULL);
		jtag_arm7_step(2, data, NULL);	
		/* run at system speed and increment the base register by two */
		jtag_arm7_step(1, ARM_NOP, NULL);
		jtag_arm7_step_brk(1, ARM_NOP);
		jtag_arm7_step_sync(1, ARM_STRH(ARM_R1, ARM_R12, 2));
		len -= 2;
		n += 2;
	}
	while (len >= 4) {
		data = *cp++;
		data += *cp++ << 8;
		data += *cp++ << 16;
		data += *cp++ << 24;
		jtag_arm7_step(1, ARM_LDR(ARM_R1, ARM_SP, 0), NULL);
		jtag_arm7_step(2, ARM_NOP, NULL);
		jtag_arm7_step(2, data, NULL);
		/* run at system speed and increment the base register by four */
		jtag_arm7_step(1, ARM_NOP, NULL);
		jtag_arm7_step_brk(1, ARM_NOP);
		jtag_arm7_step_sync(1, ARM_STR(ARM_R1, ARM_R12, 4));
		len -= 4;
		n += 4;
	}
	/* get the last word, if the address is not word (32bits) aligned */
	if (len & 0x02) {
		data = *cp++;
		data += *cp++ << 8;
		jtag_arm7_step(1, ARM_LDR(ARM_R1, ARM_SP, 0), NULL);
		jtag_arm7_step(2, ARM_NOP, NULL);
		jtag_arm7_step(2, data, NULL);	
		/* run at system speed and increment the base register by two */
		jtag_arm7_step(1, ARM_NOP, NULL);
		jtag_arm7_step_brk(1, ARM_NOP);
		jtag_arm7_step_sync(1, ARM_STRH(ARM_R1, ARM_R12, 2));
		len -= 2;
		n += 2;
	}
	/* get the last bytes, if the address is not half-word (16bits) aligned */
	if (len) {
		data = *cp;
		jtag_arm7_step(1, ARM_LDR(ARM_R1, ARM_SP, 0), NULL);
		jtag_arm7_step(2, ARM_NOP, NULL);
		jtag_arm7_step(2, data, NULL);	
		/* run at system speed and increment the base register by one */
		jtag_arm7_step(1, ARM_NOP, NULL);
		jtag_arm7_step_brk(1, ARM_NOP);
		jtag_arm7_step_sync(1, ARM_STRB(ARM_R1, ARM_R12, 1));
		n++;
	}

	return n;
}

int arm7_mem_set(const ice_drv_t * ice, uint32_t base, 
				 uint32_t offs, int len)
{
	uint32_t addr;
	uint32_t data;
	unsigned int val = 0;
	int n;

	DCC_LOG3(LOG_TRACE, "base=0x%08x offs=0x%08x len=%d,", base, offs, len);

	if (len == 0)
		return 0;

	addr = base + offs;
	val &= 0xff;
	data = val | (val << 8) | (val << 16) | (val << 24);
	n = 0;

	jtag_arm7_step(1, ARM_LDR(ARM_R12, ARM_SP, 0), NULL);
	jtag_arm7_step(2, ARM_NOP, NULL);
	jtag_arm7_step(2, addr, NULL);

	jtag_arm7_step(1, ARM_LDR(ARM_R1, ARM_SP, 0), NULL);
	jtag_arm7_step(2, ARM_NOP, NULL);
	jtag_arm7_step(2, data, NULL);	

	/* write the first bytes, 
	   if the address is not half-word (16bits) aligned */
	if (addr & 0x01) {
		/* run at system speed and increment the base register by one */
		jtag_arm7_step(1, ARM_NOP, NULL);
		jtag_arm7_step_brk(1, ARM_NOP);
		jtag_arm7_step_sync(1, ARM_STRB(ARM_R1, ARM_R12, 1));
		len--;
		addr++;
		n++;
	}
	/* write the next word, if the address is not word (32bits) aligned */
	if ((len >= 2) && (addr & 0x02)) {
		/* run at system speed and increment the base register by two */
		jtag_arm7_step(1, ARM_NOP, NULL);
		jtag_arm7_step_brk(1, ARM_NOP);
		jtag_arm7_step_sync(1, ARM_STRH(ARM_R1, ARM_R12, 2));
		len -= 2;
		n += 2;
	}
	while (len >= 4) {
		/* run at system speed and increment the base register by four */
		jtag_arm7_step(1, ARM_NOP, NULL);
		jtag_arm7_step_brk(1, ARM_NOP);
		jtag_arm7_step_sync(1, ARM_STR(ARM_R1, ARM_R12, 4));
		len -= 4;
		n += 2;
	}
	/* write the last word, if the address is not word (32bits) aligned */
	if (len & 0x02) {
		/* run at system speed and increment the base register by two */
		jtag_arm7_step(1, ARM_NOP, NULL);
		jtag_arm7_step_brk(1, ARM_NOP);
		jtag_arm7_step_sync(1, ARM_STRH(ARM_R1, ARM_R12, 2));
		len -= 2;
		n += 2;
	}
	/* write the last bytes, if the address is not half-word (16bits) aligned */
	if (len) {
		/* run at system speed and increment the base register by one */
		jtag_arm7_step(1, ARM_NOP, NULL);
		jtag_arm7_step_brk(1, ARM_NOP);
		jtag_arm7_step_sync(1, ARM_STRB(ARM_R1, ARM_R12, 1));
		n++;
	}

	return n;
}

