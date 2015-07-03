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

#include "rtsp.h"


int rtsp_request(struct rtsp_client * rtsp, const char * req, int len)
{
	struct tcp_pcb * tp = rtsp->tcp;
	int ret;

	if ((ret = tcp_send(tp, req, len, 0)) < 0)  {
		ERR("tcp_send() failed!");
		tcp_close(tp);
		return ret;
	}

	return ret;
}

void rtsp_decode_transport(struct rtsp_client * rtsp, char * s)
{
	char * cp;
	char * nam;
	char * val;
	int i;

	cp = (char *)s;
	i = 0;

	while (cp[i] >= ' ') {
		nam = &cp[i];
		for (; cp[i] >= ' '; ++i) {
			if (cp[i] == ';') {
				cp[i] = '\0';
				val = &cp[i++];
				break;
			}
			if (cp[i] == '=') {
				cp[i++] = '\0';
				val = &cp[i];
				while (cp[i] >= ' ') {
					if (cp[i] == ';') {
						cp[i++] = '\0';
						break;
					}
					i++;
				}
				break;
			}
		}

		if (strcmp(nam, "server_port") == 0) {
			rtsp->rtp.fport[0] = strtoul(val, &val, 10);
			if (*val == '-') {
				val++;
				rtsp->rtp.fport[1] = strtoul(val, NULL, 10);
			}
			DBG("server_port=%d-%d", rtsp->rtp.fport[0],  rtsp->rtp.fport[1]);
		} else if (strcmp(nam, "ssrc") == 0) {
			rtsp->rtp.ssrc = strtoul(val, NULL, 16);
			DBG("SSRC=%0x8", rtsp->rtp.ssrc);
		}
	}
}

int rtsp_wait_reply(struct rtsp_client * rtsp, int tmo)
{
	struct tcp_pcb * tp = rtsp->tcp;
	char * buf = rtsp->buf;
	int n;
	int i;
	int c1;
	int c2;
	int rem;
	int cnt;
	int ln;
	int pos;

	rem = RTSP_CLIENT_BUF_LEN; /* free space in the input buffer */
	cnt = 0; /* used space in the input buffer */
	ln = 0; /* line start */
	pos = 0; /* header position */
	c1 = '\0';

	/* receive and decode RTSP headers */
	while ((n = tcp_recv(tp, &buf[cnt], rem)) > 0)  {
		rem -= n;
		i = cnt;
		cnt += n;
		for (; i < cnt; ++i) {
			c2 = buf[i];
			if (c1 == '\r' && c2 == '\n') {
				char * val;
				unsigned int hdr;

				buf[i - 1] = '\0';
#if 0
				printf("%s\n", &buf[ln]);
#endif

				if (i == ln + 1) {
					DBG("end of RTSP header");
					i++;
					rtsp->cnt = cnt;
					rtsp->pos = i;
					rtsp->lin = i;
					return 0;
				}

				if ((hdr = rtsp_parse_hdr(&buf[ln], &val)) == 0) {
					WARN("invalid header field");
					return -1;
				}

				DBG("header field received: %s", rtsp_hdr_name[hdr]);
				if (pos == 0) {
					int code;

					if (hdr != HDR_RTSP_1_0) {
						WARN("invalid response");
						return -1;
					}
					if ((code = atoi(val)) != 200) {
						WARN("server response code %d", code);
						return -1;
					}
				} else {
					switch (hdr) {
					case HDR_CSEQ:
						if (atoi(val) != rtsp->cseq) {
							WARN("invalid CSeq");
							return -1;
						}
						break;

					case HDR_SESSION:
						rtsp->sid = strtoull(val, NULL, 16);
						break;

					case HDR_TRANSPORT:
						rtsp_decode_transport(rtsp, val);
						break;
					}

				}
				/* increment header counter */
				pos++;
				/* move to the next line */
				ln = i + 1;
			}
			c1 = c2;
		}

		if (ln != 0) {
			int j;

			for (i = 0, j = ln; j < cnt; ++i, ++j)
				buf[i] = buf[j];
			cnt = i;
			rem = RTSP_CLIENT_BUF_LEN - i;
			ln = 0;
		}

		if (rem <= 0) {
			ERR("buffer ovreflow!");
			return -1;
		}
	}

	tcp_close(tp);

	return -1;
}

int rtsp_recv(struct rtsp_client * rtsp, char * buf, int len, int tmo)
{
	char * src;
	int n;
	int i;

	/* read from RTSP internal buffer */
	if ((n = (rtsp->cnt -  rtsp->pos)) > 0) {
		if (n > len)
			n = len;
		src = &rtsp->buf[rtsp->pos];
		for (i = 0; i < n; ++i)
			buf[i] = src[i];

		rtsp->cnt += n;
		return n;
	}

	return tcp_recv(rtsp->tcp, buf, len);
}

int rtsp_line_recv(struct rtsp_client * rtsp, char * line,
		unsigned int len, unsigned int tmo)
{
	struct tcp_pcb * tp = rtsp->tcp;
	int rem;
	int cnt;
	int pos;
	int lin;
	int c1;
	int c2;
	int n;

	cnt = rtsp->cnt;
	pos = rtsp->pos;
	lin = rtsp->lin;
	c1 = (pos) ? rtsp->buf[pos - 1] : '\0';

	/* receive SDP payload */
	for (;;) {
		/* search for end of line */
		for (; pos < cnt; ++pos) {
			c2 = rtsp->buf[pos];
			if (c1 == '\r' && c2 == '\n') {
				char * dst = line;
				char * src = &rtsp->buf[lin];
				int i;

				n = pos - lin - 1;
				if (n > len)
					n = len;

				for (i = 0; i < n; ++i)
					dst[i] = src[i];

				/* move to the next line */
				lin = ++pos;
				rtsp->lin = lin;
				rtsp->pos = lin;
				return n;
			}
			c1 = c2;
		}

		if (RTSP_CLIENT_BUF_LEN == cnt) {
			int i;
			int j;

			if (lin == 0) {
				ERR("buffer ovreflow!");
				return -1;
			}

			n = cnt - lin;
			for (i = 0, j = lin; i < n; ++i, ++j)
				rtsp->buf[i] = rtsp->buf[j];

			cnt = n;
			pos = n;
			lin = 0;
		}

		rem = RTSP_CLIENT_BUF_LEN - cnt; /* free space in the input buffer */

		if ((n = tcp_recv(tp, &rtsp->buf[cnt], rem)) <= 0) {
			tcp_close(tp);
			return n;
		}

		cnt += n;
		rtsp->cnt = cnt;
	}

	return 0;
}

int rtsp_connect(struct rtsp_client * rtsp, const char * host,
		const char * mrl)
{
	struct tcp_pcb * tp;
	in_addr_t host_addr;
	char buf[512];
	int len;

	if (!inet_aton(host, (struct in_addr *)&host_addr)) {
		return -1;
	}

	if ((tp = tcp_alloc()) == NULL) {
		ERR("can't allocate socket!");
		return -1;
	}

	if (tcp_connect(tp, host_addr, htons(rtsp->port)) < 0) {
		ERR("can't connect to host!");
		tcp_close(tp);
		return -1;
	}

	rtsp->tcp = tp;
	rtsp->host_addr = host_addr;
	rtsp->rtp.faddr = host_addr;

	strcpy(rtsp->host_name, host);
	strcpy(rtsp->media_name, mrl);
	rtsp->cseq = 2;

	len = sprintf(buf,
			"OPTIONS rtsp://%s/%s RTSP/1.0\r\n"
			"CSeq: %d\r\n"
			"User-Agent: ThinkOS RTSP Client\r\n\r\n",
			rtsp->host_name, rtsp->media_name, rtsp->cseq);

	if (rtsp_request(rtsp, buf, len) < 0)
		return -1;

	if (rtsp_wait_reply(rtsp, 1000) < 0)
		return -1;

	rtsp->cseq++;
	len = sprintf(buf,
			"DESCRIBE rtsp://%s/%s RTSP/1.0\r\n"
			"CSeq: %d\r\n"
			"User-Agent: ThinkOS RTSP Client\r\n"
			"Accept: application/sdp\r\n"
			"\r\n",
			rtsp->host_name, rtsp->media_name, rtsp->cseq);

	if (rtsp_request(rtsp, buf, len) < 0)
		return -1;

	if (rtsp_wait_reply(rtsp, 1000) < 0)
		return -1;

	/* FIXME: decode SDP */
	while ((len = rtsp_line_recv(rtsp, buf, sizeof(buf), 1000)) > 0) {
		buf[len] = '\0';
		printf("'%s'\n", buf);
	}
	strcpy(rtsp->track_name, "microphone");

	if (len < 0) {
		ERR("rtsp_line_recv() failed!");
		return -1;
	}

	rtsp->cseq++;
	len = sprintf(buf,
			"SETUP rtsp://%s/%s/%s RTSP/1.0\r\n"
			"CSeq: %d\r\n"
			"User-Agent: ThinkOS RTSP Client\r\n"
			"Transport: RTP/AVP;unicast;client_port=%d-%d\r\n"
			"\r\n",
			rtsp->host_name, rtsp->media_name,
			rtsp->track_name, rtsp->cseq,
			rtsp->rtp.lport[0], rtsp->rtp.lport[1]);

	if (rtsp_request(rtsp, buf, len) < 0)
		return -1;

	if (rtsp_wait_reply(rtsp, 1000) < 0)
		return -1;

	rtsp->cseq++;
	len = sprintf(buf,
			"PLAY rtsp://%s/%s RTSP/1.0\r\n"
			"CSeq: %d\r\n"
			"User-Agent: ThinkOS RTSP Client\r\n"
			"Session: %016llx\r\n"
			"Range: ntp=0.000-\r\n"
			"\r\n",
			rtsp->host_name, rtsp->media_name,
			rtsp->cseq, rtsp->sid);

	if (rtsp_request(rtsp, buf, len) < 0)
		return -1;

	if (rtsp_wait_reply(rtsp, 1000) < 0)
		return -1;

//	rtp_client_start(&rtsp->rtp);

	return 0;
}

int rtsp_close_wait(struct rtsp_client * rtsp)
{
	int n;

	while ((n = tcp_recv(rtsp->tcp, rtsp->buf, RTSP_CLIENT_BUF_LEN)) > 0) {
		INF("RTSP recv: %d", n);
	}

	tcp_close(rtsp->tcp);

	return 0;
}

int rtsp_init(struct rtsp_client * rtsp, int port)
{
	memset(rtsp, 0, sizeof(struct rtsp_client));

	if (port == 0)
		port = 554;

	rtsp->port = port;
	rtsp->rtp.lport[0] = 6970;
	rtsp->rtp.lport[1] = 6971;
	rtsp->rtp.fport[0] = 0;
	rtsp->rtp.fport[1] = 0;

	return 0;
}
