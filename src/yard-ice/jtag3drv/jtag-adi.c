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
 * @file .c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#ifdef CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/clock.h>
#include <sys/delay.h>
#include <errno.h>

#include "jtag3drv.h"
#include "dbglog.h"

#include "jtag_adi.h"

#ifndef JTAG_VEC_BITS_MAX 
#error "JTAG_VEC_BITS_MAX undefined"
#endif

/* OUTPUT vectors (common) */

/* a scratch vector to send and receive data */
#define VEC_DATA_ADDR 0x0000
#define VEC_DATA_LEN (JTAG_VEC_BITS_MAX + 7 / 8)

/* a vector of all ones */
#define VEC_ONES (VEC_DATA_ADDR + VEC_DATA_LEN)
#define VEC_ONES_LEN (JTAG_VEC_BITS_MAX + 7 / 8)

/* a vector of all zeros */
#define VEC_ZEROS (VEC_ONES + VEC_ONES_LEN)
#define VEC_ZEROS_LEN (JTAG_VEC_BITS_MAX + 7 / 8)


/* INPUT vectors (common) */

/* a discard area to receive unused data */
#define VEC_DISCARD (VEC_DATA_ADDR + VEC_DATA_LEN)
#define VEC_DISCARD_LEN (JTAG_VEC_BITS_MAX + 7 / 8)

/* an area to receive the result of IR scans for the actvie TAP  */
#define VEC_IR_RESULT (VEC_DISCARD + VEC_DISCARD_LEN)
#define VEC_IR_RESULT_LEN 4


/* IR vectors base address */
#define IR_VECTORS_BASE_ADDR (VEC_ZEROS + VEC_ZEROS_LEN)


/* ADI vectors */

#define DR_VECTORS_BASE_ADDR (IR_VECTORS_BASE_ADDR + 16)


#define VEC_DPACC_DATA (DR_VECTORS_BASE_ADDR)
#define VEC_DPACC_ADDR (VEC_DPACC_DATA + 4)
#define VEC_DPACC_ACK VEC_DPACC_ADDR

#define VEC_APACC_DATA (VEC_DPACC_ADDR + 4)
#define VEC_APACC_ADDR (VEC_APACC_DATA + 4)
#define VEC_APACC_ACK VEC_APACC_ADDR

#define VEC_ABORT (VEC_APACC_ADDR + 4)


/* IR descriptors */
#define PTR_SCRATCH    0
#define ADI_IR_ABORT   1
#define ADI_IR_DPACC   2
#define ADI_IR_APACC   3


/* DR descriptors */
#define ADI_DR_DPACC   4
#define ADI_DR_APACC   5
#define ADI_DR_ABORT   6


struct ir_vec_info {
	uint8_t id;
	uint32_t vec;
};

const struct ir_vec_info ir_vector[] = {
	{ .id = ADI_IR_ABORT, .vec = 0x08 },
	{ .id = ADI_IR_DPACC, .vec = 0x0a },
	{ .id = ADI_IR_APACC, .vec = 0x0b }
};

typedef struct ir_vec_info ir_vec_info_t; 

#define DR_VECTORS_BASE (IR_VECTORS_BASE + 0x08)

static const uint16_t dr_vectors[] = {
};

/* IR vector alocation */
static uint32_t ir_vec_addr = IR_VECTORS_BASE_ADDR;
static uint32_t vec_desc_addr = 1;

struct jtag3drv_desc_tab {
	uint8_t ptr_addr;
	uint8_t tab_len;
	uint8_t tab_addr;
	uint32_t desc[32];	
};

typedef struct jtag3drv_desc_tab jtag3drv_desc_tab_t;

static void desc_add(jtag3drv_desc_tab_t * tab, uint32_t rx_addr, 
					 uint32_t tx_addr, unsigned int vec_len)
{
	if (vec_len > 0) {
		tab->desc[tab->tab_len++] = JTAG_DESC(rx_addr, 0, 
											  tx_addr, 0, vec_len);
	}
}

static void desc_tab_new(jtag3drv_desc_tab_t * tab, uint32_t ptr_addr)
{
	DCC_LOG1(LOG_MSG, "ptr=%d", ptr_addr);
	tab->ptr_addr = ptr_addr;
	tab->tab_len = 0;
	tab->tab_addr = vec_desc_addr;

}

static void desc_tab_insert(jtag_tap_t * tap, jtag3drv_desc_tab_t * tab)
{
	int i;
	uint32_t desc = tab->tab_addr;

	DCC_LOG3(LOG_TRACE, "tab_addr=%0x tab_len=%d ptr_addr=%d", 
			 tab->tab_addr, tab->tab_len, tab->ptr_addr);

	for (i = 0; i < tab->tab_len; i++) {
		desc_wr(desc++, tab->desc[i]);
	}

	ptr_wr(tab->ptr_addr, JTAG_PTR(tab->tab_addr, tab->tab_len));
	/* update the descriptor allocation reference */
	vec_desc_addr = tab->tab_addr + tab->tab_len;
};

static void jtag3drv_ir_vec_add(jtag_tap_t * tap, unsigned int desc_ptr, 
								uint32_t vec)
{
	uint32_t addr = ir_vec_addr;
	uint32_t desc = vec_desc_addr;
	int tab_len = 1;
	int n;

	DCC_LOG4(LOG_TRACE, "IR vec=%0x addr=%03x desc=%03x ptr=%d", 
			 vec, addr, desc, desc_ptr);

	/* how many bytes do we need ? The vectors must be word aligned 
	 so we multiply by two. */
	n = ((tap->irlen + 7) / 8) * 2;

	/* copy the vector to the controller's memory...  */
	vec_wr32(ir_vec_addr, vec);

	if (tap->preirlen > 0) {
		/* BYPASS */
		desc_wr(addr, JTAG_DESC(VEC_DISCARD, 0, VEC_ONES, 0, tap->preirlen));
		desc++;
		tab_len++;
	}

	desc_wr(desc, JTAG_DESC(VEC_IR_RESULT, 0, ir_vec_addr, 0, tap->irlen));

	if (tap->posirlen > 0) {
		/* BYPASS */
		desc++;
		desc_wr(desc, JTAG_DESC(VEC_DISCARD, 0, VEC_ONES, 0, tap->posirlen));
		desc++;
		tab_len++;
	}

	ptr_wr(desc_ptr, JTAG_PTR(vec_desc_addr, tab_len));

	ir_vec_addr += n;
	vec_desc_addr = desc + 1;
}


void jtag3drv_ir_init(jtag_tap_t * tap, 
					  ir_vec_info_t * info, int count)
{
	while (count) {
		jtag3drv_ir_vec_add(tap, info->id, info->vec);
		count--;
	}
}


int jtag_adi_tap_setup(jtag_tap_t * tap)
{
	int vaddr;
	jtag3drv_desc_tab_t desc_tab;

	/* initialize the IR vector alocation variables */
	ir_vec_addr = IR_VECTORS_BASE_ADDR;
	vec_desc_addr = 1;


	/* initialize ones vector */
	for (vaddr = VEC_ONES; vaddr < (VEC_ONES + VEC_ONES_LEN); 
		 vaddr += 4) {
		vec_wr32(vaddr, 0xffffff);
	}

	/* initialize zeros vector */
	for (vaddr = VEC_ZEROS; vaddr < (VEC_ZEROS + VEC_ZEROS_LEN); 
		 vaddr += 4) {
		vec_wr32(vaddr, 0);
	}

	/* initialize the IR vector poll */
	/* create IR vector descriptors and pointres */
	jtag3drv_ir_vec_add(tap, ADI_IR_ABORT, JTAG_ADI_IR_ABORT);
	jtag3drv_ir_vec_add(tap, ADI_IR_DPACC, JTAG_ADI_IR_DPACC);
	jtag3drv_ir_vec_add(tap, ADI_IR_APACC, JTAG_ADI_IR_APACC);

	/* initialize the DR vector poll */

	DCC_LOG2(LOG_TRACE, "tap->precnt=%d top->poscnt=%d", 
			 tap->precnt, tap->poscnt);
	DCC_LOG2(LOG_TRACE, "tap->preirlen=%d top->posirlen=%d", 
			 tap->preirlen, tap->posirlen);

	desc_tab_new(&desc_tab, ADI_DR_DPACC);
	desc_add(&desc_tab, VEC_DISCARD, VEC_ZEROS, tap->precnt);
	desc_add(&desc_tab, VEC_APACC_ACK, VEC_DPACC_ADDR, 3);
	desc_add(&desc_tab, VEC_DPACC_DATA, VEC_DPACC_DATA, 32);
	desc_add(&desc_tab, VEC_DISCARD, VEC_ZEROS, tap->poscnt);
	desc_tab_insert(tap, &desc_tab);

	desc_tab_new(&desc_tab, ADI_DR_APACC);
	desc_add(&desc_tab, VEC_DISCARD, VEC_ZEROS, tap->precnt);
	desc_add(&desc_tab, VEC_APACC_ACK, VEC_APACC_ADDR, 3);
	desc_add(&desc_tab, VEC_APACC_DATA, VEC_APACC_DATA, 32);
	desc_add(&desc_tab, VEC_DISCARD, VEC_ZEROS, tap->poscnt);
	desc_tab_insert(tap, &desc_tab);

	desc_tab_new(&desc_tab, ADI_DR_ABORT);
	desc_add(&desc_tab, VEC_DISCARD, VEC_ZEROS, tap->precnt);
	desc_add(&desc_tab, VEC_ABORT, VEC_ABORT, 35);
	desc_add(&desc_tab, VEC_DISCARD, VEC_ZEROS, tap->poscnt);
	desc_tab_insert(tap, &desc_tab);

	/* preinitializes the abort vector */
 	vec_wr32(VEC_ABORT, 0x00000008);
 	vec_wr32(VEC_ABORT + 4, 0x00000000);

#if 0
	jtag3ctrl_vec_wr(DR_VECTORS_BASE, dr_vectors, sizeof(dr_vectors));

	DCC_LOG1(LOG_TRACE, "DR vectors table: %d bytes", sizeof(dr_vectors));
	DCC_LOG2(LOG_TRACE, "DR vectors 0x%04x..0x%04x ", DR_VECTORS_BASE,
			 DR_VECTORS_BASE + sizeof(dr_vectors));
#endif

	return JTAG_OK;
}

int8_t jtag_adi_ir = -1;

/*************************************************************************/
/* low level JTAG access operations                                      */
/*************************************************************************/

int jtag_adi_abort(jtag_tap_t * tap)
{
	if (jtag_adi_ir !=  ADI_IR_ABORT) {
		insn_irscan(ADI_IR_ABORT, JTAG_TAP_IRUPDATE);
		if ((vec_rd16(VEC_IR_RESULT) & tap->irmask) != 0x0001) {
			DCC_LOG(LOG_ERROR, "ADI_IR_ABORT fail!");
			return JTAG_ERR_IR_SCAN;
		}
		jtag_adi_ir = ADI_IR_ABORT;
	}

	DCC_LOG(LOG_TRACE, "/!\\ ABORT /!\\");
	insn_drscan(ADI_DR_ABORT, JTAG_TAP_DRUPDATE);

	return JTAG_OK;
}


int jtag_adi_dpacc_rd(jtag_tap_t * tap, uint32_t addr, 
					  uint32_t din, uint32_t * dout)
{
	int opt = ((addr >> 1) | 1) & 0x7;
	int retry = 0;
	int ack;

 	vec_wr32(VEC_DPACC_DATA, din);
 	vec_wr16(VEC_DPACC_ADDR, opt);

	if (jtag_adi_ir !=  ADI_IR_DPACC) {
		insn_irscan(ADI_IR_DPACC, JTAG_TAP_IRUPDATE);
		if ((vec_rd16(VEC_IR_RESULT) & tap->irmask) != 0x0001) {
			DCC_LOG(LOG_ERROR, "ADI_IR_DPACC! fail");
			return JTAG_ERR_IR_SCAN;
		}
		jtag_adi_ir = ADI_IR_DPACC;
	}

	DCC_LOG2(LOG_MSG, "DPACC: 0x%08x,%x", din, opt);

	for (;;) {
		insn_drscan(ADI_DR_DPACC, JTAG_TAP_DRUPDATE);
		ack = vec_rd16(VEC_APACC_ACK) & 0x07;
		if (ack == JTAG_ADI_ACK_OK_FAULT) {
			DCC_LOG(LOG_MSG, "ACK=OK/FAULT");
			if (dout != NULL)
				*dout = vec_rd32(VEC_DPACC_DATA);
			break;
		}
		if (ack == JTAG_ADI_ACK_WAIT) {
			if (++retry == 8192) {
				DCC_LOG(LOG_WARNING, "ACK=WAIT - timeout!!!");
				jtag_adi_abort(tap);
				break;
			}
		} else {
			DCC_LOG1(LOG_ERROR, "ACK=%0x", ack);
			return JTAG_ERR_ADI_ACK_INVALID;
		}
	}

	return ack;
}

int jtag_adi_dpacc_wr(jtag_tap_t * tap, uint32_t addr, 
					  uint32_t din, uint32_t * dout)
{
	int opt = ((addr >> 1) & ~1) & 0x7;
	int retry = 0;
	int ack;

 	vec_wr32(VEC_DPACC_DATA, din);
 	vec_wr16(VEC_DPACC_ADDR, opt);

	if (jtag_adi_ir !=  ADI_IR_DPACC) {
		insn_irscan(ADI_IR_DPACC, JTAG_TAP_IRUPDATE);
		if ((vec_rd16(VEC_IR_RESULT) & tap->irmask) != 0x0001) {
			DCC_LOG(LOG_WARNING, "ADI_IR_DPACC! fail");
			return JTAG_ERR_IR_SCAN;
		}
		jtag_adi_ir = ADI_IR_DPACC;
	}

	DCC_LOG2(LOG_INFO, "DPACC: 0x%08x,%x", din, opt);

	for (;;) {
		insn_drscan(ADI_DR_DPACC, JTAG_TAP_DRUPDATE);
		ack = vec_rd16(VEC_APACC_ACK) & 0x07;
		if (ack == JTAG_ADI_ACK_OK_FAULT) {
			DCC_LOG(LOG_MSG, "ACK=OK/FAULT");
			if (dout != NULL)
				*dout = vec_rd32(VEC_DPACC_DATA);
			break;
		}
		if (ack == JTAG_ADI_ACK_WAIT) {
			if (++retry == 8192) {
				DCC_LOG(LOG_WARNING, "ACK=WAIT - timeout!!!");
				jtag_adi_abort(tap);
				break;
			}
		} else {
			DCC_LOG1(LOG_ERROR, "ACK=%0x", ack);
			return JTAG_ERR_ADI_ACK_INVALID;
		}
	}

	return ack;
}

int jtag_adi_apacc_rd(jtag_tap_t * tap, uint32_t addr, 
					  uint32_t din, uint32_t * dout)
{
	int opt = ((addr >> 1) | 1) & 0x7;
	int retry = 0;
	int ack;

 	vec_wr32(VEC_APACC_DATA, din);
 	vec_wr16(VEC_APACC_ADDR, opt);

	if (jtag_adi_ir !=  ADI_IR_APACC) {
		insn_irscan(ADI_IR_APACC, JTAG_TAP_IRUPDATE);
		if ((vec_rd16(VEC_IR_RESULT) & tap->irmask) != 0x0001) {
			DCC_LOG(LOG_WARNING, "ADI_IR_DPACC fail!");
			return JTAG_ERR_IR_SCAN;
		}
		jtag_adi_ir = ADI_IR_APACC;
	}

	DCC_LOG2(LOG_MSG, "APACC: 0x%08x,%x", din, opt);

	for (;;) {
		insn_drscan(ADI_DR_APACC, JTAG_TAP_DRUPDATE);
		ack = vec_rd16(VEC_APACC_ACK) & 0x07;
		if (ack == JTAG_ADI_ACK_OK_FAULT) {
			DCC_LOG(LOG_MSG, "ACK=OK/FAULT");
			if (dout != NULL)
 				*dout = vec_rd32(VEC_APACC_DATA);
			break;
		}
		if (ack == JTAG_ADI_ACK_WAIT) {
			if (++retry == 8192) {
				DCC_LOG(LOG_WARNING, "ACK=WAIT - timeout!!!");
				jtag_adi_abort(tap);
				break;
			}
		} else {
			DCC_LOG1(LOG_ERROR, "ACK=%0x", ack);
			return JTAG_ERR_ADI_ACK_INVALID;
		}
	}

	return ack;
}

int jtag_adi_apacc_wr(jtag_tap_t * tap, uint32_t addr, 
					  uint32_t din, uint32_t * dout)
{
	int opt = ((addr >> 1) & ~1) & 0x7;
	int retry = 8;
	int ack;

 	vec_wr32(VEC_APACC_DATA, din);
 	vec_wr16(VEC_APACC_ADDR, opt);

	if (jtag_adi_ir !=  ADI_IR_APACC) {
		insn_irscan(ADI_IR_APACC, JTAG_TAP_IRUPDATE);
		if ((vec_rd16(VEC_IR_RESULT) & tap->irmask) != 0x0001) {
			DCC_LOG(LOG_ERROR, "ADI_IR_DPACC fail!");
			return JTAG_ERR_IR_SCAN;
		}
		jtag_adi_ir = ADI_IR_APACC;
	}

	DCC_LOG2(LOG_INFO, "APACC: 0x%08x,%x", din, opt);

	for (;;) {
		insn_drscan(ADI_DR_APACC, JTAG_TAP_DRUPDATE);
		ack = vec_rd16(VEC_APACC_ACK) & 0x07;
		if (ack == JTAG_ADI_ACK_OK_FAULT) {
			DCC_LOG(LOG_MSG, "ACK=OK/FAULT");
			if (dout != NULL)
 				*dout = vec_rd32(VEC_APACC_DATA);
			break;
		}
		if (ack == JTAG_ADI_ACK_WAIT) {
			if (++retry == 8192) {
				DCC_LOG(LOG_WARNING, "ACK=WAIT - timeout!!!");
				jtag_adi_abort(tap);
				break;
			}
		} else {
			DCC_LOG1(LOG_ERROR, "ACK=%0x", ack);
			return JTAG_ERR_ADI_ACK_INVALID;
		}
	} 

	return ack;
}

/*************************************************************************/
/* Debug port (DP) operations                                            */
/*************************************************************************/

/* Debug port select cache */
uint32_t adi_dp_reg_select = 0;
uint32_t adi_dp_reg_ctrl_stat = 0;
int adi_dp_ctrl_stat_ready = 0;

int jtag_dp_ctrl_set(jtag_tap_t * tap, uint32_t bitmap)
{
	adi_dp_ctrl_stat_ready = 1;
	adi_dp_reg_ctrl_stat |= bitmap;
	return jtag_adi_dpacc_wr(tap, ADI_DP_CTRL_STAT, adi_dp_reg_ctrl_stat, NULL);
}

int jtag_dp_ctrl_clr(jtag_tap_t * tap, uint32_t bitmap)
{
	adi_dp_ctrl_stat_ready = 1;
	adi_dp_reg_ctrl_stat &= ~bitmap;
	return jtag_adi_dpacc_wr(tap, ADI_DP_CTRL_STAT, adi_dp_reg_ctrl_stat, NULL);
}

uint32_t jtag_dp_ctrl_stat_get(jtag_tap_t * tap)
{
	if (!adi_dp_ctrl_stat_ready) {
		jtag_adi_dpacc_rd(tap, ADI_DP_CTRL_STAT, 0, NULL);
		adi_dp_ctrl_stat_ready = 1;
	}

	jtag_adi_dpacc_rd(tap, ADI_DP_CTRL_STAT, 0, &adi_dp_reg_ctrl_stat);

	DCC_LOG1(LOG_INFO, "DP CTRL/STAT=0x%08x", adi_dp_reg_ctrl_stat);

	return adi_dp_reg_ctrl_stat;
}

int jtag_dp_sys_pwr_up(jtag_tap_t * tap)
{
	int retry = 8;

	if ((adi_dp_reg_ctrl_stat & (DP_CSYSPWRUPREQ | DP_CSYSPWRUPACK)) != 
		(DP_CSYSPWRUPREQ | DP_CSYSPWRUPACK)) {
		DCC_LOG(LOG_TRACE, "Powering up the system domain...");
		/* Power up the debug domain */
		adi_dp_reg_ctrl_stat |= DP_CSYSPWRUPREQ | DP_STICKYERR;
		jtag_adi_dpacc_wr(tap, ADI_DP_CTRL_STAT, adi_dp_reg_ctrl_stat, NULL);
		jtag_adi_dpacc_rd(tap, ADI_DP_CTRL_STAT, 0, NULL);
		do {
			if (--retry == 0) {
				DCC_LOG(LOG_ERROR, "timeout!!!");
				return JTAG_ERR_ADI_WAIT_TIMEOUT;
			}
			jtag_adi_dpacc_rd(tap, ADI_DP_CTRL_STAT, 0, &adi_dp_reg_ctrl_stat);
		} while (!(adi_dp_reg_ctrl_stat & DP_CSYSPWRUPACK));
	}

	return JTAG_OK;
}

int jtag_dp_sys_pwr_down(jtag_tap_t * tap)
{
	int retry = 8;

	if ((adi_dp_reg_ctrl_stat & DP_CSYSPWRUPACK)) {
		DCC_LOG(LOG_TRACE, "Powering down the system domain...");
		/* Power up the debug domain */
		adi_dp_reg_ctrl_stat &= ~DP_CSYSPWRUPREQ;
		jtag_adi_dpacc_wr(tap, ADI_DP_CTRL_STAT, adi_dp_reg_ctrl_stat, NULL);
		jtag_adi_dpacc_rd(tap, ADI_DP_CTRL_STAT, 0, NULL);
		do {
			if (--retry == 0) {
				DCC_LOG(LOG_ERROR, "timeout!!!");
				return JTAG_ERR_ADI_WAIT_TIMEOUT;
			}
			jtag_adi_dpacc_rd(tap, ADI_DP_CTRL_STAT, 0, &adi_dp_reg_ctrl_stat);
		} while ((adi_dp_reg_ctrl_stat & DP_CSYSPWRUPACK));
	}

	return JTAG_OK;
}

int jtag_dp_dbg_pwr_up(jtag_tap_t * tap)
{
	int retry = 8;

	if ((adi_dp_reg_ctrl_stat & (DP_CDBGPWRUPREQ | DP_CDBGPWRUPACK)) != 
		(DP_CDBGPWRUPREQ | DP_CDBGPWRUPACK)) {
		DCC_LOG(LOG_TRACE, "Powering up the debug domain...");
		/* Power up the debug domain */
		adi_dp_reg_ctrl_stat |= DP_CDBGPWRUPREQ | DP_STICKYERR;
		jtag_adi_dpacc_wr(tap, ADI_DP_CTRL_STAT, adi_dp_reg_ctrl_stat, NULL);
		jtag_adi_dpacc_rd(tap, ADI_DP_CTRL_STAT, 0, NULL);
		do {
			if (--retry == 0) {
				DCC_LOG(LOG_ERROR, "timeout!!!");
				return JTAG_ERR_ADI_WAIT_TIMEOUT;
			}
			jtag_adi_dpacc_rd(tap, ADI_DP_CTRL_STAT, 0, &adi_dp_reg_ctrl_stat);
		} while (!(adi_dp_reg_ctrl_stat & DP_CDBGPWRUPACK));
	}

	return JTAG_OK;
}

int jtag_dp_dbg_pwr_down(jtag_tap_t * tap)
{
	int retry = 8;

	if ((adi_dp_reg_ctrl_stat & DP_CDBGPWRUPACK)) {
		DCC_LOG(LOG_TRACE, "Powering down the debug domain...");
		/* Power up the debug domain */
		adi_dp_reg_ctrl_stat &= ~DP_CDBGPWRUPREQ;
		jtag_adi_dpacc_wr(tap, ADI_DP_CTRL_STAT, adi_dp_reg_ctrl_stat, NULL);
		jtag_adi_dpacc_rd(tap, ADI_DP_CTRL_STAT, 0, NULL);
		do {
			if (--retry == 0) {
				DCC_LOG(LOG_ERROR, "timeout!!!");
				return JTAG_ERR_ADI_WAIT_TIMEOUT;
			}
			jtag_adi_dpacc_rd(tap, ADI_DP_CTRL_STAT, 0, &adi_dp_reg_ctrl_stat);
		} while ((adi_dp_reg_ctrl_stat & DP_CDBGPWRUPACK));
	}

	return JTAG_OK;
}

int jtag_dp_dbg_reset(jtag_tap_t * tap)
{
	int retry = 64;

	DCC_LOG(LOG_TRACE, "Resetting the debug domain...");

	/* assert the debug reset request bit and
	 clear any possible error */
	adi_dp_reg_ctrl_stat |= DP_CDBGRSTREQ | DP_STICKYERR;
	jtag_adi_dpacc_wr(tap, ADI_DP_CTRL_STAT, adi_dp_reg_ctrl_stat, NULL);
	jtag_adi_dpacc_rd(tap, ADI_DP_CTRL_STAT, 0, NULL);
	do {
		if (--retry == 0) {
			DCC_LOG(LOG_ERROR, "timeout!!!");
			return JTAG_ERR_ADI_WAIT_TIMEOUT;
		}
		jtag_adi_dpacc_rd(tap, ADI_DP_CTRL_STAT, 0, &adi_dp_reg_ctrl_stat);
	} while (!(adi_dp_reg_ctrl_stat & DP_CDBGRSTACK));

	/* clear the debug reset request bit */
	adi_dp_reg_ctrl_stat &= ~DP_CDBGRSTREQ;
	jtag_adi_dpacc_wr(tap, ADI_DP_CTRL_STAT, adi_dp_reg_ctrl_stat, NULL);

	return 0;
}

int jtag_dp_ap_select(jtag_tap_t * tap, unsigned int ap_id) 
{
	uint32_t diff;

	DCC_LOG1(LOG_TRACE, "AP=%d", ap_id);  

	/* if 'ap_id' and the APSEL field int the SELECT registers are
	   different update the SELECT register */
	if ((diff = DP_APSEL((ap_id << 24) ^ adi_dp_reg_select))) {
		adi_dp_reg_select ^= diff;
		DCC_LOG1(LOG_TRACE, "DP SELECT=0x%08x", adi_dp_reg_select);
		return jtag_adi_dpacc_wr(tap, ADI_DP_SELECT, adi_dp_reg_select, NULL);
	}

	return JTAG_OK;
}

/* JTAG-DP initialization and configuration  */

int jtag_dp_init(jtag_tap_t * tap)
{
	uint32_t st;

	jtag_adi_ir = -1;

	/* read DP select register */
	jtag_adi_dpacc_rd(tap, ADI_DP_SELECT, 0, NULL);
	/* read DP status register */
	jtag_adi_dpacc_rd(tap, ADI_DP_CTRL_STAT, 0, &adi_dp_reg_select);
	jtag_adi_dpacc_rd(tap, ADI_DP_CTRL_STAT, 0, &adi_dp_reg_ctrl_stat);

	DCC_LOG1(LOG_TRACE, "DP SELECT=0x%08x", adi_dp_reg_select);
	DCC_LOG1(LOG_TRACE, "DP CTRL/STAT=0x%08x", adi_dp_reg_ctrl_stat);

	st = adi_dp_reg_ctrl_stat;
	DCC_LOG4(LOG_TRACE, "ORUNDETECT=%c STICKYORUN=%c STICKYCMP=%c "
			 "STICKYERR=%c", 
			 (st & DP_ORUNDETECT) ? '1' : '0',
			 (st & DP_STICKYORUN) ? '1' : '0', 
			 (st & DP_STICKYCMP) ? '1' : '0',
			 (st & DP_STICKYERR) ? '1' : '0');
	st = st;
	
	if (adi_dp_reg_ctrl_stat & DP_STICKYERR) {
		DCC_LOG(LOG_TRACE, "Clearing the sticky error flag");
		/* Clear sticky error flag */
		jtag_adi_dpacc_wr(tap, ADI_DP_CTRL_STAT, DP_STICKYERR, NULL);
	}

	adi_dp_ctrl_stat_ready = 0;

	return 0;
}



/*************************************************************************/
/* Memory Acess Port (MEM-AP) operations                                 */
/*************************************************************************/

const char mem_ap_reg[][4] = {
	"CSW",
	"TAR",
	"x08",
	"DRW",
	"BD0",
	"BD1",
	"BD2",
	"BD3",
	"x20",
	"x24",
	"x28",
	"x2c",
	"xf0",
	"CFG",
	"BAS",
	"IDR"
};

uint32_t adi_mem_ap_csw = 0;
uint32_t adi_mem_ap_tar = 0;

/* Memory Access Port Basic (non pipelined) Register Read */
int jtag_mem_ap_reg_rd(jtag_tap_t * tap, uint32_t addr, uint32_t * dout)
{
	uint32_t diff;
	int ack;

	/* if the bank in the address and select registers are
	   different update the select register */
	if ((diff = DP_APBANKSEL(addr ^ adi_dp_reg_select))) {
		adi_dp_reg_select ^= diff;
		DCC_LOG1(LOG_TRACE, "DP SELECT=0x%08x", adi_dp_reg_select);
		ack = jtag_adi_dpacc_wr(tap, ADI_DP_SELECT, adi_dp_reg_select, NULL);
	}

	ack = jtag_adi_apacc_rd(tap, addr, 0, NULL);

	ack = jtag_adi_dpacc_rd(tap, ADI_DP_CTRL_STAT, 0, dout);
	adi_dp_ctrl_stat_ready = 1;

	DCC_LOG2(LOG_TRACE, "reg=%s --> 0x%08x", 
			 mem_ap_reg[(addr >> 2) & 0x0f], *dout);

	return ack;
}

/* Memory Access Port Basic Register Write */
int jtag_mem_ap_reg_wr(jtag_tap_t * tap, uint32_t addr, uint32_t din)
{
	uint32_t diff;
	int ack;

	DCC_LOG2(LOG_TRACE, "reg=%s <-- 0x%08x", 
			 mem_ap_reg[(addr >> 2) & 0x0f], din);

	/* if the bank in the address and select registers are
	   different update the select register */
	if ((diff = DP_APBANKSEL(addr ^ adi_dp_reg_select))) {
		adi_dp_reg_select ^= diff;
		DCC_LOG1(LOG_TRACE, "DP SELECT=0x%08x", adi_dp_reg_select);
		ack = jtag_adi_dpacc_wr(tap, ADI_DP_SELECT, diff, NULL);
	}

	ack = jtag_adi_apacc_wr(tap, addr, din, NULL);
	if (ack == JTAG_ADI_ACK_WAIT) {
		DCC_LOG(LOG_PANIC, "JTAG_ADI_ACK_WAIT");
	}

	adi_dp_ctrl_stat_ready = 0;

	return ack;
}

int jtag_mem_ap_csw_set(jtag_tap_t * tap, uint32_t mask, uint32_t bitmap)
{
	int ret;

	jtag_mem_ap_reg_wr(tap, ADI_AP_CSW, (adi_mem_ap_csw & ~mask) | bitmap);
	ret = jtag_mem_ap_reg_rd(tap, ADI_AP_CSW, &adi_mem_ap_csw);

	DCC_LOG1(LOG_TRACE, "AP CSW=0x%08x", adi_mem_ap_csw);
	return ret;
}

int jtag_mem_ap_csw_clr(jtag_tap_t * tap, uint32_t mask, uint32_t bitmap)
{
	int ret;

	jtag_mem_ap_reg_wr(tap, ADI_AP_CSW, adi_mem_ap_csw & ~(bitmap & mask));
	ret = jtag_mem_ap_reg_rd(tap, ADI_AP_CSW, &adi_mem_ap_csw);

	DCC_LOG1(LOG_TRACE, "AP CSW=0x%08x", adi_mem_ap_csw);
	return ret;
}

int jtag_mem_ap_rd8(jtag_tap_t * tap, uint32_t addr, uint8_t * dout)
{
	uint32_t diff;
	uint32_t data;
	int ack;

	DCC_LOG1(LOG_INFO, "addr=0x%08x", addr);

	/* check whether the bank of the TAR and select registers are
	   different */
	diff = DP_APBANKSEL(ADI_AP_TAR ^ adi_dp_reg_select);

	if ((adi_mem_ap_tar != addr) || (diff)) {
		if (diff) {
			/* update the DP SELECT register */
			adi_dp_reg_select ^= diff;
			DCC_LOG1(LOG_INFO, "DP SELECT=0x%08x", adi_dp_reg_select);
			ack = jtag_adi_dpacc_wr(tap, ADI_DP_SELECT, 
									adi_dp_reg_select, NULL);
		}
		/* update the AP TAR register */
		DCC_LOG1(LOG_INFO, "AP TAR=0x%08x", addr);
		ack = jtag_adi_apacc_wr(tap, ADI_AP_TAR, addr, NULL);
		if (ack == JTAG_ADI_ACK_WAIT) {
			DCC_LOG(LOG_PANIC, "jtag_adi_apacc_wr(TAR) return ACK_WAIT!");
		}
		adi_mem_ap_tar = addr;
	}

	if ((adi_mem_ap_csw & CSW_SIZE) != CSW_BYTE) {
		adi_mem_ap_csw = (adi_mem_ap_csw & ~CSW_SIZE) | CSW_BYTE;
		jtag_mem_ap_reg_wr(tap, ADI_AP_CSW, adi_mem_ap_csw);
	}

	jtag_adi_apacc_rd(tap, ADI_AP_DRW, 0, NULL);

	ack = jtag_adi_dpacc_rd(tap, ADI_DP_CTRL_STAT, 0, &data);
	adi_dp_ctrl_stat_ready = 1;

	if (dout != NULL) {
		int sihft = 8 * (addr & 0x3);
		*dout = data >> sihft;
	}

	if (adi_mem_ap_csw & CSW_INC_SINGLE)
		adi_mem_ap_tar += 1;

	return ack;
}

int jtag_mem_ap_wr8(jtag_tap_t * tap, uint32_t addr, uint8_t din)
{
	uint32_t diff;
	uint32_t data;
	int shift = 8 * (addr & 0x3);
	int ack;

	DCC_LOG1(LOG_INFO, "addr=0x%08x", addr);

	data = din << shift;

	/* check whether the bank of the TAR and select registers are
	   different */
	diff = DP_APBANKSEL(ADI_AP_TAR ^ adi_dp_reg_select);

	if ((adi_mem_ap_tar != addr) || (diff)) {
		if (diff) {
			/* update the DP SELECT register */
			adi_dp_reg_select ^= diff;
			DCC_LOG1(LOG_INFO, "DP SELECT=0x%08x", adi_dp_reg_select);
			ack = jtag_adi_dpacc_wr(tap, ADI_DP_SELECT, 
									adi_dp_reg_select, NULL);
		}
		/* update the AP TAR register */
		DCC_LOG1(LOG_INFO, "AP TAR=0x%08x", addr);
		ack = jtag_adi_apacc_wr(tap, ADI_AP_TAR, addr, NULL);
		if (ack == JTAG_ADI_ACK_WAIT) {
			DCC_LOG(LOG_PANIC, "jtag_adi_apacc_wr(TAR) return ACK_WAIT!");
		}
		adi_mem_ap_tar = addr;
	}

	if ((adi_mem_ap_csw & CSW_SIZE) != CSW_BYTE) {
		adi_mem_ap_csw = (adi_mem_ap_csw & ~CSW_SIZE) | CSW_BYTE;
		jtag_mem_ap_reg_wr(tap, ADI_AP_CSW, adi_mem_ap_csw);
	}

	ack = jtag_adi_apacc_wr(tap, ADI_AP_DRW, data, NULL);
	adi_dp_ctrl_stat_ready = 0;

	if (adi_mem_ap_csw & CSW_INC_SINGLE)
		adi_mem_ap_tar += 1;

	return ack;
}

int jtag_mem_ap_rd16(jtag_tap_t * tap, uint32_t addr, uint16_t * dout)
{
	uint32_t diff;
	uint32_t data;
	int ack;

	DCC_LOG1(LOG_INFO, "addr=0x%08x", addr);

	/* check whether the bank of the TAR and select registers are
	   different */
	diff = DP_APBANKSEL(ADI_AP_TAR ^ adi_dp_reg_select);

	if ((adi_mem_ap_tar != addr) || (diff)) {
		if (diff) {
			/* update the DP SELECT register */
			adi_dp_reg_select ^= diff;
			DCC_LOG1(LOG_INFO, "DP SELECT=0x%08x", adi_dp_reg_select);
			ack = jtag_adi_dpacc_wr(tap, ADI_DP_SELECT, 
									adi_dp_reg_select, NULL);
		}
		/* update the AP TAR register */
		DCC_LOG1(LOG_INFO, "AP TAR=0x%08x", addr);
		ack = jtag_adi_apacc_wr(tap, ADI_AP_TAR, addr, NULL);
		if (ack == JTAG_ADI_ACK_WAIT) {
			DCC_LOG(LOG_PANIC, "jtag_adi_apacc_wr(TAR) return ACK_WAIT!");
		}
		adi_mem_ap_tar = addr;
	}

	if ((adi_mem_ap_csw & CSW_SIZE) != CSW_HALFWORD) {
		adi_mem_ap_csw = (adi_mem_ap_csw & ~CSW_SIZE) | CSW_HALFWORD;
		jtag_mem_ap_reg_wr(tap, ADI_AP_CSW, adi_mem_ap_csw);
	}

	jtag_adi_apacc_rd(tap, ADI_AP_DRW, 0, NULL);

	ack = jtag_adi_dpacc_rd(tap, ADI_DP_CTRL_STAT, 0, &data);
	adi_dp_ctrl_stat_ready = 1;

	if (dout != NULL) {
		*dout = (addr & 0x2) ? data >> 16 : data;
	}

	if (adi_mem_ap_csw & CSW_INC_SINGLE)
		adi_mem_ap_tar += 2;

	return ack;
}

int jtag_mem_ap_wr16(jtag_tap_t * tap, uint32_t addr, uint16_t din)
{
	uint32_t diff;
	uint32_t data;
	int ack;

	DCC_LOG1(LOG_INFO, "addr=0x%08x", addr);

	data = (addr & 0x2) ? din << 16 : din;

	/* check whether the bank of the TAR and select registers are
	   different */
	diff = DP_APBANKSEL(ADI_AP_TAR ^ adi_dp_reg_select);

	if ((adi_mem_ap_tar != addr) || (diff)) {
		if (diff) {
			/* update the DP SELECT register */
			adi_dp_reg_select ^= diff;
			DCC_LOG1(LOG_INFO, "DP SELECT=0x%08x", adi_dp_reg_select);
			ack = jtag_adi_dpacc_wr(tap, ADI_DP_SELECT, 
									adi_dp_reg_select, NULL);
		}
		/* update the AP TAR register */
		DCC_LOG1(LOG_INFO, "AP TAR=0x%08x", addr);
		ack = jtag_adi_apacc_wr(tap, ADI_AP_TAR, addr, NULL);
		if (ack == JTAG_ADI_ACK_WAIT) {
			DCC_LOG(LOG_PANIC, "jtag_adi_apacc_wr(TAR) return ACK_WAIT!");
		}
		adi_mem_ap_tar = addr;
	}

	if ((adi_mem_ap_csw & CSW_SIZE) != CSW_HALFWORD) {
		adi_mem_ap_csw = (adi_mem_ap_csw & ~CSW_SIZE) | CSW_HALFWORD;
		jtag_mem_ap_reg_wr(tap, ADI_AP_CSW, adi_mem_ap_csw);
	}

	ack = jtag_adi_apacc_wr(tap, ADI_AP_DRW, data, NULL);
	adi_dp_ctrl_stat_ready = 0;

	if (adi_mem_ap_csw & CSW_INC_SINGLE)
		adi_mem_ap_tar += 2;

	return ack;
}


int jtag_mem_ap_rd32(jtag_tap_t * tap, uint32_t addr, uint32_t * dout)
{
	uint32_t diff;
	uint32_t offs;
	uint32_t base;
	int ack;

	DCC_LOG1(LOG_MSG, "addr=0x%08x", addr);

	base = addr & 0xfffffff0;

	if (adi_mem_ap_tar != base) {
		if ((diff = DP_APBANKSEL(ADI_AP_TAR ^ adi_dp_reg_select))) {
			/* update the DP SELECT register */
			adi_dp_reg_select ^= diff;
			DCC_LOG1(LOG_INFO, "DP SELECT=0x%08x", adi_dp_reg_select);
			ack = jtag_adi_dpacc_wr(tap, ADI_DP_SELECT, 
									adi_dp_reg_select, NULL);
		}
		/* update the AP TAR register */
		DCC_LOG1(LOG_INFO, "AP TAR=0x%08x", base);
		ack = jtag_adi_apacc_wr(tap, ADI_AP_TAR, base, NULL);
		if (ack == JTAG_ADI_ACK_WAIT) {
			DCC_LOG(LOG_PANIC, "jtag_adi_apacc_wr(TAR) return ACK_WAIT!");
		}
		adi_mem_ap_tar = base;
	}

	/* select 32bits transfer size */
	if ((adi_mem_ap_csw & CSW_SIZE) != CSW_WORD) {
		if ((diff = DP_APBANKSEL(ADI_AP_CSW ^ adi_dp_reg_select))) {
			/* update the DP SELECT register */
			adi_dp_reg_select ^= diff;
			DCC_LOG1(LOG_INFO, "DP SELECT=0x%08x", adi_dp_reg_select);
			ack = jtag_adi_dpacc_wr(tap, ADI_DP_SELECT, 
									adi_dp_reg_select, NULL);
		}
		DCC_LOG(LOG_TRACE, "selecting 32bits transfer size");
		adi_mem_ap_csw = (adi_mem_ap_csw & ~CSW_SIZE) | CSW_WORD;
		jtag_mem_ap_reg_wr(tap, ADI_AP_CSW, adi_mem_ap_csw);
	}

	/* if the bank in the address and select registers are
	   different update the select register */
	if ((diff = DP_APBANKSEL(ADI_AP_BD0 ^ adi_dp_reg_select))) {
		/* update the DP SELECT register */
		adi_dp_reg_select ^= diff;
		DCC_LOG1(LOG_INFO, "DP SELECT=0x%08x", adi_dp_reg_select);
		ack = jtag_adi_dpacc_wr(tap, ADI_DP_SELECT, 
								adi_dp_reg_select, NULL);
	}

	/* use banked registers */
	offs = addr - base;
	DCC_LOG2(LOG_MSG, "addr=0x%08x 0ffs=0x%x", addr, offs);
	jtag_adi_apacc_rd(tap, ADI_AP_BD0 + offs , 0, NULL);

	ack = jtag_adi_dpacc_rd(tap, ADI_DP_CTRL_STAT, 0, dout);

#if 0
	if (ack == JTAG_ADI_ACK_WAIT) {
		adi_dp_ctrl_stat_ready = 0;
	}
#endif

	adi_dp_ctrl_stat_ready = 1;

	return ack;
}

int jtag_mem_ap_wr32(jtag_tap_t * tap, uint32_t addr, uint32_t data)
{
	uint32_t diff;
	uint32_t offs;
	uint32_t base;
	int ack;

	DCC_LOG1(LOG_MSG, "addr=0x%08x", addr);

	base = addr & 0xfffffff0;

	if (adi_mem_ap_tar != base) {
		/* check whether the bank of the TAR and select registers are
		   different */
		if ((diff = DP_APBANKSEL(ADI_AP_TAR ^ adi_dp_reg_select))) {
			/* update the DP SELECT register */
			adi_dp_reg_select ^= diff;
			DCC_LOG1(LOG_INFO, "DP SELECT=0x%08x", adi_dp_reg_select);
			ack = jtag_adi_dpacc_wr(tap, ADI_DP_SELECT, 
									adi_dp_reg_select, NULL);
		}
		/* update the AP TAR register */
		DCC_LOG1(LOG_INFO, "AP TAR=0x%08x", base);
		ack = jtag_adi_apacc_wr(tap, ADI_AP_TAR, base, NULL);
		if (ack == JTAG_ADI_ACK_WAIT) {
			DCC_LOG(LOG_PANIC, "jtag_adi_apacc_wr(TAR) return ACK_WAIT!");
		}
		adi_mem_ap_tar = base;
	}

	/* select 32bits transfer size */
	if ((adi_mem_ap_csw & CSW_SIZE) != CSW_WORD) {
		if ((diff = DP_APBANKSEL(ADI_AP_CSW ^ adi_dp_reg_select))) {
			/* update the DP SELECT register */
			adi_dp_reg_select ^= diff;
			DCC_LOG1(LOG_INFO, "DP SELECT=0x%08x", adi_dp_reg_select);
			ack = jtag_adi_dpacc_wr(tap, ADI_DP_SELECT, 
									adi_dp_reg_select, NULL);
		}
		DCC_LOG(LOG_TRACE, "selecting 32bits transfer size");
		adi_mem_ap_csw = (adi_mem_ap_csw & ~CSW_SIZE) | CSW_WORD;
		jtag_mem_ap_reg_wr(tap, ADI_AP_CSW, adi_mem_ap_csw);
	}

	/* if the bank in the address and select registers are
	   different update the select register */
	if ((diff = DP_APBANKSEL(ADI_AP_BD0 ^ adi_dp_reg_select))) {
		/* update the DP SELECT register */
		adi_dp_reg_select ^= diff;
		DCC_LOG1(LOG_INFO, "DP SELECT=0x%08x", adi_dp_reg_select);
		ack = jtag_adi_dpacc_wr(tap, ADI_DP_SELECT, 
								adi_dp_reg_select, NULL);
	}

	/* use banked registers */
	offs = addr - base;
	DCC_LOG2(LOG_INFO, "addr=0x%08x 0ffs=0x%x", addr, offs);
	ack = jtag_adi_apacc_wr(tap, ADI_AP_BD0 + offs , data, NULL);

	adi_dp_ctrl_stat_ready = 0;

	return ack;
}

int jtag_mem_ap_read(jtag_tap_t * tap, uint32_t addr, void * buf, int len)
{
	uint8_t * ptr = (uint8_t *)buf;
	int cnt = 0;

	DCC_LOG2(LOG_INFO, "addr=0x%08x len=%d", addr, len);

	if (len == 0)
		return 0;

	/* get the first byte, if the address is not half-word (16bits) aligned */
	if (addr & 0x01) {
		jtag_mem_ap_rd8(tap, addr, ptr);
		ptr++;
		len--;
		addr++;
		cnt++;
	}

	/* get the next word, if the address is not word (32bits) aligned */
	if ((len >= 2) && (addr & 0x02)) {
		uint16_t data;
		jtag_mem_ap_rd16(tap, addr, &data);
		ptr[0] = data;
		ptr[1] = data >> 8;
		ptr += 2;
		len -= 2;
		addr += 2;
		cnt += 2;
	}

	if (len >= 4) {
		uint32_t diff;
		uint32_t data;
		int i;
		int n;
		/* check whether the bank of the TAR and select registers are
		   different */
		diff = DP_APBANKSEL(ADI_AP_TAR ^ adi_dp_reg_select);

		if ((adi_mem_ap_tar != addr) || (diff)) {
			if (diff) {
				/* update the DP SELECT register */
				adi_dp_reg_select ^= diff;
				DCC_LOG1(LOG_INFO, "DP SELECT=0x%08x", adi_dp_reg_select);
				jtag_adi_dpacc_wr(tap, ADI_DP_SELECT, adi_dp_reg_select, NULL);
			}
			/* update the AP TAR register */
			DCC_LOG1(LOG_INFO, "AP TAR=0x%08x", addr);
			jtag_adi_apacc_wr(tap, ADI_AP_TAR, addr, NULL);
			adi_mem_ap_tar = addr;
		}

		if ((adi_mem_ap_csw & CSW_SIZE) != CSW_WORD) {
			adi_mem_ap_csw = (adi_mem_ap_csw & ~CSW_SIZE) | CSW_WORD;
			jtag_mem_ap_reg_wr(tap, ADI_AP_CSW, adi_mem_ap_csw);
		}

		if (!(adi_mem_ap_csw & CSW_INC_SINGLE)) {
			adi_mem_ap_csw = (adi_mem_ap_csw & ~CSW_ADDR_INC) | CSW_INC_SINGLE;
			jtag_mem_ap_reg_wr(tap, ADI_AP_CSW, adi_mem_ap_csw); 
			DCC_LOG1(LOG_TRACE, "AP CSW=0x%08x", adi_mem_ap_csw);
		}	
		n = len / 4;
		ptr = (uint8_t *)buf;

		jtag_adi_apacc_rd(tap, ADI_AP_DRW, 0, NULL);
		for (i = 1; i < n; i++) {
			jtag_adi_apacc_rd(tap, ADI_AP_DRW, 0, &data);
			ptr[0] = data;
			ptr[1] = data >> 8;
			ptr[2] = data >> 16;
			ptr[3] = data >> 24;
			ptr += 4;
		}
		jtag_adi_dpacc_rd(tap, ADI_DP_CTRL_STAT, 0, &data);
		ptr[0] = data;
		ptr[1] = data >> 8;
		ptr[2] = data >> 16;
		ptr[3] = data >> 24;
		ptr += 4;
	
		adi_dp_ctrl_stat_ready = 1;

		adi_mem_ap_tar += n * 4;
		len -= n *4;
		addr += n *4;
		cnt += n *4;
	}

	/* get the last word */
	if (len >= 2) {
		uint16_t data;
		jtag_mem_ap_rd16(tap, addr, &data);
		ptr[0] = data;
		ptr[1] = data >> 8;
		ptr += 2;
		len -= 2;
		addr += 2;
		cnt += 2;
	}

	/* get the last byte */
	if (len > 0) {
		jtag_mem_ap_rd8(tap, addr, ptr);
		cnt++;
	}

	return cnt;
}

int jtag_mem_ap_write(jtag_tap_t * tap, uint32_t addr, 
					  const void * buf, int len)
{
	uint8_t * ptr = (uint8_t *)buf;
	int cnt = 0;

	DCC_LOG2(LOG_TRACE, "addr=0x%08x len=%d", addr, len);

	if (len == 0)
		return 0;

	/* write the first byte, if the address is not half-word (16bits) aligned */
	if (addr & 0x01) {
		jtag_mem_ap_wr8(tap, addr, *ptr);
		ptr++;
		len--;
		addr++;
		cnt++;
	}

	/* get the next word, if the address is not word (32bits) aligned */
	if ((len >= 2) && (addr & 0x02)) {
		uint16_t data;
		data = ptr[0] | (ptr[1] << 8);
		jtag_mem_ap_wr16(tap, addr, data);
		ptr += 2;
		len -= 2;
		addr += 2;
		cnt += 2;
	}

	if (len >= 4) {
		uint32_t diff;
		uint32_t data;
		int i;
		int n;
		/* check whether the bank of the TAR and select registers are
		   different */
		diff = DP_APBANKSEL(ADI_AP_TAR ^ adi_dp_reg_select);

		if ((adi_mem_ap_tar != addr) || (diff)) {
			if (diff) {
				/* update the DP SELECT register */
				adi_dp_reg_select ^= diff;
				DCC_LOG1(LOG_INFO, "DP SELECT=0x%08x", adi_dp_reg_select);
				jtag_adi_dpacc_wr(tap, ADI_DP_SELECT, adi_dp_reg_select, NULL);
			}
			/* update the AP TAR register */
			DCC_LOG1(LOG_INFO, "AP TAR=0x%08x", addr);
			jtag_adi_apacc_wr(tap, ADI_AP_TAR, addr, NULL);
			adi_mem_ap_tar = addr;
		}

		if ((adi_mem_ap_csw & CSW_SIZE) != CSW_WORD) {
			adi_mem_ap_csw = (adi_mem_ap_csw & ~CSW_SIZE) | CSW_WORD;
			jtag_mem_ap_reg_wr(tap, ADI_AP_CSW, adi_mem_ap_csw);
		}

		if (!(adi_mem_ap_csw & CSW_INC_SINGLE)) {
			adi_mem_ap_csw = (adi_mem_ap_csw & ~CSW_ADDR_INC) | CSW_INC_SINGLE;
			jtag_mem_ap_reg_wr(tap, ADI_AP_CSW, adi_mem_ap_csw); 
			DCC_LOG1(LOG_TRACE, "AP CSW=0x%08x", adi_mem_ap_csw);
		}	
		n = (len + 3) / 4;

		for (i = 0; i < n; i++) {
			data = ptr[0] | (ptr[1] << 8) | (ptr[2] << 16) | (ptr[3] << 24);
			jtag_adi_apacc_wr(tap, ADI_AP_DRW, data, NULL);
			ptr += 4;
		}

		adi_dp_ctrl_stat_ready = 0;

		adi_mem_ap_tar += n * 4;
		len -= n *4;
		addr += n *4;
		cnt += n *4;
	}

	/* write the last word, if there is at leas 2 bytes remaining */
	if (len >= 2) {
		uint16_t data;
		data = ptr[0] | (ptr[1] << 8);
		jtag_mem_ap_wr16(tap, addr, data);
		ptr += 2;
		len -= 2;
		addr += 2;
		cnt += 2;
	}

	/* write the last byte, if any */
	if (len > 0) {
		jtag_mem_ap_wr8(tap, addr, *ptr);
		cnt++;
	}

	return cnt;
}

int jtag_mem_ap_set(jtag_tap_t * tap, uint32_t addr, uint8_t val, int len)
{
	uint32_t data;
	int cnt = 0;

	DCC_LOG1(LOG_INFO, "addr=0x%08x", addr);

	if (len == 0)
		return 0;

	data = val | (val << 8) | (val << 16) | (val << 24);

	/* write the first byte, if the address is not half-word (16bits) aligned */
	if (addr & 0x01) {
		jtag_mem_ap_wr8(tap, addr, val);
		len--;
		addr++;
		cnt++;
	}

	/* get the next word, if the address is not word (32bits) aligned */
	if ((len >= 2) && (addr & 0x02)) {
		jtag_mem_ap_wr16(tap, addr, data);
		len -= 2;
		addr += 2;
		cnt += 2;
	}

	if (len >= 4) {
		uint32_t diff;
		int n;
		int i;
		/* check whether the bank of the TAR and select registers are
		   different */
		diff = DP_APBANKSEL(ADI_AP_TAR ^ adi_dp_reg_select);

		if ((adi_mem_ap_tar != addr) || (diff)) {
			if (diff) {
				/* update the DP SELECT register */
				adi_dp_reg_select ^= diff;
				DCC_LOG1(LOG_INFO, "DP SELECT=0x%08x", adi_dp_reg_select);
				jtag_adi_dpacc_wr(tap, ADI_DP_SELECT, adi_dp_reg_select, NULL);
			}
			/* update the AP TAR register */
			DCC_LOG1(LOG_INFO, "AP TAR=0x%08x", addr);
			jtag_adi_apacc_wr(tap, ADI_AP_TAR, addr, NULL);
			adi_mem_ap_tar = addr;
		}

		if ((adi_mem_ap_csw & CSW_SIZE) != CSW_WORD) {
			adi_mem_ap_csw = (adi_mem_ap_csw & ~CSW_SIZE) | CSW_WORD;
			jtag_mem_ap_reg_wr(tap, ADI_AP_CSW, adi_mem_ap_csw);
		}

		if (!(adi_mem_ap_csw & CSW_INC_SINGLE)) {
			adi_mem_ap_csw = (adi_mem_ap_csw & ~CSW_ADDR_INC) | CSW_INC_SINGLE;
			jtag_mem_ap_reg_wr(tap, ADI_AP_CSW, adi_mem_ap_csw); 
			DCC_LOG1(LOG_TRACE, "AP CSW=0x%08x", adi_mem_ap_csw);
		}	
		n = (len + 3) / 4;

		for (i = 0; i < n; i++) {
			jtag_adi_apacc_wr(tap, ADI_AP_DRW, data, NULL);
		}

		adi_dp_ctrl_stat_ready = 0;

		adi_mem_ap_tar += n * 4;
		len -= n *4;
		addr += n *4;
		cnt += n *4;
	}

	/* write the last word, if there is at leas 2 bytes remaining */
	if (len >= 2) {
		jtag_mem_ap_wr16(tap, addr, data);
		len -= 2;
		addr += 2;
		cnt += 2;
	}

	/* write the last byte, if any */
	if (len > 0) {
		jtag_mem_ap_wr8(tap, addr, data);
		cnt++;
	}

	return cnt;
}

int jtag_mem_ap_init(jtag_tap_t * tap)
{
//	uint32_t st;

	adi_mem_ap_tar = 0xffffffff;

	jtag_mem_ap_reg_rd(tap, ADI_AP_CSW, &adi_mem_ap_csw);
	DCC_LOG1(LOG_TRACE, "AP CSW=0x%08x", adi_mem_ap_csw);

	DCC_LOG3(LOG_TRACE, "CSW: SPIStatus=%c TrInProg=%c DbgStatus=%c", 
			 (adi_mem_ap_csw & CSW_SPISTATUS) ? '1' : '0',
			 (adi_mem_ap_csw & CSW_TRINPROG) ? '1' : '0', 
			 (adi_mem_ap_csw & CSW_DBG_STATUS) ? '1' : '0');

	if (!(adi_mem_ap_csw & CSW_DEVICE_EN)) {
		DCC_LOG(LOG_WARNING, "CSW DeviceEn not set!!!");
		return -1;
	}

	/* single autoincrement for TAR 
	   word (32 bits) access size */ 
	/* XXX: CM3 HPROT1 must be one */
	jtag_mem_ap_reg_wr(tap, ADI_AP_CSW, CSW_HPROT1 |  CSW_MASTER_TYPE |
						   CSW_INC_SINGLE | CSW_WORD);

	jtag_mem_ap_reg_rd(tap, ADI_AP_CSW, &adi_mem_ap_csw);
	DCC_LOG1(LOG_TRACE, "AP CSW=0x%08x", adi_mem_ap_csw);


#if 0
	jtag_adi_dpacc_rd(tap, ADI_DP_CTRL_STAT, 0, &adi_dp_reg_ctrl_stat);
	DCC_LOG1(LOG_TRACE, "DP CTRL/STAT=0x%08x", adi_dp_reg_ctrl_stat);

	st = adi_dp_reg_ctrl_stat;
	DCC_LOG4(LOG_TRACE, "ORUNDETECT=%c STICKYORUN=%c STICKYCMP=%c "
			 "STICKYERR=%c", 
			 (st & DP_ORUNDETECT) ? '1' : '0',
			 (st & DP_STICKYORUN) ? '1' : '0', 
			 (st & DP_STICKYCMP) ? '1' : '0',
			 (st & DP_STICKYERR) ? '1' : '0');

	if (st & DP_STICKYERR) {
		DCC_LOG(LOG_TRACE, "Clearing the sticky error flag");
		/* Clear sticky error flag */
		jtag_adi_dpacc_wr(tap, ADI_DP_CTRL_STAT, DP_STICKYERR, NULL);
	}

#endif

	if ((adi_mem_ap_csw & (CSW_HPROT1 | CSW_ADDR_INC | CSW_SIZE)) !=
		(CSW_HPROT1 | CSW_INC_SINGLE | CSW_WORD)) {
		DCC_LOG(LOG_WARNING, "CSW initialization fail...");
		return -1;
	}

	return 0;
}

/*************************************************************************/
/* MEM-AP ancillary functions                                            */
/*************************************************************************/

int jtag_mem_ap_component_id_rd(jtag_tap_t * tap, uint32_t addr, uint32_t * id)
{
	uint32_t buf[8];
	int ret;

	if ((ret = jtag_mem_ap_read(tap, addr + 0xff0, buf, 16)) < 0)
		return ret;

	if (id != NULL)
		*id = buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);

	return ret;
}

int jtag_adi_test(void)
{

	return JTAG_OK;
}

