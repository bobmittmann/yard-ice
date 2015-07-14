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
