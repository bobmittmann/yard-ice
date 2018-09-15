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
 * @file var.h
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */



#ifndef __YARD_VAR_H__
#define __YARD_VAR_H__

#include "val.h" 

/**
 * Variable definition
 */
struct var_def {
	char * name;
	uint8_t type;
	uint8_t mod_id;
	uint16_t id;
};

typedef struct var_def var_def_t;



/**
 * Helper structure for bulk variable registration
 */
struct var_entry {
	char name[13];
	uint8_t type;
	uint16_t id;
};

typedef struct var_entry var_entry_t;



#ifdef __cplusplus
extern "C" {
#endif


/**
 *  Add single variable form the global pool
 *  @param mod_id Module identification
 *  @param name Variable name 
 *  @param type Variable type
 *  @param var_id Variable identification
 *  @returns Return 0 on success, or -1 if an error occurred.
 */
int var_global_add(int mod_id, const char * name, int type, int var_id);

/**
 *  Remove a single variable form the global pool
 *  @param mod_id Module identification
 *  @param var_id Variable identification
 *  @returns Return 0 on success, or -1 if an error occurred.
 */
int var_global_del(int mod_id, int var_id);

/**
 *  Add a new set of variables to the global variables pool
 *  @param tab An array of var_t declarations
 *  @param cnt The number of entries in the set
 *  @param name The name of this variable set context
 *  @param arg A generic pointer (handler) for this context
 *  @returns Return 0 on success, or -1 if an error occurred.
 */
int var_global_bulk_add(int mod_id, const var_entry_t * tab, int cnt);

/**
 *  Remove all variables beloging to a module
 *  @returns Return the number of variables removed  on success, 
 * or -1 if an error occurred.
 */
int var_global_mod_del_all(int mod_id);

/**
 *  Find a variable in the global pool
 *  @param s An string withe the variable name
 *  @returns Return the variable on success, or NULL if an error occurred.
 */
var_def_t * var_global_lookup(const char * name);

/**
 *  Set the value of a varible
 *  @param var The variable
 *  @param val The value to be stored
 *  @returns Return 0 on success  or -1 if an error occurred.
 */
int var_set(var_def_t * var, const value_t * val);

/**
 *  Get the value of a variable
 *  @param var The variable
 *  @returns Return the value
 */
int var_get(var_def_t * var, value_t * val);

var_def_t * var_global_next(var_def_t * var);

#ifdef __cplusplus
}
#endif

#endif /* __YARD_VAR_H__ */

