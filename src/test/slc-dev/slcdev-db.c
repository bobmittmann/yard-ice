#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <thinkos.h>
#include <sys/dcclog.h>

#include "jsmn.h"
#include "board.h"

extern const uint8_t device_db_js[];
extern unsigned int sizeof_device_db_js;
int json_dump(FILE * f, char * js, jsmntok_t *t);

struct string {
	uint16_t idx;
	uint16_t len;
};

struct pw_range {
	uint16_t min;
	uint16_t max;
};

struct pw_list {
	uint16_t ptr;
	uint16_t len;
};

struct pw_entry {
	struct string desc;
	struct pw_range range;
};

struct dev_entry {
	uint8_t id;
	uint8_t flags;
	char tag[12];        /* 16 */
	struct pw_range pw1; /* 4 */
	struct pw_list  pw2; /* 4 */
	struct pw_range pw3; /* 4 */
	struct pw_list  pw4; /* 4 */
	struct pw_range pw5; /* 4 */
	uint32_t res[3];     /* 12 */
};

struct pw_entry * pw_list_ptr;

int pw_list_write(struct pw_entry * list, unsigned int len)
{
	int ptr = 0;

	return ptr;
}

/*
	"type": { 
		"id" : 1, 
		"tag" : "2251TM", 
		"desc" : "Multicriteria Photoelectric Smoke Detector", 
		"pw1" : [280, 320],
		"pw2" : [
			{"Not In Remote Test": [280, 320]},
			{"In Remote Test": [560, 640]},
			{"Level 6 / Maintenance Alert Enabled": [840, 960]},
			{"Level 6 / Maintenance Alert Disabled": [1120, 1280]},
		],
		"pw3" : [560, 640],
		"pw4" : [
			{"Normal Photo": [748, 854]},
			{"Low Chamber Trouble": [140, 150]},
			{"Maintenance Urgent/Drift Limit": [280, 320]},
			{"Maintenance Alert": [410, 485]},
			{"Level 1 Alarm - 1% ft": [1308, 1494]},
			{"Level 2 Alarm - Acclimate 1% - 2%": [1680, 1920]},
			{"Level 3 Alarm 3 – 2%/ft.": [2052, 2346,]},
			{"Level 4 Alarm - Acclimate 2% - 4%": [2427, 2774]},
			{"Level 5 Alarm 4%/ft": [2800, 3200]},
			{"Level 6 Alarm – Thermal Alarm (60°C, 135° Heat)": 
				[3250, 3600]},
		],
		"pw5" : [1910, 2240]
	},
*/

int device_db_erase(void)
{
	uint32_t blk_offs = FLASH_BLK_DEV_DB_BIN_OFFS;
	uint32_t blk_size = FLASH_BLK_DEV_DB_BIN_SIZE;

	if (stm32_flash_erase(blk_offs, blk_size) < 0) {
		DCC_LOG(LOG_ERROR, "stm32_flash_erase() failed!");
		return -1;
	};

	return 0;
}

int device_db_compile(void)
{
	uint32_t blk_offs = FLASH_BLK_DEV_DB_BIN_OFFS;
	uint32_t blk_size = FLASH_BLK_DEV_DB_BIN_SIZE;

	if (stm32_flash_erase(blk_offs, blk_size) < 0) {
		DCC_LOG(LOG_ERROR, "stm32_flash_erase() failed!");
		return -1;
	};

	return 0;
}


#define TOK_MAX 1024
#define STR_LEN_MAX 128

int device_db_init(void)
{
	jsmn_parser p;
	jsmntok_t tok[TOK_MAX];
	char * js = (char *)device_db_js;
	int len = sizeof_device_db_js;
	int r;

	jsmn_init(&p);

	r = jsmn_parse(&p, js, len, tok, TOK_MAX);

	if (r == JSMN_ERROR_NOMEM) {
//		fprintf(stderr, "Err: Not enough tokens were provided!\n");
		DCC_LOG(LOG_ERROR, "Not enough tokens were provided!");
		return 1;
	}

	if (r == JSMN_ERROR_INVAL) {
//		fprintf(stderr, "Err: Invalid character!\n");
		DCC_LOG(LOG_ERROR, "Invalid character!");
		return 1;
	}

	if (r == JSMN_ERROR_PART) {
//		fprintf(stderr, "Err: not a full JSON packet!\n");
		DCC_LOG(LOG_ERROR, "not a full JSON packet!");
		return 1;
	}

	if (r == 0) {
//		fprintf(stderr, "Warn: empty JSON packet!\n");
		DCC_LOG(LOG_ERROR, "empty JSON packet!");
		return 1;
	}

#if 0
		if (json_traverse(js, tok) <= 0) {
			fprintf(stderr, "Err: tree traversal failed!\n");
			return 1;
		}
#endif

//	json_dump(stdout, js, tok);

	return 0;
}

