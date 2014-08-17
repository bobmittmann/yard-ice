#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/dcclog.h>

#include <microjs.h>
#include <thinkos.h>

#include "crc.h"
#include "slcdev.h"

/* 
{
	"sensor": { 
		"id" : 1, 
		"model" : "2251TM", 
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
		return -1;
	}

	/* update stack */
	db_stack = pos;

	/* check for collision */
	if (db_stack <= db_heap) {
		DCC_LOG2(LOG_ERROR, "no memory stack=%d heap=%d!", 
				db_stack, db_heap);
		return -1;
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
		return -1;
	}

	db_heap += len; /* update heap */

	/* check for collision */
	if (db_stack <= db_heap) {
		DCC_LOG2(LOG_ERROR, "no memory stack=%d heap=%d!", 
				db_stack, db_heap);
		return -1;
	}

	*ptr = (void *)(STM32_MEM_FLASH + offs);

	return len;
}

#if 0
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
		t += ret;
		size += ret;
		cnt--;
	} else {
		s.pos = 0;
		s.len = 0;
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

	if ((ret = const_str_write(js + s.pos, s.len)) < 0)
		return ret;

	if ((ret = db_stack_push(&ic, 3, ptr)) < 0)
		return ret;

	return size;
}
#endif

#define DB_DEV_TYPE_MAX 32

int db_info_write(unsigned int crc, unsigned int len, 
				  struct db_obj ** objp)
{

	uint32_t buf[(sizeof(struct obj_db_info) + 
		DB_DEV_TYPE_MAX * sizeof(struct db_obj *)) / sizeof(uint32_t)];
	struct obj_db_info * info = (struct obj_db_info *)buf;
	struct db_obj * obj;
	int ret;
	int cnt;

	memset(info, 0, sizeof(struct obj_db_info));

	cnt = 0;
	/* collect all sesnsors */
	obj = (struct db_obj *)*objp;
	while (obj != NULL) {
		if (obj->type == DB_OBJ_SENSOR) 
			info->obj[cnt++] = obj;
		obj = obj->next;
	}
			
	/* collect all modules */
	obj = (struct db_obj *)*objp;
	while (obj != NULL) {
		if (obj->type == DB_OBJ_MODULE) 
			info->obj[cnt++] = obj;
		obj = obj->next;
	}
			
	info->len = sizeof(struct obj_db_info) + cnt * sizeof(struct db_obj *);
	info->type = DB_OBJ_DB_INFO;
	info->flags = 0;
	info->id = 0xaa;
	info->next = *objp;
	info->json_crc = crc;
	info->json_len = len;
	info->obj_cnt = cnt;

	if ((ret = db_heap_push(info, info->len, (void **)objp)) < 0)
		return ret;

	DCC_LOG1(LOG_TRACE, "obj=%08x ...................", *objp);

	return ret;
}

/********************************************************************** 
 * encode a single PW range
 ***********************************************************************/

int db_pw_enc(struct microjs_json_parser * jsn, 
			  int typ, struct microjs_val * val, 
			  struct pw_entry * pw)
{
	int desc = 0;
	uint16_t min;
	uint16_t max;
	int ret;

	if (typ == MICROJS_JSON_STRING) {
		DCC_LOG(LOG_MSG, "string!");
		if ((ret = const_str_write(val->str.dat, val->str.len)) < 0)
			return ret;
		desc = ret;
		typ = microjs_json_get_val(jsn, val);
	} 
	
	if (typ != MICROJS_JSON_INTEGER) {
		DCC_LOG(LOG_ERROR, "expecting integer!");
		return -1;
	}

	min = val->u32;
	max = min;

	DCC_LOG1(LOG_MSG, "min=%d", min);

	if ((typ = microjs_json_get_val(jsn, val)) == MICROJS_JSON_INTEGER) {
		max = val->u32;
		DCC_LOG1(LOG_MSG, "max=%d", max);
		typ = microjs_json_get_val(jsn, val);
	}

	if (typ != MICROJS_JSON_END_ARRAY) {
		DCC_LOG(LOG_ERROR, "expecting array closing!");
		return -1;
	}

	pw->min = min;
	pw->max = max;
	pw->desc = desc;

	return 0;
}


/********************************************************************** 
 * encode a list of PW ranges
 ***********************************************************************/

int db_pw_list_enc(struct microjs_json_parser * jsn, 
				   struct microjs_val * val, 
				   unsigned int opt, void * ptr)
{
	struct pw_list lst;
	struct pw_entry * pw;
	int ret;
	int typ;

	lst.cnt = 0;
	pw = &lst.pw[0];

	if ((typ = microjs_json_get_val(jsn, val)) == MICROJS_JSON_ARRAY) {
		/* list of PWs */
		do {
			DCC_LOG1(LOG_MSG, "PW[%d]", lst.cnt);
			typ = microjs_json_get_val(jsn, val);
			if ((ret = db_pw_enc(jsn, typ, val, pw)) < 0) {
				DCC_LOG(LOG_ERROR, "db_parse_pw() failed!");
				return ret;
			}
			DCC_LOG3(LOG_TRACE, "PW[%d]: min=%d max=%d", 
					 lst.cnt, pw->min, pw->max);
			lst.cnt++;
			pw++;
		} while ((typ = microjs_json_get_val(jsn, val)) == MICROJS_JSON_ARRAY);
	
		if (typ != MICROJS_JSON_END_ARRAY) {
			DCC_LOG(LOG_ERROR, "expecting array closing!");
			return -1;
		}
	} else {
		/* single PW */
		if ((ret = db_pw_enc(jsn, typ, val, pw)) < 0) {
			DCC_LOG(LOG_ERROR, "db_parse_pw() failed!");
			return ret;
		}
		DCC_LOG2(LOG_TRACE, "PW: min=%d max=%d", pw->min, pw->max);
		lst.cnt++;
		pw++;
	} 

	return db_stack_push(&lst, sizeof(uint32_t) + lst.cnt * 
						 sizeof(struct pw_entry), ptr);
}


static const struct microjs_attr_desc sensor_desc[] = {
	{ "id", MICROJS_JSON_INTEGER, 0, offsetof(struct obj_module, id),
		microjs_u16_enc },
	{ "ap", MICROJS_JSON_BOOLEAN, 0, offsetof(struct obj_module, flags),
		microjs_bit_enc },
	{ "model", MICROJS_JSON_STRING, 0, offsetof(struct obj_module, model),
		microjs_const_str_enc },
	{ "desc", MICROJS_JSON_STRING, 0, offsetof(struct obj_module, desc),
		microjs_const_str_enc },
	{ "pw1", MICROJS_JSON_ARRAY, 0, offsetof(struct obj_module, pw1),
		db_pw_list_enc},
	{ "pw2", MICROJS_JSON_ARRAY, 0, offsetof(struct obj_module, pw2),
		db_pw_list_enc},
	{ "pw3", MICROJS_JSON_ARRAY, 0, offsetof(struct obj_module, pw3),
		db_pw_list_enc},
	{ "pw4", MICROJS_JSON_ARRAY, 0, offsetof(struct obj_module, pw4),
		db_pw_list_enc},
	{ "pw5", MICROJS_JSON_ARRAY, 0, offsetof(struct obj_module, pw5),
		db_pw_list_enc},
	{ "", 0, 0, 0, NULL},
};


/* Encode a sensor list */
int db_sensor_enc(struct microjs_json_parser * jsn, 
				   struct microjs_val * val, 
				   unsigned int opt, void * ptr)
{
	struct db_obj ** objp = (struct db_obj **)ptr;
	struct obj_sensor sensor;
	int ret;

	memset(&sensor, 0, sizeof(struct obj_sensor));
	sensor.len = sizeof(struct obj_sensor);
	sensor.type = DB_OBJ_SENSOR;
	sensor.next = *objp;

	if ((ret = microjs_json_parse_obj(jsn, sensor_desc, &sensor)) < 0) {
		DCC_LOG(LOG_ERROR, "microjs_json_parse_obj() failed!");
		return ret;
	}

	return db_stack_push(&sensor, sizeof(struct obj_sensor), (void **)objp);

	DCC_LOG1(LOG_TRACE, "obj=%08x ...................", *objp);

}

static const struct microjs_attr_desc module_desc[] = {
	{ "id", MICROJS_JSON_INTEGER, 0, offsetof(struct obj_module, id),
		microjs_u16_enc },
	{ "ap", MICROJS_JSON_BOOLEAN, 0, offsetof(struct obj_module, flags),
		microjs_bit_enc },
	{ "model", MICROJS_JSON_STRING, 0, offsetof(struct obj_module, model),
		microjs_const_str_enc },
	{ "desc", MICROJS_JSON_STRING, 0, offsetof(struct obj_module, desc),
		microjs_const_str_enc },
	{ "pw1", MICROJS_JSON_ARRAY, 0, offsetof(struct obj_module, pw1),
		db_pw_list_enc},
	{ "pw2", MICROJS_JSON_ARRAY, 0, offsetof(struct obj_module, pw2),
		db_pw_list_enc},
	{ "pw3", MICROJS_JSON_ARRAY, 0, offsetof(struct obj_module, pw3),
		db_pw_list_enc},
	{ "pw4", MICROJS_JSON_ARRAY, 0, offsetof(struct obj_module, pw4),
		db_pw_list_enc},
	{ "pw5", MICROJS_JSON_ARRAY, 0, offsetof(struct obj_module, pw5),
		db_pw_list_enc},
	{ "", 0, 0, 0, NULL},
};


/* Encode a module list */
int db_module_enc(struct microjs_json_parser * jsn, 
				   struct microjs_val * val, 
				   unsigned int bit, void * ptr)
{
	struct db_obj ** objp = (struct db_obj **)ptr;
	struct obj_module module;
	int ret;

	memset(&module, 0, sizeof(struct obj_module));
	module.len = sizeof(struct obj_module);
	module.type = DB_OBJ_MODULE;
	module.next = *objp;

	if ((ret = microjs_json_parse_obj(jsn, module_desc, &module)) < 0) {
		DCC_LOG(LOG_ERROR, "microjs_json_parse_obj() failed!");
		return ret;
	}

	return db_stack_push(&module, sizeof(struct obj_module), (void **)objp);
}

static const struct microjs_attr_desc db_desc[] = {
	{ "sensor", MICROJS_JSON_OBJECT, 0, 0, db_sensor_enc },
	{ "module", MICROJS_JSON_OBJECT, 0, 0, db_module_enc },
	{ "", 0, 0, 0, NULL},
};

#define JS_TOK_BUF_MAX (4096)

int device_db_compile(void)
{
	struct microjs_json_parser jsn;
	struct microjs_tokenizer tkn;
	uint8_t tok_buf[JS_TOK_BUF_MAX];
	struct obj_db_info * inf;
	unsigned int json_crc;
	struct db_obj * obj;
	int json_len;
	char * js;
	int ret;

	DCC_LOG1(LOG_TRACE, "sp=0x%08x ..........................", cm3_sp_get());

	js = (char *)(STM32_MEM_FLASH + FLASH_BLK_DEV_DB_JSON_OFFS);

	json_len = microjs_json_root_len(js);
	json_crc = crc16ccitt(0, js, json_len);
	(void)json_crc;

	DCC_LOG3(LOG_TRACE, "js=0x%08x len=%d crc=0x%04x", 
			 js, json_len, json_crc);

	/* check database integrity */
	inf = (struct obj_db_info *)(STM32_MEM_FLASH + FLASH_BLK_DEV_DB_BIN_OFFS);
	if ((inf->len == sizeof(struct obj_db_info)) && 
		(inf->type == DB_OBJ_DB_INFO) && 
		(inf->json_crc == json_crc) && (inf->json_len == json_len)) {
		printf("Database is up-to-date.\n");
		return 0;
	}

	DCC_LOG(LOG_TRACE, "1. parsing JSON file.");

	microjs_tok_init(&tkn, tok_buf, JS_TOK_BUF_MAX);

	/* parse the JASON file with the microjs tokenizer */
	if ((ret = microjs_tokenize(&tkn, js, json_len)) < 0) {
		DCC_LOG(LOG_ERROR, "microjs_parse() failed!");
		return ret;
	}

	DCC_LOG(LOG_TRACE, "2. erasing database.");
	/* erase flash block */
	if (stm32_flash_erase(FLASH_BLK_DEV_DB_BIN_OFFS, 
						  FLASH_BLK_DEV_DB_BIN_SIZE) < 0) {
		DCC_LOG(LOG_ERROR, "stm32_flash_erase() failed!");
		return -1;
	};

	DCC_LOG(LOG_TRACE, "3. erasing constant strings pool.");
	if (slcdev_const_str_purge() < 0) {
		DCC_LOG(LOG_ERROR, "slcdev_const_str_purge() failed!");
		return -1;
	};

	DCC_LOG(LOG_TRACE, "4. compiling database.");

	microjs_json_init(&jsn, &tkn);

	if (microjs_json_get_val(&jsn, NULL) != MICROJS_JSON_OBJECT) {
		DCC_LOG(LOG_ERROR, "root must be an object!");
		return -1;
	}

	/* decode the token stream */
	if ((ret = microjs_json_parse_obj(&jsn, db_desc, &obj)) < 0) {
		DCC_LOG(LOG_ERROR, "microjs_json_parse_obj() failed!");
		return ret;
	}

	ret = db_info_write(json_crc, json_len, &obj);

	printf("Device databse compiled.\n");
	printf("Free memory: %d.%02d KiB.\n", (db_stack - db_heap) / 1024, 
		   (((db_stack - db_heap) % 1024) * 100) / 1024);

	return ret;
}

struct obj_device * device_db_lookup(unsigned int id)
{
	struct obj_db_info * inf;

	inf = (struct obj_db_info*)(STM32_MEM_FLASH + FLASH_BLK_DEV_DB_BIN_OFFS);
	if (id >= inf->obj_cnt)
		return NULL;

	return (struct obj_device *)inf->obj[id];
}

struct obj_device * device_db_model_lookup(unsigned int model)
{
	struct obj_db_info * inf;
	int i;

	inf = (struct obj_db_info*)(STM32_MEM_FLASH + FLASH_BLK_DEV_DB_BIN_OFFS);

	for (i = 0 ; i < inf->obj_cnt; ++i) {
		struct obj_device * obj = (struct obj_device *)inf->obj[i];
		if (obj->model == model)
			return obj;
	}

	return NULL;
}

/********************************************************************** 
 * Diagnostics and debug
 ***********************************************************************/

static void pw_list_dump(FILE * f, struct pw_list * lst)
{
	int i;

	if (lst == NULL) {
		DCC_LOG(LOG_WARNING, "lst == NULL!");
		return;
	}

	DCC_LOG1(LOG_INFO, "lst=0x%08x", lst);

	for (i = 0; i < lst->cnt; ++i) {
		struct pw_entry * pw = &lst->pw[i];

		DCC_LOG2(LOG_INFO, "lst->pw[%d]=0x%08x", i, pw);
		fprintf(f, "\t[%4d %4d] %s\n", pw->min, pw->max, const_str(pw->desc));	
	}
}

static void ic_mode_dump(FILE * f, struct ic_entry * ic)
{
	if (ic == NULL) {
		DCC_LOG(LOG_WARNING, "ic == NULL!");
		return;
	}

	DCC_LOG1(LOG_TRACE, "ic=0x%08x", ic);
	fprintf(f, "\t\"%s\": %d\n", const_str(ic->desc), ic->mode);	
}

static void sensor_dump(FILE * f, struct obj_sensor * sens)
{
	fprintf(f, "id=%d \"%s\" \"%s\"\n", sens->id, 
		const_str(sens->model), const_str(sens->desc));
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
	fprintf(f, "id=%d \"%s\" \"%s\"\n", mod->id, 
			const_str(mod->model), const_str(mod->desc));
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




