/* 
 * Copyright(c) 2004-2012 BORESTE (www.boreste.com). All Rights Reserved.
 *
 * This file is part of the YARD-ICE.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You can receive a copy of the GNU Lesser General Public License from 
 * http://www.gnu.org/
 */

/** 
 * @file environ.c
 * @brief YARD-ICE libc
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef ENV_MAX 
#define ENV_MAX 8
#endif

char * env_tab[ENV_MAX];

const unsigned short env_max = ENV_MAX;

const char ** environ = (const char **)env_tab;

#if CONFIG_H
#include "config.h"
#endif

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <crc.h>

#include <sys/dcclog.h>

#if (ENABLE_CONFIG_SECTOR)

#if 0
struct conf_blk {
	uint32_t size: 8; 
	uint32_t key: 24;
	uint8_t data[];
};
#endif

struct conf_blk {
	uint8_t crc; 
	uint8_t size; 
	uint16_t key;
	uint8_t data[];
};

struct conf_blk * config_get(uint32_t key);
struct conf_blk * config_set(uint32_t key, const void * buf, int size);
int config_erase(void);
int config_list(struct conf_blk * blk[], unsigned int count);

#if 0
static uint32_t str_hash(const char * s)
{
	uint32_t c;
	uint32_t h = 0;

	for (;(c = *s) != '\0'; s++)
		h = 37 * h + c;

	return h;
}

static uint32_t mk_hash(const uint8_t * p, int n)
{
	uint32_t h = 0;

	for (; n > 0; n--)
		h = 37 * h + (uint32_t)*p++;

	return h;
}
#endif

static uint32_t str_hash(const char * s)
{
	return crc16ccitt_blk(0, s, strlen(s));
}

static uint32_t mk_hash(const uint8_t * p, int n)
{
	return crc16ccitt_blk(0, p, n);
}

char ** __environ(void * buf[])
{
	struct conf_blk ** blk;
	char ** env;
	char * cp;
	int c;
	int n;
	int i;
	int j;
	int k;

	blk = (struct conf_blk **)(void *)buf;
	if ((n = config_list(blk, ENVIRON_MAX)) < 0) {
		return NULL;
	}

	env = (char **)(void *)buf;
	k = 0;
	for (i = 0; i < n; i++) {
		cp = (char *)blk[i]->data;
		for (j = 0; j < blk[i]->size; j++) {
			if ((c = cp[j]) == '=') {
				if (blk[i]->key == mk_hash(blk[i]->data, j)) { 
					env[k++] = (char *)blk[i]->data;
				}
				break;
			}
			if (c == '\0')
				break;
		}
	}

	env[k] = NULL;

	return env;
}

char * getenv(const char *name)
{
	struct conf_blk * blk;
	uint32_t hash;
	char * val;
	int c;
	int i;

	if (name == NULL)
		return NULL;

	hash = str_hash(name);

	DCC_LOG2(LOG_TRACE, "name='%c...' hash=0x%04x", name[0], hash);

	if ((blk = config_get(hash)) == NULL) {
		DCC_LOG(LOG_WARNING, "not found!");
		return NULL;
	}

	val = (char *)blk->data;
	for (i = 0; i < blk->size; i++) {
		if ((c = *val++) == '=')
			return val;
		if (c == '\0')
			break;
	}

	return NULL;
}

int setenv(const char *name, const char * value, int overwrite)
{
	char s[64];
	struct conf_blk * blk;
	uint32_t hash;
	char * src;
	char * dst;
	int c;

	hash = str_hash(name);

	if ((blk = config_get(hash)) && (!overwrite)) {
		DCC_LOG(LOG_WARNING, "not overwrite!");
		return 0;
	}

	dst = s;
	src = (char *)name;

	while ((c = *src) != '\0') {
		*dst++ = c;
		src++;
	}

	*dst++ = '=';
	src = (char *)value;

	while ((*dst++ = *src++) != '\0');

	DCC_LOG2(LOG_TRACE, "%04x -> '%c...'", hash, s[0]);

	if (config_set(hash, s, strlen(s) + 1) < 0) {
		DCC_LOG(LOG_WARNING, "config_set()!");
		return -1;
	}

	return 0;
}

int unsetenv(const char *name)
{
	struct conf_blk * blk;
	uint32_t hash;

	hash = str_hash(name);

	if ((blk = config_get(hash)) == NULL) {
		DCC_LOG(LOG_TRACE, "nothing to do!");
		return 0;
	}

	if (config_set(hash, NULL, 0) < 0) {
		DCC_LOG(LOG_WARNING, "config_set()!");
		return -1;
	}

	return 0;
}

#else

char ** __environ(void * buf[])
{
	return NULL;
}

char * getenv(const char *name)
{
	return NULL;
}

int setenv(const char *name, const char * value, int overwrite)
{
	return -1;
}

int unsetenv(const char *name)
{
	return -1;
}

#endif
