#include <stdint.h>
#include <stdio.h>

char * q15_fmt(int16_t x)
{
	static char s[10];
	int32_t d;
	int32_t q;
	int32_t r;
	int sig = 0;

	if (x < 0) {
		sig = 1;
		x = -x;
	}

	d = (uint64_t)((uint64_t)x * (uint64_t)1000000LL) >> 15LL;
	q = d / 1000000;
	r = d % 1000000;

	if (sig)
		sprintf(s, "-%d.%06d", q, r);
	else
		sprintf(s, "0.%06d", r);

	return s;
}

