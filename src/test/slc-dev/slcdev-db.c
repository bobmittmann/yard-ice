#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <thinkos.h>
#include <sys/dcclog.h>

#include "jsmn.h"

extern const uint8_t device_db_js[];
extern unsigned int sizeof_device_db_js;
int json_dump(FILE * f, char * js, jsmntok_t *t);

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

	json_dump(stdout, js, tok);

	return 0;
}

