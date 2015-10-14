#ifndef __WWW_H__
#define __WWW_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <tcpip/httpd.h>

/*---------------------------------------------------------------------------
  Common HTML macros
  ---------------------------------------------------------------------------*/

#define HTML_FOOTER "<hr>\r\n"\
	"<a href=\"https://https://github.com/bobmittmann/yard-ice\">"\
	"<img src=\"/img/thinkos47x20.png\">"\
	"</a> - Cortex-M Operating System - \r\n"\
	"<i><a href=\"https://https://github.com/bobmittmann/yard-ice\">"\
	"YARD-ICE</a></i><br>\r\n"\
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

#define LINK_MODALBOX "<script type=\"text/javascript\" "\
		"src=\"/lib/modalbox.js.gz\"></script>\r\n"

#define LINK_ZEPTO "<script type=\"text/javascript\" "\
		"src=\"/lib/zepto.js.gz\"></script>\r\n"

#define LINK_ICON "<link rel=\"shortcut icon\" "\
	"href=\"/img/thinkos16x16.png\"/>\r\n"

#define LINK_CSS "<link rel=\"stylesheet\" href=\"/style.css\" "\
	"type=\"text/css\" media=\"screen\"/>\r\n"

#define HTML_MAX 1024 /* HTML formatting string buffer length */

extern const char footer_html[];
extern const uint16_t sizeof_footer_html;

extern struct httpdobj www_root[];
extern struct httpdobj www_ipcfg[];
extern struct httpdobj www_lib[];
extern struct httpdobj www_img[];


/*---------------------------------------------------------------------------
  Automatically generated code
  ---------------------------------------------------------------------------*/


/* --- bin2hex.py: file size --- */

#define SIZEOF_SIMRPC_CLNT_C 1823

#define SIZEOF_SIM_DEBUG_HTML 1780

#define SIZEOF_JQUERY_2_1_4_MIN_JS 29519

#define SIZEOF_ZEPTO_MIN_JS 9377

#define SIZEOF_SIMLIB_JS 1113

#define SIZEOF_MODALBOX_JS 1430

#define SIZEOF_LAYOUT_CSS 790

#define SIZEOF_SIM_TEST_HTML 2263

#define SIZEOF_SIM_SCRIPT_HTML 1975

#define SIZEOF_SIM_FIRMWARE_HTML 1683

#define SIZEOF_SIM_DIAG_HTML 1960

#define SIZEOF_INDEX_HTML 1471

#define SIZEOF_SIM_MONITOR_HTML 1676

#define SIZEOF_WWW_RPCTEST_C 3000

#define SIZEOF_SIM_TOOLS_HTML 1471

#define SIZEOF_SIM_DATABASE_HTML 1944

#define SIZEOF_SIM_CONFIG_HTML 1976

#define SIZEOF_RPC_TEST_HTML 2495

#define SIZEOF_NETWORK_HTML 1111

#define SIZEOF_STYLE_CSS 1029

#define SIZEOF_TOOLS_HTML 1431


/* --- bin.hex.py: file content --- */

extern const uint8_t simrpc_clnt_c[];

extern const uint8_t sim_debug_html[];

extern const uint8_t jquery_2_1_4_min_js[];

extern const uint8_t zepto_min_js[];

extern const uint8_t simlib_js[];

extern const uint8_t modalbox_js[];

extern const uint8_t layout_css[];

extern const uint8_t sim_test_html[];

extern const uint8_t sim_script_html[];

extern const uint8_t sim_firmware_html[];

extern const uint8_t sim_diag_html[];

extern const uint8_t index_html[];

extern const uint8_t sim_monitor_html[];

extern const uint8_t www_rpctest_c[];

extern const uint8_t sim_tools_html[];

extern const uint8_t sim_database_html[];

extern const uint8_t sim_config_html[];

extern const uint8_t rpc_test_html[];

extern const uint8_t network_html[];

extern const uint8_t style_css[];

extern const uint8_t tools_html[];


#ifdef __cplusplus
extern "C" {
#endif

int rpc_test_cgi(struct httpctl * http);
int rpc_exec_cgi(struct httpctl * http);
int rpc_js_cgi(struct httpctl * http);

int cfg_load_cgi(struct httpctl * http);
int cfg_compile_cgi(struct httpctl * http);
int cfg_getinfo_cgi(struct httpctl * http);

int db_load_cgi(struct httpctl * http);
int db_compile_cgi(struct httpctl * http);
int db_getinfo_cgi(struct httpctl * http);

int firmware_load_cgi(struct httpctl * http);
int get_status_cgi(struct httpctl * http);
int file_write_cgi(struct httpctl * http);
int file_read_cgi(struct httpctl * http);

int sim_getattr_cgi(struct httpctl * http);

int get_stdout_cgi(struct httpctl * http);

int rpc_json_error(struct httpctl * http, char * buf, int code, const char * msg);
int sim_getstats_cgi(struct httpctl * http);
int sim_get_os_state_cgi(struct httpctl * http);
int sim_microjs_ctl_cgi(struct httpctl * http);

#ifdef __cplusplus
}
#endif

#endif



