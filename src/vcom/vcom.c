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
 * @file .c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <thinkos.h>

#include <netinet/in.h>
#include <tcpip/tcp.h>

#include <sys/dcclog.h>
#include <sys/serial.h>

#include <sys/stm32f.h>
#include <trace.h>

#include <arpa/telnet.h>

#include "rfc2217.h"

/* CPC Serial Default Values */
#define CPC_DEF_BAUD	9600
#define CPC_DEF_DATAB	8
#define CPC_DEF_PARITY	'N'
#define CPC_DEF_STOPB	1

/* TELNET flags */
#define CPC_BINARY       0x01
#define CPC_ECHO         0x02

/* CPC states */
#define CPC_DATA         0
#define CPC_IAC_RCVD     1
#define CPC_DONT_RCVD    2
#define CPC_DO_RCVD      3
#define CPC_WONT_RCVD    4
#define CPC_WILL_RCVD    5
#define CPC_SUBOPTION_ID 6
#define CPC_SUBOPTION    7
#define CPC_SB_IAC_RCVD  8
#define CPC_INVALID_SUBOPTION	11
#define CPC_INVALID_SB_IAC_RCVD	12

/* Some TELNET options */
#define CPC_OPT_BINARY   0x01
#define CPC_OPT_ECHO     0x02
#define CPC_OPT_RCP      0x04
#define CPC_OPT_SGA      0x08
#define CPC_OPT_NAMS     0x10
#define CPC_OPT_STATUS   0x20
#define CPC_OPT_TM       0x40
#define CPC_OPT_RCTE     0x80

char cpc2par(int par)
{
	switch (par) {
		case 1: return 'N';
		case 2: return 'O';
		case 3: return 'E';
		case 4: return 'M';
		case 5: return 'S';
		default: return 'N';
	}
}

int par2cpc(char cpc)
{
	switch (cpc) {
		case 'N': return 1;
		case 'O': return 2;
		case 'E': return 3;
		case 'M': return 4;
		case 'S': return 5;
		default: return 1;
	}
}

struct serial_dev * serial_open(void);

struct vcom {
	struct serial_dev * serial;
	struct tcp_pcb * svc;
	struct tcp_pcb * volatile tp;
	int baud;
	int datab;
	int stopb;
	int parity;
};

struct tn_opt {
	uint32_t local; /* local options */
	uint32_t remote; /* remote options */
};

int cpc_opt_will(struct tcp_pcb * tp, struct tn_opt * t, int opt)
{
	uint8_t buf[4];

	if (opt > 31) {
		DCC_LOG1(LOG_WARNING, "opt %02x out of range", opt);
		return -1;
	}

	if (t->local & (1 << opt))
		return 0;

	t->local |= 1 << opt;

	buf[0] = IAC;
	buf[1] = WILL;
	buf[2] = opt;

	return tcp_send(tp, buf, 3, TCP_SEND_NOWAIT);
}

int cpc_opt_do(struct tcp_pcb * tp, struct tn_opt * t, int opt)
{
	uint8_t buf[4];

	if (opt > 31) {
		DCC_LOG1(LOG_WARNING, "opt %02x out of range", opt);
		return -1;
	}

	if (t->remote & (1 << opt))
		return 0;
	t->remote |= 1 << opt;

	buf[0] = IAC;
	buf[1] = DO;
	buf[2] = opt;

	return tcp_send(tp, buf, 3, TCP_SEND_NOWAIT);
}

int  cpc_opt_wont(struct tcp_pcb * tp, struct tn_opt * t, int opt)
{
	uint8_t buf[4];

	if (opt > 31) {
		DCC_LOG1(LOG_WARNING, "opt %02x out of range", opt);
		return -1;
	}

	if (!(t->local & (1 << opt)))
		return 0;
		
	t->local &= ~(1 << opt);

	buf[0] = IAC;
	buf[1] = WONT;
	buf[2] = opt;

	return tcp_send(tp, buf, 3, TCP_SEND_NOWAIT);
}

int  cpc_opt_dont(struct tcp_pcb * tp, struct tn_opt * t, int opt)
{
	uint8_t buf[4];

	if (opt > 31) {
		DCC_LOG1(LOG_WARNING, "opt %02x out of range", opt);
		return -1;
	}

	if (!(t->remote & (1 << opt)))
		return 0;

	buf[0] = IAC;
	buf[1] = DONT;
	buf[2] = opt;

	return tcp_send(tp, buf, 3, TCP_SEND_NOWAIT);
}

void cpc_opt_clr(struct tn_opt * t)
{
	t->local = 0;
	t->remote = 0;
}

int __attribute__((noreturn)) tcp_input_task(struct vcom * vcom)
{
	struct serial_dev * serial = vcom->serial;
	struct tcp_pcb * svc;
	struct tcp_pcb * tp;
	char buf[128];
	int len;
	int ret;
	char * src;
	int rem;
	char * dst = buf;
	int cnt;
	int binary;
	int state;
	int c;
	struct tn_opt opt;

	svc = vcom->svc;

	for (;;) {

		INF("VCOM/TCP wating for incomming connections...");

		DCC_LOG(LOG_TRACE, "VCOM/TCP wating for connection.");
		if ((tp = tcp_accept(svc)) == NULL) {
			DCC_LOG(LOG_ERROR, "tcp_accept().");
			break;
		}

		DCC_LOG(LOG_TRACE, "VCOM/TCP connection accepted.");

		INF("VCOM/TCP connection accepted.");

		vcom->tp  = tp;

		cpc_opt_clr(&opt);
		cnt = 0;
		binary = 0;
		state = CPC_DATA;
		cpc_opt_will(tp, &opt, TELOPT_SGA);
		cpc_opt_do(tp, &opt, TELOPT_SGA);
		cpc_opt_will(tp, &opt, TELOPT_ECHO);
		cpc_opt_will(tp, &opt, TELOPT_BINARY);
		cpc_opt_do(tp, &opt, TELOPT_BINARY);

		for (;;) {
			if (cnt) {
				/* flush input buffer */
				if ((ret = serial_send(serial, buf, cnt)) <= 0) {
					DCC_LOG(LOG_ERROR, "serial_send() failed!");
					break;
				}
				/* reset output data pointer */
				dst = buf;
				/* clear output byte count */
				cnt = 0;
			}

			/* receive data form network */
			if ((len = tcp_recv(tp, buf, 128)) <= 0) {
				DCC_LOG1(LOG_WARNING, "tcp_recv(): %d", len);
				break;
			}

			DCC_LOG1(LOG_MSG, "recv: %d", len);

			/* set the input processing pointer */
			src = buf;
			/* input remaining (to be processed) bytes */
			rem = len;

			while (rem > 0) {
				c = *src++;
				rem--;

				if (state == CPC_DATA) {
					if (c == IAC) {
						state = CPC_IAC_RCVD;
					} else {
						if ((binary) || ((c >= 3) && (c < 127))) {
							/* ascii characters */
							*dst++ = c;
							cnt++;
						}
					}
					continue;
				}

				/* handles TELNET inputs options */
				switch (state) {

				case CPC_IAC_RCVD:
					switch (c) {
					case IAC:
						state = CPC_DATA;
						break;
					case DONT:
						state = CPC_DONT_RCVD;
						break;
					case DO:
						state = CPC_DO_RCVD;
						break;
					case WONT:
						state = CPC_WONT_RCVD;
						break;
					case WILL:
						state = CPC_WILL_RCVD;
						break;
					case SB:
						state = CPC_SUBOPTION_ID;
						break;
					case EL:
					case EC:
					case AYT:
					case AO:
					case IP:
					case BREAK:
					case DM:
					case NOP:
					case SE:
					case EOR:
					case ABORT:
					case SUSP:
					case xEOF:
					default:
						state = CPC_DATA;
						break;
					}
					break;

				case CPC_DONT_RCVD:
					DCC_LOG1(LOG_MSG, "DONT %s", TELOPT(c));
					cpc_opt_wont(tp, &opt, c);
					state = CPC_DATA;
					break;

				case CPC_DO_RCVD:
					DCC_LOG1(LOG_MSG, "DO %s", TELOPT(c));
					switch (c) {

					case TELOPT_SGA:
						cpc_opt_will(tp, &opt, c);
						break;

					case TELOPT_ECHO:
						cpc_opt_will(tp, &opt, c);
						break;

					case TELOPT_BINARY:
						cpc_opt_will(tp, &opt, c);
						break;

					default:
						cpc_opt_wont(tp, &opt, c);
					}

					state = CPC_DATA;
					break;

				case CPC_WONT_RCVD:
					DCC_LOG1(LOG_MSG, "WONT %s", TELOPT(c));
					cpc_opt_dont(tp, &opt, c);
					state = CPC_DATA;
					break;

				case CPC_WILL_RCVD:
					DCC_LOG1(LOG_MSG, "WILL %s", TELOPT(c));

					switch (c) {
					case TELOPT_ECHO:
						cpc_opt_dont(tp, &opt, c);
						break;

					case TELOPT_SGA:
						cpc_opt_do(tp, &opt, c);
						break;

					case TELOPT_BINARY:
						cpc_opt_do(tp, &opt, c);
						binary = 1;
						break;

					default:
						cpc_opt_dont(tp, &opt, c);
					}

					state = CPC_DATA;
					break;

				case CPC_SUBOPTION_ID:
					if (c == COM_PORT_OPTION) {
						state = CPC_SUBOPTION;
					} else {
						state = CPC_INVALID_SUBOPTION;
						DCC_LOG1(LOG_WARNING, "invalid suboption: %d.", c);
					}
					break;

				case CPC_SUBOPTION:
					if (c == IAC)
						state = CPC_SB_IAC_RCVD;
					break;

				case CPC_SB_IAC_RCVD:
					if (c == SE) {
						state = CPC_DATA;
					} else {
						state = CPC_SUBOPTION;
					}
					break;

				case CPC_INVALID_SUBOPTION:
					if (c == IAC)
						state = CPC_INVALID_SB_IAC_RCVD;
					break;

				case CPC_INVALID_SB_IAC_RCVD:
					if (c == SE)
						state = CPC_DATA;
					else
						state = CPC_INVALID_SUBOPTION;
					break;

				default:
					DCC_LOG1(LOG_WARNING, "invalid state: %d!!", state);
					break;
				}

			}

		}

		DCC_LOG(LOG_TRACE, "close...");

		tcp_close(tp);
		vcom->tp  = NULL;

		INF("VCOM connection closed.");
	}

	DCC_LOG(LOG_ERROR, "thread loop break!!!");
	for(;;);
}

int vcom_write(struct tcp_pcb * tp, const void * buf, int len)
{
	char * cp;
	int cnt;
	int rem;
	int pos;

	DCC_LOG1(LOG_TRACE, "len=%d", len);

	cp = (char *)buf;
	rem = len;
	cnt = 0;

	/* search for IAC */
	for (pos = 0; (pos < rem) && (cp[pos] != IAC); pos++);

	while (pos < rem) {
		if (cp[pos] == IAC) {
			/* send data until IAC, inclusive */
			if (tcp_send(tp, cp, pos + 1, 0) <= 0)
				return cnt;
		} 
		cnt += pos + 1;
		cp += pos;
		rem -= pos;
		pos = 1;
		/* search for next IAC */
		for (; (pos < rem) && (cp[pos] != IAC); pos++);
	};

	/* send the last chunk */
	if (pos) {
		if (tcp_send(tp, cp, pos, TCP_SEND_NOWAIT) <= 0)
			return cnt;
	}

	cnt += pos;

	return cnt;
}


#define VCOM_BUF_SIZE 128

int __attribute__((noreturn)) serial_input_task(struct vcom * vcom)
{
	struct serial_dev * serial = vcom->serial;
	char buf[VCOM_BUF_SIZE];
	int len;
	int ret;

	DCC_LOG1(LOG_TRACE, "<%d> started...", thinkos_thread_self());

	for (;;) {
		if ((len = serial_send(serial, buf, VCOM_BUF_SIZE)) < 0) {
			DCC_LOG(LOG_ERROR, "serial_read(): failed!");
			continue;
		}

		if (len == 0) {
			DCC_LOG(LOG_TRACE, "timeout");
			continue;
		}

		if (vcom->tp == NULL)
			continue;

		/* FIXME: convert IAC to IAC, IAC */
		if ((ret = vcom_write(vcom->tp, buf, len)) <= 0) {
			DCC_LOG1(LOG_WARNING, "tcp_send(): %d", ret);
			continue;
		}
		DCC_LOG1(LOG_MSG, "send: %d", ret);
	}

	for (;;);
}

uint32_t tcp_input_stack[128];
uint32_t serial_input_stack[128];

struct vcom serial_vcom;

const struct thinkos_thread_inf tcp_input_inf = {
	.stack_ptr = tcp_input_stack, 
	.stack_size = sizeof(tcp_input_stack),
	.priority = 32,
	.thread_id = 32,
	.paused = false,
	.tag = "VCOM_NI"
};

const struct thinkos_thread_inf serial_input_inf = {
	.stack_ptr = serial_input_stack, 
	.stack_size = sizeof(serial_input_stack),
	.priority = 32,
	.thread_id = 32,
	.paused = false,
	.tag = "VCOM_SI"
};

int vcom_start(void)
{  
	struct tcp_pcb * svc;
	struct vcom * vcom = &serial_vcom;
	int th;

	svc = tcp_alloc();

	tcp_bind(svc, INADDR_ANY, htons(2000));

	if (tcp_listen(svc, 1) != 0) {
		INF("Can't register the TCP listner!");
		return -1;
	}

	vcom->serial = serial_open();
	vcom->svc = svc;
	vcom->tp = NULL;

	th = thinkos_thread_create_inf((void *)tcp_input_task, (void *)vcom, 
						&tcp_input_inf);
								
	INF("VCOM TCP input thread=%d", th);

	th = thinkos_thread_create_inf((void *)serial_input_task, (void *)vcom, 
						&serial_input_inf);
	(void)th;
								
	INF("VCOM serial input thread=%d", th);

	return 0;
}

