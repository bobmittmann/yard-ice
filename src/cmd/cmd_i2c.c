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
 * @file cmd_i2c.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "target.h"
#include "debugger.h"
#include "i2c.h"
#include "eval.h"

static int __i2c_reset(FILE * f)
{
	fprintf(f, " - I2C reset...\n");

	i2c_reset();

	return 0;
}

static int i2c_info(FILE * f)
{
	fprintf(f, " - I2C info...\n");

	return 0;
}

#define I2C_ADC_MAX 8
uint8_t i2c_adc_lst[I2C_ADC_MAX];
uint8_t i2c_adc_cnt;

static void adc_lst_purge(void)
{
	i2c_adc_cnt = 0;
}

static void adc_lst_insert(unsigned int addr)
{
	if (i2c_adc_cnt < I2C_ADC_MAX) {
		i2c_adc_lst[i2c_adc_cnt++] = addr;
	}
}

static int i2c_adc_scan(FILE * f)
{
	uint8_t buf[4];
	unsigned int val;
	int i;

	for (i = 0; i < i2c_adc_cnt; ++i) {
		i2c_read(i2c_adc_lst[i], buf, 2);
		val = buf[0] + (buf[1] << 8);
		fprintf(f, "%6d", val);
	}

	fprintf(f, "\n");
	return 0;
}

static int i2c_probe(FILE * f)
{
	uint8_t addr[0x78];
	int n;
	int i;

	n = i2c_bus_scan(0x1, 0x78, addr, sizeof(addr));
	adc_lst_purge();
	
	if (n == 0) {
		fprintf(f, "No devices.\n");
		return 0;
	}

	for (i = 0; i < n; ++i) {
		fprintf(f, "%2d 0x%02x - ", i, addr[i]);
		if ((addr[i] & 0x48) == 0x48) {
			fprintf(f, " MCP3221-%d", addr[i] & 0x7);
			adc_lst_insert(addr[i]);
		} else {
			fprintf(f, "Unknown");
		}
		fprintf(f, "\n");
	}

	fprintf(f, " \n");

	return 0;
}


int cmd_i2c(FILE * f, int argc, char ** argv)
{
	if (argc == 1)
		return i2c_info(f);

	argv++;
	if (argc == 2) {
		if ((strcmp(*argv, "reset") == 0) || (strcmp(*argv, "rst") == 0))
			return __i2c_reset(f);

		if ((strcmp(*argv, "probe") == 0) || (strcmp(*argv, "det") == 0))
			return i2c_probe(f);

		if ((strcmp(*argv, "adc") == 0) || (strcmp(*argv, "scan") == 0))
			return i2c_adc_scan(f);

		if (strcmp(*argv, "info") == 0)
			return i2c_info(f);
	}

#if 0
	if (argc > 2) {
		if ((strcmp(*argv, "select") == 0) || (strcmp(*argv, "sel") == 0))
			return i2c_select(f, argc, argv);
	}

	if (argc > 2) {
//		printf(msg_reset_usage);
		return -1;
	}
#endif

	return -1;
}

