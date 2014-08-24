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

const struct fs_blk flash_fs_dir[] = {
	{ .name = "strings", 
	  .offs = FLASH_BLK_CONST_STRING_OFFS,
	  .size = FLASH_BLK_CONST_STRING_SIZE
	},
	{ .name = "db.bin", 
	  .offs = FLASH_BLK_DB_BIN_OFFS,
	  .size = FLASH_BLK_DB_BIN_SIZE  
	},
	{ .name = "cfg.bin", 
	  .offs = FLASH_BLK_CFG_BIN_OFFS,
	  .size = FLASH_BLK_CFG_BIN_SIZE  
	},
	{ .name = "db.js", 
	  .offs = FLASH_BLK_DB_JSON_OFFS,
	  .size = FLASH_BLK_DB_JSON_SIZE  
	},
	{ .name = "cfg.js", 
	  .offs = FLASH_BLK_CFG_JSON_OFFS,
	  .size = FLASH_BLK_CFG_JSON_SIZE  
	}
};

#define FLASH_FS_BLK_COUNT (sizeof(flash_fs_dir) / sizeof(struct fs_blk)) 

#if 0
uint16_t flash_fs_file_size[FLASH_FS_BLK_COUNT] = {
	0, 0, 0
};
#endif

/* look up for a directory entry by name */
bool fs_dirent_lookup(const char * name, struct fs_dirent * ep)
{
	int i;

	for (i = 0; i < FLASH_FS_BLK_COUNT; ++i) {
		if (strcmp(flash_fs_dir[i].name, name) == 0) {
			strcpy(ep->name, flash_fs_dir[i].name);
			ep->blk_size = flash_fs_dir[i].size;
			ep->size = flash_fs_dir[i].size;
			ep->offs = flash_fs_dir[i].offs;
			return true;
		}
	}

	DCC_LOG(LOG_ERROR, "file not found!");

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

		} while (flash_fs_dir[i++].offs != ep->offs);
	}

	strcpy(ep->name, flash_fs_dir[i].name);
	ep->blk_size = flash_fs_dir[i].size;
	ep->size = flash_fs_dir[i].size;
	ep->offs = flash_fs_dir[i].offs;

	return ep;
}

// FIXME: move this elsewhere...
#include <crc.h>

int json_file_get(unsigned int offs, struct json_file * json)
{
	json->txt = (char *)(STM32_MEM_FLASH + offs);
	json->len = microjs_json_root_len(json->txt);
	json->crc = crc16ccitt(0, json->txt, json->len);

	DCC_LOG3(LOG_TRACE, "   json: txt=0x%08x len=%d crc=0x%04x", 
			 json->txt, json->len, json->crc);

	return 0;
}

