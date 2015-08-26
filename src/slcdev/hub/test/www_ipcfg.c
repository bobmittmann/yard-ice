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


#include "www.h"

#include <arpa/inet.h>
#include <tcpip/ethif.h>
#include <tcpip/route.h>
#include <tcpip/in.h>
#include <netinet/in.h>
#include <thinkos.h>

/*---------------------------------------------------------------------------
  IP Configuration
  ---------------------------------------------------------------------------*/

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
	char s[HTML_MAX];
	int n;

	httpd_200(ctl->tp, TEXT_HTML);

	/* get current configuration */
	if ((ifn = get_ifn_byname("eth0")) == NULL)
		return -1;

	ifn_ipv4_get(ifn, &ipv4_addr, &ipv4_mask);
	
	if ((rt = ipv4_route_get(INADDR_ANY, INADDR_ANY)) == NULL)
		gw_addr = INADDR_ANY;
	else
		gw_addr = rt->rt_gateway;

	tcp_send(ctl->tp, ipcfg_form_hdr_html, sizeof(ipcfg_form_hdr_html) - 1, 0);

	n = snprintf(s, HTML_MAX, "<tr><td>IP Address:</td><td>"
		 "<input type=\"text\" name=\"a1\" value=\"%d\" size=\"3\">."
		 "<input type=\"text\" name=\"a2\" value=\"%d\" size=\"3\">."
		 "<input type=\"text\" name=\"a3\" value=\"%d\" size=\"3\">."
		 "<input type=\"text\" name=\"a4\" value=\"%d\" size=\"3\">"
		 "</td></tr>\r\n",
		 IP4_ADDR1(ipv4_addr), IP4_ADDR2(ipv4_addr), 
		 IP4_ADDR3(ipv4_addr), IP4_ADDR4(ipv4_addr));
	tcp_send(ctl->tp, s, n, 0);

	n = snprintf(s, HTML_MAX, "<tr><td>Network mask:</td><td>"
		 "<input type=\"text\" name=\"m1\" value=\"%d\" size=\"3\">."
		 "<input type=\"text\" name=\"m2\" value=\"%d\" size=\"3\">."
		 "<input type=\"text\" name=\"m3\" value=\"%d\" size=\"3\">."
		 "<input type=\"text\" name=\"m4\" value=\"%d\" size=\"3\">"
		 "</td></tr>\r\n",
		 IP4_ADDR1(ipv4_mask), IP4_ADDR2(ipv4_mask), 
		 IP4_ADDR3(ipv4_mask), IP4_ADDR4(ipv4_mask));
	tcp_send(ctl->tp, s, n, 0);

	n = snprintf(s, HTML_MAX, "<tr><td>Gateway:</td><td>"
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
	char s[HTML_MAX];
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

	httpd_200(ctl->tp, TEXT_HTML);

	tcp_send(ctl->tp, ipcfg_set_hdr_html, 
			 sizeof(ipcfg_set_hdr_html) - 1, 0);

	n = snprintf(s, HTML_MAX, "<table><tr><td>IP Address:</td><td id=\"ipaddr\">"
		 "%d.%d.%d.%d</td></tr>\r\n",
		 IP4_ADDR1(ipv4_addr), IP4_ADDR2(ipv4_addr), 
		 IP4_ADDR3(ipv4_addr), IP4_ADDR4(ipv4_addr));
	tcp_send(ctl->tp, s, n, 0);

	n = snprintf(s, HTML_MAX, "<tr><td>Network mask:</td><td>"
		 "%d.%d.%d.%d</td></tr>\r\n",
		 IP4_ADDR1(ipv4_mask), IP4_ADDR2(ipv4_mask), 
		 IP4_ADDR3(ipv4_mask), IP4_ADDR4(ipv4_mask));
	tcp_send(ctl->tp, s, n, 0);

	n = snprintf(s, HTML_MAX, "<tr><td>Gateway:</td><td>"
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
  directory content
  ---------------------------------------------------------------------------*/

struct httpdobj www_ipcfg[] = {
	{ .oid = "ipcfg_form.cgi", .typ = OBJ_CODE_CGI, .lvl = 100,
		.len = 0, .ptr = ipcfg_form_cgi },
	{ .oid = "ipcfg_set.cgi", .typ = OBJ_CODE_CGI, .lvl = 100,
		.len = 0, .ptr = ipcfg_set_cgi },
	{ .oid = NULL, .typ = 0, .lvl = 0, .len = 0, .ptr = NULL }
};

