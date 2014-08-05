#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <thinkos.h>
#include <sys/dcclog.h>

#include "jsmn.h"
#include "board.h"

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
	uint16_t pos;
	uint16_t len;
};

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
	JSON_ERR_EXTRA_VALUE,
	JSON_ERR_INVALID_OBJECT,
	JSON_ERR_STACK_OVERFLOW
};


/*
[JSON_ERR_KEY_TYPE_INVALID] = "object keys must be strings.",
*/

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
	s->pos = t->start;

	DCC_LOG2(LOG_TRACE, "idx=%d len=%d", s->pos, s->len);

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




struct obj_desc {
	char key[12];
	int (* parser)(char * js, jsmntok_t * t, void * ptr);
	unsigned int offs;	
};


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

struct pw_entry {
	uint16_t min;
	uint16_t max;
	char desc[128 - 4]; /* The description string */
};

struct pw_list {
	uint16_t cnt;
	struct pw_entry * pw[24];
};

struct ic_entry {
	uint8_t mode;
	uint8_t flags;
	char desc[128 - 2]; /* The description string */
};

void json_string_print(char * js, struct json_string * str)
{
	char s[128];
	int n;

	n = str->len;
	if (n > 127)
		n = 127;

	memcpy(s, js + str->pos, n);
	s[n] = '\0';
	printf("\"%s\"", s);
}


#define TOK_MAX 1024
#define STR_LEN_MAX 128

struct db_obj {
	uint8_t len;
	uint8_t type;
};

struct obj_module {
	uint8_t len;
	uint8_t type;
	uint8_t id;
	uint8_t ap;
	const char * tag;	
	const char * desc;	
	struct pw_list * pw1;
	struct pw_list * pw2;
	struct pw_list * pw3;
	struct pw_list * pw4;
	struct pw_list * pw5;
	struct ic_entry * ic1;
	struct ic_entry * ic2;
	struct ic_entry * ic3;
};

struct obj_sensor {
	uint8_t len;
	uint8_t type;
	uint8_t id;
	uint8_t ap;
	const char * tag;	
	const char * desc;	
	struct pw_list * pw1;
	struct pw_list * pw2;
	struct pw_list * pw3;
	struct pw_list * pw4;
	struct pw_list * pw5;
};

enum {
	DB_OBJ_SENSOR = 0,
	DB_OBJ_MODULE
};

static void pw_list_dump(FILE * f, struct pw_list * lst)
{
	int i;

	if (lst == NULL) {
		DCC_LOG(LOG_WARNING, "lst == NULL!");
		return;
	}

	DCC_LOG1(LOG_TRACE, "lst=0x%08x", lst);

	for (i = 0; i < lst->cnt; ++i) {
		struct pw_entry * pw = lst->pw[i];

		DCC_LOG2(LOG_TRACE, "lst->pw[%d]=0x%08x", i, pw);
		fprintf(f, "\t[%4d %4d] %s\n", pw->min, pw->max, pw->desc);	
	}
}

static void ic_mode_dump(FILE * f, struct ic_entry * ic)
{
	if (ic == NULL) {
		DCC_LOG(LOG_WARNING, "ic == NULL!");
		return;
	}

	DCC_LOG1(LOG_TRACE, "ic=0x%08x", ic);
	fprintf(f, "\t\"%s\": %d\n", ic->desc, ic->mode);	
}

static int sensor_dump(FILE * f, struct obj_sensor * sens)
{
	fprintf(f, "id=%d \"%s\" \"%s\"\n", sens->id, sens->tag, sens->desc);
	fprintf(f, "PW1:");
	pw_list_dump(f, sens->pw1);
	fprintf(f, "PW2:");
	pw_list_dump(f, sens->pw2);
	fprintf(f, "PW3:");
	pw_list_dump(f, sens->pw3);
	fprintf(f, "PW4:");
	pw_list_dump(f, sens->pw4);
	fprintf(f, "PW5:");
	pw_list_dump(f, sens->pw5);

	return 0;
}

static int module_dump(FILE * f, struct obj_module * mod)
{
	fprintf(f, "id=%d \"%s\" \"%s\"\n", mod->id, mod->tag, mod->desc);
	fprintf(f, "PW1:");
	pw_list_dump(f, mod->pw1);
	fprintf(f, "PW2:");
	pw_list_dump(f, mod->pw2);
	fprintf(f, "PW3:");
	pw_list_dump(f, mod->pw3);
	fprintf(f, "PW4:");
	pw_list_dump(f, mod->pw4);
	fprintf(f, "PW5:");
	pw_list_dump(f, mod->pw5);
	fprintf(f, "IC1:");
	ic_mode_dump(f, mod->ic1);
	fprintf(f, "IC2:");
	ic_mode_dump(f, mod->ic2);
	fprintf(f, "IC3:");
	ic_mode_dump(f, mod->ic3);

	return 0;
}

uint16_t db_stack = FLASH_BLK_DEV_DB_BIN_SIZE;
uint16_t db_heap = 0;

int db_stack_push(void * buf, unsigned int len, void ** ptr)
{
	uint32_t pos;
	uint32_t offs;
	int ret;

	pos = (db_stack - len) & ~3;
	offs = FLASH_BLK_DEV_DB_BIN_OFFS + pos;

	if ((ret = stm32_flash_write(offs, buf, len)) < 0) {
		DCC_LOG(LOG_WARNING, "stm32_flash_write() failed!");
		return ret;
	}

	/* update stack */
	db_stack = pos;

	/* check for collision */
	if (db_stack <= db_heap) {
		DCC_LOG2(LOG_ERROR, "no memory stack=%d heap=%d!", 
				db_stack, db_heap);
		ret = -JSON_ERR_STACK_OVERFLOW;
	}

	*ptr = (void *)(STM32_MEM_FLASH + offs);

	return ret;
}


int db_parse_string(char * js, jsmntok_t * t, void * ptr) 
{
	struct json_string s;
	char buf[128];
	int ret;
	int size;

	if (t->start == JSMN_NULL || t->end == JSMN_NULL)
		return -JSON_ERR_INVALID_TOKEN;

	if (t->type != JSMN_STRING)
		return -JSON_ERR_NOT_STRING;

	if (ptr == NULL)
		return -JSON_ERR_NULL_POINTER;

	if ((size = json_parse_string(js, t, &s)) < 0)
		return size;

	memcpy(buf, js + s.pos, s.len);
	buf[s.len] = '\0';

	if ((ret = db_stack_push(buf, s.len + 1, ptr)) < 0)
		return ret;

	return size;
}

/********************************************************************** 
 *  add a PW range into a list 
 ***********************************************************************/
static int pw_list_add(struct pw_list * lst, struct pw_entry * pw)
{
	int ret;

	if ((ret = db_stack_push(pw, 5 + strlen(pw->desc), 
							 (void **)&lst->pw[lst->cnt])) < 0)
		return ret;

	/* update list */
	lst->cnt++;

	return 0;
}

/********************************************************************** 
 * decode a single PW range
 ***********************************************************************/
int db_parse_pw(char * js, jsmntok_t * t, struct pw_entry * pw) 
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
	} else {
		s.len = 0;
		s.pos = 0;
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
	pw->min = min;
	pw->max = max;
	memcpy(pw->desc, js + s.pos, s.len);
	pw->desc[s.len] = '\0';

	return len;
}

/********************************************************************** 
 * decode a list of PW ranges
 ***********************************************************************/
int db_parse_pw_list(char * js, jsmntok_t * t, void * ptr) 
{
	struct pw_list lst;
	struct pw_entry pw;
	int cnt;
	int ret;
	int size;
	int i;

	if (t->type != JSMN_ARRAY)
		return -JSON_ERR_NOT_ARRAY;

	if ((cnt = t->size) == 0)
		return -JSON_ERR_EMPTY_ARRAY;

	if (ptr == NULL)
		return -JSON_ERR_NULL_POINTER;

	lst.cnt = 0;

	/* try to decode a single element */
	if ((size = db_parse_pw(js, t, &pw)) > 0) {
		if ((ret = pw_list_add(&lst, &pw)) < 0) {
			DCC_LOG(LOG_ERROR, "pw_list_add() failed!");
			return ret;
		}
	} else {
		/* try to decode an array of arrays */
		t++;
		size = 1;

		for (i = 0; i < cnt; ++i) {

			if ((ret = db_parse_pw(js, t, &pw)) < 0) {
				DCC_LOG(LOG_ERROR, "db_parse_pw() failed!");
				return ret;
			}

			t += ret;
			size += ret;

			if ((ret = pw_list_add(&lst, &pw)) < 0) {
				DCC_LOG(LOG_ERROR, "pw_list_add() failed!");
				return ret;
			}
		}
	}

	if ((ret = db_stack_push(&lst, 2 + lst.cnt * 
							 sizeof(struct pw_entry *), ptr)) < 0)
		return ret;

	return size;
}

int db_parse_ic_mode(char * js, jsmntok_t * t, void * ptr) 
{
	struct json_string s;
	struct ic_entry ic;
	int ret;
	int size;
	int cnt;

	if (t->type != JSMN_ARRAY)
		return -JSON_ERR_NOT_ARRAY;

	if ((cnt = t->size) == 0)
		return -JSON_ERR_EMPTY_ARRAY;

	if (ptr == NULL)
		return -JSON_ERR_NULL_POINTER;

	t++;
	size = 1;

	if (t->type == JSMN_STRING) {
		if ((ret = json_parse_string(js, t, &s)) < 0)
			return ret;
		memcpy(ic.desc, js + s.pos, s.len);
		ic.desc[s.len] = '\0';
		t += ret;
		size += ret;
		cnt--;
	} else {
		s.len = 0;
		ic.desc[0] = '\0';
	}

	if (cnt > 1) {
		DCC_LOG(LOG_ERROR, "extra value in IC array");
		return -JSON_ERR_EXTRA_VALUE;
	}

	if (cnt == 1) {
		if ((ret = json_parse_int(js, t, &ic.mode)) < 0)
			return ret;
		size++; 
	} else {
		ic.mode = 0;
	}

	DCC_LOG1(LOG_TRACE, "ic.mode=%d", ic.mode);

	if ((ret = db_stack_push(&ic, s.len + 3, ptr)) < 0)
		return ret;

	return size;
}


const struct obj_desc sensor_obj_desc[] = {
	{ "id", json_parse_int, offsetof(struct obj_sensor, id) },
	{ "ap", json_parse_boolean, offsetof(struct obj_sensor, ap) },
	{ "tag", db_parse_string, offsetof(struct obj_sensor, tag) },
	{ "desc", db_parse_string, offsetof(struct obj_sensor, desc) },
	{ "pw1", db_parse_pw_list, offsetof(struct obj_sensor, pw1) },
	{ "pw2", db_parse_pw_list, offsetof(struct obj_sensor, pw2) },
	{ "pw3", db_parse_pw_list, offsetof(struct obj_sensor, pw3) },
	{ "pw4", db_parse_pw_list, offsetof(struct obj_sensor, pw4) },
	{ "pw5", db_parse_pw_list, offsetof(struct obj_sensor, pw5) },
	{ "", NULL, 0},
};

/*	"PW1 - Reference Pulsewidth", 
	"PW2 - Class A Fix Status", 
	"PW3 - Supervisory Latch", 
	"PW4 - Supervisory (analog)", 
	"PW5 - Type ID", */


const struct obj_desc module_obj_desc[] = {
	{ "id", json_parse_int, offsetof(struct obj_module, id) },
	{ "ap", json_parse_boolean, offsetof(struct obj_module, ap) },
	{ "tag", db_parse_string, offsetof(struct obj_module, tag) },
	{ "desc", db_parse_string, offsetof(struct obj_module, desc) },
	{ "pw1", db_parse_pw_list, offsetof(struct obj_module, pw1) },
	{ "pw2", db_parse_pw_list, offsetof(struct obj_module, pw2) },
	{ "pw3", db_parse_pw_list, offsetof(struct obj_module, pw3) },
	{ "pw4", db_parse_pw_list, offsetof(struct obj_module, pw4) },
	{ "pw5", db_parse_pw_list, offsetof(struct obj_module, pw5) },
	{ "ic1", db_parse_ic_mode, offsetof(struct obj_module, ic1) },
	{ "ic2", db_parse_ic_mode, offsetof(struct obj_module, ic2) },
	{ "ic3", db_parse_ic_mode, offsetof(struct obj_module, ic3) },
	{ "", NULL, 0},
};

int json_parse_sensor(char * js, jsmntok_t *t)
{
	struct obj_sensor sensor;
	unsigned int len;
	uint32_t offs;
	int ret;
	int cnt;

	memset(&sensor, 0, sizeof(struct obj_sensor));
	sensor.len = sizeof(struct obj_sensor);
	sensor.type = DB_OBJ_SENSOR;

	if ((cnt = json_parse_object(js, t, sensor_obj_desc, &sensor)) < 0) {
		DCC_LOG(LOG_ERROR, "json_parse_object() failed!");
		return cnt;
	}

	DCC_LOG1(LOG_TRACE, "id=%d", sensor.id); 

//	sensor_dump(stdout, &sensor);

	DCC_LOG(LOG_TRACE, "1.");

	offs = FLASH_BLK_DEV_DB_BIN_OFFS + db_heap;
	len = sizeof(struct obj_sensor);

	if ((ret = stm32_flash_write(offs, &sensor, len)) < 0) {
		DCC_LOG(LOG_WARNING, "stm32_flash_write() failed!");
		return ret;
	}

	/* update heap */
	db_heap += len;
	/* check for collision */
	if (db_stack <= db_heap) {
		DCC_LOG2(LOG_ERROR, "no memory stack=%d heap=%d!", 
				db_stack, db_heap);
		return -1;
	}

	return cnt;
}

int json_parse_module(char * js, jsmntok_t *t)
{
	struct obj_module module;
	unsigned int len;
	uint32_t offs;
	int ret;
	int cnt;

	memset(&module, 0, sizeof(struct obj_module));
	module.len = sizeof(struct obj_module);
	module.type = DB_OBJ_MODULE;

	if ((cnt = json_parse_object(js, t, module_obj_desc, &module)) < 0) {
		DCC_LOG(LOG_ERROR, "json_parse_object() failed!");
		return cnt;
	}

	DCC_LOG1(LOG_TRACE, "id=%d", module.id); 

//	module_dump(stdout, &module);

	DCC_LOG(LOG_TRACE, "1.");

	offs = FLASH_BLK_DEV_DB_BIN_OFFS + db_heap;
	len = sizeof(struct obj_module);

	if ((ret = stm32_flash_write(offs, &module, len)) < 0) {
		DCC_LOG(LOG_WARNING, "stm32_flash_write() failed!");
		return ret;
	}

	DCC_LOG(LOG_TRACE, "2.");

	/* update heap */
	db_heap += len;
	/* check for collision */
	if (db_stack <= db_heap) {
		DCC_LOG2(LOG_ERROR, "no memory stack=%d heap=%d!", 
				db_stack, db_heap);
		return -1;
	}

	DCC_LOG(LOG_TRACE, "3.");

	return cnt;

}

void jsmn_dump_line(int ln, char * lp)
{
	char * cp;
	int c;

	if (lp == NULL)
		return;

	printf("%4d: ", ln);
	for (cp = lp; (c = *cp) != '\0'; ++cp) {
		if ((c == '\r') || (c == '\n'))
			break;
		printf("%c", c);
	}
	printf("\n");
}

void jsmn_dump_err(jsmn_parser * p, char * js)
{
	char * lp[5];
	int ln;
	int c;
	int i;
	int pos = p->pos;

	lp[4] = NULL;
	lp[3] = NULL;
	lp[2] = NULL;
	lp[1] = NULL;
	lp[0] = js;
	ln = 1;
	for (i = 0; (c = js[i]) != '\0'; ++i) {
		if (js[i - 1] == '\n') {
			lp[4] = lp[3];
			lp[3] = lp[2];
			lp[2] = lp[1];
			lp[1] = lp[0];
			lp[0] = &js[i];
			ln++;
		}
		if (i == pos) {
			jsmn_dump_line(ln - 4, lp[4]);
			jsmn_dump_line(ln - 3, lp[3]);
			jsmn_dump_line(ln - 2, lp[2]);
			jsmn_dump_line(ln - 1, lp[1]);
			jsmn_dump_line(ln - 0, lp[0]);
			break;
		}
	}
}

int json_len(char * js)
{
	char * cp;
	char * ep;
	int c;

	ep = js;
	for (cp = js; (c = *cp) != '\0'; ++cp) {
		if (c == '}')
			ep = cp;
	}

	return (ep - js) + 1; 
}

int device_db_compile(void)
{
	jsmn_parser p;
	jsmntok_t tok[TOK_MAX];
	jsmntok_t * t;
	char * js;
	int len;
	int ret;
	int n;
	int i;

	js = (char *)(STM32_MEM_FLASH + FLASH_BLK_DEV_DB_JSON_OFFS);
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

int device_db_dump(FILE * f)
{
	struct db_obj * obj;
	uint32_t offs = FLASH_BLK_DEV_DB_BIN_OFFS; 

	obj = (struct db_obj *)(STM32_MEM_FLASH + offs);
	while (obj->len > 0) {
		switch (obj->type) {
		case DB_OBJ_SENSOR:
			sensor_dump(f, (struct obj_sensor *)obj);
			break;

		case DB_OBJ_MODULE:
			module_dump(f, (struct obj_module *)obj);
			break;
		}

		offs += obj->len;
		obj = (struct db_obj *)(STM32_MEM_FLASH + offs);
	}

	return 0;
}

int device_db_erase(void)
{
	uint32_t blk_offs = FLASH_BLK_DEV_DB_BIN_OFFS;
	uint32_t blk_size = FLASH_BLK_DEV_DB_BIN_SIZE;
	int ret;

	if ((ret = stm32_flash_erase(blk_offs, blk_size)) < 0) {
		DCC_LOG(LOG_ERROR, "stm32_flash_erase() failed!");
		return ret;
	};

	DCC_LOG(LOG_TRACE, "device database erased!");

	return ret;
}


