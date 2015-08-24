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


#include <arch/cortex-m3.h>
#include <sys/dcclog.h>
#include <string.h>
#include <crc.h>
#include <xmodem.h>
#include <sys/tty.h>
#include <sys/param.h>

#include "board.h"
#include "flashfs.h"

/* Filesystem block descriptor */
struct fs_blk {
	char name[10];
	uint16_t size;
	uint32_t offs;
};

/*****************************************************************************
 * Pseudo filesystem directory 
 *****************************************************************************/

const struct fs_blk flash_desc[] = {
	[FLASHFS_STRINGS] = { .name = "strings", 
		.offs = FLASH_BLK_CONST_STRING_OFFS,
		.size = FLASH_BLK_CONST_STRING_SIZE
	},
	[FLASHFS_DB_BIN] = { .name = "db.bin", 
		.offs = FLASH_BLK_DB_BIN_OFFS,
		.size = FLASH_BLK_DB_BIN_SIZE  
	},
	[FLASHFS_CFG_BIN] = { .name = "cfg.bin", 
		.offs = FLASH_BLK_CFG_BIN_OFFS,
		.size = FLASH_BLK_CFG_BIN_SIZE  
	},
	[FLASHFS_DB_JSON] = { .name = "db.js", 
		.offs = FLASH_BLK_DB_JSON_OFFS,
		.size = FLASH_BLK_DB_JSON_SIZE  
	},
	[FLASHFS_CFG_JSON] = { .name = "cfg.js", 
		.offs = FLASH_BLK_CFG_JSON_OFFS,
		.size = FLASH_BLK_CFG_JSON_SIZE  
	},

#if 0
	[FLASHFS_CODE_BIN] = { .name = "code.bin", 
		.offs = FLASH_BLK_CODE_BIN_OFFS,
		.size = FLASH_BLK_CODE_BIN_SIZE  
	},

	[FLASHFS_1_JS] = { .name = "1.js", 
		.offs = FLASH_BLK_1_JS_OFFS,
		.size = FLASH_BLK_1_JS_SIZE  
	},
	[FLASHFS_2_JS] = { .name = "2.js", 
		.offs = FLASH_BLK_2_JS_OFFS,
		.size = FLASH_BLK_2_JS_SIZE  
	},
	[FLASHFS_3_JS] = { .name = "3.js", 
		.offs = FLASH_BLK_3_JS_OFFS,
		.size = FLASH_BLK_3_JS_SIZE  
	},
	[FLASHFS_4_JS] = { .name = "4.js", 
		.offs = FLASH_BLK_4_JS_OFFS,
		.size = FLASH_BLK_4_JS_SIZE  
	}
#endif
};

#define FLASH_FS_BLK_COUNT (sizeof(flash_desc) / sizeof(struct fs_blk)) 

const struct fs_file null_fp = {
	.size = 0,
	.crc = 0
};

bool fs_dirent_get(struct fs_dirent * ep, unsigned int idx)
{
	struct fs_file * fp;

	if (idx >= FLASH_FS_BLK_COUNT)
		return false;

	fp = (struct fs_file *)(STM32_MEM_FLASH + flash_desc[idx].offs);
	ep->name = flash_desc[idx].name;
	ep->blk_size = flash_desc[idx].size;
	ep->blk_offs = flash_desc[idx].offs;

	if ((fp->size >= ep->blk_size) || 
		(fp->crc != crc16ccitt(0, fp->data, fp->size)))
		ep->fp = (struct fs_file *)&null_fp;
	else
		ep->fp = fp;

	return true;
}

/* look up for a directory entry by name */
bool fs_dirent_lookup(const char * name, struct fs_dirent * ep)
{
	int i;

	for (i = 0; i < FLASH_FS_BLK_COUNT; ++i) {
		if (strcmp(flash_desc[i].name, name) == 0)
			return fs_dirent_get(ep, i);
	}

	DCC_LOG(LOG_INFO, "file not found!");

	return false;
}

/* get the next directory entry from the one provided */
bool fs_dirent_get_next(struct fs_dirent * ep)
{
	int i = 0;

	if (ep->blk_size != 0) {
		do {
			/* if we are pointing to the one last element, 
			   there is no next in the list, bail out */
			if (i == FLASH_FS_BLK_COUNT - 1)
				return false;

		} while (flash_desc[i++].offs != ep->blk_offs);
	}

	return fs_dirent_get(ep, i);
}


bool fs_file_unlink(struct fs_dirent * ep)
{
	if (stm32_flash_erase(ep->blk_offs, ep->blk_size) < 0) {
		return false;
	};

	return true;
}

bool fs_file_commit(struct fs_dirent * ep, unsigned int size)
{
	struct fs_file file;
	struct fs_file * fp;

	DCC_LOG2(LOG_INFO, "offs=%06x size=%d", ep->blk_offs, size);
	
	fp = (struct fs_file *)(STM32_MEM_FLASH + ep->blk_offs);
	file.size = size;
	file.crc = crc16ccitt(0, fp->data, size);

	if (stm32_flash_write(ep->blk_offs, &file, sizeof(struct fs_file)) < 0) {
		ep->fp = (struct fs_file *)&null_fp;
		return false;
	}

	ep->fp = fp;
	return true;
}

/* Receive a file using XMODEM protocol */

bool fs_xmodem_recv(FILE * f, const char * name)
{
	struct fs_dirent entry;
	struct comm_dev comm;
	struct file * raw;
	uint8_t buf[128];
	unsigned int cnt;
	unsigned int rem;
	unsigned int offs;
	struct xmodem_rcv rx;
	int ret;

	if (!fs_dirent_lookup(name, &entry)) {
		return false;
	}

	fs_file_unlink(&entry);

	if ((raw = ftty_lowlevel(f)) == NULL)
		raw = f;

	comm.arg = raw->data;
	comm.op.send = (int (*)(void *, const void *, unsigned int))raw->op->write;
	comm.op.recv = (int (*)(void *, void *, 
						  unsigned int, unsigned int))raw->op->read;

	cnt = 0;
	rem = entry.blk_size - sizeof(struct fs_file);
	offs = entry.blk_offs + sizeof(struct fs_file);

	xmodem_rcv_init(&rx, &comm, XMODEM_RCV_CRC);

	do {
		unsigned int n;
	
		if ((ret = xmodem_rcv_loop(&rx, buf, 128)) < 0) {
			return ret;
		}

		if (rem == 0) {
			xmodem_rcv_cancel(&rx);
			break;
		}

		DCC_LOG2(LOG_INFO, "ret=%d rem=%d", ret, rem);

		n = MIN(rem, ret); 
	
		DCC_LOG2(LOG_INFO, "stm32_flash_write(offs=%06x size=%d)", offs, n);
		stm32_flash_write(offs, buf, n);
		cnt += n;
		offs += n;
		rem -= n;
	} while (ret > 0);

	return fs_file_commit(&entry, cnt);
}

