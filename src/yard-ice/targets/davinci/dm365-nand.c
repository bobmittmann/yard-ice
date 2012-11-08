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

#include <sys/dcclog.h>

#include <sys/clock.h>
#include "nand.h"
#include "target/davinci.h"

/****************************************************************************
  ECC
 ****************************************************************************/

void dm365_nand_ecc_enable(const void * arg, nand_chip_t * chip)
{
	ice_drv_t * ice = (ice_drv_t *)arg;
	/* Write appropriate bit to start ECC 
	   calcualtions (bit 12 for four bit ECC) */

	AEMIF_REG_SET(AEMIF_NANDFCR, NANDFCR_4BITECC_START);
}

void dm365_nand_ecc_disable(const void * arg, nand_chip_t * chip)
{
	ice_drv_t * ice = (ice_drv_t *)arg;
	volatile uint32_t dummy;

	DCC_LOG(LOG_TRACE, ".");

	/* Read any ECC register to end the calculation and 
	   clear the ECC start bit */
	dummy = AEMIF_REG_RD(AEMIF_NAND4BITECC1);
	dummy = dummy;
}


int dm365_nand_ecc_correct(const void * arg, nand_chip_t * chip,
						 uint8_t * data, uint16_t * ecc)
{
	ice_drv_t * ice = (ice_drv_t *)arg;
	struct timespec tv;
	uint32_t tm_top;
	uint32_t tm_cmp;
	uint32_t state;
	uint32_t errval;
	uint16_t fixval;
	uint32_t ecc1;
	uint32_t ecc2;
	uint32_t ecc3;
	uint32_t ecc4;
	uint32_t err;
	uint32_t addr;
	uint16_t offs;
	uint32_t tmp;
	uint32_t fsr;
	int n;
	int i;

	DCC_LOG(LOG_INFO, ".");

	/* Clear bit13 of NANDFCR */
	tmp = AEMIF_REG_RD(AEMIF_NANDERRADD1);
	tmp = tmp;

	/* Load the syndrome10 (from 7 to 0) values */
	for (i = 7; i >= 0; i--) {
//	for (i = 0; i < 8; i++) {
		AEMIF_REG_WR(AEMIF_NAND4BITECCLOAD, ecc[i]);
	}

	/* Read the EMIF status and version (dummy call)  */
	tmp = AEMIF_REG_RD(AEMIF_ERCSR);

	/* Check if error is detected */
	ecc1 = AEMIF_REG_RD(AEMIF_NAND4BITECC1);
	ecc2 = AEMIF_REG_RD(AEMIF_NAND4BITECC2);
	ecc3 = AEMIF_REG_RD(AEMIF_NAND4BITECC3);
	ecc4 = AEMIF_REG_RD(AEMIF_NAND4BITECC4);

	/* Check if error is detected */
	err = (ecc1 & 0x03ff03ff) | (ecc2 & 0x03ff03ff) | 
		(ecc3 & 0x03ff03ff) | (ecc4 & 0x03ff03ff);

	DCC_LOG4(LOG_INFO, "ecc[3..0]=%04x %04x %04x %04x", 
			 ecc1 & 0xffff, ecc1 >> 16, ecc2 & 0xffff, ecc2 >> 16);

	DCC_LOG4(LOG_INFO, "ecc[7..4]=%04x %04x %04x %04x", 
			 ecc3 & 0xffff, ecc3 >> 16, ecc4 & 0xffff, ecc4 >> 16);

	if (err == 0)
		return 0;

	/* Start calcuating the correction addresses and values */
	AEMIF_REG_SET(AEMIF_NANDFCR, NANDFCR_4BITECC_ADD_CALC_START);

	/* Loop until timeout or the ECC calculations 
	   are complete (bit 11:10 == 00b) */

	clock_gettime(CLOCK_MONOTONIC, &tv);
	/* This sum may overflow, but is ok because we are dealing 
	   with time differences */
	tm_top = (tv.tv_sec * 1000) + (tv.tv_nsec >> 20) + chip->tmo_ms;

	for (;;) {
		state = AEMIF_REG_RD(AEMIF_NANDFSR) & NANDFSR_ECC_STATE_MASK;
		if (state <= NANDFSR_ECC_COMPLETE)
			break;
		clock_gettime(CLOCK_MONOTONIC, &tv);
		tm_cmp = (tv.tv_sec * 1000) + (tv.tv_nsec >> 20);
		if ((int32_t)(tm_top - tm_cmp) < 0)
			break;
	}

	fsr = AEMIF_REG_RD(AEMIF_NANDFSR);
	/* Read final correction state (should be 0x0, 0x1, 0x2, or 0x3) */
	state = fsr & NANDFSR_ECC_STATE_MASK;

	if (state == NANDFSR_ECC_TOO_MANY_ERRS) {
		DCC_LOG(LOG_WARNING, "too many errors!");
		return -1;
	}

	if (state > NANDFSR_ECC_COMPLETE) {
		DCC_LOG(LOG_WARNING, "timeout!");
		return -1;
	}	

	if (state == NANDFSR_ECC_NO_ERR) {
		DCC_LOG(LOG_WARNING, "no errors!");
		return 0;
	}

	/* Error detected and address calculated */
	n = NANDFSR_ECC_ERRNUM(fsr);

	switch (n) {
	case 3: /* Four errors */
		addr = AEMIF_REG_RD(AEMIF_NANDERRADD2);
		offs = 519 - ((addr & 0x03ff0000) >> 16);
		if (offs > 511) 
			return -1;
		errval = AEMIF_REG_RD(AEMIF_NANDERRVAL2);
		fixval = (errval & 0x03ff0000) >> 16;
		data[offs] ^= (uint8_t)fixval;
		/* Fall through to case 2 */
	case 2: /* Three errors */
		addr = AEMIF_REG_RD(AEMIF_NANDERRADD2);
		offs = 519 - (addr & 0x000003ff);
		if (offs > 511) 
			return -1;
		errval = AEMIF_REG_RD(AEMIF_NANDERRVAL2);
		fixval = errval & 0x000003ff;
		data[offs] ^= (uint8_t)fixval;
		/* Fall through to case 1 */
	case 1: /* Two errors */
		addr = AEMIF_REG_RD(AEMIF_NANDERRADD1);
		offs = 519 - ((addr & 0x03ff0000) >> 16 );
		if (offs > 511) 
			return -1;
		errval = AEMIF_REG_RD(AEMIF_NANDERRVAL1);
		fixval = (errval & 0x03ff0000) >> 16;
		data[offs] ^= (uint8_t)fixval;        
		/* Fall through to case 0 */
	case 0: /* One error */
		addr = AEMIF_REG_RD(AEMIF_NANDERRADD1);
		offs = 519 - (addr & 0x000003ff);
		if (offs > 511) 
			return -1;
		errval = AEMIF_REG_RD(AEMIF_NANDERRVAL1);
		fixval = errval & (0x3ff);
		data[offs] ^= (uint8_t)fixval;
		break;
	}
	return 0;
}

void dm365_nand_ecc_calculate(const void * arg, nand_chip_t * chip,
						 uint8_t * data, uint16_t * ecc)
{
	ice_drv_t * ice = (ice_drv_t *)arg;
	uint32_t ecc1;
	uint32_t ecc2;
	uint32_t ecc3;
	uint32_t ecc4;

	/* Check if error is detected */
	ecc1 = AEMIF_REG_RD(AEMIF_NAND4BITECC1);
	ecc2 = AEMIF_REG_RD(AEMIF_NAND4BITECC2);
	ecc3 = AEMIF_REG_RD(AEMIF_NAND4BITECC3);
	ecc4 = AEMIF_REG_RD(AEMIF_NAND4BITECC4);

	DCC_LOG4(LOG_INFO, "ecc[3..0]=%04x %04x %04x %04x", 
			 ecc1 & 0xffff, ecc1 >> 16, ecc2 & 0xffff, ecc2 >> 16);

	DCC_LOG4(LOG_INFO, "ecc[7..4]=%04x %04x %04x %04x", 
			 ecc3 & 0xffff, ecc3 >> 16, ecc4 & 0xffff, ecc4 >> 16);

	ecc[0] = ecc1 & 0x03ff;
	ecc[1] = (ecc1 >> 16) & 0x03ff;
	ecc[2] = ecc2 & 0x03ff;
	ecc[3] = (ecc2 >> 16) & 0x03ff;
	ecc[4] = ecc3 & 0x03ff;
	ecc[5] = (ecc3 >> 16) & 0x03ff;
	ecc[6] = ecc4 & 0x03ff;
	ecc[7] = (ecc4 >> 16) & 0x03ff;
}

const struct nand_ecc_op dm365_nand_ecc = {
	.enable = (nand_ecc_enable_t)dm365_nand_ecc_enable,
	.disable = (nand_ecc_disable_t)dm365_nand_ecc_disable,
	.correct = (nand_ecc_correct_t)dm365_nand_ecc_correct,
	.calculate = (nand_ecc_calculate_t)dm365_nand_ecc_calculate
};

/****************************************************************************
  Bad Blocks
 ****************************************************************************/

/* Mark spare bytes of page */
void dm365_nand_bb_mark_spare(const void * arg, nand_chip_t * chip, 
							  uint8_t * spare)
{

}

/* Check spare bytes of page */
uint32_t dm365_nand_bb_check_spare(const void * arg, nand_chip_t * chip, 
									 uint8_t * spare)
{
	return 0;
}

const struct nand_bb_op dm365_nand_bb = {
	.mark_spare = dm365_nand_bb_mark_spare,
	.check_spare = dm365_nand_bb_check_spare
};

#include <sys/clock.h>

int dm365_nand_ready_wait(void * arg, nand_chip_t * chip)
{
	ice_drv_t * ice = (ice_drv_t *)arg;
	struct timespec tv;
	uint32_t tm_top;
	uint32_t tm_cmp;
	
	DCC_LOG(LOG_INFO, ".");

	clock_gettime(CLOCK_MONOTONIC, &tv);
	/* This sum may overflow, but is ok because we are dealing 
	   with time differences */
	tm_top = (tv.tv_sec * 1000) + (tv.tv_nsec >> 20) + chip->tmo_ms;

#if 0
	/* Wait for the status to show busy, and then after that
	   point we start checking for it to be high.  If we 
	   don't do this we might see the status as ready before
	   it has even transitioned to show itself as busy. */
	while (AEMIF_REG_RD(AEMIF_NANDFSR) & NANDFSR_READY) {
		clock_gettime(CLOCK_MONOTONIC, &tv);
		tm_cmp = (tv.tv_sec * 1000) + (tv.tv_nsec >> 20);
		if ((int32_t)(tm_top - tm_cmp) < 0)
			return -1;
	}
#endif

	while (!(AEMIF_REG_RD(AEMIF_NANDFSR) & NANDFSR_READY)) {
		clock_gettime(CLOCK_MONOTONIC, &tv);
		tm_cmp = (tv.tv_sec * 1000) + (tv.tv_nsec >> 20);
		if ((int32_t)(tm_top - tm_cmp) < 0)
			return -1;
	}

	return 0;
}

int dm365_nand_cmd8(void * arg, nand_chip_t * chip, unsigned int cmd)
{
	ice_drv_t * ice = (ice_drv_t *)arg;

	DCC_LOG1(LOG_INFO, "cmd=0x%02x", cmd);

	return ice_wr8(ice, chip->iobase + DM365_NAND_CLE_OFFSET, cmd);
}

int dm365_nand_addr8(void * arg, nand_chip_t * chip, unsigned int addr)
{
	ice_drv_t * ice = (ice_drv_t *)arg;

	DCC_LOG1(LOG_INFO, "addr=0x%02x", addr & 0xff);

	return ice_wr8(ice, chip->iobase + DM365_NAND_ALE_OFFSET, addr);
}

int dm365_nand_rd8(void * arg, nand_chip_t * chip, void * buf, int len)
{
	ice_drv_t * ice = (ice_drv_t *)arg;
	uint32_t addr = chip->iobase + DM365_NAND_DATA_OFFSET;

	DCC_LOG1(LOG_INFO, "len=%d", len);

	if (len == 1)
		return ice_rd8(ice, addr, buf);

	return ice_fifo_rd8(ice, addr, buf, len);
}

int dm365_nand_wr8(void * arg, nand_chip_t * chip, const void * buf, int len)
{
	ice_drv_t * ice = (ice_drv_t *)arg;
	uint32_t addr = chip->iobase + DM365_NAND_DATA_OFFSET;

	DCC_LOG1(LOG_INFO, "len=%d", len);

	return ice_fifo_wr8(ice, addr, buf, len);

}

/* Initialize the NAND function in the memory interfce.
XXX: the remaining initialization like bit width and timing
 is done elswere, so it seems that this should be done 
 there as well... */
int dm365_nand_init(const void * arg, nand_dev_t * nand, 
					uint32_t iobase, int bus_width)
{
	uint32_t acr;

#if 1
	ice_drv_t * ice = (ice_drv_t *)arg;

	nand->data_max = DM365_NAND_MAX_DATA_PER_OP;
	nand->spare_max = DM365_NAND_MAX_SPARE_PER_OP;

	/* NAND Flash mode enable */

	if (iobase == DM365_CE0_BASE) {
		DCC_LOG(LOG_TRACE, "CS0");
		/* Enable NAND mode */
		AEMIF_REG_SET(AEMIF_NANDFCR, NANDFCR_CS2NAND);
		/* Configure the bus width */
		acr = AEMIF_REG_RD(AEMIF_A1CR);
		acr &= ~ACR_ASIZE;
		if (bus_width == NAND_BUS_16_BITS)
			acr |= ACR_ASIZE_16BITS;
		AEMIF_REG_WR(AEMIF_A1CR, acr);
		/* enable 4bit ECC on this chip select */
		AEMIF_REG_CLR(AEMIF_NANDFCR, NANDFCR_4BITECCSEL);
		return 0;
	}

	if (iobase == DM365_CE1_BASE) {
		DCC_LOG(LOG_TRACE, "CS1");
		/* Enable NAND mode */
		/* Configure the bus width */
		AEMIF_REG_SET(AEMIF_NANDFCR, NANDFCR_CS3NAND);
		acr = AEMIF_REG_RD(AEMIF_A2CR);
		acr &= ~ACR_ASIZE;
		if (bus_width == NAND_BUS_16_BITS)
			acr |= ACR_ASIZE_16BITS;
		AEMIF_REG_WR(AEMIF_A2CR, acr);
		/* enable 4bit ECC on this chip select */
		AEMIF_REG_SET(AEMIF_NANDFCR, NANDFCR_4BITECCSEL);
		return 0;
	}



	DCC_LOG(LOG_WARNING, "chip->iobase invalid!");
	return -1;
#else
	return 0;
#endif
}

const struct nand_io_op dm365_nand_io8 = {
	.init = (nand_init_t)dm365_nand_init,
	.cmd = (nand_cmd_t)dm365_nand_cmd8,
	.addr = (nand_addr_t)dm365_nand_addr8,
	.read = (nand_read_t)dm365_nand_rd8,
	.write = (nand_write_t)dm365_nand_wr8,
	.ready_wait = (nand_ready_wait_t)dm365_nand_ready_wait
};

/*
	.data_max = DM365_NAND_MAX_DATA_PER_OP,
	.spare_max = DM365_NAND_MAX_SPARE_PER_OP, 
	.mark_en = 0,
	.check_en = 0,
*/

