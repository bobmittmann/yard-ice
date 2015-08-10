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

#include <stdarg.h>
#define __THINKOS_DMON__
#include <thinkos_dmon.h>
#include <thinkos.h>
#include <sys/param.h>
#include <sys/dcclog.h>

/* ATTENTION: check FL_CHAR first, not FL_LONG. The last is set
   simultaneously.	*/
#define FL_STAR	    0x01	/* '*': skip assignment		*/
#define FL_WIDTH    0x02	/* width is present		*/
#define FL_LONG	    0x04	/* 'long' type modifier		*/
#define FL_CHAR	    0x08	/* 'char' type modifier		*/
#define FL_OCT	    0x10	/* octal number			*/
#define FL_DEC	    0x20	/* decimal number		*/
#define FL_HEX	    0x40	/* hexidecimal number		*/
#define FL_MINUS    0x80	/* minus flag (field or value)	*/

# define GETBYTE(flag, mask, pnt)	({	\
									 unsigned char __c;				\
									 __c = ((flag) & (mask))			\
									 ? pgm_read_byte(pnt) : *pnt;		\
									 pnt++;					\
									 __c;					\
									 })

#define FILE_BUF_LEN 64

int dmgets(char * s, int size, struct dmon_comm * comm);

typedef struct {
	struct dmon_comm * comm;
	int pos;
	int len;
	char buf[FILE_BUF_LEN];
} FILE;

int getc(FILE * f)
{
	if (f->pos < f->len)
		return f->buf[f->pos++];
	return -1;
}

int ungetc(int c, FILE * f)
{
	if (f->pos > 0)
		return f->buf[--f->pos];
	return -1;
}

static void putval(void *addr, long val, unsigned char flags)
{
	if (!(flags & FL_STAR)) {
		if (flags & FL_CHAR)
			*(char *)addr = val;
		else if (flags & FL_LONG)
			*(long *)addr = val;
		else
			*(int *)addr = val;
	}
}

static unsigned long mulacc(unsigned long val, unsigned char flags, 
							unsigned char c)
{
	unsigned char cnt;

	if (flags & FL_OCT) {
		cnt = 3;
	} else if (flags & FL_HEX) {
		cnt = 4;
	} else {
		val += (val << 2);
		cnt = 1;
	}

	do { val <<= 1; } while (--cnt);
	return val + c;
}

static unsigned char conv_int(FILE *stream, unsigned int width, 
							  void *addr, unsigned char flags)
{
	unsigned long val;
	int i;

	i = getc(stream);			/* after ungetc()	*/

	switch ((unsigned char)i) {
	case '-':
		flags |= FL_MINUS;
		/* FALLTHROUGH */
	case '+':
		if (!--width || (i = getc(stream)) < 0)
			goto err;
	}

	val = 0;
	flags &= ~FL_WIDTH;

	if (!(flags & (FL_DEC | FL_OCT)) && (unsigned char)i == '0') {
		if (!--width || (i = getc(stream)) < 0)
			goto putval;
		flags |= FL_WIDTH;
		if ((unsigned char)(i) == 'x' || (unsigned char)(i) == 'X') {
			flags |= FL_HEX;
			if (!--width || (i = getc(stream)) < 0)
				goto putval;
		} else {
			if (!(flags & FL_HEX))
				flags |= FL_OCT;
		}
	}

	do {
		unsigned char c = i;
		c -= '0';
		if (c > 7) {
			if (flags & FL_OCT) goto unget;
			if (c > 9) {
				if (!(flags & FL_HEX)) goto unget;
				c &= ~('A' ^ 'a');
				c += '0' - 'A';
				if (c > 5) {
unget:
					ungetc(i, stream);
					break;
				}
				c += 10;
			}
		}
		val = mulacc(val, flags, c);
		flags |= FL_WIDTH;
		if (!--width) goto putval;
	} while ((i = getc(stream)) >= 0);
	if (!(flags & FL_WIDTH))
		goto err;

putval:
	if (flags & FL_MINUS) val = -val;
	putval(addr, val, flags);
	return 1;

err:
	return 0;
}

static int skip_spaces(FILE *stream)
{
	int i;
	do {
		if ((i = getc(stream)) < 0)
			return i;
	} while (i == ' ' || i == '\t');
	ungetc(i, stream);
	return i;
}

int dmscanf(struct dmon_comm * comm, const char *fmt, ... )
{
	FILE buf;
	FILE * stream = &buf;
	unsigned char nconvs;
	unsigned char c;
	unsigned int width;
	unsigned char flags;
	void * addr;
	va_list ap;
	int i;

	va_start(ap, fmt);

	nconvs = 0;

	stream->comm = comm;	
	stream->pos = 0;
	stream->len = dmgets(stream->buf, FILE_BUF_LEN, comm);

	while ((c = *fmt++) != '\0') {

		DCC_LOG1(LOG_MSG, "c='%c'", c);

		if (c == ' ' || c == '\t') {
			skip_spaces(stream);

		} else if (c != '%' || (c = *fmt++) == '%') {
			/* Ordinary character.	*/
			if ((i = getc(stream)) < 0)
				goto eof;
			if ((unsigned char)i != c) {
				ungetc(i, stream);
				goto eof;
//				break;
			}

		} else {
			flags = 0;

			if (c == '*') {
				flags = FL_STAR;
				c = *fmt++;
			}

			width = 0;
			while ((c -= '0') < 10) {
				flags |= FL_WIDTH;
				width = mulacc(width, FL_DEC, c);
				c = *fmt++;
			}
			c += '0';
			if (flags & FL_WIDTH) {
				/* C99 says that width must be greater than zero.
				   To simplify program do treat 0 as error in format.	*/
				if (!width) 
					break;
			} else {
				width = ~0;
			}

			/* ATTENTION: with FL_CHAR the FL_LONG is set also.	*/
			switch (c) {
			case 'h':
				if ((c = *fmt++) != 'h')
					break;
				flags |= FL_CHAR;
				/* FALLTHROUGH */
			case 'l':
				flags |= FL_LONG;
				c = *fmt++;
			}

			if (c == 'n') {
				addr = (flags & FL_STAR) ? 0 : va_arg(ap, void *);
				putval(addr, (unsigned)(stream->len), flags);
				continue;
			}

			if (c == 'c') {
				addr = (flags & FL_STAR) ? 0 : va_arg(ap, void *);
				if (!(flags & FL_WIDTH)) width = 1;
				do {
					if ((i = getc(stream)) < 0)
						goto eof;
					if (addr) *(char *)addr++ = i;
				} while (--width);
				c = 1;			/* no matter with smart GCC	*/

			} else {

				if (skip_spaces(stream) < 0)
					goto eof;

				switch (c) {

				case 's':
					addr = (flags & FL_STAR) ? 0 : va_arg(ap, void *);
					/* Now we have 1 nospace symbol.	*/
					do {
						if ((i = getc(stream)) < 0)
							break;
						if (i == ' ' || i == '\t') {
							ungetc(i, stream);
							break;
						}
						if (addr) *(char *)addr++ = i;
					} while (--width);
					if (addr) *(char *)addr = 0;
					c = 1;		/* no matter with smart GCC	*/
					break;

				case 'd':
				case 'u':
					addr = (flags & FL_STAR) ? 0 : va_arg(ap, void *);
					flags |= FL_DEC;
					goto conv_int;

				case 'o':
					flags |= FL_OCT;
					/* FALLTHROUGH */
				case 'i':
					addr = (flags & FL_STAR) ? 0 : va_arg(ap, void *);
					goto conv_int;

				default:			/* p,x,X	*/
					addr = (flags & FL_STAR) ? 0 : va_arg(ap, void *);
					flags |= FL_HEX;
conv_int:
					c = conv_int(stream, width, addr, flags);
				}
			} /* else */

			if (!c) {
				if (stream->pos == stream->len)
					goto eof;
				break;
			}
			if (!(flags & FL_STAR)) nconvs += 1;
		} /* else */
	} /* while */

	DCC_LOG1(LOG_TRACE, "nconvs=%d", nconvs);

	va_end(ap);

	return nconvs;

eof:
	va_end(ap);

	return nconvs ? nconvs : -1;
}

