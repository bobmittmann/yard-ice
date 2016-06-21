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

#ifndef MICROJS_FUNCTIONS_ENABLED
#define MICROJS_FUNCTIONS_ENABLED 0
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

#ifndef MICROJS_DEBUG_ENABLED 
#define MICROJS_DEBUG_ENABLED 1
#endif

#ifndef MICROJS_STRINGBUF_ENABLED
#define MICROJS_STRINGBUF_ENABLED 1
#endif

#ifndef MICROJS_VERBOSE_ENABLED 
#define MICROJS_VERBOSE_ENABLED 1
#endif

#ifndef MICROJS_STDLIB_ENABLED 
#define MICROJS_STDLIB_ENABLED 1
#endif

#ifndef MICROJS_OPTIMIZATION_ENABLED 
#define MICROJS_OPTIMIZATION_ENABLED 1
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
	const struct ext_libdef * libdef; /* external library */
	uint8_t * code;      /* compiled code */
	uint16_t cdsz;       /* code buffer size */
	uint16_t pc;         /* code pointer */

	uint16_t stack_pos;
	uint16_t data_pos;

	uint16_t size;       /* SDT stack size */
	uint16_t ll_sp;      /* LL Parser stack pointer */
#if MICROJS_OPTIMIZATION_ENABLED
	uint16_t spc;        /* saved code pointer */
#endif
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
#define SYM_OBJ_GLOBAL      (1 << 6)
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
	uint8_t prev;
	uint8_t next;
	uint8_t flags;
	uint8_t size;
	uint16_t addr;
	char nm[0];
} __attribute__((packed))__;

struct symtab {
	struct microjs_rt rt; /* run-time info */
	uint16_t sp;
	uint16_t bp;
	uint16_t fp;
	uint16_t top;
#if MICROJS_TRACE_ENABLED
	uint16_t tmp_lbl;
#endif
	struct sym_obj buf[];
};


#define SYM_EXTERN    (1 << 0)
#define SYM_METHOD    (1 << 1)
#define SYM_IS_EXTERN(SYM) ((SYM).flags & SYM_EXTERN)
#define SYM_IS_METHOD(SYM) ((SYM).flags & SYM_METHOD)

struct sym_tmp {
	char * s;
	uint8_t len;
};

/* object reference, this represent a pointer to a 
   target's memory location */
struct sym_ref {
#if MICROJS_TRACE_ENABLED
	uint16_t lbl;
#endif
	uint16_t addr;
};

/* For Loop Descriptor */
struct sym_fld {
#if MICROJS_TRACE_ENABLED
	uint16_t lbl;
#endif	
	uint16_t addr[4];
#if 0
	uint16_t brk; /* break list */
	uint16_t ctn; /* continue list */
#endif
};

/* While Loop Descriptor */
struct sym_wld {
#if MICROJS_TRACE_ENABLED
	uint16_t lbl;
#endif
	uint16_t loop;
	uint16_t cond;
#if 0
	uint16_t brk; /* break list */
	uint16_t ctn; /* continue list */
#endif
};

/* Function Descriptor */
struct sym_fnd {
	uint16_t nm;
	uint16_t skip;
	uint16_t ret; /* return list */
};

/* Class Descriptor */
struct sym_cld {
	uint8_t cid;
};

/* Function Call Descriptor */
struct sym_call {
	uint8_t xid;
	uint8_t argcnt;
	uint8_t argmin;
	uint8_t argmax;
	int8_t retcnt;
};

/* Stack frame */
struct sym_sf {
	uint16_t prev;
	uint16_t bp;
};

struct tabst {
	uint16_t sp;
	uint16_t bp;
};

#ifdef __cplusplus
extern "C" {
#endif

int lexer_open(struct lexer * lex, const char * txt, unsigned int len);

struct token lexer_scan(struct lexer * lex);

void lexer_print_err(FILE * f, struct lexer * lex, int err);

char * tok2str(struct token tok);

int ll_stack_dump(FILE * f, uint8_t * sp, uint8_t * sl);

/* --------------------------------------------------------------------------
   Objects
   -------------------------------------------------------------------------- */

struct sym_obj * sym_obj_new(struct symtab * tab, 
							 const char * s, unsigned int len);

struct sym_obj * sym_obj_lookup(struct symtab * tab, 
								const char * s, unsigned int len);

struct sym_obj * sym_obj_scope_lookup(struct symtab * tab, 
									  const char * s, unsigned int len);


static inline bool symtab_isempty(struct symtab * tab) {
	return (tab->sp == tab->top) ? true : false;
}

static inline const char * sym_obj_name(struct symtab * tab, 
										struct sym_obj * obj) {
	return (char *)obj->nm;
}

#if MICROJS_TRACE_ENABLED
static inline int sym_lbl_next(struct symtab * tab) {
	return tab->tmp_lbl++;
}
#endif

/* --------------------------------------------------------------------------
   Symbol table stack
   -------------------------------------------------------------------------- */

int sym_push(struct symtab * tab, const void * ptr,  unsigned int len);

int sym_pop(struct symtab * tab, void * ptr,  unsigned int len);

void sym_pick(struct symtab * tab, int pos, void * ptr,  unsigned int len);

/* --------------------------------------------------------------------------
   Push/Pop a stack frame (used to open/close scopes or blocks)
   -------------------------------------------------------------------------- */
int sym_sf_push(struct symtab * tab);

int sym_sf_pop(struct symtab * tab);

/* --------------------------------------------------------------------------
   Push/Pop addresses from/to stack
   -------------------------------------------------------------------------- */
static inline int sym_addr_push(struct symtab * tab, uint16_t * addr) {
	return sym_push(tab, addr, sizeof(uint16_t));
}

static inline int sym_addr_pop(struct symtab * tab, uint16_t * addr) {
	return sym_pop(tab, addr, sizeof(uint16_t));
}

/* --------------------------------------------------------------------------
   Classes
   -------------------------------------------------------------------------- */

/* Push a clderence into the stack */
static inline int sym_cld_push(struct symtab * tab, struct sym_cld * cld) {
	return sym_push(tab, cld, sizeof(struct sym_cld));
}

/* Pop a clderence from the stack */
static inline int sym_cld_pop(struct symtab * tab, struct sym_cld * cld) {
	return sym_pop(tab, cld, sizeof(struct sym_cld));
}

/* --------------------------------------------------------------------------
   References
   -------------------------------------------------------------------------- */

/* Push a reference into the stack */
static inline int sym_ref_push(struct symtab * tab, struct sym_ref * ref) {
	return sym_push(tab, ref, sizeof(struct sym_ref));
}

/* Pop a reference from the stack */
static inline int sym_ref_pop(struct symtab * tab, struct sym_ref * ref) {
	return sym_pop(tab, ref, sizeof(struct sym_ref));
}

/* --------------------------------------------------------------------------
   For Loop Descriptor
   -------------------------------------------------------------------------- */

static inline int sym_fld_push(struct symtab * tab, struct sym_fld * fld) {
	return sym_push(tab, fld, sizeof(struct sym_fld));
}

static inline int sym_fld_pop(struct symtab * tab, struct sym_fld * fld) {
	return sym_pop(tab, fld, sizeof(struct sym_fld));
}

/* --------------------------------------------------------------------------
   While Loop Descriptor
   -------------------------------------------------------------------------- */

static inline int sym_wld_push(struct symtab * tab, struct sym_wld * wld) {
	return sym_push(tab, wld, sizeof(struct sym_wld));
}

static inline int sym_wld_pop(struct symtab * tab, struct sym_wld * wld) {
	return sym_pop(tab, wld, sizeof(struct sym_wld));
}

/* --------------------------------------------------------------------------
   Function Descriptor
   -------------------------------------------------------------------------- */

static inline int sym_fnd_push(struct symtab * tab, struct sym_fnd * fnd) {
	return sym_push(tab, fnd, sizeof(struct sym_fnd));
}

static inline int sym_fnd_pop(struct symtab * tab, struct sym_fnd * fnd) {
	return sym_pop(tab, fnd, sizeof(struct sym_fnd));
}

/* --------------------------------------------------------------------------
   Function Call Descriptor
   -------------------------------------------------------------------------- */

static inline int sym_call_push(struct symtab * tab, struct sym_call * call) {
	return sym_push(tab, call, sizeof(struct sym_call));
}

static inline int sym_call_pop(struct symtab * tab, struct sym_call * call) {
	return sym_pop(tab, call, sizeof(struct sym_call));
}

/* --------------------------------------------------------------------------
   Temporary symbols
   -------------------------------------------------------------------------- */
static inline int sym_tmp_push(struct symtab * tab, struct sym_tmp * tmp) {
	return sym_push(tab, tmp, sizeof(struct sym_tmp));
}

static inline int sym_tmp_pop(struct symtab * tab, struct sym_tmp * tmp) {
	return sym_pop(tab, tmp, sizeof(struct sym_tmp));
}

/* --------------------------------------------------------------------------
   Externals (Library)
   -------------------------------------------------------------------------- */


static inline struct classdef * lib_classdef_get(
	const struct ext_libdef * libdef, int cid) {
	return (struct classdef *)&libdef->classtab->cdef[cid];
}

static inline struct extdef * lib_extern_get(const struct ext_libdef * libdef, 
											 unsigned int xid) {
	return (struct extdef *)&libdef->xdef[xid];
}

static inline const char * lib_extern_name(const struct ext_libdef * libdef, 
										   unsigned int xid) {
	return libdef->xdef[xid].nm;
}

static inline const char * lib_class_name(const struct ext_libdef * libdef, 
										   unsigned int cid) {
	return libdef->classtab->cdef[cid].nm;
}

int lib_lookup(const struct ext_libdef * libdef, const char * s, 
			   unsigned int len);

int lib_member_lookup(const struct ext_libdef * libdef, 
					  unsigned int cid, const char * s, unsigned int len);


#ifdef __cplusplus
}
#endif

#endif /* __MICROJS_I_H__ */

