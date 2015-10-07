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
  root directory content
  ---------------------------------------------------------------------------*/

struct httpdobj www_root[] = {
	{ .oid = "layout.css", .typ = OBJ_STATIC_CSS_GZ, .lvl = 255,
		.len = SIZEOF_LAYOUT_CSS, .ptr = layout_css },
	{ .oid = "style.css", .typ = OBJ_STATIC_CSS_GZ, .lvl = 255,
		.len = SIZEOF_STYLE_CSS, .ptr = style_css },
	{ .oid = "zepto.min.js", .typ = OBJ_STATIC_JS_GZ, .lvl = 255,
		.len = SIZEOF_ZEPTO_MIN_JS, .ptr = zepto_min_js },
	{ .oid = "simlib.js", .typ = OBJ_STATIC_JS_GZ, .lvl = 255,
		.len = SIZEOF_SIMLIB_JS, .ptr = simlib_js },
	{ .oid = "index.html", .typ = OBJ_STATIC_HTML_GZ, .lvl = 255,
		.len = SIZEOF_INDEX_HTML - 1, .ptr = index_html },
	{ .oid = "sim_test.html", .typ = OBJ_STATIC_HTML_GZ, .lvl = 255,
		.len = SIZEOF_SIM_TEST_HTML, .ptr = sim_test_html },
	{ .oid = "sim_tools.html", .typ = OBJ_STATIC_HTML_GZ, .lvl = 255,
		.len = SIZEOF_SIM_TOOLS_HTML, .ptr = sim_tools_html },
	{ .oid = "sim_config.html", .typ = OBJ_STATIC_HTML_GZ, .lvl = 255,
		.len = SIZEOF_SIM_CONFIG_HTML, .ptr = sim_config_html },
	{ .oid = "sim_database.html", .typ = OBJ_STATIC_HTML_GZ, .lvl = 255,
		.len = SIZEOF_SIM_DATABASE_HTML, .ptr = sim_database_html },
	{ .oid = "sim_monitor.html", .typ = OBJ_STATIC_HTML_GZ, .lvl = 255,
		.len = SIZEOF_SIM_MONITOR_HTML, .ptr = sim_monitor_html },
	{ .oid = "sim_diag.html", .typ = OBJ_STATIC_HTML_GZ, .lvl = 255,
		.len = SIZEOF_SIM_DIAG_HTML, .ptr = sim_diag_html },
	{ .oid = "sim_script.html", .typ = OBJ_STATIC_HTML_GZ, .lvl = 255,
		.len = SIZEOF_SIM_SCRIPT_HTML, .ptr = sim_script_html },
	{ .oid = "sim_firmware.html", .typ = OBJ_STATIC_HTML_GZ, .lvl = 255,
		.len = SIZEOF_SIM_FIRMWARE_HTML, .ptr = sim_firmware_html },
	{ .oid = "sim_debug.html", .typ = OBJ_STATIC_HTML_GZ, .lvl = 255,
		.len = SIZEOF_SIM_DEBUG_HTML, .ptr = sim_debug_html },
	{ .oid = "network.html", .typ = OBJ_STATIC_HTML_GZ, .lvl = 255,
		.len = SIZEOF_NETWORK_HTML, .ptr = network_html },
	{ .oid = "rpc_test.cgi", .typ = OBJ_CODE_CGI, .lvl = 100,
		.len = 0, .ptr = rpc_test_cgi },
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
    { .oid = "get_stdout.cgi", .typ = OBJ_CODE_CGI, .lvl = 100,
    		.len = 0, .ptr = get_stdout_cgi },
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
    { .oid = "sim_getstats.cgi", .typ = OBJ_CODE_CGI, .lvl = 100,
        .len = 0, .ptr = sim_getstats_cgi },
	{ .oid = NULL, .typ = 0, .lvl = 0, .len = 0, .ptr = NULL }
};

