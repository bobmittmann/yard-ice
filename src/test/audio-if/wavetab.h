#ifndef __WAVETAB_H__
#define __WAVETAB_H__

#include <stdint.h>

#define SAMPLE_RATE 8000

static const int16_t wave_a3[] = /* 440 Hz */
{
	     0,    5530,   10410,   14069,   16077,   16198,   14417,  10945,
	  6188,     705,   -4861,   -9856,  -13695,  -15926,  -16288, -14739,
	-11459,   -6835,   -1408,    4183,    9284,   13295,   15746,  16349,
	 15033,   11952,    7469,    2109,   -3498,   -8695,  -12871, -15537,
	-16379,  -15299,  -12423,   -8090,   -2806,    2806,    8090,  12423,
	 15299,   16379,   15537,   12871,    8695,    3498,   -2109,  -7469,
	-11952,  -15033,  -16349,  -15746,  -13295,   -9284,   -4183,   1408,
	  6835,   11459,   14739,   16288,   15926,   13695,    9856,   4861,
	  -705,   -6188,  -10945,  -14417,  -16198,  -16077,  -14069, -10410,
	 -5530, 
};

static const int16_t wave_d3[] = /* 587 Hz */
{
	     0,    7230,   12976,   16058,   15843,   12374,    6365,   -951,
	 -8071,  -13535,  -16219,  -15574,  -11730,   -5479,    1898,   8885,
	 14048,   16326,   15252,   11047,    4573,   -2839,   -9669, -14513,
	-16378,  -14880,  -10326,   -3653,    3771,   10420,   14930,  16374,
	 14457,    9571,    2720,   -4689,  -11136,  -15296,  -16316, -13985,
	 -8783,   -1778,    5592,   11814,   15611,   16202,   13466,   7966,
	   830,   -6477,  -12453,  -15873,  -16034,  -12902,   -7122, 
};

static const int16_t wave_1k[] = /* 1000 Hz */
{
	     0,   11585,   16383,   11585,       0,  -11585,  -16383, -11585,
};

static const int16_t wave_2k[] = /* 2000 Hz */
{
	     0,   16383,       0,  -16383, 
};

static const int16_t wave_3k[] = /* 3000 Hz */
{
	     0,   12381,  -16216,    8857,    4616,  -14903,   14903,  -4616,
	 -8857,   16216,  -12381, 
};

static const int16_t wave_4k[] = /* 4000 Hz */
{
	     0,       0, 
};


struct tone {
	const int16_t * buf;
	uint32_t len;
};

static const struct tone wave_lut[] = {
	{ .buf = wave_a3, .len = sizeof(wave_a3) / sizeof(uint16_t) },
	{ .buf = wave_d3, .len = sizeof(wave_d3) / sizeof(uint16_t) },
	{ .buf = wave_1k, .len = sizeof(wave_1k) / sizeof(uint16_t) },
	{ .buf = wave_2k, .len = sizeof(wave_2k) / sizeof(uint16_t) },
	{ .buf = wave_3k, .len = sizeof(wave_3k) / sizeof(uint16_t) },
	{ .buf = wave_4k, .len = sizeof(wave_4k) / sizeof(uint16_t) },
};

#define WAVE_A3 0
#define WAVE_D3 1
#define WAVE_1K 2
#define WAVE_2K 3
#define WAVE_3K 4
#define WAVE_4K 5


#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif
#endif /* __WAVETAB_H__ */

