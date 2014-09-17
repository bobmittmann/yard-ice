/* 
 * Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
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
 * @file microjs-i.h
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


/*****************************************************************************
 * MicroJS internal (private) header file
 *****************************************************************************/

#ifndef __MICROJS_I_H__
#define __MICROJS_I_H__

#ifndef __MICROJS_I__
#error "Never use <microjs-i.h> directly; include <microjs.h> instead."
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef CONFIG_H
#include "config.h"
#endif

#include <microjs.h>
#include <microjs-rt.h>

#include "microjs_ll.h"

#ifndef MICROJS_STRINGS_ENABLED
#define MICROJS_STRINGS_ENABLED 1
#endif

#ifndef MICROJS_SYMBOL_LEN_MAX 
#define MICROJS_SYMBOL_LEN_MAX 16
#endif

#ifndef MICROJS_STRING_LEN_MAX 
#define MICROJS_STRING_LEN_MAX 128
#endif

#ifndef MICROJS_TRACE_ENABLED
#define MICROJS_TRACE_ENABLED 1
#endif

#ifndef MICROJS_STRINGBUF_ENABLED
#define MICROJS_STRINGBUF_ENABLED 1
#endif

#if (MICROJS_TRACE_ENABLED)
#define	TRACEF(__FMT, ...) do { \
	fprintf(stdout, __FMT, ## __VA_ARGS__); \
	fflush(stdout); } while (0)

#define	FTRACEF(__F, __FMT, ...) do { \
	fprintf(__F, __FMT, ## __VA_ARGS__); \
	fflush(__F); } while (0)
#else 
#define TRACEF(__FMT, ...) do { } while (0)
#define	FTRACEF(__F, __FMT, ...) do { } while (0)
#endif

/* --------------------------------------------------------------------------
  Lexical Analyzer
  -------------------------------------------------------------------------- */

struct lexer {
	uint16_t off;  /* lexer text offset */
	uint16_t len;  /* lexer text length */
	const char * txt;   /* base pointer (original js txt file) */
};

/* --------------------------------------------------------------------------
   Compiler 
   -------------------------------------------------------------------------- */

struct token {
	uint8_t typ; /* token type (class) */
	uint8_t qlf; /* qualifier */
	uint16_t off; /* offset */
	union {
		char * s;
		uint32_t u32;	
		int32_t i32;	
	};
};

/* --------------------------------------------------------------------------
   Syntax Directed Translator
   -------------------------------------------------------------------------- */

struct microjs_sdt {
	struct lexer lex;
	struct token tok;    /* token buffer */
	struct symtab * tab; /* symbol table */
	uint8_t * code;      /* compiled code */
	uint16_t cdsz;       /* code buffer size */
	uint16_t pc;         /* code pointer */
	uint16_t tgt_sp;     /* target stack pointer (top of the data memory) */
	uint16_t tgt_heap;  /* target data memory heap */
	uint16_t size;       /* SDT stack size */
	uint16_t ll_sp;      /* LL Parser stack pointer */
};

/* --------------------------------------------------------------------------
   String Pool
   -------------------------------------------------------------------------- */

struct strbuf {
	uint16_t cnt;
	uint16_t pos;
	uint16_t offs[];
};

/* --------------------------------------------------------------------------
   Symbol table 
   -------------------------------------------------------------------------- */

#define SYM_OBJ_ALLOC       (1 << 7)
#define SYM_OBJ_INT         (0x0 << 4)
#define SYM_OBJ_STR         (0x1 << 4)
#define SYM_OBJ_INT_ARRAY   (0x2 << 4)
#define SYM_OBJ_STR_ARRAY   (0x3 << 4)

#define SYM_OBJ_TYPE_MASK   (0x3 << 4)
#define SYM_OBJ_TYPE(SYM)   ((SYM)->flags & SYM_OBJ_TYPE_MASK) 
#define SYM_OBJ_IS_STR(SYM) (SYM_OBJ_TYPE(SYM) == SYM_OBJ_STR)
#define SYM_OBJ_IS_INT(SYM) (SYM_OBJ_TYPE(SYM) == SYM_OBJ_INT)

/* object */
struct sym_obj {
	uint8_t flags;
	uint8_t size;
	uint16_t nm;
	uint16_t addr;
};

#define SYM_EXTERN    (1 << 0)
#define SYM_METHOD    (1 << 1)
#define SYM_IS_EXTERN(SYM) ((SYM).flags & SYM_EXTERN)
#define SYM_IS_METHOD(SYM) ((SYM).flags & SYM_METHOD)

struct sym_tmp {
	uint8_t flags;
	uint8_t len;
	union {
		struct {
			char * s;
		};
		struct {
			uint8_t xid;
			uint8_t cnt;
			uint8_t min;
			uint8_t max;
		};
	};
};

struct sym_call {
	uint8_t flags;
	uint8_t nm;
	uint8_t xid;
	uint8_t cnt;
	uint8_t min;
	uint8_t max;
};


/* external function */
struct sym_ext {
	uint8_t flags;
	uint8_t xid;
	uint16_t addr;
};

/* object reference, this represent a pointer to a 
   target's memory location */
struct sym_ref {
	uint16_t lbl;
	uint16_t addr;
};

/* For Loop Descriptor */
struct sym_fld {
	uint16_t lbl;
	uint16_t addr[4];
	uint16_t brk; /* break list */
	uint16_t ctn; /* continue list */
};

/* While Loop Descriptor */
struct sym_wld {
	uint16_t lbl;
	uint16_t loop;
	uint16_t cond;
	uint16_t brk; /* break list */
	uint16_t ctn; /* continue list */
};

/* Function Descriptor */
struct sym_fnd {
	uint16_t nm;
	uint16_t skip;
	uint16_t ret; /* return list */
};

/* Stack frame */
struct sym_sf {
	uint16_t prev;
	uint16_t bp;
};

struct symtab {
	const struct ext_libdef * libdef;
	uint16_t sp;
	uint16_t fp;
	uint16_t bp;
	uint16_t top;
	uint16_t tmp_lbl;
	struct sym_obj buf[];
};

extern int32_t (* extern_call[])(struct microjs_env *, int32_t [], int);

#ifdef __cplusplus
extern "C" {
#endif

int lexer_open(struct lexer * lex, const char * txt, unsigned int len);

struct token lexer_scan(struct lexer * lex);

void lexer_print_err(FILE * f, struct lexer * lex, int err);

char * tok2str(struct token tok);

int ll_stack_dump(FILE * f, uint8_t * sp, uint8_t * sl);

int sym_dump(FILE * f, struct symtab * tab);

/* --------------------------------------------------------------------------
   Objects
   -------------------------------------------------------------------------- */

struct sym_obj * sym_obj_new(struct symtab * tab, 
							 const char * s, unsigned int len);

struct sym_obj * sym_obj_lookup(struct symtab * tab, 
								const char * s, unsigned int len);

struct sym_obj * sym_obj_scope_lookup(struct symtab * tab, 
									  const char * s, unsigned int len);

const char * sym_obj_name(struct symtab * tab, struct sym_obj * obj);

int sym_lbl_next(struct symtab * tab);

/* --------------------------------------------------------------------------
   Symbol table stack
   -------------------------------------------------------------------------- */

bool sym_push(struct symtab * tab, const void * ptr,  unsigned int len);

bool sym_pop(struct symtab * tab, void * ptr,  unsigned int len);

bool sym_push_str(struct symtab * tab, const char * s,  unsigned int len);

/* --------------------------------------------------------------------------
   Push/Pop addresses from/to stack
   -------------------------------------------------------------------------- */
static inline bool sym_addr_push(struct symtab * tab, uint16_t * addr) {
	return sym_push(tab, addr, sizeof(uint16_t));
}

static inline bool sym_addr_pop(struct symtab * tab, uint16_t * addr) {
	return sym_pop(tab, addr, sizeof(uint16_t));
}

/* --------------------------------------------------------------------------
   Push/Pop a stack frame (used to open/close scopes or blocks)
   -------------------------------------------------------------------------- */
bool sym_sf_push(struct symtab * tab);

bool sym_sf_pop(struct symtab * tab);

/* --------------------------------------------------------------------------
   References 
   -------------------------------------------------------------------------- */

/* Push a reference into the stack */
bool sym_ref_push(struct symtab * tab, struct sym_ref * ref);

bool sym_ref_pop(struct symtab * tab, struct sym_ref * ref);

/* --------------------------------------------------------------------------
   For Loop Descriptor
   -------------------------------------------------------------------------- */

bool sym_fld_push(struct symtab * tab, struct sym_fld * fld);

bool sym_fld_pop(struct symtab * tab, struct sym_fld * fld);


/* --------------------------------------------------------------------------
   While Loop Descriptor
   -------------------------------------------------------------------------- */

bool sym_wld_push(struct symtab * tab, struct sym_wld * wld);

bool sym_wld_pop(struct symtab * tab, struct sym_wld * wld);

/* --------------------------------------------------------------------------
   Function Descriptor
   -------------------------------------------------------------------------- */

bool sym_fnd_push(struct symtab * tab, struct sym_fnd * fnd);

bool sym_fnd_pop(struct symtab * tab, struct sym_fnd * fnd);

/* --------------------------------------------------------------------------
   Temporary symbols
   -------------------------------------------------------------------------- */

bool sym_tmp_push(struct symtab * tab, struct sym_tmp * tmp);

bool sym_tmp_pop(struct symtab * tab, struct sym_tmp * tmp);

/* --------------------------------------------------------------------------
   Externals (Library)
   -------------------------------------------------------------------------- */

int sym_extern_lookup(struct symtab * tab, const char * s, unsigned int len);

struct ext_fndef * sym_extern_get(struct symtab * tab, unsigned int xid);

const char * sym_extern_name(struct symtab * tab, unsigned int xid);


#ifdef __cplusplus
}
#endif

#endif /* __MICROJS_I_H__ */

