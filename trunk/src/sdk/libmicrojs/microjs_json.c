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

bool microjs_json_expect(struct microjs_json_parser * jsn, unsigned int type)
{
	int idx = jsn->idx;
	bool ret = false;
	int tok;

	tok = jsn->tkn->tok[idx++];
	if (tok >= TOK_STRING)  {
		idx += 2;
		if (jsn->tkn->tok[idx] == TOK_COLON) {
			if (type == MICROJS_JSON_LABEL) {
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
			idx++;
			ret = MICROJS_JSON_LABEL;
			DCC_LOG(LOG_TRACE, "LABEL");
		} else {
			ret = MICROJS_JSON_STRING;
			DCC_LOG(LOG_TRACE, "STRING");
		}
	} else if (tok >= TOK_SYMBOL) {
		ret = MICROJS_JSON_INVALID;
		DCC_LOG(LOG_TRACE, "INVALID");
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
		ret = MICROJS_JSON_NUMBER;
		DCC_LOG(LOG_TRACE, "NUMBER");
	} else if (tok >= TOK_TRUE) {
		if (val != NULL)
			val->logic = true;
		ret = MICROJS_JSON_BOOLEAN;
		DCC_LOG(LOG_TRACE, "BOOLEAN");
	} else if (tok >= TOK_FALSE) {
		if (val != NULL)
			val->logic = false;
		ret = MICROJS_JSON_BOOLEAN;
		DCC_LOG(LOG_TRACE, "BOOLEAN");
	} else if (tok == TOK_LEFTBRACE) {
		ret = MICROJS_JSON_OBJECT;
		DCC_LOG(LOG_TRACE, "OBJECT");
	} else if (tok == TOK_LEFTBRACKET) {
		ret = MICROJS_JSON_ARRAY;
		DCC_LOG(LOG_TRACE, "ARRAY");
	} else {
		ret = MICROJS_JSON_INVALID;
		DCC_LOG(LOG_TRACE, "INVALID");
	}

	jsn->idx = idx;

	return ret;
}

int microjs_json_parse_obj(struct microjs_json_parser * jsn,
						   const struct microjs_attr_desc desc[],
						   void * ptr)
{
	struct microjs_val val;
	uint8_t * p;
	int cnt = 0;
	int type;
	int tok;

	DCC_LOG(LOG_TRACE, "...");

	while (microjs_json_get_val(jsn, &val) == MICROJS_JSON_LABEL) {
		int i;

		for (i = 0; desc[i].parse != NULL; ++i) {
			/* look for a decoder that matches the label */ 
			if (strncmp(desc[i].key, val.str.dat, val.str.len) == 0) {
				type = microjs_json_get_val(jsn, &val);
				if (type != desc[i].type) {
					/* the attribute type do not matches the decoder */
					DCC_LOG(LOG_WARNING, "attribute type mismatch");
					return -1;
				}
				p = (uint8_t *)ptr + desc[i].offs;
				if (desc[i].parse(jsn, &val, desc[i].opt, p) < 0) {
					return -1;
				}

				break;
			}
		}

		cnt++;

		tok = jsn->tkn->tok[jsn->idx++];
		if (tok == TOK_COMMA)  {
			DCC_LOG(LOG_TRACE, ",");
			continue;
		} 
		
		if (tok == TOK_RIGHTBRACE) {
			DCC_LOG(LOG_TRACE, "{");
			return 0;
		}
	}

	return 0;
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
					unsigned int bit, void * ptr)
{
	uint8_t * sip = (uint8_t *)ptr;
	int ret;

	if ((ret = const_str_write(val->str.dat, val->str.len)) < 0)
		return ret;

	*sip = ret;

	DCC_LOG1(LOG_TRACE, "\"%s\"", const_str(ret));

	return 0;
}

#if 0
int microjs_json_parse_array(struct microjs_json_parser * jsn,
						   const struct microjs_attr_desc * desc[],
						   void * ptr)
{
	struct microjs_tok_val val;
	uint8_t * p;
	int cnt = 0;
	int type;

	while (microjs_json_parse_val(jsn, &val) != MICROJS_INVALID) {
		int i;

		for (i = 0; desc[i].parser != NULL; ++i) {
			/* look for a decoder that matches the label */ 
			if (strcmp(desc[i].key, val.str.dat) == 0) {
				type = microjs_json_parse_val(jsn, &val);
				if (type != desc[i].type) {
					/* the attribute type do not matches the decoder */
					return -1;
				}
				p = (uint8_t *)ptr + desc[i].offs;
				if (desc[i].parse(jsn, &val, desc[i].opt, p) < 0) {
					return -1;
				}
			}
		}

		tok = jsn->tkn->tok[jsn->idx++];
		if (tok == TOK_COMMA)  {
			continue;
		} 
		
		if (tok == TOK_RIGHTBRACE) {
			return 0;
		}
	}

	return 0;
}

#endif

