#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define __MICROJS_I__
#include "microjs-i.h"

#include <sys/dcclog.h>

int microjs_str_lookup(const struct microjs_str_pool * pool, 
					   const char * s, int len)
{
	char * base = (char *)pool->base;
	int offs;
	int i;

	for (i = 0; (offs = pool->offs[i]) > 0; ++i) {
		char * cstr = base + offs;

		if (strncmp(cstr, s, len) == 0)
			return i;
	}

	return -1;
}

char * const_str(int idx)
{
	const struct microjs_str_pool * p = &microjs_str_const;

	return p->base + p->offs[idx];
}

int const_str_lookup(const char * s, int len)
{
	return microjs_str_lookup(&microjs_str_const, s, len);
}

int microjs_str_pool_dump(const struct microjs_str_pool * pool)
{
	char * base = (char *)pool->base;
	int offs;
	int i;

	for (i = 0; (offs = pool->offs[i]) > 0; ++i) {
		char * cstr = base + offs;
		printf("%2d (%04x) \"%s\"\n", i, offs, cstr);

	}

	return i;
}
