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
 * @file bkptctrl.h
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __BKPTCTRL_H__
#define __BKPTCTRL_H__

#include <stdint.h>
#include "ice_drv.h"

/* This structure controls breakpoints and watchpoints */

struct bkpt_ctrl {
	union {
		struct ice_breakpoint hdr;
		struct ice_hw_breakpoint ice_hw;
		struct ice_sw_breakpoint ice_sw;
		struct ice_hw_watchpoint ice_wp;
	};
};

struct bkpt_list;


#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
	}
#endif

#endif /* __BKPTCTRL_H__ */

