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
 * @file jtag_id_code.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#include <stdint.h>
#include <stdio.h>
#include <jtag.h>

void jtag_show_id_code(FILE * f, uint32_t id)
{
	int version;
	int part_number;
	int manufacturer;
	int marker;

	version = JTAG_IDCODE_VERSION(id);
	part_number = JTAG_IDCODE_PART_NUMBER(id);
	manufacturer = JTAG_IDCODE_MANUFACTURER(id);
	marker = JTAG_IDCODE_MARKER(id);

	fprintf(f, "ver: %d pn: 0x%04x vendor: 0x%03x marker: %d", 
			version, part_number, manufacturer, marker);
}

