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

int nand_block_erase(nand_dev_t * nand, int block)
{
	nand_chip_t * chip;
	void * arg = nand->arg;
	int status;
	int ret;

	chip = &nand->chip[nand->chip_sel];

	/* bounds checking */
	if (block >= chip->num_blocks) {
		DCC_LOG2(LOG_WARNING, "block(%d) >= chip->num_blocks(%d)", 
				 block, chip->num_blocks);
		return -1;
	}

	DCC_LOG1(LOG_INFO, "Erasing block %d", block);

	/* First erase command */
	nand->io->cmd(arg, chip, NAND_BERASEC1);

	/* Row address bytes */
	nand_row_addr(arg, chip, nand->io, chip->pgs_per_blk * block);

	/* Second erase command */
	nand->io->cmd(arg, chip, NAND_BERASEC2);

	/* Wait for the end of operation */
	if ((ret = nand->io->ready_wait(arg, chip)) < 0)
		return ret;

	/* Get the status of the operation */
	status = nand_status(arg, chip, nand->io);

	if (!(status & NAND_STATUS_READY)) {
		DCC_LOG(LOG_WARNING, "not READY!!!");
		return -1;
	}

	if (status & NAND_STATUS_ERROR) {
		DCC_LOG(LOG_WARNING, "erase ERROR!!!");
		return -1;
	}

	return 0;
}

