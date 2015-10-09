/*
 * rpc_test.c
 *
 *  Created on: Sep 23, 2015
 *      Author: bmittmann
 */
#include <stdlib.h>
#include <stdio.h>
#include <simrpc.h>
#include <trace.h>
#include <hexdump.h>
#include <thinkos.h>
#include <sys/file.h>
#include <sys/null.h>
#include "profclk.h"
#include "www.h"

#define JSON_ERR_MAX 128

int rpc_json_error(struct httpctl * http, char * buf, int code, const char * msg)
{
	char * json = buf;
	int n;

   	if (msg == NULL) {
   	 	n = snprintf(json, JSON_ERR_MAX,
   	 			"{\"error\":{\"code\":%d,\"msg\":\"\"}}", code);
   	} else {
   		n = snprintf(json, JSON_ERR_MAX,
   				"{\"error\":{\"code\":%d,\"msg\":\"%s\"}}",
				code, msg);
   	}

   	httpd_200(http->tp, APPLICATION_JSON);

	return http_send(http, json, n);
}

int sim_getstats_cgi(struct httpctl * http)
{
	struct simrpc_stats stats;
	struct simrpc_pcb * sp;
	char * cp;
	unsigned int daddr;
	char s[HTML_MAX];
	int retcode;
	int port;
	int n;

    port = atoi(http_query_lookup(http, "port"));
	daddr = port;

	if ((sp = simrpc_open(daddr)) == NULL) {
		WARN("simrpc_open() failed");
		return rpc_json_error(http, s, -1, "simrpc_open() failed");
	}

	simrpc_set_timeout(sp, 50);
	retcode = simrpc_stats_get(sp, &stats);
	simrpc_close(sp);

	if (retcode < 0) {
		WARN("simrpc_stats_get() failed");
		return rpc_json_error(http, s, retcode, "simrpc_stats_get() failed");
	}

   	httpd_200(http->tp, APPLICATION_JSON);

   	cp = s;
	cp += sprintf(cp, "{\"uptime\":\"%d\",\"db_ok\":\"%s\",\"cfg_ok\":\"%s\",",
				  stats.uptime, stats.db_ok ? "true" : "false", 
				  stats.cfg_ok ? "true" : "false");

	cp += sprintf(cp, "\"stats\":{"
				  "\"proto_err\":%d,"
				  "\"clip_poll\":%d,"
				  "\"clip_err\":%d,"
				  "\"clip_resp\":%d,"
				  "\"clip_pw5\":%d,"
				  "\"ap_poll\":%d,"
				  "\"ap_herr\":%d,"
				  "\"ap_null\":%d,"
				  "\"ap_ralt\":%d,"
				  "\"ap_rtlt\":%d,"
				  "\"ap_rtltsb\":%d,"
				  "\"ap_rp\":%d,"
				  "\"ap_ralu\":%d,"
				  "\"ap_rtlu\":%d,"
				  "\"ap_rpsb\":%d,"
				  "\"ap_rtlusb\":%d,"
				  "\"ap_uerr\":%d}}",
				  stats.dev.proto_err,
				  stats.dev.clip_poll,
				  stats.dev.clip_err,
				  stats.dev.clip_resp,
				  stats.dev.clip_pw5,
				  stats.dev.ap_poll,
				  stats.dev.ap_herr,
				  stats.dev.ap_null,
				  stats.dev.ap_ralt,
				  stats.dev.ap_rtlt,
				  stats.dev.ap_rtltsb,
				  stats.dev.ap_rp,
				  stats.dev.ap_ralu,
				  stats.dev.ap_rtlu,
				  stats.dev.ap_rpsb,
				  stats.dev.ap_rtlusb,
				  stats.dev.ap_uerr);

	n = cp - s;

	return http_send(http, s, n);
}

int sim_get_os_state_cgi(struct httpctl * http)
{
	struct simrpc_threadinfo thread;
	struct simrpc_exceptinfo except;
	struct simrpc_pcb * sp;
	char * cp;
	unsigned int daddr;
	char s[HTML_MAX];
	int retcode;
	int port;
	int n;
	int i;
	int j;

    port = atoi(http_query_lookup(http, "port"));
	daddr = port;

	INF("OS state, port=%d", port);

	if ((sp = simrpc_open(daddr)) == NULL) {
		WARN("simrpc_open() failed");
		return rpc_json_error(http, s, -1, "simrpc_open() failed");
	}

	simrpc_set_timeout(sp, 50);
	retcode = simrpc_exceptinfo_get(sp, &except);
	if (retcode < 0) {
		WARN("simrpc_exceptinfo_get() failed");
		rpc_json_error(http, s, retcode, "simrpc_exceptinfo_get() failed");
		simrpc_close(sp);
		return -1;
	}

   	cp = s;
	cp += sprintf(cp, "{\"except\":{\"ctx\":[");
	for (j = 0; j <= 12; ++j)
		cp += sprintf(cp, "%d,", except.ctx.r[j]);
	cp += sprintf(cp, "%d,%d,%d,%d]}",
			except.ctx.sp, except.ctx.lr, except.ctx.pc, except.ctx.xpsr);

	n = 0;
	for (i = 0; i < 4; ++i) {
		retcode = simrpc_threadinfo_get(sp, i, &thread);
		if (retcode < 0) {
			WARN("simrpc_threadinfo_get() failed");
			if (retcode < -2) {
				simrpc_close(sp);
				rpc_json_error(http, s, retcode, "simrpc_threadinfo_get() failed");
				return -1;
			}
		} else {
			if (n == 0)
				cp += sprintf(cp, ",\"threads\":[{\"id\":%d,\"ctx\":[", i);
			else
				cp += sprintf(cp, ",{\"id\":%d,\"ctx\":[", i);
			for (j = 0; j <= 12; ++j)
				cp += sprintf(cp, "%d,", thread.ctx.r[j]);
			cp += sprintf(cp, "%d,%d,%d,%d]}",
				thread.ctx.sp, thread.ctx.lr, thread.ctx.pc, thread.ctx.xpsr);
			n++;
		}
	}
	if (n > 0) {
		cp += sprintf(cp, "]}");
	} else {
		cp += sprintf(cp, "}");
	}
	simrpc_close(sp);

	n = cp - s;
   	httpd_200(http->tp, APPLICATION_JSON);
	return http_send(http, s, n);
}
