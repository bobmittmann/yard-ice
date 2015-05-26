/* 
 * File:	 stdio.c
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
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


#include <thinkos.h>
#include <sys/file.h>
#include <stdbool.h>

int console_write(void * dev, const void * buf, unsigned int len) 
{
	unsigned int rem = len;
	uint8_t * cp = (uint8_t *)buf;
	int n;

	while (rem) {
		n = thinkos_console_write(cp, rem);
		cp += n;
		rem -= n;
	}

	return len;
}

int console_read(void * dev, void * buf, unsigned int len, unsigned int msec) 
{
	return thinkos_console_timedread(buf, len, msec);
}

int console_drain(void * dev)
{
	return thinkos_console_drain();
}

int console_close(void * dev)
{
	return thinkos_console_close();
}

const struct fileop console_fops = {
	.write = (void *)console_write,
	.read = (void *)console_read,
	.flush = (void *)console_drain,
	.close = (void *)console_close
};

const struct file console_file = {
	.data = 0, 
	.op = &console_fops 
};

struct file * console_fopen(void)
{
	struct file * f;
	f = (struct file *)&console_file;
	return f;
}

bool is_console_file(struct file * f) 
{
	return (f->op == &console_fops) ? true : false;
}

