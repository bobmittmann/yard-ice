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
 * @file jtag3drv.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#ifdef CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/delay.h>

#include "jtag3drv.h"
#include "dbglog.h"

#include "armice.h"

#ifndef FPGA_RBF_ADDR 
#define FPGA_RBF_ADDR 0x01050000
#endif

const uint8_t * jtag3ctrl_rbf = (uint8_t *)(FPGA_RBF_ADDR);
struct jtag3drv jtag3drv;

/***************************************************************************
 *
 * JTAG - generic operations
 *
 ***************************************************************************/

#define JTAG_FMAIN 60000000

int jtag_tck_freq_set(unsigned int tck_freq)
{

	uint32_t fmain = JTAG_FMAIN;
	int32_t div;

	DCC_LOG1(LOG_TRACE, "freq: %d Hz", tck_freq);
	div = ((fmain + (tck_freq - 1)) / tck_freq);

	if (div < 2)
		div = 2;

	reg_wr(REG_CKGEN_DIV, div - 2);

	__os_sleep(5);

	/* cache the interface tck_freq */
	jtag3drv.tck_freq = fmain / (div + 2);

	DCC_LOG1(LOG_INFO, "brg freq=%d", fmain / (div + 2));

	return JTAG_OK;
}

unsigned int jtag_tck_freq_get(void)
{
	uint32_t fmain = JTAG_FMAIN;
	unsigned int div;

	/* brg divisor */
	div = reg_rd(REG_CKGEN_DIV);

	return fmain / (div + 2);
}

int jtag_rtck_freq_set(unsigned int freq)
{
	uint32_t fmain = JTAG_FMAIN;
	uint32_t div;

	DCC_LOG1(LOG_TRACE, "freq: %d Hz", freq);

	div = (fmain / freq) - 2;

	reg_wr(REG_CKGEN_RTDIV, div);

	return fmain / (div + 2);
}

unsigned int jtag_rtck_freq_get(void)
{
	uint32_t fmain = JTAG_FMAIN;
	unsigned int div;

	DCC_LOG(LOG_TRACE, ".");

	/* brg divisor */
	div = reg_rd(REG_CKGEN_RTDIV);

	return fmain / (div + 2);
}

int jtag_rtck_enable(void)
{
	int cfg = reg_rd(REG_CFG);

	DCC_LOG(LOG_TRACE, ".");

	cfg |= CFG_RTCK_EN;
	reg_wr(REG_CFG, cfg);

	return JTAG_OK;
}

int jtag_rtck_disable(void)
{
	int cfg = reg_rd(REG_CFG);

	DCC_LOG(LOG_TRACE, ".");

	cfg &= ~CFG_RTCK_EN;
	reg_wr(REG_CFG, cfg);

	return JTAG_OK;
}

int jtag_trst(bool assert)
{
	DCC_LOG1(LOG_TRACE, "%s", assert ? "LOW" : "HIGH");
	jtag3ctrl_trst(assert);
	jtag3drv.arm_scan_chain = -1;
	return JTAG_OK;
}

int jtag_nrst(bool assert)
{
	DCC_LOG1(LOG_TRACE, "%s", assert ? "LOW" : "HIGH");

	jtag3ctrl_nrst(assert);
	jtag3drv.arm_scan_chain = -1;
	return JTAG_OK;
}

void jtag_tap_reset(void)
{	
	DCC_LOG(LOG_TRACE, "[-----]");
	insn_tap_reset(5, JTAG_TAP_RESET);
	jtag3drv.arm_scan_chain = -1;
}

void jtag_run_test(int n, unsigned int final_state)
{	
	DCC_LOG2(LOG_TRACE, "cycles: %d --> %s", n, jtag_state_name[final_state]);

	insn_run_test(n, final_state);
}

int jtag_ir_scan(const jtag_vec_t vin, jtag_vec_t vout, 
				 int vlen, unsigned int final_state)
{
	unsigned int isr;
	unsigned int desc = 0;
	unsigned int ptr = 0;
	unsigned int tx_addr = 0;
	unsigned int rx_addr = 0;

	DCC_LOG2(LOG_TRACE, "len=%d state=%s", vlen, jtag_state_name[final_state]);

	/* create a descriptor table with one entry */
	desc_wr(desc, JTAG_DESC(rx_addr, 0, tx_addr, 0, vlen));
	/* create a pointer to the descriptor */
	ptr_wr(ptr, JTAG_PTR(desc, 1));
	/* write the vector in the controller's memory */
	jtag3ctrl_vec_wr(tx_addr, (uint16_t *)vin, vlen);

	/* scan the vector */
	reg_wr(REG_INSN, INSN_IR_SCAN(desc, final_state));
	isr = jtag3drv_int_wait(IRQ_TAP);

	if ((isr & IRQ_TAP) == 0) {
		DCC_LOG1(LOG_WARNING, "isr:0x%02x", isr);
		return -1;
	}

	DCC_LOG(LOG_INFO, ".");

	if (vout != NULL)
		jtag3ctrl_vec_rd(rx_addr, (uint16_t *)vout, vlen);

	return 0;
}

void jtag_ir_pause(int n, unsigned int final_state)
{
	DCC_LOG2(LOG_TRACE, "cycles: %d --> %s", n, jtag_state_name[final_state]);

	insn_ir_pause(n, final_state);
}

int jtag_dr_scan(const jtag_vec_t vin, jtag_vec_t vout, 
				 int vlen, unsigned int final_state)
{
	unsigned int isr;
	unsigned int desc = 0;
	unsigned int ptr = 0;
	unsigned int tx_addr = 0;
	unsigned int rx_addr = 0;

	DCC_LOG2(LOG_TRACE, "len=%d state=%s", vlen, jtag_state_name[final_state]);

	/* create a descriptor table with one entry */
	desc_wr(desc, JTAG_DESC(rx_addr, 0, tx_addr, 0, vlen));
	/* create a pointer to the descriptor */
	ptr_wr(ptr, JTAG_PTR(desc, 1));
	/* write the vector in the controller's memory */
	jtag3ctrl_vec_wr(tx_addr, (uint16_t *)vin, vlen);

	/* scan the vector */
	reg_wr(REG_INSN, INSN_DR_SCAN(desc, final_state));
	isr = jtag3drv_int_wait(IRQ_TAP);

	if ((isr & IRQ_TAP) == 0) {
		DCC_LOG1(LOG_WARNING, "isr:0x%02x", isr);
		return -1;
	}

	DCC_LOG(LOG_INFO, ".");

	if (vout != NULL)
		jtag3ctrl_vec_rd(rx_addr, (uint16_t *)vout, vlen);

	return 0;
}

void jtag_dr_pause(int n, unsigned int final_state)
{
	DCC_LOG2(LOG_TRACE, "cycles: %d --> %s", n, jtag_state_name[final_state]);

	insn_dr_pause(n, final_state);
}

/***************************************************************************
 *
 * Initialization and configuration
 *
 ***************************************************************************/

int jtag_tap_select(jtag_tap_t * tap)
{
	jtag3drv.tap = tap;

	return JTAG_OK;
}

int jtag_drv_init(void)
{
	DCC_LOG1(LOG_TRACE, "RBF=0x%08x", jtag3ctrl_rbf);

	if (jtag3ctrl_init(jtag3ctrl_rbf, 64 * 1024) < 0) {
		return JTAG_ERR_HARDWARE;
	}

	/* enable TAP interrupts */
	reg_wr(REG_INT_EN, IRQ_TAP);
	/* clear interrupts */
	reg_wr(REG_INT_ST, 0xffff);

	return 0;
}

int jtag_drv_done(void)
{
	DCC_LOG(LOG_TRACE, ".");

	return JTAG_OK;
}

