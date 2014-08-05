#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <thinkos.h>
#include <sys/dcclog.h>

#include "jsmn.h"
#include "board.h"

int config_dump(FILE * f)
{
	DCC_LOG(LOG_TRACE, "...");

	return 0;
}

int config_erase(void)
{
#if 0
	uint32_t blk_offs = FLASH_BLK_DEV_DB_BIN_OFFS;
	uint32_t blk_size = FLASH_BLK_DEV_DB_BIN_SIZE;
	int ret;

	if ((ret = stm32_flash_erase(blk_offs, blk_size)) < 0) {
		DCC_LOG(LOG_ERROR, "stm32_flash_erase() failed!");
		return ret;
	};

	DCC_LOG(LOG_TRACE, "configuration erased!");

	return ret;
#endif
	return 0;
}

int config_compile(void)
{
	jsmn_parser p;
	jsmntok_t tok[TOK_MAX];
	jsmntok_t * t;
	char * js;
	int len;
	int ret;
	int n;
	int i;

	js = (char *)(STM32_MEM_FLASH + FLASH_BLK_SIM_CFG_JSON_OFFS);
	len = json_len(js);

	DCC_LOG2(LOG_TRACE, "js=0x%08x len=%d", js, len);

	jsmn_init(&p);

	ret = jsmn_parse(&p, js, len, tok, TOK_MAX);

	if (ret == JSMN_ERROR_NOMEM) {
		DCC_LOG(LOG_ERROR, "Not enough tokens were provided!");
		return -JSON_ERR_NOMEM;
	}

	if (ret == JSMN_ERROR_INVAL) {
		DCC_LOG1(LOG_ERROR, "Invalid character at: %d", p.pos);
		jsmn_dump_err(&p, js);
		return -JSON_ERR_INVALID_CHAR;
	}

	if (ret == JSMN_ERROR_PART) {
		DCC_LOG(LOG_ERROR, "not a full JSON packet!");
		jsmn_dump_err(&p, js);
		return -JSON_ERR_INCOMPLETE;
	}

	if (ret == 0) {
		DCC_LOG(LOG_ERROR, "empty JSON packet!");
		return -JSON_ERR_PKT_EMPTY;
	}

	t = tok;

	/* Should never reach uninitialized tokens */
	if (t->start == JSMN_NULL || t->end == JSMN_NULL) {
		DCC_LOG(LOG_ERROR, "parameter invalid!");
		return -JSON_ERR_INVALID_TOKEN;
	}

	if (t->type != JSMN_OBJECT) {
		DCC_LOG(LOG_ERROR, "root element must be an object.");
		return -JSON_ERR_NOT_OBJECT;
	}

	if ((n = t->size) == 0)
		return -JSON_ERR_EMPTY_OBJECT;

	if (n % 2 != 0) {
		DCC_LOG(LOG_ERROR, "object must have even number of children.");
		return -JSON_ERR_NUM_CHILDREN;
	}

	DCC_LOG1(LOG_TRACE, "n=%d", n);

	/* erase flash block */
	if (stm32_flash_erase(FLASH_BLK_DEV_DB_BIN_OFFS, 
						  FLASH_BLK_DEV_DB_BIN_SIZE) < 0) {
		DCC_LOG(LOG_ERROR, "stm32_flash_erase() failed!");
		return -1;
	};

	/* initialise PW descriptor stack */
	db_stack = FLASH_BLK_DEV_DB_BIN_SIZE;
	db_heap = 0;

	t++;

	for (i = 0; i < n; i += 2) {
		char s[16];;
		int r;

		if (t->type != JSMN_STRING) {
			DCC_LOG(LOG_ERROR, "object keys must be strings.");
			return -1;
		}

		json_string(s, sizeof(s), js, t);
		t++;

		if (strcmp(s, "sensor") == 0) {
			r = json_parse_sensor(js, t);
		} else if (strcmp(s, "module") == 0) {
			r = json_parse_module(js, t);
		} else {
			r = -JSON_ERR_INVALID_OBJECT;
			DCC_LOG(LOG_ERROR, "invalid object.");
		}

		if (r < 0)
			return r;

		t += r;
	}

	printf("Device databse compiled.\n");
	printf("Free memory: %d.%02d KiB.\n", (db_stack - db_heap) / 1024, 
		   (((db_stack - db_heap) % 1024) * 100) / 1024);

	return 0;
}

