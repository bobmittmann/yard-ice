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

#ifdef __cplusplus
extern "C" {
#endif

void stm32f_dac_init(void);

void stm32f_dac_vout_set(unsigned int mv);


void stm32f_adc_init(void);

struct uart_console_dev * uart_console_init(unsigned int baudrate, 
											unsigned int flags);

struct file * uart_console_fopen(struct uart_console_dev * dev);


#ifdef __cplusplus
}
#endif	

#endif /* __DRV_H__ */

