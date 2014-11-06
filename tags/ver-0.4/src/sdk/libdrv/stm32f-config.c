/* 
 * File:	stm32f-config.c
 * Author:  Robinson Mittmann (bobmittmann@gmail.com)
 * Target: 
 * Comment: 
 * Copyright(C) 2012 Bob Mittmann. All Rights Reserved.
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

#if CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <crc.h>
#include <memory.h>
#include <stdbool.h>

#include <sys/stm32f.h>
#include <sys/dcclog.h>

#ifndef CONFIG_BLOCK_SIZE_MAX 
#define CONFIG_BLOCK_SIZE_MAX 64
#endif

struct conf_blk {
	uint8_t crc; 
	uint8_t size; 
	uint16_t key;
	uint8_t data[];
};

#define CONF_ADDR ((uint32_t)STM32F_BKPSRAM + 2048)
#define CONF_SIZE 2048

#define DATA_MAX (CONFIG_BLOCK_SIZE_MAX - sizeof(uint32_t))

/* convert from size to blocks */
#define _BLKS(SIZE) (1 + (((SIZE) + sizeof(uint32_t) - 1) / sizeof(uint32_t)))

#define _CRC(PTR) ((*PTR >> 0) & 0xff)
#define _SIZE(PTR) ((*PTR >> 8) & 0xff)
#define _KEY(PTR) ((*PTR >> 16) & 0xffff)

#define _NEXT(PTR) (PTR + _BLKS(_SIZE(PTR)))
#define _IS_EMPTY(PTR) ((*PTR == 0xffffffff) || (*PTR == 0x00000000))


static inline bool _is_valid(uint32_t * this, uint32_t * end)
{
	unsigned int key;
	unsigned int crc;
	int size;

	key = _KEY(this);
	crc = _CRC(this);
	size = _SIZE(this);

	DCC_LOG1(LOG_TRACE, "hdr=0x%08x", *this);

	if ((key == 0xffff) || (key == 0x0000)) {
		DCC_LOG(LOG_INFO, "NULL key!");
		return false;
	}

	if ((size < 0) || (size > DATA_MAX)) {
		DCC_LOG1(LOG_INFO, "invalid size: %d!", size);
		return false;
	}

	if (_NEXT(this) > end) {
		DCC_LOG(LOG_INFO, "overflow!");
		return false;
	}

	if (crc != crc8dallas_blk(size, (uint8_t *)this + 2, size + 2)) {
		DCC_LOG(LOG_INFO, "CRC error!");
		return false;
	}

	return true;;
}

static uint32_t * blk_lookup(uint32_t * start, uint32_t * end)
{
	uint32_t * this = start;

	for (; this < end; this++) {
		if (_IS_EMPTY(this)) {
			DCC_LOG(LOG_INFO, "empty!");
			return this;
		}

		if (_is_valid(this, end)) {
			DCC_LOG4(LOG_INFO, "good: 0x%08x key=%d size=%d crc=%02x", this,
				_KEY(this), _SIZE(this), _CRC(this));
			return this;	
		}
	}

	return NULL;
}

int stm32f_config_erase(void)
{
	uint32_t base_addr = CONF_ADDR;
	uint32_t conf_size = CONF_SIZE;

	memset((void *)base_addr, 0xff, conf_size);

	return conf_size;
}


struct conf_blk * stm32f_config_get(uint32_t key)
{
	uint32_t base_addr = CONF_ADDR;
	uint32_t conf_size = CONF_SIZE;
	uint32_t * end;
	uint32_t * this;

	this = (uint32_t *)base_addr;
	end = (uint32_t *)(base_addr + conf_size);

	DCC_LOG3(LOG_TRACE, "begin=%08x end=%08x key=%04x", this, end, key);

	if (!_IS_EMPTY(this) && !_is_valid(this, end)) {
		DCC_LOG(LOG_WARNING, "The first block is invalid, erasing all!");
		stm32f_config_erase();
		return NULL;
	}

	while (_KEY(this) != key) {
		if ((this = blk_lookup(_NEXT(this), end)) == NULL) {
			return NULL;
		}
		if (_IS_EMPTY(this)) {
			return NULL;
		}
	}

	return (struct conf_blk *)this;
}

struct conf_blk * stm32f_config_set(uint32_t key, const void * buf, int size)
{
	uint32_t base_addr = CONF_ADDR;
	uint32_t conf_size = CONF_SIZE;
	uint32_t hdr;
	uint32_t offs;
	unsigned int crc;
	uint32_t * end;
	uint32_t * this;
	int d_size;
	int i;

	this = (uint32_t *)base_addr;
	end = (uint32_t *)(base_addr + conf_size);

	d_size = MIN(DATA_MAX, size);

	DCC_LOG3(LOG_TRACE, "begin=%08x end=%08x size=%d", this, end, d_size);

	if (!_IS_EMPTY(this) && !_is_valid(this, end)) {
		DCC_LOG(LOG_WARNING, "The first block is invalid, erasing all!");
		stm32f_config_erase();
	}

	while (!_IS_EMPTY(this)) {
		if ((this = blk_lookup(_NEXT(this), end)) == NULL) {
			DCC_LOG(LOG_WARNING, "device full!");
			/* garbage collection */
			return NULL;
		}
	}

	if ((this + _BLKS(d_size)) > end) {
		DCC_LOG(LOG_WARNING, "device full!");
		/* garbage collection */
		return NULL;
	}

	for (i = 0; i < _BLKS(d_size); i++) {
		if ((this[i] != 0xffffffff) && (this[i] != 0x00000000)) {
			DCC_LOG(LOG_WARNING, "non empty space, check needed!");
			return NULL;
		}
	}

	crc = crc8dallas(d_size, key & 0xff);
	crc = crc8dallas(crc, (key >> 8) & 0xff);
	crc = crc8dallas_blk(crc, buf, d_size);

	hdr = (crc + (d_size << 8)) + (key << 16);

	DCC_LOG1(LOG_TRACE, "hdr=0x%08x", *this);

	offs = (uint32_t)((uint32_t)this - base_addr);

	DCC_LOG5(LOG_INFO, "blk=%08x base=%08x offs=%06x key=%04x size=%d", 
		(int)this, base_addr, offs, key, d_size);

	/* write header */
	if (memcpy((void *)(base_addr + offs), &hdr, sizeof(uint32_t)) < 0)
		return NULL;
	offs += sizeof(uint32_t);

	/* write payload */
	if (memcpy((void *)(base_addr + offs), buf, d_size) < 0)
		return NULL;

	DCC_LOG(LOG_TRACE, "4. done");

	return (struct conf_blk *)this;
}

int stm32f_config_list(struct conf_blk * blk[], unsigned int count)
{
	uint32_t base_addr = CONF_ADDR;
	uint32_t conf_size = CONF_SIZE;
	uint32_t * end;
	uint32_t * this;
	unsigned int key;
	int size;
	int cnt;
	int i;
	
	this = (uint32_t *)base_addr;
	end = (uint32_t *)(base_addr + conf_size);

	DCC_LOG2(LOG_TRACE, "begin=0x%08x end=0x%08x", 
			 (uint32_t)this, (uint32_t)end);

	if (!_IS_EMPTY(this) && !_is_valid(this, end)) {
		DCC_LOG(LOG_WARNING, "The first block is invalid, erasing all!");
		stm32f_config_erase();
	}

	cnt = 0;
	for (; this < end; this = _NEXT(this)) {

		if (_IS_EMPTY(this)) {
			break;
		}

		if ((this = blk_lookup(this, end)) == NULL) {
			break;
		}

		key = _KEY(this);
		size = _SIZE(this);

		if (size == 0) {
			/* remove from list */
			for (i = 0; i < cnt; i++) {
				if (key == blk[i]->key) {
					DCC_LOG1(LOG_TRACE, "%04x - remove", key);
					cnt--;
					break;
				}
			}
			for (; i < cnt; i++) {
				blk[i] = blk[i + 1];
			}
			continue;
		}

		/* insert into list */
		for (i = 0; ;i++) {
			if (i == cnt) {
				DCC_LOG1(LOG_TRACE, "%04x - list insert", key);
				cnt++;
				break;
			}
			if (key == blk[i]->key) {
				DCC_LOG1(LOG_TRACE, "%04x - list replace", key);
				break;
			}
		}

		blk[i] = (struct conf_blk *)this;
	}

	DCC_LOG1(LOG_TRACE, "cnt=%d", cnt);
	
	return cnt;
}

#if 0
void stm32f_config_init(void)
{
	stm32f_bkp_sram_init();
}

int stm32f_config_check(void)
{
	uint32_t base_addr = CONF_ADDR;
	uint32_t conf_size = CONF_SIZE;
	uint32_t * end;
	uint32_t * begin;
	uint32_t * this;
	
	begin = (uint32_t *)base_addr;
	end = (uint32_t *)(base_addr + conf_size);

	DCC_LOG2(LOG_TRACE, "begin=0x%08x end=0x%08x", 
			 (uint32_t)begin, (uint32_t)end);

	/* find first good block */
	if ((this = blk_lookup(begin, end)) == NULL) {
		/* no good blocks, erase all */
		stm32f_config_erase();
	}

	return 0;
}
#endif

void svc_config_set(void) 
	__attribute__ ((weak, alias ("stm32f_config_set")));

void svc_config_get(void) 
	__attribute__ ((weak, alias ("stm32f_config_get")));

void svc_config_list(void) 
	__attribute__ ((weak, alias ("stm32f_config_list")));

void svc_config_erase(void) 
	__attribute__ ((weak, alias ("stm32f_config_erase")));

