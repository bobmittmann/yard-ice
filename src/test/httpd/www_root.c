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
#include <sys/dcclog.h>
#include "www.h"

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

#if 0
	"select { background:#fff; border:1px solid #666; }\r\n"
	"fieldset {padding:4px 4px 4px;border:1px solid #666; "
		"margin-top:6px; margin-bottom:6px;}\r\n";
	"form { padding:0; margin:0; border:0; margin-left:auto;"
		"margin-right:auto; display:table;}\r\n"
	"td {padding: 4px;}\r\n"
	".hidden {background:#fdd;border:0;margin:0;padding:0;width:1px;"
		"height:1px;opacity:0;filter:alpha(opacity=0);"
		"-ms-filter:\"alpha(opacity=0)\";-khtml-opacity:0;-moz-opacity:0;}\r\n";
	"input.checkbox { border:0; }\r\n"
	"input.checkbox:hover { background:#cdf; }\r\n"
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

#endif

#define HTML_MAX 256 /* HTML formatting string buffer lenght */

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
	"<ul>\r\n"
	"<li><a href=\"tools.html\">Tools</a></li>\r\n"
	"<li><a href=\"cgi/test1.cgi\">Dynamic page 1</a></li>\r\n"
	"<li><a href=\"cgi/test2.cgi\">Dynamic page 2</a></li>\r\n"
	"<li><a href=\"cgi/qotd.cgi\">Qute of The Day</a></li>\r\n"
	"<li><a href=\"cgi/shell.html\">Command shell exec</a></li>\r\n"
	"<li><a href=\"zarathustra.html\">Thus Spake Zarathustra</a></li>\r\n"
	"<li><a href=\"form1.html\">Form Demo</a></li>\r\n"
	"<li><a href=\"ipcfg_form.cgi\">IP Configuration</a></li>\r\n"
	"<li><a href=\"treeview.html\">Treeview Demo</a></li>\r\n"
	"<li><a href=\"thinkos.html\">ThinkOS Documentation</a></li>\r\n"
	"</ul>\r\n"
	HTML_FOOTER;


/*---------------------------------------------------------------------------
  Form Demo
  ---------------------------------------------------------------------------*/

const char form1_html[] = DOCTYPE_HTML "<head>\r\n"
	"<title>ThinkOS Form Demo</title>\r\n" 
	META_HTTP META_COPY LINK_ICON LINK_CSS 
	"</head>\r\n<body>\r\n"
	"<h1>ThinkOS Form Demo</h1>\r\n"
	"<p>Please fill the infomation and press <b>Submit</b>.</p>\r\n"
	"<form action=\"form1.cgi\" method=\"POST\">\r\n"
	"First name:<br>\r\n"
	"<input type=\"text\" name=\"firstname\" value=\"Mickey\"><br>\r\n"
	"Last name:<br>\r\n"
	"<input type=\"text\" name=\"lastname\" value=\"Mouse\"><br>\r\n"
	"<br>\r\n"
	"<input type=\"submit\" value=\"Submit\">\r\n"
	"</form>\r\n"
	HTML_FOOTER;


const char form1_hdr_html[] = DOCTYPE_HTML "<head>\r\n"
	"<title>Form Demo Result</title>\r\n" 
	META_HTTP META_COPY LINK_ICON LINK_CSS "</head>\r\n<body>\r\n"
	"<h1>ThinkOS Demo Result</h1>\r\n";

int form1_cgi(struct httpctl * ctl)
{
	char s[HTML_MAX];
	int n;
	int i;

	httpd_200(ctl->tp, TEXT_HTML);
	http_send(ctl, form1_hdr_html, sizeof(form1_hdr_html) - 1);
	for (i = 0; i < ctl->qrycnt; ++i) {
		char * key = ctl->qrylst[i].key;
		char * val = ctl->qrylst[i].val;
		n = snprintf(s, HTML_MAX, "<p>%d. %s='%s'</p>\r\n", i + 1, key, val);
		http_send(ctl, s, n);
	}
	return http_send(ctl, footer_html, sizeof(footer_html) - 1);
}


const char tools_html[] = DOCTYPE_HTML "<head>\r\n"
	"<title>ThinkOS HTTP Server Demo</title>\r\n"
	META_COPY LINK_ICON LINK_CSS
	"</head>\r\n<body>\r\n"
	"<h1>ThinkOS Firmware Upgrade</h1>\r\n"
"<div class=\"form\">\r\n"
"<form method=\"post\" enctype=\"multipart/form-data\"\r\n"
"	action=\"/update.cgi\" onsubmit=\"upload_msg()\">\r\n"
"	<div class=\"fbody\">\r\n"
"		File:\r\n"
"<!--[if IE]>\r\n"
"		<input type=\"file\" id=\"browse\" class=\"text\""
"			maxlength=\"128\" size=\"40\" name=\"firmware\" />"
"<![endif]-->\r\n"
"<![if !IE]>\r\n"
"		<input type=\"text\" class=\"text\" id=\"fname\"\r\n"
"			readonly=\"readonly\" maxlength=\"128\" size=\"40\" />\r\n"
"		<input type=\"button\" class=\"button\" value=\"Browse...\"\r\n"
"			onclick=\"javascript:\r\n"
"			document.getElementById('browse').click()\" />\r\n"
"		<input type=\"file\" class=\"hidden\" id=\"browse\"\r\n"
"			name=\"firmware\" onchange=\"javascript:\r\n"
"			document.getElementById('fname').value = this.value\" />\r\n"
"<![endif]>\r\n"
"	</div>\r\n"
"	<div class=\"ffoot\">\r\n"
"		<input type=\"reset\" class=\"button\" value=\"Reset\" />\r\n"
"		<input type=\"submit\" class=\"button\" value=\"Upload...\" />\r\n"
"	</div>\r\n"
"</form>\r\n"
"</div>\r\n"
	HTML_FOOTER;

const char update_html[] = DOCTYPE_HTML "<head>\r\n"
	"<title>ThinkOS HTTP Server Demo</title>\r\n"
	META_COPY LINK_ICON LINK_CSS
	"</head>\r\n<body>\r\n"
	"<h1>ThinkOS Firmware Upgrade</h1>\r\n"
	HTML_FOOTER;

int update_cgi(struct httpctl * ctl)
{
	uint8_t s[HTML_MAX];
	int cnt = 0;
	int n;

	DCC_LOG1(LOG_TRACE, "sp=%08x", cm3_sp_get());

	httpd_200(ctl->tp, TEXT_PLAIN);

	while ((n = http_multipart_recv(ctl, s, HTML_MAX)) > 0) {
		DCC_LOG1(LOG_TRACE, "n=%d", n);
		if ((cnt == 0) || (n != HTML_MAX)) {
			DCC_LOG6(LOG_TRACE, "head: %02x %02x %02x %02x %02x %02x", 
					 s[0], s[1], s[2], s[3], s[4], s[5]);
			DCC_LOG6(LOG_TRACE, "tail: %02x %02x %02x %02x %02x %02x", 
					 s[n - 6], s[n - 5], s[n - 4], 
					 s[n - 3], s[n - 2], s[n - 1]);
		}
		cnt += n;
		http_send(ctl, s, n);
	}
	DCC_LOG1(LOG_TRACE, "file size: %d bytes", cnt);

	return 0;

	httpd_200(ctl->tp, TEXT_HTML);

	return http_send(ctl, update_html, sizeof(update_html) - 1);
}

/*---------------------------------------------------------------------------
  root directory content
  ---------------------------------------------------------------------------*/

struct httpdobj www_root[] = {
	{ .oid = "style.css", .typ = OBJ_STATIC_CSS, .lvl = 255, 
		.len = sizeof(style_css) - 1, .ptr = style_css },
	{ .oid = "index.html", .typ = OBJ_STATIC_HTML, .lvl = 255, 
		.len = sizeof(index_html) - 1, .ptr = index_html },
	{ .oid = "form1.html", .typ = OBJ_STATIC_HTML, .lvl = 255, 
		.len = sizeof(form1_html) - 1, .ptr = form1_html },
	{ .oid = "form1.cgi", .typ = OBJ_CODE_CGI, .lvl = 100, 
		.len = 0, .ptr = form1_cgi },
	{ .oid = "tools.html", .typ = OBJ_STATIC_HTML, .lvl = 255,
		.len = sizeof(tools_html) - 1, .ptr = tools_html },
	{ .oid = "update.cgi", .typ = OBJ_CODE_CGI, .lvl = 100, 
		.len = 0, .ptr = update_cgi },
	{ .oid = "update.html", .typ = OBJ_STATIC_HTML, .lvl = 255,
		.len = sizeof(update_html) - 1, .ptr = update_html },
#if 0
	{ .oid = "ipcfg_form.cgi", .typ = OBJ_CODE_CGI, .lvl = 100, 
		.len = 0, .ptr = ipcfg_form_cgi },
	{ .oid = "ipcfg_set.cgi", .typ = OBJ_CODE_CGI, .lvl = 100, 
		.len = 0, .ptr = ipcfg_set_cgi },
	{ .oid = "zarathustra.html", .typ = OBJ_STATIC_HTML_GZ, .lvl = 100, 
		.len = SIZEOF_ZARATHUSTRA_HTML_GZ, .ptr = zarathustra_html_gz },
	{ .oid = "treeview.html", .typ = OBJ_STATIC_HTML_GZ, .lvl = 100, 
		.len = SIZEOF_TREEVIEW_HTML_GZ, .ptr = treeview_html_gz },
	{ .oid = "treeview.css", .typ = OBJ_STATIC_CSS_GZ, .lvl = 100, 
		.len = SIZEOF_TREEVIEW_CSS_GZ, .ptr = treeview_css_gz },
	{ .oid = "thinkos.html", .typ = OBJ_STATIC_HTML, .lvl = 255, 
		.len = SIZEOF_THINKOS_HTML, .ptr = thinkos_html },
#endif
	{ .oid = NULL, .typ = 0, .lvl = 0, .len = 0, .ptr = NULL }
};

