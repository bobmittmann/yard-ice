/*
 * WARNING: do not edit, this file was automatically generated.

   Implementation of perfect hashing to parse MIME types
   Perfect hashing uses a Pearson Hashing Function
*/

#include <stdint.h>
#include <stdlib.h>

/*
   Table generated with Fisher and Yates permutation
   algorithm, using libc's rand() and srand(10001)
*/

/* Mime types */
const char * mime_type_name[] = {
	"",
	"text/html",
	"text/plain",
	"text/css",
	"image/png",
	"image/jpeg",
	"image/gif",
	"application/javascript",
	"application/json",
	"application/xml",
	"multipart/form-data",
	"application/x-www-form-urlencoded"
};

/* ----------------------------------------------------------------
 * MIME type permutation table
 * ---------------------------------------------------------------- */

static const uint8_t p_tab[] = {
	247, 222, 253,   6, 132, 192, 161, 122,
	 64, 164,  44,  98, 225, 237,  42, 201,
	111,  19, 106, 244,   4, 182,  12, 243,
	176, 178, 125, 248, 127, 203, 220,  53,
	 61, 198, 205, 130, 102, 242,  45,  89,
	114, 227, 214, 165, 147,  74,  10, 179,
	 93,  60, 142, 168, 155,   3,   7, 186,
	204,  32,  30, 153, 241, 240,  11, 216,
	 50,  56,  77, 218,  48, 238, 104, 224,
	137, 172,  24, 246,  25, 148,  18, 173,
	 58, 251,  85,  94,  13,  23, 159,  41,
	 16, 128,  22,  26,  36,   1,  86,  29,
	140,  38, 181,  95, 185, 167,   0,  96,
	 88,   5,  72, 169, 146, 150, 136, 232,
	152,  21, 189,  78, 193, 235, 166, 129,
	188, 107,  97, 195, 170, 120,  67,  68,
	199,   8,  40, 250,  62, 145,  70, 254,
	 69, 103, 217, 190,  87, 229, 202, 149,
	 91, 115,  15,  17,  49, 187, 124, 158,
	 33, 118, 183, 135,  84,  37, 200,  79,
	 66,  39,  34, 108, 174, 210,  20,  90,
	113, 110,  51, 213,  92, 231, 100, 211,
	 35,  65, 219, 138, 162,  71,  75, 119,
	  9, 228, 197,  47, 207, 175,  31, 133,
	121, 209, 184, 249, 194, 255, 144, 116,
	156, 177, 171, 139,  82, 239, 233,  27,
	 63,  59, 157,  76, 191,  55, 143, 126,
	 52, 131, 154, 226, 215, 160, 234,  14,
	112, 245,  28, 134,  43,   2, 141, 117,
	101, 109, 252, 230,  46, 221, 196,  54,
	151, 180, 236,  83,  99, 223,  73, 212,
	 81, 163, 208, 206, 105,  80, 123,  57
};

static const uint8_t h_tab[] = {
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,  11,   0,   0,   0,   0,   0,
	  0,   0,   0,   2,   0,   0,   0,   1,
	  0,   0,   8,   0,   0,   0,   9,   0,
	  0,   0,   0,  10,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   5,   0,   3,   4,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   7,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   6,   0,   0,   0,   0
};

/* ----------------------------------------------------------------
 * Get header id from header name
 * ---------------------------------------------------------------- */

unsigned int mime_type_id(const char * str)
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
 * Parse content type.
 * ---------------------------------------------------------------- */

unsigned int http_parse_content_type(const char * str, char ** endptr)
{
	int h = 0;
	int c;
	int i;

	for (i = 0; ; ++i) {
		c = str[i];
		if (c == '\0') {
			if (endptr != NULL)
				*endptr = (char *)&str[i];
			return h_tab[h];
		}
		if (c == ';') {
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

