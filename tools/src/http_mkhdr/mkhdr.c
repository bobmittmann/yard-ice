#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

struct {
	int code;
	char * phrase;
	char * def;
} response [] = {
	{ 100, "Continue", "HTTP_CONTINUE" },
	{ 101, "Switching Protocols", "HTTP_SWITCHING_PROTOCOLS" },
	{ 200, "OK", "HTTP_OK" },
	{ 201, "Created", "HTTP_CREATED" },
	{ 202, "Accepted", "HTTP_ACCEPTED" },
	{ 203, "Non-Authoritative Information", "HTTP_NON_AUTHORITATIVE_INFORMATION" },
	{ 204, "No Content", "HTTP_NO_CONTENT" },
	{ 205, "Reset Content", "HTTP_RESET_CONTENT" },
	{ 206, "Partial Content", "HTTP_PARTIAL_CONTENT" },
	{ 300, "Multiple Choices", "HTTP_MULTIPLE_CHOICES" },
	{ 301, "Moved Permanently", "HTTP_MOVED_PERMANENTLY" },
	{ 302, "Found", "HTTP_FOUND" },
	{ 303, "See Other", "HTTP_SEE_OTHER" },
	{ 304, "Not Modified", "HTTP_NOT_MODIFIED" },
	{ 305, "Use Proxy", "HTTP_USE_PROXY" },
	{ 307, "Temporary Redirect", "HTTP_TEMPORARY_REDIRECT" },
	{ 400, "Bad Request", "HTTP_BAD_REQUEST" },
	{ 401, "Unauthorized", "HTTP_UNAUTHORIZED" },
	{ 402, "Payment Required", "HTTP_PAYMENT_REQUIRED" },
	{ 403, "Forbidden", "HTTP_FORBIDDEN" },
	{ 404, "Not Found", "HTTP_NOT_FOUND" },
	{ 405, "Method Not Allowed", "HTTP_METHOD_NOT_ALLOWED" },
	{ 406, "Not Acceptable", "HTTP_NOT_ACCEPTABLE" },
	{ 407, "Proxy Authentication Required", "HTTP_PROXY_AUTHENTICATION_REQUIRED" },
	{ 408, "Request Time-out", "HTTP_REQUEST_TIME_OUT" },
	{ 409, "Conflict", "HTTP_CONFLICT" },
	{ 410, "Gone", "HTTP_GONE" },
	{ 411, "Length Required", "HTTP_LENGTH_REQUIRED" },
	{ 412, "Precondition Failed", "HTTP_PRECONDITION_FAILED" },
	{ 413, "Request Entity Too Large", "HTTP_REQUEST_ENTITY_TOO_LARGE" },
	{ 414, "Request-URI Too Large", "HTTP_REQUEST_URI_TOO_LARGE" },
	{ 415, "Unsupported Media Type", "HTTP_UNSUPPORTED_MEDIA_TYPE" },
	{ 416, "Requested range not satisfiable", "HTTP_REQUESTED_RANGE_NOT_SATISFIABLE" },
	{ 417, "Expectation Failed", "HTTP_EXPECTATION_FAILED" },
	{ 500, "Internal Server Error", "HTTP_INTERNAL_SERVER_ERROR" },
	{ 501, "Not Implemented", "HTTP_NOT_IMPLEMENTED" },
	{ 502, "Bad Gateway", "HTTP_BAD_GATEWAY" },
	{ 503, "Service Unavailable", "HTTP_SERVICE_UNAVAILABLE" },
	{ 504, "Gateway Time-out", "HTTP_GATEWAY_TIME_OUT" },
	{ 505, "HTTP Version not supported", "HTTP_VERSION_NOT_SUPPORTED" },
	{ 999, NULL, NULL },
};  

struct sym {
	char * nm;
	char * def;
};

struct sym http_req_hdr[] = {
	{"Accept", "HTTP_HDR_ACCEPT"},
	{"Accept-Charset", "HTTP_HDR_ACCEPT_CHARSET"},
	{"Accept-Encoding", "HTTP_HDR_ACCEPT_ENCODING"},
	{"Accept-Language", "HTTP_HDR_ACCEPT_LANGUAGE"},
	{"Accept-Ranges", "HTTP_HDR_ACCEPT_RANGES"},
	{"Age", "HTTP_HDR_AGE"},
	{"Allow", "HTTP_HDR_ALLOW"},
	{"Authorization", "HTTP_HDR_AUTHORIZATION"},
	{"Cache-Control", "HTTP_HDR_CACHE_CONTROL"},
	{"Connection", "HTTP_HDR_CONNECTION"},
	{"Content-Disposition", "HTTP_HDR_CONTENT_DISPOSITION"},
	{"Content-Encoding", "HTTP_HDR_CONTENT_ENCODING"},
	{"Content-Language", "HTTP_HDR_CONTENT_LANGUAGE"},
	{"Content-Length", "HTTP_HDR_CONTENT_LENGTH"},
	{"Content-Location", "HTTP_HDR_CONTENT_LOCATION"},
	{"Content-MD5", "HTTP_HDR_CONTENT_MD5"},
	{"Content-Range", "HTTP_HDR_CONTENT_RANGE"},
	{"Content-Type", "HTTP_HDR_CONTENT_TYPE"},
	{"Cookie", "HTTP_HDR_COOKIE"},
	{"Date", "HTTP_HDR_DATE"},
	{"ETag", "HTTP_HDR_ETAG"},
	{"Expect", "HTTP_HDR_EXPECT"},
	{"Expires", "HTTP_HDR_EXPIRES"},
	{"From", "HTTP_HDR_FROM"},
	{"Host", "HTTP_HDR_HOST"},
	{"If-Match", "HTTP_HDR_IF_MATCH"},
	{"If-Modified-Since", "HTTP_HDR_IF_MODIFIED_SINCE"},
	{"If-None-Match", "HTTP_HDR_IF_NONE_MATCH"},
	{"If-Range", "HTTP_HDR_IF_RANGE"},
	{"If-Unmodified-Since", "HTTP_HDR_IF_UNMODIFIED_SINCE"},
	{"Last-Modified", "HTTP_HDR_LAST_MODIFIED"},
	{"Location", "HTTP_HDR_LOCATION"},
	{"Max-Forwards", "HTTP_HDR_MAX_FORWARDS"},
	{"Origin", "HTTP_HDR_ORIGIN"},
	{"Pragma", "HTTP_HDR_PRAGMA"},
	{"Proxy-Authenticate", "HTTP_HDR_PROXY_AUTHENTICATE"},
	{"Proxy-Authorization", "HTTP_HDR_PROXY_AUTHORIZATION"},
	{"Range", "HTTP_HDR_RANGE"},
	{"Referer", "HTTP_HDR_REFERER"},
	{"Retry-After", "HTTP_HDR_RETRY_AFTER"},
	{"Server", "HTTP_HDR_SERVER"},
	{"Set-Cookie", "HTTP_HDR_SET_COOKIE"},
	{"Set-Cookie2", "HTTP_HDR_SET_COOKIE2"},
	{"TE", "HTTP_HDR_TE"},
	{"Trailer", "HTTP_HDR_TRAILER"},
	{"Transfer-Encoding", "HTTP_HDR_TRANSFER_ENCODING"},
	{"Upgrade", "HTTP_HDR_UPGRADE"},
	{"Upgrade-Insecure-Requests", "HTTP_HDR_UPGRADE_INSECURE_REQUESTS"},
	{"User-Agent", "HTTP_HDR_USER_AGENT"},
	{"Vary", "HTTP_HDR_VARY"},
	{"Via", "HTTP_HDR_VIA"},
	{"Warning", "HTTP_HDR_WARNING"},
	{"WWW-Authenticate", "HTTP_HDR_WWW_AUTHENTICATE"},
	{"X-Requested-With", "HTTP_HDR_X_REQUESTED_WITH"},
	{NULL, NULL}
};

struct sym mime_type[] = {
	{"text/html",                         "TEXT_HTML"},
	{"text/plain",                        "TEXT_PLAIN"},
	{"text/css",                          "TEXT_CSS"},
	{"image/png",                         "IMAGE_PNG"},
	{"image/jpeg",                        "IMAGE_JPEG"},
	{"image/gif",                         "IMAGE_GIF"},
	{"application/javascript",            "APPLICATION_JAVASCRIPT"},
	{"application/json",                  "APPLICATION_JSON"},
	{"application/xml",                   "APPLICATION_XML"},
	{"multipart/form-data",               "MULTIPART_FORM_DATA"},
	{"application/x-www-form-urlencoded", "APPLICATION_X_WWW_FORM_URLENCODED"},
	{NULL, NULL}
};

/* Pearson Hashing */
void mk_p_tab(uint8_t * tab, int len, unsigned int seed)
{
	int i;
	int j;
	int tmp;

//	srandom(seed);
	srand(seed);

	for (i = 0; i < len; i++)
		tab[i] = i;

	for (i = len - 1; i >= 1; i--) {
//		j = random() % (i);
		j = rand() % (i);
		/* exchange a[i] and a[j] */
		tmp = tab[i];
		tab[i] = tab[j];
		tab[j] = tmp;
	}
}

void dump_tab(FILE * f, char * prefix, uint8_t * tab, int len, char * name)
{
	int i;

	if (prefix)
		fprintf(f, "%s uint8_t %s[] = {", prefix, name);
	else
		fprintf(f, "uint8_t %s[] = {", name);

	for (i = 0; i < len; i++) {
		if ((i % 8) == 0) {
			if (i == 0)
				fprintf(f, "\n\t");
			else
				fprintf(f, ",\n\t");
		} else {
			fprintf(f, ", ");
		}
		fprintf(f, "%3d", tab[i]);
	}
	fprintf(f, "\n};\n\n");
}

static int p_hash(uint8_t * a, char * s)
{
	int i, c;
	int h = 0;

	while ((c = *s++) != '\0') {
		i = h ^ c;
		h = a[i];
	}

	return h;
}

int mk_h_tab(FILE * f, struct sym sym[],
			 uint8_t * h, int h_len, 
			 uint8_t * p, int p_len)
{
	char * s;
	int i;
	int j;
	int k;
	int seed = 10000;

	for (k = 0; k < 100000; k++) {
		seed += 1;
		mk_p_tab(p, p_len, seed);

		/* mark the buckets as empty */
		for (i = 0; i < h_len; i++)
			h[i] = 0;

		for (i = 0; (s = sym[i].nm) != NULL; i++) {
			j = p_hash(p, s);
			if (h[j] != 0) {
				/* collision */
				break;
			}
			h[j] = i + 1;
		}

		if (s == NULL) {

			fprintf(f, "/*\n");
			fprintf(f, "   Table generated with Fisher and Yates permutation\n");
			fprintf(f, "   algorithm, using libc's rand() and srand(%d)\n", 
					seed);
			fprintf(f, "*/\n");
			fprintf(f, "\n");
			return h_len;
		}
	}

	return -1;
}

void dump_symtab(FILE * f, char * name, struct sym sym[])
{
	char * s;
	int i;

	fprintf(f, "const char * %s[] = {", name);
	fprintf(f, "\n\t\"\",");
	for (i = 0; (s = sym[i].nm) != NULL; i++) {
		if (i == 0)
			fprintf(f, "\n\t");
		else
			fprintf(f, ",\n\t");

		fprintf(f, "\"%s\"", s);
	}
	fprintf(f, "\n};\n\n");
}

void dump_symdef(FILE * f, struct sym sym[])
{
	int i;

	fprintf(f, "enum {");
	for (i = 0; sym[i].nm != NULL; i++) {
		if (i == 0)
			fprintf(f, "\n\t");
		else
			fprintf(f, ",\n\t");
		fprintf(f, "%s = %d", sym[i].def, i + 1);
	}
	fprintf(f, "\n};\n\n");
}

void write_parser(FILE * f, char * name, char * sep, bool cont)
{
	int i;
	int sep_cnt;

	sep_cnt = (sep == NULL) ? 0 : strlen(sep);

	if (sep_cnt == 0) {
		fprintf(f, "unsigned int %s(const char * str)\n", name);
		fprintf(f, "{\n");
		fprintf(f, "\tint h = 0;\n");
		fprintf(f, "\tint c;\n");
		fprintf(f, "\tint i;\n");
		fprintf(f, "\n");
		fprintf(f, "\tfor (i = 0; (c = str[i]) != '\\0'; ++i) {\n");
		fprintf(f, "\t\th = p_tab[c ^ h];\n");
		fprintf(f, "\t}\n");
		fprintf(f, "\n");
		fprintf(f, "\treturn h_tab[h];\n");
		fprintf(f, "}\n");
		fprintf(f, "\n");
	} else {
		fprintf(f, "unsigned int %s(const char * str, char ** endptr)\n", 
				name);
		fprintf(f, "{\n");
		fprintf(f, "\tint h = 0;\n");
		fprintf(f, "\tint c;\n");
		fprintf(f, "\tint i;\n");
		fprintf(f, "\n");
		if (cont) {
			fprintf(f, "\tfor (i = 0; (c = str[i]) != '\\0'; ++i) {\n");
		} else {
			fprintf(f, "\tfor (i = 0; ; ++i) {\n");
			fprintf(f, "\t\tc = str[i];\n");
			fprintf(f, "\t\tif (c == '\\0') {\n");
			fprintf(f, "\t\t\tif (endptr != NULL)\n");
			fprintf(f, "\t\t\t\t*endptr = (char *)&str[i];\n");
			fprintf(f, "\t\t\treturn h_tab[h];\n");
			fprintf(f, "\t\t}\n");
		}

		if (sep_cnt == 1) {
			fprintf(f, "\t\tif (c == '%c') {\n", sep[0]);
		} else {
			fprintf(f, "\t\tif ((c == '%c')", sep[0]);
			for (i = 1; i < sep_cnt; ++i)
				fprintf(f, " || (c == '%c')", sep[i]);
			fprintf(f, ") {\n");
		}

		fprintf(f, "\t\t\tif (endptr != NULL) {\n");
		fprintf(f, "\t\t\t\tchar * cp = (char *)&str[i + 1];\n");
		fprintf(f, "\t\t\t\twhile (*cp == ' ')\n");
		fprintf(f, "\t\t\t\t\tcp++;\n");
		fprintf(f, "\t\t\t\t*endptr = cp;\n");
		fprintf(f, "\t\t\t}\n");
		fprintf(f, "\t\t\treturn h_tab[h];\n");
		fprintf(f, "\t\t}\n");
		fprintf(f, "\t\th = p_tab[c ^ h];\n");
		fprintf(f, "\t}\n");
		fprintf(f, "\n");
		fprintf(f, "\treturn 0;\n");
		fprintf(f, "}\n");
		fprintf(f, "\n");
	}
}

/* -------------------------------------------------------------------------
 *
 *
 * -------------------------------------------------------------------------
 */

int make_h(char * fname)
{
	FILE * f;
	int i;

	if ((f = fopen(fname, "w+")) == NULL) {
		fprintf(stderr, "#ERROR: fopen(): %s\n", strerror(errno));
		return -1;
	}

	fprintf(f, "/*\n");
	fprintf(f, " * WARNING: do not edit, this file was automatically generated.\n");
	fprintf(f, " */\n\n");
	fprintf(f, "#ifndef __HTTP_HDR_H__\n");
	fprintf(f, "#define __HTTP_HDR_H__\n\n");

	fprintf(f, "#include <stdint.h>\n\n");

	dump_symdef(f, http_req_hdr);

	dump_symdef(f, mime_type);

	fprintf(f, "enum {");
	for (i = 0; response[i].phrase != NULL; i++) {
		if (i == 0)
			fprintf(f, "\n\t");
		else
			fprintf(f, ",\n\t");
		fprintf(f, "%s = %d", response[i].def, response[i].code);
	}
	fprintf(f, "\n};\n\n");


	fprintf(f, "extern const char * http_hdr_name[];\n\n");

	fprintf(f, "#ifdef __cplusplus\nextern \"C\" {\n#endif\n\n");

	fprintf(f, "unsigned int http_hdr_id(const char * s);\n");
	fprintf(f, "\n");
	fprintf(f, "unsigned int http_parse_field(const char * buf, char ** val);\n");
	fprintf(f, "\n");
	fprintf(f, "unsigned int http_parse_content_type(const char * str, "
		"char ** endptr);\n");
	fprintf(f, "\n");
	fprintf(f, "const char * http_reason_phrase(unsigned int code);\n");
	fprintf(f, "\n");
	fprintf(f, "#ifdef __cplusplus\n}\n#endif\n\n");

	fprintf(f, "#endif /* __HTTP_HDR_H__ */\n");

	fclose(f);

	return 0;
}

void comment_block(FILE * f, char * comment)
{
	fprintf(f, "/* --------------------------------------------------");
	fprintf(f, "--------------\n");
	fprintf(f, " * %s\n", comment);
	fprintf(f, " * --------------------------------------------------");
	fprintf(f, "-------------- */\n\n");
}

int make_hdr_c(char * fname)
{
	uint8_t p_tab[256];
	uint8_t h_tab[256];
	FILE * f;

	if ((f = fopen(fname, "w+")) == NULL) {
		fprintf(stderr, "#ERROR: fopen(): %s\n", strerror(errno));
		return -1;
	}

	fprintf(f, "/*\n");
	fprintf(f, " * WARNING: do not edit, this file was automatically generated.\n");
	fprintf(f, "\n");
	fprintf(f, "   Implementation of perfect hashing to parse HTTP headers\n");
	fprintf(f, "   Perfect hashing uses a Pearson Hashing Function\n");
	fprintf(f, "*/\n");
	fprintf(f, "\n");

	fprintf(f, "#include <stdint.h>\n");
	fprintf(f, "#include <stdlib.h>\n");
	fprintf(f, "\n");

	if (mk_h_tab(f, http_req_hdr, h_tab, 256, p_tab, 256) < 0) {
		fprintf(f, "#error \"Perfect hashing is not possible\"\n");
		fprintf(f, "\n");
		fclose(f);
		return 1;
	}

	fprintf(f, "/* http://www.w3.org/Protocols/rfc2616/rfc2616-sec14.html */\n");
	dump_symtab(f, "http_hdr_name", http_req_hdr);

	comment_block(f, "HTTP header permutation table");
	dump_tab(f, "static const", p_tab, 256, "p_tab");

	dump_tab(f, "static const", h_tab, 256, "h_tab");

	comment_block(f, "Get header id from header name");

	write_parser(f, "http_hdr_id", "", false);

	comment_block(f, "Parse header field, return field id and value.");

	write_parser(f, "http_parse_field", ": ", true);

	fprintf(f, "\n");
	fprintf(f, "#ifndef HTTP_HDR_TEST\n");
	fprintf(f, "#define HTTP_HDR_TEST 0\n");
	fprintf(f, "#endif\n");
	fprintf(f, "\n");
	fprintf(f, "#if HTTP_HDR_TEST\n");
	fprintf(f, "\n");
	fprintf(f, "#include <stdio.h>\n");
	fprintf(f, "#include <assert.h>\n");
	fprintf(f, "\n");
	fprintf(f, "int http_parser_test(void)\n");
	fprintf(f, "{\n");
	fprintf(f, "\tint i;\n");
	fprintf(f, "\n");
	fprintf(f, "\tfor (i = 1; i < sizeof(http_hdr_name) / sizeof(char *); ++i) {\n");

	fprintf(f, "\t\tchar s[512];\n");
	fprintf(f, "\t\tchar * cp;\n");
	fprintf(f, "\t\tint hdr;\n");
	fprintf(f, "\n");
	fprintf(f, "\t\tsprintf(s, \"%%s: field-value\\n\", http_hdr_name[i]);\n");
	fprintf(f, "\t\thdr = http_parse_field(s, &cp);\n");
	fprintf(f, "\n");
	fprintf(f, "\t\tassert(hdr == i);\n");
	fprintf(f, "\n");
	fprintf(f, "\t\tif (hdr != i) {\n");
	fprintf(f, "\t\t\tprintf(\"Can't parse: \\\"%%s\\\"\\n\", s);\n");
	fprintf(f, "\t\t\treturn -1;\n");
	fprintf(f, "\t\t}\n");
	fprintf(f, "\t}\n");
	fprintf(f, "\n");
	fprintf(f, "\treturn 0;\n");
	fprintf(f, "}\n");
	fprintf(f, "\n");
	fprintf(f, "#endif /* HTTP_HDR_TEST */\n");
	fprintf(f, "\n");

	fclose(f);

	return 0;
}

int make_mime_c(char * fname)
{
	uint8_t p_tab[256];
	uint8_t h_tab[256];
	FILE * f;

	if ((f = fopen(fname, "w+")) == NULL) {
		fprintf(stderr, "#ERROR: fopen(): %s\n", strerror(errno));
		return -1;
	}

	fprintf(f, "/*\n");
	fprintf(f, " * WARNING: do not edit, this file was automatically generated.\n");
	fprintf(f, "\n");
	fprintf(f, "   Implementation of perfect hashing to parse MIME types\n");
	fprintf(f, "   Perfect hashing uses a Pearson Hashing Function\n");
	fprintf(f, "*/\n");
	fprintf(f, "\n");

	fprintf(f, "#include <stdint.h>\n");
	fprintf(f, "#include <stdlib.h>\n");
	fprintf(f, "\n");

	if (mk_h_tab(f, mime_type, h_tab, 256, p_tab, 256) < 0) {
		fprintf(f, "#error \"Perfect hashing is not possible\"\n");
		fprintf(f, "\n");
		fclose(f);
		return 1;
	}

	fprintf(f, "/* Mime types */\n");
	dump_symtab(f, "mime_type_name", mime_type);

	comment_block(f, "MIME type permutation table");

	dump_tab(f, "static const", p_tab, 256, "p_tab");
	dump_tab(f, "static const", h_tab, 256, "h_tab");

	comment_block(f, "Get header id from header name");

	write_parser(f, "mime_type_id", "", false);

	comment_block(f, "Parse content type.");

	write_parser(f, "http_parse_content_type", ";", false);

	fclose(f);

	return 0;
}


int make_tabs_c(char * fname)
{
	FILE * f;

	if ((f = fopen(fname, "w+")) == NULL) {
		fprintf(stderr, "#ERROR: fopen(): %s\n", strerror(errno));
		return -1;
	}

	fprintf(f, "/*\n");
	fprintf(f, " * WARNING: do not edit, this file was automatically generated.\n");
	fprintf(f, " */\n\n");
	fprintf(f, "#include <stdint.h>\n");
	fprintf(f, "#include <stdlib.h>\n");
	fprintf(f, "\n");

	dump_symtab(f, "http_hdr_name", http_req_hdr);
	dump_symtab(f, "mime_type_name", mime_type);

	fclose(f);

	return 0;
}

int int_hash(uint32_t v, uint32_t r)
{
	uint32_t h;

	h = v * r;
	h ^= h >> 16;
	h ^= h >> 8;

	return h & 0xff;
}

int short_hash(uint32_t v, uint32_t r)
{
	uint32_t h;

	h = v * r;
	h ^= h >> 8;

	return h & 0xff;
}

int mk_i_tab(FILE * f, uint8_t * t, int i_len)
{
	char * s;
	int r; 
	int i;
	int v;
	int h;

	for (r = 1; r < 65536; r++) {
		for (i = 0; i < 256; i++)
			t[i] = 0;

		for (i = 0; (s = response[i].phrase) != NULL; i++) {
			v = response[i].code;
//			h = int_hash(v, r);
			h = short_hash(v, r);
			if (t[h] != 0) {
				/* collision */
				break;
			}
			t[h] = i + 1;
		}

		if (s == NULL)
			break;
	}

	return r;
}

int make_status_c(char * fname)
{
	uint8_t i_tab[256];
	FILE * f;
	char * s;
	int i;
	int r;

	if ((f = fopen(fname, "w+")) == NULL) {
		fprintf(stderr, "#ERROR: fopen(): %s\n", strerror(errno));
		return -1;
	}

	fprintf(f, "/*\n");
	fprintf(f, " * WARNING: do not edit, this file was automatically generated.\n");
	fprintf(f, " */\n\n");
	fprintf(f, "#include <stdint.h>\n");
	fprintf(f, "#include <stdlib.h>\n");
	fprintf(f, "\n");

	fprintf(f, "static const char * reason_phrase[] = {");
	fprintf(f, "\n\t\"\",");
	for (i = 0; (s = response[i].phrase) != NULL; i++) {
		if (i == 0)
			fprintf(f, "\n\t");
		else
			fprintf(f, ",\n\t");

		fprintf(f, "\"%s\"", s);
	}
	fprintf(f, "\n};\n\n");

	comment_block(f, "Reason phrase hash table");

	if ((r = mk_i_tab(f, i_tab, 256)) < 0) {
		fprintf(f, "#error \"Perfect hashing is not possible\"\n");
		fprintf(f, "\n");
		fclose(f);
		return 1;
	}

	dump_tab(f, "static const", i_tab, 256, "i_tab");

	fprintf(f, "const char * http_reason_phrase(unsigned int code)\n{\n");
	fprintf(f, "\tunsigned int h;\n\n");
	if (r == 1)
		fprintf(f, "\th = code;\n");
	else
		fprintf(f, "\th = code * %d;\n", r);
//	fprintf(f, "\th ^= h >> 16;\n");
	fprintf(f, "\th ^= h >> 8;\n");
	fprintf(f, "\treturn reason_phrase[i_tab[h & 0xff]];\n}\n\n");

	return 0;
}


int main(int argc, char *argv[])
{

	if (make_h("http_hdr.h") < 0)
		return 1;

	if (make_hdr_c("http_hdr.c") < 0)
		return 2;

	if (make_status_c("http_resp.c") < 0)
		return 4;

	if (make_mime_c("http_mime.c") < 0)
		return 5;

	return 0;
}

