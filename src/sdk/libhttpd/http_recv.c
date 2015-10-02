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
	int head = ctl->rcvq.head;
	int tail = ctl->rcvq.tail;
	int n;

	if ((n = (head - tail)) > 0) {
		uint8_t * queue = (uint8_t *)ctl->rcvq.buf;

		if (n > len)
			n = len;
		/* read from internal buffer */
		memcpy(buf, &queue[tail], n);
		tail += n;
		if (tail == head) {
			tail = 0;
			ctl->rcvq.head = tail;
		}
		ctl->rcvq.tail = tail;
		return n;
	}

	return tcp_recv(ctl->tp, buf, len);
}

int http_content_recv(struct httpctl * ctl, void * buf, unsigned int len)
{
	int head = ctl->rcvq.head;
	int tail = ctl->rcvq.tail;
	int max;
	int n;

	if ((max = ctl->content.len - ctl->content.pos) == 0)
		return 0;

	if (max > len)
		max = len;

	if ((n = (head - tail)) > 0) {
		uint8_t * queue = (uint8_t *)ctl->rcvq.buf;

		if (n > max)
			n = max;
		/* read from internal buffer */
		memcpy(buf, &queue[tail], n);
		tail += n;
		if (tail == head) {
			tail = 0;
			ctl->rcvq.head = tail;
		}
		ctl->rcvq.tail = tail;
		ctl->content.pos += n;
		return n;
	}

	n = tcp_recv(ctl->tp, buf, max);
	if (n > 0)
		ctl->content.pos += n;

	return n;
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

	cnt = ctl->rcvq.head;
	pos = ctl->rcvq.pos;
	lin = ctl->rcvq.tail;
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
				ctl->rcvq.tail = lin;
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
			ctl->rcvq.tail = lin;
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
		ctl->rcvq.head = cnt;
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

	cnt = ctl->rcvq.head;
	pos = ctl->rcvq.pos;
	pat = 0x00000d0a;

	/* receive payload */
	for (;;) {
		/* search for the pattern */
		while (pos < cnt) {
			pat <<= 8;
			pat |= queue[pos++];
			if (pat == 0x0d0a2d2d)
				goto pattern_match;
		}

		if (HTTP_RCVBUF_LEN == cnt) {
			DCC_LOG(LOG_TRACE, "buffer full!");
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
	 we have enough data in the processing buffer
	 (boundary marker length + CR+LF) */
	while (cnt < ctl->content.bdry_len + 2) {
		/* receive more data from the network */
		rem = HTTP_RCVBUF_LEN - cnt;
		if ((n = tcp_recv(ctl->tp, &queue[cnt], rem)) <= 0) {
			tcp_close(ctl->tp);
			return n;
		}
		cnt += n;
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

	/* write back the queue state */
	ctl->rcvq.pat = 0;
	ctl->rcvq.pos = pos;
	ctl->rcvq.tail = pos;
	ctl->rcvq.head = cnt;

	return 0;
}

int http_recv_queue_shift(struct httpctl * ctl)
{
	uint8_t * queue = (uint8_t *)ctl->rcvq.buf;
	int head = ctl->rcvq.head;
	int tail = ctl->rcvq.tail;
	int n;

	/* move any data in the queue to the beginning of the buffer */
	n = head - tail;
	memcpy(queue, &queue[tail], n);
	ctl->rcvq.tail = 0;
	ctl->rcvq.pos = 0;
	ctl->rcvq.head = n;

	if (n < HTTP_RCVBUF_LEN)
		queue[n] = '\0';

	return n;
}

int http_multipart_recv(struct httpctl * ctl, void * buf, unsigned int len)
{
	uint8_t * queue = (uint8_t *)ctl->rcvq.buf;
	uint8_t * cpy_buf;
	int cpy_cnt;
	int tail;
	int head;
	int pos;
	uint32_t pat;
	uint32_t hash;

	head = ctl->rcvq.head;
	tail = ctl->rcvq.tail;
	pos = ctl->rcvq.pos;
	pat = ctl->rcvq.pat;

#ifdef DEBUG
	if (head > HTTP_RCVBUF_LEN) {
		DCC_LOG(LOG_ERROR, "(head > HTTP_RCVBUF_LEN)!");
		return 0;
	}
	if (pos > head) {
		DCC_LOG(LOG_ERROR, "(pos > head)!");
		return 0;
	}
	if (tail > pos) {
		DCC_LOG(LOG_ERROR, "(pos > head)!");
		return 0;
	}
	if (ctl->content.bdry_hash == 0) {
		DCC_LOG(LOG_ERROR, "(ctl->content.bdry_hash == 0)!");
		return 0;
	}
#endif

	cpy_buf = (uint8_t *)buf;
	cpy_cnt = 0;

	if (pat == 0x0d0a2d2d) {
		int i;
		uint8_t * cp;

pattern_match:
		DCC_LOG3(LOG_INFO, "pattern match, tail=%d pos=%d head=%d", 
				 tail, pos, head);
		if (pos > 4) {
			int n;
			/* copy all data up to the match pattern to the receiving buffer */
			n = pos - tail - 4;
			n = MIN(n, len - cpy_cnt);
			memcpy(&cpy_buf[cpy_cnt], &queue[tail], n);
			tail += n;
			cpy_cnt += n;
			if (cpy_cnt == len) {
				/* no more space left in the receiving buffer,
				   update the position of the transfer */
			
				/* write back the receive queue state */
				ctl->rcvq.pat = pat;
				ctl->rcvq.pos = pos;
				ctl->rcvq.tail = tail;
				return cpy_cnt;
			}
			/* move remaining data to the beginning of the buffer */
			n = head - pos + 4;
			memcpy(queue, &queue[pos - 4], n); 
			/* the data left in the buffer is the new total count */
			head = n;
			/* set the search position to just after the 4 bytes 
			   pattern in the buffer */
			pos = 4;
			/* reset the copy pointer */
			tail = 0;
			/* write back the receive queue state */
			ctl->rcvq.pat = pat;
			ctl->rcvq.head = head;
			ctl->rcvq.pos = pos;
			ctl->rcvq.tail = tail;
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
		while (head < (ctl->content.bdry_len + 4)) {
			int rem;
			int n;
			/* receive more data from the network */
			rem = HTTP_RCVBUF_LEN - head;
			if ((n = tcp_recv(ctl->tp, &queue[head], rem)) <= 0) {
				tcp_close(ctl->tp);
				return n;
			}
			head += n;
			ctl->rcvq.head = head;
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
		int rem;
		int n;

		DCC_LOG4(LOG_INFO, "tail=%d pos=%d head=%d cpy_cnt=%d", 
				 tail, pos, head, cpy_cnt);

		/* search for the pattern */
		while (pos < head) {
			pat <<= 8;
			pat |= queue[pos++];
			if (pat == 0x0d0a2d2d)
				goto pattern_match;
		}

		if (tail < (pos - 3)) {
			/* copy data to receiving buffer but leave 3 bytes for a possible
			   match on next round. */
			n = pos - tail - 3;
			n = MIN(n, len - cpy_cnt);
			memcpy(&cpy_buf[cpy_cnt], &queue[tail], n);
			tail += n;
			cpy_cnt += n;
			if (cpy_cnt == len) {
				/* write back the receive queue state */
				ctl->rcvq.pat = pat;
				ctl->rcvq.pos = pos;
				ctl->rcvq.tail = tail;
				return cpy_cnt;
			}
		}

		if (HTTP_RCVBUF_LEN == head) {
			DCC_LOG(LOG_INFO, "buffer full!");
#ifdef DEBUG
			if (tail != (pos - 3)) {
				DCC_LOG(LOG_ERROR, "transfer queue integrity failed!");
				return 0;
			}
#endif
			/* move remaining data to the beginning of the buffer */
			queue[0] = queue[pos - 3]; 
			queue[1] = queue[pos - 2]; 
			queue[2] = queue[pos - 1]; 
			head = 3;
			pos = 3;
			tail = 0;
		}

		/* read more */
		rem = HTTP_RCVBUF_LEN - head;
		/* read more data */
		if ((n = tcp_recv(ctl->tp, &queue[head], rem)) <= 0) {
			tcp_close(ctl->tp);
			return n;
		}
		head += n;
		ctl->rcvq.head = head;
	}

	return 0;
}

