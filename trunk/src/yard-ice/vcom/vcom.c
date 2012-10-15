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

#include <netinet/in.h>
#include <tcpip/tcp.h>

#include <sys/dcclog.h>
#include <sys/at91.h>
#include <sys/serial.h>

#define TELOPTS

#include <arpa/telnet.h>
#include <rfc2217.h>

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
#define CPC_ESC_RCVD     9
#define CPC_ESC_SQR      10
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

#define CPC_SB_BUF_LEN 16

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

int at91_us_init(struct at91_us_dev * dev, int port);

int at91_us_write(struct at91_us_dev * dev, const void * buf, int len);

int at91_us_read(struct at91_us_dev * dev, void * buf, int len, int msec);

int at91_us_config(struct at91_us_dev * dev, int baudrate, 
				   int mode, int tmo_bits);

struct vcom {
	struct at91_us_dev * dev;
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

int  cpc_opt_will(struct tcp_pcb * tp, struct tn_opt * t, int opt)
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

int __attribute__((noreturn)) tcp_input_task(struct vcom * vcom, 
											 uthread_id_t id)
{
	struct at91_us_dev * dev = vcom->dev;
	struct tcp_pcb * svc;
	struct tcp_pcb * tp;
	char buf[128];
	char sb_buf[64];
	int sb_len;
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

	svc = vcom->tp;
	vcom->tp = NULL;

	for (;;) {
		if ((tp = tcp_accept(svc)) == NULL) {
			DCC_LOG(LOG_ERROR, "tcp_accept().");
			break;
		}

		DCC_LOG2(LOG_TRACE, "%I:%d accepted.", tp->t_faddr, ntohs(tp->t_fport));

		vcom->tp  = tp;

		cpc_opt_clr(&opt);
		cnt = 0;
		sb_len = 0;
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
				if ((ret = at91_us_write(dev, buf, cnt)) <= 0) {
					DCC_LOG1(LOG_WARNING, "at91_us_write(): %d", ret);
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

			DCC_LOG3(LOG_MSG, "%I:%d recv: %d", tp->t_faddr, 
					 ntohs(tp->t_fport), len);

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
						} else  {
							if (c == IN_ESC) {
								/* escape sequence */
								state = CPC_ESC_RCVD;
							}
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
					if (sb_len < CPC_SB_BUF_LEN) {
						DCC_LOG1(LOG_TRACE, "suboption: %d", c);
					}
					sb_buf[sb_len++] = c;
					break;

				case CPC_SB_IAC_RCVD:
					if (c == SE) {
						state = CPC_DATA;
//						cpc_suboption(cpc, sb_buf, sb_len);
					} else {
						state = CPC_SUBOPTION;
						sb_buf[sb_len++] = c;
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

					/* slave tty processing */
				case CPC_ESC_RCVD:
					if (c == '[')
						state = CPC_ESC_SQR;
					else
						state = CPC_DATA;		
					break;

				case CPC_ESC_SQR:
					state = CPC_DATA;		
					break;

				default:
					DCC_LOG1(LOG_WARNING, "invalid state: %d!!", state);
					break;
				}

			}

		}

		DCC_LOG2(LOG_TRACE, "%I:%d close...", tp->t_faddr, ntohs(tp->t_fport));

		tcp_close(tp);
		vcom->tp  = NULL;
	}

	DCC_LOG(LOG_ERROR, "thread loop break!!!");
	for(;;);
}

int __attribute__((noreturn)) serial_input_task(struct vcom * vcom, 
												uthread_id_t id)
{
	struct at91_us_dev * dev = vcom->dev;
	char buf[128];
	int len;
	int ret;

	for (;;) {
		for (;;) {
			if ((len = at91_us_read(dev, buf, 128, 10000)) < 0) {
				DCC_LOG1(LOG_WARNING, "at91_us_read(): %d", len);
				break;
			}
			//			len = 0;
			//			uthread_sleep(100000);

			if (len == 0) {
				DCC_LOG(LOG_MSG, "timeout");
				continue;
			}

			if (vcom->tp == NULL)
				continue;

			if ((ret = tcp_send(vcom->tp, buf, len, TCP_SEND_NOWAIT)) <= 0) {
				DCC_LOG1(LOG_WARNING, "tcp_send(): %d", ret);
				break;
			}
			DCC_LOG3(LOG_MSG, "%I:%d send: %d", vcom->tp->t_faddr, 
					 ntohs(vcom->tp->t_fport), ret);
		}
	}
}

uint32_t tcp_input_stack[512];
uint32_t serial_input_stack[512];

int vcom_start(void)
{  
	struct at91_us_dev * dev;
	struct tcp_pcb * svc;
	struct vcom * vcom;
	int th;

	svc = tcp_alloc();

	tcp_bind(svc, INADDR_ANY, htons(2000));

	if (tcp_listen(svc, 1) != 0) {
		printf("Can't register the TCP listner!\n");
		return -1;
	}

	if ((dev = malloc(sizeof(struct at91_us_dev))) == NULL) {
		DCC_LOG(LOG_ERROR, "malloc() failed!");
		return -1;
	}

	DCC_LOG1(LOG_TRACE, "dev=%08x", dev);

	if (at91_us_init(dev, 0) < 0) {
		free(dev);
		printf("Can't initialize serial port driver!\n");
		return -1;
	}

	at91_us_config(dev, 115200, SERIAL_8N1, 50);

	vcom = malloc(sizeof(struct vcom));
	vcom->dev = dev;
	vcom->tp = svc;

	th = uthread_create(tcp_input_stack, sizeof(tcp_input_stack), 
						(uthread_task_t)tcp_input_task, 
						(void *)vcom, 1, NULL); 
	printf("<%d> ", th);


	th = uthread_create(serial_input_stack, sizeof(serial_input_stack), 
						(uthread_task_t)serial_input_task, 
						(void *)vcom, 1, NULL); 
	printf("<%d> ", th);

	return 0;
}

