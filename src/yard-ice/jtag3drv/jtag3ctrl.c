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
 * @file jtag3ctrl.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#ifdef CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "altera.h"
#include "jtag3drv.h"
#include "jtag.h"

#include <sys/stm32f.h>

#include <sys/dcclog.h>
#include <altera.h>

uint16_t volatile * jtagdrv_reg = JTAGDRV_REG;
uint16_t volatile * jtagdrv_vec = JTAGDRV_VEC;

/* read a vector of 'len' bits from the controller's memory */
void jtag3ctrl_vec_rd(unsigned int addr, uint16_t * buf, int len)
{
	uint16_t * ptr = (uint16_t *)buf;
	int n;
	int i;

	DCC_LOG2(LOG_INFO, "addr:0x%03x len:%d", addr, len);

	/* round down the address */
	addr /= sizeof(uint16_t);
	/* get the number of words to be transfered */
	n = (len + 15) / 16;

	for (i = 0; i < n; i++) {
		DCC_LOG2(LOG_MSG, "0x%03x -> 0x%04x", addr + i, jtagdrv_vec[addr + i]);
		ptr[i] = jtagdrv_vec[addr + i];
	}
}

/* write a vector of 'len' bits to the controller's memory */
void jtag3ctrl_vec_wr(unsigned int addr, const uint16_t * buf, int len)
{
	uint16_t * ptr = (uint16_t *)buf;
	int n;
	int i;

	DCC_LOG2(LOG_INFO, "addr:0x%03x len:%d", addr, len);

	/* round down the address */
	addr /= sizeof(uint16_t);
	/* get the number of words to be transfered */
	n = (len + 15) / 16;

	for (i = 0; i < n; i++) {
		jtagdrv_vec[addr + i] = ptr[i];
		DCC_LOG2(LOG_MSG, "0x%03x -> 0x%04x", addr + i, jtagdrv_vec[addr + i]);
	}
}

/*
 Current  Final
 State    State     Default State Path
 RESET    RESET     At least one TCK cycle applied with TMS = 1
 RESET    IDLE      RESET-IDLE
 RESET    DRPAUSE   RESET-IDLE-DRSELECT-DRCAPTURE-DREXIT1-DRPAUSE
 RESET    IRPAUSE   RESET-IDLE-DRSELECT-IRSELECT-IRCAPTURE-IREXIT1-IRPAUSE
 IDLE     RESET     IDLE-DRSELECT-IRSELECT-RESET
 IDLE     IDLE      At least one TCK cycle applied with TMS = 0
 IDLE     DRPAUSE   IDLE-DRSELECT-DRCAPTURE-DREXIT1-DRPAUSE
 IDLE     IRPAUSE   IDLE-DRSELECT-IRSELECT-IRCAPTURE-IREXIT1-IRPAUSE
 DRPAUSE  RESET     DRPAUSE-DREXIT2-DRUPDATE-DRSELECT-IRSELECT-RESET
 DRPAUSE  IDLE      IDLE-DRPAUSE-DREXIT2-DRUPDATE-IDLE
 DRPAUSE  DRPAUSE   At least one TCK cycle applied with TMS = 0
 DRPAUSE  IRPAUSE   DRPAUSE-DREXIT2-DRUPDATE-DRSELECT-
                    IRSELECT-IRCAPTURE-IREXIT1-IRPAUSE
 IRPAUSE  RESET     IRPAUSE-IREXIT2-IRUPDATE-DRSELECT-IRSELECT-RESET
 IRPAUSE  IDLE      IRPAUSE-IREXIT2-IRUPDATE-IDLE
 IRPAUSE  DRPAUSE   IRPAUSE-IREXIT2-IRUPDATE-DRSELECT-DRCAPTURE-DREXIT1-DRPAUSE
 IRPAUSE  IRPAUSE   At least one TCK cycle applied with TMS = 0
*/

int jtag3ctrl_state(int state)
{
	return 0;
}

int jtag3ctrl_wait(int state)
{
	return 0;
}

/*
 * TRST is a low level assertion signal...
 * TRST is synchronous and must be clocked by TCK
 */
void jtag3ctrl_trst(bool assert)
{
	int cfg = reg_rd(REG_CFG);

	if (assert)
		cfg |= CFG_TAP_TRST;
	else
		cfg &= ~CFG_TAP_TRST;
	reg_wr(REG_CFG, cfg);
}

void jtag3ctrl_nrst(bool assert)
{
	int cfg = reg_rd(REG_CFG);
	if (assert)
		cfg |= CFG_TAP_NRST;
	else
		cfg &= ~CFG_TAP_NRST;
	reg_wr(REG_CFG, cfg);
}

void jtag3ctrl_loopback(bool enable)
{
	unsigned int cfg;

	cfg = reg_rd(REG_CFG);
	cfg = (enable) ? cfg | CFG_LOOP : cfg & ~CFG_LOOP;
	reg_wr(REG_CFG, cfg);
}

void jtag3ctrl_rtck(bool enable)
{
	int cfg = reg_rd(REG_CFG);
	if (enable)
		cfg |= CFG_RTCK_EN;
	else
		cfg &= ~CFG_RTCK_EN;
	reg_wr(REG_CFG, cfg);
}

void jtag3ctrl_int_en(unsigned int mask)
{
	reg_wr(REG_INT_EN, mask);
}

void jtag3ctrl_int_clr(unsigned int mask)
{
	reg_wr(REG_INT_ST, mask);
}

void jtag3ctrl_sys_rst(void)
{
	/* reset the controller */
	reg_wr(REG_CFG, CFG_RST);
}

#if 0
/* 75 MHz */
const uint32_t jtag_clk_tab[] = {
	8421052, /* (0) mck=a div=19 */
	9411764, /* (1) mck=a div=17 */
	10256410, /* (2) mck=b div=13 */
	12121212, /* (3) mck=b div=11 */
	12307692, /* (4) mck=a div=13 */
	14545454, /* (5) mck=a div=11 */
	19047619, /* (6) mck=b div=7 */
	22857142, /* (7) mck=a div=7 */
	26666666, /* (8) mck=b div=5 */
	32000000, /* (9) mck=a div=5 */
	44444444, /* (10) mck=b div=3 */
	53333333, /* (11) mck=a div=3 */
	66666666, /* (12) mck=b div=2 */
	80000000, /* (13) mck=a div=2 */
	133333333, /* (14) mck=b div=1 */
	160000000, /* (15) mck=a div=1 */
};

unsigned int jtag3ctrl_set_speed(unsigned int freq)
{
	uint32_t brg_freq;
	uint32_t brg_fmax;
	uint32_t brg_fmin;
	uint32_t tap_freq;
	uint32_t cfg;
	uint32_t clk;
	int32_t div;
	int sel;
	int err;
	int min = INT_MAX;
	int best_sel;

	brg_fmax = jtag_clk_tab[15] / 2;
	brg_freq = freq * 2;

	if (brg_freq > brg_fmax) {
		brg_freq = brg_fmax;
	}

	brg_fmin = jtag_clk_tab[0] / 0x10000;
	if (brg_freq < brg_fmin) {
		brg_freq = brg_fmin;
	}

	DCC_LOG1(LOG_INFO, "brg freq=%d", brg_freq);

	/* find the best matching frequency, but never bigger than
	   the desired frequency */
	best_sel = -1;
	for (sel = 0; sel <= 15; sel++) {
		clk = jtag_clk_tab[sel];
		div = ((clk + (brg_freq / 2)) / brg_freq) - 2;
		/* range checking */
		if (div < 0)
			div = 0;
		if (div >= 0xffff)
			div = 0xfffe;

		err = brg_freq - (clk / (div + 2));

		DCC_LOG3(LOG_INFO, "clk=%d div=%d err=%d", clk, (div + 2), 
				 brg_freq - (clk / (div + 2)));

		if ((err >= 0) && (err < min)) {
			min = err;
			best_sel = sel;
			if (err == 0)
				break;
		}
	}

	if (best_sel < 0)
		best_sel = 0;

	clk = jtag_clk_tab[best_sel];
	div = ((clk + (brg_freq / 2)) / brg_freq) - 2;
	/* range checking */
	if (div < 0)
		div = 0;
	if (div >= 0xffff)
		div = 0xfffe;

	/* select the clock source */
	cfg = reg_rd(REG_CFG) & ~CFG_CLK_SEL(0xf);
	reg_wr(REG_CFG, cfg | CFG_CLK_SEL(best_sel));
	/* configure the brg divisor */
	reg_wr(REG_BRG_DIV, div);

	brg_freq = clk / (div + 2);
	tap_freq = brg_freq / 2;

	DCC_LOG2(LOG_INFO, "sel: %d div: %d", best_sel, div + 2);

	DCC_LOG4(LOG_TRACE, "clk=%d.%06d MHz, TAP freq: %d.%06d MHz", 
		clk / 1000000, clk % 1000000, 
		tap_freq / 1000000, tap_freq % 1000000); 

	return tap_freq;
}

unsigned int jtag3ctrl_get_speed(void)
{
	unsigned int div;
	uint32_t clk;
	int sel;

	/* clock source */
	sel = GET_CLK_SEL(reg_rd(REG_CFG));
	clk = jtag_clk_tab[sel];
	/* brg divisor */
	div = reg_rd(REG_BRG_DIV) + 2;

	DCC_LOG3(LOG_TRACE, "sel: %d (%d MHz) div: %d", sel, clk / 1000000, div);
	return (clk / div) / 2;
}
#endif

int jtag3ctrl_init(const void * rbf, int size)
{
	int ret;

	/* Enable clock output */
	stm32f_mco2_init();

	/* Configure memory controller ... */
	stm32f_fsmc_init();
		
	/* Configure external interrupt ... */
	stm32f_exti_init(STM32F_GPIOD, 6, EXTI_EDGE_RISING);

	if ((ret = altera_configure(rbf, 60000)) < 0) {
		printf(" # altera_configure() failed: %d!\n", ret);
		for(;;);
	};

	printf("- FPGA configuration done.\n");

	/* wait for the FPGA initialize */
	__os_sleep(20);

	/* initial configuration */
	reg_wr(REG_CFG, 0);

	return 0;
}

