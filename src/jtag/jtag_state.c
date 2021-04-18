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
 * @file jtag_state.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#ifdef CONFIG_H
#include "config.h"
#endif

#include <jtag.h>


/***************************************************************************
 *
 * JTAG - States
 *
 ***************************************************************************/

const char * const jtag_state_name[] =
{
	"DREXIT2",
	"DREXIT1",
	"DRSHIFT",
	"DRPAUSE",
	"IRSELECT",
	"DRUPDATE",
	"DRCAPTURE",
	"DRSELECT",
	"IREXIT2",
	"IREXIT1",
	"IRSHIFT",
	"IRPAUSE",
	"RUN/IDLE",
	"IRUPDATE",
	"IRCAPTURE",
	"RESET"
};


