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

int http_send(struct httpctl * ctl, const void * buf, unsigned int len)
{
	return tcp_send(ctl->tp, buf, len, 0);
}
