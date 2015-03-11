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

const char style_css[] = "* { border:0; margin:0; padding:0; }\r\n"
	"a { color:#777; text-decoration:none; }\r\n"
	"a:hover { color:#333; text-decoration:none; }\r\n"
	"body {background:#f8f8f8;color:#555;font:0.8em Arial,Helvetica,"
		"\"bitstream vera sans\",sans-serif; }\r\n"
	"td {padding: 4px;}\r\n"
	"h1 {font:0.5em Arial,Helvetica,\"bitstream vera sans\",sans-serif;}\r\n"
	"table { border-collapse: collapse; }\r\n"
	"input.text { background:#fff; margin:1px 2px 1px; border:1px solid #444;"
		"padding:1px 2px 1px; }\r\n"
	"input.text:hover { background:#cdf; }\r\n"
	"input.checkbox { border:0; }\r\n"
	"input.checkbox:hover { background:#cdf; }\r\n"
	"input.button { background:#ccc; margin:1px 4px 1px; "
		"border:1px solid #444; padding:2px 8px 2px; }\r\n"
	"input.button:hover {background:#458;color:#fff;text-decoration:none;}\r\n"
	"textarea { background:#fff; margin:1px 2px 1px; border:1px "
		"solid #aaa; padding:1px 2px 1px; }\r\n"
	"select { background:#fff; border:1px solid #666; }\r\n"
	"fieldset {padding:4px 4px 4px;border:1px solid #666; "
		"margin-top:6px; margin-bottom:6px;}\r\n"
	"form {padding:0;margin:0;border:0;margin-left:auto;"
		"margin-right:auto; display:table;}\r\n"
	".hidden {background:#fdd;border:0;margin:0;padding:0;width:1px;"
		"height:1px;opacity:0;filter:alpha(opacity=0);"
		"-ms-filter:\"alpha(opacity=0)\";-khtml-opacity:0;-moz-opacity:0;}\r\n";

#define HTML_FOOTER "<hr>&copy; Copyrigth 2013-2015, Bob Mittmann" \
	"<br><b>ThinkOS</b> - Cortex-M Operating System - "\
	"<i><a href=\"https://code.google.com/p/yard-ice\">YARD-ICE</a></i>"\
	"</body></html>\r\n"

const char footer_html[] = HTML_FOOTER;

#define DOCTYPE_HTML "<!DOCTYPE html PUBLIC " \
	"\"-//W3C//DTD XHTML 1.0 Strict//EN\" "\
	"\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\r\n"\
	"<html xmlns=\"http://www.w3.org/1999/xhtml\">\r\n"

#define META_COPY "<meta name=\"copyright\" "\
	"content=\"Copyright (c) Bob Mittmann 2014\"/>\r\n"

#define META_HTTP "<meta http-equiv=\"content-type\" "\
	"content=\"text/html; charset=utf-8\"/>\r\n"

#define LINK_ICON "<link rel=\"shortcut icon\" href=\"favicon.ico\"/>\r\n"

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

const char index_html[] = DOCTYPE_HTML 
	"<head>\r\n"
	"<title>Index</title>\r\n" 
	META_HTTP META_COPY LINK_ICON LINK_CSS STYLE_LOGIN_CSS
	"</head>\r\n"
	"<body>\r\n"
    "<html><head><title>Index</title></head>\r\n"
	"<h2>Hello World</h2><p>Hello World!</p>\r\n"
	HTML_FOOTER;

int index_cgi(struct httpctl * ctl)
{
	const char dynamic_html[] = "<html><head><title>%s</title></head>" 
		"<h2>Hello World</h2><p>Cnt=%d</p>";
	static unsigned int cnt = 0;
	char s[256];
	int n;

	n = snprintf(s, 256, dynamic_html, "CGI-BIN", cnt++);

	tcp_send(ctl->tp, s, n, 0);
	return tcp_send(ctl->tp, footer_html, sizeof(footer_html), 0);
}

struct httpdobj httpd_root[] = {

	{ .oid = "style.css", .typ = OBJ_STATIC_CSS, .lvl = 255, 
		.len = sizeof(style_css), .ptr = style_css },
	{ .oid = "index.html", .typ = OBJ_STATIC_HTML, .lvl = 255, 
		.len = sizeof(index_html), .ptr = index_html },
	{ .oid = "index.cgi", .typ = OBJ_CODE_CGI, .lvl = 100, 
		.len = 0, .ptr = index_cgi },
	{ .oid = NULL, .typ = 0, .lvl = 0, .len = 0, .ptr = NULL }
};

