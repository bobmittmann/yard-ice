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
 * @file yard-ice.c
 * @brief YARD-ICE application main
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifdef CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <thinkos.h>
#define __THINKOS_SYS__
#include <thinkos_sys.h>
#include <sys/dcclog.h>

#include "board.h"
#include "simlnk.h"
#include "crc32.h"
#include "simrpc_svc.h"

/* FIXME: this is a mark to keep the symbol __heap_end in 
   the symbols listing. */
extern int __heap_base;
const void * heap_base = &__heap_base; 
uint32_t except_crc __attribute__((section(".heap")));

void app_default(int mode);
int __simrpc_send(uint32_t opc, const void * data, unsigned int cnt);

void thinkos_exception_dsr(struct thinkos_except * xcpt)
{
	except_crc = crc32_align((void *)xcpt, sizeof(struct thinkos_except));
#ifdef DEBUG
	__tdump();
#endif
	DCC_LOG2(LOG_TRACE, "except=%d crc=%08x.", xcpt->type, except_crc);
}

void cm3_debug_mon_isr(void)
{
	struct simrpc_mgmt mgmt;
	struct thinkos_except * xcpt;
	bool fault = false;
	uint32_t crc;
	uint32_t opc;
	int mode;
	
	xcpt = __thinkos_except_buf();
	if (xcpt->type != 0) {
		DCC_LOG(LOG_TRACE, "check for exception.");
		crc = crc32_align((void *)xcpt, sizeof(struct thinkos_except));
		if (crc == except_crc) {
			DCC_LOG(LOG_TRACE, "valid exception in the buffer.");
			except_crc = 0;
			fault = true;
		} else {
			__thinkos_memset32((void *)xcpt, 0, sizeof(struct thinkos_except));
		}
	} 

	if (fault) {
		mode = APP_MODE_FAULT;
		mgmt.typ = SIMRPC_MGMT_FAULT;
	} else {
		mode = APP_MODE_NORMAL;
		mgmt.typ = SIMRPC_MGMT_LINK_UP;
	}
	opc = simrpc_mkopc(SIMRPC_ADDR_LHUB, SIMRPC_ADDR_ANY, 
					   0, SIMRPC_MGMT_SIGNAL);
	__simrpc_send(opc, &mgmt, sizeof(struct simrpc_mgmt));

	DCC_LOG1(LOG_TRACE, "board_app_exec(%d).", mode);
	if (fault || !board_app_exec(THINKOS_APP_ADDR, mode)) {
		DCC_LOG1(LOG_TRACE, "board_exec(%d).", mode);
		board_exec(app_default, mode);
	}	
}

void app_exec(void)
{
	struct cm3_dcb * dcb = CM3_DCB;

	DCC_LOG(LOG_TRACE, "debug/monitor");

	/* enable monitor and send the reset event */
	dcb->demcr |= DCB_DEMCR_MON_EN | DCB_DEMCR_MON_PEND;
	asm volatile ("isb\n" :  :  : );
}

int main(int argc, char ** argv)
{
	DCC_LOG_INIT();
	DCC_LOG_CONNECT();

#ifdef DEBUG
	DCC_LOG(LOG_TRACE, "0. cm3_udelay_calibrate().");
	cm3_udelay_calibrate();
#endif

	DCC_LOG(LOG_TRACE, "1. board_init().");
	board_init();

	DCC_LOG(LOG_TRACE, "2. thinkos_init().");
	thinkos_init(THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(0));

	DCC_LOG(LOG_TRACE, "3. simlnk_init().");
	simlnk_init(NULL, "", 0, NULL);

	DCC_LOG(LOG_TRACE, "4. app_exec().");
	app_exec();

	DCC_LOG(LOG_ERROR, "5. error!!!");

	return 0;
}

