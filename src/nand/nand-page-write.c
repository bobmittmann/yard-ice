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

/* Convert from 8 10-bit ECC values from HW (in 16 byte input) 
   to 10 8-bit continguous values for storage in spare bytes */
void nand_ecc_store(uint8_t * spare, uint16_t * ecc, uint32_t offs)
{
	DCC_LOG4(LOG_TRACE, "ecc[0..3]=%04x %04x %04x %04x", 
			 ecc[0], ecc[1], ecc[2], ecc[3]);
	DCC_LOG4(LOG_TRACE, "ecc[4..7]=%04x %04x %04x %04x", 
			 ecc[4], ecc[5], ecc[6], ecc[7]);

	/* 0000 0000 */
	spare[offs + 0] = ecc[0];
	/* 1111 1100 */
	spare[offs + 1] = (ecc[0] >> 8) | (ecc[1] << 2);
	/* 2222 1111 */
	spare[offs + 2] = (ecc[1] >> 6) | (ecc[2] << 4);
	/* 3322 2222 */
	spare[offs + 3] = (ecc[2] >> 4) | (ecc[3] << 6);
	/* 3333 3333 */
	spare[offs + 4] = ecc[3] >> 2;
	/* 4444 4444 */
	spare[offs + 5] = ecc[4];
	/* 5555 5544 */
	spare[offs + 6] = (ecc[4] >> 8) | (ecc[5] << 2);
	/* 6666 5555 */
	spare[offs + 7] = (ecc[5] >> 6) | (ecc[6] << 4);
	/* 7766 6666 */
	spare[offs + 8] = (ecc[6] >> 4) | (ecc[7] << 6);
	/* 7777 7777 */
	spare[offs + 9] = ecc[7] >> 2;
}

int nand_page_write(nand_dev_t * nand, int page, const void * buf)
{
	uint8_t * data  = (uint8_t *)buf;
	nand_chip_t * chip;
	void * arg = nand->arg;
	uint32_t spare[16];
	uint16_t ecc[8];
	int status;
	int ret;
	int i;

	chip = &nand->chip[nand->chip_sel];

	if (chip->data_size <= nand->data_max) {
		/* The ECC engine can handle a full page. That's the easy case */
		DCC_LOG(LOG_INFO, "Full page writing...");

		/* Write read command */
		nand->io->cmd(arg, chip, NAND_LO_PAGE);

		/* Write program command */
		nand->io->cmd(arg, chip, NAND_PGRM_START);


		/* Write address bytes */
		/* Set the read address to the beginning of the page */
		nand_col_addr(arg, chip, nand->io, 0);
		nand_row_addr(arg, chip, nand->io, page);

		/* Clear the ECC hardware before starting */
		nand->ecc->disable(arg, chip);

		/* Enable ECC */
		nand->ecc->enable(arg, chip);

		DCC_LOG1(LOG_TRACE, "Writing %d bytes of data...", chip->data_size);

		DCC_LOG8(LOG_TRACE, "Data: %02x %02x %02x %02x %02x %02x %02x %02x", 
				 data[0], data[1], data[2], data[3],
				 data[4], data[5], data[6], data[7]);

		/* write the data bytes */
		nand->io->write(arg, chip, data, chip->data_size); 

		if (nand->ecc_en) {
			nand->ecc->disable(arg, chip);
			nand->ecc->calculate(arg, chip, data, ecc);

			/* Fill in the spare bytes region with 0xff */
			for (i = 0; i < (chip->spare_size / sizeof(uint32_t)); i++) {
				spare[i] = 0xffffffff;
			}

			nand_ecc_store((uint8_t *)spare, ecc, chip->ecc_offs);

			DCC_LOG1(LOG_INFO, "Writing %d spare bytes...", 
					 chip->spare_size);

			DCC_LOG4(LOG_INFO, "Spare: %08x %08x %08x %08x",
					 spare[0], spare[1], spare[2], spare[3]);

			/* write the spare bytes */
			nand->io->write(arg, chip, spare, chip->spare_size); 
		}

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

		return chip->data_size;
	}

	return -1;
}

