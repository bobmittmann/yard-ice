#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <microjs.h>
#include <microjs-json.h>

#include <sys/dcclog.h>

#include "crc.h"
#include "slcdev.h"
#include "isink.h"
#include "slcdev-lib.h"

/* ATTENTION: The CFG_BLK_VERSION macro should be incremented whenever a
   configuration structure changes. */
#define CFG_BLK_VERSION 7
#define CFG_BLK_MAGIC (0xcf124800 + CFG_BLK_VERSION)

struct cfg_info {
	uint32_t magic;
	uint16_t json_crc;
	uint16_t json_len;
	const char * json_txt;
};

#define CFG_STACK_LIMIT (sizeof(struct fs_file) + \
						 sizeof(struct cfg_info) + \
						 sizeof(ss_dev_tab) + \
						 sizeof(usr) + \
						 sizeof(struct slcdev_trig) + \
						 sizeof(slcdev_symbuf))

uint16_t cfg_stack = CFG_STACK_LIMIT;

static int cfg_stack_push(void * buf, unsigned int len, void ** ptr)
{
	uint32_t pos;
	uint32_t offs;
	int ret;

//	pos = (cfg_stack - len) & ~3;

	pos = (cfg_stack + 3) & ~3;
	offs = FLASH_BLK_CFG_BIN_OFFS + pos;
	DCC_LOG3(LOG_TRACE, "buf=0x%08x len=%d offs=%06x", buf, len, offs);

	if ((ret = stm32_flash_write(offs, buf, len)) < 0) {
		DCC_LOG(LOG_WARNING, "stm32_flash_write() failed!");
		return -1;
	}

	/* update stack */
	cfg_stack = pos + len;

	/* check for overflow */
	if (cfg_stack > FLASH_BLK_CFG_BIN_SIZE) {
		DCC_LOG2(LOG_ERROR, "no memory stack=%d limit=%d!", 
				cfg_stack, FLASH_BLK_CFG_BIN_SIZE);
		return -1;
	}

	if (ptr != NULL)
		*ptr = (void *)(STM32_MEM_FLASH + offs);

	return len;
}

static const struct microjs_attr_desc info_desc[] = {
	{ "tag", MICROJS_JSON_STRING, 0, 
		offsetof(struct slcdev_cfg_info, tag), microjs_const_str_enc },
	{ "desc", MICROJS_JSON_STRING, 0, 
		offsetof(struct slcdev_cfg_info, desc), microjs_const_str_enc },
	{ "author", MICROJS_JSON_STRING, 0, 
		offsetof(struct slcdev_cfg_info, author), microjs_const_str_enc },
	{ "version", MICROJS_JSON_ARRAY, 3, 
		offsetof(struct slcdev_cfg_info, version), microjs_array_u8_enc },
	{ "", 0, 0, 0, NULL}
};

int cfg_info_enc(struct microjs_json_parser * jsn, 
				   struct microjs_val * val, 
				   unsigned int opt, void * ptr)
{
	struct slcdev_cfg_info * info = (struct slcdev_cfg_info *)&usr.cfg_info;
	int ret;

	memset(info, 0, sizeof(struct slcdev_cfg_info));

	if ((ret = microjs_json_parse_obj(jsn, info_desc, info)) < 0) {
		DCC_LOG(LOG_ERROR, "microjs_json_parse_obj() failed!");
	}

	return ret;
}


int cfg_script_enc(struct microjs_json_parser * jsn, 
				   struct microjs_val * val, 
				   unsigned int opt, void * ptr) 
{
	struct json_js jj;
	uint8_t code[256];
	int ret;

	jj.code_sz = sizeof(code);
	jj.code = code;
	jj.symtab = (struct symtab *)slcdev_symbuf;
	jj.libdef = &slcdev_lib;

	if ((ret = mcrojs_js_array_enc(jsn, val, 0, &jj)) < 0) {
		fprintf(stdout, "script %d error: %s\n", 
				opt, microjs_strerr(ret));
		return ret;
	}

	cfg_stack_push(jj.code, jj.code_sz, ptr);

	return ret;
}


static const struct microjs_attr_desc events_desc[] = {
	{ "init", MICROJS_JSON_ARRAY, 0, 
		offsetof(struct slcdev_usr, init), cfg_script_enc },
	{ "tmr1", MICROJS_JSON_ARRAY, SLC_EV_TMR1, 
		offsetof(struct slcdev_usr, tmr[0]), cfg_script_enc },
	{ "tmr2", MICROJS_JSON_ARRAY, SLC_EV_TMR2, 
		offsetof(struct slcdev_usr, tmr[1]), cfg_script_enc },
	{ "tmr3", MICROJS_JSON_ARRAY, SLC_EV_TMR3, 
		offsetof(struct slcdev_usr, tmr[2]), cfg_script_enc },
	{ "tmr4", MICROJS_JSON_ARRAY, SLC_EV_TMR4, 
		offsetof(struct slcdev_usr, tmr[3]), cfg_script_enc },
	{ "usr1", MICROJS_JSON_ARRAY, SLC_EV_USR1, 
		offsetof(struct slcdev_usr, usr[0]), cfg_script_enc },
	{ "usr2", MICROJS_JSON_ARRAY, SLC_EV_USR2, 
		offsetof(struct slcdev_usr, usr[1]), cfg_script_enc },
	{ "usr3", MICROJS_JSON_ARRAY, SLC_EV_USR3, 
		offsetof(struct slcdev_usr, usr[2]), cfg_script_enc },
	{ "usr4", MICROJS_JSON_ARRAY, SLC_EV_USR4, 
		offsetof(struct slcdev_usr, usr[3]), cfg_script_enc },
	{ "usr5", MICROJS_JSON_ARRAY, SLC_EV_USR5, 
		offsetof(struct slcdev_usr, usr[4]), cfg_script_enc },
	{ "usr6", MICROJS_JSON_ARRAY, SLC_EV_USR6, 
		offsetof(struct slcdev_usr, usr[5]), cfg_script_enc },
	{ "usr7", MICROJS_JSON_ARRAY, SLC_EV_USR7, 
		offsetof(struct slcdev_usr, usr[6]), cfg_script_enc },
	{ "usr8", MICROJS_JSON_ARRAY, SLC_EV_USR8, 
		offsetof(struct slcdev_usr, usr[7]), cfg_script_enc },
	{ "", 0, 0, 0, NULL},
};


/* Encode an event by name */
int cfg_device_event_enc(struct microjs_json_parser * jsn, 
						struct microjs_val * val, 
						unsigned int opt, void * ptr)
{
	const struct microjs_attr_desc * desc = events_desc;
	uint8_t * evp = (uint8_t *)ptr;
	int i;

	DCC_LOG2(LOG_TRACE, "'%c%c'...", val->str.dat[0], val->str.dat[1]);

	for (i = 0; desc[i].parse != NULL; ++i) {
		if ((strncmp(desc[i].key, val->str.dat, val->str.len) == 0) && 
			(desc[i].key[val->str.len] == '\0')) {
			DCC_LOG1(LOG_TRACE, "'%s'", desc[i].key);
			*evp = desc[i].opt;
			return 0;
		}
	}

	DCC_LOG(LOG_WARNING, "invalid event!");

	return -1;
}

/* Encode the array of addresses. This effectivelly write the configuration 
   into the device objects. */
int cfg_device_addr_enc(struct microjs_json_parser * jsn, 
						struct microjs_val * val, 
						unsigned int opt, void * ptr)
{
	uint32_t * addr_bmp = (uint32_t *)ptr;
	int typ;

	DCC_LOG(LOG_TRACE, "...");

	while ((typ = microjs_json_get_val(jsn, val)) == MICROJS_JSON_INTEGER) {
		unsigned int addr = val->u32;
		if ((addr < 1) || (addr > 159)) {
			DCC_LOG1(LOG_WARNING, "invalid address addr=%d", addr);
			printf("Invalid address: %d\n", addr);
			return -1;
		}
		DCC_LOG1(LOG_INFO, "addr=%d", addr);
		addr_bmp[addr / 32] |= (1 << (addr % 32));
	}

	return (typ == MICROJS_JSON_END_ARRAY) ? 0 : -1;
}



/* This is an auxiliarly structure for parsing the device 
   configuration JSON file */
struct cfg_device {
	union {
		struct {
			uint32_t enabled: 1;
			uint32_t module: 1;
		}; 
		uint32_t opt;	
	};

	uint8_t ilat;
	uint8_t ipre;
	uint8_t imode;
	uint8_t irate;
	uint8_t model;   /* reference to a database device model */
	uint8_t tag; 
	uint8_t ledno; 
	uint8_t tbias;
	uint8_t event;
	uint8_t grp[4]; /* list of groups */	
	uint32_t addr_bmp[160 / 32]; /* list of addresses */
};

static const struct microjs_attr_desc device_desc[] = {
	{ "model", MICROJS_JSON_STRING, 0, offsetof(struct cfg_device, model),
		microjs_const_str_enc },
	{ "tag", MICROJS_JSON_STRING, 0, offsetof(struct cfg_device, tag),
		microjs_const_str_enc },
	{ "enabled", MICROJS_JSON_BOOLEAN, 0, offsetof(struct cfg_device, opt),
		microjs_bit_enc },
	{ "ilat", MICROJS_JSON_INTEGER, 0, offsetof(struct cfg_device, ilat),
		microjs_u8_enc },
	{ "ipre", MICROJS_JSON_INTEGER, 0, offsetof(struct cfg_device, ipre),
		microjs_u8_enc },
	{ "irate", MICROJS_JSON_INTEGER, 0, offsetof(struct cfg_device, irate),
		microjs_u8_enc },
	{ "imode", MICROJS_JSON_INTEGER, 0, offsetof(struct cfg_device, imode),
		microjs_u8_enc },
	{ "tbias", MICROJS_JSON_INTEGER, 0, offsetof(struct cfg_device, tbias),
		microjs_u8_enc },
	{ "group", MICROJS_JSON_ARRAY, 4, offsetof(struct cfg_device, grp), 
		microjs_array_u8_enc },
	{ "addr", MICROJS_JSON_ARRAY, 0, offsetof(struct cfg_device, addr_bmp), 
		cfg_device_addr_enc },
	{ "event", MICROJS_JSON_STRING, 0, offsetof(struct cfg_device, event),
		cfg_device_event_enc },
	{ "ledno", MICROJS_JSON_INTEGER, 0, offsetof(struct cfg_device, ledno),
		microjs_u8_enc },
	{ "", 0, 0, 0, NULL},
};

static int cfg_device_list_add(struct cfg_device * cdev)
{
	struct ss_device * dev;
	struct db_dev_model * mod;
	unsigned int addr;
	int mod_idx;
	int tbias;
	int icfg;

	if ((mod_idx = db_dev_model_index_by_name(db_info_get(), 
											  cdev->model)) < 0) {
		DCC_LOG1(LOG_WARNING, "invalid model: %d", cdev->model);
		printf("Invalid model: \"%s\"\n", const_str(cdev->model));
		return mod_idx;
	}

	DCC_LOG2(LOG_INFO, "model=%d idx=%d", cdev->model, mod_idx);

	mod = db_dev_model_by_index(db_info_get(), mod_idx);
	printf("%c \"%s\" \"%s\":", cdev->enabled ? '+' : '-', 
		   const_str(mod->model), const_str(mod->desc));

	if (mod->module != cdev->module) {
		DCC_LOG(LOG_WARNING, "mod->module != cdev->module");
		printf(" not a %s!\n", cdev->module ? "module" : "sensor");
		return -1;
	}

	tbias = (cdev->tbias * 128) / 100;
	icfg = (cdev->irate << 5) + (cdev->imode & 0x1f);

	DCC_LOG4(LOG_INFO, "grp={%d %d %d %d}", 
			 cdev->grp[0], cdev->grp[1], cdev->grp[2], cdev->grp[3]);

	for (addr = 0; addr < 160; ++addr) {
		if (!(cdev->addr_bmp[addr / 32] & (1 << (addr % 32))))
			continue;

		DCC_LOG1(LOG_INFO, "addr=%d", addr);
		printf(" %d", addr);

		dev = slcdev_ssdev_getinstance(cdev->module, addr);

		if (dev->addr != addr) {
			DCC_LOG2(LOG_WARNING, "addr(%d) != dev->addr", 
					 addr, dev->addr);
			printf("Internal error!\n");
			return -1;
		};

		if (dev->module != cdev->module) {
			DCC_LOG2(LOG_WARNING, "module(%d) != dev->module", 
					 addr, dev->addr);
			printf("Internal error!\n");
			return -1;
		};

		dev->model = mod_idx;
		dev->pw1 = device_db_pw_lookup(mod->pw1lst, 0);
		dev->pw2 = device_db_pw_lookup(mod->pw2lst, 0);
		dev->pw3 = device_db_pw_lookup(mod->pw3lst, 0);
		dev->pw4 = device_db_pw_lookup(mod->pw4lst, 0);
		dev->pw5 = device_db_pw_lookup(mod->pw5lst, 0);

		dev->ilat = cdev->ilat;
		dev->ipre = cdev->ipre;
		dev->icfg = icfg;
		dev->tbias = tbias;
		dev->grp[0] = cdev->grp[0];
		dev->grp[1] = cdev->grp[1];
		dev->grp[2] = cdev->grp[2];
		dev->grp[3] = cdev->grp[3];
		/* mark the device as configured */
		dev->cfg = 1;
		/* LED */
		dev->ledno = cdev->ledno;
		/* event */
		dev->event = cdev->event;
		/* AP enabled */
		dev->apen = mod->ap;
		/* enable/disable the device per configuration */
		if (cdev->enabled) 
			slcdev_ssdev_enable(dev);

		DCC_LOG3(LOG_INFO, "%s %d: enabled=%d", 
				 dev->module ? "module" : "sensor", dev->addr, dev->enabled);
	}

	printf("\n");

	return 0;
}

/* Encode a device list (sensors or modules) */
int cfg_device_enc(struct microjs_json_parser * jsn, 
				   struct microjs_val * val, 
				   unsigned int opt, void * ptr)
{
	struct cfg_device cdev;
	int ret;

	memset(&cdev, 0, sizeof(struct cfg_device));
	cdev.module = opt;
	cdev.ilat = ILAT_DEFAULT;
	cdev.ipre = IPRE_DEFAULT;
	cdev.irate = ISINK_RATE_NORMAL >> 5;
	cdev.imode = ISINK_CURRENT_NOM;
	cdev.tbias = 100; /* 100 % */

	DCC_LOG(LOG_TRACE, "<<<");

	if ((ret = microjs_json_parse_obj(jsn, device_desc, &cdev)) < 0) {
		DCC_LOG(LOG_ERROR, "microjs_json_parse_obj() failed!");
		return ret;
	}

	DCC_LOG(LOG_TRACE, ">>>");

	return cfg_device_list_add(&cdev);
}


static const struct microjs_attr_desc switch_desc[] = {
	{ "up", MICROJS_JSON_ARRAY, 0, 
		offsetof(struct usr_switch, up), cfg_script_enc },
	{ "down", MICROJS_JSON_ARRAY, 1, 
		offsetof(struct usr_switch, down), cfg_script_enc },
	{ "off", MICROJS_JSON_ARRAY, 2, 
		offsetof(struct usr_switch, off), cfg_script_enc },
	{ "", 0, 0, 0, NULL},
};

int cfg_switch_enc(struct microjs_json_parser * jsn, 
				   struct microjs_val * val, 
				   unsigned int opt, void * ptr)
{
	struct usr_switch * sw;
	int ret;

	DCC_LOG1(LOG_TRACE, "SW%d", opt + 1);
	sw = &usr.sw[opt];
	memset(sw, 0, sizeof(struct usr_switch));

	if ((ret = microjs_json_parse_obj(jsn, switch_desc, sw)) < 0) {
		DCC_LOG(LOG_ERROR, "microjs_json_parse_obj() failed!");
		return ret;
	}

	return ret;
}

int cfg_events_enc(struct microjs_json_parser * jsn, 
				 struct microjs_val * val, 
				 unsigned int opt, void * ptr)
{
	int ret;

	if ((ret = microjs_json_parse_obj(jsn, events_desc, &usr)) < 0) {
		DCC_LOG(LOG_ERROR, "microjs_json_parse_obj() failed!");
		return ret;
	}

	return ret;
}

struct cfg_trig {
	uint8_t module;
	uint8_t sensor;
	uint8_t addr;
	uint8_t * script;
};

static const struct microjs_attr_desc trig_desc[] = {
	{ "module", MICROJS_JSON_BOOLEAN, 0, 
		offsetof(struct cfg_trig, module), microjs_bit_enc},
	{ "sensor", MICROJS_JSON_BOOLEAN, 0, 
		offsetof(struct cfg_trig, sensor), microjs_bit_enc},
	{ "addr", MICROJS_JSON_INTEGER, 0, 
		offsetof(struct cfg_trig, addr), microjs_u8_enc },
	{ "script", MICROJS_JSON_ARRAY, 8, 
		offsetof(struct cfg_trig, script), cfg_script_enc },
	{ "", 0, 0, 0, NULL},
};


int cfg_trig_enc(struct microjs_json_parser * jsn, 
				 struct microjs_val * val, 
				 unsigned int opt, void * ptr)
{
	struct cfg_trig cfg;
	int ret;

	memset(&cfg, 0, sizeof(struct cfg_trig));

	if ((ret = microjs_json_parse_obj(jsn, trig_desc, &cfg)) < 0) {
		DCC_LOG(LOG_ERROR, "microjs_json_parse_obj() failed!");
		return ret;
	}

	trig_addr_set(cfg.addr);
	trig_module_set(cfg.module);
	trig_sensor_set(cfg.sensor);
	usr.trig = cfg.script;

	return ret;
}


static const struct microjs_attr_desc cfg_desc[] = {
	{ "info", MICROJS_JSON_OBJECT, 0, 0, cfg_info_enc },
	{ "sensor", MICROJS_JSON_OBJECT, 0, 0, cfg_device_enc },
	{ "module", MICROJS_JSON_OBJECT, 1, 0, cfg_device_enc },
	{ "sw1", MICROJS_JSON_OBJECT, 0, 0, cfg_switch_enc },
	{ "sw2", MICROJS_JSON_OBJECT, 1, 0, cfg_switch_enc },
	{ "events", MICROJS_JSON_OBJECT, 0, 0, cfg_events_enc },
	{ "trigger", MICROJS_JSON_OBJECT, 0, 0, cfg_trig_enc },
	{ "", 0, 0, 0, NULL},
};

#define JSON_TOK_BUF_MAX 384

/* lables recognized by the JSON scanner */
const char * const cfg_labels[] = {
	"sensor",
	"module",
	"model",
	"group",
	"enabled",
	"addr",
	"pw1",
	"pw2",
	"pw3",
	"pw4",
	"pw5",
	"irate",
	"imode",
	"ilat",
	"ipre",
	"tbias",
	"tag",
	"sw1",
	"sw2",
	"on",
	"off",
	"up",
	"down",
	"script",
	"init",
	"events",
	"event",
	"tmr1",
	"tmr2",
	"tmr3",
	"tmr4",
	"usr1",
	"usr2",
	"usr3",
	"usr4",
	"usr5",
	"usr6",
	"usr7",
	"usr8",
	"trigger",
	"ledno",
//	"rem",
	"info",
	"desc",
	"version",
	"author",
	NULL	
};

int config_compile(struct fs_file * json)
{
	struct microjs_json_parser jsn;
	uint8_t tok_buf[JSON_TOK_BUF_MAX];
	int ret;

	microjs_json_init(&jsn, tok_buf, JSON_TOK_BUF_MAX, cfg_labels);

	if ((ret = microjs_json_open(&jsn, (char *)json->data, json->size)) < 0) {
		DCC_LOG(LOG_ERROR, "microjs_json_open() failed!");
		return ret;
	}

	/* initialize stack */
	cfg_stack = CFG_STACK_LIMIT;

	/* uncofigure all devices */
	dev_sim_uncofigure_all();

	/* clear user info */
	memset(&usr, 0, sizeof(usr));

	/* skip to the object oppening to allow object-by-object parsing */
	microjs_json_flush(&jsn);

	/* parse the JASON file with the microjs tokenizer */
	while ((ret = microjs_json_scan(&jsn)) == MICROJS_OK) {
		/* decode the token stream */
		if ((ret = microjs_json_parse_obj(&jsn, cfg_desc, NULL)) < 0) {
			DCC_LOG(LOG_ERROR, "microjs_json_parse_obj() failed!");
			microjs_json_print(stdout, &jsn);
//			microjs_json_dump(stdout, &jsn);
			return ret;
		}
		microjs_json_flush(&jsn);
	}

	if (ret != MICROJS_EMPTY_STACK) {
		DCC_LOG(LOG_ERROR, "microjs_json_scan() failed!");
		return ret;
	}

	return 0;
}

struct cfg_info * cfg_info_get(void)
{
	struct fs_dirent entry;
	struct cfg_info * inf;

	fs_dirent_get(&entry, FLASHFS_CFG_BIN);
	if (entry.fp->size == 0)
		return NULL;

	inf = (struct cfg_info *)(entry.fp->data);
	if (inf->magic != CFG_BLK_MAGIC)
		return NULL;

	return inf;
}

int config_load(void)
{
	struct cfg_info * inf;
	unsigned int size;
	const void * ptr;

	if ((inf = cfg_info_get()) == NULL)
		return -1;

	/* read device table */
	ptr = ((const void *)inf) + sizeof(struct cfg_info);
	memcpy(ss_dev_tab, ptr, sizeof(ss_dev_tab));
	size = (sizeof(ss_dev_tab) + 3) & ~3;

	/* read user config */
	ptr += size;
	memcpy(&usr, ptr, sizeof(usr));
	size = sizeof(usr);

	/* read trigger config */
	ptr += size;
	memcpy(&slcdev_drv.trig, ptr, sizeof(struct slcdev_trig));
	size = sizeof(struct slcdev_trig);

	/* read symbol table */
	ptr += size;
	memcpy(slcdev_symbuf, ptr, sizeof(slcdev_symbuf));

	slcdev_update_ap();

	return 0;
}

int config_save(struct fs_file * json)
{
	struct fs_dirent entry;
	struct cfg_info inf;
	unsigned int offs;
	unsigned int size;
	int ret;

	fs_dirent_get(&entry, FLASHFS_CFG_BIN);

	/* reserve space for file entry */
	offs = entry.blk_offs + sizeof(struct fs_file);
	DCC_LOG1(LOG_TRACE, "off=0x%06x", offs);


	/* write device table */
	offs += sizeof(struct cfg_info); /* reserve space for config info */
	size = sizeof(ss_dev_tab);
	if ((ret = stm32_flash_write(offs, ss_dev_tab, size)) < 0) {
		DCC_LOG(LOG_WARNING, "stm32_flash_write() failed!");
		return -1;
	}

	/* write user config */
	offs += size;
	size = sizeof(usr);
	if ((ret = stm32_flash_write(offs, &usr, size)) < 0) {
		DCC_LOG(LOG_WARNING, "stm32_flash_write() failed!");
		return -1;
	}

	/* write trigger config */
	offs += size;
	size = sizeof(struct slcdev_trig);
	if ((ret = stm32_flash_write(offs, &slcdev_drv.trig, size)) < 0) {
		DCC_LOG(LOG_WARNING, "stm32_flash_write() failed!");
		return -1;
	}

	/* write symbol table */
	offs += size;
	size = sizeof(slcdev_symbuf);
	if ((ret = stm32_flash_write(offs, &slcdev_symbuf, size)) < 0) {
		DCC_LOG(LOG_WARNING, "stm32_flash_write() failed!");
		return -1;
	}

	inf.magic = CFG_BLK_MAGIC;
	if (json == NULL) {
		inf.json_crc = 0;
		inf.json_len = 0;
		inf.json_txt = NULL;
	} else {
		inf.json_crc = json->crc;
		inf.json_len = json->size;
		inf.json_txt = (char *)json->data;
	}

	/* write config info */
	offs = entry.blk_offs + sizeof(struct fs_file);
	if ((ret = stm32_flash_write(offs, &inf, sizeof(struct cfg_info))) < 0) {
		DCC_LOG(LOG_WARNING, "stm32_flash_write() failed!");
	}

	size = cfg_stack - sizeof(struct fs_file);
	return fs_file_commit(&entry, size);
}

bool config_is_sane(void)
{
	return (cfg_info_get() == NULL) ? false : true;
}

bool config_need_update(struct fs_file * json)
{
	struct cfg_info * inf;

	if ((inf = cfg_info_get()) == NULL)
		return true;

	/* check configuration integrity */
	if (inf->json_txt != (char *)json->data ||
		inf->json_crc != json->crc || inf->json_len != json->size)
		return true;

	return false;
}

int config_show_info(FILE * f)
{
	struct cfg_info * inf;

	if ((inf = cfg_info_get()) == NULL)
		return -1;
	fprintf(f, " -     Tag: \"%s\"\n", const_str(usr.cfg_info.tag));
	fprintf(f, " -  Author: \"%s\"\n", const_str(usr.cfg_info.author));
	fprintf(f, " -    Desc: \"%s\"\n", const_str(usr.cfg_info.desc));
	fprintf(f, " - Version: %d.%d.%d\n", 
			usr.cfg_info.version[0], 
			usr.cfg_info.version[1], 
			usr.cfg_info.version[2]);
	fprintf(f, " - JSON: txt=0x%08x len=%d crc=0x%04x\n", 
			(uint32_t)inf->json_txt, inf->json_len, inf->json_crc);

	return 0;
}

