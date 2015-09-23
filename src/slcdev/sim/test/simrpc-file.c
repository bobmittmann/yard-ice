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

#include "board.h"
#include "simlnk.h"
#include "simrpc.h"
#include "flashfs.h"
#include "simrpc_svc.h"


#define SIMRPC_UNLOCK SIMRPC_BCAST

struct {
	uint32_t clk;
	uint16_t offs;
	uint16_t max;
	struct fs_dirent entry;
	uint8_t lock;
	uint8_t nsync;
} simrpc_file = {
	.clk = 0,
	.offs = 0,
	.max = 0,
	.lock = SIMRPC_UNLOCK,
	.nsync = false
};

#define FILE_LOCK_TIMEOUT_MS 1000

void simrpc_file_open_svc(uint32_t hdr, uint32_t * data, unsigned int cnt)
{
	struct fs_dirent entry;
	uint32_t clk;
	char * fname;

	fname = (char *)data;
	fname[cnt] = '\0';

	clk = thinkos_clock();

	if (simrpc_file.lock != SIMRPC_UNLOCK) {
		if ((int32_t)(clk - simrpc_file.clk) < 0) {
			DCC_LOG(LOG_WARNING, "locked");
			simrpc_send_int(SIMRPC_REPLY_ERR(hdr), SIMRPC_EBUSY);
			return;
		}	
		DCC_LOG(LOG_WARNING, "lock expired");
	}

	if (!fs_dirent_lookup(fname, &entry)) {
		DCC_LOG(LOG_WARNING, "file does not exist");
		simrpc_send_int(SIMRPC_REPLY_ERR(hdr), SIMRPC_ENOENT);
		return;
	}

	simrpc_send_opc(SIMRPC_REPLY_OK(hdr));

	simrpc_file.entry = entry;
	simrpc_file.max = entry.blk_size - sizeof(struct fs_file);
	simrpc_file.offs = entry.blk_offs + sizeof(struct fs_file);
	simrpc_file.lock = SIMRPC_SRC(hdr);
	simrpc_file.clk = clk + FILE_LOCK_TIMEOUT_MS;
	simrpc_file.nsync = false;
}

void simrpc_file_create_svc(uint32_t hdr, uint32_t * data, unsigned int cnt)
{
	struct fs_dirent entry;
	uint32_t clk;
	char * fname;

 	DCC_LOG1(LOG_TRACE, "cnt=%d", cnt);

	fname = (char *)data;
	fname[cnt] = '\0';

	clk = thinkos_clock();

	if (simrpc_file.lock != SIMRPC_UNLOCK) {
		if ((int32_t)(clk - simrpc_file.clk) < 0) {
			DCC_LOG(LOG_WARNING, "locked");
			simrpc_send_int(SIMRPC_REPLY_ERR(hdr), SIMRPC_EBUSY);
			return;
		}	
		DCC_LOG(LOG_WARNING, "lock expired");
	}

	if (!fs_dirent_lookup(fname, &entry)) {
		DCC_LOGSTR(LOG_WARNING, "file does not exist: \"%s\"", fname);
		simrpc_send_int(SIMRPC_REPLY_ERR(hdr), SIMRPC_ENOENT);
		return;
	}

	fs_file_unlink(&entry);

	simrpc_send_opc(SIMRPC_REPLY_OK(hdr));

	simrpc_file.entry = entry;
	simrpc_file.max = entry.blk_size - sizeof(struct fs_file);
	simrpc_file.offs = entry.blk_offs + sizeof(struct fs_file);
	simrpc_file.lock = SIMRPC_SRC(hdr);
	simrpc_file.clk = clk + FILE_LOCK_TIMEOUT_MS;
	simrpc_file.nsync = false;
}

void simrpc_file_unlink_svc(uint32_t hdr, uint32_t * data, unsigned int cnt)
{
	struct fs_dirent entry;
	uint32_t clk;
	char * fname;

	fname = (char *)data;
	fname[cnt] = '\0';

	clk = thinkos_clock();

	if (!fs_dirent_lookup(fname, &entry)) {
		DCC_LOG(LOG_WARNING, "file does not exist");
		simrpc_send_int(SIMRPC_REPLY_ERR(hdr), SIMRPC_ENOENT);
		return;
	}

	if (simrpc_file.lock != SIMRPC_UNLOCK) {
		if (entry.fp == simrpc_file.entry.fp) {
			if ((int32_t)(clk - simrpc_file.clk) < 0) {
				DCC_LOG(LOG_WARNING, "locked");
				simrpc_send_int(SIMRPC_REPLY_ERR(hdr), SIMRPC_EBUSY);
				return;
			}	
			simrpc_file.clk = clk;
			simrpc_file.lock = SIMRPC_UNLOCK;
			DCC_LOG(LOG_WARNING, "lock expired");
		}
	}

	fs_file_unlink(&entry);

	simrpc_send_opc(SIMRPC_REPLY_OK(hdr));
}

void simrpc_file_crc16_svc(uint32_t hdr, uint32_t * data, unsigned int cnt)
{
	struct fs_dirent entry;
	struct fs_file * fp;
	uint32_t clk;
	char * fname;

	fname = (char *)data;
	fname[cnt] = '\0';

	clk = thinkos_clock();

	if (!fs_dirent_lookup(fname, &entry)) {
		DCC_LOG(LOG_WARNING, "file does not exist");
		simrpc_send_int(SIMRPC_REPLY_ERR(hdr), SIMRPC_ENOENT);
		return;
	}

	if (simrpc_file.lock != SIMRPC_UNLOCK) {
		if (entry.fp == simrpc_file.entry.fp) {
			if ((int32_t)(clk - simrpc_file.clk) < 0) {
				DCC_LOG(LOG_WARNING, "locked");
				simrpc_send_int(SIMRPC_REPLY_ERR(hdr), SIMRPC_EBUSY);
				return;
			}	
			simrpc_file.clk = clk;
			simrpc_file.lock = SIMRPC_UNLOCK;
			DCC_LOG(LOG_WARNING, "lock expired");
		}
	}

	fp = entry.fp;

	simrpc_send_int(SIMRPC_REPLY_OK(hdr), fp->crc);
}

void simrpc_file_write_svc(uint32_t hdr, uint32_t * data, unsigned int cnt)
{
	unsigned int rem;
	unsigned int n;
	uint32_t clk;
	
	clk = thinkos_clock();
#if 0
	if ((simrpc_file.id != id) || (id == 0)) {
		simrpc_send_int(SIMRPC_REPLY_ERR(hdr), SIMRPC_EBADF);
		if ((simrpc_file.id == 0) && 
			((int32_t)(clk - simrpc_file.clk) >= 0)) {
			DCC_LOG(LOG_WARNING, "lock expired");
			simrpc_file.clk = clk;
			simrpc_file.id = 0;
		}	
		return;
	}
#endif

	if (simrpc_file.lock == SIMRPC_UNLOCK) {
		DCC_LOG(LOG_WARNING, "locked");
		simrpc_send_int(SIMRPC_REPLY_ERR(hdr), SIMRPC_EBUSY);
		return;
	}

	rem = simrpc_file.max - simrpc_file.offs;
	n = MIN(rem, cnt); 
	
	DCC_LOG2(LOG_INFO, "stm32_flash_write(offs=%06x size=%d)", 
			 simrpc_file.offs, n);

	stm32_flash_write(simrpc_file.offs, &data[1], n);

	simrpc_send_int(SIMRPC_REPLY_OK(hdr), n);

	simrpc_file.nsync = true;
	simrpc_file.offs += n;
	simrpc_file.clk = clk + FILE_LOCK_TIMEOUT_MS;
}

void simrpc_file_read_svc(uint32_t hdr, uint32_t * data, unsigned int cnt)
{
	struct fs_file * fp;
	unsigned int offs;
	unsigned int rem;
	unsigned int len;
	unsigned int n;
	uint32_t clk;

	if (cnt != 4) {
		DCC_LOG1(LOG_WARNING, "Invalid packet size, cnt=%d", cnt);
		return;
	};

	len = data[0];
	clk = thinkos_clock();

	if (simrpc_file.lock == SIMRPC_UNLOCK) {
		DCC_LOG(LOG_WARNING, "locked");
		simrpc_send_int(SIMRPC_REPLY_ERR(hdr), SIMRPC_EBUSY);
		return;
	}

	fp = simrpc_file.entry.fp;
	offs = simrpc_file.offs;

	rem = fp->size - offs;
	n = MIN(rem, len);
	n = MIN(n, (SIMLNK_MTU - 4));

	simrpc_send(SIMRPC_REPLY_OK(hdr), (void *)&fp->data[offs], n);

	simrpc_file.offs = offs + n;
	simrpc_file.clk = clk + FILE_LOCK_TIMEOUT_MS;
}


void simrpc_file_close_svc(uint32_t hdr, uint32_t * data, unsigned int cnt)
{
	uint32_t clk;

	clk = thinkos_clock();

	if (simrpc_file.lock == SIMRPC_UNLOCK) {
		DCC_LOG(LOG_WARNING, "locked");
		simrpc_send_int(SIMRPC_REPLY_ERR(hdr), SIMRPC_EBUSY);
		return;
	}

	if (simrpc_file.nsync)
		fs_file_commit(&simrpc_file.entry, cnt);

	simrpc_send_opc(SIMRPC_REPLY_OK(hdr));

	simrpc_file.lock = SIMRPC_UNLOCK;
	simrpc_file.clk = clk;
}

void simrpc_file_seek_svc(uint32_t hdr, uint32_t * data, unsigned int cnt)
{
	unsigned int offs;
	uint32_t clk;

	if (cnt != 4) {
		DCC_LOG(LOG_WARNING, "Invalid packet size");
		return;
	};

	offs = data[0];
	clk = thinkos_clock();

	if (simrpc_file.lock == SIMRPC_UNLOCK) {
		DCC_LOG(LOG_WARNING, "locked");
		simrpc_send_int(SIMRPC_REPLY_ERR(hdr), SIMRPC_EBUSY);
		return;
	}

	if (offs > simrpc_file.max)
		offs = simrpc_file.max;

	simrpc_send_opc(SIMRPC_REPLY_OK(hdr));

	simrpc_file.offs = offs;
	simrpc_file.clk = clk + FILE_LOCK_TIMEOUT_MS;
}

