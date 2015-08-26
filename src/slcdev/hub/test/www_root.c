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
	"</ul>\r\n"
	HTML_FOOTER;

/*---------------------------------------------------------------------------
  Cascading Style Sheet (style.css)
  ---------------------------------------------------------------------------*/

const char style_css[] = "* { border: 0; margin: 0; }\r\n"
	"body { background: #f8f8f8; color: #555; font: 0.9em Arial,Helvetica,"
		"\"bitstream vera sans\",sans-serif; margin: 10px 10px 25px 10px; }\r\n"
	"a { color: #779; text-decoration:none; }\r\n"
	"a:hover { color:#335; text-decoration:none; }\r\n"
	"p { color: #111; text-align: justify; margin: 10px 0px 20px 0px; }\r\n"
	"ul { margin: 10px 0px 20px 0px; }\r\n"
	"h1 { font-size: 1.5em; text-align:left; margin: 10px 0px 10px 0px; }\r\n"
	"h2 { font-size: 1.3em; text-align:left;  margin: 10px 0px 25px 0px; }\r\n"
	"hr { background-color:#114; color:#112; width: 100%; height: 1px; "
		"margin: 10px 0px 5px 0px; }\r\n"
	"table { border-collapse: collapse;  margin: 0; }\r\n"
	"textarea { background:#fff; margin:1px 2px 1px; border:1px "
		"solid #aaa; padding:1px 2px 1px; }\r\n"
	"form { padding:0; margin:0; border:0; display:table;}\r\n"
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
