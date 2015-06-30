/*
 * WARNING: do not edit, this file was automatically generated.
 */

#include <stdint.h>
#include <stdlib.h>

/* Table generated with Fisher and Yates permutation 
   algorithm, using libc's random() and srandom(10813) */

const char * rtsp_hdr_name[] = {
	"",
	"RTSP/1.0",
	"Accept",
	"Accept-Charset",
	"Accept-Encoding",
	"Accept-Language",
	"Allow",
	"Authorization",
	"Bandwidth",
	"Blocksize",
	"Cache-Control",
	"Conference",
	"Connection",
	"Content-Base",
	"Content-Encoding",
	"Content-Language",
	"Content-Length",
	"Content-Location",
	"Content-Type",
	"CSeq",
	"Date",
	"Expires",
	"From",
	"If-Modified-Since",
	"Last-Modified",
	"Proxy-Authenticate",
	"Proxy-Require",
	"Public",
	"Range",
	"Referer",
	"Require",
	"Retry-After",
	"RTP-Info",
	"Scale",
	"Session",
	"Server",
	"Speed",
	"Supported",
	"Timestamp",
	"Transport",
	"Unsupported",
	"User-Agent",
	"Via",
	"WWW-Authenticate",
	"X-Accept-Authentication",
	"ClientChallenge",
	"ClientID",
	"PlayerStarttime",
	"CompanyID",
	"GUID",
	"RegionData",
	"DESCRIBE",
	"ANNOUNCE",
	"GET_PARAMETER",
	"OPTIONS",
	"PAUSE",
	"PLAY",
	"RECORD",
	"REDIRECT",
	"SETUP",
	"SET_PARAMETER",
	"TEARDOWN"
};

static const char * reason_phrase[] = {
	"",
	"Continue",
	"OK",
	"Created",
	"Accepted",
	"Bad Request",
	"Authorization Required",
	"Forbidden",
	"Not Found",
	"Not Acceptable",
	"Unsupported Media Type",
	"Parameter Not Understood",
	"Not Enough Bandwith",
	"Session Not Found",
	"Method Not Valid In This State",
	"Header Field Not Valid for Resource",
	"Invalid Range",
	"Aggregate Operation Not Allowed",
	"Only Aggregate Operation Allowed",
	"Unsupported Transport",
	"Internal Server Error",
	"Not Implemented",
	"Service Unavailable",
	"RTSP Version Not Supported",
	"Option not supported"
};

/* ----------------------------------------------------------------
 * RTSP header permutation table
 * ---------------------------------------------------------------- */

static const uint8_t p_tab[] = {
	194,  25, 197, 182, 253, 196, 150, 127,
	 54, 229, 159, 241,  45, 162,  70,  42,
	 56, 209, 111, 136, 119, 185, 210,  13,
	138,  22, 231, 248,  81,  30,  80,  19,
	224, 125, 178,  18,  16,  41, 102, 160,
	149,  71, 170,  27,  46, 242, 128,  37,
	 50, 134, 120,  44, 205, 142, 217,  33,
	 95, 107, 228, 151,  51, 186, 249, 176,
	245,  85, 146,  79, 246,  83,  28, 219,
	 48, 121, 190, 177, 108, 237,   2, 133,
	 47,  52,  87, 213,  93,   1, 227, 152,
	  0, 124, 112, 165,  74, 179, 137,  23,
	202, 221, 141, 181, 139,  10,  26, 255,
	193,  20,  34,   3, 208,  96, 106,  17,
	200, 235, 216, 240, 118,  39,  21, 113,
	247, 215,  40,  12, 143, 129,  49, 188,
	244, 174,   7, 250, 223,  29,  68, 154,
	175, 187, 218, 254, 167, 234,  65, 110,
	 15, 207, 131, 173,  61, 214, 135, 201,
	211,   8, 206, 192, 184,  76,  14, 144,
	183, 155, 158, 147, 203,  11, 114,  43,
	195,  55, 168, 232, 161, 103,  86,  82,
	163,  73,  88, 204, 239,  69, 252,  32,
	 75, 226, 117,  77, 180, 166, 132, 164,
	 84, 126,  64,   6, 156,  53,  72,  24,
	238,  92,  66,  91,  35, 100, 220,  62,
	 94, 157, 191, 243, 101, 236,   9, 130,
	 58,  99,   4,  98, 198,  60, 145, 153,
	172,  90, 115,  67,  63, 123,  59, 105,
	233, 122, 116,  36, 251,  57, 230, 148,
	 89, 225,   5,  97, 169, 171, 140,  78,
	 38, 104, 199, 222, 212, 189, 109,  31
};

static const uint8_t h_tab[] = {
	  0,   0,  36,   0,   0,  46,  20,  37,
	  0,   0,   0,   0,   0,  14,   0,   0,
	  0,   0,  47,   0,   7,   0,   0,   0,
	  0,   0,   0,  17,   0,  33,   0,   0,
	  0,   0,   0,   0,   0,  24,  51,   0,
	  0,   0,   0,  25,   0,   0,   0,   0,
	 15,   0,   0,  34,   0,   4,   0,  22,
	  0,   0,   0,   0,  13,   0,   0,   0,
	 39,   0,   0,   0,   0,   0,   0,  43,
	  0,  10,   0,   0,   0,  42,   1,  16,
	  0,   0,   0,   0,   0,   0,   0,  52,
	  0,  56,   0,  23,   0,  40,   0,   0,
	 58,   0,   0,   0,  21,   0,   0,   0,
	  0,   0,  54,   0,   0,   0,   0,  12,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,  31,   0,   0,
	  0,   0,   0,   0,  28,   6,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   3,   0,   0,  26,  44,  55,  29,
	  0,  53,   0,   0,   0,   0,   0,  48,
	 35,   0,   0,   0,   0,   9,   0,   0,
	  0,  41,  57,   0,   0,   0,   0,   0,
	  0,   0,   0,  32,  19,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	 50,  11,   5,   0,   0,   8,   0,  27,
	  0,   0,   0,  30,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,  60,
	 49,  61,   0,   0,   0,  18,   0,   0,
	  0,   0,   0,   0,   0,  59,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,  38,
	  0,   0,   0,   0,  45,   0,   2,   0,
	  0,   0,   0,   0,   0,   0,   0,   0
};

/* ----------------------------------------------------------------
 * Get header id from header name
 * ---------------------------------------------------------------- */

unsigned int rtsp_hdr_id(const char * s)
{
	int i, c;
	int h = 0;

	while ((c = *s++) != '\0') {
		i = h ^ c;
		h = p_tab[i];
	}

	return h_tab[h];
}

/* ----------------------------------------------------------------
 * Parse header line into header id and value
 * ---------------------------------------------------------------- */

unsigned int rtsp_parse_hdr(const char * buf, char ** val)
{
	int h = 0;
	int c;
	int i;
	for (i = 0; (c = buf[i]) != '\0'; ++i) {
		if ((c == ':') || (c == ' ')) {
			if (val != NULL) {
				char * cp = (char *)&buf[i + 1];
				/* remove leading blanks */
				while (*cp == ' ')
					cp++;
				*val = cp;
			}
			return h_tab[h];
		}
		h = p_tab[c ^ h];
	}
	return 0;
}

/* ----------------------------------------------------------------
 * Reason phrase hash table
 * ---------------------------------------------------------------- */

static const uint8_t i_tab[] = {
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,  12,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,  20,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,  14,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	 13,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   2,  22,  16,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   3,   0,  15,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,  21,  17,   0,   4,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,  19,   0,   0,   0,   0,
	  0,   0,   5,   0,   0,   0,   0,   0,
	  0,   0,  18,   0,  10,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   1,   0,   0,   0,   0,   0,
	 23,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,  24,   0,   0,   0,   0,
	  0,   0,   0,   6,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   8,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   7,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   9,   0,  11,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0
};

const char * rtsp_reason_phrase(unsigned int code)
{
	uint32_t h;

	h = code * 17;
	h ^= h >> 16;
	h ^= h >> 8;
	return reason_phrase[i_tab[h & 0xff]];
}

