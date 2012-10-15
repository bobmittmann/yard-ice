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

#include "target.h"
#include "armice.h"
#include "nand.h"
#include "target/davinci.h"

#include <string.h>

#include <sys/dcclog.h>

static nand_chip_t chip;
static nand_dev_t nand;
static uint32_t cache[128];
static int32_t cache_pg;
static int8_t cache_wp = 0;


int nand_mem_write(armice_ctrl_t * ctrl, uint32_t base, 
					  uint32_t offs, const void * ptr, int len)
{
	uint8_t * cp = (uint8_t *)cache;
	int pos;
	int ret;
	int pg;
	int end;
	int n;

	pg = offs / chip.data_size;
	end = (offs + len) / chip.data_size;

	if ((pg != cache_pg) && (cache_wp)) {
		/* write back */
		cache_wp = 0;
		if ((ret = nand_page_write(&nand, cache_pg, cache)) < 0) {
			cache_pg = -1;
			return ret;
		}
	}

	pos = offs % chip.data_size;
	n = chip.data_size - pos;
	n = (len < n) ? len : n;
	memcpy(&cp[pos], ptr, n);

	if (end != pg) {
		/* write thrugh */
		cache_wp = 0;
		if ((ret = nand_page_write(&nand, pg, cache)) < 0) {
			cache_pg = -1;
			return ret;
		}
	} else
		cache_wp = 1;

	cache_pg = pg;

	return n;
}

int nand_mem_read(armice_ctrl_t * ctrl, uint32_t base, 
					  uint32_t offs, void * ptr, int len)
{
	uint8_t * cp = (uint8_t *)cache;
	int pos;
	int ret;
	int pg;
	int n;

	pg = offs / chip.data_size;

	if (pg != cache_pg) {
		if (cache_wp) {
			/* write back */
			cache_wp = 0;
			if ((ret = nand_page_write(&nand, cache_pg, cache)) < 0) {
				cache_pg = -1;
				return ret;
			}
		}

		if ((ret = nand_page_read(&nand, pg, cache)) < 0) {
			cache_pg = -1;
			return ret;
		}
		cache_pg = pg;
	}

	pos = offs % chip.data_size;
	n = chip.data_size - pos;
	n = (len < n) ? len : n;

	memcpy(ptr, &cp[pos], n);

	return n;
}

int nand_mem_erase(armice_ctrl_t * ctrl, uint32_t base, 
					  uint32_t offs, int len)
{
	int blk;
	int end;
	int blk_size;;

	cache[0] = 0;

	cache_pg = -1;

	blk_size = (chip.data_size * chip.pgs_per_blk);
	blk = (offs / blk_size);
	end = (offs + len - 1) / blk_size;

	while (blk <= end) {
		if (nand_block_erase(&nand, blk) < 0)
			return -1;
		blk++;
	}

	return len;
}

nand_dev_t * dm365_nand_mem_init(const ice_drv_t * ice)
{
	/* Chip configuration */
	if (nand_chip_init(ice, &chip, &dm365_nand_io8, DM365_CE0_BASE) < 0)
		return NULL;

	if (nand_detect(ice, &chip, &dm365_nand_io8) < 0)
		return NULL;

	/* NAND operations */

	if (nand_open(&nand, ice, &chip, &dm365_nand_io8, 
						 &dm365_nand_ecc, &dm365_nand_bb) < 0) {
		return NULL;
	}



	cache_pg = -1;

	return &nand;
}

const struct ice_mem_oper nand_mem_oper = {
	.read = (ice_mem_read_t)nand_mem_read,
	.write = (ice_mem_write_t)nand_mem_write, 
	.erase = (ice_mem_erase_t)nand_mem_erase
};

