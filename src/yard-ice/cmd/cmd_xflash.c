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
 * @file cmd_let.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/shell.h>
#include <sys/tty.h>
#include <sys/console.h>
#include <sys/serial.h>
#include <sys/stm32f.h>

#include "config.h"
#include "target.h"
#include "debugger.h"
#include "eval.h"

int usb_xflash(uint32_t offs, uint32_t len);

int usart_xflash(void * uart, uint32_t offs, uint32_t len);

extern const struct fileop uart_console_ops;

int cmd_xflash(FILE * f, int argc, char ** argv)
{
	uint32_t offs = 0x00000;
	uint32_t size = 0x00000;
	uint32_t pri;
	FILE * raw;
	int ret;

	if (argc < 2)
		return SHELL_ERR_ARG_MISSING;

	if (argc > 2)
		return SHELL_ERR_EXTRA_ARGS;

	do {
		if ((strcmp(argv[1], "firmware") == 0) || 
			(strcmp(argv[1], "firm") == 0) ||
			(strcmp(argv[1], "f") == 0)) {
			offs = 0;
			size = 256 * 1024;
			fprintf(f, "Firmware update...\n");
			break;
		} 

		return SHELL_ERR_ARG_INVALID;
	} while (0);

	fflush(f);

	raw = isfatty(f) ? ftty_lowlevel(f) : f;
 
	if (is_console_file(raw)) {
		pri = cm3_primask_get();
		cm3_primask_set(1);
		ret = usb_xflash(offs, size);
		cm3_primask_set(pri);
		return ret;
	} 

	if (is_serial(raw)) {
		pri = cm3_primask_get();
		cm3_primask_set(1);
		ret = usart_xflash(STM32_UART5, offs, size);
		cm3_primask_set(pri);

		return ret;
	}
		  
	fprintf(f, "Operation not permited in this terminal.\n");
	return -1;
}

