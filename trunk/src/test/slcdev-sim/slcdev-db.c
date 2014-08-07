#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/dcclog.h>

#include "json.h"
#include "crc.h"
#include "slcdev.h"

/* 
{
	"sensor": { 
		"id" : 1, 
		"tag" : "2251TM", 
		"desc" : "Multicriteria Photoelectric Smoke Detector", 
		"pw1" : [280, 320],
		"pw2" : [
			["Not In Remote Test", 280, 320],
			["In Remote Test", 560, 640],
			["Level 6 / Maintenance Alert Enabled", 840, 960],
			["Level 6 / Maintenance Alert Disabled", 1120, 1280],
		],
		"pw3" : [560, 640],
		"pw4" : [
			["Normal Photo", 748, 854],
			["Low Chamber Trouble", 140, 150],
			["Maintenance Urgent/Drift Limit", 280, 320],
			["Maintenance Alert", 410, 485],
			["Level 1 Alarm - 1% ft", 1308, 1494],
			["Level 2 Alarm - Acclimate 1% - 2%", 1680, 1920],
			["Level 3 Alarm 3 2%/ft.", 2052, 2346,],
			["Level 4 Alarm - Acclimate 2% - 4%", 2427, 2774],
			["Level 5 Alarm 4%/ft", 2800, 3200],
			["Level 6 Alarm Thermal Alarm (60°C, 135° Heat)", 
			3250, 3600],
		],
		"pw5" : [1910, 2240]
	},
*/


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
		return -JSON_ERR_FLASH_WRITE;
	}

	/* update stack */
	db_stack = pos;

	/* check for collision */
	if (db_stack <= db_heap) {
		DCC_LOG2(LOG_ERROR, "no memory stack=%d heap=%d!", 
				db_stack, db_heap);
		return -JSON_ERR_STACK_OVERFLOW;
	}

	*ptr = (void *)(STM32_MEM_FLASH + offs);

	return len;
}

int db_heap_push(void * buf, unsigned int len, void ** ptr)
{
	uint32_t offs;
	int ret;

	offs = FLASH_BLK_DEV_DB_BIN_OFFS + db_heap;

	/* 32 bits alignement!! */
	if (len != ((len + 3) & ~3))
		DCC_LOG(LOG_WARNING, "unaligned structure size!");

	if ((ret = stm32_flash_write(offs, buf, len)) < 0) {
		DCC_LOG(LOG_WARNING, "stm32_flash_write() failed!");
		return -JSON_ERR_FLASH_WRITE;
	}

	db_heap += len; /* update heap */

	/* check for collision */
	if (db_stack <= db_heap) {
		DCC_LOG2(LOG_ERROR, "no memory stack=%d heap=%d!", 
				db_stack, db_heap);
		return -JSON_ERR_HEAP_OVERFLOW;
	}

	*ptr = (void *)(STM32_MEM_FLASH + offs);

	return len;
}


int db_parse_string(char * js, jsmntok_t * t, void * ptr) 
{
	struct json_string s;
	char buf[128];
	int ret;
	int size;

	if (t->type != JSMN_STRING)
		return -JSON_ERR_NOT_STRING;

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
	
	if ((ret = json_parse_uint16(js, t, &min)) < 0)
		return ret;

	t += ret;
	len += ret;
	cnt--;

	if (cnt > 1) {
		DCC_LOG(LOG_ERROR, "extra value in PW array");
		return -JSON_ERR_EXTRA_VALUE;
	}

	if (cnt == 1) {
		if ((ret = json_parse_uint16(js, t, &max)) < 0)
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
		if ((ret = json_parse_uint16(js, t, &ic.mode)) < 0)
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


const struct json_obj sensor_json_obj[] = {
	{ "id", json_parse_uint16, offsetof(struct obj_sensor, id) },
	{ "ap", json_parse_boolean, offsetof(struct obj_sensor, flags) },
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


const struct json_obj module_json_obj[] = {
	{ "id", json_parse_uint16, offsetof(struct obj_module, id) },
	{ "ap", json_parse_boolean, offsetof(struct obj_module, flags) },
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

int db_parse_sensor(char * js, jsmntok_t *t, struct db_obj ** objp)
{
	struct obj_sensor sensor;
	int ret;
	int cnt;

	memset(&sensor, 0, sizeof(struct obj_sensor));
	sensor.len = sizeof(struct obj_sensor);
	sensor.type = DB_OBJ_SENSOR;
	sensor.next = *objp;

	if ((cnt = json_parse_object(js, t, sensor_json_obj, &sensor)) < 0) {
		DCC_LOG(LOG_ERROR, "json_parse_object() failed!");
		return cnt;
	}

//	sensor_dump(stdout, &sensor);

	if ((ret = db_stack_push(&sensor, sizeof(struct obj_sensor), 
							(void **)objp)) < 0)
		return ret;

	DCC_LOG1(LOG_TRACE, "obj=%08x ...................", *objp);

	return cnt;
}

int db_parse_module(char * js, jsmntok_t *t, struct db_obj ** objp)
{
	struct obj_module module;
	int ret;
	int cnt;

	memset(&module, 0, sizeof(struct obj_module));
	module.len = sizeof(struct obj_module);
	module.type = DB_OBJ_MODULE;
	module.next = *objp;

	if ((cnt = json_parse_object(js, t, module_json_obj, &module)) < 0) {
		DCC_LOG(LOG_ERROR, "json_parse_object() failed!");
		return cnt;
	}

	DCC_LOG1(LOG_TRACE, "id=%d", module.id); 

//	module_dump(stdout, &module);

	if ((ret = db_stack_push(&module, sizeof(struct obj_module), 
							(void **)objp)) < 0)
		return ret;

	DCC_LOG1(LOG_TRACE, "obj=%08x ...................", *objp);

	return cnt;

}

int db_info_write(unsigned int crc, unsigned int len, 
				  struct db_obj ** objp)
{
	struct obj_db_info info;
	int ret;

	memset(&info, 0, sizeof(struct obj_db_info));
	info.len = sizeof(struct obj_db_info);
	info.type = DB_OBJ_DB_INFO;
	info.flags = 0;
	info.id = 0xaa;
	info.next = *objp;
	info.json_crc = crc;
	info.json_len = len;

//	info.obj = *objp;

	if ((ret = db_heap_push(&info, sizeof(struct obj_db_info), 
							(void **)objp)) < 0)
		return ret;

	DCC_LOG1(LOG_TRACE, "obj=%08x ...................", *objp);

	return ret;
}


#define TOK_MAX 1024

int device_db_compile(void)
{
	struct obj_db_info * inf;
	unsigned int json_crc;
	int json_len;
	jsmn_parser p;
	jsmntok_t tok[TOK_MAX];
	struct db_obj * obj;
	jsmntok_t * t;
	char * js;
	int ret;
	int n;
	int i;

	js = (char *)(STM32_MEM_FLASH + FLASH_BLK_DEV_DB_JSON_OFFS);
	json_len = json_root_len(js);
	json_crc = crc16ccitt(0, js, json_len);

	DCC_LOG2(LOG_TRACE, "js=0x%08x len=%d", js, json_len);

	/* check database integrity */
	inf = (struct obj_db_info *)(STM32_MEM_FLASH + FLASH_BLK_DEV_DB_BIN_OFFS);
	if ((inf->len == sizeof(struct obj_db_info)) && 
		(inf->type == DB_OBJ_DB_INFO) && 
		(inf->json_crc == json_crc) && (inf->json_len == json_len)) {
		printf("Database is up-to-date.\n");
		return 0;
	}

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

	/* erase flash block */
	if (stm32_flash_erase(FLASH_BLK_DEV_DB_BIN_OFFS, 
						  FLASH_BLK_DEV_DB_BIN_SIZE) < 0) {
		DCC_LOG(LOG_ERROR, "stm32_flash_erase() failed!");
		return -1;
	};

	/* initialise PW descriptor stack */
	db_stack = FLASH_BLK_DEV_DB_BIN_SIZE;
	db_heap = 0;
	err_tok = t;

	t++;

	obj = NULL;
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

		if (strcmp(s, "sensor") == 0) {
			ret = db_parse_sensor(js, t, &obj);
		} else if (strcmp(s, "module") == 0) {
			ret = db_parse_module(js, t, &obj);
		} else {
			ret = -JSON_ERR_INVALID_OBJECT;
			DCC_LOG(LOG_ERROR, "invalid object.");
		}

		if (ret < 0)
			goto compile_error;

		t += ret;
	}

	db_info_write(json_crc, json_len, &obj);

	printf("Device databse compiled.\n");
	printf("Free memory: %d.%02d KiB.\n", (db_stack - db_heap) / 1024, 
		   (((db_stack - db_heap) % 1024) * 100) / 1024);

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

static void sensor_dump(FILE * f, struct obj_sensor * sens)
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
}

static void module_dump(FILE * f, struct obj_module * mod)
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
}

static void db_info_dump(FILE * f, struct obj_db_info * inf)
{
	fprintf(f, "Device database: id=%d, json crc=0x%04x\n", inf->id, 
			inf->json_crc);
}
int device_db_dump(FILE * f)
{
	struct db_obj * obj;
	uint32_t offs = FLASH_BLK_DEV_DB_BIN_OFFS; 

	obj = (struct db_obj *)(STM32_MEM_FLASH + offs);

	while (obj->len > 0) {
		DCC_LOG2(LOG_TRACE, "obj=%08x next=%08x", obj, obj->next);
		switch (obj->type) {
		case DB_OBJ_SENSOR:
			sensor_dump(f, (struct obj_sensor *)obj);
			break;

		case DB_OBJ_MODULE:
			module_dump(f, (struct obj_module *)obj);
			break;

		case DB_OBJ_DB_INFO:
			db_info_dump(f, (struct obj_db_info *)obj);
			break;
		}

		obj = obj->next;
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

