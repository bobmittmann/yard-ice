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

#include <thinkos.h>

#include "board.h"
#include "lcd20x4.h"
#include "fzc_io.h"
#include "sflash.h"

void stdio_init(void)
{
	FILE * f;

	f = console_fopen();
	/* initialize STDIO */
	stderr = f;
	stdout = f;
	stdin = f;
}

void hexdump(uint32_t addr, void * data, unsigned int size)
{
	uint8_t * cmp = (uint8_t *)-1;
	uint8_t * src = (uint8_t *)data;
	unsigned int rem = size;
	char buf[80];
	bool eq = false;

	while (rem) {
		int n = rem < 16 ? rem : 16;
		char * cp = buf;
		unsigned int i;
	
		if (cmp != (uint8_t *)-1) {
			for (i = 0; i < n; ++i) {
				if (src[i] != cmp[i]) {
					eq = false;
					goto dump_line;
				}
			}

			if (!eq) {
				printf(" ...\n");
				eq = true;
			}
		} else {	

dump_line:
			cp += sprintf(cp, "%06x:", addr);

//			for (i = 0; i < n; i += 2)
//				cp += sprintf(cp, " %02x%02x", src[i], src[i + 1]);
			for (i = 0; i < n; ++i)
				cp += sprintf(cp, " %02x", src[i]);

			printf("%s\n", buf);
		}

		cmp = src;
		addr += n;
		rem -= n;
		src += n;
	}
}

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
	struct sflash_id id;

	if (sflash_probe(sf, &id) < 0) {
		printf("ERR: sflash_device_id() Failed!\n");
		return false;
	}

	printf("Manufacturer: 0x%02x\n", id.manufacturer);
	printf("Device Id: 0x%02x%02x\n", id.device_type, id.capacity);
	printf("Capacity: %dMb\n", micron_sf_capacity_mbits(id.capacity));

	if (id.manufacturer != JEDEC_MFID_MICRON)
		return false;

	if (id.device_type != 0xba)
		return false;

	if (id.capacity != 0x20)
		return false;

	return true;
}

#define BLOCK_SIZE 4096
#define PAGE_SIZE 256

bool block_erase(struct sflash_dev * sf, uint32_t addr)
{
	sflash_subsector_erase(sf, addr);
	printf("0x%08x Sub-sector erase\n", addr);

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
				hexdump(addr, page, sizeof(page));
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
		for(i = 0; i < sizeof(page); i += 2) {
			unsigned int val = (addr + i) & 0xffff;
			page[i] = val >> 8;
			page[i + 1] = val;
		}

		sflash_page_write(sf, addr, page, sizeof(page));
		printf("0x%08x Page write\n", addr);
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
				hexdump(addr, page, sizeof(page));
				return false;
			}
		}
		addr += PAGE_SIZE;
	}

	return true;
}

bool block_dump(struct sflash_dev * sf, uint32_t addr)
{
	uint8_t page[PAGE_SIZE];
	int j;

	for (j = 0; j < BLOCK_SIZE / PAGE_SIZE; ++j) {
		memset(page, 0, sizeof(page));
		sflash_page_read(sf, addr, page, sizeof(page));
		hexdump(addr, page, sizeof(page));
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

int main(int argc, char ** argv)
{
	struct lcd_dev * lcd;
	struct sflash_dev * sf;

	stdio_init();

	lcd = lcd20x4_init();
	lcd_at_puts(lcd, 0, 0, "= ThinkOS Firelink =");
	lcd_at_puts(lcd, 1, 0, " Zigbee Coordinator ");
	lcd_at_puts(lcd, 2, 0, " Serial Flash Test ");
	lcd_at_puts(lcd, 3, 0, "====================");

	sf = sflash_init();

	if (!serial_flash_probe(sf)) {
		printf("Serial flash error!\n");
		thinkos_sleep(5000);
		return 1;
	}

	low_level_test(sf);

	thinkos_sleep(5000);

	return 0;
}
