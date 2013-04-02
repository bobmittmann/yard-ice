#ifndef __WAVETAB_H__
#define __WAVETAB_H__

#include <stdint.h>

#define SAMPLE_RATE 8000

static const uint8_t a3[] = /* 440 Hz */
{
	   127,     170,     207,     235,     251,     252,     238,    211,
	   175,     132,      90,      51,      22,       5,       2,     14,
	    39,      74,     116,     159,     198,     229,     248,    253,
	   243,     219,     184,     143,     100,      60,      28,      8,
	     1,       9,      31,      65,     105,     149,     189,    223,
	   245,     253,     246,     226,     194,     154,     111,     70,
	    35,      11,       1,       6,      25,      56,      95,    138,
	   180,     215,     240,     252,     249,     232,     203,    164,
	   122,      79,      43,      16,       2,       3,      19,     47,
	    84, 
};

static const uint8_t d3[] = /* 587 Hz */
{
	   127,     183,     227,     251,     249,     222,     176,    120,
	    65,      23,       2,       7,      37,      85,     142,    195,
	   235,     253,     244,     212,     162,     105,      53,     15,
	     1,      13,      48,      99,     156,     207,     242,    253,
	   238,     201,     148,      91,      41,       9,       2,     19,
	    59,     113,     170,     218,     247,     252,     231,    188,
	   133,      77,      31,       5,       4,      28,      72, 
};


static const struct {
	const uint8_t * buf;
	uint32_t len;
} tone_lut[] = {
	{ .buf = a3, .len = sizeof(a3) },
	{ .buf = d3, .len = sizeof(d3) },
};

#define TONE_A3 0
#define TONE_D3 1


#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif
#endif /* __WAVETAB_H__ */

