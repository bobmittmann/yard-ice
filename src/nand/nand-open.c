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

#include <stdlib.h>
#include <sys/dcclog.h>

/* XXX: number of operations not complete... */

/* Initialze NAND interface */
int nand_open(nand_dev_t * nand, const void * arg, 
			  nand_chip_t * chip, const nand_io_op_t * io,
			  const nand_ecc_op_t * ecc, const nand_bb_op_t * bb)
{
	uint32_t size_mb;
	int ret;
	int n;
	int i;

	if ((nand == NULL) || (chip == NULL) || (io == NULL)) {
		DCC_LOG(LOG_WARNING, "invalid argument!");
		return -1;
	}

	if (nand->is_open) {
		DCC_LOG(LOG_WARNING, "already open!");
		return -1;
	}

	/* Chip size in Mbits */
	size_mb = ((chip->pgs_per_blk * chip->num_blocks) >> 9) * 
		(chip->data_size >> 8);

	/* Chip size in Mbits */
	DCC_LOG1(LOG_TRACE, "Chip size: %dMbits", size_mb);

	if (size_mb == 0) {
		DCC_LOG(LOG_WARNING, "Invalid chip data!");
		return -1;
	}

	nand->arg = (void *)arg;
	nand->io = io;
	nand->bb = bb;
	nand->ecc = ecc;
	nand->chip = chip;

	if ((ret = nand_reset(arg, chip, io)) < 0) {
		DCC_LOG(LOG_WARNING, "nand_reset() fail!");
		return ret;
	}

	/* Setup row and column address cycle count */
	chip->col_addr_cnt = (chip->data_size > 512) ? 2 : 1;

	n = chip->num_blocks * chip->pgs_per_blk;
	
	DCC_LOG1(LOG_TRACE, "%d pages", n);

	for (i = 0; (n >>= 1) > 0; i++);

	DCC_LOG1(LOG_TRACE, "%d row address bits", i);

	chip->row_addr_cnt = (i + 0x7) >> 3;

	DCC_LOG2(LOG_TRACE, "Addr cycles: row=%d, col=%d", 
			 chip->row_addr_cnt, chip->col_addr_cnt);

#if 0
	/* XXX: Assign the number of operations per page value */
	chip->numOpsPerPage = 0;
	while ((chip->numOpsPerPage * DEVICE_NAND_MAX_BYTES_PER_OP) < chip->data_size)
		chip->numOpsPerPage++;
#endif

#if 0
	/* Assign the bytes per operation value */
	if (chip->data_size < ecc->data_max)
		nand->dataBytesPerOp = chip->data_size;
	else
		nand->dataBytesPerOp = ecc->data_max;

	/* Assign the spare bytes per operation value */
	if (chip->spare_size < ecc->spare_max)
		nand->spareBytesPerOp = chip->spare_size;
	else
		nand->spareBytesPerOp = ecc->data_max;
#endif

#if 0
	/* Check to make sure there are enough spare bytes to satisfy our needs */
	if ((chip->numOpsPerPage * chip->spareBytesPerOp) > chip->spareBytesPerPage)
		return -1;

	/* Check and make sure we have enough spare bytes per op */
	if (chip->spareBytesPerOp < DEVICE_NAND_MIN_SPAREBYTES_PER_OP)
		return -1;
#endif

	/* Assign the large page flag */
	chip->pg_large = (chip->data_size > 512) ? 1 : 0;

#ifdef IPNC_DM365
	if (chip->id == 0x75 && chip->mfg_id == 0xec) {
		SYSTEM->PINMUX[2] |= 0x80;
		GPIO->DIR23 &= 0xfeffffff;
		GPIO->CLRDATA23 = 0x01000000;
	}
#endif

	nand->is_open = 1;

	DCC_LOG(LOG_TRACE, "[OPEN]");

	return 0;
}

