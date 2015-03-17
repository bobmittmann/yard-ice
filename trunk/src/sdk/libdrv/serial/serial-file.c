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
 * @file stm32f-serial.c
 * @brief STM32F serial driver
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#define __STM32_SERIAL_I__
#include "stm32-serial-i.h"

/* ----------------------------------------------------------------------
 * Serial file operations 
 * ----------------------------------------------------------------------
 */

const struct fileop serial_fileop = {
	.write = (void *)stm32f_serial_write,
	.read = (void *)stm32f_serial_read,
	.flush = (void *)stm32f_serial_flush,
	.close = (void *)stm32f_serial_close
};

struct file * serial_fopen(struct serial_dev * dev)
{
	return file_alloc(dev, &serial_fileop);
}

