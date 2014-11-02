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

#include <sys/dcclog.h>

#include <sys/usb-cdc.h>

const struct fileop usb_cdc_ops= {
	.write = (void *)usb_cdc_write,
	.read = (void *)usb_cdc_read,
	.flush = (void *)usb_cdc_flush,
	.close = (void *)usb_cdc_release
};

FILE * usb_cdc_fopen(struct usb_cdc_class * dev)
{
	return file_alloc(dev, &usb_cdc_ops);
}

bool usb_cdc_is_usb_file(FILE * f)
{
	return (f != NULL) && (f->op == &usb_cdc_ops);
}

