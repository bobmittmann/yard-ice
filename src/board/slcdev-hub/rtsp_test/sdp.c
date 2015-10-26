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
 * but WITHOUT ANY WARRANTY; without even the implied warranty of * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You can receive a copy of the GNU Lesser General Public License from 
 * http://www.gnu.org/
 */

/** 
 * @file rtp_test.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <trace.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "rtsp.h"

static int sdp_parse_attribute(struct rtsp_client * rtsp, char * s, int len)
{
	if (strncmp(s, "control", 7) == 0) {
		char * cp = s + len;
		int c;
		while (cp != s) {
			c = *(cp - 1);
			if (c == '/')
				break;
			cp--;
		}

		strncpy(rtsp->track_name, cp, RTSP_TRACK_NAME_MAX);
		rtsp->track_name[RTSP_TRACK_NAME_MAX] = '\0';

		DBG("Control: \"%s\"", s);
	}

	if (strncmp(s, "rtpmap", 6) == 0) {
		unsigned int fmt;
		char * codec;
		(void)fmt;

		fmt = strtoul(strtok(&s[6], " "), NULL, 10);
		codec = strtok(NULL, " ");

		if (strcmp(codec, "PCMA/11025") != 0) {
			DBG("Invalid codec: \"%s\"", codec);
			return -1;
		}

		DBG("Rtpmap: %d %s", fmt, codec);
	}
	return 0;
}

static int sdp_parse_media(struct rtsp_client * rtsp, char * s, int len)
{
	if (strncmp(s, "audio", 5) == 0) {
		char * port;
		char * proto;
		unsigned int fmt;

		port = strtok(&s[5], " ");
		proto = strtok(NULL, " ");
		fmt = strtoul(strtok(NULL, " "), NULL, 10);

		if (strcmp(proto, "RTP/AVP") != 0) {
			DBG("Invalid media protocol: \"%s\"", proto);
			return -1;
		}

		if (strcmp(port, "0") != 0) {
			DBG("Invalid media port: \"%s\"", port);
			return -1;
		}

		rtsp->media.fmt = fmt;

		DBG("Media: \"%s %s %d\"", port, proto, rtsp->media.fmt);
	}
	return 0;
}

int rtsp_sdp_decode(struct rtsp_client * rtsp)
{
	char buf[256];
	int len = 0;
	int rec = 0;

	/* decode SDP */
	while ((len = rtsp_line_recv(rtsp, buf, sizeof(buf) - 1)) > 1) {

		if (buf[1] != '=') {
			break;
		}

		buf[len] = '\0';
		DBG("\"%s\"", buf);

		switch (buf[0]) {
		case 's': /* Session description */
			rec = 's';
			break;
		case 't': /* Time description */
			rec = 't';
			break;
		case 'm': /* Media description */
			if (sdp_parse_media(rtsp, &buf[2], len - 2) < 0)
				return -1;
			rec = 'm';
			break;
		case 'a': /* Attribute */
			if (sdp_parse_attribute(rtsp, &buf[2], len - 2) < 0)
				return -1;
		}

		(void)rec;
	}

	if (len < 0) {
		ERR("rtsp_line_recv() failed!");
		return -1;
	}

	return 0;
}
