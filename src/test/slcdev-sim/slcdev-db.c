#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/dcclog.h>

#include <microjs.h>
#include <thinkos.h>

#include "crc.h"
#include "slcdev.h"

const char db_default[] =
	"\"sensor\": { \n"
	" \"id\" : 0, \n"
	" \"model\" : \"Photo\", \n"
	" \"desc\" : \"Photoelectric Smoke Detector\", \n"
	" \"pw1\" : [270, 330],\n"
	" \"pw2\" : [\n"
	"  [\"Remote test disabled\", 270, 330],\n"
	"  [\"Remote test enabled\", 540, 660]\n"
	" ],\n"
	" \"pw3\" : [540, 660],\n"
	" \"pw4\" : [\n"
	"  [\"Normal\", 618, 1068],\n"
	"  [\"Trouble\", 120, 660],\n"
	"  [\"Smoke Alarm 1\", 1260, 1775],\n"
	"  [\"Smoke Alarm 2\", 1775, 2395],\n"
	"  [\"Smoke Alarm 3\", 2395, 2800],\n"
	"  [\"Thermal Alarm (57°C)\", 2800, 3220],\n"
	"  [\"Remote Test\", 2800, 3200]\n"
	" ],\n"
	" \"pw5\" : [810, 990]\n"
	"},\n"
	"\"sensor\": { \n"
	" \"id\" : 1, \n"
	" \"model\" : \"Ion\", \n"
	" \"desc\" : \"Ionization Smoke Detector\", \n"
	" \"pw1\" : [270, 330],\n"
	" \"pw2\" : [\n"
	"  [\"Remote test disabled\", 270, 330],\n"
	"  [\"Remote test enabled\", 540, 660]\n"
	" ],\n"
	" \"pw3\" : [540, 660],\n"
	" \"pw4\" : [\n"
	"  [\"Normal\", 870, 930],\n"
	"  [\"Low Chamber\", 120, 180],\n"
	"  [\"High Chamber\", 260, 330],\n"
	"  [\"Smoke Alarm 1\", 1260, 1775],\n"
	"  [\"Smoke Alarm 2\", 1775, 2395],\n"
	"  [\"Smoke Alarm 3\", 2395, 2800],\n"
	"  [\"Remote Test\", 2130, 2880]\n"
	" ],\n"
	" \"pw5\" : [570, 630]\n"
	"},\n"
	"\"sensor\": { \n"
	" \"id\" : 2, \n"
	" \"model\" : \"Heat\", \n"
	" \"desc\" : \"Heat Detector\", \n"
	" \"pw1\" : [270, 330],\n"
	" \"pw2\" : [\n"
	"  [\"Remote test disabled\", 270, 330],\n"
	"  [\"Remote test enabled\", 540, 660]\n"
	" ],\n"
	" \"pw3\" : [540, 660],\n"
	" \"pw4\" : [\n"
	"  [\"Normal, 25°C\", 1070, 1400],\n"
	"  [\"Normal, 57.2°C\", 1895, 2110],\n"
	"  [\"Heat Alarm 57.2°C\", 2110],\n"
	"  [\"Remote Test\", 3270, 3330]\n"
	" ],\n"
	" \"pw5\" : [270, 330],\n"
	" \"cmd\": [\n"
	"  {\n"
	"   \"tag\": \"LED on\",\n"
	"   \"seq\": [\"0xx\"],\n"
	"   \"js\": [ \"dev.led_on = true;\" ]\n"
	"  },\n"
	"  {\n"
	"   \"tag\": \"LED off\",\n"
	"   \"seq\": [\"1x0\"],\n"
	"   \"js\": [ \"dev.led_on = false;\" ]\n"
	"  },\n"
	"  {\n"
	"   \"tag\": \"LED blink\",\n"
	"   \"seq\": [\"1x1\"],\n"
	"   \"js\": [ \"dev.led_blink = true;\" ]\n"
	"  },\n"
	"  {\n"
	"   \"tag\": \"Enable Type ID\",\n"
	"   \"seq\": [\"x1x\"],\n"
	"   \"js\": [ \"dev.pw5_en = true;\" ]\n"
	"  },\n"
	"  {\n"
	"   \"tag\": \"Remote Test on\",\n"
	"   \"seq\": [\"0x0\", \"0x0\"],\n"
	"   \"js\": [ \"dev.test = true;\" ]\n"
	"  },\n"
	"  {\n"
	"   \"tag\": \"Remote Test off\",\n"
	"   \"seq\": [\"1x1\", \"1x1\"],\n"
	"   \"js\": [ \"dev.test = false;\" ]\n"
	"  }\n"
	" ]\n"
	"}\n";

uint16_t db_stack = FLASH_BLK_DB_BIN_SIZE;
uint16_t db_heap = 0;

static int db_stack_push(void * buf, unsigned int len, void ** ptr)
{
	uint32_t pos;
	uint32_t offs;
	int ret;

	DCC_LOG2(LOG_INFO, "1. buf=0x%08x len=%d", buf, len);

	pos = (db_stack - len) & ~3;
	offs = FLASH_BLK_DB_BIN_OFFS + pos;

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

	if (ptr != NULL)
		*ptr = (void *)(STM32_MEM_FLASH + offs);

	return len;
}

static int db_heap_push(void * buf, unsigned int len, void ** ptr)
{
	uint32_t offs;
	int ret;

	offs = FLASH_BLK_DB_BIN_OFFS + db_heap;

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

	if (ptr != NULL)
		*ptr = (void *)(STM32_MEM_FLASH + offs);

	return len;
}

/********************************************************************** 
 * encode a command
 ***********************************************************************/

static bool cmd_seq_bits_parse(const char * s, unsigned int len, 
							   struct cmd_seq * seq)
{
	uint32_t msk = 0;
	uint32_t val = 0;
	int i;

	if (len != 3)
		return false;

	for (i = 0 ; i < 3; ++i) {
		val <<= 1;
		msk <<= 1;
		switch (s[i]) {
		case 'x':
		case 'X':
			val |= 0;
			msk |= 0;
			break;
		case '1':
			val |= 1;
			msk |= 1;
			break;
		case '0':
			val |= 0;
			msk |= 1;
			break;
		default:
			return false;
		}
	}

	seq->msk = (seq->msk << 3) | msk;
	seq->val = (seq->val << 3) | val;

	return true;
}

static int db_cmd_seq_enc(struct microjs_json_parser * jsn, 
						  struct microjs_val * val, 
						  unsigned int opt, void * ptr) 
{
	struct cmd_seq * seq = (struct cmd_seq *)ptr;
	int typ;

	seq->val = 0;
	seq->msk = 0;

	while ((typ = microjs_json_get_val(jsn, val)) == MICROJS_JSON_STRING) {
		if (!cmd_seq_bits_parse(val->str.dat, val->str.len, seq)) {
			/* Not a valid bit pattern, discard! */
			DCC_LOG(LOG_ERROR, "invalid bit pattern!");
			return -1;
		}
	} 

	if (typ != MICROJS_JSON_END_ARRAY) {
		DCC_LOG(LOG_ERROR, "expecting array closing!");
		return -1;
	}

	return 0;
}

int db_cmd_js_enc(struct microjs_json_parser * jsn, 
			   struct microjs_val * val, 
			   unsigned int opt, void * ptr) 
{
	uint8_t * js = (uint8_t *)ptr;
	int cnt = 0;
	int typ;
	int ret;

	while ((typ = microjs_json_get_val(jsn, val)) == MICROJS_JSON_STRING) {
		if ((ret = const_str_write(val->str.dat, val->str.len)) < 0)
			return ret;
		js[cnt] = ret;
		DCC_LOG2(LOG_INFO, "js[%d] = %d...", cnt, js[cnt]);
		cnt++;
	} 

	if (typ != MICROJS_JSON_END_ARRAY) {
		DCC_LOG(LOG_ERROR, "expecting array closing!");
		return -1;
	}

	return 0;
}

static const struct microjs_attr_desc command_desc[] = {
	{ "tag", MICROJS_JSON_STRING, 0, offsetof(struct cmd_entry, tag),
		microjs_const_str_enc },
	{ "seq", MICROJS_JSON_ARRAY, 0, offsetof(struct cmd_entry, seq),
		db_cmd_seq_enc },
	{ "js", MICROJS_JSON_ARRAY, 0, offsetof(struct cmd_entry, js),
		db_cmd_js_enc },
	{ "", 0, 0, 0, NULL}
};

/********************************************************************** 
 * encode a list of commands
 ***********************************************************************/
int db_cmd_list_enc(struct microjs_json_parser * jsn, 
				   struct microjs_val * val, 
				   unsigned int opt, void * ptr)
{
	struct cmd_list lst;
	int ret;
	int typ;

	lst.cnt = 0;

	/* list of commands */
	while ((typ = microjs_json_get_val(jsn, val)) == MICROJS_JSON_OBJECT) {

		struct cmd_entry * cmd = &lst.cmd[lst.cnt];

		if (lst.cnt == SLCDEV_CMD_LIST_LEN_MAX) {
			DCC_LOG(LOG_ERROR, "too many items!");
			return -1;
		}

		memset(cmd, 0, sizeof(struct cmd_entry));

		if ((ret = microjs_json_parse_obj(jsn, command_desc, cmd)) < 0) {
			DCC_LOG(LOG_ERROR, "microjs_json_parse_obj() failed!");
			return ret;
		}

		DCC_LOG3(LOG_TRACE, "CMD[%d]: val=0x%04x msk=0x%04x", 
				 lst.cnt, cmd->seq.val, cmd->seq.msk);

		lst.cnt++;
	} 
	
	if (typ != MICROJS_JSON_END_ARRAY) {
		DCC_LOG(LOG_ERROR, "expecting array closing!");
		return -1;
	}

	return db_stack_push(&lst, sizeof(uint32_t) + lst.cnt * 
						 sizeof(struct cmd_entry), ptr);
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
	uint32_t buf[(sizeof(struct pw_list) + SLCDEV_PW_LIST_LEN_MAX * 
				  sizeof(struct pw_entry)) / sizeof(uint32_t)];
	struct pw_list * lst = (struct pw_list *)buf;
	struct pw_entry * pw;
	int ret;
	int typ;

	lst->cnt = 0;
	pw = &lst->pw[0];

	if ((typ = microjs_json_get_val(jsn, val)) == MICROJS_JSON_ARRAY) {
		/* list of PWs */
		do {
			if (lst->cnt == SLCDEV_PW_LIST_LEN_MAX) {
				DCC_LOG(LOG_ERROR, "too many items!");
				return -1;
			}

			DCC_LOG1(LOG_INFO, "PW[%d]", lst->cnt);
			typ = microjs_json_get_val(jsn, val);
			if ((ret = db_pw_enc(jsn, typ, val, pw)) < 0) {
				DCC_LOG(LOG_ERROR, "db_parse_pw() failed!");
				return ret;
			}
			DCC_LOG3(LOG_INFO, "PW[%d]: min=%d max=%d", 
					 lst->cnt, pw->min, pw->max);
			lst->cnt++;
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
		DCC_LOG2(LOG_INFO, "PW: min=%d max=%d", pw->min, pw->max);
		lst->cnt++;
	} 

	DCC_LOG1(LOG_INFO, "lst.cnt=%d", lst->cnt);

	ret = db_stack_push(lst, sizeof(struct pw_list) + lst->cnt * 
						sizeof(struct pw_entry), ptr);

	return ret;
}

static int db_sim_algorithm_enc(struct microjs_json_parser * jsn, 
								struct microjs_val * val, 
								unsigned int opt, void * ptr) 
{
	uint8_t * sim = (uint8_t *)ptr;

	if (opt == 0)
		*sim = sensor_sim_lookup(val->str.dat, val->str.len);
	else
		*sim = module_sim_lookup(val->str.dat, val->str.len);

	return 0;
}


static const struct microjs_attr_desc sensor_desc[] = {
	{ "ap", MICROJS_JSON_BOOLEAN, 0, offsetof(struct obj_sensor, opt),
		microjs_bit_enc },
	{ "sim", MICROJS_JSON_STRING, 0, offsetof(struct obj_sensor, sim),
		db_sim_algorithm_enc },
	{ "model", MICROJS_JSON_STRING, 0, offsetof(struct obj_sensor, model),
		microjs_const_str_enc },
	{ "desc", MICROJS_JSON_STRING, 0, offsetof(struct obj_sensor, desc),
		microjs_const_str_enc },
	{ "pw1", MICROJS_JSON_ARRAY, 0, offsetof(struct obj_sensor, pw1),
		db_pw_list_enc},
	{ "pw2", MICROJS_JSON_ARRAY, 0, offsetof(struct obj_sensor, pw2),
		db_pw_list_enc},
	{ "pw3", MICROJS_JSON_ARRAY, 0, offsetof(struct obj_sensor, pw3),
		db_pw_list_enc},
	{ "pw4", MICROJS_JSON_ARRAY, 0, offsetof(struct obj_sensor, pw4),
		db_pw_list_enc},
	{ "pw5", MICROJS_JSON_ARRAY, 0, offsetof(struct obj_sensor, pw5),
		db_pw_list_enc},
	{ "cmd", MICROJS_JSON_ARRAY, 0, offsetof(struct obj_sensor, cmd),
		db_cmd_list_enc},
	{ "", 0, 0, 0, NULL}
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
	sensor.module = 0;
	sensor.next = *objp;
	sensor.sim = sensor_sim_default();

	if ((ret = microjs_json_parse_obj(jsn, sensor_desc, &sensor)) < 0) {
		DCC_LOG(LOG_ERROR, "microjs_json_parse_obj() failed!");
		return ret;
	}

	ret = db_stack_push(&sensor, sizeof(struct obj_sensor), (void **)objp);

	DCC_LOG1(LOG_TRACE, "obj=%08x ...................", *objp);

	return ret;
}

static const struct microjs_attr_desc module_desc[] = {
	{ "ap", MICROJS_JSON_BOOLEAN, 0, offsetof(struct obj_module, opt),
		microjs_bit_enc },
	{ "sim", MICROJS_JSON_STRING, 1, offsetof(struct obj_sensor, sim),
		db_sim_algorithm_enc },
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
	{ "cmd", MICROJS_JSON_ARRAY, 0, offsetof(struct obj_module, cmd),
		db_cmd_list_enc},
	{ "", 0, 0, 0, NULL}
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
	module.module = 1;
	module.next = *objp;
	module.sim = module_sim_default();

	if ((ret = microjs_json_parse_obj(jsn, module_desc, &module)) < 0) {
		DCC_LOG(LOG_ERROR, "microjs_json_parse_obj() failed!");
		return ret;
	}

	ret = db_stack_push(&module, sizeof(struct obj_module), (void **)objp);

	DCC_LOG1(LOG_INFO, "obj=%08x ...................", *objp);

	return ret;
}

static const struct microjs_attr_desc db_desc[] = {
	{ "sensor", MICROJS_JSON_OBJECT, 0, 0, db_sensor_enc },
	{ "module", MICROJS_JSON_OBJECT, 0, 0, db_module_enc },
	{ "", 0, 0, 0, NULL},
};

//#define JSON_TOK_BUF_MAX (4096)
#define JSON_TOK_BUF_MAX 384

const char * const db_label[] = {
	"sensor",
	"module",
	"ap",
	"model",
	"desc",
	"pw1",
	"pw2",
	"pw3",
	"pw4",
	"pw5",
	"cmd",
	"tag",
	"seq",
	"js",
	"sim",
	NULL	
};

static struct db_obj * db_json_parse(struct json_file * json)
{
	struct microjs_json_parser jsn;
	uint8_t tok_buf[JSON_TOK_BUF_MAX];
	struct db_obj * root;
	const char * text = json->txt;
	unsigned int len = json->len;
	int ret;

	DCC_LOG(LOG_TRACE, "1. JSON tokenizer.");

	microjs_json_init(&jsn, tok_buf, JSON_TOK_BUF_MAX, db_label);

	if (microjs_json_open(&jsn, text, len) < 0) {
		DCC_LOG(LOG_ERROR, "microjs_json_open() failed!");
		return NULL;
	}

	DCC_LOG(LOG_TRACE, "2. erasing database.");
	/* erase flash block */
	if (stm32_flash_erase(FLASH_BLK_DB_BIN_OFFS, 
						  FLASH_BLK_DB_BIN_SIZE) < 0) {
		DCC_LOG(LOG_ERROR, "stm32_flash_erase() failed!");
		return NULL;
	};

	/* initialize database */
	db_stack = FLASH_BLK_DB_BIN_SIZE;
	db_heap = 0;

	DCC_LOG(LOG_TRACE, "3. erasing constant strings pool.");
	if (slcdev_const_str_purge() < 0) {
		DCC_LOG(LOG_ERROR, "slcdev_const_str_purge() failed!");
		return NULL;
	};

	/* initialize the database with default devices */
	root = NULL;

	DCC_LOG(LOG_TRACE, "4. parsing JSON.");

	/* skip to the object oppening to allow object by object parsing */
	microjs_json_flush(&jsn);

	/* parse the JASON file with the microjs tokenizer */
	while ((ret = microjs_json_scan(&jsn)) == MICROJS_OK) {
		/* decode the token stream */
		if (microjs_json_parse_obj(&jsn, db_desc, &root) < 0) {
			DCC_LOG(LOG_ERROR, "microjs_json_parse_obj() failed!");
			return NULL;
		}
		microjs_json_flush(&jsn);
	}

	if (ret != MICROJS_EMPTY_STACK) {
		DCC_LOG(LOG_ERROR, "microjs_json_scan() failed!");
		return NULL;
	}

	DCC_LOG2(LOG_TRACE, "5. done, root=0x%08x sp=0x%08x.", root, cm3_sp_get());

	return root;
}



#define DB_OBJ_MODEL_MAX 64

#define DB_PTR_IS_VALID(PTR) \
	(((void *)(PTR) >= (void *)(STM32_MEM_FLASH + \
								FLASH_BLK_DB_BIN_OFFS)) && \
	 ((void *)(PTR) < (void *)(STM32_MEM_FLASH + FLASH_BLK_DB_BIN_OFFS + \
						 FLASH_BLK_DB_BIN_SIZE))) 

int db_info_write(struct json_file * json, struct db_obj * root)
{

	uint32_t buf[(sizeof(struct db_info) + 
		DB_OBJ_MODEL_MAX * sizeof(struct db_obj *)) / sizeof(uint32_t)];
	struct db_info * info = (struct db_info *)buf;
	struct db_obj * obj;
	int cnt;
	int i;

	DCC_LOG(LOG_TRACE, "1. clear info");

	memset(info, 0, sizeof(struct db_info));


	cnt = 0;

	DCC_LOG(LOG_TRACE, "2. collect modules");
	/* collect all modules */
	obj = (struct db_obj *)root;
	while (obj != NULL) {
/*
		if (!DB_PTR_IS_VALID(obj)) {
			DCC_LOG1(LOG_ERROR, "invalid pointer: 0x%08x", obj);
			return -1;
		}
*/
		if (obj->type == DB_OBJ_MODULE) {
			if (cnt == DB_OBJ_MODEL_MAX) {
				DCC_LOG(LOG_ERROR, "too many models.");
				return -1;
			}
			info->obj[cnt++] = obj;
		}
		obj = obj->next;
	}

	DCC_LOG(LOG_TRACE, "3. collect sensors");
	/* collect all sesnsors */
	obj = (struct db_obj *)root;
	while (obj != NULL) {
/*
		if (!DB_PTR_IS_VALID(obj)) {
			DCC_LOG1(LOG_ERROR, "invalid pointer: 0x%08x", obj);
			return -1;
		}
*/
		if (obj->type == DB_OBJ_SENSOR) {
			if (cnt == DB_OBJ_MODEL_MAX) {
				DCC_LOG(LOG_ERROR, "too many models.");
				return -1;
			}
			info->obj[cnt++] = obj;
		}
		obj = obj->next;
	}
			
	/* invert the list order */
	for (i = 0; i < (cnt / 2); ++i) {
		obj = info->obj[i];
		info->obj[i] = info->obj[cnt - i - 1];
		info->obj[cnt - i - 1] = obj;
	}

	DCC_LOG1(LOG_TRACE, "%d models.", cnt);
			
	DCC_LOG(LOG_TRACE, "4. fill structure");

	info->len = sizeof(struct db_info) + cnt * sizeof(struct db_obj *);
	info->type = DB_OBJ_DB_INFO;
	info->next = root;
	info->json_txt= json->txt;
	info->json_crc = json->crc;
	info->json_len = json->len;
	info->obj_cnt = cnt;

	DCC_LOG(LOG_TRACE, "5. flash write");

	return db_heap_push(info, info->len, NULL);
}

int device_db_compile(struct json_file * json)
{
	struct db_obj * root;
	int ret;

	DCC_LOG1(LOG_TRACE, "sp=0x%08x ..........................", cm3_sp_get());


	DCC_LOG(LOG_TRACE, "1. compiling JSON file.");

	if ((root = db_json_parse(json)) == NULL) {
		DCC_LOG(LOG_ERROR, "db_json_parse() failed.");
		return -1;
	}

	DCC_LOG(LOG_TRACE, "5. updating database info.");

	if ((ret = db_info_write(json, root)) < 0) {
		return ret;
	}

//	struct db_info * inf;
//	printf("Device databse compiled.\n");
//	printf(" - %d devices.\n", inf->obj_cnt);
//	printf(" - Free memory: %d.%02d KiB.\n", (db_stack - db_heap) / 1024, 
//		   (((db_stack - db_heap) % 1024) * 100) / 1024);

	return ret;
}

bool device_db_sanity_check(struct json_file * json)
{
	struct db_info * inf;

	DCC_LOG1(LOG_TRACE, "sp=0x%08x ..........................", cm3_sp_get());

	/* check database integrity */
	inf = (struct db_info *)(STM32_MEM_FLASH + FLASH_BLK_DB_BIN_OFFS);
	if ((inf->len >= sizeof(struct db_info)) && 
		(inf->type == DB_OBJ_DB_INFO) && (inf->json_txt == json->txt) && 
		(inf->json_crc == json->crc) && (inf->json_len == json->len)) {
		return true;
	}

	DCC_LOG3(LOG_TRACE, "db_info: txt=0x%08x len=%d crc=0x%04x", 
			 inf->json_txt, inf->json_len, inf->json_crc);

	return false;
}

/********************************************************************** 
 * Database query
 ***********************************************************************/

struct db_dev_model * device_db_lookup(unsigned int id)
{
	struct db_info * inf;

	inf = (struct db_info*)(STM32_MEM_FLASH + FLASH_BLK_DB_BIN_OFFS);
	if (id >= inf->obj_cnt)
		return NULL;

	return (struct db_dev_model *)inf->obj[id];
}

int db_dev_model_index_by_name(unsigned int str_id)
{
	struct db_info * inf;
	int i;

	inf = (struct db_info*)(STM32_MEM_FLASH + FLASH_BLK_DB_BIN_OFFS);

	for (i = 0 ; i < inf->obj_cnt; ++i) {
		struct db_dev_model * obj = (struct db_dev_model *)inf->obj[i];
		if (obj->model == str_id)
			return i;
	}

	return -1;
}

struct db_dev_model * db_dev_model_by_index(unsigned int idx)
{
	struct db_info * inf;

	inf = (struct db_info*)(STM32_MEM_FLASH + FLASH_BLK_DB_BIN_OFFS);

	if (idx >= inf->obj_cnt) {
		return NULL;
	}

	return (struct db_dev_model *)inf->obj[idx];
}



int device_db_pw1_lookup(struct db_dev_model * mod, unsigned int sel,
					  unsigned int bias)
{
	uint32_t avg;
	uint32_t min;
	uint32_t max;
	uint32_t pw;

	if (sel >= mod->pw1->cnt)
		sel = mod->pw1->cnt - 1;

	max = mod->pw1->pw[sel].max;
	min = mod->pw1->pw[sel].min;
	avg = (max + min) / 2;
	pw = (avg * bias) / 128;
	DCC_LOG4(LOG_INFO, "min=%d max=%d avg=%d pw=%d", min, max, avg, pw);

	return pw;
}

int device_db_pw2_lookup(struct db_dev_model * mod, unsigned int sel,
					  unsigned int bias)
{
	uint32_t avg;
	uint32_t min;
	uint32_t max;
	uint32_t pw;

	if (sel >= mod->pw2->cnt)
		sel = mod->pw2->cnt - 1;

	max = mod->pw2->pw[sel].max;
	min = mod->pw2->pw[sel].min;
	avg = (max + min) / 2;
	pw = (avg * bias) / 128;
	DCC_LOG4(LOG_INFO, "min=%d max=%d avg=%d pw=%d", min, max, avg, pw);

	return pw;
}

int device_db_pw3_lookup(struct db_dev_model * mod, unsigned int sel,
					  unsigned int bias)
{
	uint32_t avg;
	uint32_t min;
	uint32_t max;
	uint32_t pw;

	if (sel >= mod->pw3->cnt)
		sel = mod->pw3->cnt - 1;

	max = mod->pw3->pw[sel].max;
	min = mod->pw3->pw[sel].min;
	avg = (max + min) / 2;
	pw = (avg * bias) / 128;
	DCC_LOG4(LOG_INFO, "min=%d max=%d avg=%d pw=%d", min, max, avg, pw);

	return pw;
}

int device_db_pw4_lookup(struct db_dev_model * mod, unsigned int sel,
					  unsigned int bias)
{
	uint32_t avg;
	uint32_t min;
	uint32_t max;
	uint32_t pw;

	if (sel >= mod->pw4->cnt)
		sel = mod->pw4->cnt - 1;

	max = mod->pw4->pw[sel].max;
	min = mod->pw4->pw[sel].min;
	avg = (max + min) / 2;
	pw = (avg * bias) / 128;
	DCC_LOG4(LOG_INFO, "min=%d max=%d avg=%d pw=%d", min, max, avg, pw);

	return pw;
}

int device_db_pw5_lookup(struct db_dev_model * mod, unsigned int sel,
					  unsigned int bias)
{
	uint32_t avg;
	uint32_t min;
	uint32_t max;
	uint32_t pw;

	if (sel >= mod->pw5->cnt)
		sel = mod->pw5->cnt - 1;

	max = mod->pw5->pw[sel].max;
	min = mod->pw5->pw[sel].min;
	avg = (max + min) / 2;
	pw = (avg * bias) / 128;
	DCC_LOG4(LOG_TRACE, "min=%d max=%d avg=%d pw=%d", min, max, avg, pw);

	return pw;
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

void cmd_seq_dec(char * s, struct cmd_seq * seq)
{
	uint32_t val = seq->val;
	uint32_t msk = seq->msk;
	char * cp = s;
	int i;
	int j;

	i = 4;
	while ((msk & 0x7000) == 0) {
		val <<= 3;
		msk <<= 3;
		--i;
	}

	for (; i >= 0; --i) {
		/* add a separation between triplets */
		if (cp != s)
			*cp++ = ' ';
		for (j = 2; j >= 0; --j) {
			int c;
			if (msk & 0x4000)
				c = (val & 0x4000) ? '1' : '0';
			else
				c = 'x';
			val <<= 1;
			msk <<= 1;
			*cp++ = c;
		}
	}

	*cp = '\0';
}

static void cmd_list_dump(FILE * f, struct cmd_list * lst)
{
	int i;

	if (lst == NULL) {
		DCC_LOG(LOG_WARNING, "lst == NULL!");
		return;
	}

	DCC_LOG1(LOG_INFO, "lst=0x%08x", lst);

	for (i = 0; i < lst->cnt; ++i) {
		struct cmd_entry * cmd = &lst->cmd[i];
		char s[20];

		cmd_seq_dec(s, &cmd->seq);
		fprintf(f, "CMD[%d]: \"%s\" %s [%04x %04x]\n", 
				i, const_str(cmd->tag), s, cmd->seq.msk, cmd->seq.val);	
	}
}

static void sensor_dump(FILE * f, struct obj_sensor * sens)
{
	fprintf(f, "\"%s\" \"%s\"\n", 
			const_str(sens->model), const_str(sens->desc));
	fprintf(f, "SIM: %s\n", model_sim_name(sens->sim));
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
	cmd_list_dump(f, sens->cmd);
	fprintf(f, "\n");
}

static void module_dump(FILE * f, struct obj_module * mod)
{
	fprintf(f, "\"%s\" \"%s\"\n", 
			const_str(mod->model), const_str(mod->desc));
	fprintf(f, "SIM: %s\n", model_sim_name(mod->sim));
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
	cmd_list_dump(f, mod->cmd);
	fprintf(f, "\n");
}

static void db_info_dump(FILE * f, struct db_info * inf)
{
	fprintf(f, "Device database: txt=0x%08x len=%d crc=0x%04x\n", 
			(uint32_t)inf->json_txt, inf->json_len ,inf->json_crc);
}

int device_db_dump(FILE * f)
{
	struct db_info * inf;
	int i;

	inf = (struct db_info*)(STM32_MEM_FLASH + FLASH_BLK_DB_BIN_OFFS);
	db_info_dump(f, inf);

	for (i = 0 ; i < inf->obj_cnt; ++i) {
		struct db_obj * obj = (struct db_obj *)inf->obj[i];

		fprintf(f, "%2d - ", i);

		switch (obj->type) {
		case DB_OBJ_SENSOR:
			sensor_dump(f, (struct obj_sensor *)obj);
			break;

		case DB_OBJ_MODULE:
			module_dump(f, (struct obj_module *)obj);
			break;
		}
	}

	return 0;
} 


int device_db_erase(void)
{
	uint32_t blk_offs = FLASH_BLK_DB_BIN_OFFS;
	uint32_t blk_size = FLASH_BLK_DB_BIN_SIZE;
	int ret;

	if ((ret = stm32_flash_erase(blk_offs, blk_size)) < 0) {
		DCC_LOG(LOG_ERROR, "stm32_flash_erase() failed!");
		return ret;
	};

	DCC_LOG(LOG_TRACE, "device database erased!");

	return ret;
}

