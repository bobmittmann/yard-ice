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
 * @file telnet.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <netinet/in.h>
#include <tcpip/tcp.h>
#include <arpa/telnet.h>

#include <sys/os.h>

#include <sys/dcclog.h>
#include <trace.h>

/* TELNET states */
#define TN_DATA         0
#define TN_IAC_RCVD     1
#define TN_DONT_RCVD    2
#define TN_DO_RCVD      3
#define TN_WONT_RCVD    4
#define TN_WILL_RCVD    5
#define TN_SUBOPTION_ID 6
#define TN_SUBOPTION    7
#define TN_SB_IAC_RCVD  8
#define TN_INVALID_SUBOPTION	11
#define TN_INVALID_SB_IAC_RCVD	12

/* TELNET flags */
#define TN_BINARY       0x01
#define TN_ECHO         0x02

/* Some TELNET options */
#define TN_OPT_BINARY   0x01
#define TN_OPT_ECHO     0x02
#define TN_OPT_RCP      0x04
#define TN_OPT_SGA      0x08
#define TN_OPT_NAMS     0x10
#define TN_OPT_STATUS   0x20
#define TN_OPT_TM       0x40
#define TN_OPT_RCTE     0x80

/* TTY input chars */
#define IN_NULL     '\0'
#define IN_BS       '\x8'
#define IN_DEL      0x7F
#define IN_EOL      '\r'
#define IN_SKIP     '\3'
#define IN_EOF      '\x1A'
#define IN_ESC      '\033'

/* TTY output translation */
#define OUT_DEL     "\x8 \x8"
#define OUT_EOL     "\r\n"
#define OUT_SKIP    "^C\n"
#define OUT_EOF     "^Z"
#define OUT_BEL     "\7"

#ifndef MIN
#define MIN(X, Y) (((int)(X) < (int)(Y)) ? (X) : (Y))
#endif

#define TN_SB_BUF_LEN 16

#ifndef TELNET_SVC_RX_BUF_LEN 
#define TELNET_SVC_RX_BUF_LEN 128
#endif

struct telnet_svc {
	struct tcp_pcb * volatile svc;
	struct tcp_pcb * volatile tp;
	struct {
		int nonempty_flag;
		int nonfull_flag;
		char buf[TELNET_SVC_RX_BUF_LEN];
		volatile unsigned int head;
		volatile unsigned int tail;
	} rx;
};

struct tn_opt {
	uint32_t local; /* local options */
	uint32_t remote; /* remote options */
};

int tn_opt_will(struct tcp_pcb * tp, struct tn_opt * set, int opt)
{
	uint8_t buf[4];

	if (opt > 31) {
		DCC_LOG1(LOG_WARNING, "opt %d out of range", opt);
		return -1;
	}

	if (set->local & (1 << opt))
		return 0;

	set->local |= 1 << opt;

	buf[0] = IAC;
	buf[1] = WILL;
	buf[2] = opt;

	return tcp_send(tp, buf, 3, TCP_SEND_NOWAIT);
}

int tn_opt_do(struct tcp_pcb * tp, struct tn_opt * set, int opt)
{
	uint8_t buf[4];

	if (opt > 31) {
		DCC_LOG1(LOG_WARNING, "opt %d out of range", opt);
		return -1;
	}

	if (set->remote & (1 << opt))
		return 0;
	set->remote |= 1 << opt;

	buf[0] = IAC;
	buf[1] = DO;
	buf[2] = opt;

	return tcp_send(tp, buf, 3, TCP_SEND_NOWAIT);
}

int  tn_opt_wont(struct tcp_pcb * tp, struct tn_opt * set, int opt)
{
	uint8_t buf[4];

	if (opt > 31) {
		DCC_LOG1(LOG_WARNING, "opt %0d out of range", opt);
		return -1;
	}

	if (!(set->local & (1 << opt)))
		return 0;
		
	set->local &= ~(1 << opt);

	buf[0] = IAC;
	buf[1] = WONT;
	buf[2] = opt;

	return tcp_send(tp, buf, 3, TCP_SEND_NOWAIT);
}

int  tn_opt_dont(struct tcp_pcb * tp, struct tn_opt * set, int opt)
{
	uint8_t buf[4];

	if (opt > 31) {
		DCC_LOG1(LOG_WARNING, "opt %0d out of range", opt);
		return -1;
	}

	if (!(set->remote & (1 << opt)))
		return 0;

	buf[0] = IAC;
	buf[1] = DONT;
	buf[2] = opt;

	return tcp_send(tp, buf, 3, TCP_SEND_NOWAIT);
}

void tn_opt_clr(struct tn_opt * set)
{
	set->local = 0;
	set->remote = 0;
}

int __attribute__((noreturn)) telnet_input_task(struct telnet_svc * tn)
{
	struct tcp_pcb * svc;
	struct tcp_pcb * tp;
	char buf[128];
	int sb_len;
	int len;
	char * src;
	int rem;
	int binary;
	int state;
	int c;
	struct tn_opt opt;
	unsigned int head;

	svc = tn->svc;

	for (;;) {

		tracef("TELNET wating for connection.");
		DCC_LOG(LOG_TRACE, "TELNET: waiting for connection...");

		if ((tp = tcp_accept(svc)) == NULL) {
			DCC_LOG(LOG_ERROR, "tcp_accept().");
			break;
		}

		tracef("TELNET connection accepted.");
		DCC_LOG(LOG_TRACE, "TELNET: accepted.");

		tn->tp  = tp;

		tn_opt_clr(&opt);
		sb_len = 0;
		binary = 0;
		state = TN_DATA;
		tn_opt_will(tp, &opt, TELOPT_SGA);
		tn_opt_do(tp, &opt, TELOPT_SGA);
		tn_opt_will(tp, &opt, TELOPT_ECHO);
		tn_opt_will(tp, &opt, TELOPT_BINARY);
		tn_opt_do(tp, &opt, TELOPT_BINARY);

		head = tn->rx.head;

		for (;;) {

			if (head != tn->rx.tail) {
				/* update the head */
				tn->rx.head = head;
				DCC_LOG1(LOG_TRACE, "rx nonempty: head=%d", head);
				/* signal the head update */
				thinkos_flag_set(tn->rx.nonempty_flag);
			}

			/* receive data form network */
			if ((len = tcp_recv(tp, buf, 128)) <= 0) {
				DCC_LOG1(LOG_WARNING, "tcp_recv(): %d", len);
				break;
			}

			DCC_LOG1(LOG_TRACE, "recv: %d", len);

			/* set the input processing pointer */
			src = buf;
			/* input remaining (to be processed) bytes */
			rem = len;

			while (rem > 0) {
				c = *src++;
				rem--;

				if (state == TN_DATA) {
					if (c == IAC) {
						state = TN_IAC_RCVD;
					} else {
						if ((binary) || ((c >= 3) && (c < 127))) {
							/* ASCII characters */
							DCC_LOG1(LOG_TRACE, "rx nonempty: head=%d", head);

							/* buffer is full */
							if (head == (tn->rx.tail + TELNET_SVC_RX_BUF_LEN)) {
								/* update the head */
								tn->rx.head = head;
								/* signal the head update */
								thinkos_flag_set(tn->rx.nonempty_flag);
								
								/* wait for space in the input buffer */
								while (1) {
									thinkos_flag_clr(tn->rx.nonfull_flag);
									if (head < (tn->rx.tail + 
												 TELNET_SVC_RX_BUF_LEN)) {
										break;
									}
									thinkos_flag_wait(tn->rx.nonfull_flag);
								}
							} 

							tn->rx.buf[head++ % TELNET_SVC_RX_BUF_LEN] = c;

						} 
					}
					continue;
				}

				/* handles TELNET inputs options */
				switch (state) {

				case TN_IAC_RCVD:
					switch (c) {
					case IAC:
						state = TN_DATA;
						break;
					case DONT:
						state = TN_DONT_RCVD;
						break;
					case DO:
						state = TN_DO_RCVD;
						break;
					case WONT:
						state = TN_WONT_RCVD;
						break;
					case WILL:
						state = TN_WILL_RCVD;
						break;
					case SB:
						state = TN_SUBOPTION_ID;
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
						state = TN_DATA;
						break;
					}
					break;

				case TN_DONT_RCVD:
					DCC_LOG1(LOG_TRACE, "DONT %s", TELOPT(c));
					tn_opt_wont(tp, &opt, c);
					state = TN_DATA;
					break;

				case TN_DO_RCVD:
					DCC_LOG1(LOG_TRACE, "DO %s", TELOPT(c));
					switch (c) {

					case TELOPT_SGA:
						tn_opt_will(tp, &opt, c);
						break;

					case TELOPT_ECHO:
						tn_opt_will(tp, &opt, c);
						break;

					case TELOPT_BINARY:
						tn_opt_will(tp, &opt, c);
						break;

					default:
						tn_opt_wont(tp, &opt, c);
					}

					state = TN_DATA;
					break;

				case TN_WONT_RCVD:
					DCC_LOG1(LOG_TRACE, "WONT %s", TELOPT(c));
					tn_opt_dont(tp, &opt, c);
					state = TN_DATA;
					break;

				case TN_WILL_RCVD:
					DCC_LOG1(LOG_TRACE, "WILL %s", TELOPT(c));

					switch (c) {
					case TELOPT_ECHO:
						tn_opt_dont(tp, &opt, c);
						break;

					case TELOPT_SGA:
						tn_opt_do(tp, &opt, c);
						break;

					case TELOPT_BINARY:
						tn_opt_do(tp, &opt, c);
						binary = 1;
						break;

					default:
						tn_opt_dont(tp, &opt, c);
					}

					state = TN_DATA;
					break;

				case TN_SUBOPTION_ID:
					state = TN_SUBOPTION;
					break;

				case TN_SUBOPTION:
					if (c == IAC)
						state = TN_SB_IAC_RCVD;
					if (sb_len < TN_SB_BUF_LEN) {
						DCC_LOG1(LOG_TRACE, "suboption: %d", c);
					}
//					sb_buf[sb_len++] = c;
					break;

				case TN_SB_IAC_RCVD:
					if (c == SE) {
						state = TN_DATA;
//						tn_suboption(cpc, sb_buf, sb_len);
					} else {
						state = TN_SUBOPTION;
//						sb_buf[sb_len++] = c;
					}
					break;

				case TN_INVALID_SUBOPTION:
					if (c == IAC)
						state = TN_INVALID_SB_IAC_RCVD;
					break;

				case TN_INVALID_SB_IAC_RCVD:
					if (c == SE)
						state = TN_DATA;
					else
						state = TN_INVALID_SUBOPTION;
					break;

				default:
					DCC_LOG1(LOG_WARNING, "invalid state: %d!!", state);
					break;
				}

			}

		}

		DCC_LOG(LOG_TRACE, "close...");

		tcp_close(tp);
		tracef("TELNET connection closed.");
		tn->tp  = NULL;
	}

	DCC_LOG(LOG_ERROR, "thread loop break!!!");
	for(;;);
}

int telnet_svc_write(struct telnet_svc * tn, 
					 const void * buf, unsigned int len)
{
	struct tcp_pcb * tp = tn->tp;
	char * cp;
	int cnt;
	int rem;
	int pos;

	DCC_LOG1(LOG_TRACE, "len=%d", len);

	if (tn->tp == NULL) {
		/* discard if we are not connected */
		return len;
	}

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
		cnt += pos;
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

	return len;
}

int telnet_svc_read(struct telnet_svc * tn, void * buf,
					unsigned int len, unsigned int msec)
{
	char * cp = (char *) buf;
	unsigned int tail;
	int max;
	int cnt;
	int pos;

	DCC_LOG3(LOG_TRACE, "<%d> len=%d msec=%d", 
			 __os_thread_self(), len, msec);

	/* rx.tail can oly be changed inside this function, it is declared 
	   as volatile, for performance reasons we read it only once at 
	   the beginning and write it back at the end. */
	tail = tn->rx.tail;

	while (1) {
		DCC_LOG(LOG_INFO, "flag clr...");
		thinkos_flag_clr(tn->rx.nonempty_flag);

		/* get the maximum number of chars we can read from the buffer */
		if ((max = tn->rx.head - tail) > 0)
			break;

		DCC_LOG(LOG_INFO, "waiting...");
		/* wait for a signal indicating that there is some data in the
		   input buffer */
		thinkos_flag_wait(tn->rx.nonempty_flag);
	} 

	/* cnt is the number of chars we will read from the buffer,
	   it should be the the minimum of max and len */
	cnt = MIN(max, len);
	/* get the tail position in the buffer */
	pos = (tail % TELNET_SVC_RX_BUF_LEN);
	/* check whether to wrap arround or on not */
	if ((pos + cnt) > TELNET_SVC_RX_BUF_LEN) {
		/* we need to perform two reads */
		int n;
		int m;
		/* get the number of chars from tail pos until the end of buffer */
		n = TELNET_SVC_RX_BUF_LEN - pos;
		/* the remaining chars are at the beginning of the buffer */
		m = cnt - n;
	
		DCC_LOG2(LOG_TRACE, "n=%d m=%d", n, m);

		memcpy(cp, &tn->rx.buf[pos], n);
		cp += n;
		memcpy(cp, &tn->rx.buf[0], m);
	} else {
		memcpy(cp, &tn->rx.buf[pos], cnt);
	}

	tn->rx.tail = tail += cnt;

	if (cnt) {
		DCC_LOG(LOG_TRACE, "rx buffer non empty signal");
		thinkos_flag_set(tn->rx.nonfull_flag);
	}

	return cnt;
}

int telnet_svc_flush(struct telnet_svc * tn)
{
	DCC_LOG(LOG_ERROR, "not implemented!");

	return 0;
}

int telnet_svc_release(struct telnet_svc * tn)
{
	return tcp_close(tn->tp);
}

uint32_t telnet_input_stack[512];

struct telnet_svc telnet_svc;

struct telnet_svc * telnet_svc_init(int port)
{  
	struct telnet_svc * tn = &telnet_svc;
	int th;

	if (port == 0)
		port = 23; /* use default TELNET srvice port */

	tn->svc = tcp_alloc();
	tn->tp = NULL;
	tcp_bind(tn->svc, INADDR_ANY, htons(port));

	if (tcp_listen(tn->svc, 1) != 0) {
		tracef("%s(): Can't register the TCP listner!", __func__);
		return NULL;
	}

	tn->rx.nonempty_flag = thinkos_flag_alloc();
	tn->rx.nonfull_flag = thinkos_flag_alloc();
	tn->rx.head = 0;
	tn->rx.tail = 0;

	th = __os_thread_create((void *)telnet_input_task, (void *)tn, 
								telnet_input_stack, sizeof(telnet_input_stack), 
								__OS_PRIORITY_LOWEST);

	tracef("TELNET TCP input thread=%d", th);

	return tn;
}

