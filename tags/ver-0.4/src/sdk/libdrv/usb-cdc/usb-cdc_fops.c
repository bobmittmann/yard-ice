/* 
 * File:	 usb-test.c
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(C) 2011 Bob Mittmann. All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <sys/stm32f.h>
#include <arch/cortex-m3.h>
#include <sys/delay.h>
#include <sys/serial.h>
#include <sys/param.h>
#include <sys/file.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <sys/dcclog.h>

#include <sys/usb-cdc.h>

const struct fileop usb_cdc_ops= {
	.write = (void *)usb_cdc_write,
	.read = (void *)usb_cdc_read,
	.flush = (void *)usb_cdc_flush,
	.close = (void *)NULL
};

extern void * usb_cdc_dev;

const struct file usb_cdc_file = {
	.data = (void *)&usb_cdc_dev, 
	.op = &usb_cdc_ops
};

struct file * usb_cdc_fopen(void)
{
	struct usb_cdc_class * dev = (struct usb_cdc_class *)usb_cdc_file.data;

	usb_device_init(dev);

	return (struct file *)&usb_cdc_file;
}

