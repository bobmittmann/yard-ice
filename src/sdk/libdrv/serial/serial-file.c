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

#include <sys/serial.h>
#include <sys/file.h>
#include <stdio.h>

/* ----------------------------------------------------------------------
 * Serial file operations 
 * ----------------------------------------------------------------------
 */

int serial_send(struct serial_dev * dev, const void * buf, 
							  unsigned int len) {
	return dev->op->send(dev->drv, buf, len);
}

int serial_recv(struct serial_dev * dev, void * buf, 
							  unsigned int len, unsigned int msec) {
	return dev->op->recv(dev->drv, buf, len, msec);
}

int serial_drain(struct serial_dev * dev) {
	return dev->op->drain(dev->drv);
}

int serial_close(struct serial_dev * dev){
	return dev->op->close(dev->drv);
}

const struct fileop serial_fileop = {
	.write = (void *)serial_send,
	.read = (void *)serial_recv,
	.flush = (void *)serial_drain,
	.close = (void *)serial_close
};

struct file * serial_fopen(struct serial_dev * dev)
{
	return file_alloc(dev, &serial_fileop);
}

bool is_serial(struct file * f) 
{
	return (f->op == &serial_fileop) ? true : false;
}
