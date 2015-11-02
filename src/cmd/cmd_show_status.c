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


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "target.h"
#include "debugger.h"

#include <debug.h>

int cmd_show_status(FILE * f, int argc, char ** argv)
{
	char tbit;
	char nmreq;
	char ifen;
	char dbgrq;
	char dbgack;
	int status;
/*	

	if (dbg->state != ST_HALTED) {
		fprintf(f, msg_not_debugging_err);
		return -1;
	} */

/*
 * FIXME: driver private: arm7ice_dbg_status
 */

	status = 0;
/*
	if ((status = arm7ice_get_status()) < 0) {
		fprintf(f, "#ICE error: %s\n", target_strerror(dbg));
		return -1;
	}
*/
	tbit = ((status >> 4) & 0x01) + '0';
	nmreq = ((status >> 3) & 0x01) + '0';
	ifen = ((status >> 2) & 0x01) + '0';
	dbgrq = ((status >> 1) & 0x01) + '0';
	dbgack = ((status >> 0) & 0x01) + '0';

	fprintf(f, "TBIT %c, nMREQ %c, IFEN %c, DBGRQ %c, DBGACK %c\n",
		tbit, nmreq, ifen, dbgrq, dbgack);

//	if (status & ARM7ICE_DBGACK)
//		show_cpsr(dbg->ct->cpsr);

//	show_mem_info(dbg);

	return 0;
}

