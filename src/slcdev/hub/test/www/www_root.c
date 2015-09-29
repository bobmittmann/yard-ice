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
	"<title>CRAB DEvice Simulator - Home</title>\r\n"
	META_COPY LINK_ICON LINK_CSS
	"</head>\r\n<body>\r\n"
	"<h1>CRAB Device Simulator - Home</h1>\r\n"
	"<p>Welcome to the <b>CRAB</b> device simulator web interface.</p>\r\n"
	"<p>Please select one option:</p>\r\n"
	"<ul>\r\n"
	"<li><a href=\"sim_monitor.html\">Devices Monitor</a></li>\r\n"
	"<li><a href=\"sim_config.html\">Simulator Configuration</a></li>\r\n"
	"<li><a href=\"sim_database.html\">Simulator Database</a></li>\r\n"
	"<li><a href=\"sim_tools.html\">Simulator Tools</a></li>\r\n"
	"<li><a href=\"ipcfg/ipcfg_form.cgi\">IP Configuration</a></li>\r\n"
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

/*---------------------------------------------------------------------------
  root directory content
  ---------------------------------------------------------------------------*/

struct httpdobj www_root[] = {
	{ .oid = "style.css", .typ = OBJ_STATIC_CSS_GZ, .lvl = 255,
		.len = SIZEOF_STYLE_CSS, .ptr = style_css },
	{ .oid = "index.html", .typ = OBJ_STATIC_HTML, .lvl = 255,
		.len = sizeof(index_html) - 1, .ptr = index_html },
	{ .oid = "rpc_test.html", .typ = OBJ_STATIC_HTML_GZ, .lvl = 255,
		.len = SIZEOF_RPC_TEST_HTML, .ptr = rpc_test_html },
	{ .oid = "sim_tools.html", .typ = OBJ_STATIC_HTML_GZ, .lvl = 255,
		.len = SIZEOF_SIM_TOOLS_HTML, .ptr = sim_tools_html },
	{ .oid = "sim_config.html", .typ = OBJ_STATIC_HTML_GZ, .lvl = 255,
		.len = SIZEOF_SIM_CONFIG_HTML, .ptr = sim_config_html },
	{ .oid = "sim_database.html", .typ = OBJ_STATIC_HTML_GZ, .lvl = 255,
		.len = SIZEOF_SIM_DATABASE_HTML, .ptr = sim_database_html },
	{ .oid = "sim_monitor.html", .typ = OBJ_STATIC_HTML_GZ, .lvl = 255,
		.len = SIZEOF_SIM_MONITOR_HTML, .ptr = sim_monitor_html },
	{ .oid = "rpc_test.cgi", .typ = OBJ_CODE_CGI, .lvl = 100,
		.len = 0, .ptr = rpc_test_cgi },
    { .oid = "update.cgi", .typ = OBJ_CODE_CGI, .lvl = 100,
        .len = 0, .ptr = update_cgi },
    { .oid = "cfg_load.cgi", .typ = OBJ_CODE_CGI, .lvl = 100,
        .len = 0, .ptr = cfg_load_cgi },
    { .oid = "file_read.cgi", .typ = OBJ_CODE_CGI, .lvl = 100,
        .len = 0, .ptr = file_read_cgi },
    { .oid = "file_write.cgi", .typ = OBJ_CODE_CGI, .lvl = 100,
        .len = 0, .ptr = file_write_cgi },
    { .oid = "db_load.cgi", .typ = OBJ_CODE_CGI, .lvl = 100,
        .len = 0, .ptr = db_load_cgi },
    { .oid = "firmware_load.cgi", .typ = OBJ_CODE_CGI, .lvl = 100,
        .len = 0, .ptr = firmware_load_cgi },
    { .oid = "get_status.cgi", .typ = OBJ_CODE_CGI, .lvl = 100,
        .len = 0, .ptr = get_status_cgi },
    { .oid = "rpc_exec.cgi", .typ = OBJ_CODE_CGI, .lvl = 100,
        .len = 0, .ptr = rpc_exec_cgi },
    { .oid = "rpc_js.cgi", .typ = OBJ_CODE_CGI, .lvl = 100,
        .len = 0, .ptr = rpc_js_cgi },
    { .oid = "db_compile.cgi", .typ = OBJ_CODE_CGI, .lvl = 100,
        .len = 0, .ptr = db_compile_cgi },
    { .oid = "cfg_compile.cgi", .typ = OBJ_CODE_CGI, .lvl = 100,
        .len = 0, .ptr = cfg_compile_cgi },
    { .oid = "cfg_getinfo.cgi", .typ = OBJ_CODE_CGI, .lvl = 100,
        .len = 0, .ptr = cfg_getinfo_cgi },
    { .oid = "db_getinfo.cgi", .typ = OBJ_CODE_CGI, .lvl = 100,
        .len = 0, .ptr = db_getinfo_cgi },
    { .oid = "sim_getattr.cgi", .typ = OBJ_CODE_CGI, .lvl = 100,
        .len = 0, .ptr = sim_getattr_cgi },
	{ .oid = "update.html", .typ = OBJ_STATIC_HTML, .lvl = 255,
		.len = sizeof(update_html) - 1, .ptr = update_html },
	{ .oid = "network.html", .typ = OBJ_STATIC_HTML_GZ, .lvl = 255,
		.len = SIZEOF_NETWORK_HTML, .ptr = network_html },
	{ .oid = NULL, .typ = 0, .lvl = 0, .len = 0, .ptr = NULL }
};

