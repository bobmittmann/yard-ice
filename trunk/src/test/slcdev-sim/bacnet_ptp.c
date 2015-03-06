/* 
 * File:	 usb-test.c
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(C) 2011 Bob Mittmann. All Rights Reserved.
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <sys/shell.h>
#include <sys/tty.h>
#include <sys/param.h>
#include <thinkos.h>

#include <sys/dcclog.h>
#include <sys/delay.h>

#include "bacnet_ptp.h"

/* -------------------------------------------------------------------------
 * Help
 * ------------------------------------------------------------------------- */

int bacnet_ptp(struct serdrv * dev)
{
	uint8_t buf[32];
	int i;
	int n;

	DCC_LOG(LOG_TRACE, "Starting BACnet PtP Data Link");

	while ((n = serdrv_recv(dev, buf, 32, 5000)) > 0) {
		for (i = 0; i < n; ++i) {
			DCC_LOG1(LOG_TRACE, "%02x", buf[i]);
		}
	}

	return 0;
}

