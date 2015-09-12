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
 * @file webserver.c
 * @brief YARD-ICE application main
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <tcpip/httpd.h>
#include <thinkos.h>

/*---------------------------------------------------------------------------
  Common HTML macros
  ---------------------------------------------------------------------------*/

#define HTML_FOOTER "<hr>\r\n"\
	"<a href=\"https://code.google.com/p/yard-ice\">"\
	"<img src=\"/img/thinkos57x24.png\"></a>\r\n"\
	" - Cortex-M Operating System - \r\n"\
	"<i><a href=\"https://code.google.com/p/yard-ice\">YARD-ICE</a></i><br>\r\n"\
	"&copy; Copyright 2013-2015, Bob Mittmann<br>\r\n"\
	"</body>\r\n</html>\r\n"

#define DOCTYPE_HTML "<!DOCTYPE html PUBLIC " \
	"\"-//W3C//DTD XHTML 1.0 Strict//EN\" "\
	"\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\r\n"\
	"<html xmlns=\"http://www.w3.org/1999/xhtml\">\r\n"

#define SERVER "Server: " HTTPD_SERVER_NAME "\r\n"

#define META_COPY "<meta name=\"copyright\" "\
	"content=\"Copyright (c) Bob Mittmann 2014-2015\"/>\r\n"

#define META_HTTP "<meta http-equiv=\"content-type\" "\
	"content=\"text/html; charset=utf-8\"/>\r\n"

#define LINK_ICON "<link rel=\"shortcut icon\" "\
	"href=\"/img/thinkos16x16.png\"/>\r\n"

#define LINK_CSS "<link rel=\"stylesheet\" href=\"/style.css\" "\
	"type=\"text/css\" media=\"screen\"/>\r\n"

#define LINK_MODALBOX "<script type=\"text/javascript\" "\
		"src=\"/lib/modalbox.js.gz\"></script>\r\n"

#define LINK_ZEPTO "<script type=\"text/javascript\" "\
		"src=\"/lib/zepto.js.gz\"></script>\r\n"

const char style_css[] = "* { border: 0; margin: 0; padding:1; }\r\n"
	"body { background: #f8f8f8; color: #555; font: 1.0em Arial,Helvetica,"
		"\"bitstream vera sans\",sans-serif; margin: 10px 10px 25px 10px; }\r\n"
	"a { color: #779; text-decoration:none; }\r\n"
	"a:hover { color:#335; text-decoration:none; }\r\n"
	"p { color: #111; text-align: justify; margin: 10px 0px 20px 0px; }\r\n"
	"ul { margin: 10px 0px 20px 0px; }\r\n"
	"h1 { font: 0.9em; text-align:center; margin: 10px 0px 10px 0px; }\r\n"
	"h2 { font: 0.9em; text-align:center;  margin: 10px 0px 25px 0px; }\r\n"
	"hr { background-color:#114; color:#112; width: 100%; height: 1px; " 
		"margin: 10px 0px 5px 0px; }\r\n"
	"table { border-collapse: collapse; }\r\n"
	"textarea { background:#fff; margin:1px 2px 1px; border:1px "
		"solid #aaa; padding:1px 2px 1px; }\r\n"
	"form { padding:0; margin:0; border:0; display:table;}\r\n"
	"div.form { padding:0; margin-left:auto; margin-right:auto; "
		"display:table; }\r\n"
	"div.fbody { margin:0; border:0; padding:4px 12px 4px; }\r\n"
	"div.fcol { float:left; margin:0; border:0; padding:0px 6px 0px; }\r\n"
	"div.ffoot { padding:4px 12px 4px; border:0; margin:0; "
		"text-align:left; }\r\n"
	"input[type=text] { background:#fff; margin:1px 2px 1px; " 
		"border:1px solid #444; padding:3px 3px 2px; }\r\n"
	"input[type=text]:hover { background:#cdf; }\r\n"
	"input[type=button],input[type=reset],input[type=submit],button {"  
		"background:#ccc; margin:1px 4px 1px; "
		"border:1px solid #444; padding:4px 8px 3px; }\r\n"
	"input[type=button]:hover,input[type=reset]:hover,"
	"input[type=submit]:hover,button:hover {"
		" background:#458; color:#fff; text-decoration:none; }\r\n"
	/* modal dialog box */
	"#mbox { background-color:#eee; padding:8px; border:2px solid #222; }\r\n"
	"#dlg { clear:both; }\r\n"
	"#dlg h1 { text-align:left; border-bottom:1px dashed #666; "
		"font-size:1.1em; padding:1px 0 0; margin-bottom:4px; "
		"clear:both; width:100%; }\r\n"
	"#ol { background-image: url(img/overlay.png); } \r\n"
	".dialog { display:none }\r\n"
	"* html #ol{ background-image:none; "
		"filter:progid:DXImageTransform.Microsoft.AlphaImageLoader("
		"src=\"img/overlay.png\", sizingMethod=\"scale\"); } \r\n"
	".hidden { background:#fdd; border:0; margin:0; padding:0;"
		"width: 1px; height: 1px; opacity: 0; filter: alpha(opacity=0);" 
		"-ms-filter: \"alpha(opacity=0)\"; -khtml-opacity: 0; "
		"-moz-opacity: 0; } \r\n";

#define S_MAX 256 /* HTML formatting string buffer lenght */

static const char footer_html[] = HTML_FOOTER;

/*---------------------------------------------------------------------------
  Initial page (index.html)
  ---------------------------------------------------------------------------*/

const char index_html[] = DOCTYPE_HTML "<head>\r\n"
	"<title>ThinkOS HTTP Server Demo</title>\r\n" 
	META_HTTP META_COPY LINK_ICON LINK_CSS 
	"</head>\r\n<body>\r\n"
	"<h1>ThinkOS Web Server Demo</h1>\r\n"
	"<p>Welcome to the <b>ThinkOS</b> web server demo initial page.</p>\r\n"
	"<p>Please fill the infomation and press <b>Submit</b>.</p>\r\n"
	"<form action=\"form1.cgi\" method=\"POST\">\r\n"
	"First name:<br>\r\n"
	"<input type=\"text\" name=\"firstname\" value=\"Mickey\"><br>\r\n"
	"Last name:<br>\r\n"
	"<input type=\"text\" name=\"lastname\" value=\"Mouse\"><br>\r\n"
	"<br>\r\n"
	"<input type=\"submit\" value=\"Submit\">\r\n"
	"</form>\r\n"
	"<ul>\r\n"
	"<li><a href=\"ipcfg_form.cgi\">IP Configuration</a></li>\r\n"
	"</ul>\r\n"
	HTML_FOOTER;

const char form1_hdr_html[] = DOCTYPE_HTML "<head>\r\n"
	"<title>Form Demo Result</title>\r\n" 
	META_HTTP META_COPY LINK_ICON LINK_CSS "</head>\r\n<body>\r\n"
	"<h1>ThinkOS Demo Result</h1>\r\n";

int form1_cgi(struct httpctl * ctl)
{
	char s[S_MAX];
	int n;
	int i;

	httpd_200(ctl->tp, TEXT_HTML);
	http_send(ctl, form1_hdr_html, sizeof(form1_hdr_html) - 1);
	for (i = 0; i < ctl->qrycnt; ++i) {
		char * key = ctl->qrylst[i].key;
		char * val = ctl->qrylst[i].val;
		n = snprintf(s, S_MAX, "<p>%d. %s='%s'</p>\r\n", i + 1, key, val);
		http_send(ctl, s, n);
	}
	return http_send(ctl, footer_html, sizeof(footer_html) - 1);
}


/*---------------------------------------------------------------------------
  root directory content
  ---------------------------------------------------------------------------*/

struct httpdobj www_root[] = {
	{ .oid = "style.css", .typ = OBJ_STATIC_CSS, .lvl = 255, 
		.len = sizeof(style_css) - 1, .ptr = style_css },
	{ .oid = "index.html", .typ = OBJ_STATIC_HTML, .lvl = 255, 
		.len = sizeof(index_html) - 1, .ptr = index_html },
	{ .oid = NULL, .typ = 0, .lvl = 0, .len = 0, .ptr = NULL }
};

struct httpddir httpd_dir[] = {
	{ .path = "/", .objlst = www_root },
	{ .path = NULL, .objlst = NULL }
};

int httpd_server_task(struct httpd * httpd)
{
	struct httpctl httpctl;
	struct httpctl * ctl = &httpctl;
	const struct httpdobj * obj;
	unsigned int cnt = 0;
	unsigned int id = thinkos_thread_self();
	(void)id;

	for (;;) {
//		printf("Wating for connection.\n");
		if (http_accept(httpd, ctl) < 0) {
//			printf("tcp_accept() failed!\n");
			thinkos_sleep(1000);
			continue;
		}

//		printf("Connection accepted.\n");
		if ((obj = http_obj_lookup(ctl)) != NULL) {
			cnt++;
			switch (ctl->method) {
			case HTTP_GET:
//				DCC_LOG3(LOG_TRACE, "%2d %6d GET \"%s\"", id, cnt, obj->oid);
				http_get(ctl, obj);
				break;
			case HTTP_POST:
//				DCC_LOG3(LOG_TRACE, "%2d %6d POST \"%s\"", id, cnt, obj->oid);
				http_post(ctl, obj);
				break;
			}
		}

		http_close(ctl);
	}

	return 0;
}

uint32_t httpd_stack[512];

const struct thinkos_thread_inf httpd_inf = {
	.stack_ptr = httpd_stack,
	.stack_size = sizeof(httpd_stack),
	.priority = 32,
	.thread_id = 12,
	.paused = 0,
	.tag = "HTTPD"
};

int webserver_init(void)
{
	struct httpd * httpd;

	httpd = httpd_alloc();

	httpd_init(httpd, 80, 4, httpd_dir, NULL);

	return thinkos_thread_create_inf((void *)httpd_server_task,
			(void *)httpd, &httpd_inf);
}
