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
 * @file .h
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */



#ifndef __JTAG3DRV_H__
#define __JTAG3DRV_H__

#define VEC_ADDR_BITS 10
#define VEC_MEM_SIZE (1 << VEC_ADDR_BITS)
#define VEC_LEN_BITS 10
#define VEC_LEN_MAX  ((1 << VEC_LEN_BITS) - 1)

#define MEM_VECTOR_OFFS (0 << 11)
#define MEM_DESCRIPTOR_OFFS (0 << 11)
#define MEM_PTR_OFFS (1 << 10)
#define MEM_REGISTER_OFFS (1 << 11)

#define REG_INSN 0
#define REG_STATUS 0
#define REG_INT_ST 1
#define REG_INT_EN 2

#define REG_CKGEN_DIV 4
#define REG_CKGEN_RTDIV 5
#define REG_CFG 6
#define REG_TMR 7

#define CFG_CLK_SEL(X) (((X) & 0xf) << 0)
#define GET_CLK_SEL(CFG) (((CFG) >> 0) & 0xf)
#define CFG_LOOP (1 << 4)
#define CFG_RTCK_EN (1 << 5)
#define CFG_RST (1 << 6)
#define CFG_TDO_PROBE_EN (1 << 7)
#define CFG_TAP_VCC_EN (1 << 8)
#define CFG_RELAY_EN (1 << 9)
#define CFG_TAP_TRST (1 << 10)
#define CFG_TAP_NRST (1 << 11)

#define IRQ_TAP (1 << 0)
#define IRQ_TMO (1 << 1)
#define IRQ_1KHZ (1 << 2)


/* JTAG controller instructions */
#define INSN_IR_SCAN(D, E) (0xf000 | (((E) & 0xf) << 8) | ((D) & 0xff))
#define INSN_DR_SCAN(D, E) (0xe000 | (((E) & 0xf) << 8) | ((D) & 0xff))
/* FIXME: 
#define INSN_TAP_RESET(N, E) (0xd000 | (((E) & 0xf) << 8) | (((N) - 1) & 0xff))
*/
#define INSN_TAP_RESET(N, E) (0xd000 | (((E) & 0xf) << 8) | ((N) & 0xff))

#define INSN_RUN_TEST(N, E) (0xc000 | (((E) & 0xf) << 8) | ((N) & 0xff))
#define INSN_IR_PAUSE(N, E) (0xb000 | (((E) & 0xf) << 8) | ((N) & 0xff))
#define INSN_DR_PAUSE(N, E) (0xa000 | (((E) & 0xf) << 8) | ((N) & 0xff))
#define INSN_SET_STATE(E) (0x9000 | ((E) & 0xf))


/* Descriptor Pointer fields */
#define PTR_DESC_LEN (0x00ff << 0)
#define PTR_DESC_ADDR (0x00ff << 8)

/* Vector descriptor fields */
#define DESC_VEC_LEN (0x03ff << 0)
#define DESC_TX_VEC_ADDR (0x03ff << 10)
#define DESC_RX_VEC_ADDR (0x03ff << 20)
#define DESC_TX_VEC_MSBF (1 << 30)
#define DESC_RX_VEC_MSBF (1 << 31)

#define RX_DIR (1 << 31)
#define TX_DIR (1 << 30)
#define RX_LSB_FIRST 0
#define TX_LSB_FIRST 0
#define RX_MSB_FIRST RX_DIR
#define TX_MSB_FIRST TX_DIR


/* JTAG vector descriptor:
	RA: RX address
	RM: RX MSB first
	TA: TX address
	TM: TX MSB first
	N: vector length
*/
#define JTAG_DESC(RA, RM, TA, TM, N) (\
	(((RA) & 0x07fe) << 19) | (((RM) & 1) << 31) | \
	(((TA) & 0x07fe) << 9) | (((TM) & 1) << 30) | (((N) - 1) & 0x03ff))

#define JTAG_PTR(DESC, LEN) ((((DESC) & 0x0ff) << 8) | (((LEN) - 1) & 0x0ff))

#define DESC_TX_ADDR(DESC) ((DESC >> 9) & 0x7fe)
#define DESC_TX_MSBF(DESC) ((DESC >> 30) & 1)
#define DESC_RX_ADDR(DESC) ((DESC >> 19) & 0x7fe)
#define DESC_RX_MSBF(DESC) ((DESC >> 31) & 1)
#define DESC_LEN(DESC) ((DESC & 0x3ff) + 1)

#define EBI_CS2_BASE 0x02400000
#define EBI_CS3_BASE 0x02800000

#include <stdint.h>
#include <sys/at91.h>
#include <uthreads.h>
#include "jtag.h"

extern uint16_t volatile * jtagdrv_vec;
extern uint16_t volatile * jtagdrv_reg;
extern uint32_t volatile * jtagdrv_desc;
extern uint16_t volatile * jtagdrv_ptr;

#if 1
#define JTAGDRV_VEC ((volatile uint16_t *)(EBI_CS2_BASE + MEM_VECTOR_OFFS))
#define JTAGDRV_DESC ((volatile uint32_t *)(EBI_CS3_BASE + MEM_DESCRIPTOR_OFFS))
#define JTAGDRV_REG ((volatile uint16_t *)(EBI_CS3_BASE + MEM_REGISTER_OFFS))
#define JTAGDRV_PTR ((volatile uint16_t *)(EBI_CS3_BASE + MEM_PTR_OFFS))
#else

#define JTAGDRV_VEC jtagdrv_vec 
#define JTAGDRV_DESC jtagdrv_desc 
#define JTAGDRV_REG jtagdrv_reg
#endif

extern inline uint16_t reg_rd(unsigned int n) {
	return JTAGDRV_REG[n];
}

extern inline void reg_wr(unsigned int n, uint16_t val) {
	JTAGDRV_REG[n] = val;
}

extern inline uint16_t vec_rd16(unsigned int n) {
	return JTAGDRV_VEC[n / 2];
}

extern inline void vec_wr16(unsigned int n, uint16_t val) {
	JTAGDRV_VEC[n / 2] = val;
}

extern inline uint32_t vec_rd32(unsigned int n) {
	n  /= 2;
	return JTAGDRV_VEC[n] + (JTAGDRV_VEC[n + 1] << 16);
}

extern inline void vec_wr32(unsigned int n, uint32_t val) {
	n  /= 2;
	JTAGDRV_VEC[n] = val;
	JTAGDRV_VEC[n + 1] = val >> 16;
}

extern inline uint32_t desc_rd(unsigned int pos) {
	return JTAGDRV_DESC[pos];
}

extern inline void desc_wr(unsigned int pos, uint32_t desc) {
	JTAGDRV_DESC[pos] = desc;
}

extern inline void ptr_wr(unsigned int id, uint16_t ptr) {
	JTAGDRV_PTR[id] = ptr;
}

extern inline void insn_irscan(unsigned int desc, unsigned int final_state) {
	unsigned int isr;
	reg_wr(REG_INSN, INSN_IR_SCAN(desc, final_state));
	uthread_int_wait(IRQ0);
	isr = reg_rd(REG_INT_ST);
}

extern inline void insn_drscan(unsigned int desc, unsigned int final_state) {
	unsigned int isr;
	reg_wr(REG_INSN, INSN_DR_SCAN(desc, final_state));
	uthread_int_wait(IRQ0);
	isr = reg_rd(REG_INT_ST);
}

extern inline void insn_run_test(unsigned int cnt, unsigned int final_state) {
	unsigned int isr;
	reg_wr(REG_INSN, INSN_RUN_TEST(cnt, final_state));
	uthread_int_wait(IRQ0);
	isr = reg_rd(REG_INT_ST);
}

extern inline void insn_tap_reset(unsigned int cnt, unsigned int final_state) {
	unsigned int isr;
	reg_wr(REG_INSN, INSN_TAP_RESET(cnt, final_state));
	uthread_int_wait(IRQ0);
	isr = reg_rd(REG_INT_ST);
}

extern inline void insn_ir_pause(unsigned int cnt, unsigned int final_state) {
	unsigned int isr;
	reg_wr(REG_INSN, INSN_IR_PAUSE(cnt, final_state));
	uthread_int_wait(IRQ0);
	isr = reg_rd(REG_INT_ST);
}

extern inline void insn_dr_pause(unsigned int cnt, unsigned int final_state) {
	unsigned int isr;
	reg_wr(REG_INSN, INSN_DR_PAUSE(cnt, final_state));
	uthread_int_wait(IRQ0);
	isr = reg_rd(REG_INT_ST);
}

struct jtag3drv {

	jtag_tap_t * volatile tap;

	uint32_t tck_freq;
	uint8_t arm_scan_n_cmp;
	uint8_t arm_scan_n_mask;
	int8_t arm_scan_chain;
};

extern struct jtag3drv jtag3drv;




#ifdef __cplusplus
extern "C" {
#endif

uint16_t reg_rd(unsigned int n);
void reg_wr(unsigned int n, uint16_t val);

uint16_t vec_rd16(unsigned int n);
void vec_wr16(unsigned int n, uint16_t val);

uint32_t vec_rd32(unsigned int n);
void vec_wr32(unsigned int n, uint32_t val);

uint32_t desc_rd(unsigned int pos);
void desc_wr(unsigned int pos, uint32_t desc);

void jtag3ctrl_vec_rd(unsigned int addr, uint16_t * buf, int len);

void jtag3ctrl_vec_wr(unsigned int addr, const uint16_t * buf, int len);

int jtag3ctrl_init(const void * rbf, int size);

unsigned int jtag3ctrl_tck_freq_set(unsigned int freq);

unsigned int jtag3ctrl_tck_freq_get(void);

int jtag3ctrl_irscan(unsigned int desc);

int jtag3ctrl_drscan(unsigned int desc, unsigned int final_state);

void  jtag3ctrl_tap_reset(void);

void  jtag3ctrl_run_test(int n);

void jtag3ctrl_tdo_probe(int enable);

void jtag3ctrl_tap_vcc(int enable);

void jtag3ctrl_relay(int enable);

void jtag3ctrl_trst(int set);
	
void jtag3ctrl_nrst(int set);

void jtag3ctrl_loopback(int enable);

void jtag3ctrl_int_en(unsigned int mask);

void jtag3ctrl_sys_rst(void);

void jtag3ctrl_rtck(int enable);

#ifdef __cplusplus
}
#endif

#endif /* __JTAG3DRV_H__ */
