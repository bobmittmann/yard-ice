#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <thinkos.h>
#include <sys/dcclog.h>

#include "jsmn.h"
#include "board.h"

char * json_token_tostr(char *js, jsmntok_t *t);
int json_walk_node(FILE * f, char * js, jsmntok_t *t, int lvl);
int json_walk_object(FILE * f, char * js, jsmntok_t *t, int lvl);
int json_walk_array(FILE * f, char * js, jsmntok_t *t, int lvl);

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

struct json_string {
	uint16_t idx;
	uint16_t len;
};

extern const uint8_t device_db_js[];
extern unsigned int sizeof_device_db_js;
int json_dump(FILE * f, char * js, jsmntok_t *t);

enum {
	JSON_ERR_GENERAL = 1,
	JSON_ERR_INVALID_TOKEN,
	JSON_ERR_INVALID_TYPE,
	JSON_ERR_NOT_PRIMITIVE,
	JSON_ERR_NOT_STRING,
	JSON_ERR_NOT_OBJECT,
	JSON_ERR_NOT_ARRAY,
	JSON_ERR_EMPTY_OBJECT,
	JSON_ERR_EMPTY_ARRAY,
	JSON_ERR_NUM_CHILDREN,
	JSON_ERR_NOMEM,
	JSON_ERR_INVALID_CHAR,
	JSON_ERR_INCOMPLETE,
	JSON_ERR_PKT_EMPTY,
	JSON_ERR_KEY_TYPE_INVALID,
	JSON_ERR_KEY_NAME_INVALID,
	JSON_ERR_NULL_POINTER,
	JSON_ERR_INVALID_BOOLEAN,
	JSON_ERR_MISSING_VALUE,
	JSON_ERR_EXTRA_VALUE
};


/*
[JSON_ERR_KEY_TYPE_INVALID] = "object keys must be strings.",
*/

struct obj_desc {
	char key[12];
	int (* parser)(char * js, jsmntok_t * t, void * ptr);
	unsigned int offs;	
};


int json_parse_int(char * js, jsmntok_t * t, void * ptr)
{
	int * pval = (int *)ptr;
	char s[16];

	if (t->start == JSMN_NULL || t->end == JSMN_NULL)
		return -JSON_ERR_INVALID_TOKEN;

	if (t->type != JSMN_PRIMITIVE)
		return -JSON_ERR_NOT_PRIMITIVE;

	if (pval == NULL)
		return -JSON_ERR_NULL_POINTER;

	json_string(s, sizeof(s), js, t);

	*pval = strtol(s, NULL, 10); 

	DCC_LOG2(LOG_TRACE, "val=%d ptr=%08x", *pval, pval);

	return 1;
}

int json_parse_boolean(char * js, jsmntok_t * t, void * ptr)
{
	bool * pval = (bool *)ptr;
	char s[16];

	if (t->start == JSMN_NULL || t->end == JSMN_NULL)
		return -JSON_ERR_INVALID_TOKEN;

	if (t->type != JSMN_PRIMITIVE)
		return -JSON_ERR_NOT_PRIMITIVE;

	if (pval == NULL)
		return -JSON_ERR_NULL_POINTER;

	json_string(s, sizeof(s), js, t);

	if (strcmp(s, "true") == 0) 
		*pval = true;
	else
		if (strcmp(s, "false") == 0) 
			*pval = false;
		else
			return -JSON_ERR_INVALID_BOOLEAN;

	DCC_LOG1(LOG_TRACE, "val=%d", *pval);

	return 1;
}

int json_parse_string(char * js, jsmntok_t * t, void * ptr) 
{
	struct json_string * s = (struct json_string *)ptr;

	if (t->start == JSMN_NULL || t->end == JSMN_NULL)
		return -JSON_ERR_INVALID_TOKEN;

	if (t->type != JSMN_STRING)
		return -JSON_ERR_NOT_STRING;

	if (s == NULL)
		return -JSON_ERR_NULL_POINTER;

	s->len = t->end - t->start;
	s->idx = t->start;

	DCC_LOG2(LOG_TRACE, "idx=%d len=%d", s->idx, s->len);

	return 1;
}

int json_parse_any(char * js, jsmntok_t * t, void * ptr) 
{
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
		printf("\"%s\"", json_token_tostr(js, t));
		return 1;
		
	case JSMN_PRIMITIVE:
		printf("%s", json_token_tostr(js, t));
		return 1;
	}

	return -JSON_ERR_INVALID_TYPE;
}


int json_parse_object(char * js, jsmntok_t * t, 
					  const struct obj_desc * desc, void * obj)
{
//	uint8_t * ptr = (uint8_t * ptr);
	int len;
	int n;
	int i;
	int j;

	if (t->start == JSMN_NULL || t->end == JSMN_NULL)
		return -JSON_ERR_INVALID_TOKEN;

	if (t->type != JSMN_OBJECT)
		return -JSON_ERR_NOT_OBJECT;

	if ((n = t->size) == 0)
		return -JSON_ERR_EMPTY_OBJECT;

	if (n % 2 != 0)
		return -JSON_ERR_NUM_CHILDREN;

	t++;
	len = 1;

	for (i = 0; i < n; i += 2) {
		char s[16];
		int r;

		if (t->type != JSMN_STRING)
			return -JSON_ERR_KEY_TYPE_INVALID;

		json_string(s, sizeof(s), js, t);

		t++;
		len++;

		for (j = 0; ; ++j) {
			if (desc[j].parser == NULL) {
				DCC_LOG(LOG_ERROR, "invalid key");
				return -JSON_ERR_KEY_NAME_INVALID;
			}
				
			if (strcmp(s, desc[j].key) == 0)
				break;
		}

		DCC_LOG3(LOG_TRACE, "obj=%08x offs=%d ptr=%08x", 
				 obj, desc[j].offs, obj + desc[j].offs); 

		r = desc[j].parser(js, t, obj + desc[j].offs);

		if (r < 0)
			return r;

		t += r;
		len += r;
	}

	return len;
}

struct pw_range {
	uint16_t min;
	uint16_t max;
};

struct pw_list {
	uint16_t ptr;
	uint16_t len;
};

struct pw_entry {
	struct json_string desc;
	struct pw_range range;
};

struct ic_mode {
	struct json_string desc;
	int mode;
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


void json_string_print(char * js, struct json_string * str)
{
	char s[128];
	int n;

	n = str->len;
	if (n > 127)
		n = 127;

	memcpy(s, js + str->idx, n);
	s[n] = '\0';
	printf("\"%s\"", s);
}


struct pw_entry * pw_list_ptr;

int pw_list_write(struct pw_entry * list, unsigned int len)
{
	int ptr = 0;

	return ptr;
}

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

int json_parse_pw(char * js, jsmntok_t * t, struct pw_list * pw) 
{

	struct json_string s;
	int min;
	int max;
	int ret;
	int len;
	int cnt;

	if (t->type != JSMN_ARRAY)
		return -JSON_ERR_NOT_ARRAY;

	if ((cnt = t->size) == 0)
		return -JSON_ERR_EMPTY_ARRAY;

	t++;
	len = 1;

	if (t->type == JSMN_STRING) {
		if ((ret = json_parse_string(js, t, &s)) < 0)
			return ret;
		t += ret;
		len += ret;
		cnt--;
	}

	if (cnt == 0) {
		DCC_LOG(LOG_ERROR, "missing PW value");
		return -JSON_ERR_MISSING_VALUE;
	} 
	
	if ((ret = json_parse_int(js, t, &min)) < 0)
		return ret;

	t += ret;
	len += ret;
	cnt--;

	if (cnt > 1) {
		DCC_LOG(LOG_ERROR, "extra value in PW array");
		return -JSON_ERR_EXTRA_VALUE;
	}

	if (cnt == 1) {
		if ((ret = json_parse_int(js, t, &max)) < 0)
			return ret;
		len++; 
	} else {
		max = min;
	}

	DCC_LOG2(LOG_TRACE, "min=%d max=%d", min, max);

	return len;
}

int json_parse_ic_mode(char * js, jsmntok_t * t, void * ptr) 
{
	struct ic_mode * ic  = (struct ic_mode *)ptr;
	int ret;
	int len;
	int cnt;

	if (t->type != JSMN_ARRAY)
		return -JSON_ERR_NOT_ARRAY;

	if ((cnt = t->size) == 0)
		return -JSON_ERR_EMPTY_ARRAY;

	t++;
	len = 1;

	if (t->type == JSMN_STRING) {
		if ((ret = json_parse_string(js, t, &ic->desc)) < 0)
			return ret;
		t += ret;
		len += ret;
		cnt--;
	}

	if (cnt > 1) {
		DCC_LOG(LOG_ERROR, "extra value in IC array");
		return -JSON_ERR_EXTRA_VALUE;
	}

	if (cnt == 1) {
		if ((ret = json_parse_int(js, t, &ic->mode)) < 0)
			return ret;
		len++; 
	} else {
		ic->mode = 0;
	}

	DCC_LOG1(LOG_TRACE, "ic.mode=%d", ic->mode);

	return len;
}

int json_parse_pw_list(char * js, jsmntok_t * t, void * ptr) 
{
	struct pw_list * pw  = (struct pw_list *)ptr;
	int cnt;
	int len;
	int ret;
	int i;

	if (t->type != JSMN_ARRAY)
		return -JSON_ERR_NOT_ARRAY;

	if ((cnt = t->size) == 0)
		return -JSON_ERR_EMPTY_ARRAY;

	/* try to decode a single element */
	if ((ret = json_parse_pw(js, t, pw)) > 0) {
		return ret;
	}

	t++;
	len = 1;

	for (i = 0; i < cnt; ++i) {

		if ((ret = json_parse_pw(js, t, pw)) < 0) {
			DCC_LOG(LOG_ERROR, "json_parse_pw() failed!");
			return -1;
		}

		t += ret;
		len += ret;
		pw++;
	}

	return len;
}

struct obj_sensor {
	int id;
	bool ap;
	struct json_string tag;
	struct json_string desc;
	struct pw_list pw1;
	struct pw_list pw2;
	struct pw_list pw3;
	struct pw_list pw4;
	struct pw_list pw5;
};

const struct obj_desc sensor_obj_desc[] = {
	{ "id", json_parse_int, offsetof(struct obj_sensor, id) },
	{ "ap", json_parse_boolean, offsetof(struct obj_sensor, ap) },
	{ "tag", json_parse_string, offsetof(struct obj_sensor, tag) },
	{ "desc", json_parse_string, offsetof(struct obj_sensor, desc) },
	{ "pw1", json_parse_pw_list, offsetof(struct obj_sensor, pw1) },
	{ "pw2", json_parse_pw_list, offsetof(struct obj_sensor, pw2) },
	{ "pw3", json_parse_pw_list, offsetof(struct obj_sensor, pw3) },
	{ "pw4", json_parse_pw_list, offsetof(struct obj_sensor, pw4) },
	{ "pw5", json_parse_pw_list, offsetof(struct obj_sensor, pw5) },
	{ "", NULL, 0},
};

struct obj_module {
	int id;
	bool ap;
	struct json_string tag;
	struct json_string desc;
	struct pw_list pw1;
	struct pw_list pw2;
	struct pw_list pw3;
	struct pw_list pw4;
	struct pw_list pw5;
	struct ic_mode ic1;
	struct ic_mode ic2;
	struct ic_mode ic3;
};
/*	"PW1 - Reference Pulsewidth", 
	"PW2 - Class A Fix Status", 
	"PW3 - Supervisory Latch", 
	"PW4 - Supervisory (analog)", 
	"PW5 - Type ID", */


const struct obj_desc module_obj_desc[] = {
	{ "id", json_parse_int, offsetof(struct obj_module, id) },
	{ "ap", json_parse_boolean, offsetof(struct obj_module, ap) },
	{ "tag", json_parse_string, offsetof(struct obj_module, tag) },
	{ "desc", json_parse_string, offsetof(struct obj_module, desc) },
	{ "pw1", json_parse_pw_list, offsetof(struct obj_module, pw1) },
	{ "pw2", json_parse_pw_list, offsetof(struct obj_module, pw2) },
	{ "pw3", json_parse_pw_list, offsetof(struct obj_module, pw3) },
	{ "pw4", json_parse_pw_list, offsetof(struct obj_module, pw4) },
	{ "pw5", json_parse_pw_list, offsetof(struct obj_module, pw5) },
	{ "ic1", json_parse_ic_mode, offsetof(struct obj_module, ic1) },
	{ "ic2", json_parse_ic_mode, offsetof(struct obj_module, ic2) },
	{ "ic3", json_parse_ic_mode, offsetof(struct obj_module, ic3) },
	{ "", NULL, 0},
};


int json_parse_sensor(char * js, jsmntok_t *t)
{
	struct obj_sensor sensor;
	int ret;

	ret = json_parse_object(js, t, sensor_obj_desc, &sensor);

	if (ret < 0) {
		DCC_LOG(LOG_ERROR, "json_parse_object() failed!");
		return ret;
	}

	DCC_LOG1(LOG_TRACE, "id=%d", sensor.id); 

	printf("id=%d ", sensor.id); 
	json_string_print(js, &sensor.tag);
	printf(" "); 
	json_string_print(js, &sensor.desc);
	printf("\n"); 

	return ret;
}

int json_parse_module(char * js, jsmntok_t *t)
{
	struct obj_module module;
	int ret;

	ret = json_parse_object(js, t, module_obj_desc, &module);

	if (ret < 0) {
		DCC_LOG(LOG_ERROR, "json_parse_object() failed!");
		return ret;
	}

	DCC_LOG1(LOG_TRACE, "id=%d", module.id); 

	printf("id=%d ", module.id); 
	json_string_print(js, &module.tag);
	printf(" "); 
	json_string_print(js, &module.desc);
	printf("\n"); 

	return ret;
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
		return -JSON_ERR_NOMEM;
	}

	if (r == JSMN_ERROR_INVAL) {
		fprintf(f, "Err: Invalid character!\n");
		DCC_LOG(LOG_ERROR, "Invalid character!");
		return -JSON_ERR_INVALID_CHAR;
	}

	if (r == JSMN_ERROR_PART) {
		fprintf(f, "Err: not a full JSON packet!\n");
		DCC_LOG(LOG_ERROR, "not a full JSON packet!");
		return -JSON_ERR_INCOMPLETE;
	}

	if (r == 0) {
		fprintf(f, "Warn: empty JSON packet!\n");
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
			printf("\n- sensor ---------------\n");
			r = json_parse_sensor(js, t);
		} else if (strcmp(s, "module") == 0) {
//			db_decode_module(f, js, t, 0);
			printf("\n- module ---------------\n");
			r = json_parse_module(js, t);
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

