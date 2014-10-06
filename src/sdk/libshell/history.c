/* 
 * Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
 * 
 * This file is part of the YARD-ICE.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You can receive a copy of the GNU Lesser General Public License from 
 * http://www.gnu.org/
 */

/** 
 * @file shell.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/tty.h>
#include <sys/serial.h>

#include <sys/dcclog.h>

#define __SHELL_I__
#include "shell-i.h"

#define IN_BS      '\x8'
#define IN_TN_BS     0x7F /* TELNET back space */
#define IN_EOL      '\r'
#define IN_SKIP     '\3'
#define IN_EOF      '\x1A'
#define IN_ESC      '\033'

#define MK_IN_KEY(CODE)   (0x2000 + (CODE))
#define IN_CTRL        0x4000

#define IN_CURSOR_UP    MK_IN_KEY(0)
#define IN_CURSOR_DOWN  MK_IN_KEY(1)
#define IN_CURSOR_RIGHT MK_IN_KEY(2)
#define IN_CURSOR_LEFT  MK_IN_KEY(3)
#define IN_PAGE_UP      MK_IN_KEY(5)
#define IN_PAGE_DOWN    MK_IN_KEY(6)
#define IN_INSERT       MK_IN_KEY(7)
#define IN_DELETE       MK_IN_KEY(8)
#define IN_HOME         MK_IN_KEY(9)
#define IN_END          MK_IN_KEY(10)

#define IN_CTRL_CURSOR_UP    IN_CURSOR_UP + IN_CTRL 
#define IN_CTRL_CURSOR_DOWN  IN_CURSOR_DOWN + IN_CTRL   
#define IN_CTRL_CURSOR_RIGHT IN_CURSOR_RIGHT + IN_CTRL    
#define IN_CTRL_CURSOR_LEFT  IN_CURSOR_LEFT + IN_CTRL   
#define IN_CTRL_PAGE_UP      IN_PAGE_UP + IN_CTRL   
#define IN_CTRL_PAGE_DOWN    IN_PAGE_DOWN + IN_CTRL   

#define OUT_CURSOR_LEFT     "\x8"
#define OUT_BS              "\x8 \x8"
#define OUT_SKIP            "^C"
#define OUT_BEL             "\7"

#define MODE_ESC 1
#define MODE_ESC_VAL1 2
#define MODE_ESC_VAL2 3
#define MODE_ESC_O 4

struct cmd_history * history_init(void * buf, unsigned int bufsz,
								  unsigned int line_len)
{
	struct cmd_history * ht = (struct cmd_history *)buf;

	ht->pos = 0;
	ht->head = 0;
	ht->tail = 0;
	ht->len = line_len;
	ht->max = (bufsz - sizeof(struct cmd_history)) / line_len;

	DCC_LOG2(LOG_INFO, "size=%d entries=%d", bufsz , ht->max);

	return ht;
}


char * history_prev(struct cmd_history * ht)
{
	char * cp;

	if (ht->tail == ht->head)
		return NULL;

	if (ht->pos != ht->tail) {
		if (ht->pos == 0)
			ht->pos = ht->max;
		ht->pos--;
	}

	cp = ht->buf + ht->pos * ht->len;

	DCC_LOG1(LOG_MSG, "pos=%d", ht->pos);

	return cp;
}

char * history_next(struct cmd_history * ht)
{
	char * cp;

	if (ht->tail == ht->head)
		return NULL;

	if (ht->pos != ht->head) {
		ht->pos++;
		if (ht->pos == ht->max)
			ht->pos = 0;
	}

	cp = ht->buf + ht->pos * ht->len;

	DCC_LOG1(LOG_MSG, "pos=%d", ht->pos);

	return cp;
}

char * history_head(struct cmd_history * ht)
{
	return ht->buf + ht->head * ht->len;
}

void history_add(struct cmd_history * ht, char * s)
{
	if ((s == NULL) || (*s == '\0'))
		return;

	if (ht->head != ht->tail) {
		int n;

		n = (ht->head == 0) ? ht->max : ht->head;
		n--;
	
		if (strcmp(s, ht->buf + n * ht->len) == 0) {
			/* don't repeat the last insertion,
			 just reposition the current pointer */
			ht->pos = ht->head;
			return;
		}
	}

	strcpy(ht->buf + ht->head * ht->len, s);

	ht->head++;

	if (ht->head == ht->max)
		ht->head = 0;

	if (ht->head == ht->tail) {
		/* override the last line */
		ht->tail++;
		if (ht->tail == ht->max)
			ht->tail = 0;
		DCC_LOG2(LOG_MSG, "override tail=%d head=%d", ht->tail, ht->head);
	} else {
		DCC_LOG2(LOG_MSG, "tail=%d head=%d", ht->tail, ht->head);
	}
	
	/* invalidate the head position */
	ht->buf[ht->head * ht->len] = '\0';
	ht->pos = ht->head;
}

char * history_readline(struct cmd_history * ht, FILE * f, 
						char * buf, int max)
{
	int mode;
	int pos;
	int val;
	int len;
	int ctrl;
	int c;
	int i;
	char * s;

	if (isfatty(f)) {
		f = ftty_lowlevel(f);
	}

	mode = 0;
	pos = 0;
	val = 0;
	ctrl = 0;
	len = 0;
	/* reserve space for NULL termination */
	max--;
	for (;;) {
		if ((c = fgetc(f)) == EOF) {
			DCC_LOG(LOG_INFO, "EOF");
			return NULL;
		}

		DCC_LOG1(LOG_MSG, "[%02x]", c);

		switch (mode) {
		case MODE_ESC:
			switch (c) {
			case '[':
				mode = MODE_ESC_VAL1;
				val = 0;
				ctrl = 0;
				break;
			case 'O':
				mode = MODE_ESC_O;
				break;
			default:
				DCC_LOG1(LOG_INFO, "ESC:'%c'", c);
				mode = 0;
			};
			continue;

		case MODE_ESC_VAL1:
		case MODE_ESC_VAL2:
			switch (c) {
			case '0'...'9':
				val = val * 10 + c - '0';
				continue;
			case 'A':
				/* cursor up */
				DCC_LOG2(LOG_MSG, "up %d, %d", ctrl, val);
				c = IN_CURSOR_UP + ctrl;
				break;
			case 'B':
				/* cursor down */
				DCC_LOG2(LOG_MSG, "down %d, %d", ctrl, val);
				c = IN_CURSOR_DOWN + ctrl;
				break;
			case 'C':
				/* cursor right */
				DCC_LOG2(LOG_MSG, "right %d, %d", ctrl, val);
				c = IN_CURSOR_RIGHT + ctrl;
				break;
			case 'D':
				/* cursor left */
				DCC_LOG2(LOG_MSG, "left %d, %d", ctrl, val);
				c = IN_CURSOR_LEFT + ctrl;
				break;
			case '~':
				switch (val) {
				case 1:
					DCC_LOG1(LOG_MSG, "home %d", ctrl);
					c = IN_HOME + ctrl;
					break;
				case 2:
					DCC_LOG1(LOG_MSG, "insert %d", ctrl);
					c = IN_INSERT + ctrl;
					break;
				case 3:
					/* delete */
					DCC_LOG1(LOG_MSG, "delete %d", ctrl);
					c = IN_DELETE + ctrl;
					break;
				case 4:
					/* end */
					DCC_LOG(LOG_MSG, "end");
					c = IN_END + ctrl;
					break;
				case 5:
					DCC_LOG1(LOG_MSG, "pg up %d", ctrl);
					c = IN_PAGE_UP + ctrl;
					break;
				case 6:
					DCC_LOG1(LOG_MSG, "pg down %d", ctrl);
					c = IN_PAGE_DOWN + ctrl;
					break;
				default:
					DCC_LOG1(LOG_INFO, "seq %d", val);
					mode = 0;
					continue;
				}
				break;
			case ';':
				mode = MODE_ESC_VAL2;
				ctrl = IN_CTRL;
				val = 0;
				continue;
			default:
				DCC_LOG1(LOG_MSG, "VAL:'%c'", c);
				mode = 0;
				continue;
			};
			mode = 0;
			break;

		case MODE_ESC_O:
			switch (c) {
			case 'F':
				/* end */
				DCC_LOG(LOG_MSG, "end");
				c = IN_END;
				break;
			case 'H':
				/* home */
				DCC_LOG(LOG_MSG, "home");
				c = IN_HOME;
				break;
			default:
				DCC_LOG1(LOG_INFO, "ESC O:'%c'", c);
				mode = 0;
				continue;
			}
			mode = 0;
			break;
		} 

		switch (c) {
		case IN_ESC:
			DCC_LOG(LOG_MSG, "ESC");
			mode = MODE_ESC;
			continue;

		case IN_CURSOR_UP:
			if (ht->pos == ht->head) {
				buf[len] = '\0';
				/* save current line on history's head */
				strcpy(ht->buf + ht->pos * ht->len, buf);
			}

			if ((s = history_prev(ht)) == NULL)
				continue;
			goto set_buf;

		case IN_CURSOR_DOWN:
			if ((s = history_next(ht)) == NULL)
				continue;
			goto set_buf;

		case IN_CURSOR_RIGHT:
			if (pos < len)
				fputc(buf[pos++], f);
			else
				fputs(OUT_BEL, f);
			continue;

		case IN_CURSOR_LEFT:
			if (pos > 0) {
				fputs(OUT_CURSOR_LEFT, f);
				pos--;
			} else {
				fputs(OUT_BEL, f);
			}
			continue;

		case IN_CTRL_CURSOR_RIGHT:
			while (pos < len) {
				fputc(buf[pos++], f);
				if ((buf[pos - 1] != ' ') && (buf[pos] == ' '))
					break;
			} 
			continue;

		case IN_CTRL_CURSOR_LEFT:
			if (pos > 0) {
				do {
					fputs(OUT_CURSOR_LEFT, f);
					pos--;
					if ((buf[pos - 1] == ' ') && (buf[pos] != ' '))
						break;
				} while (pos > 0);
			}
			continue;

		case IN_PAGE_UP:
			continue;

		case IN_PAGE_DOWN:
			continue;

		case IN_INSERT:
			continue;

		case IN_HOME:
			while (pos > 0) {
				fputs(OUT_CURSOR_LEFT, f);
				pos--;
			} 
			continue;

		case IN_END:
			while (pos < len) {
				fputc(buf[pos++], f);
			} 
			continue;

		case IN_TN_BS:     
			DCC_LOG(LOG_MSG, "IN_TN_BS");
		case IN_BS:
			if (pos == 0) {
				fputs(OUT_BEL, f);
				continue;
			}
			if (len == pos) {
				pos--;
				len--;
				fputs(OUT_BS, f);
				continue;
			}

			fputs(OUT_CURSOR_LEFT, f);
			pos--;
			/* fall back */

		case IN_DELETE:

			if (len == pos) {
				continue;
			}
			len--;

			for (i = pos; i < len; i++) {
				buf[i] = buf[i + 1];
			}
			buf[len] = '\0';
			fputs(&buf[pos], f);
			fputc(' ', f);
			for (i = len + 1; i > pos; i--)
				fputs(OUT_CURSOR_LEFT, f);
			continue;

		case IN_EOL:
			buf[len] = '\0';
			fputs("\r\n", f);
			return buf;

		case IN_SKIP:
			fputs(OUT_SKIP, f);
			return NULL;
		}

		if (len == max) {
			fputs(OUT_BEL, f);
			continue;
		}

		if (len == pos) {
			fputc(c, f);
			buf[pos] = c;
			pos++;
			len++;
			continue;
		}

		for (i = len; i > pos; i--) {
			buf[i] = buf[i - 1];
		}

		len++;
		buf[pos] = c;
		buf[len] = '\0';

		fputs(&buf[pos], f);
		pos++;

		for (i = len; i > pos; i--)
			fputs(OUT_CURSOR_LEFT, f);

		continue;

set_buf:
		for (i = pos; i > 0; i--)
			fputs(OUT_CURSOR_LEFT, f);

		strcpy(buf, s);
		fputs(s, f);

		pos = strlen(s);
		if (pos < len) {
			for (i = pos; i < len; i++)
				fputc(' ', f);
			for (i = pos; i < len; i++)
				fputs(OUT_CURSOR_LEFT, f);
		}
		len = pos;

	}
}

