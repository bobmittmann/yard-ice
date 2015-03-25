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

#include <sys/param.h>
#include <sys/file.h>
#include <stdlib.h>
#include <stdio.h>

#include <sys/usb-cdc.h>
#include <sys/serial.h>

#include <sys/dcclog.h>

const struct serial_op cdc_acm_serial_op = {
	.send = (void *)usb_cdc_write,
	.recv = (void *)usb_cdc_read,
	.drain = (void *)usb_cdc_flush,
	.close = (void *)usb_cdc_release
};

extern struct usb_cdc_acm_dev usb_cdc_rt;

const struct serial_dev cdc_acm_serial_dev = {
	.drv = &usb_cdc_rt,
	.op = &cdc_acm_serial_op
};

