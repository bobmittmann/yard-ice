/*
 * WARNING: do not edit, this file was automatically generated.

   Implementation of perfect hashing to parse HTTP headers
   Perfect hashing uses a Pearson Hashing Function
*/

#include <stdint.h>
#include <stdlib.h>

/*
   Table generated with Fisher and Yates permutation
   algorithm, using libc's rand() and srand(10264)
*/

/* http://www.w3.org/Protocols/rfc2616/rfc2616-sec14.html */
const char * http_hdr_name[] = {
	"",
	"Accept",
	"Accept-Charset",
	"Accept-Encoding",
	"Accept-Language",
	"Accept-Ranges",
	"Age",
	"Allow",
	"Authorization",
	"Cache-Control",
	"Connection",
	"Content-Disposition",
	"Content-Encoding",
	"Content-Language",
	"Content-Length",
	"Content-Location",
	"Content-MD5",
	"Content-Range",
	"Content-Type",
	"Cookie",
	"Date",
	"ETag",
	"Expect",
	"Expires",
	"From",
	"Host",
	"If-Match",
	"If-Modified-Since",
	"If-None-Match",
	"If-Range",
	"If-Unmodified-Since",
	"Last-Modified",
	"Location",
	"Max-Forwards",
	"Origin",
	"Pragma",
	"Proxy-Authenticate",
	"Proxy-Authorization",
	"Range",
	"Referer",
	"Retry-After",
	"Server",
	"Set-Cookie",
	"Set-Cookie2",
	"TE",
	"Trailer",
	"Transfer-Encoding",
	"Upgrade",
	"Upgrade-Insecure-Requests",
	"User-Agent",
	"Vary",
	"Via",
	"Warning",
	"WWW-Authenticate",
	"X-Requested-With"
};

/* ----------------------------------------------------------------
 * HTTP header permutation table
 * ---------------------------------------------------------------- */

static const uint8_t p_tab[] = {
	254,  90, 102, 134, 147, 186, 248,  39,
	 56, 114, 126, 152, 189, 164,  88, 218,
	 74,  62, 247, 185, 230, 165, 108, 121,
	 50,  30, 222, 163, 142, 141, 236, 130,
	 42,  71, 213, 131,  85,  76,  84,  66,
	180, 145,  38, 212, 232,  15,  64,  13,
	202,  65,  26, 137, 214,  54,   6,   2,
	 34,  12, 190, 194, 175,  17, 244, 221,
	228,  32, 217, 243,  48,  10,  81, 146,
	 46,  82, 107, 171,   3,  21,  63, 181,
	 52,  80, 172, 103,  55, 160,   8,  41,
	 77, 111, 151, 136, 206, 209, 168,  49,
	210,  94,  70, 156,  51,  75, 205, 198,
	105, 183, 162,  36,  91, 140, 253, 220,
	148, 192, 211, 122, 251,   1,  92,  67,
	196,  69,  59, 238, 132,  61,  22, 159,
	227, 208, 149, 219, 123,  45,  78, 106,
	201, 216, 233,  93,  57, 117,  43, 187,
	169, 143, 138, 193, 124,  24, 139,  83,
	245, 242,  58,  27,  28,  47,  97,  73,
	 44, 173, 167, 199, 125, 207,  89, 200,
	184, 119,  29,  14, 128, 127, 115, 224,
	 96, 182,  40, 118,  11, 234,  95, 225,
	133, 170, 241,  98,  19,  31,  60, 129,
	176, 100,   7,   9,  99,  87, 150,   0,
	113, 229,  86, 191,  53, 250, 246, 239,
	204, 231, 116, 157, 101, 153, 203, 120,
	177,  25, 154, 195, 255, 110, 252, 104,
	226, 197,   4,  33,  20, 237,  68, 144,
	188, 235, 161,  72,  18, 135, 174, 179,
	 37, 158, 166, 109, 215, 155,  79, 178,
	240,  35, 112, 223, 249,   5,  16,  23
};

static const uint8_t h_tab[] = {
	  0,   0,   0,  19,   0,   0,   0,   0,
	  0,   0,   9,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   1,   0,
	  0,  43,   0,   0,   0,   0,   0,  47,
	  0,  31,   0,   0,   0,   0,   0,   0,
	 48,   0,  18,   0,   0,  32,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,  52,
	  0,   0,   0,   0,   0,  37,   0,   0,
	 50,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,  45,   0,   0,
	  0,  54,   0,   0,  17,   0,   0,   0,
	  0,   0,   7,   0,   0,   0,   0,   0,
	  0,   0,   0,  53,  33,  36,   0,   0,
	  0,   0,   0,   0,  29,   0,  40,  15,
	  0,   0,   0,  23,   0,   8,   0,   0,
	  0,  22,   0,   0,   0,   0,   0,   0,
	  0,  27,   0,   0,   0,   0,   0,   0,
	  0,  49,  14,  35,   0,   0,   0,   0,
	  0,   0,  41,   0,  30,  21,  38,   0,
	  0,   0,   0,   0,  16,   0,   0,  24,
	  3,   0,   0,   2,   0,   0,   0,   0,
	 28,   0,  13,   0,   0,  34,   0,   0,
	  0,   0,   6,   0,   0,   0,   0,   0,
	  0,  25,   0,   0,   0,   0,   0,   0,
	 11,   0,  10,   0,   0,   0,   0,   0,
	  0,  46,  20,   0,   0,   0,   0,   0,
	  0,  26,   0,  44,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,  51,   0,   0,
	 12,   4,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,  42,   0,  39,   0,   0,
	  0,   5,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0
};

/* ----------------------------------------------------------------
 * Get header id from header name
 * ---------------------------------------------------------------- */

unsigned int http_hdr_id(const char * str)
{
	int h = 0;
	int c;
	int i;

	for (i = 0; (c = str[i]) != '\0'; ++i) {
		h = p_tab[c ^ h];
	}

	return h_tab[h];
}

/* ----------------------------------------------------------------
 * Parse header field, return field id and value.
 * ---------------------------------------------------------------- */

unsigned int http_parse_field(const char * str, char ** endptr)
{
	int h = 0;
	int c;
	int i;

	for (i = 0; (c = str[i]) != '\0'; ++i) {
		if ((c == ':') || (c == ' ')) {
			if (endptr != NULL) {
				char * cp = (char *)&str[i + 1];
				while (*cp == ' ')
					cp++;
				*endptr = cp;
			}
			return h_tab[h];
		}
		h = p_tab[c ^ h];
	}

	return 0;
}


#ifndef HTTP_HDR_TEST
#define HTTP_HDR_TEST 0
#endif

#if HTTP_HDR_TEST

#include <stdio.h>
#include <assert.h>

int http_parser_test(void)
{
	int i;

	for (i = 1; i < sizeof(http_hdr_name) / sizeof(char *); ++i) {
		char s[512];
		char * cp;
		int hdr;

		sprintf(s, "%s: field-value\n", http_hdr_name[i]);
		hdr = http_parse_field(s, &cp);

		assert(hdr == i);

		if (hdr != i) {
			printf("Can't parse: \"%s\"\n", s);
			return -1;
		}
	}

	return 0;
}

#endif /* HTTP_HDR_TEST */

