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
 * @file .h
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */



#ifndef __YARD_MODULE_H__
#define __YARD_MODULE_H__

#include "val.h" 
#include "var.h" 

typedef int (* module_init_t)(void * arg, int mod_id);
typedef int (* module_done_t)(void * arg, int mod_id);
typedef int (* module_var_get_t)(void * arg, int var_id, value_t * val);
typedef int (* module_var_set_t)(void * arg, int var_id, const value_t * val);


#define MODULE_NAME_MAX 15

/**
 * struct module_def - static definition of a module
 * @name: the name of the module
 */
struct module_def {
	char name[MODULE_NAME_MAX + 1];
	module_init_t init;
	module_done_t done;
	module_var_get_t var_get;
	module_var_set_t var_set;
};

typedef struct module_def module_def_t;

/**
 * struct module_rec - registered module info
 * @def: pointer to the module definition
 * @ptr: pointer to a private module data
 */
struct module_rec {
	struct module_def * def;
	void * ptr;
};

typedef struct module_rec module_rec_t;


extern inline int mod_var_get(module_rec_t * mod, int var_id, 
							  value_t * val) {
	return mod->def->var_get(mod->ptr, var_id, val);
}

extern inline int mod_var_set(module_rec_t * mod, int var_id, 
							  const value_t * val) {
	return mod->def->var_set(mod->ptr, var_id, val);
}


#ifdef __cplusplus
extern "C" {
#endif

/**
 *  modules_init - initialize the modules framework
 *  @returns Return 0 on success, or -1 if an error occurred.
 */
int modules_init(void);

/**
 *  Remove a set of variables form the global pool
 *  @param tab An array of var_t declarations
 *  @param cnt The number of entries in the set
 *  @returns Return 0 on success, or -1 if an error occurred.
 */
int module_register(const struct module_def * def, void * ptr);

int module_unregister(int mod_id);

struct module_rec * get_module_rec(int mod_id);

char * module_name(int mod_id);

#ifdef __cplusplus
}
#endif

#endif /* __YARD_VAR_H__ */

