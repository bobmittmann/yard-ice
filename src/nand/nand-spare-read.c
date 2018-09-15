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

/* read the spare bytes region */
int nand_spare_read(nand_dev_t * nand, int page, void * spare)
{
	nand_chip_t * chip = nand->chip;
	void * arg = nand->arg;
	int ret;

	/* For small page devices, set pointer */
	nand->io->cmd(arg, chip, NAND_EXTRA_PAGE);

	/* Write read command */
	nand->io->cmd(arg, chip, NAND_READ_PAGE);

	/* Set the read address to the beginning of the page */
	nand_col_addr(arg, chip, nand->io, 0);
	nand_row_addr(arg, chip, nand->io, page);

	/* Wait for data to be available */
	if ((ret = nand->io->ready_wait(arg, chip)) < 0) {
		DCC_LOG(LOG_WARNING, "ready_wait() fail!");
		return ret;
	}

	/* read the spare bytes */
	nand->io->read(arg, chip, spare, chip->spare_size); 

	return chip->spare_size;
}

