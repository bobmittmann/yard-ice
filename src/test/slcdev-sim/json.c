#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/dcclog.h>

#include "json.h"

int json_token_tostr(char * s, unsigned int max, char * js, jsmntok_t *t)
{
	int n;

	n = t->end - t->start;
	if (n > (max - 1))
		n = (max - 1);

	memcpy(s, js + t->start, n);
	s[n] = '\0';

	return n;
}

int json_root_len(char * js)
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


int json_parse_uint16(char * js, jsmntok_t * t, void * ptr)
{
	uint16_t * pval = (uint16_t *)ptr;
	char s[16];

	if (t->start == JSMN_NULL || t->end == JSMN_NULL)
		return -JSON_ERR_INVALID_TOKEN;

	if (t->type != JSMN_PRIMITIVE)
		return -JSON_ERR_NOT_PRIMITIVE;

	if (pval == NULL)
		return -JSON_ERR_NULL_POINTER;

	json_token_tostr(s, sizeof(s), js, t);

	*pval = strtol(s, NULL, 10); 

	DCC_LOG2(LOG_TRACE, "val=%d ptr=%08x", *pval, pval);

	return 1;
}

int json_parse_boolean(char * js, jsmntok_t * t, void * ptr)
{
	uint8_t * pval = (uint8_t *)ptr;
	char s[16];

	if (t->start == JSMN_NULL || t->end == JSMN_NULL)
		return -JSON_ERR_INVALID_TOKEN;

	if (t->type != JSMN_PRIMITIVE)
		return -JSON_ERR_NOT_PRIMITIVE;

	if (pval == NULL)
		return -JSON_ERR_NULL_POINTER;

	json_token_tostr(s, sizeof(s), js, t);

	if (strcmp(s, "true") == 0) 
		*pval = 1;
	else
		if (strcmp(s, "false") == 0) 
			*pval = 0;
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

jsmntok_t * err_tok = NULL;

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

	err_tok = t;

	if (obj == NULL)
		return -JSON_ERR_NULL_POINTER;

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
		int ret;

		if (t->type != JSMN_STRING)
			return -JSON_ERR_KEY_TYPE_INVALID;

		json_token_tostr(s, sizeof(s), js, t);

		t++;
		len++;

		if (t->start == JSMN_NULL || t->end == JSMN_NULL)
			return -JSON_ERR_INVALID_TOKEN;

		err_tok = t;

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

		if ((ret = desc[j].parser(js, t, obj + desc[j].offs)) < 0)
			return ret;

		t += ret;
		len += ret;
	}

	return len;
}

static void json_dump_line(int ln, char * lp)
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

void json_dump_err(char * js, int pos)
{
	char * lp[5];
	int ln;
	int c;
	int i;

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
			json_dump_line(ln - 4, lp[4]);
			json_dump_line(ln - 3, lp[3]);
			json_dump_line(ln - 2, lp[2]);
			json_dump_line(ln - 1, lp[1]);
			json_dump_line(ln - 0, lp[0]);
			break;
		}
	}
}

