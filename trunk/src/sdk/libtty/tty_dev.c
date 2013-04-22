/* 
 * File:	 usb-cdc.c
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

#include <sys/file.h>
#include <thinkos.h>

#include <sys/dcclog.h>

#include <sys/tty.h>

struct tty_dev {
	/* underling device */
	void * data;
	const struct fileop * op;
};

int tty_write(struct tty_dev * tty, const void * buf, unsigned int len)
{
	char * cp;
	int cnt;
	char cr;
	int m;
	int n;

	cp = (char *)buf;
	n = len;
	cnt = 0;
	cr = '\r';

	/* search for control, '\n' or IAC */
	for (m = 0; (m < n) && (cp[m] != '\n'); m++);

	while (m < n) {
		if (cp[m] > 0) {
			/* send data except '\n' */
			if (m) {
				if (tty->op->write(tty->data, cp, m) <= 0)
					return cnt;
			}

			/* cr - force to send */
			if (tty->op->write(tty->data, &cr, 1) <= 0)
				return cnt;

			cnt += m + 1;
			cp += m;
			n -= m;
			m = 1;
		} else {
			if (m) {
				if (tty->op->write(tty->data, cp, m) <= 0)
					return cnt;
			}
			cnt += m;
			m++;
			cp += m;
			n -= m;
			m = 0;
		}
		/* search for next '\n' */
		for (; (m < n) && (cp[m] != '\n'); m++);
	};

	/* send the last chunk */
	if (m) {
		if (tty->op->write(tty->data, cp, m) <= 0)
			return cnt;
	}

	cnt += m;

	return cnt;
}

int tty_read(struct tty_dev * tty, void * buf, unsigned int len)
{
	return tty->op->read(tty->data, buf, len);
}

int tty_flush(struct tty_dev * tty)
{
	return tty->op->flush(tty->data);
}


/* FIXME: dynamic allocation */
struct tty_dev tty_dev;

struct tty_dev * tty_init(struct file * f)
{
	struct tty_dev * tty = &tty_dev;

	tty->op = f->op;
	tty->data = f->data;

	return tty;
}

