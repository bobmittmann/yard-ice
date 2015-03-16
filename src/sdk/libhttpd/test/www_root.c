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
		"border:1px solid #444; padding:1px 2px 1px; }\r\n"
	"input[type=text]:hover { background:#cdf; }\r\n"
	"input[type=button] { background:#ccc; margin:1px 4px 1px; "
		"border:1px solid #444; padding:2px 8px 2px; }\r\n"
	"input[type=button]:hover { background:#458; "
		"color:#fff; text-decoration:none; }\r\n"
	"input[type=reset] { background:#ccc; margin:1px 4px 1px; "
		"border:1px solid #444; padding:2px 8px 2px; }\r\n"
	"input[type=reset]:hover { background:#458; "
		"color:#fff; text-decoration:none; }\r\n"
	"input[type=submit] { background:#ccc; margin:1px 4px 1px; "
		"border:1px solid #444; padding:2px 8px 2px; }\r\n"
	"input[type=submit]:hover { background:#458; "
		"color:#fff; text-decoration:none; }\r\n"
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

#define HTML_FOOTER "<hr>\r\n"\
	"<a href=\"https://code.google.com/p/yard-ice\">"\
	"<img src=\"img/thinkos57x24.png\"></a>\r\n"\
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
	"href=\"img/thinkos16x16.png\"/>\r\n"

#define LINK_CSS "<link rel=\"stylesheet\" href=\"style.css\" "\
	"type=\"text/css\" media=\"screen\"/>\r\n"

#define LINK_MODALBOX "<script type=\"text/javascript\" "\
		"src=\"lib/modalbox.js.gz\"></script>\r\n"

#define LINK_ZEPTO "<script type=\"text/javascript\" "\
		"src=\"lib/zepto.js.gz\"></script>\r\n"

#define S_MAX 256 /* HTML formatting string buffer lenght */

const char footer_html[] = HTML_FOOTER;

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
	"<li><a href=\"test1.cgi\">Dynamic page 1</a></li>\r\n"
	"<li><a href=\"test2.cgi\">Dynamic page 2</a></li>\r\n"
	"<li><a href=\"zarathustra.html\">Thus Spake Zarathustra</a></li>\r\n"
	"<li><a href=\"form1.html\">Form Demo</a></li>\r\n"
	"<li><a href=\"ipcfg_form.cgi\">IP Configuration</a></li>\r\n"
	"<li><a href=\"treeview.html\">Treeview Demo</a></li>\r\n"
	"</ul>\r\n"
	HTML_FOOTER;

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

	n = snprintf(s, S_MAX, "<p>This page was accessed %d times!</p>", ++cnt);
	tcp_send(ctl->tp, test2_hdr_html, sizeof(test2_hdr_html) - 1, 0);
	tcp_send(ctl->tp, s, n, 0);
	return tcp_send(ctl->tp, footer_html, sizeof(footer_html) - 1, 0);
}

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
	char s[S_MAX];
	int n;
	int i;

	tcp_send(ctl->tp, form1_hdr_html, sizeof(form1_hdr_html) - 1, 0);
	for (i = 0; i < ctl->qrycnt; ++i) {
		char * key = ctl->qrylst[i].key;
		char * val = ctl->qrylst[i].val;
		n = snprintf(s, S_MAX, "<p>%d. %s='%s'</p>\r\n", i + 1, key, val);
		tcp_send(ctl->tp, s, n, 0);
	}
	return tcp_send(ctl->tp, footer_html, sizeof(footer_html) - 1, 0);
}

/*---------------------------------------------------------------------------
  IP Configuration
  ---------------------------------------------------------------------------*/

#include <arpa/inet.h>
#include <tcpip/ethif.h>
#include <tcpip/route.h>
#include <tcpip/in.h>
#include <netinet/in.h>

const char ipcfg_form_hdr_html[] = DOCTYPE_HTML "<head>\r\n"
	"<title>ThinkOS IP Configuration</title>\r\n" 
	META_HTTP META_COPY LINK_ICON LINK_CSS LINK_MODALBOX 
	"<script type=\"text/javascript\">\r\n"
	"function restore_defaults() { dlg.okSelected = function () {\r\n"
		"dlg.hide(); window.location.assign('restore.html'); }\r\n"
		"dlg.cancelSelected = function () { dlg.hide(); }\r\n"
		"dlg.show('defaults_dlg', 'Restore factory settings?', 400, 100); }\r\n"
	"function ipcfg_set() { dlg.okSelected = function () {\r\n"
		"dlg.hide(); document.getElementById('ipcfg_form').submit(); }\r\n"
		"dlg.cancelSelected = function () { dlg.hide(); }\r\n"
		"dlg.show('ipcfg_dlg', 'Change IP Settings?', 400, 100); }\r\n"
	"function init() { dlg.init(); }\r\n"
	"</script>\r\n</head>\r\n<body onload=\"init()\">\r\n"
	"<h1>ThinkOS IP Configuration</h1>\r\n"
	"<p>Please fill in the form and press <b>OK</b>.</p>\r\n"
	"<form id=\"ipcfg_form\" action=\"ipcfg_set.cgi\" method=\"POST\">\r\n"
	"<table>\r\n";

const char ipcfg_form_ftr_html[] = "</table><br>\r\n"
	"<input type=\"hidden\" name=\"apply\" value=\"0\">"
	"<input type=\"reset\" value=\"Reset\"> "
	"<input type=\"button\" onclick=\"history.back();\" value=\"Cancel\">\r\n"
	"<input type=\"button\" onclick=\"ipcfg_set()\" value=\"OK\" />\r\n"
	"</form>\r\n"
	"<div id=\"defaults_dlg\" class=\"dialog\">"
	"<h1 id=\"title\"></h1><div class=\"fbody\"><center>"
	"<p>Press <b>Confirm</b> if you are sure about restoring the "
		"factory default configuration.</p>	"
	"</center></div><div class=\"ffoot\"><center>"
	"<input type=\"button\" class=\"button\" "
		"onclick=\"dlg.cancelSelected()\" value=\"Cancel\" />"
	"<input type=\"button\" class=\"button\" " 
		"onclick=\"dlg.okSelected()\" value=\"Confirm\" />"
	"</center></div></div>\r\n"
	"<div id=\"ipcfg_dlg\" class=\"dialog\">"
	"<h1 id=\"title\"></h1><div class=\"fbody\"><center>"
	"<p>Press <b>Confirm</b> if you are sure about modifying the "
		"IP configuration.</p>	"
	"</center></div><div class=\"ffoot\"><center>"
	"<input type=\"button\" class=\"button\" "
		"onclick=\"dlg.cancelSelected()\" value=\"Cancel\" />"
	"<input type=\"button\" class=\"button\" " 
		"onclick=\"dlg.okSelected()\" value=\"Confirm\" />"
	"</center></div></div>\r\n"
	HTML_FOOTER;

int ipcfg_form_cgi(struct httpctl * ctl)
{
	in_addr_t ipv4_mask = INADDR_ANY;
	in_addr_t gw_addr = INADDR_ANY;
	in_addr_t ipv4_addr;
	struct ifnet * ifn;
	struct route * rt;
	char s[S_MAX];
	int n;

	/* get current configuration */
	if ((ifn = get_ifn_byname("eth0")) == NULL)
		return -1;

	ifn_ipv4_get(ifn, &ipv4_addr, &ipv4_mask);
	
	if ((rt = ipv4_route_get(INADDR_ANY, INADDR_ANY)) == NULL)
		gw_addr = INADDR_ANY;
	else
		gw_addr = rt->rt_gateway;

	tcp_send(ctl->tp, ipcfg_form_hdr_html, sizeof(ipcfg_form_hdr_html) - 1, 0);

	n = snprintf(s, S_MAX, "<tr><td>IP Address:</td><td>"
		 "<input type=\"text\" name=\"a1\" value=\"%d\" size=\"3\">."
		 "<input type=\"text\" name=\"a2\" value=\"%d\" size=\"3\">."
		 "<input type=\"text\" name=\"a3\" value=\"%d\" size=\"3\">."
		 "<input type=\"text\" name=\"a4\" value=\"%d\" size=\"3\">"
		 "</td></tr>\r\n",
		 IP4_ADDR1(ipv4_addr), IP4_ADDR2(ipv4_addr), 
		 IP4_ADDR3(ipv4_addr), IP4_ADDR4(ipv4_addr));
	tcp_send(ctl->tp, s, n, 0);

	n = snprintf(s, S_MAX, "<tr><td>Network mask:</td><td>"
		 "<input type=\"text\" name=\"m1\" value=\"%d\" size=\"3\">."
		 "<input type=\"text\" name=\"m2\" value=\"%d\" size=\"3\">."
		 "<input type=\"text\" name=\"m3\" value=\"%d\" size=\"3\">."
		 "<input type=\"text\" name=\"m4\" value=\"%d\" size=\"3\">"
		 "</td></tr>\r\n",
		 IP4_ADDR1(ipv4_mask), IP4_ADDR2(ipv4_mask), 
		 IP4_ADDR3(ipv4_mask), IP4_ADDR4(ipv4_mask));
	tcp_send(ctl->tp, s, n, 0);

	n = snprintf(s, S_MAX, "<tr><td>Gateway:</td><td>"
		 "<input type=\"text\" name=\"g1\" value=\"%d\" size=\"3\">."
		 "<input type=\"text\" name=\"g2\" value=\"%d\" size=\"3\">."
		 "<input type=\"text\" name=\"g3\" value=\"%d\" size=\"3\">."
		 "<input type=\"text\" name=\"g4\" value=\"%d\" size=\"3\">"
		 "</td></tr>\r\n",
		 IP4_ADDR1(gw_addr), IP4_ADDR2(gw_addr), 
		 IP4_ADDR3(gw_addr), IP4_ADDR4(gw_addr));
	tcp_send(ctl->tp, s, n, 0);

	return tcp_send(ctl->tp, ipcfg_form_ftr_html, 
					sizeof(ipcfg_form_ftr_html) - 1, 0);
}

const char ipcfg_set_hdr_html[] = DOCTYPE_HTML "<head>\r\n"
	"<title>ThinkOS IP Configuration Changed</title>\r\n" 
	META_HTTP META_COPY LINK_ICON LINK_CSS 
	"<script type=\"text/javascript\">\r\n"
	"function countdown() {" 
	"\tvar tmr = document.getElementById('countdown');\r\n"
	"\tvar sec = parseInt(tmr.innerHTML, 10);\r\n"
	"\tvar ipaddr = document.getElementById('ipaddr').innerHTML;\r\n"
	"\tsetInterval(function() { \r\n"
	"\tif (sec <= 1)\r\n"
	"\t\twindow.location.assign(\"http://\" + ipaddr + \"/\");\r\n"
	"\telse tmr.innerHTML = --sec;\r\n"
	"\t}, 1000);\r\n"
	"}\r\n"
	"</script>\r\n"
	"</head>\r\n"
	"<body onload=\"countdown()\">\r\n"
	"<h1>Ethernet IP Configuration Changed</h1><p>\r\n";

const char ipcfg_set_ftr_html[] = "</p>\r\n"
	"<p><b>WARNING:</b> The IP address was changed.</p>\r\n"
	"<p>This page will be redirected to the new address in "
	"<b id=\"countdown\">5</b> seconds...</p>\r\n"
	HTML_FOOTER;

int ipcfg_set_cgi(struct httpctl * ctl)
{
	in_addr_t ipv4_mask = INADDR_ANY;
	in_addr_t gw_addr = INADDR_ANY;
	in_addr_t ipv4_addr;
	struct ifnet * ifn;
	int use_dhcp = 0;
	char ip[16];
	char * cp;
	char s[S_MAX];
	int n;

	ipv4_addr = IP4_ADDR(atoi(http_query_lookup(ctl, "a1")), 
						 atoi(http_query_lookup(ctl, "a2")),
						 atoi(http_query_lookup(ctl, "a3")),
						 atoi(http_query_lookup(ctl, "a4")));

	ipv4_mask = IP4_ADDR(atoi(http_query_lookup(ctl, "m1")), 
						 atoi(http_query_lookup(ctl, "m2")),
						 atoi(http_query_lookup(ctl, "m3")),
						 atoi(http_query_lookup(ctl, "m4")));

	gw_addr = IP4_ADDR(atoi(http_query_lookup(ctl, "g1")), 
					   atoi(http_query_lookup(ctl, "g2")),
					   atoi(http_query_lookup(ctl, "g3")),
					   atoi(http_query_lookup(ctl, "g4")));

	tcp_send(ctl->tp, ipcfg_set_hdr_html, 
			 sizeof(ipcfg_set_hdr_html) - 1, 0);

	n = snprintf(s, S_MAX, "<table><tr><td>IP Address:</td><td id=\"ipaddr\">"
		 "%d.%d.%d.%d</td></tr>\r\n",
		 IP4_ADDR1(ipv4_addr), IP4_ADDR2(ipv4_addr), 
		 IP4_ADDR3(ipv4_addr), IP4_ADDR4(ipv4_addr));
	tcp_send(ctl->tp, s, n, 0);

	n = snprintf(s, S_MAX, "<tr><td>Network mask:</td><td>"
		 "%d.%d.%d.%d</td></tr>\r\n",
		 IP4_ADDR1(ipv4_mask), IP4_ADDR2(ipv4_mask), 
		 IP4_ADDR3(ipv4_mask), IP4_ADDR4(ipv4_mask));
	tcp_send(ctl->tp, s, n, 0);

	n = snprintf(s, S_MAX, "<tr><td>Gateway:</td><td>"
		 "%d.%d.%d.%d</td></tr>\r\n</table><br>\r\n",
		 IP4_ADDR1(gw_addr), IP4_ADDR2(gw_addr), 
		 IP4_ADDR3(gw_addr), IP4_ADDR4(gw_addr));
	tcp_send(ctl->tp, s, n, 0);

	tcp_send(ctl->tp, ipcfg_set_ftr_html, 
			 sizeof(ipcfg_set_ftr_html) - 1, 0);

	/* close thisp page connection */
	tcp_close(ctl->tp);
	ctl->tp = NULL;

	cp = s + sprintf(s, "%s", inet_ntop(AF_INET, 
										(void *)&ipv4_addr, ip, 16));
	cp += sprintf(cp, " %s", inet_ntop(AF_INET, 
									   (void *)&ipv4_mask, ip, 16));
	cp += sprintf(cp, " %s", inet_ntop(AF_INET, 
									   (void *)&gw_addr, ip, 16));
	sprintf(cp, " %d", use_dhcp);

	setenv("IPCFG", s, 1);

	//		tcp_flush(ctl->tp, TCP_FLUSH_SEND);
	/* wait for the TCP transmission to go trhugh */
	thinkos_sleep(2500);

	/* apply the config change */
	if ((ifn = get_ifn_byname("eth0")) != NULL) {
		ifn_ipv4_set(ifn, ipv4_addr, ipv4_mask);
		/* set the default route */
		ipv4_route_del(INADDR_ANY);
		ipv4_route_add(INADDR_ANY, INADDR_ANY, gw_addr, ifn);
	}

	return 0;
}


/*---------------------------------------------------------------------------
  root directory content
  ---------------------------------------------------------------------------*/

struct httpdobj www_root[] = {
	{ .oid = "style.css", .typ = OBJ_STATIC_CSS, .lvl = 255, 
		.len = sizeof(style_css) - 1, .ptr = style_css },
	{ .oid = "index.html", .typ = OBJ_STATIC_HTML, .lvl = 255, 
		.len = sizeof(index_html) - 1, .ptr = index_html },
	{ .oid = "test1.cgi", .typ = OBJ_CODE_CGI, .lvl = 100, 
		.len = 0, .ptr = test1_cgi },
	{ .oid = "test2.cgi", .typ = OBJ_CODE_CGI, .lvl = 100, 
		.len = 0, .ptr = test2_cgi },
	{ .oid = "zarathustra.html", .typ = OBJ_STATIC_HTML_GZ, .lvl = 100, 
		.len = SIZEOF_ZARATHUSTRA_HTML_GZ, .ptr = zarathustra_html_gz },
	{ .oid = "form1.html", .typ = OBJ_STATIC_HTML, .lvl = 255, 
		.len = sizeof(form1_html) - 1, .ptr = form1_html },
	{ .oid = "form1.cgi", .typ = OBJ_CODE_CGI, .lvl = 100, 
		.len = 0, .ptr = form1_cgi },
	{ .oid = "ipcfg_form.cgi", .typ = OBJ_CODE_CGI, .lvl = 100, 
		.len = 0, .ptr = ipcfg_form_cgi },
	{ .oid = "ipcfg_set.cgi", .typ = OBJ_CODE_CGI, .lvl = 100, 
		.len = 0, .ptr = ipcfg_set_cgi },
	{ .oid = "treeview.html", .typ = OBJ_STATIC_HTML_GZ, .lvl = 100, 
		.len = SIZEOF_TREEVIEW_HTML_GZ, .ptr = treeview_html_gz },
	{ .oid = "treeview.css", .typ = OBJ_STATIC_CSS_GZ, .lvl = 100, 
		.len = SIZEOF_TREEVIEW_CSS_GZ, .ptr = treeview_css_gz },
	{ .oid = NULL, .typ = 0, .lvl = 0, .len = 0, .ptr = NULL }
};

