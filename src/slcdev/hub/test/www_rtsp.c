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
 * @file webserver.c
 * @brief YARD-ICE application main
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <thinkos.h>
#include <trace.h>
#include "www.h"

/*---------------------------------------------------------------------------
  Dynamic content
  ---------------------------------------------------------------------------*/

const char index_hdr_html[] = DOCTYPE_HTML "<head>\r\n"
	"<title>ThinkOS RTSP/RTP Demo</title>\r\n"
	META_HTTP META_COPY LINK_ICON LINK_CSS
	"</head>\r\n<body>\r\n"
	"<h1>ThinkOS RTSP/RTP Demo</h1>\r\n";

const char play_hdr_html[] =
	"<p>Please fill the infomation and press <b>Play</b>.</p>\r\n"
	"<form action=\"play.cgi\" method=\"POST\"><table  width=\"400\">"
	"<col width=\"40%\"><col width=\"60%\">"
	"\r\n";

const char play_foot_html[] =
	"</table><br>\r\n"
	"<input type=\"submit\" value=\"Play\">\r\n"
	"</form>\r\n";

const char stop_hdr_html[] =
	"<form action=\"stop.cgi\" method=\"POST\">\r\n";

const char stop_foot_html[] =
	"<br>\r\n"
	"<input type=\"submit\" value=\"Stop\">\r\n"
	"</form>\r\n";

const char rtsp_iframe_html[] =
	"<iframe src=\"rtsp_msg.cgi\""
	" width=\"100%\""
	"></iframe>\r\n";

char rtsp_host[128] = "192.168.10.254";
char rtsp_media[128] = "music";
uint16_t rtsp_port = 5544;
uint16_t audio_level = 100;

int send_play_form(struct httpctl * ctl)
{
	char s[HTML_MAX];
	int n;

	httpd_200(ctl->tp, TEXT_HTML);
	http_send(ctl, index_hdr_html, sizeof(index_hdr_html) - 1);
	http_send(ctl, play_hdr_html, sizeof(play_hdr_html) - 1);

	n = snprintf(s, HTML_MAX, "<tr><td align=\"right\">RTSP server:</td><td>"
			"<input type=\"text\" name=\"host\"  size=\"32\" maxlength=\"128\" value=\"%s\"></td></tr>\r\n",
			rtsp_host);
	http_send(ctl, s, n);

	n = snprintf(s, HTML_MAX, "<tr><td align=\"right\">TCP Port:</td><td>"
			"<input type=\"text\" name=\"port\" size=\"5\" maxlength=\"5\" value=\"%d\"></td></tr>\r\n",
			rtsp_port);
	http_send(ctl, s, n);

	n = snprintf(s, HTML_MAX, "<tr><td align=\"right\">Media (MRL):</td><td>"
			"<input type=\"text\" name=\"mrl\" size=\"32\" maxlength=\"128\" value=\"%s\"></td></tr>\r\n",
			rtsp_media);
	http_send(ctl, s, n);

	n = snprintf(s, HTML_MAX, "<tr><td align=\"right\">Volume (0..200):</td><td>"
			"<input type=\"text\" name=\"vol\" size=\"4\" maxlength=\"3\" value=\"%d\"></td></tr>\r\n",
			audio_level);
	http_send(ctl, s, n);

	return http_send(ctl, play_foot_html, sizeof(play_foot_html) - 1);
}

int send_stop_form(struct httpctl * ctl)
{
/*	char * host;
	char * mrl;
	char * track;
	int port;
	char s[S_MAX];
	int n;

	host = rtsp_host_name(&rtsp);
	mrl = rtsp_media_name(&rtsp);
	track = rtsp_track_name(&rtsp);
	port = rtsp_port_get(&rtsp);

	httpd_200(ctl->tp, TEXT_HTML);
	http_send(ctl, index_hdr_html, sizeof(index_hdr_html) - 1);
	http_send(ctl, stop_hdr_html, sizeof(stop_hdr_html) - 1);

	n = snprintf(s, S_MAX, "<p><b>Playing</b> RTSP://%s:%d/%s/%s</p>\r\n",
			host, port, mrl, track);
	http_send(ctl, s, n);
*/
	return http_send(ctl, stop_foot_html, sizeof(stop_foot_html) - 1);
}

int send_error(struct httpctl * ctl, char * msg)
{
	char s[HTML_MAX];
	int n;
	n = snprintf(s, HTML_MAX, "<p><b>Error:</b> %s!</p>\r\n", msg);
	return http_send(ctl, s, n);
}

int index_cgi(struct httpctl * ctl)
{
	send_play_form(ctl);
	return http_send(ctl, footer_html, sizeof_footer_html);
}

int play_cgi(struct httpctl * ctl)
{
	strcpy(rtsp_host, http_query_lookup(ctl, "host"));
	strcpy(rtsp_media, http_query_lookup(ctl, "mrl"));
	rtsp_port = atoi(http_query_lookup(ctl, "port"));
	audio_level = atoi(http_query_lookup(ctl, "vol"));
	if (audio_level > 800)
		audio_level = 800;
#if 0
	if (rtsp_connect(&rtsp, rtsp_host, rtsp_port, rtsp_media) < 0) {
		int code;

		WARN("RTSP connection failed!");
		send_play_form(ctl);
		send_error(ctl, "RTSP playback failed");
		if ((code = rtsp_response_code(&rtsp)) > 200) {
			http_send(ctl, rtsp_iframe_html, sizeof(rtsp_iframe_html) - 1);
		}
	} else {
		audio_gain_set(audio_level * Q15(1.0) / 100);
		send_stop_form(ctl);
	}
#endif
	return http_send(ctl, footer_html, sizeof_footer_html);
}

int stop_cgi(struct httpctl * ctl)
{
#if 0
	if (rtsp_teardown(&rtsp) < 0) {
		WARN("RTSP teardown failed!");
	}
#endif
	send_play_form(ctl);
	return http_send(ctl, footer_html, sizeof_footer_html);
}


/*---------------------------------------------------------------------------
  directory content
  ---------------------------------------------------------------------------*/

struct httpdobj www_rtsp[] = {
	{ .oid = "play.cgi", .typ = OBJ_CODE_CGI, .lvl = 100,
		.len = 0, .ptr = play_cgi },
	{ .oid = "stop.cgi", .typ = OBJ_CODE_CGI, .lvl = 100,
		.len = 0, .ptr = stop_cgi },
	{ .oid = NULL, .typ = 0, .lvl = 0, .len = 0, .ptr = NULL }
};

