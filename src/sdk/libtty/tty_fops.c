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
	.close = (void *)tty_release
};

FILE * tty_fopen(struct tty_dev * dev)
{
	if (dev == NULL) {
		DCC_LOG(LOG_WARNING, "(dev == NULL)!");
		return NULL;
	}

	DCC_LOG(LOG_TRACE, "...");

	return file_alloc(dev, &tty_ops);
}

int isfatty(struct file * f)
{
	return (f->op == &tty_ops) ? 1 : 0;
}

struct file * ftty_lowlevel(struct file * f)
{
	struct tty_dev * dev;

	if (f->op != &tty_ops) {
		DCC_LOG(LOG_WARNING, "not a tty!");
		return NULL;
	}

	dev = (struct tty_dev *)f->data;

	return tty_lowlevel(dev);
}

