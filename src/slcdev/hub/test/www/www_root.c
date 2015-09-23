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
 * @file yard-ice.c
 * @brief YARD-ICE application main
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <tcpip/httpd.h>
#include <thinkos.h>
#include <trace.h>
#include <sys/file.h>
#include <sys/null.h>
#include "www.h"

const char footer_html[] = HTML_FOOTER;
const uint16_t sizeof_footer_html = sizeof(footer_html) - 1;

/*---------------------------------------------------------------------------
  Initial page (index.html)
  ---------------------------------------------------------------------------*/

const char index_html[] = DOCTYPE_HTML "<head>\r\n"
	"<title>ThinkOS HTTP Server Demo</title>\r\n" 
	META_COPY LINK_ICON LINK_CSS
	"</head>\r\n<body>\r\n"
	"<h1>ThinkOS Web Server Demo</h1>\r\n"
	"<p>Welcome to the <b>ThinkOS</b> web server demo initial page.</p>\r\n"
	"<ul>\r\n"
	"<li><a href=\"ipcfg/ipcfg_form.cgi\">IP Configuration</a></li>\r\n"
	"<li><a href=\"tools.html\">Tools</a></li>\r\n"
	"<li><a href=\"rpc_test.html\">RPC Test</a></li>\r\n"
	"</ul>\r\n"
	HTML_FOOTER;

const char update_html[] = DOCTYPE_HTML "<head>\r\n"
    "<title>ThinkOS HTTP Server Demo</title>\r\n"
    META_COPY LINK_ICON LINK_CSS
    "</head>\r\n<body>\r\n"
    "<h1>ThinkOS Firmware Upgrade</h1>\r\n"
    HTML_FOOTER;

int update_cgi(struct httpctl * ctl)
{
    char s[HTML_MAX + 1];
    int n;
    int cnt = 0;

    while ((n = http_multipart_recv(ctl, s, 1)) > 0) {
        (void)n;
        DBG("http_multipart_recv=%d", n);
        cnt += n;
    }

    DBG("file size=%d", cnt);

    httpd_200(ctl->tp, TEXT_HTML);

    return http_send(ctl, update_html, sizeof(update_html) - 1);
}

const struct fileop http_fops = {
	.write = (void *)http_send,
	.read = (void *)null_read,
	.flush = (void *)null_flush,
	.close = (void *)null_close
};

int rpc_test_file_write(FILE * f, int port);
int rpc_test_file_read(FILE * f, int port);

int test1_cgi(struct httpctl * http)
{
	struct file http_file = {
		.data = http,
		.op = &http_fops
	};
	struct file * f = &http_file;
    int code;
    int port;

   	httpd_200(http->tp, TEXT_PLAIN);

   	code = atoi(http_query_lookup(http, "code"));
    INF("code=%d", code);
    port = atoi(http_query_lookup(http, "port"));
    INF("port=%d", port);

    fprintf(f, "This is a test!\n");
    fprintf(f, "Code = %d\n", code);
    fprintf(f, "Port = %d\n", port);

    switch (code) {
    case 1:
    	rpc_test_file_write(f, port);
    	break;
    case 2:
    	rpc_test_file_read(f, port);
    	break;
    }

   	return 0;
}


int test2_cgi(struct httpctl * http)
{
    char s[HTML_MAX + 1];
   	int param;
    int n;
   	int i;

   	param = atoi(http_query_lookup(http, "param"));
    INF("rows=%d", param);

    httpd_200(http->tp, APPLICATION_JSON);

    n = snprintf(s, HTML_MAX, "{\"list\": [");
    http_send(http, s, n);
    for (i = 0; i < param; ++i) {
    	if (i == 0)
    		n = snprintf(s, HTML_MAX, "{\"fname\":\"Row %d\"}", i);
    	else
    		n = snprintf(s, HTML_MAX, ",{\"fname\":\"Row %d\"}", i);
    	http_send(http, s, n);
    }
    n = snprintf(s, HTML_MAX, "]}\r\n");
    http_send(http, s, n);

   	return 0;
}
/*---------------------------------------------------------------------------
  root directory content
  ---------------------------------------------------------------------------*/

struct httpdobj www_root[] = {
	{ .oid = "style.css", .typ = OBJ_STATIC_CSS_GZ, .lvl = 255,
		.len = SIZEOF_STYLE_CSS, .ptr = style_css },
	{ .oid = "index.html", .typ = OBJ_STATIC_HTML, .lvl = 255,
		.len = sizeof(index_html) - 1, .ptr = index_html },
	{ .oid = "tools.html", .typ = OBJ_STATIC_HTML_GZ, .lvl = 255,
		.len = SIZEOF_TOOLS_HTML, .ptr = tools_html },
	{ .oid = "rpc_test.html", .typ = OBJ_STATIC_HTML_GZ, .lvl = 255,
		.len = SIZEOF_RPC_TEST_HTML, .ptr = rpc_test_html },
    { .oid = "update.cgi", .typ = OBJ_CODE_CGI, .lvl = 100,
        .len = 0, .ptr = update_cgi },
	{ .oid = "update.html", .typ = OBJ_STATIC_HTML, .lvl = 255,
		.len = sizeof(update_html) - 1, .ptr = update_html },
	{ .oid = "network.html", .typ = OBJ_STATIC_HTML_GZ, .lvl = 255,
		.len = SIZEOF_NETWORK_HTML, .ptr = network_html },
	{ .oid = "test1.cgi", .typ = OBJ_CODE_CGI, .lvl = 100,
		.len = 0, .ptr = test1_cgi },
	{ .oid = "test2.cgi", .typ = OBJ_CODE_CGI, .lvl = 100,
		.len = 0, .ptr = test2_cgi },
	{ .oid = NULL, .typ = 0, .lvl = 0, .len = 0, .ptr = NULL }
};

