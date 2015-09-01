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
#include <netinet/in.h>
#include <thinkos.h>
#include <sys/dcclog.h>
#include <sys/file.h>
#include <sys/null.h>
#include <sys/shell.h>
#include "www.h"

static const char footer_html[] = HTML_FOOTER;

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

	httpd_200(ctl->tp, APPLICATION_JSON);

	n = snprintf(s, S_MAX, "{\"list\": [");
	http_send(ctl, s, n);
	for (i = 0; i < rows; ++i) {
		if (i == 0)
			n = snprintf(s, S_MAX, "{\"fname\":\"Row %d\"}", i);
		else
			n = snprintf(s, S_MAX, ",{\"fname\":\"Row %d\"}", i);
		http_send(ctl, s, n);
	}
	n = snprintf(s, S_MAX, "]}\r\n");
	http_send(ctl, s, n);

	return 0;
}
const char test1_hdr_html[] = DOCTYPE_HTML "<head>\r\n"
	"<title>ThinkOS Dynamic Page 1</title>\r\n" 
	META_HTTP META_COPY LINK_ICON LINK_CSS "</head>\r\n<body>\r\n"
	"<h1>ThinkOS Dynamic Page 1</h1>\r\n";

/*---------------------------------------------------------------------------
  Dynamic page 1
  ---------------------------------------------------------------------------*/

int test1_cgi(struct httpctl * ctl)
{
	static unsigned int cnt = 0;
	char s[S_MAX];
	int n;

	httpd_200(ctl->tp, TEXT_HTML);
	n = snprintf(s, S_MAX, "<p>This page was accessed %d times!</p>", ++cnt);
	tcp_send(ctl->tp, test1_hdr_html, sizeof(test1_hdr_html) - 1, 0);
	tcp_send(ctl->tp, s, n, 0);
	return tcp_send(ctl->tp, footer_html, sizeof(footer_html) - 1, 0);
}

const char test2_hdr_html[] = DOCTYPE_HTML "<head>\r\n"
	"<title>ThinkOS Dynamic Page 2</title>\r\n" 
	META_HTTP META_COPY LINK_ICON LINK_CSS "</head>\r\n<body>\r\n"
	"<h1>ThinkOS Dynamic Page 2</h1>\r\n";

/*---------------------------------------------------------------------------
  Dynamic page 2
  ---------------------------------------------------------------------------*/

int test2_cgi(struct httpctl * ctl)
{
	static unsigned int cnt = 0;
	char s[S_MAX];
	int n;

	httpd_200(ctl->tp, TEXT_HTML);
	n = snprintf(s, S_MAX, "<p>This page was accessed %d times!</p>", ++cnt);
	tcp_send(ctl->tp, test2_hdr_html, sizeof(test2_hdr_html) - 1, 0);
	tcp_send(ctl->tp, s, n, 0);
	return tcp_send(ctl->tp, footer_html, sizeof(footer_html) - 1, 0);
}

/*---------------------------------------------------------------------------
  Quote of the Day 
  ---------------------------------------------------------------------------*/

const char qotd_hdr_html[] = DOCTYPE_HTML "<head>\r\n"
	"<title>ThinkOS Quote of The Day</title>\r\n" 
	"<meta http-equiv=\"refresh\" content=\"1\" />\r\n"
	META_HTTP META_COPY LINK_ICON LINK_CSS "</head>\r\n<body>\r\n"
	"<h1>ThinkOS Quote of The Day</h1>\r\n"
	"<p><pre>\r\n";

int qotd_cgi(struct httpctl * ctl)
{
	struct tcp_pcb * tp;
	char s[S_MAX];
	int n;

	httpd_200(ctl->tp, TEXT_HTML);
	tcp_send(ctl->tp, qotd_hdr_html, sizeof(qotd_hdr_html) - 1, 0);

	tp = tcp_alloc();
	if (tcp_connect(tp, INADDR_LOOPBACK, htons(17)) < 0) {
		n = snprintf(s, S_MAX, "ERROR: can't connect to QOTD!");
		tcp_send(ctl->tp, s, n, 0);
	} else {
		while ((n = tcp_recv(tp, s, S_MAX)) > 0)
			tcp_send(ctl->tp, s, n, 0);
	}
	tcp_close(tp);
	n = snprintf(s, S_MAX, "</pre></p>\r\n");
	tcp_send(ctl->tp, s, n, 0);

	return tcp_send(ctl->tp, footer_html, sizeof(footer_html) - 1, 0);
}

/*---------------------------------------------------------------------------
  Shell Command Exec
  ---------------------------------------------------------------------------*/

int http_write(struct httpctl * ctl, 
			   const void * buf, unsigned int len)
{
	return tcp_send(ctl->tp, buf, len, 0);
}

const struct fileop http_fops = {
	.write = (void *)http_write,
	.read = (void *)null_read,
	.flush = (void *)null_flush,
	.close = (void *)null_close
};

extern const struct shell_cmd shell_cmd_tab[];

int cmd_exec_cgi(struct httpctl * ctl)
{
	struct file http_file = {
		.data = ctl, 
		.op = &http_fops 
	};
	struct file * f = &http_file;
	struct shell_cmd * cmd;
	char * cmd_ln;
	int ret;

	cmd_ln = http_query_lookup(ctl, "cmd"); 
	if (cmd_ln == NULL) {
		DCC_LOG(LOG_WARNING, "invalid request!");
		/* 400 Bad Request */
		return httpd_400(ctl->tp);
	}

	DCC_LOGSTR(LOG_TRACE, "cmd=\"%s\"", cmd_ln);

	httpd_200(ctl->tp, TEXT_PLAIN);

	if ((cmd = cmd_lookup(shell_cmd_tab, cmd_ln)) == NULL) {
		fprintf(f, "Command not found!\n");
		return -1;
	}

	ret = cmd_exec(f, cmd, cmd_ln);
	if ((ret < 0) && (ret !=  SHELL_ABORT)) {
		fprintf(f, "Error: %d\n", -ret);
	}

	return 0;
}

const char shell_html[] = DOCTYPE_HTML "<head>\r\n"
	"<title>ThinkOS Shell Demo</title>\r\n" 
	META_HTTP META_COPY LINK_ICON LINK_CSS LINK_ZEPTO 
	"</head>\r\n<body>\r\n"
	"<h1>ThinkOS Shell Demo</h1>\r\n"
	"<p>Welcome to the <b>ThinkOS</b> shell demo</p>\r\n"
	"<pre>\r\n<div id=\"results\" style=\"min-height: 200px;"
	"border-style: solid; border-width: 1px; border-color: #111;"
	"\">\r\n</div>\r\n</pre>\r\n"
	"<div>\r\n<input type=\"text\" id=\"cmd\" value=\"help\" size=\"40\"/>\r\n"
	"<button id=\"btn\">Exec</button>\r\n"
	"<input type=\"text\" id=\"tmo\" value=\"1000\" size=\"4\"/>\r\n"
	"<input type=\"checkbox\" id=\"repeat\" />Repeat\r\n"
	"</div>\r\n"
	"<script>\r\n"
	"var timeout = 0;\r\n"
	"function cmd_exec() {\r\n"
	" $.ajax({\r\n"
	"  type: 'GET', \r\n"
	"  url: 'exec.cgi', \r\n"
	"  data: { cmd: $('#cmd').val() }, \r\n"
	"  dataType: 'text', \r\n"
	"  timeout: 500, \r\n"
	"  context: $('body'), \r\n"
	"  success: function(data) {\r\n"
	"   $('#results').empty();\r\n"
	"   $('#results').text(data);\r\n"
	"  },\r\n"
	"  error: function(xhr, type){ alert('Ajax error!') },\r\n"
	"  complete: function() { \r\n"
	"   if (timeout > 100) {\r\n"
	"    setTimeout(cmd_exec, timeout);\r\n"
	"   }\r\n"
	"  }\r\n"
	" });\r\n"
	"};\r\n"
	"function tmr_cfg() {\r\n"
	" if ($('#repeat').prop('checked')) {\r\n"
	"  timeout = parseInt($('#tmo').val());\r\n"
	" } else {\r\n"
	"  timeout = 0;\r\n"
	" } \r\n"
	"};\r\n"
	"$(function() {\r\n"
	" $(document).on('click', '#btn', cmd_exec);\r\n"
	" $(document).on('click', '#repeat', tmr_cfg);\r\n"
	"});\r\n"
	"</script>\r\n"
	HTML_FOOTER;


/*---------------------------------------------------------------------------
  root directory content
  ---------------------------------------------------------------------------*/

struct httpdobj www_cgi[] = {
	{ .oid = "index.html", .typ = OBJ_STATIC_HTML, .lvl = 255, 
		.len = sizeof(cgi_index_html) - 1, .ptr = cgi_index_html },
	{ .oid = "shell.html", .typ = OBJ_STATIC_HTML, .lvl = 255, 
		.len = sizeof(shell_html) - 1, .ptr = shell_html },
	{ .oid = "get_data.cgi", .typ = OBJ_CODE_CGI, .lvl = 100, 
		.len = 0, .ptr = get_data_cgi },
	{ .oid = "qotd.cgi", .typ = OBJ_CODE_CGI, .lvl = 100, 
		.len = 0, .ptr = qotd_cgi },
	{ .oid = "test1.cgi", .typ = OBJ_CODE_CGI, .lvl = 100, 
		.len = 0, .ptr = test1_cgi },
	{ .oid = "test2.cgi", .typ = OBJ_CODE_CGI, .lvl = 100, 
		.len = 0, .ptr = test2_cgi },
	{ .oid = "exec.cgi", .typ = OBJ_CODE_CGI, .lvl = 100, 
		.len = 0, .ptr = cmd_exec_cgi },
	{ .oid = NULL, .typ = 0, .lvl = 0, .len = 0, .ptr = NULL }
};


