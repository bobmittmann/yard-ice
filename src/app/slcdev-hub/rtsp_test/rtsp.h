/* 
 *
 * This file is part of the libtcpip.
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
 * @file rtsp.h
 * @brief 
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __RTSP_H__
#define __RTSP_H__

#include <tcpip/tcp.h>
#include "rtsp_hdr.h"
#include "rtp.h"

#define RTSP_HOST_NAME_MAX 63
#define RTSP_MEDIA_NAME_MAX 127
#define RTSP_TRACK_NAME_MAX 63

#define RTSP_CLIENT_BUF_LEN 128

struct rtsp_client {
	struct rtp_session rtp;
	struct tcp_pcb * tcp;
	uint32_t cseq;
	in_addr_t host_addr;
	uint16_t svr_port[2];
	uint16_t port;
	uint16_t cnt;
	uint16_t pos;
	uint16_t lin;
	uint64_t sid;
	struct {
		uint16_t code;
		uint32_t seq;
		uint32_t content_pos;
		uint32_t content_len;
	} resp;
	struct {
		uint8_t fmt;
	} media;
	char buf[RTSP_CLIENT_BUF_LEN];
	char host_name[RTSP_HOST_NAME_MAX + 1];
	char media_name[RTSP_MEDIA_NAME_MAX + 1];
	char track_name[RTSP_TRACK_NAME_MAX + 1];
};

#ifdef __cplusplus
extern "C" {
#endif


int rtsp_request(struct rtsp_client * rtsp, const char * req, int len);

int rtsp_wait_reply(struct rtsp_client * rtsp, int tmo);

int rtsp_recv(struct rtsp_client * rtsp, char * buf, int len);

int rtsp_line_recv(struct rtsp_client * rtsp, char * line,
		unsigned int len);

int rtsp_connect(struct rtsp_client * rtsp, const char * host,
		unsigned int port, const char * mrl);

int rtsp_teardown(struct rtsp_client * rtsp);

int rtsp_close_wait(struct rtsp_client * rtsp);

int rtsp_init(struct rtsp_client * rtsp, int port);

char * rtsp_track_name(struct rtsp_client * rtsp);

char * rtsp_media_name(struct rtsp_client * rtsp);

char * rtsp_host_name(struct rtsp_client * rtsp);

int rtsp_response_code(struct rtsp_client * rstp);

unsigned int rtsp_port_get(struct rtsp_client * rtsp);

int rtsp_sdp_decode(struct rtsp_client * rtsp);

#ifdef __cplusplus
}
#endif

#endif /* __RTSP_H__ */

