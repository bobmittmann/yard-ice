/* $Id: dcclog.c,v 1.10 2006/09/28 19:31:45 bob Exp $ 
 *
 * File:	dcclog.c
 * Module:
 * Project:	ARM-DCC logger expander
 * Author:	Robinson Mittmann (bob@boreste.com, bob@methafora.com.br)
 * Target:
 * Comment:
 * Copyright(C) 2005 Robinson Mittmann. All Rights Reserved.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

#include <unistd.h>
#include <inttypes.h>
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>

#if defined(WIN32)
  #include <io.h>
  #include <fcntl.h>
  #ifndef in_addr_t
    #define in_addr_t uint32_t
  #endif
#else
  #include <pthread.h>
  #include <assert.h>
#endif

#include "dcc.h"

const char * level_tab[] = {
	"NONE",
	"PANIC",
	"EXCEPT",
	"ERROR",
	"WARNING",
	"TRACE",
	"INFO",
	"MSG",
	"YAP",
	"LVL9",
	"LVL10",
	"LVL11",
	"LVL12",
	"LVL13",
	"LVL14",
};

/* log entries */ 
struct log_def dcc[16384];
int dcc_count = 0;

/* symbol table */ 
struct sym_def sym[16384];
int sym_count = 0;

/* memory block content */ 
struct mem_def mem[64];
int mem_count = 0;

extern int verbose;
extern char * prog;

struct sym_def * sym_lookup(uint32_t addr) 
{
	int i;

	for (i= 0; i < sym_count; i++) {
		if ((addr >= sym[i].addr) && (addr < sym[i].addr + sym[i].size)) {
			return &sym[i];
		}
	}

	return NULL;
}

struct sym_def * get_sym_by_name(const char * name) 
{
	int i;

	for (i= 0; i < sym_count; i++) {
		if (strcmp(sym[i].name, name) == 0) {
			return &sym[i];
		}
	}

	return NULL;
}

struct sym_def * get_sym(int i) 
{
	if (!sym_count)
		return NULL;

	return &sym[i];
}

int add_sym(char * section, char * name, uint32_t addr, uint32_t size)
{

	if (section == NULL)
		return -1;

	if (name == NULL)
		return -1;
	
	if ((strcmp(section, ".dcclog")  == 0) && 
		(size == sizeof(struct dcclog_entry))) {
		if (verbose) {
			if (verbose > 1) {
				printf("+log: ");
			}
			printf("%12s  %08x %6d %s\n", section, addr, size, name); 
			fflush(stdout);
		}
		dcc[dcc_count++].addr = addr;
		return 0;
	}

	if ((strcmp(section, ".text")  == 0) || 
		(strcmp(section, ".init")  == 0)) {
		if (verbose) {
			if (verbose > 1) {
				printf("+log: ");
			}
			printf("%12s  %08x %6d %s\n", section, addr, size, name); 
			fflush(stdout);
		}
		sym[sym_count].addr = addr;
		sym[sym_count].size = size;
		strcpy(sym[sym_count].name, name);
		sym_count++; 
		return 0;
	} 

	if (verbose) {
		if (verbose > 1) {
			printf("-   : ");
		}
		printf("%12s  %08x %6d %s\n", section, addr, size, name); 
		fflush(stdout);
	}
	return -1;
}

char * log_level(struct log_def * log)
{
	int level;

	if (log == NULL) {
		fprintf(stderr, "%s:%s log is NULL.\n", prog, __func__);
		return NULL;
	}

	level = log->level;

	if ((level < LVL_PANIC) && (level > LVL_YAP)) {
		fprintf(stderr, "%s:%s invalid level: %d.\n", 
				__FILE__, __func__, level);
		return NULL;
	}

	return (char *)level_tab[level];
}

/*
 * Get an local pointer from target memory image
 */
void * image_ptr(uint32_t addr)
{
	int32_t offs;
	int i;


	for (i = 0; i < mem_count; i++) {
		//		printf("%08x %8d <- %08x\n", mem[i].addr, mem[i].size, addr);
		offs = addr - mem[i].addr;
		if ((offs >= 0) && (offs < mem[i].size)) {
			return (void *) &mem[i].image[offs];
		}
	}

	fprintf(stderr, "ERROR: %s(): can't translate pointer %08x\n",
			__func__, addr);
	return NULL;
}

