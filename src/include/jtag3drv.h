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
 * @file jtag3drv.h
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */



#ifndef __JTAG3DRV_H__
#define __JTAG3DRV_H__

/* -- +---------+---------------+---------------+
	-- | Address | Read          | Write         |
	-- +---------+---------------+---------------+ 
	-- | 0x1ffff |               |               |
	-- |   ...   | Invalid       | Invalid       |
	-- |         |               |               |
	-- | 0x10010 |               |               |
	-- +----------+---------------+---------------+ 
	-- | 0x1000f |               |               |
	-- |  ...    | Registers     |  Control      |
	-- |         |               |  Registers    |
	-- | 0x10000 |               |               |
	-- +---------+---------------+---------------+ 
	-- | 0x0ffff |               |               |
	-- |   ...   | Invalid       | Invalid       |
	-- |         |               |               |
	-- | 0x08200 |               |               |
	-- +---------+---------------+---------------+
	-- |  0x81ff |               |               |
	-- |   ...   | Invalid       | Descriptor    |
	-- |         |               | Pointers      |
	-- |  0x8000 |               |               |
	-- +---------+---------------+---------------+ 
	-- |  0x5fff |               |               |
	-- |   ...   | Invalid       | Invalid       |
	-- |         |               |               |
	-- |  0x4100 |               |               |
	-- +---------+---------------+---------------+
	-- |  0x41ff |               |               |
	-- |   ...   | Invalid       | Vector        |
	-- |         |               | Descriptors   |
	-- |  0x4000 |               |               |
	-- +---------+---------------+---------------+
	-- |  0x4fff |               |               |
	-- |   ...   | Invalid       | Invalid       |
	-- |         |               |               |
	-- |  0x1000 |               |               |
	-- +---------+---------------+---------------+
	-- |  0x03ff |               |               |
	-- |   ...   | Vector TX     | Vector RX     |
	-- |         | Memory        | Memory        |
	-- |  0x0000 |               |               |
	-- +---------+---------------+---------------+
*/

#define VEC_ADDR_BITS 10
#define VEC_MEM_SIZE (1 << VEC_ADDR_BITS)
#define VEC_LEN_BITS 10
#define VEC_LEN_MAX  ((1 << VEC_LEN_BITS) - 1)

#define MEM_VECTOR_OFFS     (0x00000)
#define MEM_DESCRIPTOR_OFFS (0x04000)
#define MEM_PTR_OFFS        (0x08000)
#define MEM_AUX_OFFS        (0x0c000)
#define MEM_REGISTER_OFFS   (0x10000)

#define REG_INSN 0
#define REG_STATUS 0
#define REG_INT_ST 1
#define REG_INT_EN 2

#define REG_CKGEN_DIV 4
#define REG_CKGEN_RTDIV 5
#define REG_CFG 6
#define REG_TMR 7

#define REG_MEM_WR_ADDR 8
#define REG_MEM_RD_ADDR 9

#define REG_DESC_LO 10
#define REG_DESC_HI 11

#define CFG_CLK_SEL(X) (((X) & 0xf) << 0)
#define GET_CLK_SEL(CFG) (((CFG) >> 0) & 0xf)
#define CFG_LOOP (1 << 4)
#define CFG_RTCK_EN (1 << 5)
#define CFG_RST (1 << 6)
#define CFG_UART_EN (1 << 7)

#define CFG_TAP_TRST (1 << 10)
#define CFG_TAP_NRST (1 << 11)

#define IRQ_TAP (1 << 0)
#define IRQ_TMO (1 << 1)
#define IRQ_1KHZ (1 << 2)
#define IRQ_TMR  (1 << 3)

/* JTAG controller instructions 
   DP: Descriptor Pointer 
    E: End state
    N: Counter
 */
#define INSN_IR_SCAN(DP, E)  (0xf000 | (((E) & 0xf) << 8) | ((DP) & 0xff))
#define INSN_DR_SCAN(DP, E)  (0xe000 | (((E) & 0xf) << 8) | ((DP) & 0xff))
#define INSN_TAP_RESET(N, E) (0xd000 | (((E) & 0xf) << 8) | ((N) & 0xff))
#define INSN_RUN_TEST(N, E)  (0xc000 | (((E) & 0xf) << 8) | ((N) & 0xff))
#define INSN_IR_PAUSE(N, E)  (0xb000 | (((E) & 0xf) << 8) | ((N) & 0xff))
#define INSN_DR_PAUSE(N, E)  (0xa000 | (((E) & 0xf) << 8) | ((N) & 0xff))
#define INSN_SET_STATE(E)    (0x9000 | ((E) & 0xf))


/* Descriptor Pointer fields */
#define PTR_DESC_LEN        (0x00ff << 0)
#define PTR_DESC_ADDR       (0x00ff << 8)

/* Vector descriptor fields */
#define DESC_VEC_LEN        (0x03ff << 0)
#define DESC_TX_VEC_ADDR    (0x03ff << 10)
#define DESC_RX_VEC_ADDR    (0x03ff << 20)
#define DESC_TX_VEC_MSBF    (1 << 30)
#define DESC_RX_VEC_MSBF    (1 << 31)

/* Vector options */
#define RX_DIR       (1 << 31)
#define TX_DIR       (1 << 30)
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

/* | 31 | 30 | 29 .. 20 | 19 .. 10 | 9 .. 0 | 
   | RM | TM |    RA    |    TA    |   N    | */

#define JTAG_DESC(RA, RM, TA, TM, N) (\
	(((RA) & 0x07fe) << 19) | (((RM) & 1) << 31) | \
	(((TA) & 0x07fe) << 9) | (((TM) & 1) << 30) | (((N) - 1) & 0x03ff))

#define JTAG_PTR(DESC, LEN) ((((DESC) & 0x0ff) << 8) | (((LEN) - 1) & 0x0ff))

#define DESC_TX_ADDR(DESC) ((DESC >> 9) & 0x7fe)
#define DESC_TX_MSBF(DESC) ((DESC >> 30) & 1)
#define DESC_RX_ADDR(DESC) ((DESC >> 19) & 0x7fe)
#define DESC_RX_MSBF(DESC) ((DESC >> 31) & 1)
#define DESC_LEN(DESC) ((DESC & 0x3ff) + 1)

#include <stdint.h>
#include <thinkos.h>
#include <sys/stm32f.h>
#include "jtag.h"

struct jtag_io { 
	union {
		uint16_t h[1024];
		uint32_t w[512];
		uint64_t d[256];
	} vec;
	uint32_t res1[(0x4000 - 0x2000 + 2048) / 4];
	union {
		uint16_t h[128];
		uint32_t w[64];
		uint64_t d[32];
	} desc;
	uint32_t res2[(0x8000 - 0x4000 + 256) / 4];
	union {
		uint16_t h[256];
		uint32_t w[128];
		uint64_t d[64];
	} ptr;
	uint32_t res3[(0xc000 - 0x8000 + 512) / 4];
	union {
		uint16_t h[256];
		uint32_t w[128];
		uint64_t d[64];
	} aux;
	uint32_t res4[(0x100000 - 0xc000 + 512) / 4];
	union {
		volatile uint16_t reg[8];
		volatile uint32_t r32[4];
		volatile uint64_t r64[2];
		struct {
			volatile uint16_t inst;
			volatile uint16_t int_st;
			volatile uint16_t int_en;
			volatile uint16_t r3;

			volatile uint16_t div;
			volatile uint16_t rtdiv;
			volatile uint16_t cfg;
			volatile uint16_t tmr;
		};
	};
};

/* FIXME: irq number */
#ifndef JTAG3DRV_IRQ
#define JTAG3DRV_IRQ 23
#endif

#ifndef JTAG3DRV_BASE
#define JTAG3DRV_BASE (0x60000000)
#endif

#define JTAG_IO ((struct jtag_io *)JTAG3DRV_BASE)

#define JTAGDRV_VEC ((volatile uint16_t *)(JTAG3DRV_BASE + MEM_VECTOR_OFFS))
//#define JTAGDRV_DESC ((uint32_t *)(JTAG3DRV_BASE + MEM_DESCRIPTOR_OFFS))
#define JTAGDRV_DESC ((uint16_t *)(JTAG3DRV_BASE + MEM_DESCRIPTOR_OFFS))
#define JTAGDRV_PTR ((uint16_t *)(JTAG3DRV_BASE + MEM_PTR_OFFS))
#define JTAGDRV_AUX ((uint32_t *)(JTAG3DRV_BASE + MEM_AUX_OFFS))
#define JTAGDRV_REG ((volatile uint16_t *)(JTAG3DRV_BASE + MEM_REGISTER_OFFS))

static inline uint16_t reg_rd(unsigned int n) {
	return JTAGDRV_REG[n];
}

static inline void reg_wr(unsigned int n, uint16_t val) {
	JTAGDRV_REG[n] = val;
}

static inline uint16_t vec_rd16(unsigned int offs) {
	return JTAGDRV_VEC[offs / 2];
}

static inline void vec_wr16(unsigned int offs, uint16_t val) {
	JTAGDRV_VEC[offs / 2] = val;
}

static inline uint32_t vec_rd32(unsigned int offs) {
	unsigned pos = offs / 2;
	return JTAGDRV_VEC[pos] + (JTAGDRV_VEC[pos + 1] << 16);
}

static inline void vec_wr32(unsigned int offs, uint32_t val) {
	unsigned pos = offs / 2;
	JTAGDRV_VEC[pos] = val;
	JTAGDRV_VEC[pos + 1] = val >> 16;
}

static inline uint32_t desc_rd(unsigned int pos) {
	return JTAGDRV_DESC[pos];
}

static inline void desc_wr(unsigned int pos, uint32_t desc) {
	JTAGDRV_DESC[pos * 2] = desc;
	JTAGDRV_DESC[pos * 2 + 1] = desc >> 16;
//	JTAGDRV_DESC[pos] = desc;
}

static inline uint32_t aux_rd(unsigned int pos) {
	return JTAGDRV_AUX[pos];
}

static inline void aux_wr(unsigned int pos, uint32_t data) {
	JTAGDRV_AUX[pos] = data;
}


static inline void ptr_wr(unsigned int id, uint16_t ptr) {
	JTAGDRV_PTR[id] = ptr;
}

static inline uint32_t __attribute__((always_inline)) 
jtag3drv_int_wait(unsigned int irq_mask) {
	struct stm32f_exti * exti = STM32F_EXTI;
	uint32_t isr;
	while (((isr = reg_rd(REG_INT_ST)) & irq_mask) == 0) {
		thinkos_irq_wait(JTAG3DRV_IRQ);
   		exti->pr = (1 << 6); /* clear external interrupt pending flag */
	}
	return isr;
}

static inline uint32_t __attribute__((always_inline)) 
jtag3drv_int_wait_any(void) {
	struct stm32f_exti * exti = STM32F_EXTI;
	uint32_t isr;
	while ((isr = reg_rd(REG_INT_ST)) == 0) {
		thinkos_irq_wait(JTAG3DRV_IRQ);
   		exti->pr = (1 << 6); /* clear external interrupt pending flag */
	}
	return isr;
}

static inline void insn_irscan(unsigned int dsc_ptr, unsigned int final_state) {
	reg_wr(REG_INSN, INSN_IR_SCAN(dsc_ptr, final_state));
	jtag3drv_int_wait(IRQ_TAP);
}

static inline void insn_drscan(unsigned int dsc_ptr, unsigned int final_state) {
	reg_wr(REG_INSN, INSN_DR_SCAN(dsc_ptr, final_state));
	jtag3drv_int_wait(IRQ_TAP);
}

static inline void insn_run_test(unsigned int cnt, unsigned int final_state) { 
	reg_wr(REG_INSN, INSN_RUN_TEST(cnt, final_state));
	jtag3drv_int_wait(IRQ_TAP);
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

void jtag3ctrl_trst(bool set);
	
void jtag3ctrl_nrst(bool set);

void jtag3ctrl_loopback(bool enable);

void jtag3ctrl_int_en(unsigned int mask);

void jtag3ctrl_int_clr(unsigned int mask);

void jtag3ctrl_sys_rst(void);

void jtag3ctrl_rtck(bool enable);

void jtag3ctrl_aux_uart(bool enable);

bool jtag3ctrl_irq_status(void);

bool jtag3ctrl_fpga_probe(void);


#ifdef __cplusplus
}
#endif

#endif /* __JTAG3DRV_H__ */
