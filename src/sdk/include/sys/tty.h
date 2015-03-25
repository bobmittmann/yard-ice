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
 * @file sys/tty.h
 * @brief YARD-ICE libstm32f
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 


#ifndef __SYS_TTY_H__
#define __SYS_TTY_H__

#include <stdint.h>
#include <sys/file.h>

struct tty_dev;

#ifdef __cplusplus
extern "C" {
#endif

struct tty_dev * tty_attach(const struct file * f);

struct file * tty_fopen(struct tty_dev * tty);

int tty_write(struct tty_dev * tty, 
			  const void * buf, unsigned int len);

int tty_read(struct tty_dev * tty, void * buf, unsigned int len);

int tty_flush(struct tty_dev * tty);

int tty_release(struct tty_dev * tty);



int isfatty(struct file * f);

struct file * tty_lowlevel(struct tty_dev * tty);

struct file * ftty_lowlevel(struct file * f);

void * tty_drv(struct tty_dev * tty);

void * ftty_drv(struct file * f);

#ifdef __cplusplus
}
#endif

#endif /* __SYS_TTY_H__ */

