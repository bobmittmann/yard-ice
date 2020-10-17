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

#include "telnet_svc.h"

#include <sys/dcclog.h>

const struct fileop telnet_svc_ops = {
	.write = (void *)telnet_svc_write,
	.read = (void *)telnet_svc_read,
	.flush = (void *)telnet_svc_flush,
	.close = (void *)telnet_svc_release
};

/* FIXME file structure dynamic allocation */
struct file telnet_svc_file;

FILE * telnet_svc_fopen(struct telnet_svc * svc)
{
	struct file * f = (struct file *)&telnet_svc_file;

	DCC_LOG(LOG_MSG, "...");

	f->data = (void *)svc;
	f->op = &telnet_svc_ops; 

	return f;
}

