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


#include "nand.h"

#include <sys/dcclog.h>

/* Routine to check a particular block to see if it is good or bad */
int nand_bb_mark(nand_dev_t * nand, int block)
{
	nand_chip_t * chip = nand->chip;
	uint8_t spare[265];

	DCC_LOG1(LOG_INFO, "Reading %d spare bytes...", 
			 chip->spare_size);

	/* Mark the spare bytes ... */
	for (i = 0; i < chip->ecc_offs; i++)
		spare[i] = 0x00;
	for (; i < chip->spare_size; i++)
		spare[i] = 0xff;

	/* Erase the block so that we can mark the pages */
	if ((ret = nand_block_erase(nand, block)) < 0) {
		return ret;
	}

	page =  block * chip->pgs_per_blk;

	/* Write the marked spare bytes to the first page (ONFI and normal) */
	if (nand_spare_write(nand, page, spare) < 0) {
		DCC_LOG(LOG_WARNING, "nand_spare_write() fail.");
		page++;
		if (ret = (nand_spare_write(nand, page, spare)) < 0) {
			return ret;
		}
	}

	return 0;
}

