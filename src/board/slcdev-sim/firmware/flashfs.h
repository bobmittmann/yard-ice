/* 
 * File:	 flashfs.h
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(c) 2003-2006 BORESTE (www.boreste.com). All Rights Reserved.
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

#ifndef __FLASHFS_H__
#define __FLASHFS_H__

#include <stdio.h>
#include <stdint.h>

struct fs_file {
	uint16_t size;
	uint16_t crc;
	const uint8_t data[];
};

struct fs_dirent {
	const char * name;
	struct fs_file * fp;
	uint32_t blk_offs;
	uint16_t blk_size;
};

enum {
	FLASHFS_STRINGS = 0,
	FLASHFS_DB_BIN,
	FLASHFS_CFG_BIN,
	FLASHFS_DB_JSON,
	FLASHFS_CFG_JSON,
	FLASHFS_CODE_BIN,
	FLASHFS_1_JS,
	FLASHFS_2_JS,
	FLASHFS_3_JS,
	FLASHFS_4_JS,
};

#ifdef __cplusplus
extern "C" {
#endif

bool fs_dirent_lookup(const char * name, struct fs_dirent * ep);

bool fs_dirent_get_next(struct fs_dirent * ep);

bool fs_xmodem_recv(FILE * f, const char * name);

bool fs_file_unlink(struct fs_dirent * ep);

bool fs_dirent_get(struct fs_dirent * ep, unsigned int idx);

bool fs_file_commit(struct fs_dirent * ep, unsigned int size);

#ifdef __cplusplus
}
#endif

#endif /* __FLASHFS_H__ */

