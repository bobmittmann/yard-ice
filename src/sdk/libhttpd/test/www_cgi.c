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
#include <sys/dcclog.h>
#include "www.h"

/*---------------------------------------------------------------------------
  Initial page (index.html)
  ---------------------------------------------------------------------------*/

//	"  $.getJSON('get_data.cgi?rows=5&fname={firstName}&callback=?', "
//	"   $.each(data, function (index,item) {"
//	"   $('#results').append(item.fname + '<hr />')"
//	"  });"
//	"$(function () {"
//	" $(document).on('click', '#btn',function (event) {"
//	"  $('#results').empty();"
//	"  $.getJSON('get_data.cgi?rows=5', "
//	"  function (data) { console.log(data) });"
//	" });"
//	"});"

const char cgi_index_html[] = DOCTYPE_HTML "<head>\r\n"
	"<title>ThinkOS HTTP Server Demo</title>\r\n" 
	META_HTTP META_COPY LINK_ICON LINK_CSS LINK_ZEPTO 
	"</head>\r\n<body>\r\n"
	"<h1>ThinkOS Web Server Demo</h1>\r\n"
	"<p>Welcome to the <b>ThinkOS</b> web server demo initial page.</p>\r\n"
	"<div id=\"results\"></div>"
	"<input type=\"text\" id=\"cnt\" value=\"10\" />\r\n"
	"<button id=\"btn\">Get Names</button>"
	"<script>\r\n"
	"function get_names() {\r\n"
	" $.ajax({\r\n"
	"  type: 'GET', \r\n"
	"  url: 'get_data.cgi', \r\n"
	"  data: { rows: $('#cnt').val() }, \r\n"
	"  dataType: 'json', \r\n"
	"  timeout: 300, \r\n"
	"  context: $('body'), \r\n"
	"  success: function(data) {\r\n"
	"   $('#results').empty();\r\n"
	"   $.each(data.list, function (index, item) {\r\n"
	"    $('#results').append(item.fname + '<hr />')\r\n"
	"   });\r\n"
	"  }, \r\n"
	"  error: function(xhr, type){ alert('Ajax error!') }\r\n"
	" });\r\n"
	"};\r\n"
	"$(function () {\r\n"
	" $(document).on('click', '#btn', get_names);\r\n"
	"});\r\n"
	"</script>\r\n"
	HTML_FOOTER;

#define S_MAX 256 /* HTML formatting string buffer lenght */

int get_data_cgi(struct httpctl * ctl)
{
	char s[S_MAX];
	int rows;
	int n;
	int i;

	rows = atoi(http_query_lookup(ctl, "rows")); 
	DCC_LOG1(LOG_TRACE, "rows=%d", rows);

	httpd_200(ctl->tp, APPLICATION_JASON);

	n = snprintf(s, S_MAX, "{\"list\": [");
	tcp_send(ctl->tp, s, n, 0);
	for (i = 0; i < rows; ++i) {
		if (i == 0)
			n = snprintf(s, S_MAX, "{\"fname\":\"Row %d\"}", i);
		else
			n = snprintf(s, S_MAX, ",{\"fname\":\"Row %d\"}", i);
		tcp_send(ctl->tp, s, n, 0);
	}
	n = snprintf(s, S_MAX, "]}\r\n");
	tcp_send(ctl->tp, s, n, 0);

	/* close thisp page connection */
	tcp_close(ctl->tp);

	return 0;
}

/*---------------------------------------------------------------------------
  root directory content
  ---------------------------------------------------------------------------*/

struct httpdobj www_cgi[] = {
	{ .oid = "index.html", .typ = OBJ_STATIC_HTML, .lvl = 255, 
		.len = sizeof(cgi_index_html) - 1, .ptr = cgi_index_html },
	{ .oid = "get_data.cgi", .typ = OBJ_CODE_CGI, .lvl = 100, 
		.len = 0, .ptr = get_data_cgi },
	{ .oid = NULL, .typ = 0, .lvl = 0, .len = 0, .ptr = NULL }
};

