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
int nand_bb_check(nand_dev_t * nand, int block)
{
	nand_chip_t * chip = nand->chip;
	void * arg = nand->arg;
	uint32_t spare[64];
	uint8_t * cp;
	int page;
	int ret;
	int i;

	/* bounds checking */
	if (block >= nand->chip->num_blocks)
		return -1;

	page = nand->chip->pgs_per_blk * block;

	DCC_LOG2(LOG_INFO, "block %d, %d spare bytes...", 
			 block, chip->spare_size);

	/* extra page command */
	nand->io->cmd(arg, chip, NAND_EXTRA_PAGE);

	/* Set the read address */
	nand_col_addr(arg, chip, nand->io, 0);
	nand_row_addr(arg, chip, nand->io, page);

	/* Wait for data to be available */
	if ((ret = nand->io->ready_wait(arg, chip)) < 0) {
		DCC_LOG(LOG_WARNING, "ready_wait() fail!");
		return ret;
	}

	/* Read spare bytes (includes ECC data)  */
	/* read the spare bytes */
	nand->io->read(arg, chip, spare, chip->spare_size); 

	cp = (uint8_t *)spare;

	for (i = 0; i < chip->ecc_offs; i++) {
		if (cp[i] != 0xff) {
			DCC_LOG3(LOG_TRACE, "bad block: %d, page: %d, offs: 0x%08x", 
					 block, page, page * chip->data_size);
			DCC_LOG8(LOG_TRACE, "Spare: %08x %08x %08x %08x "
					 "%08x %08x %08x %08x", 
					 spare[0], spare[1], spare[2], spare[3],
					 spare[4], spare[5], spare[6], spare[7]);
			return -1;
		}
	}

	DCC_LOG(LOG_INFO, "block ok.");
	return 0;
}

