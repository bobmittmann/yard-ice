#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/dcclog.h>

#include <thinkos.h>
#include <microjs.h>
#include <microjs-json.h>

#include "crc.h"
#include "slcdev.h"
#include "slcdev-lib.h"

/* ATTENTION: The DB_BLK_VERSION macro should be incremented whenever a
   structure of the database changes. This is to prevent errors/crashes 
   when reading the database from the memory with potential layout 
   differences. These differences will happen if a field is added or 
   resized. */
#define DB_BLK_VERSION 4
#define DB_BLK_MAGIC (0xdb356900 + DB_BLK_VERSION)

#define DB_STACK_LIMIT (sizeof(struct fs_file) + sizeof(struct db_info))
uint16_t db_stack = DB_STACK_LIMIT;

struct db_info * db_info_get(void)
{
	struct fs_dirent entry;
	struct db_info * inf;

	fs_dirent_get(&entry, FLASHFS_DB_BIN);
	if (entry.fp->size == 0)
		return NULL;

	inf = (struct db_info *)(entry.fp->data);
	if (inf->magic != DB_BLK_MAGIC)
		return NULL;

	return inf;
}

static int db_stack_push(void * buf, unsigned int len, void ** ptr)
{
	uint32_t pos;
	uint32_t offs;
	int ret;

	pos = (db_stack + 3) & ~3;
	offs = FLASH_BLK_DB_BIN_OFFS + pos;
	DCC_LOG3(LOG_INFO, "buf=0x%08x len=%d offs=%06x", buf, len, offs);

	if ((ret = stm32_flash_write(offs, buf, len)) < 0) {
		DCC_LOG(LOG_WARNING, "stm32_flash_write() failed!");
		return -1;
	}

	/* update stack */
	db_stack = pos + len;

	/* check for collision */
	if (db_stack > FLASH_BLK_DB_BIN_SIZE) {
		DCC_LOG2(LOG_ERROR, "no memory stack=%d limit=%d!", 
				db_stack, FLASH_BLK_DB_BIN_SIZE);
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
	struct json_js jj;
	uint8_t code[256];
	int ret;

	DCC_LOG(LOG_TRACE, "...");

	jj.code_sz = sizeof(code);
	jj.code = code;
	jj.symtab = (struct symtab *)slcdev_symbuf;
	jj.libdef = &slcdev_lib;

	if ((ret = mcrojs_js_array_enc(jsn, val, opt, &jj)) < 0)
		return ret;

	return db_stack_push(jj.code, jj.code_sz, ptr);
}


static const struct microjs_attr_desc command_desc[] = {
	{ "tag", MICROJS_JSON_STRING, 0, offsetof(struct cmd_entry, tag),
		microjs_const_str_enc },
	{ "seq", MICROJS_JSON_ARRAY, 0, offsetof(struct cmd_entry, seq),
		db_cmd_seq_enc },
	{ "js", MICROJS_JSON_ARRAY, 0, offsetof(struct cmd_entry, code),
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

		DCC_LOG3(LOG_INFO, "CMD[%d]: val=0x%04x msk=0x%04x", 
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
		if ((ret = const_str_add(val->str.dat, val->str.len)) < 0)
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
 * encode a set of lookup ranges
 ***********************************************************************/

static const struct microjs_attr_desc pw4_lut_desc[] = {
	{ "tbl", MICROJS_JSON_ARRAY, 2, offsetof(struct pw4_set, tbl),
		microjs_array_u8_enc },
	{ "alm", MICROJS_JSON_ARRAY, 2, offsetof(struct pw4_set, alm),
		microjs_array_u8_enc },
	{ "tst", MICROJS_JSON_ARRAY, 2, offsetof(struct pw4_set, tst),
		microjs_array_u8_enc },
	{ "", 0, 0, 0, NULL},
};

int db_pw4_lut_enc(struct microjs_json_parser * jsn, 
				   struct microjs_val * val, 
				   unsigned int opt, void * ptr)
{
	int ret;

	if ((ret = microjs_json_parse_obj(jsn, pw4_lut_desc, ptr)) < 0) {
		DCC_LOG(LOG_ERROR, "microjs_json_parse_obj() failed!");
		return ret;
	}

	return ret;
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

	if (*sim == sensor_sim_default())
		*sim = sensor_sim_lookup(val->str.dat, val->str.len);
	else
		*sim = module_sim_lookup(val->str.dat, val->str.len);

	return 0;
}

static const struct microjs_attr_desc model_desc[] = {
	{ "ap", MICROJS_JSON_BOOLEAN, 0, offsetof(struct db_dev_model, opt),
		microjs_bit_enc },
	{ "sim", MICROJS_JSON_STRING, 0, offsetof(struct db_dev_model, sim),
		db_sim_algorithm_enc },
	{ "model", MICROJS_JSON_STRING, 0, offsetof(struct db_dev_model, model),
		microjs_const_str_enc },
	{ "desc", MICROJS_JSON_STRING, 0, offsetof(struct db_dev_model, desc),
		microjs_const_str_enc },
	{ "pw1", MICROJS_JSON_ARRAY, 0, offsetof(struct db_dev_model, pw1lst),
		db_pw_list_enc},
	{ "pw2", MICROJS_JSON_ARRAY, 0, offsetof(struct db_dev_model, pw2lst),
		db_pw_list_enc},
	{ "pw3", MICROJS_JSON_ARRAY, 0, offsetof(struct db_dev_model, pw3lst),
		db_pw_list_enc},
	{ "pw4", MICROJS_JSON_ARRAY, 0, offsetof(struct db_dev_model, pw4lst),
		db_pw_list_enc},
	{ "pw5", MICROJS_JSON_ARRAY, 0, offsetof(struct db_dev_model, pw5lst),
		db_pw_list_enc},
	{ "lut", MICROJS_JSON_OBJECT, 0, offsetof(struct db_dev_model, pw4lut),
		db_pw4_lut_enc},
	{ "cmd", MICROJS_JSON_ARRAY, 0, offsetof(struct db_dev_model, cmd),
		db_cmd_list_enc},
	{ "", 0, 0, 0, NULL}
};




struct db_cfg {
	uint8_t desc; /* Description string */
	uint8_t version[3]; /* version info */
	struct db_dev_model * root;
};

/* Encode a device model */
int db_model_enc(struct microjs_json_parser * jsn, 
				   struct microjs_val * val, 
				   unsigned int opt, void * ptr)
{
	struct db_dev_model ** rootp = (struct db_dev_model **)ptr;
	struct db_dev_model model;
	int ret;

	memset(&model, 0, sizeof(struct db_dev_model));
	model.module = opt;
	model.next = *rootp;
	if (opt)
		model.sim = module_sim_default();
	else
		model.sim = sensor_sim_default();

	if ((ret = microjs_json_parse_obj(jsn, model_desc, &model)) < 0) {
		DCC_LOG(LOG_ERROR, "microjs_json_parse_obj() failed!");
		return ret;
	}

	DCC_LOG6(LOG_TRACE, "tbl[%d, %d] alm[%d, %d] tst[%d, %d]",
			 model.pw4lut.tbl.idx, model.pw4lut.tbl.cnt,
			 model.pw4lut.alm.idx, model.pw4lut.alm.cnt,
			 model.pw4lut.tst.idx, model.pw4lut.tst.cnt);

	ret = db_stack_push(&model, sizeof(struct db_dev_model), (void **)rootp);

	return ret;
}

static const struct microjs_attr_desc info_desc[] = {
	{ "desc", MICROJS_JSON_STRING, 0, offsetof(struct db_cfg, desc),
		microjs_const_str_enc },
	{ "version", MICROJS_JSON_ARRAY, 3, offsetof(struct db_cfg, version),
		microjs_array_u8_enc },
	{ "", 0, 0, 0, NULL}
};

int db_info_enc(struct microjs_json_parser * jsn, 
				   struct microjs_val * val, 
				   unsigned int opt, void * ptr)
{
	int ret;

	DCC_LOG2(LOG_TRACE, "ptr=%08x *ptr=%08x", ptr, *(void **)ptr);

	if ((ret = microjs_json_parse_obj(jsn, info_desc, ptr)) < 0) {
		DCC_LOG(LOG_ERROR, "microjs_json_parse_obj() failed!");
		return ret;
	}

	return ret;
}

static const struct microjs_attr_desc db_desc[] = {
	{ "sensor", MICROJS_JSON_OBJECT, 0, offsetof(struct db_cfg, root), 
		db_model_enc },
	{ "module", MICROJS_JSON_OBJECT, 1, offsetof(struct db_cfg, root), 
		db_model_enc },
	{ "info", MICROJS_JSON_OBJECT, 0, 0, 
		db_info_enc },
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
	"version",
	"info",
	"lut",
	"pw",
	"alm",
	"tbl",
	"tst",
	"lvl",
	"min",
	"max",
	"idx",
	NULL	
};

static int db_json_parse(struct fs_file * json, 
						 struct db_cfg * db)
{
	struct microjs_json_parser jsn;
	uint8_t tok_buf[JSON_TOK_BUF_MAX];
	const char * text = (const char *)json->data;
	unsigned int len = json->size;
	int ret;

	DCC_LOG(LOG_INFO, "1. JSON tokenizer.");
	microjs_json_init(&jsn, tok_buf, JSON_TOK_BUF_MAX, db_label);

	if ((ret = microjs_json_open(&jsn, text, len)) < 0) {
		DCC_LOG(LOG_ERROR, "microjs_json_open() failed!");
		return ret;
	}

	DCC_LOG(LOG_INFO, "5. parsing JSON.");
	/* skip to the object oppening to allow object by object parsing */
	microjs_json_flush(&jsn);

	/* parse the JASON file with the microjs tokenizer */
	while ((ret = microjs_json_scan(&jsn)) == MICROJS_OK) {
		/* decode the token stream */
		if ((ret = microjs_json_parse_obj(&jsn, db_desc, db)) < 0) {
			DCC_LOG(LOG_ERROR, "microjs_json_parse_obj() failed!");
			return ret;
		}
		microjs_json_flush(&jsn);
	}

	if (ret != MICROJS_EMPTY_STACK) {
		DCC_LOG(LOG_ERROR, "microjs_json_scan() failed!");
		return -1;
	}

	DCC_LOG2(LOG_INFO, "6. done, root=0x%08x sp=0x%08x.", db->root, 
			 cm3_sp_get());

	return 0;
}


int db_info_write(unsigned int offs, struct fs_file * json, 
				  struct db_cfg * db)
{
	struct db_info info;
	struct db_dev_model * obj;
	int cnt = 0;
	int i;

	memset(&info, 0, sizeof(struct db_info));

	/* collect all modules */
	obj = (struct db_dev_model *)db->root;
	while (obj != NULL) {
		if (obj->module == 1) {
			if (cnt == DB_MODEL_MAX) {
				DCC_LOG(LOG_ERROR, "too many models.");
				return -1;
			}
			info.obj[cnt++] = obj;
		}
		obj = obj->next;
	}

	/* collect all sesnsors */
	obj = (struct db_dev_model *)db->root;
	while (obj != NULL) {
		if (obj->module == 0) {
			if (cnt == DB_MODEL_MAX) {
				DCC_LOG(LOG_ERROR, "too many models.");
				return -1;
			}
			info.obj[cnt++] = obj;
		}
		obj = obj->next;
	}
			
	/* invert the list order */
	for (i = 0; i < (cnt / 2); ++i) {
		obj = info.obj[i];
		info.obj[i] = info.obj[cnt - i - 1];
		info.obj[cnt - i - 1] = obj;
	}

	if (db_stack_push(slcdev_symbuf, sizeof(slcdev_symbuf), 
					  (void **)&info.symbuf) < 0) {
		DCC_LOG(LOG_ERROR, "db_stack_push() failed!");
		return -1;
	}

	info.symbuf_sz = sizeof(slcdev_symbuf);

	DCC_LOG1(LOG_INFO, "%d models.", cnt);

	info.magic = DB_BLK_MAGIC;
	info.desc = db->desc;
	info.version[0] = db->version[0];
	info.version[1] = db->version[1];
	info.version[2] = db->version[2];
	info.json.txt= (const char *)json->data;
	info.json.crc = json->crc;
	info.json.len = json->size;
	info.obj_cnt = cnt;

	return stm32_flash_write(offs, &info, sizeof(struct db_info));
}

bool device_db_compile(struct fs_file * json)
{
	struct fs_dirent entry;
	unsigned int offs;
	unsigned int size;
	struct db_cfg db;

	/* initialize database stack */
	db_stack = DB_STACK_LIMIT;

	memset(&db, 0, sizeof(struct db_cfg));
	if (db_json_parse(json, &db) < 0) {
		DCC_LOG(LOG_ERROR, "db_json_parse() failed.");
		return false;
	}

	fs_dirent_get(&entry, FLASHFS_DB_BIN);

	/* reserve space for the file entry */
	offs = entry.blk_offs + sizeof(struct fs_file);
	if (db_info_write(offs, json, &db) < 0)
		return false;

	size = db_stack - sizeof(struct fs_file);
	return fs_file_commit(&entry, size);
}

/* check database integrity */
bool device_db_is_sane(void)
{
	return (db_info_get() == NULL) ? false : true;
}

void device_db_init(void)
{
	struct db_info * inf;

	if ((inf = db_info_get()) == NULL) {
		/* initialize an empty symbol table */
		symtab_init(slcdev_symbuf, sizeof(slcdev_symbuf));
		return;
	}

	/* initialize the sybol table from the database */
	memcpy(slcdev_symbuf, inf->symbuf, inf->symbuf_sz);
}

/* check JSON file against database */
bool device_db_need_update(struct fs_file * json)
{
	struct db_info * inf;

	if ((inf = db_info_get()) == NULL)
		return true;

	if ((inf->json.txt != (const char *)json->data) ||
		(inf->json.crc != json->crc) ||
		(inf->json.len != json->size)) {
		return true;
	}

	return false;
}

/********************************************************************** 
 * Database query
 ***********************************************************************/

int db_dev_model_index_by_name(struct db_info * inf,
							   unsigned int str_id)
{
	int i;

	if (inf == NULL)
		return -1;

	for (i = 0 ; i < inf->obj_cnt; ++i) {
		struct db_dev_model * obj = (struct db_dev_model *)inf->obj[i];
		if (obj->model == str_id)
			return i;
	}

	return -1;
}

const struct db_dev_model db_dev_model_null = {
	.next = NULL,
	.opt = 0,
	.model = 0,
	.desc = 0,
	.sim = 0, /* Simulation algorithm */
	.res = 0,
	.pw1lst = NULL, /* Reference Pulse Width */
	.pw2lst = NULL, /* Remote Test Status */
	.pw3lst = NULL, /* Manufacturer Code */
	.pw4lst = NULL, /* Analog */
	.pw5lst = NULL, /* Type Id */
	.pw4lut = {
		.tbl = { .idx = 0, .cnt = 0 },
		.alm = { .idx = 0, .cnt = 0 },
		.tst = { .idx = 0, .cnt = 0 }
	},
	.cmd = NULL
};

struct db_dev_model * db_dev_model_by_index(struct db_info * inf,
											unsigned int idx)
{
	if (inf == NULL)
		return (struct db_dev_model *)&db_dev_model_null;

	return (struct db_dev_model *)inf->obj[idx];
}

/* select one PW range from a list of PWs.
   The selection index 'sel' starts from 1.
   If 'sel' is 0 the first item in the list is selected.
FIXME: empty list shuld return an error */

int device_db_pw_lookup(const struct pw_list * lst, unsigned int sel)
{
	uint32_t min;
	uint32_t max;
	uint32_t pw;

	if (sel >= lst->cnt)
		sel = lst->cnt;

	if (sel > 0)
		sel--;

	max = lst->pw[sel].max;
	min = lst->pw[sel].min;
	pw = (max + min) / 2;
	DCC_LOG3(LOG_INFO, "min=%d max=%d pw=%d", min, max, pw);

	return pw;
}

uint8_t * db_js_lookup(const char * model, const char * jstag)
{
	struct db_dev_model * mdl;
	struct cmd_list * lst;
	struct db_info * inf;
	int tagid;
	int nmid;
	int idx;
	int j;

	if ((inf = db_info_get()) == NULL)
		return NULL;

	DCC_LOG(LOG_TRACE, "1. str_lookup()");

	if ((tagid = str_lookup(jstag, strlen(jstag))) < 0) {
		DCC_LOG(LOG_WARNING, "tag not found!");
		return NULL;
	}	

	DCC_LOG(LOG_TRACE, "2. str_lookup()");

	if ((nmid = str_lookup(model, strlen(model))) < 0) {
		DCC_LOG(LOG_WARNING, "model name not found!");
		return NULL;
	}	

	DCC_LOG(LOG_TRACE, "3. db_dev_model_index_by_name()");

	idx = db_dev_model_index_by_name(inf, nmid);

	if ((mdl = db_dev_model_by_index(inf, idx)) == NULL) {
		DCC_LOG(LOG_WARNING, "device not found!");
		return NULL;
	}

	if ((lst = mdl->cmd) == NULL) {
		DCC_LOG(LOG_WARNING, "command list empty!");
		return NULL;
	}

	DCC_LOG(LOG_TRACE, "4. lookup...");

	for (j = 0; j < lst->cnt; ++j) {
		struct cmd_entry * cmd = &lst->cmd[j];
		if (cmd->tag == tagid)
			return cmd->code;
	}

	DCC_LOG(LOG_WARNING, "command not found!");
	return NULL;
} 

/********************************************************************** 
 * Diagnostics and debug
 ***********************************************************************/

static void pw_list_dump(FILE * f, const struct pw_list * lst)
{
	int i;

	if (lst == NULL) {
		DCC_LOG(LOG_WARNING, "lst == NULL!");
		return;
	}

	DCC_LOG1(LOG_INFO, "lst=0x%08x", lst);

	for (i = 0; i < lst->cnt; ++i) {
		const struct pw_entry * pw = &lst->pw[i];

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
//	int j;

	if (lst == NULL)
		return;

	DCC_LOG1(LOG_INFO, "lst=0x%08x", lst);

	for (i = 0; i < lst->cnt; ++i) {
		struct cmd_entry * cmd = &lst->cmd[i];
		char s[20];

		cmd_seq_dec(s, &cmd->seq);
		fprintf(f, "CMD[%d]: \"%s\" %s [%04x %04x]\n", 
				i, const_str(cmd->tag), s, cmd->seq.msk, cmd->seq.val);	
	
//		for (j = 0; j < SLCDEV_CMD_JS_LINE_MAX; ++j) {
//			if (cmd->js[j] > 0)
//				fprintf(f, "\"%s\"\n", const_str(cmd->js[j]));
//		}
	}
}

static void model_dump(FILE * f, struct db_dev_model * sens)
{
	fprintf(f, "\"%s\" \"%s\"\n", 
			const_str(sens->model), const_str(sens->desc));
	fprintf(f, "SIM: %s\n", model_sim_name(sens->sim));
	fprintf(f, "PW1:");
	pw_list_dump(f, sens->pw1lst);
	fprintf(f, "PW2:");
	pw_list_dump(f, sens->pw2lst);
	fprintf(f, "PW3:");
	pw_list_dump(f, sens->pw3lst);
	fprintf(f, "PW4:");
	pw_list_dump(f, sens->pw4lst);
	fprintf(f, "PW5:");
	pw_list_dump(f, sens->pw5lst);
	cmd_list_dump(f, sens->cmd);
	fprintf(f, "\n");
}


static void db_info_dump(FILE * f, struct db_info * inf)
{
	fprintf(f, " - Desc: \"%s\"\n", const_str(inf->desc));
	fprintf(f, " - Version: %d.%d.%d\n", 
			inf->version[0], inf->version[1], inf->version[2]);
	fprintf(f, " - JSON: txt=0x%08x len=%d crc=0x%04x\n", 
			(uint32_t)inf->json.txt, inf->json.len ,inf->json.crc);

}

int device_db_info(FILE * f)
{
	struct db_info * inf;

	if ((inf = db_info_get()) == NULL)
		return -1;

	db_info_dump(f, inf);

	return 0;
}

int device_db_dump(FILE * f)
{
	struct db_info * inf;
	int i;

	if ((inf = db_info_get()) == NULL)
		return -1;

	db_info_dump(f, inf);

	for (i = 0 ; i < inf->obj_cnt; ++i) {
		struct db_dev_model * obj = inf->obj[i];

		fprintf(f, "%2d - ", i);
		model_dump(f, obj); 
	}

	return 0;
} 

