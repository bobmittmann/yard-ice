/*
 * eval.h
 * 
 * Copyright (C) 2010 Robinson Mittmann                                 
 * bobmittmann@gmail.com
 *
 * This file is part of YARD-ICE
 *
 * YARD: Yet Another Remote Debugger
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http ://www.gnu.org/licenses/>.
 */


#ifndef __YARD_EVAL_H__
#define __YARD_EVAL_H__

#include "val.h" 
#include "var.h" 

#ifdef __cplusplus
extern "C" {
#endif

int eval_uint32(value_t * val, int argc, char * argv[]);

#ifdef __cplusplus
}
#endif

#endif /* __YARD_EVAL_H__ */

