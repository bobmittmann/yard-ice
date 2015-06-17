/* 
 * thikos_sysinfo.c
 *
 * Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
 * 
 * This file is part of the ThinkOS library.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without flagen the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You can receive a copy of the GNU Lesser General Public License from 
 * http://www.gnu.org/
 */

_Pragma ("GCC optimize (\"Os\")")

#define __THINKOS_SYS__
#include <thinkos_sys.h>
#include <thinkos.h>

#include <sys/sysclk.h>

#if THINKOS_ENABLE_SYSINFO

extern int32_t udelay_factor;

void thinkos_sysinfo_svc(int32_t * arg)
{
	unsigned int req = arg[0];
	int32_t * pval;
	const uint32_t ** ptr;
	
	switch (req) {
	case SYSINFO_CLOCKS:
		arg[0] = 0;
		ptr = (const uint32_t **)arg[1];
		*ptr = sysclk_hz;
		break;

	case SYSINFO_UDELAY_FACTOR:
		arg[0] = 0;
		pval = (int32_t *)arg[1];
		*pval = udelay_factor;
		break;

	default:
		DCC_LOG1(LOG_ERROR, "invalid sysinfo request %d!", req);
		arg[0] = THINKOS_EINVAL;
		break;
	}
}

#endif /* THINKOS_ENABLE_SYSINFO */

