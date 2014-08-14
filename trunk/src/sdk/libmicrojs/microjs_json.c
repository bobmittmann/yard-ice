#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define __MICROJS_I__
#include "microjs-i.h"

#include <sys/dcclog.h>

struct __json_obj {
	char key[12];
	int (* parser)(char * js, jsmntok_t * t, void * ptr);
	unsigned int offs;	
};

#define MICROJS_JSON_INVALID 0
#define MICROJS_JSON_OBJECT 1
#define MICROJS_JSON_ARRAY 2
#define MICROJS_JSON_LABEL 3
#define MICROJS_JSON_NUMBER 4
#define MICROJS_JSON_STRING 5
#define MICROJS_JSON_BOOL 6

int microjs_json_init(struct microjs_json_parser * jsn, 
					  struct microjs_tokenizer * tkn)
{
	jsn->tkn = tkn;
	jsn->idx = 0;

	return 0;
}

int microjs_json_expect(struct microjs_json_parser * jsn, unsigned int type)
{
	unsigned int offs;
	int idx = jsn->idx;
	uint32_t x;
	int tok;
	int len;
	(void)len;

	tok = jsn->tkn->tok[idx++];
	if (tok >= TOK_STRING)  {
		len = tok - TOK_STRING;
		offs = p->tok[idx++];
		offs |= p->tok[idx++] << 8;
		if (jsn->tkn->tok[idx++] == TOK_COLON) {
			if (type == MICROJS_JSON_LABEL) {
				idx++;
				ret = true;
			}
		} else if (type == MICROJS_JSON_STRING) {
			ret = true;
		}
	} else if (tok >= TOK_SYMBOL) {
		ret = false;
	} else if ((tok >= TOK_INT8) && (type == MICROJS_JSON_NUMBER)) {
		ret = true;
	} else if ((tok == TOK_LEFTBRACE) && (type == MICROJS_JSON_OBJECT)) {
		ret = true;
	} else if ((tok == TOK_LEFTBRACKET) && (type == MICROJS_JSON_ARRAY)) {
		ret = true;
	}

	return ret;
}

int microjs_json_parse_val(struct microjs_json_parser * jsn,
						   struct microjs_tok_val * val)
{
	unsigned int offs;
	uint32_t x;
	int idx;
	int tok;
	int len;
	int ret;

	idx = jsn->idx;
	tok = p->tok[idx++];
	if (tok >= TOK_STRING) {
		len = tok - TOK_STRING;
		offs = p->tok[idx++];
		offs |= p->tok[idx++] << 8;
		val->str.dat = (char *)p->js + offs;
		val->str.len = len;
		if (jsn->tkn->tok[idx] == TOK_COLON) {
			idx++;
			ret = MICROJS_JSON_LABEL;
		} else
			ret = MICROJS_JSON_STRING;
	} else if (tok >= TOK_SYMBOL) {
		len = tok - TOK_SYMBOL + 1;
		val->str.dat = (char *)&p->tok[idx];
		val->str.len = len;
		idx += len;
		ret = 
		tok = MICROJS_JSON_INVALID;
	} else if (tok >= TOK_INT8) {
		x = p->tok[idx++];
		if (tok >= TOK_INT16) {
			x |= p->tok[idx++] << 8;
			if (tok == TOK_INT24) {
				x |= p->tok[idx++] << 16;
				if (tok >= TOK_INT32)
					x |= p->tok[idx++] << 24;
			}
		} 
		val->u32 = x;
		ret = MICROJS_JSON_NUMBER;
	} else if (tok >= TOK_TRUE) {
		val->logic = true;
		ret = MICROJS_JSON_BOOLEAN;
	} else if (tok >= TOK_FALSE) {
		val->logic = false;
		ret = MICROJS_JSON_BOOLEAN;
	} else if (tok == TOK_LEFTBRACE) {
		ret = MICROJS_JSON_OBJECT;
	} else if (tok == TOK_LEFTBRACKET) {
		ret = MICROJS_JSON_ARRAY;
	} else {
		ret = MICROJS_JSON_INVALID;
	}

	jsn->idx = idx;

	return ret;
}

}

int microjs_json_parse_obj(struct microjs_json_parser * jsn)
{
	struct microjs_tok_val val;
	int type;

	while ((type = microjs_json_parse_val(jsn, &val)) != MICROJS_JSON_INVALID) {
	}

	return 0;
}

