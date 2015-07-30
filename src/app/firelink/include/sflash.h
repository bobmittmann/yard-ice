/* 
 * File:	 zigbee.h
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

#ifndef __SFLASH_H__
#define __SFLASH_H__

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

struct sflash_dev * sflash_init(void);

int sflash_sector_erase(struct sflash_dev * sf, uint32_t addr);

int sflash_write_enable(struct sflash_dev * sf);

int sflash_page_write(struct sflash_dev * sf, uint32_t addr, 
					  const void * buf, size_t count);

int sflash_page_read(struct sflash_dev * sf, uint32_t addr, 
					 void * buf, size_t count);

int sflash_reset(struct sflash_dev * sf);

int sflash_device_id(struct sflash_dev * sf, void * buf, size_t count);

int sflash_read_status(struct sflash_dev * sf);

int sflash_flag_status_read(struct sflash_dev * sf);

#ifdef __cplusplus
}
#endif

#endif /* __SFLASH_H__ */

