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

#include "board.h"
#include "simlnk.h"

#define RPC_SRC_DST_SWAP(OPC) (((OPC) & 0xffff0000) | \
							   (((OPC) & 0x0000ff00) >> 8) | \
							   (((OPC) & 0x00000000) << 8))

#define RPC_RESPONSE_BIT (1 << 31)

int simrpc_send(uint32_t opc, void * data, unsigned int cnt);

void simrpc_mem_lock_svc(uint32_t opc, uint32_t * data, unsigned int cnt)
{
	uint32_t base;
	uint32_t size;

	if (cnt != 8) {
		DCC_LOG(LOG_WARNING, "Invalid argument size");
	};

	base = data[0];
	size = data[1];

	DCC_LOG2(LOG_TRACE, "base=%08x size-%d", base, size);

	opc = RPC_SRC_DST_SWAP(opc) | RPC_RESPONSE_BIT;

	simrpc_send(opc, NULL, 0);
}

void simrpc_mem_unlock_svc(uint32_t opc, uint32_t * data, unsigned int cnt)
{
	uint32_t base;
	uint32_t size;

	if (cnt != 8) {
		DCC_LOG(LOG_WARNING, "Invalid argument size");
	};

	base = data[0];
	size = data[1];

	DCC_LOG2(LOG_TRACE, "base=%08x size-%d", base, size);

	opc = RPC_SRC_DST_SWAP(opc) | RPC_RESPONSE_BIT;

	simrpc_send(opc, NULL, 0);
}

void simrpc_mem_erase_svc(uint32_t opc, uint32_t * data, unsigned int cnt)
{
	uint32_t base;
	uint32_t size;

	if (cnt != 8) {
		DCC_LOG(LOG_WARNING, "Invalid argument size");
	};

	base = data[0];
	size = data[1];

	DCC_LOG2(LOG_TRACE, "base=%08x size-%d", base, size);

	opc = RPC_SRC_DST_SWAP(opc) | RPC_RESPONSE_BIT;

	simrpc_send(opc, NULL, 0);
}

void simrpc_mem_read_svc(uint32_t opc, uint32_t * data, unsigned int cnt)
{
	uint32_t base;
	uint32_t size;

	if (cnt != 8) {
		DCC_LOG(LOG_WARNING, "Invalid argument size");
	};

	base = data[0];
	size = data[1];

	DCC_LOG2(LOG_TRACE, "base=%08x size-%d", base, size);

	opc = RPC_SRC_DST_SWAP(opc) | RPC_RESPONSE_BIT;

	simrpc_send(opc, NULL, 0);
}

void simrpc_mem_write_svc(uint32_t opc, uint32_t * data, unsigned int cnt)
{
	uint32_t base;
	uint32_t size;

	if (cnt != 8) {
		DCC_LOG(LOG_WARNING, "Invalid argument size");
	};

	base = data[0];
	size = data[1];

	DCC_LOG2(LOG_TRACE, "base=%08x size-%d", base, size);

	opc = RPC_SRC_DST_SWAP(opc) | RPC_RESPONSE_BIT;

	simrpc_send(opc, NULL, 0);
}

void simrpc_mem_seek_svc(uint32_t opc, uint32_t * data, unsigned int cnt)
{
	uint32_t base;
	uint32_t size;

	if (cnt != 8) {
		DCC_LOG(LOG_WARNING, "Invalid argument size");
	};

	base = data[0];
	size = data[1];

	DCC_LOG2(LOG_TRACE, "base=%08x size-%d", base, size);

	opc = RPC_SRC_DST_SWAP(opc) | RPC_RESPONSE_BIT;

	simrpc_send(opc, NULL, 0);
}

