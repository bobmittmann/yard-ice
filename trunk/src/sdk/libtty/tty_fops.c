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

#include <sys/tty.h>

#include <sys/dcclog.h>

const struct fileop tty_ops = {
	.write = (void *)tty_write,
	.read = (void *)tty_read,
	.flush = (void *)tty_flush,
	.close = (void *)NULL
};

/* FIXME file structure dynamic allocation */
struct file tty_file;

FILE * tty_fopen(struct tty_dev * dev)
{
	struct file * f = (struct file *)&tty_file;

	DCC_LOG(LOG_TRACE, "...");

	f->data = (void *)dev;
	f->op = &tty_ops; 

	return f;
}

