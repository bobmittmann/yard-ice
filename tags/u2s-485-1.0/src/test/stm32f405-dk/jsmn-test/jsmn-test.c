#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stm32f.h>
#include <arch/cortex-m3.h>
#include <sys/delay.h>
#include <sys/serial.h>
#include <sys/param.h>
#include <sys/file.h>
#include <sys/dcclog.h>
#include <sys/usb-cdc.h>
#include <sys/tty.h>

#include <thinkos.h>

#include "jsmn.h"

#define TOK_MAX 1024
#define STR_LEN_MAX 128

char * json_token_tostr(char *js, jsmntok_t *t)
{
	static char s[STR_LEN_MAX + 1];
	int n;

	n = t->end - t->start;
	if (n > STR_LEN_MAX)
		n = STR_LEN_MAX;

	memcpy(s, js + t->start, t->end - t->start);
	s[n] = '\0';

	return s;
}

int json_walk_node(char * js, jsmntok_t *t, int lvl);

int json_walk_object(char * js, jsmntok_t *t, int lvl)
{
	int len;
	int n;
	int i;
	int j;

	n = t->size;

//	printf("OBJECT: (size=%d)\n", n);

	printf("{\n");

	if (n == 0)
		return 0;

	if (n % 2 != 0) {
		printf("Invalid response: object must have even number of children.");
		return -1;
	}

	lvl++;

	t++;
	len = 1;

	for (i = 0; i < n; i += 2) {
		int r;

		if (t->type != JSMN_STRING) {
			printf("Invalid response: object keys must be strings.");
			return -1;
		}

		if (i != 0)
			printf(",\n");

		for (j = 0; j < lvl; ++j)
			printf("  ");

		printf("\"%s\": ", json_token_tostr(js, t));
		t++;
		len++;

		r = json_walk_node(js, t, lvl);
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

int json_walk_array(char * js, jsmntok_t *t, int lvl)
{
	int n;
	int i;
	int len;

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

		r = json_walk_node(js, t, lvl);
		if (r < 0)
			return -1;

		t += r;
		len += r;
	}

	printf("]");

	return len;
}

int json_walk_node(char * js, jsmntok_t *t, int lvl)
{
	if (t->type == JSMN_OBJECT) {
		return json_walk_object(js, t, lvl);
	}

	if (t->type == JSMN_ARRAY) {
		return json_walk_array(js, t, lvl);
	}

	if (t->type == JSMN_STRING) {
//		printf("STRING: \"%s\"\n", json_token_tostr(js, t));
		printf("\"%s\"", json_token_tostr(js, t));
		return 1;
	}
		
	if (t->type == JSMN_PRIMITIVE) {
//		printf("PRIMITIVE: \"%s\"\n", json_token_tostr(js, t));
		printf("%s", json_token_tostr(js, t));
		return 1;
	}

	printf("Invalid response:");
	return -1;

}

int json_traverse(char * js, jsmntok_t *t)
{
	int ret;

	if (t->type != JSMN_OBJECT) {
		printf("Invalid response: root element must be an object.");
		return -1;
	}

	ret = json_walk_object(js, t, 0);

	printf("\n");

	return ret;
}

int main(int argc, char ** argv)
{
	usb_cdc_class_t * cdc;
	FILE * f_tty;
	FILE * f_raw;
	struct tty_dev * tty;
	int i;

	DCC_LOG_INIT();
	DCC_LOG_CONNECT();

	DCC_LOG(LOG_TRACE, "1. cm3_udelay_calibrate()");
	cm3_udelay_calibrate();

	DCC_LOG(LOG_TRACE, "2. thinkos_init()");
	thinkos_init(THINKOS_OPT_PRIORITY(8) | THINKOS_OPT_ID(7));

	DCC_LOG(LOG_TRACE, "usb_cdc_init()");
	cdc = usb_cdc_init(&stm32f_otg_fs_dev, *((uint64_t *)STM32F_UID));

	DCC_LOG(LOG_TRACE, "usb_cdc_fopen()");
	f_raw = usb_cdc_fopen(cdc);

	tty = tty_attach(f_raw);
	f_tty = tty_fopen(tty);

	for (i = 0; ;i++) {
//		usb_xmodem_recv(cdc);
		thinkos_sleep(2000);
		fprintf(f_tty, "[%d] hello world...\n", i);
	}


	return 0;
}


#if 0
char * json_load(const char * path, int * plen)
{
	char buf[1024];
	char * json = NULL;
	int lastlen = 0;
	int len;
	FILE *f;

	if ((f = fopen(path ,"r")) == NULL) {
		fprintf(stderr, "fopen(\"%s\") failed!\n", path);
		exit(1);
	}

	while((len = fread(buf, 1, 1024, f)) > 0) {
		json = realloc(json,lastlen + len);
		memcpy(json + lastlen, buf, len);
		lastlen +=len;
	}
	fclose(f);

	if (plen != NULL)
		*plen = lastlen;

	return json;
}


int main(int argc, char **argv)
{
	jsmn_parser p;
	jsmntok_t tok[TOK_MAX];
	char * js;
	int len;
	int r;

	js = json_load(argv[1], &len);
	printf("got[%.*s]\n", len, js);

	while (len > 0) {

		jsmn_init(&p);

		r = jsmn_parse(&p, js, len, tok, TOK_MAX);

		if (r == JSMN_ERROR_NOMEM) {
			fprintf(stderr, "Err: Not enough tokens were provided!\n");
			return 1;
		}

		if (r == JSMN_ERROR_INVAL) {
			fprintf(stderr, "Err: Invalid character!\n");
			return 1;
		}

		if (r == JSMN_ERROR_PART) {
			fprintf(stderr, "Err: not a full JSON packet!\n");
			return 1;
		}

		if (r == 0) {
			fprintf(stderr, "Warn: empty JSON packet!\n");
			return 1;
		}

		if (json_traverse(js, tok) <= 0) {
			fprintf(stderr, "Err: tree traversal failed!\n");
			return 1;
		}
	
		js += tok[0].end;
		len -= tok[0].end;
	
/*
		printf("Parsed result %d\n", r);
		printf("len = %d\n", len);
		printf("p.pos = %d\n", p.pos);
		printf("tok[0].end = %d\n", tok[0].end);

		tok[0].start = JSMN_NULL;
		tok[0].end = JSMN_NULL; */
	}

	return 0;
}
#endif

