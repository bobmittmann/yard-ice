#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <microjs.h>

#include <sys/dcclog.h>

#include "json.h"
#include "crc.h"
#include "slcdev.h"

uint16_t cfg_stack = FLASH_BLK_DEV_DB_BIN_SIZE;
uint16_t cfg_heap = 0;

/* Basic field (attribute) types */
enum {
	CFG_VOID,
	CFG_BIT,
	CFG_BFIELD,
	CFG_UINT8,
	CFG_UINT16
};

#define BFIELD(BITS, POS) ((((BITS - 1) & 0x1f) << 5) | (POS & 0x1f))

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

int cfg_bfield_encode(const char * s, int opt, void * ptr)
{
	uint32_t mask = 0xffffffff >> (31 - ((opt >> 5) & 0x1f));
	int shift = opt & 0x1f;
	uint32_t * bfield = (uint32_t *)ptr;
	uint32_t val;

	val = strtoul(s, NULL, 0);

	*bfield = (*bfield & ~(mask << shift)) | ((val & mask) << shift);

	return 0;
} 

int cfg_bfield_decode(char * s, int opt, void * ptr)
{
	uint32_t * bfield = (uint32_t *)ptr;
	uint32_t mask = 0xffffffff >> (31 - ((opt >> 5) & 0x1f));
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

const struct cfg_type cfg_type_lut[] = {
	{ NULL, NULL }, /* VOID */
	{ cfg_bit_encode, cfg_bit_decode },      /* BIT */
	{ cfg_bfield_encode, cfg_bfield_decode }, /* BFIELD */
	{ cfg_uint8_encode, cfg_uint8_decode },  /* UINT8 */
	{ cfg_uint16_encode, cfg_uint16_decode } /* UINT16 */
};

const struct cfg_attr dev_attr_lut[] = {
	{ "enabled", CFG_BIT,    0, offsetof(struct ss_device, opt) },
	{ "addr",    CFG_BFIELD, BFIELD(9, 1), offsetof(struct ss_device, opt) },
	{ "ap",      CFG_BIT,    10, offsetof(struct ss_device, opt) },
	{ "type",    CFG_BFIELD, BFIELD(6, 11), offsetof(struct ss_device, opt) },
	{ "poll",    CFG_BIT,    17, offsetof(struct ss_device, opt) },
	{ "dev",     CFG_UINT8,  0, offsetof(struct ss_device, dev) },
	{ "tbias",   CFG_UINT8,  0, offsetof(struct ss_device, tbias) },
	{ "icfg",    CFG_UINT8,  1, offsetof(struct ss_device, icfg) },
	{ "ipre",    CFG_UINT8,  0, offsetof(struct ss_device, ipre) },
	{ "ilat",    CFG_UINT8,  0, offsetof(struct ss_device, ilat) },
	{ "pw1",     CFG_UINT16, 0, offsetof(struct ss_device, pw1) },
	{ "pw2",     CFG_UINT16, 0, offsetof(struct ss_device, pw2) },
	{ "pw3",     CFG_UINT16, 0, offsetof(struct ss_device, pw3) },
	{ "pw4",     CFG_UINT16, 0, offsetof(struct ss_device, pw4) },
	{ "pw5",     CFG_UINT16, 0, offsetof(struct ss_device, pw5) },
	{ "usr1",    CFG_UINT16, 0, offsetof(struct ss_device, usr1) },
	{ "usr2",    CFG_UINT16, 0, offsetof(struct ss_device, usr2) },
	{ "usr3",    CFG_UINT16, 0, offsetof(struct ss_device, usr3) },
	{ "usr4",    CFG_UINT16, 0, offsetof(struct ss_device, usr4) },
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

int device_pw3_lookup(unsigned int addr, unsigned int sel)
{
	struct ss_device * dev;
	struct obj_device * obj;
	uint32_t avg;
	uint32_t min;
	uint32_t max;

	dev = &ss_dev_tab[addr];
	obj = device_db_lookup(dev->type);

	if (sel >= obj->pw3->cnt)
		sel = obj->pw3->cnt - 1;

	max = obj->pw3->pw[sel].max;
	min = obj->pw3->pw[sel].min;
	avg = (max + min) / 2;
	dev->pw3 = (avg * dev->tbias) / 128;
	DCC_LOG4(LOG_TRACE, "min=%d max=%d avg=%d val=%d", 
			 min, max, avg, dev->pw3);

	return 0;
}


#if 0
int cmd_eeprom(FILE * f, int argc, char ** argv)
{
	unsigned int offs;
	unsigned int data;

	if (argc > 2)
		return SHELL_ERR_EXTRA_ARGS;

	fprintf(f, "Testing EEPROM...\n");
	stm32_eeprom_unlock();

	data = rand();
	for (offs = 0; offs < 4096; offs += 4) {
		stm32_eeprom_wr32(offs, data);
		data++;
	}
		
	return 0;
}
#endif

int config_dump(FILE * f)
{
	DCC_LOG(LOG_TRACE, "...");

	device_dump(f, 1);
	device_dump(f, 33);

	return 0;
}

int config_erase(void)
{
	DCC_LOG1(LOG_TRACE, "configuration size = %d.", sizeof(struct devsim_cfg));
#if 0
	uint32_t blk_offs = FLASH_BLK_DEV_DB_BIN_OFFS;
	uint32_t blk_size = FLASH_BLK_DEV_DB_BIN_SIZE;
	int ret;

	if ((ret = stm32_flash_erase(blk_offs, blk_size)) < 0) {
		DCC_LOG(LOG_ERROR, "stm32_flash_erase() failed!");
		return ret;
	};

	DCC_LOG(LOG_TRACE, "configuration erased!");

	return ret;
#endif
	return 0;
}

int script_compile(const char * js, unsigned int len)
{
	struct microjs_parser p;
	uint8_t tok[1024];

	microjs_init(&p, tok, 1024);

	microjs_parse(&p, js, len);

	return 0;
};

#define TOK_MAX 1024

int config_compile(void)
{
	unsigned int json_crc;
	int json_len;
	jsmn_parser p;
	jsmntok_t tok[TOK_MAX];
//	struct db_obj * obj;
	jsmntok_t * t;
	char * js;
	int ret;
	int n;
	int i;

	js = (char *)(STM32_MEM_FLASH + FLASH_BLK_SIM_CFG_JSON_OFFS);
	json_len = json_root_len(js);
	json_crc = crc16ccitt(0, js, json_len);

	DCC_LOG2(LOG_TRACE, "js=0x%08x len=%d", js, json_len);
	(void)json_crc;
#if 0
	/* check database integrity */
	inf = (struct obj_db_info *)(STM32_MEM_FLASH + FLASH_BLK_DEV_DB_BIN_OFFS);
	if ((inf->len == sizeof(struct obj_db_info)) && 
		(inf->type == DB_OBJ_DB_INFO) && 
		(inf->json_crc == json_crc) && (inf->json_len == json_len)) {
		printf("Database is up-to-date.\n");
		return 0;
	}
#endif

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

#if 0
	/* erase flash block */
	if (stm32_flash_erase(FLASH_BLK_DEV_DB_BIN_OFFS, 
						  FLASH_BLK_DEV_DB_BIN_SIZE) < 0) {
		DCC_LOG(LOG_ERROR, "stm32_flash_erase() failed!");
		return -1;
	};
#endif

	/* initialise PW descriptor stack */
	cfg_stack = FLASH_BLK_DEV_DB_BIN_SIZE;
	cfg_heap = 0;
	err_tok = t;

	t++;

//	obj = NULL;
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
/*
		if (strcmp(s, "sw1") == 0) {
			ret = cfg_parse_sw(js, t, &obj);
		} else if (strcmp(s, "sw2") == 0) {
			ret = cfg_parse_sw(js, t, &obj);
		} else if (strcmp(s, "group") == 0) {
			ret = cfg_parse_group(js, t, &obj);
		} else {
			ret = -JSON_ERR_INVALID_OBJECT;
			DCC_LOG(LOG_ERROR, "invalid object.");
		}
*/
		if (ret < 0)
			goto compile_error;

		t += ret;
	}


//	cfg_info_write(json_crc, json_len, &obj);

	printf("Configuration databse compiled.\n");
	printf("Free memory: %d.%02d KiB.\n", (cfg_stack - cfg_heap) / 1024, 
		   (((cfg_stack - cfg_heap) % 1024) * 100) / 1024);

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


