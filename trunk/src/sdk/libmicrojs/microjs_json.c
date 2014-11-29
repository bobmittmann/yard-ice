#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/dcclog.h>
#include <arch/cortex-m3.h>


#define __MICROJS_I__
#include "microjs-i.h"
#include <microjs-json.h>

#ifndef JSON_LABEL_LST_MAX 
#define JSON_LABEL_LST_MAX 40
#endif

#ifndef JSON_STRING_LEN_MAX 
#define JSON_STRING_LEN_MAX 200
#endif

#define JSON_TOK_STRING       (256 - JSON_STRING_LEN_MAX - 1)
#define JSON_TOK_LABEL       (JSON_TOK_STRING - JSON_LABEL_LST_MAX)

#define JSON_TOK_EOF           0
#define JSON_TOK_STOP          1
#define JSON_TOK_NULL          2
#define JSON_TOK_LEFTBRACE     3
#define JSON_TOK_LEFTBRACKET   4 
#define JSON_TOK_RIGHTBRACE    5
#define JSON_TOK_RIGHTBRACKET  6

#define JSON_TOK_FALSE         7
#define JSON_TOK_TRUE          8

#define JSON_TOK_ZERO          9

#define JSON_TOK_INT8         12
#define JSON_TOK_INT16        13 
#define JSON_TOK_INT24        14
#define JSON_TOK_INT32        15

#define JSON_TOK_LAST         JSON_TOK_INT32

#if (JSON_LABEL_LST_MAX + JSON_STRING_LEN_MAX + JSON_TOK_LAST) > 255
#error "(JSON_LABEL_LST_MAX + JSON_STRING_LEN_MAX + JSON_TOK_LAST) > 255"
#endif

int microjs_json_init(struct microjs_json_parser * jsn, 
					 uint8_t * tok, unsigned int size,
					 const char * const label[])
{
	jsn->cnt = 0;
	jsn->off = 0;
	jsn->end = 0;
	jsn->idx = 0;
	jsn->sp = size;
	jsn->top = size;
	jsn->tok = tok;
	jsn->txt  = NULL;
	jsn->len = 0;
	jsn->lbl = label;

	memset(jsn->tok, JSON_TOK_EOF, jsn->top);
	DCC_LOG2(LOG_INFO, "tok=0x%08x size=%d", tok, size);

	return 0;
}

int microjs_json_open(struct microjs_json_parser * jsn, 
					  const char * txt, unsigned int len)
{
	int ret = MICROJS_EMPTY_FILE;
	int c;
	int i;

	/* set the base javascript file reference */
	jsn->txt = txt;
	jsn->len = len;

	/* search for the initial oppening braces */
	for (i = 0; i < len; i++) {
		c = txt[i];
		if (isspace(c)) /* Remove lead blanks */
			continue;

		if (c != '{') {
			ret = MICROJS_OBJECT_EXPECTED;
		} else {
			i++;
			/* insert oppening brace into heap */
			jsn->tok[jsn->cnt++] = JSON_TOK_LEFTBRACE;
			/* push closing brace into stack */
			jsn->tok[--jsn->sp] = JSON_TOK_RIGHTBRACE;
			ret = MICROJS_OK;
		}
		break;
	}

	jsn->end = i;

	return ret;
}

void microjs_json_flush(struct microjs_json_parser * jsn)
{
	jsn->cnt = 0;
	jsn->sp = jsn->top;
	jsn->idx = 0;
}

/* JSON lexer (scanner) */
int microjs_json_scan(struct microjs_json_parser * jsn)
{
	unsigned int tok = JSON_TOK_NULL;
	unsigned int cnt;
	unsigned int sp;
	unsigned int i;
	unsigned int len;
	const char * txt;
	int err;
	int c;

	/* skip the previous chunk */
	jsn->off = jsn->end;

	/* initialize variables */
	cnt = jsn->cnt;
	sp = jsn->sp;
	txt = jsn->txt;
	len = jsn->len;

	DCC_LOG(LOG_INFO, "parse start");
	DCC_LOG1(LOG_INFO, "script length = %d bytes.", len);

	for (i = jsn->off; i < len; ) {

		c = txt[i];
		/* Remove lead blanks */
		if (isspace(c)) {
			i++;
			continue;
		}

		DCC_LOG1(LOG_INFO, "'%c'", c);

		/* Quotes: copy verbatim */
		if ((c == '\'') || (c == '\"')) {
			unsigned int offs = i + 1; /* string offset in the file */
			unsigned int j = 0; /* length of the string */
			int qt = c; /* quote character */

			DCC_LOG(LOG_INFO, "string");
			for (;;) {
				if (++i == len) {
					/* parse error, unclosed quotes */
					err = MICROJS_UNCLOSED_STRING;
					goto error;
				}
				c = txt[i];
				if (c == qt) {
					i++;
					break;
				}
				j++;
			}

			if (j > JSON_STRING_LEN_MAX) {
				err = MICROJS_STRING_TOO_LONG;
				goto error;
			}

			if ((sp - cnt) < 3) {
				err = MICROJS_TOKEN_BUF_OVF;
				goto error;
			}

			tok = JSON_TOK_STRING;
			jsn->tok[cnt++] = tok + j;
			jsn->tok[cnt++] = offs;
			jsn->tok[cnt++] = (offs >> 8);
			continue;
		}

		/* Symbol */
		if (isalpha(c) || (c == '_')) {
			unsigned int j;
			char * s;

			/* check wether we have room to copy the symbol 
			   to token buffer or not */
			if ((sp - cnt) < 8) {
				err = MICROJS_TOKEN_BUF_OVF;
				goto error;
			}

			j = 0;
			s = (char *)&jsn->tok[cnt + 1];
			do {
				s[j++] = c;
				if (++i == len)	
					break;
				c = txt[i];
			} while (isalnum(c) || (c == '_'));
			s[j++] = '\0';

			/* look up in the kwywords table */
			if (strcmp(s, "true") == 0) {
				DCC_LOG(LOG_INFO, "true");
				tok = JSON_TOK_TRUE;
				goto push;
			} 
			
			if (strcmp(s, "false") == 0) {
				DCC_LOG(LOG_INFO, "false");
				tok = JSON_TOK_FALSE;
				goto push;
			}

			if (strcmp(s, "null") == 0) {
				DCC_LOG(LOG_INFO, "null");
				tok = JSON_TOK_NULL;
				goto push;
			}

			err = MICROJS_INVALID_SYMBOL;
			goto error;
		}

		/* Number */
		if (isdigit(c) || (c == '-'))  {
			int32_t val = 0;
			bool neg = false;
			if (c == '-') {
				if (++i == len)	{
					err = MICROJS_INVALID_SYMBOL;
					goto error;
				}	
				neg = true;
				c = txt[i];
				if (!isdigit(c))  { 
					err = MICROJS_UNEXPECTED_CHAR;
					goto error;
				}
			}

			DCC_LOG(LOG_INFO, "Number");
			/* Decimal */
			do {
				val = ((val << 2) + val) << 1;
				val += c - '0';
				if (++i == len)	
					break;
				c = txt[i];
			} while (isdigit(c));

			if (neg) {
				val = -val;
			}	
		
			if ((sp - cnt) < 5) {
				err = MICROJS_TOKEN_BUF_OVF;
				goto error;
			}

			if ((val & 0xffffff00) == 0) {
				tok = JSON_TOK_INT8;
				jsn->tok[cnt++] = tok;
				jsn->tok[cnt++] = val;
				continue;
			} 
			
			if ((val & 0xffff0000) == 0) {
				tok = JSON_TOK_INT16;
				jsn->tok[cnt++] = tok;
				jsn->tok[cnt++] = val;
				jsn->tok[cnt++] = val >> 8;
				continue;
			}

			if ((val & 0xffffff00) == 0) {
				tok = JSON_TOK_INT24;
				jsn->tok[cnt++] = tok;
				jsn->tok[cnt++] = val;
				jsn->tok[cnt++] = val >> 8;
				jsn->tok[cnt++] = val >> 16;
				continue;
			}

			tok = JSON_TOK_INT32;
			jsn->tok[cnt++] = tok;
			jsn->tok[cnt++] = val;
			jsn->tok[cnt++] = val >> 8;
			jsn->tok[cnt++] = val >> 16;
			jsn->tok[cnt++] = val >> 24;
			continue;
		}
	
		switch (c) {
		case ':':
			if (tok == JSON_TOK_STRING) {
				int n = jsn->tok[cnt - 3] - JSON_TOK_STRING;
				int o = jsn->tok[cnt - 2] | (jsn->tok[cnt - 1] << 8);
				char * lbl = (char *)txt + o;
				const char * cp;
				int j;

				DCC_LOG1(LOG_INFO, "label: %d", n);

				for (j = 0; (cp = jsn->lbl[j]) != NULL; ++j) {
					if ((strncmp(cp, lbl, n) == 0) && (strlen(cp) == n)) {
						break;
					}
				}

				if (cp != NULL) {
					DCC_LOG1(LOG_INFO, "%s:", cp);
				} else {
					/* not in the list of labels !!! */
					DCC_LOG3(LOG_TRACE, "unlisted label: %c%c%c...", 
							lbl[0], lbl[1], lbl[2]);
				}

				cnt -= 2; /* remove the string */
				tok = JSON_TOK_LABEL; /* insert the label */
				jsn->tok[cnt - 1] = tok + j;
				i++;
				continue;
			}
			err = MICROJS_INVALID_LABEL;
			goto error;
		case ',':
			i++;
			continue;
		case '[':
			tok = JSON_TOK_LEFTBRACKET;
			/* push closing bracket into stack */
			jsn->tok[--sp] = JSON_TOK_RIGHTBRACKET;
			break;
		case '{':
			tok = JSON_TOK_LEFTBRACE;
			/* push closing brace into stack */
			jsn->tok[--sp] = JSON_TOK_RIGHTBRACE;
			break;
		case ']':
			tok = JSON_TOK_RIGHTBRACKET;
			goto bkt_pop;
		case '}':
			tok = JSON_TOK_RIGHTBRACE;
			goto bkt_pop;
		default:
			err = MICROJS_UNEXPECTED_CHAR;
			goto error;
		}

inc_push:
		/* increment the index pointer and push a token into the buffer */
		i++;
push:
		/* push a token into the buffer */
		if ((sp - cnt) < 1) {
			err = MICROJS_TOKEN_BUF_OVF;
			goto error;
		}
		jsn->tok[cnt++] = tok;
	}

	/* the matching bracket stack must be empty at this point */
	if (sp != jsn->top) {
		err = MICROJS_BRACKET_MISMATCH;
		goto error;
	}

end:
	DCC_LOG2(LOG_INFO, "End Scan! (cnt=%d sp=0x%08x).", cnt, cm3_sp_get());
	jsn->cnt = cnt;
	jsn->sp = sp;
	jsn->end = i;

	return MICROJS_OK;

bkt_pop:
	/* get a brakcet from the stack and check for matching pair */
	if (sp == jsn->top) {
		err = MICROJS_EMPTY_STACK;
		goto error;
	}
	if (jsn->tok[sp++] != tok) {
		err = MICROJS_BRACKET_MISMATCH;
		goto error;
	}
	if (sp == jsn->top) {
		DCC_LOG(LOG_INFO, "topmost object end!");
		i++;

		/* push a token into the buffer */
		if ((sp - cnt) < 2) {
			err = MICROJS_TOKEN_BUF_OVF;
			goto error;
		}
		jsn->tok[cnt++] = tok;
		jsn->tok[cnt++] = JSON_TOK_STOP;
		goto end;
	}

	goto inc_push;

error:
	jsn->cnt = cnt;
	jsn->sp = sp;
	jsn->off = i;

	return err;
}

/* JSON Parser */
int microjs_json_get_val(struct microjs_json_parser * jsn,
						 struct microjs_val * val)
{
	unsigned int offs;
	int32_t x;
	int idx;
	int tok;
	int len;
	int ret;

	idx = jsn->idx;
	tok = jsn->tok[idx++];
	if (tok >= JSON_TOK_STRING) {
		len = tok - JSON_TOK_STRING;
		offs = jsn->tok[idx++];
		offs |= jsn->tok[idx++] << 8;
		if (val != NULL) {
			val->str.dat = (char *)jsn->txt + offs;
			val->str.len = len;
		}
		ret = MICROJS_JSON_STRING;
	} else if (tok >= JSON_TOK_LABEL) {
		int j = tok - JSON_TOK_LABEL;
		const char * cp = jsn->lbl[j];
		if (val != NULL) {
			val->lbl.sz = cp;
			val->lbl.id = j;
		}
		ret = MICROJS_JSON_LABEL;
	} else if (tok >= JSON_TOK_INT8) {
		x = jsn->tok[idx++];
		if (tok >= JSON_TOK_INT16) {
			x |= jsn->tok[idx++] << 8;
			if (tok == JSON_TOK_INT24) {
				x |= jsn->tok[idx++] << 16;
				if (tok >= JSON_TOK_INT32)
					x |= jsn->tok[idx++] << 24;
			}
		} 
		if (val != NULL)
			val->u32 = x;
		ret = MICROJS_JSON_INTEGER;
	} else if (tok >= JSON_TOK_TRUE) {
		if (val != NULL)
			val->logic = true;
		ret = MICROJS_JSON_BOOLEAN;
	} else if (tok >= JSON_TOK_FALSE) {
		if (val != NULL)
			val->logic = false;
		ret = MICROJS_JSON_BOOLEAN;
	} else
		ret = tok;

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
	int cnt = 0;
	int typ;

	DCC_LOG(LOG_INFO, "...");

	while ((typ = microjs_json_get_val(jsn, &val)) == MICROJS_JSON_LABEL) {
		microjs_attr_parser_t parse = NULL;
		int i;

		if (val.lbl.sz != NULL) {
			for (i = 0; (parse = desc[i].parse) != NULL; ++i) {
				/* look for a decoder that matches the label */ 
				if (strcmp(desc[i].key, val.lbl.sz) == 0) {
					break;
				}
			}
		}

		if (parse != NULL) {
			uint8_t * p;

			DCC_LOG1(LOG_INFO, "%s:", desc[i].key);
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
		} else {
			DCC_LOG(LOG_TRACE, "unsupported attribute");
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

	if (typ == MICROJS_JSON_STOP) {
		DCC_LOG(LOG_INFO, ".");
		return cnt;
	}

	DCC_LOG1(LOG_WARNING, "invalid type: %d!", typ);
	return -1;
}

/* Encode a 16 bits integral value */
int microjs_u16_enc(struct microjs_json_parser * jsn, 
					struct microjs_val * val, 
					unsigned int opt, void * ptr)
{
	uint16_t * pval = (uint16_t *)ptr;
	*pval = val->u32;
	DCC_LOG2(LOG_INFO, "val=%d ptr=%08x", *pval, pval);
	return 0;
}

/* Encode a 8 bits integral value */
int microjs_u8_enc(struct microjs_json_parser * jsn, 
					struct microjs_val * val, 
					unsigned int opt, void * ptr)
{
	uint8_t * pval = (uint8_t *)ptr;
	*pval = val->u32;
	DCC_LOG2(LOG_INFO, "val=%d ptr=%08x", *pval, pval);
	return 0;
}


int microjs_array_u8_enc(struct microjs_json_parser * jsn, 
					struct microjs_val * val, 
					unsigned int len, void * ptr)
{
	uint8_t * p = (uint8_t *)ptr;
	int n = 0;
	int typ;

	while ((typ = microjs_json_get_val(jsn, val)) == MICROJS_JSON_INTEGER) {
		if (n < len) 
			p[n++] = val->u32;
	}

	return (typ == MICROJS_JSON_END_ARRAY) ? n : -1;
}



/* Encode a boolean as a single bit */
int microjs_bit_enc(struct microjs_json_parser * jsn, 
					struct microjs_val * val, 
					unsigned int bit, void * ptr)
{
	uint32_t * bfield = (uint32_t *)ptr;

	*bfield &= ~(1 << bit);
	*bfield |= (val->logic ? 1 : 0) << bit;

	DCC_LOG2(LOG_INFO, "val=%d bfield=0x%08x", 
			 val->logic ? 1 : 0, *bfield);
	return 0;
}

/* Encode a string as a index to the constant string pool */
int microjs_const_str_enc(struct microjs_json_parser * jsn, 
					struct microjs_val * val, 
					unsigned int opt, void * ptr)
{
	uint8_t * sip = (uint8_t *)ptr;
	int ret;

	if ((ret = const_str_add(val->str.dat, val->str.len)) < 0)
		return ret;

	*sip = ret;

	DCC_LOG1(LOG_INFO, "<%d>", ret);

	return 0;
}

/* -------------------------------------------------------------------------
   javascript code embedded on JSON array
   ------------------------------------------------------------------------- */

int mcrojs_js_array_enc(struct microjs_json_parser * jsn, 
						struct microjs_val * val, 
						unsigned int opt, void * ptr) 
{
	struct json_js * jj = (struct json_js *)ptr;
	struct microjs_sdt * microjs;
	uint32_t js_sdtbuf[32]; /* compiler buffer */
	struct symstat symstat;
	int line = 0;
	int typ;
	int ret;

	/* initialize compiler */
	microjs = microjs_sdt_init(js_sdtbuf, sizeof(js_sdtbuf), 
							   jj->symtab, jj->libdef);

	/* Save symbol table state. In case of a compilation error,
	   the symbol table may have some invalid entries, saving
	   the state allow us to roll back to a clean state without
	   loosing the good symbols */
	symstat = symtab_state_save(jj->symtab);

	/* begin compilation */
	microjs_sdt_begin(microjs, jj->code, jj->code_sz);

	/* compile line by line */
	ret = 0;
	while ((typ = microjs_json_get_val(jsn, val)) == MICROJS_JSON_STRING) {

		++line;
		DCC_LOG1(LOG_INFO, "line %d ...", line);

		if ((ret = microjs_compile(microjs, val->str.dat, val->str.len)) < 0) {
			DCC_LOG1(LOG_INFO, "compile err %d", ret);
			if (ret != -ERR_UNEXPECED_EOF)
				goto compile_error;
		} 
		
		DCC_LOG1(LOG_INFO, "compile ret %d", ret);
			
		if (ret > 0)
			microjs_sdt_reset(microjs);
	} 

	if (typ != MICROJS_JSON_END_ARRAY) {
		ret = 0;
		DCC_LOG1(LOG_ERROR, "expecting array closing, got %d!", typ);
		goto compile_error;
	}

	if (ret > 0) {
		/* finish compilation */
		if ((ret = microjs_sdt_end(microjs)) < 0) 
			goto compile_error;

	}

	jj->code_sz = ret;

	return ret;

compile_error:
	DCC_LOG1(LOG_ERROR, "Javascript compile error %d!", -ret);
	symtab_state_rollback(jj->symtab, symstat);

	return ret;
}

#if 0
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
#endif

int microjs_json_print(FILE * f, struct microjs_json_parser * jsn)
{
	const char * txt;
	int i;
	
	txt = jsn->txt;
	for (i = jsn->off; i < jsn->end; ++i)
		fprintf(f, "%c", txt[i]);

	fprintf(f, "\n");

	return 0;
}


#if 0
int microjs_json_dump(FILE * f, struct microjs_json_parser * jsn)
{
	unsigned int offs;
	int lvl = 0;
	int idx;
	int tok;
	int len;
	int j;

	for (idx = 0; idx < jsn->cnt; ) {
		tok = jsn->tok[idx++];
		if (tok >= JSON_TOK_STRING) {
			char buf[JSON_STRING_LEN_MAX + 1];
			len = tok - JSON_TOK_STRING;
			offs = jsn->tok[idx++];
			offs |= jsn->tok[idx++] << 8;
			memcpy(buf, (char *)jsn->txt + offs, len);
			buf[len] = '\0';
			fprintf(f, "\"%s\" ", buf);
		} else if (tok >= JSON_TOK_LABEL) {
			const char * cp = jsn->lbl[tok - JSON_TOK_LABEL];
			fprintf(f, "\n\"%s\":", cp);
		} else if (tok >= JSON_TOK_INT8) {
			int32_t x;
			x = jsn->tok[idx++];
			if (tok >= JSON_TOK_INT16) {
				x |= jsn->tok[idx++] << 8;
				if (tok == JSON_TOK_INT24) {
					x |= jsn->tok[idx++] << 16;
					if (tok >= JSON_TOK_INT32)
						x |= jsn->tok[idx++] << 24;
				}
			} 
			fprintf(f, "%d ", x);
		} else if (tok == JSON_TOK_LEFTBRACE) {
			fprintf(f, "{");
			lvl++;
			for (j = 0; j < lvl; ++j)
				fprintf(f, "    ");
		} else if (tok == JSON_TOK_RIGHTBRACE) {
			fprintf(f, "}\n");
			lvl--;
			for (j = 0; j < lvl; ++j)
				fprintf(f, "    ");
		} else if (tok == JSON_TOK_LEFTBRACKET) {
			fprintf(f, "[ ");
		} else if (tok == JSON_TOK_RIGHTBRACKET) {
			fprintf(f, "] ");
		} else if (tok >= JSON_TOK_TRUE) {
			fprintf(f, "true ");
		} else if (tok >= JSON_TOK_FALSE) {
			fprintf(f, "false ");
		} else if (tok >= JSON_TOK_NULL) {
			fprintf(f, "null ");
		} else if (tok >= JSON_TOK_STOP) {
			fprintf(f, "<STOP>\n");
		} else if (tok >= JSON_TOK_EOF) {
			fprintf(f, "<EOF>\n");
		} else {
			fprintf(f, "<<ERROR>>>\n");
			return -1;
		}
	}

	return 0;
}
#endif

