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


/* Function to write only the spare bytes region (page must be */
/* erased and not written to prior to calling this) */
int nand_spare_write(nand_dev_t * nand, int page, const void * spare)
{
	nand_chip_t * chip = nand->chip;
	void * arg = nand->arg;
	int status;
	int ret;

	/* For small page devices, set pointer */
	nand->io->cmd(arg, chip, NAND_EXTRA_PAGE);

	/* Write program command */
	nand->io->cmd(arg, chip, NAND_PGRM_START);

	/* Write address bytes */
	/* Set the read address to the beginning of the page */
	nand_col_addr(arg, chip, nand->io, 0);
	nand_row_addr(arg, chip, nand->io, page);

	/* write the spare bytes */
	nand->io->write(arg, chip, spare, chip->spare_size); 

	/* Write program end command */
	nand->io->cmd(arg, chip, NAND_PGRM_END);

	/* Wait for data to be available */
	if ((ret = nand->io->ready_wait(arg, chip)) < 0)
		return ret;

	status = nand_status(arg, chip, nand->io);

	if (!(status & NAND_STATUS_READY)) {
		DCC_LOG(LOG_WARNING, "not READY!!!");
		return -1;
	}

	if (status & NAND_STATUS_ERROR) {
		DCC_LOG(LOG_WARNING, "write ERROR!!!");
		return -1;
	}

	return chip->spare_size;
}

