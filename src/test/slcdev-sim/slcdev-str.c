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

/**********************************************************************
  Strings
 **********************************************************************/
struct const_strbuf {
	uint16_t * offs; /* point to the offset table */
	char * base;     /* base pointer */
	char * top;      /* top pointer */
};

const struct const_strbuf flash_strbuf = {
	.offs = (uint16_t *)(STM32_MEM_EEPROM),
	.base = (char *)(STM32_MEM_FLASH + FLASH_BLK_CONST_STRING_OFFS),
	.top = (char *)(STM32_MEM_FLASH + FLASH_BLK_CONST_STRING_OFFS +
					FLASH_BLK_CONST_STRING_SIZE)
};

uint16_t const_str_stack = FLASH_BLK_CONST_STRING_SIZE;
uint16_t const_str_heap = 0;

int flash_str_write(const char * s, unsigned int len)
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
		return -ERR_GENERAL;
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

int const_strbuf_purge(void)
{
	uint32_t blk_offs = FLASH_BLK_CONST_STRING_OFFS;
	uint32_t blk_size = FLASH_BLK_CONST_STRING_SIZE;
	char buf[2];
	int ret;

	if ((ret = stm32_flash_erase(blk_offs, blk_size)) < 0) {
		DCC_LOG(LOG_ERROR, "stm32_flash_erase() failed!");
		return ret;
	};

	const_str_stack = FLASH_BLK_CONST_STRING_SIZE;
	const_str_heap = 0;

	/* initialize string pool with a zero length string */
	buf[0] = '\0';

	return flash_str_write(buf, 1);
}

int const_strbuf_dump(FILE * f)
{
	const struct const_strbuf * p = &flash_strbuf;
	char * base = (char *)p->base;
	int size = p->top - base;
	int free = size;
	int offs;
	int i;

	for (i = 0; (offs = p->offs[i]) > 0; ++i) {
		if (offs < free)
			free = offs;
		char * cstr = base + offs;
		fprintf(f, "%2d (%04x) \"%s\"\n", i, offs, cstr);
	}
	fprintf(f, "- pool: size=%d free=%d\n", size, free);

	return i;
}

int str_lookup(const char * s, unsigned int len)
{
	const struct const_strbuf * p = &flash_strbuf;
	char * base = (char *)p->base;
	int offs;
	int i;

	for (i = 0; (offs = p->offs[i]) > 0; ++i) {
		char * cstr = base + offs;

		if ((strncmp(cstr, s, len) == 0) && (strlen(cstr) == len))
			return i;
	}

	return -ERR_STRING_NOT_FOUND;
}

#define MICROJS_STRING_LEN_MAX 128

int str_add(const char * s, unsigned int len)
{
	char buf[MICROJS_STRING_LEN_MAX + 1];
	int idx;

	if ((idx = const_str_lookup(s, len)) >= 0) {
		return idx;
	}

	if (len > MICROJS_STRING_LEN_MAX)
		return -ERR_STRING_TOO_LONG;

	/* NULL terminate the string */
	memcpy(buf, s, len);
	buf[len] = '\0';

	return flash_str_write(buf, len + 1);
}

const char * str(unsigned int idx)
{
	const struct const_strbuf * p = &flash_strbuf;

	return p->base + p->offs[idx];
}

int cstr_decode(char * dst, const char * src, unsigned int len)
{
	bool esc;
	int c;
	int i;
	/* FIXME: octal and hexadecimal coded chars */

	esc = false;
	for (i = 0; i < len; ++i) {
		c = src[i];
		if (esc) {
			switch (c) {
			case 'a': /* Alarm (Bell) */
				c = '\a';
				break;
			case 'b': /* Backspace */
				c = '\b';
				break;
			case 'f': /* Formfeed */
				c = '\f';
				break;
			case 'n': /* Newline */
				c = '\n';
				break;
			case 'r': /* Carriage Return */
				c = '\r';
				break;
			case 't': /* Horizontal Tab */
				c = '\t';
				break;
			case 'v': /* Vertical Tab */
				c = '\v';
				break;
			}
			esc = false;
		} else if (c == '\\') {
			esc = true;
			continue;
		} 
		*dst++ = c;
	}
	/* NULL terminate the string */
	*dst = '\0';

	return 0;
}

/* add a string to the var buffer translating the most 
   common C scape sequences */
int cstr_add(const char * s, unsigned int len)
{
	char dst[MICROJS_STRING_LEN_MAX + 1];
	int idx;

	if ((idx = const_str_lookup(s, len)) >= 0) {
		return idx;
	}

	if (len > MICROJS_STRING_LEN_MAX)
		return -ERR_STRING_TOO_LONG;

	/* Convert and copy the string to the buffer */
	cstr_decode(dst, s, len);

	return flash_str_write(dst, len + 1);
}



int const_str_lookup(const char * s, unsigned int len)
	__attribute__((weak, alias("str_lookup")));

int const_str_add(const char * s, unsigned int len)
	__attribute__((weak, alias("str_add")));

const char * const_str(unsigned int idx)
	__attribute__((weak, alias("str")));


