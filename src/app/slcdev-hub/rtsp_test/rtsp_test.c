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
#include <sys/param.h>
#include <sys/stm32f.h>
#include <errno.h>

#include <tcpip/udp.h>
#include <tcpip/tcp.h>
#include <tcpip/ethif.h>
#include <tcpip/route.h>
#include <tcpip/loopif.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <thinkos.h>
#include <sys/console.h>

#include <trace.h>

#include "rtsp.h"

jitbuf_t * audio_init(void);

int rtp_task(struct rtp_session * rtp_s)
{
	struct ntp_time ntp;
	struct sockaddr_in sin;
	struct udp_pcb * rtp_udp;
	struct udp_pcb * rtcp_udp;
	uint32_t clk;
	uint32_t now;
	int32_t dt;
	int len;


	DBG("Thread: %d", thinkos_thread_self());
	DBG("RTP port: %d", rtp_s->lport[0]);
	DBG("RTCP port: %d", rtp_s->lport[1]);

	if ((rtp_udp = udp_alloc()) == NULL) {
		return -1;
	}

	if (udp_bind(rtp_udp, INADDR_ANY, htons(rtp_s->lport[0])) < 0) {
		return -1;
	}

	if ((rtcp_udp = udp_alloc()) == NULL) {
		return -1;
	}

	if (udp_bind(rtcp_udp, INADDR_ANY, htons(rtp_s->lport[1])) < 0) {
		return -1;
	}

	rtp_s->udp[0] = rtp_udp;
	rtp_s->udp[1] = rtcp_udp;

	ntp.sec = 2208988800u;
	ntp.frac = 0;

	clk = thinkos_clock();
	for (;;) {
		len = rtp_g711_recv(rtp_s, &sin);

		if (len < 0) {
			udp_close(rtp_s->udp[0]);
			return -1;
		}

		now = thinkos_clock();
		if ((dt = (int32_t)(now - clk)) > 5000) {
			clk = now;

			DBG("RTCP report");

		    ntp.sec += dt / 1000;
		    ntp.frac = (((uint64_t)(dt % 1000)) << 32) / 1000u;

			sin.sin_family = AF_INET;
			sin.sin_addr.s_addr = rtp_s->faddr;
			sin.sin_port = htons(rtp_s->fport[1]);
			rtcp_send_sr_sdes(rtp_s, 0, &ntp, &sin);
		}

	}	

	return 0;
}

uint32_t rtp_stack[1024];

const struct thinkos_thread_inf rtp_inf = {
	.stack_ptr = rtp_stack,
	.stack_size = sizeof(rtp_stack),
	.priority = 32,
	.thread_id = 8, 
	.paused = 0,
	.tag = "RTP"
};

void rtp_g711_start(struct rtp_session * rtp, struct jitbuf * jb)
{
	rtp->jb = jb;
	thinkos_thread_create_inf((void *)rtp_task, (void *)rtp, &rtp_inf);
}

void tcpip_init(void);

int network_config(void)
{
	struct ifnet * ifn;
	in_addr_t ip_addr;
	in_addr_t netmask = INADDR_ANY;
	in_addr_t gw_addr = INADDR_ANY;
	char s[64];
	char s1[16];
	char s2[16];
	char * env;
	uint8_t ethaddr[6] = { 0x1c, 0x95, 0x5d, 0x00, 0x00, 0x80};
	uint64_t esn;
	int dhcp;

	esn = *((uint64_t *)STM32F_UID);

	ethaddr[0] = ((esn >>  0) & 0xfc) | 0x02; /* Locally administered MAC */
	ethaddr[1] = ((esn >>  8) & 0xff);
	ethaddr[2] = ((esn >> 16) & 0xff);
	ethaddr[3] = ((esn >> 24) & 0xff);
	ethaddr[4] = ((esn >> 32) & 0xff);
	ethaddr[5] = ((esn >> 40) & 0xff);

	printf("* mac addr: %02x-%02x-%02x-%02x-%02x-%02x\n ", 
		   ethaddr[0], ethaddr[1], ethaddr[2],
		   ethaddr[3], ethaddr[4], ethaddr[5]);

	tcpip_init();

//	if ((env = getenv("IPCFG")) == NULL) {
	if (1) {
		printf("IPCFG not set, using defaults!\n");
		/* default configuration */
		strcpy(s, "192.168.10.128 255.255.255.0 192.168.10.254 0");
		/* set the default configuration */
		setenv("IPCFG", s, 1);
	} else {
		strcpy(s, env);
	}

	if (!inet_aton(strtok(s, " ,"), (struct in_addr *)&ip_addr)) {
		return -1;
	}

	if (inet_aton(strtok(NULL, " ,"), (struct in_addr *)&netmask)) {
		if (inet_aton(strtok(NULL, " ,"), (struct in_addr *)&gw_addr)) {
			dhcp = strtoul(strtok(NULL, ""), NULL, 0);
		}
	}

	/* initialize the Ethernet interface */
	/* configure the ip address */
	ifn = ethif_init(ethaddr, ip_addr, netmask);
//	ifn = loopif_init(ip_addr, netmask);

	ifn_getname(ifn, s);
	ifn_ipv4_get(ifn, &ip_addr, &netmask);
	printf("* netif %s: %s, %s\n", s, 
		   inet_ntop(AF_INET, (void *)&ip_addr, s1, 16),
		   inet_ntop(AF_INET, (void *)&netmask, s2, 16));

	if (gw_addr != INADDR_ANY) {
		/* add the default route (gateway) to ethif */
		ipv4_route_add(INADDR_ANY, INADDR_ANY, gw_addr, ifn);
		printf("* default route gw: %s\n", 
			   inet_ntop(AF_INET, (void *)&gw_addr, s1, 16));
	}

	if (dhcp) {
#if 0
		/* configure the initial ip address */
		dhcp_start();
		/* schedule the interface to be configured through dhcp */
		dhcp_ifconfig(ethif, dhcp_callback);
		printf("DHCP started.\n");
#endif
	}

	return 0;
}

void stdio_init(void)
{
	FILE * f;

	f = console_fopen();
	/* initialize STDIO */
	stderr = f;
	stdout = f;
	stdin = f;
}

int rtsp_connect(struct rtsp_client * rtsp, const char * host, const char * mrl)
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

const char * const trace_lvl_tab[] = {
		"   NONE",
		"  ERROR",
		"WARNING",
		"   INFO",
		"  DEBUG"
};

void __attribute__((noreturn)) supervisor_task(void)
{
	struct trace_entry ent;
	struct timeval tv;
	char s[80];

	INF("<%d> started...", thinkos_thread_self());

	trace_tail(&ent);

	for (;;) {
		thinkos_sleep(250);

		while (trace_getnext(&ent, s, sizeof(s)) >= 0) {
			trace_ts2time(&tv, ent.dt);
			printf("%s %2d.%06d: %s\n", trace_lvl_tab[ent.ref->lvl],
					(int)tv.tv_sec, (int)tv.tv_usec, s);
		}

		trace_flush(&ent);
	}
}


uint32_t supervisor_stack[128];

const struct thinkos_thread_inf supervisor_inf = {
	.stack_ptr = supervisor_stack,
	.stack_size = sizeof(supervisor_stack),
	.priority = 1,
	.thread_id = 1,
	.paused = false,
	.tag = "SUPV"
};

void supervisor_init(void)
{
	trace_init();

	thinkos_thread_create_inf((void *)supervisor_task, (void *)NULL,
							  &supervisor_inf);
}


volatile bool do_connect = true;

int main(int argc, char ** argv)
{
	struct rtsp_client rtsp;
	struct jitbuf * jb;

	stdio_init();

	supervisor_init();

	INF("Starting RTSP test");

	stm32f_nvram_env_init();

	network_config();

	jb = audio_init();

	thinkos_sleep(100);

	rtsp_init(&rtsp, 554);

	rtp_g711_start(&rtsp.rtp, jb);

	for (;;) {
		if (do_connect) {
			thinkos_sleep(1000);
			if (rtsp_connect(&rtsp, "192.168.10.254", "audio") < 0) {
				WARN("RTSP connection failed!");
			} else	{
				do_connect = false;
			}
		}
		thinkos_sleep(1000);
	}

	return 0;
}


