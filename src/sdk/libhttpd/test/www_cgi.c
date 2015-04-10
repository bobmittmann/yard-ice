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
 * @file www_cgi.c
 * @brief YARD-ICE application main
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <tcpip/httpd.h>
#include <thinkos.h>
#include "www.h"

/*---------------------------------------------------------------------------
  Initial page (index.html)
  ---------------------------------------------------------------------------*/

const char cgi_index_html[] = DOCTYPE_HTML "<head>\r\n"
	"<title>ThinkOS HTTP Server Demo</title>\r\n" 
	META_HTTP META_COPY LINK_ICON LINK_CSS LINK_ZEPTO 
	"</head>\r\n<body>\r\n"
	"<h1>ThinkOS Web Server Demo</h1>\r\n"
	"<p>Welcome to the <b>ThinkOS</b> web server demo initial page.</p>\r\n"
	"<div id=\"results\"></div>"
	"<button id=\"btn\">Get Names</button>"
	"<script>\r\n"
	"$(function () {"
	" $(document).on('click', '#btn',function (event) {"
	"  $('#results').empty();"
	"  $.getJSON('http://www.filltext.com/?rows=5&fname={firstName}&callback=?',"
	"  function (data) {"
	"   $.each(data, function (index,item) {"
	"   $('#results').append(item.fname + '<hr />')"
	"   });"
	"  });"
	" });"
	"});"
	"</script>\r\n"
	HTML_FOOTER;


/*---------------------------------------------------------------------------
  root directory content
  ---------------------------------------------------------------------------*/

struct httpdobj www_cgi[] = {
	{ .oid = "index.html", .typ = OBJ_STATIC_HTML, .lvl = 255, 
		.len = sizeof(cgi_index_html) - 1, .ptr = cgi_index_html },
	{ .oid = NULL, .typ = 0, .lvl = 0, .len = 0, .ptr = NULL }
};

