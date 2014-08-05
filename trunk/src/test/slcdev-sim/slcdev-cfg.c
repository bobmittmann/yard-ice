#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <sys/dcclog.h>

#include "json.h"
#include "crc.h"
#include "slcdev.h"

uint16_t cfg_stack = FLASH_BLK_DEV_DB_BIN_SIZE;
uint16_t cfg_heap = 0;

struct devsim_cfg * cfg = (struct devsim_cfg *)STM32_MEM_EEPROM;

#if 0
int cmd_eeprom(FILE * f, int argc, char ** argv)
{
	unsigned int offs;
	unsigned int data;

	if (argc > 2)
		return SHELL_ERR_EXTRA_ARGS;

	fprintf(f, "Testing EEPROM...\n");
	stm32_eeprom_unlock();

	data = rand();
	for (offs = 0; offs < 4096; offs += 4) {
		stm32_eeprom_wr32(offs, data);
		data++;
	}
		
	return 0;
}
#endif

int config_dump(FILE * f)
{
	DCC_LOG(LOG_TRACE, "...");

	return 0;
}

int config_erase(void)
{
	DCC_LOG1(LOG_TRACE, "configuration size = %d.", sizeof(struct devsim_cfg));
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

#define TOK_MAX 1024

int config_compile(void)
{
	unsigned int json_crc;
	int json_len;
	jsmn_parser p;
	jsmntok_t tok[TOK_MAX];
//	struct db_obj * obj;
	jsmntok_t * t;
	char * js;
	int ret;
	int n;
	int i;

	js = (char *)(STM32_MEM_FLASH + FLASH_BLK_SIM_CFG_JSON_OFFS);
	json_len = json_root_len(js);
	json_crc = crc16ccitt(0, js, json_len);

	DCC_LOG2(LOG_TRACE, "js=0x%08x len=%d", js, json_len);
	(void)json_crc;
#if 0
	/* check database integrity */
	inf = (struct obj_db_info *)(STM32_MEM_FLASH + FLASH_BLK_DEV_DB_BIN_OFFS);
	if ((inf->len == sizeof(struct obj_db_info)) && 
		(inf->type == DB_OBJ_DB_INFO) && 
		(inf->json_crc == json_crc) && (inf->json_len == json_len)) {
		printf("Database is up-to-date.\n");
		return 0;
	}
#endif

	jsmn_init(&p);

	ret = jsmn_parse(&p, js, json_len, tok, TOK_MAX);

	if (ret == JSMN_ERROR_NOMEM) {
		DCC_LOG(LOG_ERROR, "Not enough tokens were provided!");
		ret = -JSON_ERR_NOMEM;
		goto parse_error;
	}

	if (ret == JSMN_ERROR_INVAL) {
		DCC_LOG1(LOG_ERROR, "Invalid character at: %d", p.pos);
		ret = -JSON_ERR_INVALID_CHAR;
		goto parse_error;
	}

	if (ret == JSMN_ERROR_PART) {
		DCC_LOG(LOG_ERROR, "not a full JSON packet!");
		ret = -JSON_ERR_INCOMPLETE;
		goto parse_error;
	}

	if (ret == 0) {
		DCC_LOG(LOG_ERROR, "empty JSON packet!");
		ret = -JSON_ERR_PKT_EMPTY;
		goto parse_error;
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

#if 0
	/* erase flash block */
	if (stm32_flash_erase(FLASH_BLK_DEV_DB_BIN_OFFS, 
						  FLASH_BLK_DEV_DB_BIN_SIZE) < 0) {
		DCC_LOG(LOG_ERROR, "stm32_flash_erase() failed!");
		return -1;
	};
#endif

	/* initialise PW descriptor stack */
	cfg_stack = FLASH_BLK_DEV_DB_BIN_SIZE;
	cfg_heap = 0;
	err_tok = t;

	t++;

//	obj = NULL;
	for (i = 0; i < n; i += 2) {
		char s[16];;

		if (t->type != JSMN_STRING) {
			DCC_LOG(LOG_ERROR, "object keys must be strings.");
			ret = -JSON_ERR_KEY_TYPE_INVALID;
			goto compile_error;
		}

		json_token_tostr(s, sizeof(s), js, t);
		t++;

		err_tok = t;
/*
		if (strcmp(s, "sw1") == 0) {
			ret = cfg_parse_sw(js, t, &obj);
		} else if (strcmp(s, "sw2") == 0) {
			ret = cfg_parse_sw(js, t, &obj);
		} else if (strcmp(s, "group") == 0) {
			ret = cfg_parse_group(js, t, &obj);
		} else {
			ret = -JSON_ERR_INVALID_OBJECT;
			DCC_LOG(LOG_ERROR, "invalid object.");
		}
*/
		if (ret < 0)
			goto compile_error;

		t += ret;
	}


//	cfg_info_write(json_crc, json_len, &obj);

	printf("Configuration databse compiled.\n");
	printf("Free memory: %d.%02d KiB.\n", (cfg_stack - cfg_heap) / 1024, 
		   (((cfg_stack - cfg_heap) % 1024) * 100) / 1024);

	return 0;

parse_error:
	printf("Parse error %d.\n", -ret);
	json_dump_err(js, p.pos);
	return ret;

compile_error:
	printf("Compile error %d.\n", -ret);
	json_dump_err(js, err_tok->start);
	return ret;

}


