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


#ifdef CONFIG_H
#include "config.h"
#endif

#include <jtag.h>

const char * const jtag_errtab[] = {
	"JTAG OK",
	"JTAG undefined error",
	"JTAG parameter invalid",
	"JTAG hardware failude",
	"JTAG invalid TAP",
	"JTAG IR scan fail",
	"JTAG too many TAPs",
	"JTAG ARM DR_SCAN_N fail",
	"JTAG ARM sync timeout",
	"JTAG ARM DCC timeout",
	"JTAG ARM POLL_STOP ???"
};

char * jtag_strerror(int errno)
{
	errno += 200;

	if (errno < -10)
		errno = -1;

	return (char *)jtag_errtab[-errno];
}

