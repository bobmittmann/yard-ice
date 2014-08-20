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

		if ((strncmp(cstr, s, len) == 0) && (strlen(cstr) == len))
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

int const_str_write(const char * s, unsigned int len)
{
	char buf[MICROJS_STRING_LEN_MAX];
	struct microjs_str_pool * pool;
	int idx;
	
	pool = (struct microjs_str_pool *)&microjs_str_const;

	if (len == 0) {
		DCC_LOG(LOG_WARNING, "empty string!");
	} 

	if ((idx = microjs_str_lookup(pool, s, len)) >= 0) {
		DCC_LOG2(LOG_INFO, "match idx=%d len=%d", idx, len);
		return idx;
	}

	/* NULL terminate the string */
	memcpy(buf, s, len);
	buf[len] = '\0';

	return pool->write(pool, buf, len + 1);
}

int microjs_str_pool_dump(const struct microjs_str_pool * pool)
{
	char * base = (char *)pool->base;
	int offs;
	int size = pool->top - base;
	int free = size;
	int i;

	for (i = 0; (offs = pool->offs[i]) > 0; ++i) {
		if (offs < free)
			free = offs;
		char * cstr = base + offs;
		printf("%2d (%04x) \"%s\"\n", i, offs, cstr);
	}
	printf("- pool: size=%d free=%d\n", size, free);

	return i;
}
