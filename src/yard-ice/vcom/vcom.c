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
#include <sys/serial.h>
#include <sys/os.h>

#include <sys/stm32f.h>
#include <trace.h>

#define TELOPTS

//#include <arpa/telnet.h>
#include "rfc2217.h"

/*
 * Definitions for the TELNET protocol.
 */
#define	IAC	255		/* interpret as command: */
#define	DONT	254		/* you are not to use option */
#define	DO	253		/* please, you use option */
#define	WONT	252		/* I won't use option */
#define	WILL	251		/* I will use option */
#define	SB	250		/* interpret as subnegotiation */
#define	GA	249		/* you may reverse the line */
#define	EL	248		/* erase the current line */
#define	EC	247		/* erase the current character */
#define	AYT	246		/* are you there */
#define	AO	245		/* abort output--but let prog finish */
#define	IP	244		/* interrupt process--permanently */
#define	BREAK	243		/* break */
#define	DM	242		/* data mark--for connect. cleaning */
#define	NOP	241		/* nop */
#define	SE	240		/* end sub negotiation */
#define EOR     239             /* end of record (transparent mode) */
#define	ABORT	238		/* Abort process */
#define	SUSP	237		/* Suspend process */
#define	xEOF	236		/* End of file: EOF is already used... */

#define SYNCH	242		/* for telfunc calls */

/* telnet options */
#define TELOPT_BINARY	0	/* 8-bit data path */
#define TELOPT_ECHO	1	/* echo */
#define	TELOPT_RCP	2	/* prepare to reconnect */
#define	TELOPT_SGA	3	/* suppress go ahead */
#define	TELOPT_NAMS	4	/* approximate message size */
#define	TELOPT_STATUS	5	/* give status */
#define	TELOPT_TM	6	/* timing mark */
#define	TELOPT_RCTE	7	/* remote controlled transmission and echo */
#define TELOPT_NAOL	8	/* negotiate about output line width */
#define TELOPT_NAOP	9	/* negotiate about output page size */
#define TELOPT_NAOCRD	10	/* negotiate about CR disposition */
#define TELOPT_NAOHTS	11	/* negotiate about horizontal tabstops */
#define TELOPT_NAOHTD	12	/* negotiate about horizontal tab disposition */
#define TELOPT_NAOFFD	13	/* negotiate about formfeed disposition */
#define TELOPT_NAOVTS	14	/* negotiate about vertical tab stops */
#define TELOPT_NAOVTD	15	/* negotiate about vertical tab disposition */
#define TELOPT_NAOLFD	16	/* negotiate about output LF disposition */
#define TELOPT_XASCII	17	/* extended ascic character set */
#define	TELOPT_LOGOUT	18	/* force logout */
#define	TELOPT_BM	19	/* byte macro */
#define	TELOPT_DET	20	/* data entry terminal */
#define	TELOPT_SUPDUP	21	/* supdup protocol */
#define	TELOPT_SUPDUPOUTPUT 22	/* supdup output */
#define	TELOPT_SNDLOC	23	/* send location */
#define	TELOPT_TTYPE	24	/* terminal type */
#define	TELOPT_EOR	25	/* end or record */
#define	TELOPT_TUID	26	/* TACACS user identification */
#define	TELOPT_OUTMRK	27	/* output marking */
#define	TELOPT_TTYLOC	28	/* terminal location number */
#define	TELOPT_3270REGIME 29	/* 3270 regime */
#define	TELOPT_X3PAD	30	/* X.3 PAD */
#define	TELOPT_NAWS	31	/* window size */
#define	TELOPT_TSPEED	32	/* terminal speed */
#define	TELOPT_LFLOW	33	/* remote flow control */
#define TELOPT_LINEMODE	34	/* Linemode option */
#define TELOPT_XDISPLOC	35	/* X Display Location */
#define TELOPT_OLD_ENVIRON 36	/* Old - Environment variables */
#define	TELOPT_AUTHENTICATION 37/* Authenticate */
#define	TELOPT_ENCRYPT	38	/* Encryption option */
#define TELOPT_NEW_ENVIRON 39	/* New - Environment variables */
#define	TELOPT_EXOPL	255	/* extended-options-list */
#define TELOPT_ENVIRON TELOPT_OLD_ENVIRON

#define	NTELOPTS	(1 + TELOPT_NEW_ENVIRON)
const char * const telopts[NTELOPTS + 1] = {
	"BINARY", "ECHO", "RCP", "SUPPRESS GO AHEAD", "NAME",
	"STATUS", "TIMING MARK", "RCTE", "NAOL", "NAOP",
	"NAOCRD", "NAOHTS", "NAOHTD", "NAOFFD", "NAOVTS",
	"NAOVTD", "NAOLFD", "EXTEND ASCII", "LOGOUT", "BYTE MACRO",
	"DATA ENTRY TERMINAL", "SUPDUP", "SUPDUP OUTPUT",
	"SEND LOCATION", "TERMINAL TYPE", "END OF RECORD",
	"TACACS UID", "OUTPUT MARKING", "TTYLOC",
	"3270 REGIME", "X.3 PAD", "NAWS", "TSPEED", "LFLOW",
	"LINEMODE", "XDISPLOC", "OLD-ENVIRON", "AUTHENTICATION",
	"ENCRYPT", "NEW-ENVIRON",
	0,
};
#define	TELOPT_FIRST	TELOPT_BINARY
#define	TELOPT_LAST	TELOPT_NEW_ENVIRON
#define	TELOPT_OK(x)	((unsigned int)(x) <= TELOPT_LAST)
#define	TELOPT(x)	telopts[(x)-TELOPT_FIRST]


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

struct serial_dev * serial_open(void);

int serial_write(struct serial_dev * dev, const void * buf, 
				 unsigned int len);

int serial_read(struct serial_dev * dev, char * buf, 
				unsigned int len, unsigned int msec);

struct vcom {
	struct serial_dev * serial;
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
//	char sb_buf[64];
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

		DCC_LOG(LOG_TRACE, "accepted.");

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
				if ((ret = serial_write(serial, buf, cnt)) <= 0) {
					DCC_LOG(LOG_ERROR, "serial_write() failed!");
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
//					sb_buf[sb_len++] = c;
					break;

				case CPC_SB_IAC_RCVD:
					if (c == SE) {
						state = CPC_DATA;
//						cpc_suboption(cpc, sb_buf, sb_len);
					} else {
						state = CPC_SUBOPTION;
//						sb_buf[sb_len++] = c;
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

		DCC_LOG(LOG_TRACE, "close...");

		tcp_close(tp);
		vcom->tp  = NULL;
	}

	DCC_LOG(LOG_ERROR, "thread loop break!!!");
	for(;;);
}

#define VCOM_BUF_SIZE 128

int __attribute__((noreturn)) serial_input_task(struct vcom * vcom)
{
	struct serial_dev * serial = vcom->serial;
	char buf[VCOM_BUF_SIZE];
	int len;
	int ret;

	DCC_LOG1(LOG_TRACE, "<%d> started...", __os_thread_self());

	for (;;) {
		if ((len = serial_read(serial, buf, VCOM_BUF_SIZE, 100)) < 0) {
			DCC_LOG(LOG_ERROR, "serial_read(): failed!");
			continue;
		}

		if (len == 0) {
			DCC_LOG(LOG_TRACE, "timeout");
			continue;
		}

		if (vcom->tp == NULL)
			continue;

		if ((ret = tcp_send(vcom->tp, buf, len, TCP_SEND_NOWAIT)) <= 0) {
			DCC_LOG1(LOG_WARNING, "tcp_send(): %d", ret);
			continue;
		}
		DCC_LOG1(LOG_MSG, "send: %d", ret);
	}

	for (;;);
}

uint32_t tcp_input_stack[512];
uint32_t serial_input_stack[512];

struct vcom serial_vcom;

int vcom_start(void)
{  
	struct tcp_pcb * svc;
	struct vcom * vcom = &serial_vcom;
	int th;

	svc = tcp_alloc();

	tcp_bind(svc, INADDR_ANY, htons(2000));

	if (tcp_listen(svc, 1) != 0) {
		tracef("%s(): Can't register the TCP listner!", __func__);
		return -1;
	}

	vcom->serial = serial_open();
	vcom->tp = svc;

	th = __os_thread_create((void *)tcp_input_task, (void *)vcom, 
								tcp_input_stack, sizeof(tcp_input_stack), 
								__OS_PRIORITY_LOWEST);


	tracef("VCOM TCP input thread=%d", th);


	th = __os_thread_create((void *)serial_input_task, (void *)vcom, 
								serial_input_stack, sizeof(serial_input_stack), 
								__OS_PRIORITY_LOWEST);

	tracef("VCOM serial input thread=%d", th);

	return 0;
}

