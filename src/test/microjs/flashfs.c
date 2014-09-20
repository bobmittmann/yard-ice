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
#include <string.h>
#include <sys/param.h>
#include <xmodem.h>
#include <sys/tty.h>
#include <crc.h>
#include <sys/dcclog.h>

#include "board.h"
#include "flashfs.h"

/* Filesystem block descriptor */
struct fs_blk {
	uint32_t offs;
	uint16_t size;
	uint16_t flags;
};

/*****************************************************************************
 * Pseudo filesystem directory 
 *****************************************************************************/

const struct fs_blk flash_desc[] = {
	{
	  .offs = FLASH_BLK_SLOT1_OFFS,
	  .size = FLASH_BLK_SLOT1_SIZE  
	},
	{ 
	  .offs = FLASH_BLK_SLOT2_OFFS,
	  .size = FLASH_BLK_SLOT2_SIZE  
	},
	{ 
	  .offs = FLASH_BLK_SLOT3_OFFS,
	  .size = FLASH_BLK_SLOT3_SIZE  
	},
	{ 
	  .offs = FLASH_BLK_SLOT4_OFFS,
	  .size = FLASH_BLK_SLOT4_SIZE  
	},
	{ 
	  .offs = FLASH_BLK_SLOT5_OFFS,
	  .size = FLASH_BLK_SLOT5_SIZE  
	},
	{ 
	  .offs = FLASH_BLK_SLOT6_OFFS,
	  .size = FLASH_BLK_SLOT6_SIZE  
	},
	{ 
	  .offs = FLASH_BLK_SLOT7_OFFS,
	  .size = FLASH_BLK_SLOT7_SIZE  
	},
	{ 
	  .offs = FLASH_BLK_SLOT8_OFFS,
	  .size = FLASH_BLK_SLOT8_SIZE  
	},
	{
	  .offs = FLASH_BLK_SLOT9_OFFS,
	  .size = FLASH_BLK_SLOT9_SIZE  
	},
	{ 
	  .offs = FLASH_BLK_SLOT10_OFFS,
	  .size = FLASH_BLK_SLOT10_SIZE  
	},
	{
	  .offs = FLASH_BLK_SLOT11_OFFS,
	  .size = FLASH_BLK_SLOT11_SIZE  
	},
	{ 
	  .offs = FLASH_BLK_SLOT12_OFFS,
	  .size = FLASH_BLK_SLOT12_SIZE  
	},
	{ 
	  .offs = FLASH_BLK_SLOT13_OFFS,
	  .size = FLASH_BLK_SLOT13_SIZE  
	},
	{ 
	  .offs = FLASH_BLK_SLOT14_OFFS,
	  .size = FLASH_BLK_SLOT14_SIZE  
	},
	{ 
	  .offs = FLASH_BLK_SLOT15_OFFS,
	  .size = FLASH_BLK_SLOT15_SIZE  
	},
	{ 
	  .offs = FLASH_BLK_SLOT16_OFFS,
	  .size = FLASH_BLK_SLOT16_SIZE  
	},
};

#define FLASH_FS_BLK_COUNT (sizeof(flash_desc) / sizeof(struct fs_blk)) 

/* look up for a directory entry by name */
bool fs_dirent_lookup(const char * name, struct fs_dirent * ep)
{
	struct fs_file * fp;
	int n;
	int i;

	n = strlen(name);
	for (i = 0; i < FLASH_FS_BLK_COUNT; ++i) {
		fp = (struct fs_file *)(STM32_MEM_FLASH + flash_desc[i].offs);
		if ((fp->size == 0) || 
			(fp->size > flash_desc[i].size - sizeof(struct fs_file)))
			continue;
		if ((strncmp(fp->name, name, 12) == 0) && (n <= 12)) {
			if (fp->crc != crc16ccitt(0, fp->data, fp->size))
				continue;
			ep->blk_size = flash_desc[i].size;
			ep->blk_offs = flash_desc[i].offs;
			ep->fp = fp;
			return true;
		}
	}
	return false;
}

/* get the next directory entry from the one provided */
bool fs_dirent_get_next(struct fs_dirent * ep)
{
	struct fs_file * fp;
	int i = 0;

	if (ep->blk_size != 0) {
		do {
			/* if we are pointing to the one last element, 
			   there is no next in the list, bail out */
			if (i == FLASH_FS_BLK_COUNT - 1)
				return false;
		} while (flash_desc[i++].offs != ep->blk_offs);
	}

	for (; i < FLASH_FS_BLK_COUNT; ++i) {
		fp = (struct fs_file *)(STM32_MEM_FLASH + flash_desc[i].offs);
		if ((fp->size == 0) || 
			(fp->size > flash_desc[i].size - sizeof(struct fs_file))) 
			continue;
		if (fp->crc != crc16ccitt(0, fp->data, fp->size))
			continue;

		ep->blk_size = flash_desc[i].size;
		ep->blk_offs = flash_desc[i].offs;
		ep->fp = fp;
		return true;
	}

	return false;
}

bool fs_dirent_get_free(struct fs_dirent * ep)
{
	struct fs_file * fp;
	int i = 0;

	for (i = 0; i < FLASH_FS_BLK_COUNT; ++i) {
		fp = (struct fs_file *)(STM32_MEM_FLASH + flash_desc[i].offs);
		if ((fp->size > 0) && 
			(fp->size <= flash_desc[i].size - sizeof(struct fs_file)) &&
			(fp->crc == crc16ccitt(0, fp->data, fp->size)))
			continue;

		ep->blk_size = flash_desc[i].size;
		ep->blk_offs = flash_desc[i].offs;
		ep->fp = fp;
		return true;
	}

	return false;
}

bool fs_file_unlink(struct fs_dirent * ep)
{
	if (stm32_flash_erase(ep->blk_offs, ep->blk_size) < 0) {
		return false;
	};

	return true;
}

bool fs_xmodem_recv(FILE * f, const char * name)
{
	struct fs_dirent entry;
	struct fs_file * fp;
	struct comm_dev comm;
	struct file * raw;
	uint8_t buf[128];
	unsigned int cnt;
	unsigned int rem;
	unsigned int offs;
	struct xmodem_rcv rx;
	int ret;


	if (!fs_dirent_lookup(name, &entry)) {
		if (!fs_dirent_get_free(&entry)) {
			fprintf(f, "Can't allocate file block!\n");
			return false;
		}
	}

	fs_file_unlink(&entry);

	if ((raw = ftty_lowlevel(f)) == NULL)
		raw = f;

	DCC_LOG1(LOG_MSG, "raw=%p", raw);

	comm.arg = raw->data;
	comm.op.send = (int (*)(void *, const void *, unsigned int))raw->op->write;
	comm.op.recv = (int (*)(void *, void *, 
						  unsigned int, unsigned int))raw->op->read;

	DCC_LOG(LOG_INFO, ".................................");

	xmodem_rcv_init(&rx, &comm, XMODEM_RCV_CRC);

	cnt = 0;
	rem = entry.blk_size;
	offs = entry.blk_offs + sizeof(struct fs_file);
	do {
		unsigned int n;
	

		if ((ret = xmodem_rcv_loop(&rx, buf, 128)) < 0) {
			DCC_LOG1(LOG_ERROR, "ret=%d", ret);
			return ret;
		}

		if (rem == 0) {
			xmodem_rcv_cancel(&rx);
			break;
		}

		n = MIN(rem, ret); 
		stm32_flash_write(offs, buf, n);
		cnt += n;
		offs += n;
		rem -= n;
		DCC_LOG1(LOG_TRACE, "n=%d", n);
	} while (ret > 0);

	fp = (struct fs_file *)buf;
	fp->size = cnt;
	fp->crc = crc16ccitt(0, entry.fp->data, cnt);
	strncpy(fp->name, name, 12);
	
	return stm32_flash_write(entry.blk_offs, fp, sizeof(struct fs_file));
}

