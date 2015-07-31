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

			for (i = 0; i < n; i += 2)
				cp += sprintf(cp, " %02x%02x", src[i], src[i + 1]);

			printf("%s\n", buf);
		}

		cmp = src;
		addr += n;
		rem -= n;
		src += n;
	}
}

void serial_flash_test(struct sflash_dev * sf, uint32_t addr)
{
	uint8_t id[20];
	uint8_t page[256];
	int cnt;
	int sr;
	int n;
	int i;

	if ((n = sflash_device_id(sf, id, sizeof(id))) < 0) {
		printf("ERR: sflash_device_id() Failed!\n");
		return;
	}

	printf("Manufacturer: 0x%02x\n", id[0]);
	printf("Device Id: 0x%02x%02x\n", id[1], id[2]);
	printf("Capacity: %dMb\n", micron_sf_capacity_mbits(id[2]));

	sflash_page_read(sf, addr, page, sizeof(page));
	hexdump(addr, page, sizeof(page));

#if 0
	sflash_write_enable(sf);
	sflash_sector_erase(sf, addr);
	cnt = 0;
	do {
		sr = sflash_flag_status_read(sf);
		cnt++;
		/* wait for ready */
	} while ((sr & 0x80) == 0);

	printf("Sector erase: %d\n", cnt);

	memset(page, 0, sizeof(page));
	sflash_page_read(sf, addr, page, sizeof(page));
	hexdump(addr, page, sizeof(page));
#endif

	for(i = 0; i < sizeof(page); i += 2) {
		int val = addr + i;
		page[i] = val >> 8;
		page[i + 1] = val;
	}

	sflash_write_enable(sf);
	sflash_page_write(sf, addr, page, sizeof(page));
	cnt = 0;
	do {
		sr = sflash_flag_status_read(sf);
		cnt++;
		/* wait for ready */
	} while ((sr & 0x80) == 0);

	printf("Page write: %d\n", cnt);

	memset(page, 0, sizeof(page));

	sflash_page_read(sf, addr, page, sizeof(page));
	hexdump(addr, page, sizeof(page));
}

int main(int argc, char ** argv)
{
	struct lcd_dev * lcd;
	struct sflash_dev * sf;
	uint32_t addr = 0;
//	int rate = RATE_OFF;
//	int c;

//	io_init();
	stdio_init();

	lcd = lcd20x4_init();
	lcd_at_puts(lcd, 0, 0, "= ThinkOS Firelink =");
	lcd_at_puts(lcd, 1, 0, " Zigbee Coordinator ");
	lcd_at_puts(lcd, 2, 0, " Serial Flash Test ");
	lcd_at_puts(lcd, 3, 0, "====================");

	sf = sflash_init();

	sflash_reset(sf);
	sflash_flag_status_read(sf);

	for (;;) {
		serial_flash_test(sf, addr);
		thinkos_sleep(2000);
		addr += 256;
	};

	return 0;
}

