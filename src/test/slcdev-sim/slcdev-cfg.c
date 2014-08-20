#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <microjs.h>

#include <sys/dcclog.h>

#include "crc.h"
#include "slcdev.h"
#include "isink.h"

uint16_t cfg_stack = FLASH_BLK_DEV_DB_BIN_SIZE;
uint16_t cfg_heap = 0;

/* Basic field (attribute) types */
enum {
	CFG_VOID,
	CFG_BIT,
	CFG_BFIELD8,
	CFG_BFIELD16,
	CFG_BFIELD32,
	CFG_UINT8,
	CFG_UINT16,
	CFG_UINT32
};

#define BFIELD_OPT(BITS, POS) ((((BITS - 1) & 0x1f) << 5) | (POS & 0x1f))


struct cfg_type {
	int (* encode)(const char * s, int opt, void * ptr); /* encode string into 
												   attribute */
	int (* decode)(char * s, int opt, void * ptr); /* serialize into string */
};

struct cfg_attr {
	char name[11];
	uint8_t type;
	uint16_t opt;
	uint16_t offs;
};

#define BFIELD32(NM, OBJ, FIELD, POS, BITS) \
	 { NM, CFG_BFIELD32, BFIELD_OPT(BITS, POS), offsetof(OBJ, FIELD) }

#define BFIELD16(NM, OBJ, FIELD, POS, BITS) \
	 { NM, CFG_BFIELD16, BFIELD_OPT(BITS, POS), offsetof(OBJ, FIELD) }

#define BFIELD8(NM, OBJ, FIELD, POS, BITS) \
	 { NM, CFG_BFIELD8, BFIELD_OPT(BITS, POS), offsetof(OBJ, FIELD) }

#define BIT(NM, OBJ, FIELD, POS) \
	 { NM, CFG_BIT, POS, offsetof(OBJ, FIELD) }

int cfg_bit_encode(const char * s, int bit, void * ptr)
{
	uint32_t * bfield = (uint32_t *)ptr;

	if ((strcmp(s, "yes") == 0) || (strcmp(s, "true") == 0) || 
		(strcmp(s, "1") == 0))
		*bfield |= 1 << bit;
	else if ((strcmp(s, "no") == 0) || (strcmp(s, "false") == 0) || 
			 (strcmp(s, "0") == 0))
		*bfield &= ~(1 << bit);
	else
		return -EINVAL;

	return 0;
} 

int cfg_bit_decode(char * s, int bit, void * ptr)
{
	uint32_t * val = (uint32_t *)ptr;

	return sprintf(s, "%s", (*val & (1 << bit)) ? "yes" : "no");
} 

int cfg_bfield8_encode(const char * s, int opt, void * ptr)
{
	uint32_t mask = 0xffffffff >> (31 - ((opt >> 5) & 0x1f));
	uint8_t * bfield = (uint8_t *)ptr;
	int shift = opt & 0x1f;
	uint32_t val;

	val = strtoul(s, NULL, 0);

	*bfield = (*bfield & ~(mask << shift)) | ((val & mask) << shift);

	return 0;
} 

int cfg_bfield8_decode(char * s, int opt, void * ptr)
{
	uint32_t mask = 0xffffffff >> (31 - ((opt >> 5) & 0x1f));
	uint8_t * bfield = (uint8_t *)ptr;
	int shift = opt & 0x1f;

	return sprintf(s, "%d", (*bfield >> shift) & mask);
} 


int cfg_bfield16_encode(const char * s, int opt, void * ptr)
{
	uint32_t mask = 0xffffffff >> (31 - ((opt >> 5) & 0x1f));
	uint16_t * bfield = (uint16_t *)ptr;
	int shift = opt & 0x1f;
	uint32_t val;

	val = strtoul(s, NULL, 0);

	*bfield = (*bfield & ~(mask << shift)) | ((val & mask) << shift);

	return 0;
} 

int cfg_bfield16_decode(char * s, int opt, void * ptr)
{
	uint32_t mask = 0xffffffff >> (31 - ((opt >> 5) & 0x1f));
	uint16_t * bfield = (uint16_t *)ptr;
	int shift = opt & 0x1f;

	return sprintf(s, "%d", (*bfield >> shift) & mask);
} 


int cfg_bfield32_encode(const char * s, int opt, void * ptr)
{
	uint32_t mask = 0xffffffff >> (31 - ((opt >> 5) & 0x1f));
	uint32_t * bfield = (uint32_t *)ptr;
	int shift = opt & 0x1f;
	uint32_t val;

	val = strtoul(s, NULL, 0);

	*bfield = (*bfield & ~(mask << shift)) | ((val & mask) << shift);

	return 0;
} 

int cfg_bfield32_decode(char * s, int opt, void * ptr)
{
	uint32_t mask = 0xffffffff >> (31 - ((opt >> 5) & 0x1f));
	uint32_t * bfield = (uint32_t *)ptr;
	int shift = opt & 0x1f;

	return sprintf(s, "%d", (*bfield >> shift) & mask);
} 



int cfg_uint8_encode(const char * s, int opt, void * ptr)
{
	uint8_t * val = (uint8_t *)ptr;

	*val = strtoul(s, NULL, 0);

	return 0;
} 

int cfg_uint8_decode(char * s, int hex, void * ptr)
{
	uint8_t * val = (uint8_t *)ptr;

	if (hex)
		return sprintf(s, "0x%02x", *val);

	return sprintf(s, "%d", *val);
} 

int cfg_uint16_encode(const char * s, int opt, void * ptr)
{
	uint16_t * val = (uint16_t *)ptr;

	*val = strtoul(s, NULL, 0);

	return 0;
} 

int cfg_uint16_decode(char * s, int hex, void * ptr)
{
	uint16_t * val = (uint16_t *)ptr;

	if (hex)
		return sprintf(s, "0x%04x", *val);

	return sprintf(s, "%d", *val);
} 

int cfg_uint32_encode(const char * s, int opt, void * ptr)
{
	uint16_t * val = (uint16_t *)ptr;

	*val = strtoul(s, NULL, 0);

	return 0;
} 

int cfg_uint32_decode(char * s, int hex, void * ptr)
{
	uint16_t * val = (uint16_t *)ptr;

	if (hex)
		return sprintf(s, "0x%04x", *val);

	return sprintf(s, "%d", *val);
} 


const struct cfg_type cfg_type_lut[] = {
	{ NULL, NULL }, /* VOID */
	{ cfg_bit_encode, cfg_bit_decode },      /* BIT */
	{ cfg_bfield8_encode, cfg_bfield8_decode }, /* BFIELD8 */
	{ cfg_bfield16_encode, cfg_bfield16_decode }, /* BFIELD16 */
	{ cfg_bfield32_encode, cfg_bfield32_decode }, /* BFIELD32 */
	{ cfg_uint8_encode, cfg_uint8_decode },  /* UINT8 */
	{ cfg_uint16_encode, cfg_uint16_decode }, /* UINT16 */
	{ cfg_uint32_encode, cfg_uint32_decode } /* UINT32 */
};

const struct cfg_attr dev_attr_lut[] = {
	BFIELD32( "addr", struct ss_device, opt,  0, 8), /* RO */
	BFIELD32("model", struct ss_device, opt,  8, 6), /* RO */
	BIT(        "ap", struct ss_device, opt, 14),    /* RO */
	BIT(    "module", struct ss_device, opt, 15),    /* RO */
	BIT(   "enabled", struct ss_device, opt, 16),
	BIT(       "led", struct ss_device, opt, 17),
	BIT(     "pw5en", struct ss_device, opt, 18),
	BIT(       "tst", struct ss_device, opt, 19),
	{ "tbias",   CFG_UINT8,  0, offsetof(struct ss_device, tbias) },
	{ "icfg",    CFG_UINT8,  1, offsetof(struct ss_device, icfg) },
	{ "ipre",    CFG_UINT8,  0, offsetof(struct ss_device, ipre) },
	{ "ilat",    CFG_UINT8,  0, offsetof(struct ss_device, ilat) },
	{ "pw1",     CFG_UINT16, 0, offsetof(struct ss_device, pw1) },
	{ "pw2",     CFG_UINT16, 0, offsetof(struct ss_device, pw2) },
	{ "pw3",     CFG_UINT16, 0, offsetof(struct ss_device, pw3) },
	{ "pw4",     CFG_UINT16, 0, offsetof(struct ss_device, pw4) },
	{ "pw5",     CFG_UINT16, 0, offsetof(struct ss_device, pw5) },
	{ "", CFG_VOID, 0}
};


int object_dump(void * obj, const struct cfg_attr attr[], FILE * f)
{
	int i;

	if (obj == NULL)
		return -1;

	for (i = 0; (attr[i].type != CFG_VOID); ++i) {
		const struct cfg_attr * a = &attr[i];
		char s[16];

		cfg_type_lut[a->type].decode(s, a->opt, obj + a->offs);
		fprintf(f, "%s = %s\n", a->name, s);
	}

	return 0;
}

static const struct cfg_attr * attr_lookup(const struct cfg_attr attr[], 
										   const char * name)
{
	int i;

	if (name == NULL)
		return NULL;

	for (i = 0; (attr[i].type != CFG_VOID); ++i) {
		const struct cfg_attr * a = &attr[i];
		if (strcmp(a->name, name) == 0)
			return a;
	}

	return NULL;
}

int object_attr_set(void * obj, const struct cfg_attr attr[], 
					const char * name, const char * val) 
{
	const struct cfg_attr * a;

	if ((obj == NULL) | (val == NULL))
		return -EINVAL;

	if ((a = attr_lookup(attr, name)) == NULL)
		return -EINVAL;

	return cfg_type_lut[a->type].encode(val, a->opt, obj + a->offs);
}

int object_attr_get(void * obj, const struct cfg_attr attr[], 
					const char * name, char * val) 
{
	const struct cfg_attr * a;

	if ((obj == NULL) | (val == NULL))
		return -EINVAL;

	if ((a = attr_lookup(attr, name)) == NULL)
		return -EINVAL;

	return cfg_type_lut[a->type].decode(val, a->opt, obj + a->offs);
}


int device_dump(FILE * f, unsigned int addr)
{
	return object_dump(&ss_dev_tab[addr], dev_attr_lut, f);
}

int device_attr_set(int addr, const char * name, const char * val)
{
	return object_attr_set(&ss_dev_tab[addr], dev_attr_lut, name, val);
}

int device_attr_print(FILE * f, unsigned int addr, const char * name)
{
	char val[16];
	int ret;

	if ((ret = object_attr_get(&ss_dev_tab[addr], dev_attr_lut, 
							  name, val)) > 0) {
		fprintf(f, "dev[%d].%s = %s\n", addr, name, val);
	}

	return ret;
}

int config_dump(FILE * f)
{
	DCC_LOG(LOG_TRACE, "...");

	return 0;
}
/*
#define CFG_MAGIC 0x90f93e9a

struct cfg_info {
	uint32_t magic;
	uint16_t json_crc;
	uint16_t json_len;
	uint16_t dev_offs;
	uint16_t dev_crc;
};
*/

#if 0
{
	DCC_LOG1(LOG_TRACE, "configuration size = %d.", sizeof(struct devsim_cfg));
	uint32_t blk_offs = FLASH_BLK_DEV_DB_BIN_OFFS;
	uint32_t blk_size = FLASH_BLK_DEV_DB_BIN_SIZE;
	int ret;

	if ((ret = stm32_flash_erase(blk_offs, blk_size)) < 0) {
		DCC_LOG(LOG_ERROR, "stm32_flash_erase() failed!");
		return ret;
	};

	DCC_LOG(LOG_TRACE, "configuration erased!");

	return ret;
	return 0;
}
#endif

int config_erase(void)
{
	uint32_t blk_offs = FLASH_BLK_CFG_BIN_OFFS;
	uint32_t blk_size = FLASH_BLK_CFG_BIN_SIZE;
	int ret;

	if ((ret = stm32_flash_erase(blk_offs, blk_size)) < 0) {
		DCC_LOG(LOG_ERROR, "stm32_flash_erase() failed!");
		return ret;
	};

	DCC_LOG(LOG_TRACE, "configuration erased!");

	return ret;
}

/* This is an auxiliarly structure for parsing the device 
   configuration JSON file */
struct cfg_device {
	union {
		struct {
			uint32_t enabled: 1;
			uint32_t module: 1;
		}; 
		uint32_t bf_opt;	
	};

	uint8_t ilat;
	uint8_t ipre;
	uint8_t imode;
	uint8_t irate;
	uint8_t model;   /* reference to a device model */
	uint8_t tbias;
};

/* Encode the array of address. This effectivelly write the configuration 
   into the device objects. */
int cfg_device_addr_enc(struct microjs_json_parser * jsn, 
						struct microjs_val * val, 
						unsigned int bit, void * ptr)
{
	struct cfg_device * cdev = (struct cfg_device *)ptr;
	struct ss_device * dev;
	struct db_dev_model * mod;
	int tbias;
	int icfg;
	int cnt = 0;
	int typ;
	int addr;
	int mod_idx;

	DCC_LOG(LOG_TRACE, "...");

	if ((mod_idx = db_dev_model_index_by_name(cdev->model)) < 0) {
		DCC_LOG1(LOG_WARNING, "invalid model: %d", cdev->model);
		return -1;
	}

	mod = db_dev_model_by_index(mod_idx);
	tbias = (cdev->tbias * 128) / 100;
	icfg = (cdev->irate << 5) + (cdev->imode & 0x1f);

	while ((typ = microjs_json_get_val(jsn, val)) == MICROJS_JSON_INTEGER) {
		addr = val->u32;
		DCC_LOG1(LOG_TRACE, "addr=%d", addr);

		if ((addr < 1) || (addr > 159)) {
			DCC_LOG1(LOG_WARNING, "invalid address addr=%d", addr);
			return -1;
		}

		if (cdev->module)
			addr += 160;
	
		dev = &ss_dev_tab[addr];
		/* disable the device prior to configuration */
		dev->enabled = 0;

		dev->model = mod_idx;
		dev->pw1 = device_db_pw1_lookup(mod, 0, tbias);
		dev->pw2 = device_db_pw2_lookup(mod, 0, tbias);
		dev->pw3 = device_db_pw3_lookup(mod, 0, tbias);
		dev->pw4 = device_db_pw4_lookup(mod, 0, tbias);
		dev->pw5 = device_db_pw5_lookup(mod, 0, tbias);

		dev->ilat = cdev->ilat;
		dev->ipre = cdev->ipre;
		dev->icfg = icfg;
		dev->tbias = tbias;

		/* enable the device per configuration */
		dev->enabled = cdev->enabled;
		cnt++;
	}

	if (typ != MICROJS_JSON_END_ARRAY) {
		return -1;
	}

	return cnt;
}

static const struct microjs_attr_desc sensor_desc[] = {
	{ "model", MICROJS_JSON_STRING, 0, offsetof(struct cfg_device, model),
		microjs_const_str_enc },
	{ "enabled", MICROJS_JSON_BOOLEAN, 0, offsetof(struct cfg_device, bf_opt),
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
	{ "addr", MICROJS_JSON_ARRAY, 0, 0, cfg_device_addr_enc },
	{ "", 0, 0, 0, NULL},
};

/* Encode a sensor list */
int cfg_sensor_enc(struct microjs_json_parser * jsn, 
				   struct microjs_val * val, 
				   unsigned int bit, void * ptr)
{
	struct cfg_device cdev;
	int ret;

	DCC_LOG(LOG_TRACE, "...");
	cdev.enabled = 0;
	cdev.model = 0;
	cdev.module = 0;
	cdev.ilat = ILAT_DEFAULT;
	cdev.ipre = IPRE_DEFAULT;
	cdev.irate = ISINK_RATE_NORMAL >> 5;
	cdev.imode = ISINK_CURRENT_NOM;
	cdev.tbias = 100; /* 100 % */

	if ((ret = microjs_json_parse_obj(jsn, sensor_desc, &cdev)) < 0) {
		DCC_LOG(LOG_ERROR, "microjs_json_parse_obj() failed!");
		return ret;
	}

	return 0;
}

/* Encode a module list */
int cfg_module_enc(struct microjs_json_parser * jsn, 
				   struct microjs_val * val, 
				   unsigned int bit, void * ptr)
{
	struct cfg_device cdev;
	int ret;

	DCC_LOG(LOG_TRACE, "...");
	cdev.enabled = 0;
	cdev.model = 0;
	cdev.module = 1;
	cdev.ilat = ILAT_DEFAULT;
	cdev.ipre = IPRE_DEFAULT;
	cdev.irate = ISINK_RATE_NORMAL >> 5;
	cdev.imode = ISINK_CURRENT_NOM;
	cdev.tbias = 100; /* 100 % */

	if ((ret = microjs_json_parse_obj(jsn, sensor_desc, &cdev)) < 0) {
		DCC_LOG(LOG_ERROR, "microjs_json_parse_obj() failed!");
		return ret;
	}

	return 0;
}

static const struct microjs_attr_desc cfg_desc[] = {
	{ "sensor", MICROJS_JSON_OBJECT, 0, 0, cfg_sensor_enc },
	{ "module", MICROJS_JSON_OBJECT, 0, 0, cfg_module_enc },
	{ "", 0, 0, 0, NULL},
};

#define JSON_TOK_BUF_MAX 4096

const char * const cfg_labels[] = {
	"sensor",
	"module",
	"model",
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
	NULL	
};

#define CFG_MAGIC 0xc01fe9da

struct cfg_info {
	uint32_t magic;
	const char * json_txt;
	uint16_t json_crc;
	uint16_t json_len;
	const void * tab_ptr;
	uint16_t tab_size;
	uint16_t tab_crc;
};


int dev_tab_save(struct cfg_info * inf)
{
	const void * ptr;
	int offs;
	int size;
	int ret;

	/* wrtie device table */
	size = (sizeof(ss_dev_tab) + 3) & ~3;
	offs = FLASH_BLK_CFG_BIN_OFFS + FLASH_BLK_CFG_BIN_SIZE - size;

	if ((ret = stm32_flash_write(offs, ss_dev_tab, size)) < 0) {
		DCC_LOG(LOG_WARNING, "stm32_flash_write() failed!");
		return -1;
	}

	ptr = (const void *)(STM32_MEM_FLASH + offs);
	inf->tab_crc = crc16ccitt(0, ptr, size);
	inf->tab_size = size;
	inf->tab_ptr = ptr;

	return ret;
}

int cfg_info_write(const char * txt, unsigned int crc, unsigned int len)
	
{
	struct cfg_info inf;
	int offs;
	int size;
	int ret;

	DCC_LOG(LOG_TRACE, "...");

	inf.magic = CFG_MAGIC;
	inf.json_txt = txt;
	inf.json_crc = crc;
	inf.json_len = len;
	if ((ret = dev_tab_save(&inf)) < 0)
		return ret;

	size = sizeof(struct cfg_info);
	offs = FLASH_BLK_CFG_BIN_OFFS;

	if ((ret = stm32_flash_write(offs, &inf, size)) < 0) {
		DCC_LOG(LOG_WARNING, "stm32_flash_write() failed!");
	}

	return ret;
}

int config_compile(void)
{
	struct microjs_json_parser jsn;
	uint8_t tok_buf[JSON_TOK_BUF_MAX];
	unsigned int json_crc;
	int json_len;
	char * json_txt;
	struct cfg_info * inf;
	int ret;

	DCC_LOG1(LOG_TRACE, "sp=0x%08x ..........................", cm3_sp_get());

	json_txt = (char *)(STM32_MEM_FLASH + FLASH_BLK_SIM_CFG_JSON_OFFS);
	json_len = microjs_json_root_len(json_txt);
	json_crc = crc16ccitt(0, json_txt, json_len);
	(void)json_crc;

	DCC_LOG3(LOG_TRACE, "txt=0x%08x len=%d crc=0x%04x", 
			 json_txt, json_len, json_crc);

	/* check database integrity */
	inf = (struct cfg_info *)(STM32_MEM_FLASH + FLASH_BLK_CFG_BIN_OFFS);
	if ((inf->magic == CFG_MAGIC) && (inf->json_txt == json_txt) && 
		(inf->json_crc == json_crc) && (inf->json_len == json_len)) {
		printf("Configuration is up-to-date.\n");
		return 0;
	}

	printf("Parsing JSON file...\n");

	microjs_json_init(&jsn, tok_buf, JSON_TOK_BUF_MAX, cfg_labels);
	microjs_json_open(&jsn, json_txt, json_len);

	/* parse the JASON file with the microjs tokenizer */
	if ((ret = microjs_json_scan(&jsn)) < 0) {
		DCC_LOG(LOG_ERROR, "microjs_parse() failed!");
		return ret;
	}

	if (microjs_json_get_val(&jsn, NULL) != MICROJS_JSON_OBJECT) {
		DCC_LOG(LOG_ERROR, "root must be an object!");
		return -1;
	}

	/* decode the token stream */
	if ((ret = microjs_json_parse_obj(&jsn, cfg_desc, NULL)) < 0) {
		DCC_LOG(LOG_ERROR, "microjs_json_parse_obj() failed!");
		return ret;
	}

	printf("Erasing old configuration ...\n");

	if ((ret = config_erase()) < 0) {
		DCC_LOG(LOG_ERROR, "config_erase() failed!");
		return ret;
	}

	printf("Saving new configuration ...\n");

	if ((ret = cfg_info_write(json_txt, json_crc, json_len)) < 0) {
		DCC_LOG(LOG_ERROR, "cfg_info_write() failed!");
		return ret;
	}

	return 0;
}

int config_load(void)
{
	struct cfg_info * inf;
	unsigned int offs;
	unsigned int size;
	unsigned int crc;
	const void * ptr;

	/* check configuration integrity */
	inf = (struct cfg_info *)(STM32_MEM_FLASH + FLASH_BLK_CFG_BIN_OFFS);
	/* wrtie device table */
	size = (sizeof(ss_dev_tab) + 3) & ~3;
	offs = FLASH_BLK_CFG_BIN_OFFS + FLASH_BLK_CFG_BIN_SIZE - size;
	ptr = (const void *)(STM32_MEM_FLASH + offs);
	crc = crc16ccitt(0, ptr, size);

	if ((inf->magic == CFG_MAGIC) && (inf->tab_ptr == ptr) && 
		(inf->tab_crc == crc) && (inf->tab_size == size)) {
		memcpy(ss_dev_tab, ptr, sizeof(ss_dev_tab));
		return 0;
	}

	return -1;
}

int config_save(void)
{
	DCC_LOG(LOG_TRACE, "...");
	return cfg_info_write(NULL, 0, 0);
}

int config_show_info(FILE * f)
{
	struct cfg_info * inf;
	unsigned int offs;
	unsigned int size;
	unsigned int crc;
	const void * ptr;

	/* check configuration integrity */
	inf = (struct cfg_info *)(STM32_MEM_FLASH + FLASH_BLK_CFG_BIN_OFFS);
	/* wrtie device table */
	size = (sizeof(ss_dev_tab) + 3) & ~3;
	offs = FLASH_BLK_CFG_BIN_OFFS + FLASH_BLK_CFG_BIN_SIZE - size;
	ptr = (const void *)(STM32_MEM_FLASH + offs);
	crc = crc16ccitt(0, ptr, size);

	fprintf(f, " Info: magic=0x%08x size=%d crc=0x%04x\n", inf->magic,
			inf->tab_size, inf->tab_crc);
	fprintf(f, " JSON: txt=0x%08x len=%d crc=0x%04x\n", 
			(uint32_t)inf->json_txt, inf->json_len, inf->json_crc);

	if ((inf->magic == CFG_MAGIC) && (inf->tab_ptr == ptr) && 
		(inf->tab_crc == crc) && (inf->tab_size == size)) {
		fprintf(f, "Ok.\n");
		return 0;
	}

	fprintf(f, "Invalid configuration!\n");
	fprintf(f, "Table: magic=0x%08x size=%d crc=0x%04x\n", 
			(uint32_t)CFG_MAGIC, size, crc);

	return -1;
}
