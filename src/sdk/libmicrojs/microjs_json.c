#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define __MICROJS_I__
#include "microjs-i.h"

#include <sys/dcclog.h>

int microjs_json_init(struct microjs_json_parser * jsn, 
					  struct microjs_tokenizer * tkn)
{
	jsn->tkn = tkn;
	jsn->idx = 0;

	return 0;
}

int microjs_json_get_val(struct microjs_json_parser * jsn,
						   struct microjs_val * val)
{
	unsigned int offs;
	uint32_t x;
	int idx;
	int tok;
	int len;
	int ret;

	idx = jsn->idx;
	tok = jsn->tkn->tok[idx++];
	if (tok >= TOK_STRING) {
		len = tok - TOK_STRING;
		offs = jsn->tkn->tok[idx++];
		offs |= jsn->tkn->tok[idx++] << 8;
		if (val != NULL) {
			val->str.dat = (char *)jsn->tkn->js + offs;
			val->str.len = len;
		}
		if (jsn->tkn->tok[idx] == TOK_COLON) {
			jsn->idx = ++idx;
			DCC_LOG1(LOG_INFO, "<%d> :",idx);
			return MICROJS_JSON_LABEL;
		} else {
			ret = MICROJS_JSON_STRING;
			DCC_LOG(LOG_INFO, "STRING");
		}
	} else if (tok >= TOK_SYMBOL) {
		DCC_LOG(LOG_WARNING, "INVALID");
		return MICROJS_JSON_INVALID;
	} else if (tok >= TOK_INT8) {
		x = jsn->tkn->tok[idx++];
		if (tok >= TOK_INT16) {
			x |= jsn->tkn->tok[idx++] << 8;
			if (tok == TOK_INT24) {
				x |= jsn->tkn->tok[idx++] << 16;
				if (tok >= TOK_INT32)
					x |= jsn->tkn->tok[idx++] << 24;
			}
		} 
		if (val != NULL)
			val->u32 = x;
		ret = MICROJS_JSON_INTEGER;
		DCC_LOG(LOG_INFO, "int");
	} else if (tok == TOK_LEFTBRACE) {
		DCC_LOG1(LOG_INFO, "<%d> {",idx);
		jsn->idx = idx;
		return MICROJS_JSON_OBJECT;
	} else if (tok == TOK_LEFTBRACKET) {
		DCC_LOG(LOG_INFO, "[");
		jsn->idx = idx;
		return MICROJS_JSON_ARRAY;
	} else if (tok >= TOK_TRUE) {
		if (val != NULL)
			val->logic = true;
		ret = MICROJS_JSON_BOOLEAN;
		DCC_LOG(LOG_TRACE, "true");
	} else if (tok >= TOK_FALSE) {
		if (val != NULL)
			val->logic = false;
		ret = MICROJS_JSON_BOOLEAN;
		DCC_LOG(LOG_TRACE, "false");
	} else if (tok == TOK_RIGHTBRACE) {
		ret = MICROJS_JSON_END_OBJECT;
		DCC_LOG(LOG_INFO, "}");
	} else if (tok == TOK_RIGHTBRACKET) {
		ret = MICROJS_JSON_END_ARRAY;
		DCC_LOG(LOG_INFO, "]");
	} else {
		DCC_LOG(LOG_TRACE, "INVALID");
		return MICROJS_JSON_INVALID;
	}

	tok = jsn->tkn->tok[idx];

	if (tok == TOK_COMMA) {
		idx++;
	} else if ((tok != TOK_RIGHTBRACE) && 
			   (tok != TOK_RIGHTBRACKET) && 
			   (tok != TOK_EOF) ) {
		DCC_LOG(LOG_TRACE, "INVALID");
		return MICROJS_JSON_INVALID;
	} 

	jsn->idx = idx;

	return ret;
}

const struct microjs_attr_desc null_desc[] = {
	{ "", 0, 0, 0, NULL},
};

int microjs_json_parse_array(struct microjs_json_parser * jsn, void * ptr)
{
	struct microjs_val val;
	int typ;

	while ((typ = microjs_json_get_val(jsn, &val)) != MICROJS_JSON_END_ARRAY) {
		if (typ == MICROJS_JSON_ARRAY) {
			microjs_json_parse_array(jsn, NULL);
		} else if (typ == MICROJS_JSON_OBJECT) {
			microjs_json_parse_obj(jsn, null_desc, NULL);
		}
	}

	return 0;
}

int microjs_json_parse_obj(struct microjs_json_parser * jsn,
						   const struct microjs_attr_desc desc[],
						   void * ptr)
{
	struct microjs_val val;
	uint8_t * p;
	int cnt = 0;
	int typ;

	DCC_LOG(LOG_INFO, "...");

	while ((typ = microjs_json_get_val(jsn, &val)) == MICROJS_JSON_LABEL) {
		int i;

		for (i = 0; desc[i].parse != NULL; ++i) {
			/* look for a decoder that matches the label */ 
			if (strncmp(desc[i].key, val.str.dat, val.str.len) == 0) {
				DCC_LOG1(LOG_TRACE, "%s:", desc[i].key);
				typ = microjs_json_get_val(jsn, &val);
				if (typ != desc[i].type) {
					/* the attribute type do not matches the decoder */
					DCC_LOG(LOG_WARNING, "attribute type mismatch");
					return -1;
				}
				p = (uint8_t *)ptr + desc[i].offs;
				if (desc[i].parse(jsn, &val, desc[i].opt, p) < 0) {
					DCC_LOG(LOG_WARNING, "attribute parse error");
					return -1;
				}

				break;
			}
		}

		if (desc[i].parse == NULL) {
			if (val.str.len == 1) {
				DCC_LOG1(LOG_WARNING, "unsupported attribute: %c", 
						 val.str.dat[0]);
			} else if (val.str.len == 2) {
				DCC_LOG2(LOG_WARNING, "unsupported attribute: %c%c", 
					 	val.str.dat[0], val.str.dat[1]);
			} else if (val.str.len == 3) {
				DCC_LOG3(LOG_WARNING, "unsupported attribute: %c%c%c", 
					 	val.str.dat[0], val.str.dat[1], val.str.dat[2]);
			} else {
				DCC_LOG4(LOG_WARNING, "unsupported attribute: %c%c%c%c", 
						 val.str.dat[0], val.str.dat[1],
						 val.str.dat[2], val.str.dat[3]);
			}
			/* skip unsupported attribute */
			typ = microjs_json_get_val(jsn, &val);
			if (typ == MICROJS_JSON_ARRAY) {
				microjs_json_parse_array(jsn, NULL);
			} else if (typ == MICROJS_JSON_OBJECT) {
				microjs_json_parse_obj(jsn, null_desc, NULL);
			}
		}

		cnt++;
	}

	if (typ == MICROJS_JSON_END_OBJECT) {
		DCC_LOG(LOG_INFO, "}");
		return cnt;
	}

	DCC_LOG(LOG_WARNING, "invalid type!");
	return -1;
}

/* Encode a 16bit integral value */
int microjs_u16_enc(struct microjs_json_parser * jsn, 
					struct microjs_val * val, 
					unsigned int opt, void * ptr)
{
	uint16_t * pval = (uint16_t *)ptr;
	*pval = val->u32;

	DCC_LOG2(LOG_INFO, "val=%d ptr=%08x", *pval, pval);
	return 0;
}

/* Encode a boolean as a single bit */
int microjs_bit_enc(struct microjs_json_parser * jsn, 
					struct microjs_val * val, 
					unsigned int bit, void * ptr)
{
	uint32_t * bfield = (uint32_t *)ptr;

	*bfield &= ~(1 << bit);
	*bfield |= (val->logic ? 1 : 0) << bit;

	DCC_LOG1(LOG_INFO, "val=%d", val->logic ? 1 : 0);
	return 0;
}

/* Encode a string as a index to the constant string pool */
int microjs_const_str_enc(struct microjs_json_parser * jsn, 
					struct microjs_val * val, 
					unsigned int opt, void * ptr)
{
	uint8_t * sip = (uint8_t *)ptr;
	int ret;

	if ((ret = const_str_write(val->str.dat, val->str.len)) < 0)
		return ret;

	*sip = ret;

	DCC_LOG1(LOG_TRACE, "<%d>", ret);

	return 0;
}

int microjs_json_root_len(const char * js)
{
	char * cp;
	char * ep;
	int c;

	for (cp = ep = (char *)js; (c = *cp) != '\0'; ++cp) {
		if (c == '}')
			ep = cp;
	}

	return (ep - js) + 1; 
}

