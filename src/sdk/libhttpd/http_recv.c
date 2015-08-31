/*
 * Copyright(c) 2004-2014 BORESTE (www.boreste.com). All Rights Reserved.
 *
 * This file is part of the libhttpd.
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
 * @file httpd.c
 * @brief HTTPD library
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include "httpd-i.h"

int http_recv(struct httpctl * ctl, void * buf, unsigned int len)
{
	int n;

	if ((n = (ctl->rcvq.cnt - ctl->rcvq.pos)) > 0) {
		uint8_t * src;
		uint8_t * dst;
		int i;

		if (n > len)
			n = len;

		/* read from internal buffer */
		src = (uint8_t *)ctl->rcvq.buf;
		dst = (uint8_t *)buf;
		src += ctl->rcvq.pos;
		for (i = 0; i < n; ++i)
			dst[i] = src[i];

		ctl->rcvq.pos += n;

		return n;
	}

	return tcp_recv(ctl->tp, buf, len);
}

int http_line_recv(struct httpctl * ctl, char * line,
		unsigned int len)
{
	struct tcp_pcb * tp = ctl->tp;
	int rem;
	int cnt;
	int pos;
	int lin;
	int c1;
	int c2;
	int n;

	cnt = ctl->rcvq.cnt;
	pos = ctl->rcvq.pos;
	lin = ctl->rcvq.lin;
	c1 = (pos) ? ctl->rcvq.buf[pos - 1] : '\0';

	/* receive SDP payload */
	for (;;) {
		/* search for end of line */
		while (pos < cnt) {
			c2 = ctl->rcvq.buf[pos++];
			if (c1 == '\r' && c2 == '\n') {
				char * dst = line;
				char * src = (char *)&ctl->rcvq.buf[lin];
				int i;

				n = pos - lin - 2;
				if (n > len)
					n = len;

				for (i = 0; i < n; ++i)
					dst[i] = src[i];

				/* move to the next line */
				lin = pos;
				ctl->rcvq.lin = lin;
				ctl->rcvq.pos = lin;
				return n;
			}
			c1 = c2;
		}

		/* */
		if (ctl->content.len == ctl->content.pos) {
			/* get the number of remaining characters, ignoring
			 * a possible CR at the end*/
			n = pos - lin - (c1 == '\r') ? 1 : 0;

			if (n != 0) {
				/* this is the last line and there is no CR+LF at the end of it */
				char * dst = line;
				char * src = (char *)&ctl->rcvq.buf[lin];
				int i;

				if (n > len)
					n = len;
				for (i = 0; i < n; ++i)
					dst[i] = src[i];
			}
			/* update our pointers */
			ctl->rcvq.pos = pos;
			ctl->rcvq.lin = lin;
			return n;
		}

		if (HTTP_RCVBUF_LEN == cnt) {
			int i;
			int j;

			if (lin == 0) {
				DCC_LOG(LOG_ERROR, "buffer overflow!");
				return -1;
			}

			/* move remaining data to the beginning of the buffer */
			n = cnt - lin;
			for (i = 0, j = lin; i < n; ++i, ++j)
				ctl->rcvq.buf[i] = ctl->rcvq.buf[j];

			cnt = n;
			pos = n;
			lin = 0;
		}

		/* free space in the input buffer */
		rem = HTTP_RCVBUF_LEN - cnt;
		/* read more data */
		if ((n = tcp_recv(tp, &ctl->rcvq.buf[cnt], rem)) <= 0) {
			tcp_close(tp);
			return n;
		}

		ctl->content.pos += n;
		cnt += n;
		ctl->rcvq.cnt = cnt;
	}

	return 0;
}


#define HASH_N 40
#define HASH_M ((HASH_N * HASH_N * HASH_N) + (HASH_N * HASH_N) + HASH_N + 1)

uint32_t http_hash(uint32_t hash, uint32_t c)
{
	return hash ^ HASH_M * (c - ' ');
}

uint32_t http_boundary_hash(char * s)
{
	uint32_t hash = 0;
	char * cp = s;
	int c;

	while ((c = *cp++) != '\0') {
		hash = http_hash(hash, c);
	}

	return hash;
}

int http_parse_multipart_form_data(struct httpctl * ctl, char * val)
{
	char * cp = val;

	if (strncmp(cp, "boundary=", 9) == 0) {
		uint32_t hash;
		int n;
		int c;

		cp += 9;

		hash = 0;
		for (n = 0; (c = cp[n]) != '\0'; ++n)
			hash = http_hash(hash, c);

		DCC_LOGSTR(LOG_TRACE, "boundary=\"%s\"", cp);
		ctl->content.bdry_hash = hash;
		ctl->content.bdry_len = n;
		DCC_LOG2(LOG_TRACE, "boundary hash=%04x len=%d", hash, n);
	}

	return 0;
}

int http_multipart_recv(struct httpctl * ctl, void * buf, unsigned int len)
{
	unsigned int cnt;
	unsigned int pos;
	unsigned int lin;
	unsigned int rem;
	int c1;
	int c2;
	int n;

	cnt = ctl->rcvq.cnt;
	pos = ctl->rcvq.pos;
	lin = ctl->rcvq.lin;
	c1 = (pos) ? ctl->rcvq.buf[pos - 1] : '\0';

	/* receive SDP payload */
	for (;;) {
		/* search for end of line */
		while (pos < cnt) {
			c2 = ctl->rcvq.buf[pos++];
			if (c1 == '\r' && c2 == '\n') {

				DCC_LOG(LOG_TRACE, "new line!");
				n = pos - lin;

				if (n == ctl->content.bdry_len + 2) {
					char * cp = (char *)&ctl->rcvq.buf[lin];
					uint32_t hash = 0;
					int i;

					DCC_LOG(LOG_TRACE, "boundary length match!");
					for (i = 0; i < ctl->content.bdry_len; ++i)
						hash = http_hash(hash, cp[i]);

					if (ctl->content.bdry_hash == hash) {
						DCC_LOG(LOG_TRACE, "boundary hash match!");
						return 0;
					}
				}

				if (n > len)
					n = len;

				memcpy(buf, &ctl->rcvq.buf[lin], n); 
				ctl->content.pos += n;

				/* move to the next line */
				lin += n;
				ctl->rcvq.lin = lin;
				ctl->rcvq.pos = lin;
				return n;
			}
			c1 = c2;
		}

		if (HTTP_RCVBUF_LEN == cnt) {
			if (lin == 0) {
				DCC_LOG(LOG_TRACE, "buffer full!");

				n = cnt - lin;
				if (n > len)
					n = len;

				memcpy(buf, &ctl->rcvq.buf[lin], n); 
				ctl->content.pos += n;

				/* move to the line and position pointers */
				lin += n;
				ctl->rcvq.lin = lin;
				ctl->rcvq.pos = lin;
				return n;
			}

			/* move remaining data to the beginning of the buffer */
			n = cnt - lin;
			memcpy(ctl->rcvq.buf, &ctl->rcvq.buf[lin], n); 

			cnt = n;
			pos = n;
			lin = 0;
		}

		/* free space in the input buffer */
		rem = HTTP_RCVBUF_LEN - cnt;
		/* read more data */
		if ((n = tcp_recv(ctl->tp, &ctl->rcvq.buf[cnt], rem)) <= 0) {
			tcp_close(ctl->tp);
			return n;
		}

		cnt += n;
		ctl->rcvq.cnt = cnt;
	}

}

