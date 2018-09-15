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
 * @file nand-detect.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#include "nand.h"

#include <sys/dcclog.h>

/* convert a sequence of bytes into uint32_t 
   little endian byte order */
static inline uint32_t to_uint32_le(void * __buf)
{
	uint8_t * __p = (uint8_t *)__buf;
	return __p[0] | (__p[1] << 8) | (__p[2] << 16) | (__p[3] << 24);
}

#define TO_UINT32_LE(__BUF) ({ uint8_t * __p = (uint8_t *)(void *)(__BUF); \
	(__p[0] | (__p[1] << 8) | (__p[2] << 16) | (__p[3] << 24)) })

static inline uint16_t to_uint16_le(void * __buf)
{
	uint8_t * __p = (uint8_t *)__buf;
	return __p[0] | (__p[1] << 8);
}

#define TO_UINT16_LE(__BUF) ({ uint8_t * __p = (uint8_t *)(void *)(__BUF); \
	(__p[0] | (__p[1] << 8)) })


uint16_t onfi_crc(uint8_t * data)
{
	/* Polynomial: x^16 + x^15 + x^4 + x^1 -> 0x8005 */
	unsigned int j;
	unsigned int i;
	uint16_t crc = 0x4f4e;	
	uint16_t bit;

	for (i = 0; i < 254; i++) {
		for (j = 0x80; j; j >>= 1) {
			bit = crc & 0x8000;
			crc <<= 1;
			if (data[i] & j)
				bit ^= 0x8000;
			if (bit)
				crc ^= 0x8005;
		}
	}

	return crc;
}

int nand_onfi_get_info(const void * arg, nand_chip_t * chip,
					   const nand_io_op_t * io)
{
	uint8_t onfi[256];
	uint16_t crc;
	int ret;
	int i;
	int n;

	/* Issue read param page command */
	io->cmd(arg, chip, NANDONFI_RDPARAMPAGE);
	/* Issue lo-page address */
	io->addr(arg, chip, NAND_READ_PAGE);
	/* Wait for data to be available */
	if ((ret = io->ready_wait(arg, chip)) < 0)
		return ret;

	for (n = 3;;) {
		/* Read 256 bytes of param page data */
		for (i = 0; i < 256; i++)
			io->read(arg, chip, &onfi[i], 1);

		crc = onfi_crc(onfi);

		if (crc == to_uint16_le(&onfi[254]))
			break;

		DCC_LOG(LOG_WARNING, "ONFI CRC error!");

		if (--n == 0) {
			/* No good param page found */
			return -1;
		}
	} 

	DCC_LOG(LOG_TRACE, "valid ONFI Parameter Page");

	/* Use JEDEC manufacturer ID from Parameter Page */
	chip->mfg_id = onfi[64];

	/* Use Parameter Page data to fill in NAND device onfi structure */
	chip->data_size = to_uint16_le(&onfi[80]);
	chip->spare_size = to_uint16_le(&onfi[84]);
	chip->pgs_per_blk = onfi[92];
	chip->num_blocks = to_uint32_le(&onfi[96]) * onfi[100];
	chip->col_addr_cnt = (onfi[101] >> 4) & 0x0f;
	chip->row_addr_cnt = onfi[101] & 0x0f;

	return 0;
}

#define B_16 NAND_BUS_16_BITS
#define B_8 NAND_BUS_8_BITS

/* NAND chip geometry table */
const struct nand_info_entry nand_info_tab[] = {
	{ 0xe3,  9,  4,   512 + 16,  B_8}, /* 32Mb */
	{ 0xe5,  9,  4,   512 + 16,  B_8}, /* 32Mb */
	{ 0x39, 10,  4,   512 + 16,  B_8}, /* 64Mb */
	{ 0xe6, 10,  4,   512 + 16,  B_8}, /* 64Mb */
	{ 0x49, 10,  4,   512 + 16, B_16}, /* 64Mb */
	{ 0x59, 10,  4,   512 + 16, B_16}, /* 64Mb */
	{ 0x6b, 10,  4,   512 + 16, B_16}, /* 64Mb */
	{ 0x33, 10,  5,   512 + 16,  B_8}, /* 128Mb */
	{ 0x73, 10,  5,   512 + 16,  B_8}, /* 128Mb */
	{ 0x43, 10,  5,   512 + 16, B_16}, /* 128Mb */
	{ 0x53, 10,  5,   512 + 16, B_16}, /* 128Mb */
	{ 0x35, 11,  5,   512 + 16,  B_8}, /* 256Mb */
	{ 0x75, 11,  5,   512 + 16,  B_8}, /* 256Mb */
	{ 0x45, 11,  5,   512 + 16, B_16}, /* 256Mb */
	{ 0x55, 11,  5,   512 + 16, B_16}, /* 256Mb */
	{ 0x36, 12,  5,   512 + 16,  B_8}, /* 512Mb */
	{ 0x76, 12,  5,   512 + 16,  B_8}, /* 512Mb */
	{ 0x46, 12,  5,   512 + 16, B_16}, /* 512Mb */
	{ 0x56, 12,  5,   512 + 16, B_16}, /* 512Mb */
	{ 0x79, 13,  5,   512 + 16,  B_8}, /* 1Gb */
	{ 0x74, 13,  5,   512 + 16, B_16}, /* 1Gb */
	{ 0x71, 14,  5,   512 + 16,  B_8}, /* 2Gb */
	{ 0xf1, 10,  6,  2048 + 64,  B_8}, /* 1Gb */
	{ 0xa1, 10,  6,  2048 + 64,  B_8}, /* 1Gb */
	{ 0xb1, 10,  6,  2048 + 64, B_16}, /* 1Gb */
	{ 0xc1, 10,  6,  2048 + 64, B_16}, /* 1Gb */
	{ 0xaa, 11,  6,  2048 + 64,  B_8}, /* 2Gb */
	{ 0xda, 11,  6,  2048 + 64,  B_8}, /* 2Gb */
	{ 0xac, 12,  6,  2048 + 64,  B_8}, /* 4Gb */
	{ 0xdc, 12,  6,  2048 + 64,  B_8}, /* 4Gb */
	{ 0xd3, 13,  6,  2048 + 64,  B_8}, /* 8Gb */
	{ 0xd5, 13,  6, 4096 + 128,  B_8}, /* 16Gb */
	{ 0x00,  0,  0,  0, B_8} 
};

/* Detect the NAND flash and fill the chip structure */
int nand_detect(const void * arg, nand_chip_t * chip, const nand_io_op_t * io)
{
	const nand_info_entry_t * info = nand_info_tab;
	nand_chip_t * chip;
	const nand_io_op_t * io;
	uint8_t buf[4];
	uint8_t id[4];
	uint32_t size_mb;
	int ret;
	int i;
	int n;

	DCC_LOG(LOG_TRACE, "RESET");

	/* Send reset command */
	if ((ret = nand_reset(arg, chip, io)) < 0) {
		DCC_LOG(LOG_WARNING, "nand_reset() fail!");
		return ret;
	}

	DCC_LOG(LOG_TRACE, "reading chip ID");

	/* Issue device read ID command */
	io->cmd(arg, chip, NAND_RDID);
	io->addr(arg, chip, NAND_RDIDADD);
	/* Wait for data to be available */
	if ((ret = io->ready_wait(arg, chip)) < 0)
		return ret;

	/* Read ID bytes (to get true device ID data) */
	for (i = 0; i < 4; i++)
		io->read(arg, chip, &id[i], 1);

	DCC_LOG1(LOG_TRACE, "ID=0x%08x", to_uint32_le(id));

	DCC_LOG(LOG_TRACE, "RESET");

	if ((ret = nand_reset(arg, chip, io)) < 0) {
		DCC_LOG(LOG_WARNING, "nand_reset() fail!");
		return ret;
	}

	/* Manufacturer ID */
	chip->mfg_id = id[0];
	/* Device ID */
	chip->dev_id = id[1];

	DCC_LOG2(LOG_TRACE, "Manufacturer: 0x%02x, Device: 0x%02x",
			 chip->mfg_id, chip->dev_id);

	/* Check for ONFI compatibility */
	io->cmd(arg, chip, NAND_RDID);
	io->addr(arg, chip, NAND_ONFIRDIDADD);
	/* Wait for data to be available */
	if ((ret = io->ready_wait(arg, chip)) < 0)
		return ret;

	/* Read ID bytes (to check for ONFI signature) */
	for (i = 0; i < 4; i++)
		io->read(arg, chip, &buf[i], 1);

	/* Is it ONFI compatible ? */
	if (to_uint32_le(buf) == NANDONFI_ID) {
		DCC_LOG(LOG_TRACE, "ONFI signature Ok.");
		if (nand_onfi_get_info(arg, chip, io) == 0)
			return 0;
		/* If we fail to get the ONFI information then 
		   fallback to table lookup */
	} else {
		DCC_LOG(LOG_TRACE, "No ONFI comaptible chip.");
	}

	DCC_LOG(LOG_TRACE, "RESET");
	if ((ret = nand_reset(arg, chip, io)) < 0) {
		DCC_LOG(LOG_WARNING, "nand_reset() fail!");
		return ret;
	}

	size_mb = 0;

	/* Search for Device ID in table */
	for (i = 0; info[i].id != 0x00; i++) {
		if (info[i].id == chip->dev_id) {
			/* Number of blocks */
			chip->num_blocks = 1 << info[i].blk_bits;
			/* Pages per block */
			chip->pgs_per_blk = 1 << info[i].pg_bits;
			/* Data size (Page size less spare bytes) */
			chip->data_size = info[i].pg_size & ~0xff;
			/* Spare bytes in the page */
			chip->spare_size = info[i].pg_size & 0xff;

			chip->bus_width = info[i].bus_width;

			DCC_LOG3(LOG_TRACE, "%d blocks, %d pages/block, %d bytes/page", 
					 chip->num_blocks, chip->pgs_per_blk, chip->data_size);

			/* Chip size in Mbits */
			size_mb = ((chip->num_blocks * chip->pgs_per_blk) >> 9) * 
				(chip->data_size >> 8);
			break;
		}
	}

	if (info[i].id == 0x00) {
		DCC_LOG(LOG_WARNING, "Chip is not listed!");
	}

	/* Chip size in Mbits */
	DCC_LOG1(LOG_TRACE, "Chip size: %dMbits", size_mb);

	/* XXX: change this comment
	   If we didn't find the device in the device table or
	   we did find the device and it is reportedly greater 
	   than 1Gb in size, let's check and use the 4th ID byte 
	   This will possibly allow support for 4K-page devices with same 
	   device ID as those in table, and bigger than 1 Gb in size */
	if ((size_mb == 0) || 
		((chip->data_size > 512) && (size_mb > 1024))) {

		/* Either 1K, 2K, 4K, or 8K */
		chip->data_size = 0x1 << (10 + (id[3] & 0x03));

		/* Calculates BlockSize/PageSize = pagesPerBlock */
		chip->pgs_per_blk = 0x1 << (6 + ((id[3] >> 4) & 0x3) - (id[3] & 0x3));

		/* Number of 512 byte blocks per page * spare bytes per 512 block */
		n = 0x1 << (3 + ((id[3] >> 2) & 0x1));	

		/* Spare bytes per 512 bytes */
		chip->spare_size = (chip->data_size >> 9) * n;

		/* If the device ID is not in the table then assume a value for the */
		/* total number of blocks. */
		chip->num_blocks = 8192;
	}

	/* FIXME: this value will depend on the ECC hardware capabilities,
	 we assumming 10 bytes ECC */
	chip->ecc_offs = chip->spare_size - 10;

	return 0;
}

