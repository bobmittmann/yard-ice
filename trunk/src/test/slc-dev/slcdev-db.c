#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <thinkos.h>
#include <sys/dcclog.h>

#include "jsmn.h"
#include "board.h"

char * json_token_tostr(char *js, jsmntok_t *t);
int json_walk_node(FILE * f, char * js, jsmntok_t *t, int lvl);

#define JSON_STR_LEN_MAX 128

char * json_string(char * s, unsigned int max, char * js, jsmntok_t *t)
{
	int n;

	n = t->end - t->start;
	if (n > (max - 1))
		n = (max - 1);

	memcpy(s, js + t->start, n);
	s[n] = '\0';

	return s;
}

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


int device_db_dump(FILE * f)
{
	jsmn_parser p;
	jsmntok_t tok[TOK_MAX];
	jsmntok_t * t;
	char * js = (char *)device_db_js;
	int len = sizeof_device_db_js;
	int r;
	int n;
	int i;


	jsmn_init(&p);

	r = jsmn_parse(&p, js, len, tok, TOK_MAX);

	if (r == JSMN_ERROR_NOMEM) {
		fprintf(f, "Err: Not enough tokens were provided!\n");
		DCC_LOG(LOG_ERROR, "Not enough tokens were provided!");
		return 1;
	}

	if (r == JSMN_ERROR_INVAL) {
		fprintf(f, "Err: Invalid character!\n");
		DCC_LOG(LOG_ERROR, "Invalid character!");
		return 1;
	}

	if (r == JSMN_ERROR_PART) {
		fprintf(f, "Err: not a full JSON packet!\n");
		DCC_LOG(LOG_ERROR, "not a full JSON packet!");
		return 1;
	}

	if (r == 0) {
		fprintf(f, "Warn: empty JSON packet!\n");
		DCC_LOG(LOG_ERROR, "empty JSON packet!");
		return 1;
	}

	t = tok;

	/* Should never reach uninitialized tokens */
	if (t->start == JSMN_NULL || t->end == JSMN_NULL) {
		DCC_LOG(LOG_ERROR, "parameter invalid!");
		return -1;
	}

	if (t->type != JSMN_OBJECT) {
		DCC_LOG(LOG_ERROR, "root element must be an object.");
		return -1;
	}

	n = t->size;

	if (n % 2 != 0) {
		DCC_LOG(LOG_ERROR, "object must have even number of children.");
		return -1;
	}

	if (n == 0)
		return 0;

	DCC_LOG1(LOG_TRACE, "n=%d", n);

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
//			db_decode_sensor(f, js, t, 0);
			r = json_walk_node(f, js, t, 0);
		} else if (strcmp(s, "module") == 0) {
//			db_decode_module(f, js, t, 0);
			r = json_walk_node(f, js, t, 0);
		} else {
			fprintf(f, "invalid object: \"%s\".\n", s);
			return -1;
		}


		if (r < 0) {
			DCC_LOG(LOG_ERROR, "json_walk_node() failed.");
			return r;
		}

		t += r;
	}

	printf("\n");

	return 0;
}

