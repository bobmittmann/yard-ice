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
 * @file config.h
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#ifndef __CONFIG_H__
#define __CONFIG_H__

#ifndef FPGA_RBF_ADDR 
#define FPGA_RBF_ADDR 0x01050000
#endif

#define JTAG_VEC_BITS_MAX 256

#define JTAG_TAP_MAX 8

#define	ENABLE_HISTORY 1

#define ENABLE_SHELL_PROMPT_SHOW_TARGET_STATUS 1

#define ENABLE_ARMICE_ARM9 1

#define SHELL_ARG_MAX 16

#define ENABLE_ICE_TEST 0

#endif /* __CONFIG_H__ */

