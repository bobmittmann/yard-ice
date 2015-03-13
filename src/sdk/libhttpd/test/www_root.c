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
		"solid #aaa; padding:1px 2px 1px; }\r\n";

#if 0
	"td {padding: 4px;}\r\n"
	"input.text { background:#fff; margin:1px 2px 1px; border:1px solid #444;"
		"padding:1px 2px 1px; }\r\n"
	"input.text:hover { background:#cdf; }\r\n"
	"input.checkbox { border:0; }\r\n"
	"input.checkbox:hover { background:#cdf; }\r\n"
	"input.button { background:#ccc; margin:1px 4px 1px; "
		"border:1px solid #444; padding:2px 8px 2px; }\r\n"
	"input.button:hover {background:#458;color:#fff;text-decoration:none;}\r\n"
	"select { background:#fff; border:1px solid #666; }\r\n"
	"fieldset {padding:4px 4px 4px;border:1px solid #666; "
		"margin-top:6px; margin-bottom:6px;}\r\n"
	"form {padding:0;margin:0;border:0;margin-left:auto;"
		"margin-right:auto; display:table;}\r\n"
	".hidden {background:#fdd;border:0;margin:0;padding:0;width:1px;"
		"height:1px;opacity:0;filter:alpha(opacity=0);"
		"-ms-filter:\"alpha(opacity=0)\";-khtml-opacity:0;-moz-opacity:0;}\r\n";
#endif

#define HTML_FOOTER "<hr>"\
	"<a href=\"https://code.google.com/p/yard-ice\">"\
	"<img src=\"img/thinkos57x24.png\"></a>"\
	" - Cortex-M Operating System - "\
	"<i><a href=\"https://code.google.com/p/yard-ice\">YARD-ICE</a></i>"\
	"<br>&copy; Copyrigth 2013-2015, Bob Mittmann<br>"\
	"</body>\r\n</html>\r\n"

const char footer_html[] = HTML_FOOTER;

#define DOCTYPE_HTML "<!DOCTYPE html PUBLIC " \
	"\"-//W3C//DTD XHTML 1.0 Strict//EN\" "\
	"\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\r\n"\
	"<html xmlns=\"http://www.w3.org/1999/xhtml\">\r\n"

#define SERVER "Server: " HTTPD_SERVER_NAME "\r\n"

#define META_COPY "<meta name=\"copyright\" "\
	"content=\"Copyright (c) Bob Mittmann 2014\"/>\r\n"

#define META_HTTP "<meta http-equiv=\"content-type\" "\
	"content=\"text/html; charset=utf-8\"/>\r\n"

#define LINK_ICON "<link rel=\"shortcut icon\" "\
	"href=\"img/thinkos16x16.png\"/>\r\n"

#define LINK_CSS "<link rel=\"stylesheet\" href=\"style.css\" "\
	"type=\"text/css\" media=\"screen\"/>\r\n"

#define STYLE_LOGIN_CSS "<style type=\"text/css\">\r\n"\
	" #login { font-size:110%; padding:10px 10px 10px; }"\
	" #login input { font-size:110%; }"\
	" #login input.text { background:#f8f8f8; width:160px; }"\
	" #login input.text:hover { background:#cdf; }"\
	" #login input.button { font-size:110%; background:#ccc; color:#444;"\
	" border:1px solid #444; padding:4px 10px 4px; }"\
	" #login input.button:hover { background:#458; color:#fff;"\
	" text-decoration:none; }"\
	" #login table { margin:10px 10px 10px; }"\
	" #login p { margin:10px 10px 10px; }"\
	"\r\n</style>\r\n"

const char index_html[] = DOCTYPE_HTML "<head>\r\n"
	"<title>ThinkOS HTTP Server Demo</title>\r\n" 
	META_HTTP META_COPY LINK_ICON LINK_CSS STYLE_LOGIN_CSS 
	"</head>\r\n<body>\r\n"
	"<h1>ThinkOS Web Server Demo</h1>\r\n"
	"<p>Welcome to the <b>ThinkOS</b> web server demo initial page.</p>\r\n"
	"<ul>\r\n"
	"<li><a href=\"test1.cgi\">Dynamic page 1</a></li>\r\n"
	"<li><a href=\"test2.cgi\">Dynamic page 2</a></li>\r\n"
	"<li><a href=\"zarathustra.cgi\">Thus Spake Zarathustra</a></li>\r\n"
	"</ul>\r\n"
	HTML_FOOTER;

const char test1_hdr_html[] = DOCTYPE_HTML "<head>\r\n"
	"<title>ThinkOS Dynamic Page 1</title>\r\n" 
	META_HTTP META_COPY LINK_ICON LINK_CSS "</head>\r\n<body>\r\n"
	"<h1>ThinkOS Dynamic Page 1</h1>\r\n";

int test1_cgi(struct httpctl * ctl)
{
	static unsigned int cnt = 0;
	char s[256];
	int n;

	n = snprintf(s, 256, "<p>This page was accessed %d times!</p>", ++cnt);
	tcp_send(ctl->tp, test1_hdr_html, sizeof(test1_hdr_html), 0);
	tcp_send(ctl->tp, s, n, 0);
	return tcp_send(ctl->tp, footer_html, sizeof(footer_html), 0);
}

const char test2_hdr_html[] = DOCTYPE_HTML "<head>\r\n"
	"<title>ThinkOS Dynamic Page 2</title>\r\n" 
	META_HTTP META_COPY LINK_ICON LINK_CSS "</head>\r\n<body>\r\n"
	"<h1>ThinkOS Dynamic Page 2</h1>\r\n";

int test2_cgi(struct httpctl * ctl)
{
	static unsigned int cnt = 0;
	char s[256];
	int n;

	n = snprintf(s, 256, "<p>This page was accessed %d times!</p>", ++cnt);
	tcp_send(ctl->tp, test2_hdr_html, sizeof(test2_hdr_html), 0);
	tcp_send(ctl->tp, s, n, 0);
	return tcp_send(ctl->tp, footer_html, sizeof(footer_html), 0);
}

const char zarathustra_hdr_html[] = DOCTYPE_HTML "<head>\r\n"
	"<title>Thus Spake Zarathustra</title>\r\n" 
	META_HTTP META_COPY LINK_ICON LINK_CSS "</head>\r\n<body>\r\n"
	"<h1>Thus Spake Zarathustra</h1>\r\n"
	"<h2>by Friedrich Wilhelm Nietzsche</h2>\r\n"
	"<img src=\"img/nietzsche.jpg\" alt=\"Nietzsche\" align=\"middle\">"
	"<p>\r\n";

const char zarathustra_foot_html[] = "</p>\r\n";

extern const char const * zarathustra_txt[];
extern const uint16_t zarathustra_len[];

int zarathustra_cgi(struct httpctl * ctl)
{
	const char * txt;
	int i;

	tcp_send(ctl->tp, zarathustra_hdr_html, sizeof(zarathustra_hdr_html), 0);
	for (i = 0; (txt = zarathustra_txt[i]) != NULL; ++i) { 
		tcp_send(ctl->tp, txt, zarathustra_len[i], 0);
	}
	tcp_send(ctl->tp, zarathustra_foot_html, sizeof(zarathustra_foot_html), 0);
	return tcp_send(ctl->tp, footer_html, sizeof(footer_html), 0);
}


struct httpdobj www_root[] = {
	{ .oid = "style.css", .typ = OBJ_STATIC_CSS, .lvl = 255, 
		.len = sizeof(style_css) - 1, .ptr = style_css },
	{ .oid = "index.html", .typ = OBJ_STATIC_HTML, .lvl = 255, 
		.len = sizeof(index_html) - 1, .ptr = index_html },
	{ .oid = "test1.cgi", .typ = OBJ_CODE_CGI, .lvl = 100, 
		.len = 0, .ptr = test1_cgi },
	{ .oid = "test2.cgi", .typ = OBJ_CODE_CGI, .lvl = 100, 
		.len = 0, .ptr = test2_cgi },
	{ .oid = "zarathustra.cgi", .typ = OBJ_CODE_CGI, .lvl = 100, 
		.len = 0, .ptr = zarathustra_cgi },
	{ .oid = NULL, .typ = 0, .lvl = 0, .len = 0, .ptr = NULL }
};

