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

#include <thinkos.h>
#include <sys/serial.h>
#include <sys/param.h>
#include <string.h>
#include <time.h>

#include "board.h"
#include "simlnk.h"
#include "simrpc.h"
#include "simrpc_svc.h"
#include "slcdev.h"

void simrpc_attr_get_svc(uint32_t hdr, uint32_t * data, unsigned int len)
{
	struct simrpc_attr_req req;
	uint32_t val[128];
	unsigned int addr;
	unsigned int cnt;
	unsigned int idx;
	unsigned int i;
	unsigned int off;
	bool mod;

	if (len != 4) {
		DCC_LOG1(LOG_WARNING, "Invalid packet size, len=%d", len);
		return;
	};

	req = *(struct simrpc_attr_req *)data;

	cnt = req.count;
	addr = req.base_addr;
	mod = (req.dev_type == 1);
	off = req.attr_offs;

	if (cnt > 128)
		cnt = 128;

	if (addr > 159)
		addr = 159;

	if ((addr + cnt) > 159)
		cnt = 160 - addr;

	if (off > sizeof(struct ss_device) - sizeof(uint32_t)) {
		simrpc_send_int(SIMRPC_REPLY_ERR(hdr), SIMRPC_EINVAL);
		return;
	}

	idx = addr + (mod ? 160 : 0);

	for (i = 0; i < cnt; ++i) {
		struct ss_device * dev;
		uint32_t * attr;

		dev = &ss_dev_tab[idx + i];
		attr = (uint32_t *)dev;
		val[i] = attr[off];
	}

	simrpc_send(SIMRPC_REPLY_OK(hdr), val, cnt * sizeof(uint32_t));
}

void simrpc_stats_get_svc(uint32_t hdr, uint32_t * data, unsigned int cnt)
{
	struct simrpc_stats stats;

	stats.uptime = time(NULL);
	stats.db_ok = device_db_is_sane();
	stats.cfg_ok = config_is_sane();
	slcdev_stats_get((struct slcdev_stats *)&stats.dev);

	simrpc_send(SIMRPC_REPLY_OK(hdr), &stats, sizeof(stats));
}

