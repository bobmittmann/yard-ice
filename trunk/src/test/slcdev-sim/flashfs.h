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

struct fs_dirent {
	char name[10];
	uint16_t size;
	uint16_t blk_size;
	uint32_t offs;
};

#include <microjs.h>

#ifdef __cplusplus
extern "C" {
#endif

bool fs_dirent_lookup(const char * name, struct fs_dirent * ep);

bool fs_dirent_get_next(struct fs_dirent * ep);

int json_file_get(uint32_t offs, struct json_file * json);

#ifdef __cplusplus
}
#endif

#endif /* __FLASHFS_H__ */

