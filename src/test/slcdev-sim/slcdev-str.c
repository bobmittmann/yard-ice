#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/dcclog.h>

#include <microjs.h>

#include "json.h"
#include "crc.h"
#include "slcdev.h"

const struct microjs_str_pool microjs_str_const = {
//	.offs = (uint16_t *)(STM32_MEM_FLASH + FLASH_BLK_CONST_STRING_OFFS),
	.offs = (uint16_t *)(STM32_MEM_EEPROM),
	.base = (char *)(STM32_MEM_FLASH + FLASH_BLK_CONST_STRING_OFFS)
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
		return -JSON_ERR_FLASH_WRITE;
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

	stm32_eeprom_wr32(const_str_heap, 0);
	stm32_eeprom_wr32(const_str_heap + 4, 0);

	/* initialize string pool with a zero length string */
	return flash_str_write("", 0);
}

int slcdev_const_str_write(const char * s, unsigned int len)
{
	char buf[128];
	int idx;

	if (len == 0) {
		DCC_LOG(LOG_WARNING, "empty string!");
	} 

	if ((idx = const_str_lookup(s, len)) >= 0) {
		DCC_LOG2(LOG_TRACE, "match idx=%d len=%d", idx, len);
		return idx;
	}

	/* NULL terminate the string */
	memcpy(buf, s, len);
	buf[len] = '\0';

	return flash_str_write(buf, len);
}

