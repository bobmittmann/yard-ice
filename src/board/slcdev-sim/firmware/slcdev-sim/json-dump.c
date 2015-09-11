#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/dcclog.h>

#include "json.h"

#define JSON_STR_LEN_MAX 128

int json_walk_object(FILE * f, char * js, jsmntok_t *t, int lvl)
{
	char s[JSON_STR_LEN_MAX + 1];
	int len;
	int n;
	int i;
	int j;

	if (t->type != JSMN_OBJECT) {
		DCC_LOG(LOG_ERROR, "invalid type!");
		return -1;
	}

	n = t->size;

	printf("{\n");

	if (n == 0)
		return 0;

	if (n % 2 != 0) {
		DCC_LOG(LOG_ERROR, "object must have even number of children.");
		return -1;
	}

	lvl++;

	t++;
	len = 1;

	for (i = 0; i < n; i += 2) {
		int r;

		if (t->type != JSMN_STRING) {
			DCC_LOG(LOG_ERROR, "object keys must be strings.");
			return -1;
		}

		if (i != 0)
			printf(",\n");

		for (j = 0; j < lvl; ++j)
			printf("  ");

		json_token_tostr(s, JSON_STR_LEN_MAX, js, t);
		printf("\"%s\": ", s);
		t++;
		len++;

		r = json_walk_node(f, js, t, lvl);
		if (r < 0)
			return -1;

		t += r;
		len += r;
	}

	printf("\n");

	lvl--;
	for (j = 0; j < lvl; ++j)
		printf("  ");

	printf("}");

	return len;
}

int json_walk_array(FILE * f, char * js, jsmntok_t *t, int lvl)
{
	int n;
	int i;
	int len;

	if (t->type != JSMN_ARRAY) {
		DCC_LOG(LOG_ERROR, "invalid type!");
		return -1;
	}

	n = t->size;

	if (n == 0)
		return 0;

	t++;
	len = 1;

//	printf("ARRAY: (size=%d)\n", n);
	printf("[");

	for (i = 0; i < n; ++i) {
		int r;

		if (i != 0)
			printf(", ");

		r = json_walk_node(f, js, t, lvl);
		if (r < 0)
			return -1;

		t += r;
		len += r;
	}

	printf("]");

	return len;
}

int json_walk_node(FILE * f, char * js, jsmntok_t *t, int lvl)
{
	char s[JSON_STR_LEN_MAX + 1];

	if(t->start == JSMN_NULL || t->end == JSMN_NULL) {
		DCC_LOG(LOG_ERROR, "parameter invalid!");
		return -1;
	}

	switch (t->type) {
	case JSMN_OBJECT:
		return json_walk_object(f, js, t, lvl);

	case JSMN_ARRAY:
		return json_walk_array(f, js, t, lvl);

	case JSMN_STRING:
		json_token_tostr(s, JSON_STR_LEN_MAX, js, t);
		fprintf(f, "\"%s\"", s);
		return 1;
		
	case JSMN_PRIMITIVE:
		json_token_tostr(s, JSON_STR_LEN_MAX, js, t);
		fprintf(f, "%s", s);
		return 1;
	}

	DCC_LOG(LOG_ERROR, "Invalid type");

	return -1;

}

int json_dump(FILE * f, char * js, jsmntok_t *t)
{
	int ret;

	/* Should never reach uninitialized tokens */
	if(t->start == JSMN_NULL || t->end == JSMN_NULL) {
		DCC_LOG(LOG_ERROR, "parameter invalid!");
		return -1;
	}

	if (t->type != JSMN_OBJECT) {
		DCC_LOG(LOG_ERROR, "root element must be an object.");
		return -1;
	}

	ret = json_walk_object(f, js, t, 0);

	printf("\n");

	return ret;
}

int json_parse_dump(char * js, jsmntok_t * t, void * ptr) 
{
	char s[JSON_STR_LEN_MAX + 1];

	if (t->start == JSMN_NULL || t->end == JSMN_NULL)
		return -JSON_ERR_INVALID_TOKEN;

	if (ptr == NULL)
		return -JSON_ERR_NULL_POINTER;

	switch (t->type) {
	case JSMN_OBJECT:
		return json_walk_object(stdout, js, t, 1);

	case JSMN_ARRAY:
		return json_walk_array(stdout, js, t, 1);

	case JSMN_STRING:
		json_token_tostr(s, JSON_STR_LEN_MAX, js, t);
		fprintf(stdout, "\"%s\"", s);
		return 1;
		
	case JSMN_PRIMITIVE:
		json_token_tostr(s, JSON_STR_LEN_MAX, js, t);
		fprintf(stdout, "%s", s);
		return 1;
	}

	return -JSON_ERR_INVALID_TYPE;
}

