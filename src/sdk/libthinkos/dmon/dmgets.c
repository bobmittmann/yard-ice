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

#define __THINKOS_DMON__
#include <thinkos_dmon.h>
#include <sys/dcclog.h>

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
#define OUT_BEL     '\7'

int dmputc(int c, struct dmon_comm * comm)
{
	char buf[1] = { c };
	return dmon_comm_send(comm, buf, sizeof(char));
}

int dmputs(char * s, struct dmon_comm * comm)
{
	int n = 0;

	while (s[n] != '\0')
		n++;
	return dmon_comm_send(comm, s, n);
}

int dmgets(char * s, int size, struct dmon_comm * comm)
{
	char buf[1];
	int c;
	int pos;

	/* left room to '\0' */
	size--;
	pos = 0;

	for (;;) {
		if (dmon_comm_recv(comm, buf, sizeof(char)) <= 0)
			return -1;

		c = buf[0];

		DCC_LOG1(LOG_MSG, "c=%02x", c);

		if (c == IN_EOL) {
			dmputs(OUT_EOL, comm);
			break;
		} else if (c == IN_SKIP) {
			dmputs(OUT_SKIP, comm);
			return -1;
		} else if (c == IN_BS || c == IN_DEL) {
			if (pos == 0) {
				dmputc(OUT_BEL, comm);
			} else {
				pos--;
				dmputs(OUT_DEL, comm);
			}
		} else if (c == IN_ESC) {
		} else if (pos == size) {
			dmputc(OUT_BEL, comm);
			continue;
		}

		s[pos++] = c;
		dmputc(c, comm);
	}

	DCC_LOG1(LOG_TRACE, "pos=%d", pos);
	s[pos] = '\0';

	return pos;
}

