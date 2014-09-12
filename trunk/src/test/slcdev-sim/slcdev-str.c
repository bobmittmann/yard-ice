#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/dcclog.h>

#include <microjs.h>

#include "crc.h"
#include "slcdev.h"

/****************************************************************************
 * This file implement the constant string pool
 * The strings are stored in a flash block
 * the string index is stored in the EEPROM
 ****************************************************************************/

int slcdev_const_str_write(const struct microjs_str_pool * pool, 
						   const char * s, unsigned int len);

const struct microjs_str_pool microjs_str_const = {
	.offs = (uint16_t *)(STM32_MEM_EEPROM),
	.base = (char *)(STM32_MEM_FLASH + FLASH_BLK_CONST_STRING_OFFS),
	.top = (char *)(STM32_MEM_FLASH + FLASH_BLK_CONST_STRING_OFFS +
					FLASH_BLK_CONST_STRING_SIZE),
	.write = slcdev_const_str_write
};

uint16_t const_str_stack = FLASH_BLK_CONST_STRING_SIZE;
uint16_t const_str_heap = 0;

static int flash_str_write(const char * s, unsigned int len)
{
	uint32_t flash_offs;
	uint16_t pos;
	int ret;
	int idx;

	len++; /* make room for the NULL */
	pos = (const_str_stack - len) & ~3;
	flash_offs = FLASH_BLK_CONST_STRING_OFFS + pos;

	if ((ret = stm32_flash_write(flash_offs, s, len)) < 0) {
		DCC_LOG(LOG_WARNING, "stm32_flash_write() failed!");
		return -1;
	}

	/* update stack */
	const_str_stack = pos;


	stm32_eeprom_unlock();
	if ((const_str_heap & 3) == 0) {
		stm32_eeprom_wr32(const_str_heap, pos);
	} else {
		uint32_t data;

		stm32_eeprom_rd32(const_str_heap - sizeof(uint16_t), &data);
		data |= pos << 16;
		stm32_eeprom_wr32(const_str_heap - sizeof(uint16_t), data);
		stm32_eeprom_wr32(const_str_heap + sizeof(uint16_t), 0);
	}

	idx = const_str_heap / sizeof(uint16_t);
	const_str_heap += sizeof(uint16_t);


#if 0
	flash_offs = FLASH_BLK_CONST_STRING_OFFS + const_str_heap;

	if ((ret = stm32_flash_write(flash_offs, &pos, sizeof(uint16_t))) < 0) {
		DCC_LOG(LOG_WARNING, "stm32_flash_write() failed!");
		return -JSON_ERR_FLASH_WRITE;
	}

	idx = const_str_heap / sizeof(uint16_t);
	const_str_heap += sizeof(uint16_t);

	/* check for collision */
	if (const_str_stack <= const_str_heap) {
		DCC_LOG2(LOG_ERROR, "no memory stack=%d heap=%d!", 
				const_str_stack , const_str_heap);
		return -1;
	}

	DCC_LOG3(LOG_TRACE, "idx=%d offs=%04x len=%d", idx, pos, len);
#endif

	return idx;
}

int slcdev_const_str_purge(void)
{
	uint32_t blk_offs = FLASH_BLK_CONST_STRING_OFFS;
	uint32_t blk_size = FLASH_BLK_CONST_STRING_SIZE;
	int ret;

	if ((ret = stm32_flash_erase(blk_offs, blk_size)) < 0) {
		DCC_LOG(LOG_ERROR, "stm32_flash_erase() failed!");
		return ret;
	};

	const_str_stack = FLASH_BLK_CONST_STRING_SIZE;
	const_str_heap = 0;

	/* initialize string pool with a zero length string */
	return flash_str_write("", 0);
}

int slcdev_const_str_write(const struct microjs_str_pool * pool, 
						   const char * s, unsigned int len)
{
	return flash_str_write(s, len);
}

int microjs_str_pool_dump(const struct microjs_str_pool * pool)
{
	char * base = (char *)pool->base;
	int offs;
	int size = pool->top - base;
	int free = size;
	int i;

	for (i = 0; (offs = pool->offs[i]) > 0; ++i) {
		if (offs < free)
			free = offs;
		char * cstr = base + offs;
		printf("%2d (%04x) \"%s\"\n", i, offs, cstr);
	}
	printf("- pool: size=%d free=%d\n", size, free);

	return i;
}


int microjs_str_lookup(const struct microjs_str_pool * pool, 
					   const char * s, int len)
{
	char * base = (char *)pool->base;
	int offs;
	int i;

	for (i = 0; (offs = pool->offs[i]) > 0; ++i) {
		char * cstr = base + offs;

		if ((strncmp(cstr, s, len) == 0) && (strlen(cstr) == len))
			return i;
	}

	return -1;
}

char * const_str(int idx)
{
	const struct microjs_str_pool * p = &microjs_str_const;

	return p->base + p->offs[idx];
}

int const_str_lookup(const char * s, int len)
{
	return microjs_str_lookup(&microjs_str_const, s, len);
}

#define MICROJS_STRING_LEN_MAX 128

int const_str_write(const char * s, unsigned int len)
{
	char buf[MICROJS_STRING_LEN_MAX];
	struct microjs_str_pool * pool;
	int idx;

	pool = (struct microjs_str_pool *)&microjs_str_const;

	if (len == 0) {
		DCC_LOG(LOG_WARNING, "empty string!");
	} 

	if ((idx = microjs_str_lookup(pool, s, len)) >= 0) {
		DCC_LOG2(LOG_INFO, "match idx=%d len=%d", idx, len);
		return idx;
	}

	/* NULL terminate the string */
	memcpy(buf, s, len);
	buf[len] = '\0';

	return pool->write(pool, buf, len + 1);
}

