/* 
 * File:	ice-trace_init.c
 * Author:  Robinson Mittmann (bobmittmann@gmail.com)
 * Target: 
 * Comment: 
 * Copyright(C) 2012 Bob Mittmann. All Rights Reserved.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <sys/param.h> /* MIN(), MAX() macros */
#include <stdlib.h>
#include <sys/ice-comm.h>
#include <sys/dcclog.h>
#include <arch/cortex-m3.h>

void ice_trace_init(void)
{
	static const char __f[] 
		__attribute__ ((section(".dccdata"))) = __FILE__;
	static const char _m[] 
		__attribute__ ((section(".dccdata"))) = "starting DCC LOG...\000";
	static const struct dcc_trace_entry 
		__attribute__((section(".dcclog"))) 
		log_entry = { 
			__f, 
			__LINE__, 
			LOG_EXCEPT, 
			0, 
			__FUNCTION__,
			_m
		};

	ice_comm_sync();
	ice_comm_w32((uint32_t)&log_entry);
}

void dcclog_init(const struct dcc_trace_entry * __entry) 
	__attribute__ ((weak, alias ("ice_trace_init")));
