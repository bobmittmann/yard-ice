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
 * @file armice-test.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifdef CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "jtag_arm.h"
#include "armice.h"
#include "arm_pn.h"
#include "dbglog.h"

/*****************************************************************************
 * TEST functions
 ****************************************************************************/

#if (ENABLE_ICE_TEST)

#include <sys/delay.h>

int armice_insn_show(armice_ctrl_t * ctrl, uint32_t addr,
					 struct armice_insn * insn,  FILE * f);

int armice_test_(armice_ctrl_t * ctrl, FILE * f)
{
	uint32_t addr = ctrl->work_addr;
	uint32_t d1;
	uint32_t d2;

	fprintf(f, "\n== ARM ICE testbench ==\n");

	addr = 0x00000000 + (rand() & 0xfc);
	d1 = rand() + rand();
	ctrl->arm_wr32(ctrl->tap, addr, d1);
	ctrl->arm_rd32(ctrl->tap, addr, &d2);
	fprintf(f, " - addr:%08x d1:%08x d2:%08x\n", addr, d1, d2);

	return 0;
}


extern struct armice_codelet codelet_putchar;
extern struct armice_codelet codelet_step;
extern struct armice_codelet codelet_step2;
extern struct armice_codelet codelet_step_thumb;

const char msg[] = "Hello world!\r\n";
void jtag_cache_clr(void);

static int test_save_context(FILE * f, armice_ctrl_t * ctrl, 
							 struct armice_context * ct)
{
	int dbg_status;

	/* get the status of the ICE driver */
	if ((dbg_status = ctrl->jtag_arm_dbg_status(ctrl->tap)) < 0) {
		fprintf(f, " # Debug status error!\n");
		return dbg_status;
	}

	if ((dbg_status & ARMICE_ST_DBGACK) == 0) {
		fprintf(f, " # Core is running!\n");
		return -1;
	}

	if (dbg_status & ARMICE_ST_TBIT) {
		fprintf(f, " - Saving THUMB context <<< <<<\n");
		ctrl->thumb_context_save(ctrl->tap, 0, ct);
	} else {
		fprintf(f, " - Saving ARM context <<< <<<\n");
		ctrl->arm_context_save(ctrl->tap, 0, ct);
	}

//	arm_show_cpsr(f, ct->cpsr);
//	arm_show_regs(f, ct->r);

	return dbg_status;
}

static int test_restore_context(FILE * f, armice_ctrl_t * ctrl, 
							 struct armice_context * ct)
{
	int dbg_status;

	/* get the status of the ICE driver */
	if ((dbg_status = ctrl->jtag_arm_dbg_status(ctrl->tap)) < 0) {
		fprintf(f, " # Debug status error!\n");
		return dbg_status;
	}

	if ((dbg_status & ARMICE_ST_DBGACK) == 0) {
		fprintf(f, " # Core is running!\n");
		return -1;
	}

	if (ct->cpsr & ARM_STATE_THUMB) {
		fprintf(f, " - Restoring THUMB context >>> >>>\n");
		ctrl->thumb_context_restore(ctrl->tap, ct);
	} else {
		fprintf(f, " - Restoring ARM context >>> >>>\n");
		ctrl->arm_context_restore(ctrl->tap, ct);
	}

//	arm_show_cpsr(f, ct->cpsr);
//	arm_show_regs(f, ct->r);

	return dbg_status;
}

/*
 * Perform a single memory write, read compare 
 */
static int test_wr_rd_cmp(FILE * f, armice_ctrl_t * ctrl, 
					  uint32_t addr, uint32_t din)
{
	int dbg_status;
	uint32_t dout;
	int ret;

	/* get the status of the ICE driver */
	if ((dbg_status = ctrl->jtag_arm_dbg_status(ctrl->tap)) < 0) {
		fprintf(f, " # Debug status error!\n");
		return dbg_status;
	}

	if ((dbg_status & ARMICE_ST_DBGACK) == 0) {
		fprintf(f, " # Core is running!\n");
		return -1;
	}

	if ((ret = ctrl->arm_wr32(ctrl->tap, addr, din)) < 0) {
		fprintf(f, "# Error: arm_wr32()!\n");
		return ret;
	}

	if ((ret = ctrl->arm_rd32(ctrl->tap, addr, &dout)) < 0) {
		fprintf(f, " # Error: arm_rd32()!\n");
		return ret;
	}

	if (din != dout) {
		fprintf(f, " # Error: din(%08x) != dount(%08x)!\n", din, dout);
		return ret;
	}

	return 0;
}

void mk_context_rand(FILE * f, struct armice_context * ct, uint32_t mode)
{
	int i;

	/* PC must be half word aligned for THUMB and
	 word aligned for ARM */
	
	if (mode & ARM_STATE_THUMB) {
		fprintf(f, " - Creating a random THUMB context...\n");
		ct->pc = ((uint32_t)rand() + (uint32_t)rand()) & 0xfffffffe;
		ct->cpsr = ARM_STATE_THUMB;
	} else {
		fprintf(f, " - Creating a random ARM context...\n");
		ct->pc = ((uint32_t)rand() + (uint32_t)rand()) & 0xfffffffc;
		ct->cpsr = 0;
	}

	ct->cpsr |= (rand() & 0x0f) << 28;
	ct->cpsr |= mode & 0x1f;
	ct->cpsr |= ARM_DISABLE_IRQ | ARM_DISABLE_FIQ;

	for (i = 0; i < 15; i++) {
		ct->r[i] = (uint32_t)rand() + (uint32_t)rand();
	}
}

uint32_t mk_xpsr_rand(uint32_t mode)
{
	uint32_t xcpsr;

	xcpsr = (rand() & 0x0f) << 28;
	xcpsr |= mode & 0x1f;
	xcpsr |= ARM_DISABLE_IRQ | ARM_DISABLE_FIQ;

	return xcpsr;
}
	
int xcpsr_cmp(FILE * f, uint32_t xpsr0, uint32_t xpsr1)
{
	if (xpsr0 == xpsr1) {
		fprintf(f, " - xPSR0: 0x%08x == xPSR1: 0x%08x\n", xpsr0, xpsr1);
		return 0;
	}

	fprintf(f, "#ERR: xPSR0: 0x%08x != xPSR1: 0x%08x\n", xpsr0, xpsr1);
	return -1;
}

int context_cmp(FILE * f, struct armice_context * ct0, 
				struct armice_context * ct1)
{
	int i;
	int err = 0;

	for (i = 0; i < 16; i++) {
		if (ct0->r[i] != ct1->r[i]) {
			fprintf(f, "# ERROR: R%d -> 0x%08x != 0x%08x\n", 
					i, ct0->r[i], ct1->r[i]);
			err++;
		}
	}

	if (ct0->cpsr != ct1->cpsr) {
		fprintf(f, "# ERROR: CPSR -> 0x%08x != 0x%08x\n", ct0->cpsr, ct1->cpsr);
		err++;
	}

	return err ? -1 : 0;
}

/*
 * Perform memory read and write test
 */
int armice_mem_test(armice_ctrl_t * ctrl, FILE * f)
{
	struct armice_context ct;
	int dbg_status;
	uint32_t addr = ctrl->work_addr;
	int ret;

	fprintf(f, "== MEM TEST ==\n");

	fprintf(f, " - Working address: 0x%08x\n", addr); 

	if ((dbg_status = test_save_context(f, ctrl, &ct)) < 0) {
		return -1;
	}

	if ((ret = test_wr_rd_cmp(f, ctrl, addr, 0x12345678)) < 0)
		return ret;

	if ((ret = test_wr_rd_cmp(f, ctrl, addr, 0x98338f91)) < 0)
		return ret;

	if ((ret = test_wr_rd_cmp(f, ctrl, addr + 4, 0xbd341fdc)) < 0)
		return ret;

	if ((ret = test_wr_rd_cmp(f, ctrl, addr + 8, 0x68a87f9c)) < 0)
		return ret;

	return 0;
}

int armice_context_test(armice_ctrl_t * ctrl, FILE * f)
{
	struct armice_context ct0;
	struct armice_context ct1;

	fprintf(f, "== CONTEXT TEST ==\n");

	mk_context_rand(f, &ct0, ARM_MODE_ABORT);
	if (test_restore_context(f, ctrl, &ct0) < 0)
		return -1;	
	if (test_save_context(f, ctrl, &ct1) < 0)
		return -1;
	if (context_cmp(f, &ct0, &ct1) < 0)
		return -1;

	mk_context_rand(f, &ct0, ARM_MODE_UNDEF);
	if (test_restore_context(f, ctrl, &ct0) < 0)
		return -1;	
	if (test_save_context(f, ctrl, &ct1) < 0)
		return -1;
	if (context_cmp(f, &ct0, &ct1) < 0)
		return -1;

	mk_context_rand(f, &ct0, ARM_MODE_IRQ);
	if (test_restore_context(f, ctrl, &ct0) < 0)
		return -1;	
	if (test_save_context(f, ctrl, &ct1) < 0)
		return -1;
	if (context_cmp(f, &ct0, &ct1) < 0)
		return -1;

	mk_context_rand(f, &ct0, ARM_MODE_FIQ);
	if (test_restore_context(f, ctrl, &ct0) < 0)
		return -1;	
	if (test_save_context(f, ctrl, &ct1) < 0)
		return -1;
	if (context_cmp(f, &ct0, &ct1) < 0)
		return -1;

	mk_context_rand(f, &ct0, ARM_MODE_SUPERVISOR);
	if (test_restore_context(f, ctrl, &ct0) < 0)
		return -1;	
	if (test_save_context(f, ctrl, &ct1) < 0)
		return -1;
	if (context_cmp(f, &ct0, &ct1) < 0)
		return -1;

	mk_context_rand(f, &ct0, ARM_MODE_SYSTEM);
	if (test_restore_context(f, ctrl, &ct0) < 0)
		return -1;	
	if (test_save_context(f, ctrl, &ct1) < 0)
		return -1;
	if (context_cmp(f, &ct0, &ct1) < 0)
		return -1;

	mk_context_rand(f, &ct0, ARM_MODE_USER);
	if (test_restore_context(f, ctrl, &ct0) < 0)
		return -1;	
	if (test_save_context(f, ctrl, &ct1) < 0)
		return -1;
	if (context_cmp(f, &ct0, &ct1) < 0)
		return -1;


	mk_context_rand(f, &ct0, ARM_MODE_ABORT | ARM_STATE_THUMB);
	if (test_restore_context(f, ctrl, &ct0) < 0)
		return -1;	
	if (test_save_context(f, ctrl, &ct1) < 0)
		return -1;
	if (context_cmp(f, &ct0, &ct1) < 0)
		return -1;

	mk_context_rand(f, &ct0, ARM_MODE_UNDEF | ARM_STATE_THUMB);
	if (test_restore_context(f, ctrl, &ct0) < 0)
		return -1;	
	if (test_save_context(f, ctrl, &ct1) < 0)
		return -1;
	if (context_cmp(f, &ct0, &ct1) < 0)
		return -1;

	mk_context_rand(f, &ct0, ARM_MODE_IRQ | ARM_STATE_THUMB);
	if (test_restore_context(f, ctrl, &ct0) < 0)
		return -1;	
	if (test_save_context(f, ctrl, &ct1) < 0)
		return -1;
	if (context_cmp(f, &ct0, &ct1) < 0)
		return -1;

	mk_context_rand(f, &ct0, ARM_MODE_FIQ | ARM_STATE_THUMB);
	if (test_restore_context(f, ctrl, &ct0) < 0)
		return -1;	
	if (test_save_context(f, ctrl, &ct1) < 0)
		return -1;
	if (context_cmp(f, &ct0, &ct1) < 0)
		return -1;

	mk_context_rand(f, &ct0, ARM_MODE_SUPERVISOR | ARM_STATE_THUMB);
	if (test_restore_context(f, ctrl, &ct0) < 0)
		return -1;	
	if (test_save_context(f, ctrl, &ct1) < 0)
		return -1;
	if (context_cmp(f, &ct0, &ct1) < 0)
		return -1;

	mk_context_rand(f, &ct0, ARM_MODE_SYSTEM | ARM_STATE_THUMB);
	if (test_restore_context(f, ctrl, &ct0) < 0)
		return -1;	
	if (test_save_context(f, ctrl, &ct1) < 0)
		return -1;
	if (context_cmp(f, &ct0, &ct1) < 0)
		return -1;

	mk_context_rand(f, &ct0, ARM_MODE_USER | ARM_STATE_THUMB);
	if (test_restore_context(f, ctrl, &ct0) < 0)
		return -1;	
	if (test_save_context(f, ctrl, &ct1) < 0)
		return -1;
	if (context_cmp(f, &ct0, &ct1) < 0)
		return -1;

	fprintf(f, "\n[TEST OK]\n");

	if (test_save_context(f, ctrl, &ct0) < 0)
		return -1;

	fprintf(f, "\n - TARGET context:\n");
	arm_show_cpsr(f, ct1.cpsr);
	arm_show_regs(f, ct1.r);

	fprintf(f, "\n - DEBUG context:\n");
	arm_show_cpsr(f, ct0.cpsr);
	arm_show_regs(f, ct0.r);

	return 0;
}

void arm9_cpsr_get(jtag_tap_t * tap, uint32_t * cpsr);
void arm9_cpsr_set(jtag_tap_t * tap, uint32_t cpsr);

int armice_cpsr_test(armice_ctrl_t * ctrl, FILE * f)
{
	uint32_t cpsr0;
//	uint32_t cpsr1;

	fprintf(f, " == CPSR TEST ==\n");

	cpsr0 = mk_xpsr_rand(ARM_MODE_SYSTEM);

//	arm9_cpsr_get(ctrl->tap, &cpsr1);
//	xcpsr_cmp(f, cpsr0, cpsr1);

//	cpsr0 = mk_xpsr_rand(ARM_MODE_USER);
//	arm9_cpsr_set(ctrl->tap, cpsr0);
//	arm9_cpsr_get(ctrl->tap, &cpsr1);
//	xcpsr_cmp(f, cpsr0, cpsr1);

	return 0;
}


static int show_insn(FILE * f, armice_ctrl_t * ctrl, 
					 struct armice_context * ct)
{
	struct armice_insn insn;
	uint32_t addr;
	int n;

	insn.thumb = (ct->cpsr & ARM_STATE_THUMB) ? 1 : 0;
	addr = ct->pc - ((insn.thumb) ? 4 : 8);

	if ((n = ctrl->insn_fetch(ctrl->tap, addr, insn.thumb, &insn.data)) < 0) {
		fprintf(f, "# Instruction fetch error!!!!\n");
		return n;
	}
	insn.len = n;

//	if (insn.thumb)
//		thumb_show_asm(f, addr, insn.data);
//	else
//		arm_show_asm(f, addr, insn.data);

	fprintf(f, "\n");

	return 0;
}

int armice_codelet_test(armice_ctrl_t * ctrl, FILE * f)
{
	armice_codelet_t * cl = &codelet_step2;
	struct armice_context ct0;
	struct armice_context ct1;
	int dbg_status;
	uint32_t addr = ctrl->work_addr;
	uint32_t reg[16];
	int ret;
	int i;

	fprintf(f, " == ARM CODELET TEST ==\n");

	fprintf(f, " - Loading codelet @ addr:0x%08x, size:%d ...\n", 
			addr, cl->size);

	if ((ret = armice_code_load(ctrl, addr, cl->code, cl->size)) < 0) { 
		fprintf(f, "# Error: arm7ice_code_load()!\n");
		return ret;
	}

	for (i = 0; i < 12; i++) {
		ct0.r[i] = 0xf0000000 + i;
	}
	ct0.sp = 0xdddd0000;
	ct0.lr = 0xeeee0000;
	ct0.pc = 0xffff0000;
	ct0.cpsr = ARM_MODE_SYSTEM | ARM_DISABLE_IRQ | ARM_DISABLE_FIQ;

	fprintf(f, " - Installing a fresh ARM context >>> >>>\n");
	ctrl->arm_context_restore(ctrl->tap,  &ct0);

	reg[0] = 0x00000000;
	reg[1] = 0x11111111;
	reg[2] = 0x22222222;
	reg[3] = 0x33333333;
	reg[4] = 0x44444444;
	reg[5] = 0x55555555;
	reg[6] = 0x66666666;
	reg[7] = 0x77777777;
	reg[8] = 0x88888888;
	reg[9] = 0x99999999;
	reg[10] = 0x10101010;
	reg[11] = 0x11111111;
	reg[12] = 0x12121212;
	reg[12] = 0x13131313;


	if ((ret = armice_code_exec(ctrl, addr, reg, 13)) < 0) { 
		fprintf(f, " # ERROR: armice_code_exec()!\n");
		return ret;
	}

	if ((dbg_status = ctrl->jtag_arm_dbg_status(ctrl->tap)) < 0) {
		DCC_LOG1(LOG_ERROR, "arm7ice_get_status(): %d", dbg_status);
		return dbg_status;
	}

	if ((dbg_status & ARMICE_ST_DBGACK) != 0) {
		fprintf(f, " # ERROR: Core is stopped!!!!!!!\n");
	}

	fprintf(f, " - Wait 10 ms...\n");
	uthread_sleep(10);

	armice_code_stop(ctrl);

	if ((dbg_status = test_save_context(f, ctrl, &ct0)) < 0)
		return -1;

	if (dbg_status & ARMICE_ST_TBIT) {
		fprintf(f, " #ERROR: THUMB state!!\n");
		return -1;
	}

	fprintf(f, " - TARGET context, before step:\n");
	arm_show_cpsr(f, ct0.cpsr);
	arm_show_regs(f, ct0.r);
	show_insn(f, ctrl, &ct0);

	fprintf(f, " - Step @ 0x%08x:\n", addr);

	fprintf(f, " - ARM step ...\n");
	if ((ret = ctrl->arm_insn_step(ctrl->tap, &ct0)) < 0) {
		fprintf(f, "# ARM step error!!!!\n");
		return -1;
	}

	if ((dbg_status = test_save_context(f, ctrl, &ct1)) < 0)
		return -1;

	fprintf(f, " - TARGET context, after step:\n");
	arm_show_cpsr(f, ct1.cpsr);
	arm_show_regs(f, ct1.r);
	show_insn(f, ctrl, &ct1);

	return 0;
}

int armice_thumb_codelet_test(armice_ctrl_t * ctrl, FILE * f)
{
	armice_codelet_t * cl = &codelet_step_thumb;
	struct armice_context ct0;
	struct armice_context ct1;
	int dbg_status;
	uint32_t addr = ctrl->work_addr;
	uint32_t reg[16];
	int ret;

	fprintf(f, " == THUMB CODELET TEST ==\n");

	if (test_save_context(f, ctrl, &ct0) < 0)
		return -1;

	fprintf(f, "- Loading codelet: %p, size:%d ...\n", cl, cl->size);

	if ((ret = armice_code_load(ctrl, addr, cl->code, cl->size)) < 0) { 
		fprintf(f, "# Error: arm7ice_code_load()!\n");
		return ret;
	}

	reg[0] = 0x0000;
	reg[1] = 0x11111111;
	reg[2] = 0x22222222;
	reg[3] = 0x33333333;
	reg[4] = 0x44444444;
	reg[5] = 0x55555555;
	reg[6] = 0x66666666;
	reg[7] = 0x77777777;
	reg[8] = 0x88888888;
	reg[9] = 0x99999999;
	reg[10] = 0x10101010;
	reg[11] = 0x11111111;
	reg[12] = 0x12121212;

	if ((ret = armice_code_exec(ctrl, addr, reg, 13)) < 0) { 
		fprintf(f, "# Error: armice_code_exec()!\n");
		return ret;
	}

	if ((dbg_status = ctrl->jtag_arm_dbg_status(ctrl->tap)) < 0) {
		DCC_LOG1(LOG_ERROR, "arm7ice_get_status(): %d", dbg_status);
		return dbg_status;
	}

	if ((dbg_status & ARMICE_ST_DBGACK) != 0) {
		fprintf(f, "# Core is stopped!!!!!!!\n");
	}

	fprintf(f, " - Wait 50 ms...\n");
	uthread_sleep(50);

	armice_code_stop(ctrl);

	if (test_save_context(f, ctrl, &ct0) < 0)
		return -1;
	arm_show_cpsr(f, ct0.cpsr);
	arm_show_regs(f, ct0.r);

	fprintf(f, " - TARGET context, before step:\n");
	arm_show_cpsr(f, ct0.cpsr);
	arm_show_regs(f, ct0.r);
	show_insn(f, ctrl, &ct0);

	fprintf(f, " - Step @ 0x%08x:\n", addr);

	if (ct0.cpsr & ARM_STATE_THUMB) {
		fprintf(f, " - THUMB step ...\n");
		if ((ret = ctrl->thumb_insn_step(ctrl->tap, &ct0)) < 0) {
			fprintf(f, "# THUMB step error!!!!\n");
			return -1;
		}
	} else {
		fprintf(f, " - ARM step ...\n");
		if ((ret = ctrl->arm_insn_step(ctrl->tap, &ct0)) < 0) {
			fprintf(f, "# ARM step error!!!!\n");
			return -1;
		}
	}

	if (test_save_context(f, ctrl, &ct1) < 0)
		return -1;

	fprintf(f, " - TARGET context, after step:\n");
	arm_show_cpsr(f, ct1.cpsr);
	arm_show_regs(f, ct1.r);
	show_insn(f, ctrl, &ct1);

	return 0;
}

int armice_cp15_test(armice_ctrl_t * ctrl, FILE * f)
{
	uint32_t data;
	uint32_t cp15_ctrl;

	fprintf(f, "== CP15 TEST ==\n");

	jtag_arm926_cp15_rd(ctrl->tap, CP15_C0_IDCODE, &data);
	fprintf(f, " - IDCODE = %08x\n", data);

	jtag_arm926_cp15_rd(ctrl->tap, CP15_C0_CACHE_TYPE, &data);
	fprintf(f, " - ICahe: %d, DCache: %d\n", 
			CACHE_SIZE(C0_ISIZE(data)), CACHE_SIZE(C0_DSIZE(data)));

	jtag_arm926_cp15_rd(ctrl->tap, CP15_C0_TCM_TYPE, &data);
	fprintf(f, " - CP15 DTCM:%d ITCM:%d\n", 
			C0_TCM_DTCM(data), C0_TCM_ITCM(data));

	jtag_arm926_cp15_rd(ctrl->tap, CP15_C1_CONTROL, &cp15_ctrl);

	fprintf(f, " - CP15 C1 ICache:%s DCache:%s MMU:%s\n", 
			(cp15_ctrl & C1_ICACHE) ? "Enabled" : "Disabled",
			(cp15_ctrl & C1_DCACHE) ? "Enabled" : "Disabled",
			(cp15_ctrl & C1_MMU) ? "Enabled" : "Disabled");


	jtag_arm926_cp15_rd(ctrl->tap, CP15_ADDR(7, 0, 15, 0), &data);
	data |= 0x7;
	jtag_arm926_cp15_wr(ctrl->tap, CP15_ADDR(7, 0, 15, 0), data);


	if (cp15_ctrl & C1_MMU) {
		/* invalidate TLB */
		fprintf(f, " - Invalidate TLB...\n");
		jtag_arm926_cp15_wr(ctrl->tap, CP15_ADDR(0, 0, 8, 7), 0x00000000);
		cp15_ctrl &= ~C1_MMU;
	}

	if (cp15_ctrl & C1_DCACHE) {
		uint32_t dbg_ovr;
		/* read-modify-write CP15 debug override register
		 * to enable "test and clean all" */
		jtag_arm926_cp15_rd(ctrl->tap, CP15_C15_DBG_OVR, &dbg_ovr);
		jtag_arm926_cp15_wr(ctrl->tap, CP15_C15_DBG_OVR, dbg_ovr | 0x80000);

		fprintf(f, " - Invalidate DCache...\n");
		jtag_arm926_cp15_wr(ctrl->tap, CP15_ADDR(0, 0, 7, 5), 0x00000000);

		/* write CP15 debug override register
		 * to disable "test and clean all" */
		jtag_arm926_cp15_wr(ctrl->tap, CP15_C15_DBG_OVR, dbg_ovr);

		cp15_ctrl &= ~C1_DCACHE;
	}

	if (cp15_ctrl & C1_ICACHE) {
		fprintf(f, " - Invalidate ICache...\n");
		jtag_arm926_cp15_wr(ctrl->tap, CP15_ADDR(0, 0, 7, 5), 0x00000000);
		cp15_ctrl &= ~C1_ICACHE;
	}


	jtag_arm926_cp15_wr(ctrl->tap, CP15_C1_CONTROL, cp15_ctrl);

	arm9_cpsr_set(ctrl->tap, ARM_DISABLE_IRQ | ARM_DISABLE_FIQ | 
				  ARM_MODE_SYSTEM);

	return 0;
}

int armice_wp_dump(armice_ctrl_t * ctrl, FILE * f)
{
	uint32_t data_mask;
	uint32_t data_value;
	uint32_t addr_mask;
	uint32_t addr_value;
	uint32_t ctrl_mask;
	uint32_t ctrl_value;
	jtag_tap_t * tap = ctrl->tap;

	fprintf(f, "== WP DUMP ==\n");

	jtag_arm_ice_rt_rd(tap, ARMICE_WP0_ADDR_MASK, &addr_mask);
	jtag_arm_ice_rt_rd(tap, ARMICE_WP0_ADDR_VALUE, &addr_value);
	jtag_arm_ice_rt_rd(tap, ARMICE_WP0_DATA_MASK, &data_mask);
	jtag_arm_ice_rt_rd(tap, ARMICE_WP0_DATA_VALUE, &data_value);
	jtag_arm_ice_rt_rd(tap, ARMICE_WP0_CTRL_MASK, &ctrl_mask);
	jtag_arm_ice_rt_rd(tap, ARMICE_WP0_CTRL_VALUE, &ctrl_value);

	fprintf(f, " - WP0:\n");
	fprintf(f, "   - addr=<0x%08x 0x%08x>\n", addr_value, addr_mask);
	fprintf(f, "   - data=<0x%08x 0x%08x>\n", data_value, data_mask);
	fprintf(f, "   - ctlr=<0x%04x 0x%04x>\n", ctrl_value, ctrl_mask); 

	jtag_arm_ice_rt_rd(tap, ARMICE_WP1_ADDR_MASK, &addr_mask);
	jtag_arm_ice_rt_rd(tap, ARMICE_WP1_ADDR_VALUE, &addr_value);
	jtag_arm_ice_rt_rd(tap, ARMICE_WP1_DATA_MASK, &data_mask);
	jtag_arm_ice_rt_rd(tap, ARMICE_WP1_DATA_VALUE, &data_value);
	jtag_arm_ice_rt_rd(tap, ARMICE_WP1_CTRL_MASK, &ctrl_mask);
	jtag_arm_ice_rt_rd(tap, ARMICE_WP1_CTRL_VALUE, &ctrl_value);


	fprintf(f, " - WP1:\n");
	fprintf(f, "   - addr=<0x%08x 0x%08x>\n", addr_value, addr_mask);
	fprintf(f, "   - data=<0x%08x 0x%08x>\n", data_value, data_mask);
	fprintf(f, "   - ctlr=<0x%04x 0x%04x>\n", ctrl_value, ctrl_mask); 

	return 0;
}

void armice_context_update(armice_ctrl_t * ctrl, armice_context_t * ct);

int armice_test(armice_ctrl_t * ctrl, FILE * f, uint32_t val)
{
//	struct armice_context * ct = &ctrl->context;

//	core_state_save(ctrl, ct);

	fprintf(f, "- Test %d ...\n", val);

	switch (val) {
	case 1:
		return armice_context_test(ctrl, f);
	case 2:
		return armice_mem_test(ctrl, f);
	case 3:
		return armice_codelet_test(ctrl, f);
	case 4:
		return armice_thumb_codelet_test(ctrl, f);
	case 5:
		return armice_cpsr_test(ctrl, f);
	case 6:
		return armice_cp15_test(ctrl, f);
	case 7:
		return armice_wp_dump(ctrl, f);
	default:
		fprintf(f, "# Valid test numbers:\n");
		fprintf(f, "  1 - context\n");
		fprintf(f, "  2 - memory\n");
		fprintf(f, "  3 - ARM codelet\n");
		fprintf(f, "  4 - THUMB codelet\n");
		fprintf(f, "  5 - CPSR\n");
		fprintf(f, "  6 - C15\n");
		fprintf(f, "  7 - WP dump\n");
		return -1;
	};

	return 0;
}

#else

int armice_test(armice_ctrl_t * ctrl, FILE * f, uint32_t val)
{
	uint32_t dbg_ctrl;
	int status;

	if ((status = ctrl->jtag_arm_dbg_status(ctrl->tap)) < 0) {
		return status;
	}

	fprintf(f, " - DBG status: %08x\n", status);
	jtag_arm_dbg_ctrl_rd(ctrl->tap, &dbg_ctrl);
	fprintf(f, " - DBG ctrl: %08x\n", dbg_ctrl);

	return 0;
}

#endif
