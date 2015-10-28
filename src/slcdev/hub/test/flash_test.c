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
 * @file hello.c
 * @brief application test
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <sys/stm32f.h>
#include <sys/console.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <hexdump.h>
#include <thinkos.h>
#include <trace.h>

#include "board.h"
#include "sflash.h"

unsigned int micron_sf_capacity_mbits(unsigned int id)
{
	switch (id) {
	case 0x19:
		return 256;
	case 0x20:
		return 512;
	}

	return 0;
}

/* Serial flash: N25Q512A13GF840E
 *
 * Manufacturer: 0x20 (Micron)
 * Memory Type: 0xBA
 * Memory Capacity: 0x20 (512MiB)
 *  */

#define JEDEC_MFID_MICRON 0x20

bool serial_flash_probe(struct sflash_dev * sf)
{
	struct sflash_inf inf;

	if (sflash_probe(sf, &inf) < 0) {
		printf("ERR: sflash_device_id() Failed!\n");
		return false;
	}

	printf("Manufacturer: 0x%02x\n", inf.manufacturer);
	printf("Type: 0x%02x%02x\n", inf.device_type, inf.capacity);
	printf("Capacity: %dMb\n", micron_sf_capacity_mbits(inf.capacity));

	if (inf.manufacturer != JEDEC_MFID_MICRON)
		return false;

	if (inf.device_type != 0xba)
		return false;

	if ((inf.capacity != 0x19) && (inf.capacity != 0x20)) {
		return false;
	}

	return true;
}

/* ------------------------------------------------------------------------
   Serial Flash Low Level API test
   ------------------------------------------------------------------------- */

#define BLOCK_SIZE 4096
#define PAGE_SIZE 256

bool sflash_dump(struct sflash_dev * sf, uint32_t addr, unsigned int len)
{
	uint8_t page[PAGE_SIZE];
	int j;

	for (j = 0; j < len / PAGE_SIZE; ++j) {
		memset(page, 0, sizeof(page));
		sflash_page_read(sf, addr, page, sizeof(page));
		show_hex8(stdout, addr, page, sizeof(page));
		addr += PAGE_SIZE;
	}

	return true;
}

bool block_erase(struct sflash_dev * sf, uint32_t addr)
{
	uint32_t ts[2];
	uint32_t dt;

	ts[0] = trace_timestamp();
	sflash_subsector_erase(sf, addr);
	ts[1] = trace_timestamp();
	dt = trace_ts2us(ts[1] - ts[0]);
	printf("0x%08x Sub-sector erase: %d us\n", addr, dt);

	return true;
}

bool block_erase_verify(struct sflash_dev * sf, uint32_t addr)
{
	uint8_t page[PAGE_SIZE];
	int i;
	int j;

	for (j = 0; j < BLOCK_SIZE / PAGE_SIZE; ++j) {
		memset(page, 0, sizeof(page));
		sflash_page_read(sf, addr, page, sizeof(page));
		for (i = 0; i < sizeof(page); ++i) {
			if (page[i] != 0xff) {
				printf("Erase check failed at address 0x%08x\n", addr + i);
				show_hex8(stdout, addr, page, sizeof(page));
				return false;
			}
		}
		addr += PAGE_SIZE;
	}

	return true;
}

bool block_write(struct sflash_dev * sf, uint32_t addr)
{
	uint8_t page[PAGE_SIZE];
	int i;
	int j;

	for (j = 0; j < BLOCK_SIZE / PAGE_SIZE; ++j) {
		uint32_t ts[2];
		uint32_t dt;

		for(i = 0; i < sizeof(page); i += 2) {
			unsigned int val = (addr + i) & 0xffff;
			page[i] = val >> 8;
			page[i + 1] = val;
		}

		ts[0] = trace_timestamp();
		sflash_page_write(sf, addr, page, sizeof(page));
		ts[1] = trace_timestamp();
		dt = trace_ts2us(ts[1] - ts[0]);
		printf("0x%08x Page write: %d us\n", addr, dt);
		addr += PAGE_SIZE;
	}

	return true;
}

bool block_write_verify(struct sflash_dev * sf, uint32_t addr)
{
	uint8_t page[PAGE_SIZE];
	int i;
	int j;

	for (j = 0; j < BLOCK_SIZE / PAGE_SIZE; ++j) {
		memset(page, 0, sizeof(page));
		sflash_page_read(sf, addr, page, sizeof(page));

		for(i = 0; i < sizeof(page); i += 2) {
			unsigned int val = (addr + i) & 0xffff;
			unsigned int cmp = (page[i] << 8) | page[i + 1];
			if (val != cmp) {
				printf("Write check failed at address 0x%08x\n", addr + i);
				show_hex8(stdout, addr, page, sizeof(page));
				return false;
			}
		}
		addr += PAGE_SIZE;
	}

	return true;
}


void low_level_test(struct sflash_dev * sf)
{
	uint32_t addr = 0;

	for (addr = 0; addr < 512 * 1024 * 1024; addr += BLOCK_SIZE) {
		block_erase(sf, addr);
		if (!block_erase_verify(sf, addr)) {
			break;
		}

		block_write(sf, addr);
		if (!block_write_verify(sf, addr)) {
			break;
		}

//		thinkos_sleep(2000);
	};
}


/* ------------------------------------------------------------------------
   Serial Flash High Level API test
   ------------------------------------------------------------------------- */

void high_level_test(struct sflash_dev * sf)
{
	unsigned int partition_start = 8192;
	unsigned int partition_size = 128 * 1024;
	unsigned int i = 0;

	printf("Erasing partition: %d bytes\n", partition_size);
	sflash_erase(sf, 0, partition_size);

	printf("Writing: ");
	/* Move the pointer to the beginning of the partition */
	sflash_seek(sf, partition_start);
	/* Write sequentially into flash */
	for (i = 0; i < (partition_size / sizeof(uint32_t)); ++i) {
		uint32_t buf[1];

		if ((i % 1024) == 0)
			printf(".");

		buf[0] = i;
		sflash_write(sf, buf, sizeof(uint32_t));

	};

	printf("\nVerifying: ");

	/* Move the pointer to the beginning of the partition */
	sflash_seek(sf, partition_start);
	/* Read from flash and compare */
	for (i = 0; i < (partition_size / sizeof(uint32_t)); ++i) {
		uint32_t buf[1];

		if ((i % 1024) == 0)
			printf(".");

		sflash_read(sf, buf, sizeof(uint32_t));

		if (buf[0] != i) {
			printf("\nWrite check failed at address 0x%08x\n",
					i * sizeof(uint32_t));
			thinkos_sleep(10000);
			break;
		}

	};

	printf("\nDone.\n");

//	sflash_dump(sf, partition_start, partition_size);
}


void flash_test(void)
{
	struct sflash_dev * sf;

	sf = sflash_init();

	while (!serial_flash_probe(sf)) {
		printf("Serial flash error!\n");
		thinkos_sleep(500);
	}

	high_level_test(sf);

	thinkos_sleep(500);

	low_level_test(sf);
}
