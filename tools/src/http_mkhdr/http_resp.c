/*
 * WARNING: do not edit, this file was automatically generated.
 */

#include <stdint.h>
#include <stdlib.h>

static const char * reason_phrase[] = {
	"",
	"Continue",
	"Switching Protocols",
	"OK",
	"Created",
	"Accepted",
	"Non-Authoritative Information",
	"No Content",
	"Reset Content",
	"Partial Content",
	"Multiple Choices",
	"Moved Permanently",
	"Found",
	"See Other",
	"Not Modified",
	"Use Proxy",
	"Temporary Redirect",
	"Bad Request",
	"Unauthorized",
	"Payment Required",
	"Forbidden",
	"Not Found",
	"Method Not Allowed",
	"Not Acceptable",
	"Proxy Authentication Required",
	"Request Time-out",
	"Conflict",
	"Gone",
	"Length Required",
	"Precondition Failed",
	"Request Entity Too Large",
	"Request-URI Too Large",
	"Unsupported Media Type",
	"Requested range not satisfiable",
	"Expectation Failed",
	"Internal Server Error",
	"Not Implemented",
	"Bad Gateway",
	"Service Unavailable",
	"Gateway Time-out",
	"HTTP Version not supported"
};

/* ----------------------------------------------------------------
 * Reason phrase hash table
 * ---------------------------------------------------------------- */

static const uint8_t i_tab[] = {
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,  11,  10,  13,  12,
	 15,  14,  16,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   1,   2,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	 18,  17,  20,  19,  22,  21,  24,  23,
	 26,  25,  28,  27,  30,  29,  32,  31,
	 34,  33,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  3,   4,   5,   6,   7,   8,   9,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,  36,  35,  38,  37,
	 40,  39,   0,   0,   0,   0,   0,   0
};

const char * http_reason_phrase(unsigned int code)
{
	unsigned int h;

	h = code;
	h ^= h >> 8;
	return reason_phrase[i_tab[h & 0xff]];
}

