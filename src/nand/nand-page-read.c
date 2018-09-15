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
#include <sys/delay.h>

/* loadd 10 8-bit values from spare buffer and 
   convert to 8 10-bit values */
void nand_ecc_load(uint8_t * spare, uint16_t * ecc, uint32_t offs)
{
	/* 11 00000000 */
	ecc[0] = spare[offs + 0] | 
		((spare[offs + 1] & 0x03) << 8);
	/* 22 22111111 */
	ecc[1] = ((spare[offs + 1] >> 2) & 0x3f) | 
		((spare[offs + 2] & 0x0f) << 6);
	/* 33 33332222 */
	ecc[2] = ((spare[offs + 2] >> 4) & 0x0f) | 
		((spare[offs + 3] & 0x3f) << 4);
	/* 44 44444433 */
	ecc[3] = ((spare[offs + 3] >> 6) & 0x03) | 
		(spare[offs + 4] << 2);

	DCC_LOG4(LOG_TRACE, "ecc[0..3]=%04x %04x %04x %04x", 
			 ecc[0], ecc[1], ecc[2], ecc[3]);

	/* 66 55555555 */
	ecc[4] = spare[offs + 5] | 
		((spare[offs + 6] & 0x03) << 8);
	/* 77 77666666 */
	ecc[5] = ((spare[offs + 6] >> 2) & 0x3f) | 
		((spare[offs + 7] & 0x0f) << 6);
	/* 88 88887777 */
	ecc[6] = ((spare[offs + 7] >> 4) & 0x0f) | 
		((spare[offs + 8] & 0x3f) << 4);
	/* 99 99999988 */
	ecc[7] = ((spare[offs + 8] >> 6) & 0x03) | 
		(spare[offs + 9] << 2);
		
	DCC_LOG4(LOG_TRACE, "ecc[4..7]=%04x %04x %04x %04x", 
			 ecc[4], ecc[5], ecc[6], ecc[7]);
}

int nand_page_read(nand_dev_t * nand, int page, void * buf)
{
	nand_chip_t * chip;
	void * arg = nand->arg;
	uint8_t * data  = (uint8_t *)buf;
	uint32_t spare[16];
	uint16_t ecc[8];
	int ret;

	chip = &nand->chip[nand->chip_sel];

	DCC_LOG3(LOG_INFO, "nand=%p arg=%p chip=%p", nand, arg, chip);

	if (chip->data_size <= nand->data_max) {

		/* The ECC engine can handle a full page. That's the easy case */
		DCC_LOG(LOG_INFO, "Full page reading...");
	
		/* Write read command */
		nand->io->cmd(arg, chip, NAND_READ_PAGE);

		/* Set the read address to the beginning of the page */
		nand_col_addr(arg, chip, nand->io, 0);
		nand_row_addr(arg, chip, nand->io, page);

		/* Additional confirm command for large page devices */
		if (chip->pg_large)
			nand->io->cmd(arg, chip, NAND_READ_30H);

		/* Wait for data to be available */
		if ((ret = nand->io->ready_wait(arg, chip)) < 0) {

			DCC_LOG(LOG_WARNING, "ready_wait() fail!");
			return ret;
		}

		/* Clear the ECC hardware before starting */
		nand->ecc->disable(arg, chip);

		/* Enable ECC */
		nand->ecc->enable(arg, chip);

		DCC_LOG1(LOG_TRACE, "Reading %d bytes of data...", chip->data_size);
		/* read the data bytes */
		nand->io->read(arg, chip, data, chip->data_size); 

		DCC_LOG8(LOG_INFO, "Data: %02x %02x %02x %02x %02x %02x %02x %02x", 
				 data[0], data[1], data[2], data[3],
				 data[4], data[5], data[6], data[7]);

		if (nand->ecc_en) {
			/* Stop ECC */
			nand->ecc->disable(arg, chip);

			DCC_LOG1(LOG_INFO, "Reading %d spare bytes...", 
					 chip->spare_size);

			/* read the spare bytes */
			nand->io->read(arg, chip, spare, chip->spare_size); 

			DCC_LOG4(LOG_INFO, "Spare: %08x %08x %08x %08x",
					 spare[0], spare[1], spare[2], spare[3]);

			if ((spare[0] == 0xffffffff) && (spare[1] == 0xffffffff) && 
				(spare[2] == 0xffffffff) && (spare[3] == 0xffffffff)) {
				DCC_LOG(LOG_TRACE, "no ECC data");

				return chip->data_size;
			}

			/* load the ECC data from spare bytes */
			nand_ecc_load((uint8_t *)spare, ecc, chip->ecc_offs);

			if ((ret = nand->ecc->correct(arg, chip, spare, ecc)) < 0) {
				DCC_LOG(LOG_WARNING, "ecc->correct() fail!");
				return -1;
			}
		}

		return chip->data_size;
	}

	return -1;
}

