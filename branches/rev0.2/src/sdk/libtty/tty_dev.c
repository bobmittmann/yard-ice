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
#include <stdlib.h>

#include <sys/dcclog.h>

#include <sys/tty.h>

#define TTY_INBUF_LEN 64
#define TTY_EDITBUF_SIZE 79

struct tty_dev {
	struct file f;

	char inbuf[TTY_INBUF_LEN];
	int inpos;
	int inlen;

	char editbuf[TTY_EDITBUF_SIZE + 1];
	int editpos;
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
				if (tty->f.op->write(tty->f.data, cp, m) <= 0)
					return cnt;
			}

			/* cr - force to send */
			if (tty->f.op->write(tty->f.data, &cr, 1) <= 0)
				return cnt;

			cnt += m + 1;
			cp += m;
			n -= m;
			m = 1;
		} else {
			if (m) {
				if (tty->f.op->write(tty->f.data, cp, m) <= 0)
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
		if (tty->f.op->write(tty->f.data, cp, m) <= 0)
			return cnt;
	}

	cnt += m;

	return cnt;
}

#define IN_BS      '\x8'
#define IN_DEL      0x7F
#define IN_EOL      '\r'
#define IN_SKIP     '\3'
#define IN_EOF      '\x1A'
#define IN_ESC      '\033'

#define OUT_DEL     "\x8 \x8"
#define OUT_EOL     "\r\n"
#define OUT_SKIP    "^C\r\n"
#define OUT_EOF     "^Z"
#define OUT_BEL     "\7"

static int tty_get_char(struct tty_dev * tty)
{
	int ret;

	while (tty->inpos >= tty->inlen) {
		if ((ret = tty->f.op->read(tty->f.data, 
								   tty->inbuf, TTY_INBUF_LEN)) < 0) {
			tty->inlen = 0;
			return ret;
		}

		tty->inlen = ret;
		tty->inpos = 0;
	}

	return tty->inbuf[tty->inpos++];
}

int tty_read(struct tty_dev * tty, void * buf, unsigned int len)
{
	char * s;
	int size;
	int pos;
	int c;

	s = (char *)buf;
	size = len;
	pos = 0;

	for (;;) {
		c = tty_get_char(tty);

		if ((c == IN_DEL) || (c == IN_BS)) {
			if (pos > 0) {
				pos--;
				tty->f.op->write(tty->f.data, OUT_DEL, sizeof(OUT_DEL) - 1);
			}
			continue;
		}

		if (c == IN_ESC) {
			continue;
		}

		if (c != IN_EOL) {
			if (pos == size) {
				tty->f.op->write(tty->f.data, OUT_BEL, sizeof(OUT_BEL) - 1);
				continue;
			}
			s[pos++] = c;
			tty->f.op->write(tty->f.data, &c, sizeof(char));
			continue;
		}

		s[pos] = '\n';
		tty->f.op->write(tty->f.data, OUT_EOL, sizeof(OUT_EOL) - 1);
		break;
	}

	return len;
}

int tty_flush(struct tty_dev * tty)
{
	return tty->f.op->flush(tty->f.data);
}

#ifndef TTY_MAX
#define TTY_MAX 4
#endif

static struct tty_dev __tty[TTY_MAX];

int tty_release(struct tty_dev * tty)
{
	tty->f.op->close(tty->f.data);
/*	tty->f.op = NULL;
	tty->f.data = NULL;
*/
	return 0;
}

struct tty_dev * tty_attach(struct file * f)
{
	struct tty_dev * tty;
	int i;

	for (i = 0; i < TTY_MAX; ++i) {
		tty = &__tty[i];
		if (tty->f.data == NULL) {
			tty->inpos = 0;
			tty->inlen = 0;
			tty->editpos = 0;
			tty->f.op = f->op;
			tty->f.data = f->data;
			return tty;
		}
	}

	return NULL;
}

struct file * tty_lowlevel(struct tty_dev * tty)
{
	return &tty->f;
}

