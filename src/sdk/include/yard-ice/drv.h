/* 
 * Copyright(c) 2004-2012 BORESTE (www.boreste.com). All Rights Reserved.
 *
 * This file is part of the libcrc.
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
 * @file drv.h
 * @brief YARD-ICE libdrv
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __DRV_H__
#define __DRV_H__


#include <sys/file.h>
#include <yard-ice/audio.h>

#ifdef __cplusplus
extern "C" {
#endif

void stm32f_dac_init(void);

void stm32f_dac_vout_set(unsigned int mv);

void stm32f_adc_init(void);

void bsp_io_ini(void);

void relay_on(void);

void relay_off(void);

int relay_stat(void);

void ext_pwr_on(void);

void ext_pwr_off(void);

int ext_pwr_stat(void);

int ext_pwr_mon(void);

void tone_play(unsigned int tone, unsigned int ms);

#ifdef __cplusplus
}
#endif	

#endif /* __DRV_H__ */

