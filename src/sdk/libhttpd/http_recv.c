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
#include <sys/param.h>
#include <arch/cortex-m3.h>
#include <trace.h>

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
#define HASH_M ((HASH_N * HASH_N * HASH_N * HASH_N) + \
				(HASH_N * HASH_N * HASH_N) + (HASH_N * HASH_N) + HASH_N + 1)

static inline uint32_t http_hash(uint32_t hash, uint32_t c)
{
	return hash ^ (HASH_M * c);
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

int http_multipart_boundary_lookup(struct httpctl * ctl)
{
	uint8_t * queue = (uint8_t *)ctl->rcvq.buf;
	int cnt;
	int pos;
	int rem;
	uint32_t pat;
	uint32_t hash;
	int n;
	int i;

	cnt = ctl->rcvq.cnt;
	pos = ctl->rcvq.pos;
	pat = 0x000d0a00;

	/* receive payload */
	for (;;) {
		/* search for the pattern */
		while (pos < cnt) {
			pat |= queue[pos++];
			if (pat == 0x0d0a2d2d)
				goto pattern_match;
			pat <<= 8;
		}

		if (HTTP_RCVBUF_LEN == cnt) {
			DCC_LOG(LOG_TRACE, "buffer full!");
#ifdef DEBUG
			if (cpy_pos != (pos - 3)) {
				DCC_LOG(LOG_ERROR, "transfer queue integrity failed!");
				return 0;
			}
#endif
			/* move remaining data to the beginning of the buffer */
			n = 3;
			memcpy(queue, &queue[pos - n], n);
			cnt = n;
			pos = n;
		}

		/* read more */
		rem = HTTP_RCVBUF_LEN - cnt;
		/* read more data */
		if ((n = tcp_recv(ctl->tp, &queue[cnt], rem)) <= 0) {
			tcp_close(ctl->tp);
			return n;
		}
		cnt += n;
		ctl->rcvq.cnt = cnt;
	}

pattern_match:
	/* move remaining data to the beginning of the buffer */
	n = cnt - pos;
	memcpy(queue, &queue[pos], n);
	/* the data left in the buffer is the new total count */
	cnt = n;
	/* set the search position  */
	pos = 0;

	/* calculate the boundary hash but first make sure
	 we have enough data in the processing buffer (boundary marker length + CR LF) */
	while (cnt < ctl->content.bdry_len + 2) {
		/* receive more data from the network */
		rem = HTTP_RCVBUF_LEN - cnt;
		if ((n = tcp_recv(ctl->tp, &queue[cnt], rem)) <= 0) {
			tcp_close(ctl->tp);
			return n;
		}
		cnt += n;
		ctl->rcvq.cnt = cnt;
	}

	hash = 0;
	for (i = 0; i < ctl->content.bdry_len; ++i)
		hash = http_hash(hash, queue[i]);

	if (ctl->content.bdry_hash != hash) {
		DCC_LOG(LOG_TRACE, "boundary hash mismatch");
		return -1;
	}

	/* skip boundary marker and CR+LF */
	pos += ctl->content.bdry_len + 2;
	ctl->rcvq.pat = 0;
	ctl->rcvq.pos = pos;
	ctl->rcvq.lin = pos;

	return 0;
}

int http_multipart_recv(struct httpctl * ctl, void * buf, unsigned int len)
{
	uint8_t * queue = (uint8_t *)ctl->rcvq.buf;
	uint8_t * cpy_buf;
	int cpy_cnt;
	uint8_t * cp;
	int mrk;
	int cnt;
	int pos;
	int rem;
	uint32_t pat;
	uint32_t hash;
	int n;
	int i;

	DCC_LOG(LOG_TRACE, "1. ...................................");
	DCC_LOG1(LOG_TRACE, "sp=%08x", cm3_sp_get());

	cnt = ctl->rcvq.cnt;
	pos = ctl->rcvq.pos;
	pat = ctl->rcvq.pat;
	mrk = ctl->rcvq.lin;

	cpy_buf = (uint8_t *)buf;
	cpy_cnt = 0;

	if (pat == 0x0d0a2d2d) {
pattern_match:
		DCC_LOG(LOG_TRACE, "pattern match!");
		if (pos > 4) {
			int n;
			/* copy all data up to the match pattern to the receiving buffer */
			n = pos - mrk - 4;
			n = MIN(n, len);
			memcpy(&cpy_buf[cpy_cnt], &queue[mrk], n);
			cpy_cnt += n;
			if (cpy_cnt == len) {
				/* no more space left in the receiving buffer,
				   update the position of the transfer */
				mrk += cpy_cnt;
				/* update the transfer pointer */
				ctl->rcvq.lin = mrk;
				return cpy_cnt;
			}
			/* move remaining data to the beginning of the buffer */
			n = cnt - pos + 4;
			memcpy(queue, &queue[pos - 4], n); 
			/* the data left in the buffer is the new total count */
			cnt = n;
			/* set the search position to just after the 4 bytes 
			   pattern in the buffer */
			pos = 4;
			/* reset the copy pointer */
			mrk = 0;
			/* write back the receive queue state */
			ctl->rcvq.pat = pat;
			ctl->rcvq.cnt = cnt;
			ctl->rcvq.pos = pos;
			ctl->rcvq.lin = mrk;
			return cpy_cnt;
		}
#ifdef DEBUG
		if (pos != 4) {
			DCC_LOG(LOG_ERROR, "processing poistion should be 4!");
			return 0;
		}
#endif

		/* calculate the boundary hash but first make sure
		 we have enough data in the processing buffer */
		while (cnt < (ctl->content.bdry_len + 4)) {
			/* receive more data from the network */
			rem = HTTP_RCVBUF_LEN - cnt;
			if ((n = tcp_recv(ctl->tp, &queue[cnt], rem)) <= 0) {
				tcp_close(ctl->tp);
				return n;
			}
			cnt += n;
			ctl->rcvq.cnt = cnt;
		}

		hash = 0;
		cp = &queue[4];
		for (i = 0; i < ctl->content.bdry_len; ++i)
			hash = http_hash(hash, cp[i]);

		if (ctl->content.bdry_hash == hash) {
			DCC_LOG(LOG_TRACE, "boundary hash match!");
			return 0;
		}

		DCC_LOG(LOG_TRACE, "not match!");
		/* reset the pattern match buffer */
		pat = 0;
	}

	/* receive payload */
	for (;;) {
//		DCC_LOG2(LOG_TRACE, "pos=%d cnt=%d", pos, cnt);

		/* search for the pattern */
		while (pos < cnt) {
			pat |= queue[pos++];
			if (pat == 0x0d0a2d2d)
				goto pattern_match;
			pat <<= 8;
		}

		if (mrk < (pos - 3)) {
			int n;
			/* copy data to receving buffer but leave 3 bytes for a possible 
			   match on next round. */
			n = pos - mrk - 3;
			n = MIN(n, len);
			memcpy(&cpy_buf[cpy_cnt], &queue[mrk], n);
			mrk += n;
			cpy_cnt += n;
			if (cpy_cnt == len) {
				/* write back the receive queue state */
				ctl->rcvq.pat = pat;
				ctl->rcvq.pos = pos;
				ctl->rcvq.lin = mrk;
				return cpy_cnt;
			}
		}

		if (HTTP_RCVBUF_LEN == cnt) {
			DCC_LOG(LOG_TRACE, "buffer full!");
#ifdef DEBUG
			if (mrk != (pos - 3)) {
				DCC_LOG(LOG_ERROR, "transfer queue integrity failed!");
				return 0;
			}
#endif
			/* move remaining data to the beginning of the buffer */
			n = 3;
			memcpy(queue, &queue[pos - n], n); 
			cnt = n;
			pos = n;
			mrk = 0;
		}

		/* read more */
		rem = HTTP_RCVBUF_LEN - cnt;
		/* read more data */
		if ((n = tcp_recv(ctl->tp, &queue[cnt], rem)) <= 0) {
			tcp_close(ctl->tp);
			return n;
		}
		cnt += n;
		ctl->rcvq.cnt = cnt;
	}
}

