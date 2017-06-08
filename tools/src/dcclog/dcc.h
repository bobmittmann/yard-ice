/* $Id: dcclog.c,v 1.10 2006/09/28 19:31:45 bob Exp $ 
 *
 * File:	dcc.h
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

#ifndef __DCC_H__
#define __DCC_H__

#include <inttypes.h>

#define LVL_PANIC   1
#define LVL_EXCEPT  2
#define LVL_ERROR   3
#define LVL_WARNING 4
#define LVL_TRACE   5
#define LVL_INFO    6
#define LVL_MSG     7
#define LVL_YAP     8

struct dcclog_entry {
	uint32_t file;
	uint16_t line;
	uint8_t level;
	uint8_t opt;
	uint32_t function;
	uint32_t msg;
};

enum {
	LOG_OPT_NONE = 0,
	LOG_OPT_STR  = 1,
	LOG_OPT_XXD  = 2
};

struct log_def {
	uint32_t addr;
	int level;
	char * file;
	unsigned int line;
	unsigned int opt;
	char * function;
	char * fmt;
};

struct sym_def {
	uint32_t addr;
	int      size;
	uint8_t  type;
	uint8_t  binding;
	uint8_t  options;
	uint8_t  flags;
	char name[128];
};

struct mem_def {
	uint32_t addr;
	uint32_t size;
	uint32_t options;
	uint8_t * image;
};

struct dcc_lnk_op {
	int (* read_addr)(void * drv, uint32_t * addr);
	uint32_t (* read_u32)(void * drv);
	void * (* read_ptr)(void * drv);
	char * (* read_str)(void * drv);
	float (* read_float)(void * drv);
	uint8_t * (* read_bin)(void * drv, unsigned int len);
	int (* close)(void * drv);
};

struct dcc_lnk {
	void * drv;
	const struct dcc_lnk_op * op;
};

/* log entries */ 
extern struct log_def dcc[16384];
extern int dcc_count;

/* symbol table */ 
extern struct sym_def sym[16384];
extern int sym_count;

/* memory block content */ 
extern struct mem_def mem[64];
extern int mem_count;

extern const char * level_tab[];

#ifdef __cplusplus
extern "C" {
#endif

char * log_level(struct log_def * log);
void * image_ptr(uint32_t addr);
struct sym_def * sym_lookup(uint32_t addr);
struct sym_def * get_sym_by_name(const char * name);
struct sym_def * get_sym(int i);
int add_sym(char * section, char * name, uint32_t addr, uint32_t size);


static inline int dcc_read_addr(struct dcc_lnk * lnk, uint32_t * addr) {
	return lnk->op->read_addr(lnk->drv, addr);
};

static inline uint32_t dcc_read_u32(struct dcc_lnk * lnk) {
	return lnk->op->read_u32(lnk->drv);
};

static inline float dcc_read_float(struct dcc_lnk * lnk) {
	return lnk->op->read_float(lnk->drv);
};

static inline void * dcc_read_ptr(struct dcc_lnk * lnk) {
	return lnk->op->read_ptr(lnk->drv);
};

static inline char * dcc_read_str(struct dcc_lnk * lnk) {
	return lnk->op->read_str(lnk->drv);
};

static inline uint8_t * dcc_read_bin(struct dcc_lnk * lnk, unsigned int len) {
	return lnk->op->read_bin(lnk->drv, len);
};

static inline int dcc_close(struct dcc_lnk * lnk) {
	return lnk->op->close(lnk->drv);
};

int net_connect(struct dcc_lnk * lnk, char * host, int port);

int dcc_file_link(struct dcc_lnk * lnk, FILE * f);

#ifdef __cplusplus
}
#endif	

#endif /* __DCC_H__ */
