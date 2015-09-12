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
 * @file tftp.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifdef CONFIG_H
#include "config.h"
#endif

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <errno.h>
#include <arpa/tftp.h>

#include <tcpip/udp.h>
#include <thinkos.h>

#include <sys/dcclog.h>

#define MAX_TFTP_SEGSIZE 1428
#define MAX_TFTP_MSG (MAX_TFTP_SEGSIZE + sizeof(struct tftphdr))

enum {
	TFTP_NETASCII = 0,
	TFTP_OCTET = 1,
	TFTP_EMAIL = 2
};

const char * const tftp_opc[] = {
	"UNKNOWN", "RRQ", "WRQ", "DATA", "ACK", "ERROR"
};

const char * const tftp_mode[] = {
	"NETASCII", "OCTET", "EMAIL"
};

const char * const tftp_err[] = {
	"EUNDEF", "ENOTFOUND", "EACCESS", "ENOSPACE", 
	"EBADOP", "EBADID", "EEXISTS", "ENOUSER"
};

enum {
	TFTPD_IDLE = 0,
	TFTPD_RECV_NETASCII = 1,
	TFTPD_RECV_OCTET = 2,
	TFTPD_SEND_NETASCII = 3,
	TFTPD_SEND_OCTET = 4,
	TFTPD_RECV_ERROR = 5
};

#define TFTP_ERR_MSG_MAX 63

struct tftp_pkt_err {
	struct tftphdr hdr;
	uint8_t payload[TFTP_ERR_MSG_MAX + 1];
};

#define TFTP_OACK_OPT_MAX 64

struct tftp_pkt_oack {
	uint16_t th_opcode;
	char opt[TFTP_OACK_OPT_MAX];
};

int tftp_bin_read(unsigned int addr, unsigned char * buf, int size)
{
	/* Read data at buffer's last half... */
	memset(buf, 'B', size);

	return size;
}

int tftp_bin_write(unsigned int addr, const unsigned char * buf, int size)
{
	return size;
}

int tftp_ascii_read(unsigned int addr, unsigned char * buf, int size)
{
	/* Read data at buffer's last half... */
	memset(buf, 'A', size);

	return size;
}

int tftp_ascii_write(unsigned int addr, const unsigned char * buf, int size)
{
	return size;
}


int tftp_error(struct udp_pcb * udp, struct sockaddr_in * sin, 
			   int errno, char * msg)
{
	struct tftp_pkt_err pkt;
	int len;
	int n;

	if (msg == NULL) {
		msg = (char *)tftp_err[errno];
	}

	n = MIN(strlen(msg), TFTP_ERR_MSG_MAX);

	pkt.hdr.th_opcode = htons(TFTP_ERROR);
	pkt.hdr.th_code = htons(errno);
	strncpy((char *)pkt.hdr.th_msg, msg, n);
	pkt.hdr.th_msg[n] = '\0';

	len = sizeof(struct tftphdr) + n + 1;

	// DCC_LOG(LOG_TRACE, "1.");

	return udp_sendto(udp, &pkt, len, sin);
}

int tftp_ack(struct udp_pcb * udp, int block, struct sockaddr_in * sin)
{
	struct tftphdr hdr;
	int ret;

	// DCC_LOG1(LOG_TRACE, "block: %d", block);

	hdr.th_opcode = htons(TFTP_ACK);
	hdr.th_block = htons(block);

	if ((ret = udp_sendto(udp, &hdr, sizeof(struct tftphdr), sin)) < 0) {
		// DCC_LOG(LOG_WARNING, "udp_sendto() fail");
	}

	return ret;
}

int tftp_oack(struct udp_pcb * udp, struct sockaddr_in * sin,
			  char * opt, int len)
{
	struct tftp_pkt_oack pkt;

	if (len > TFTP_OACK_OPT_MAX) {
		// DCC_LOG1(LOG_ERROR, "len(%d) > TFTP_OACK_OPT_MAX", len);
		return -1;
	}

	// DCC_LOG(LOG_TRACE, "OACK....");

	pkt.th_opcode = htons(TFTP_OACK);
	memcpy(pkt.opt, opt, len);

	return udp_sendto(udp, &pkt, 2 + len, sin);
}

struct tftp_req {
	char * fname;
	uint8_t mode;
	uint8_t opt_len;
	uint16_t blksize;
	char opt[TFTP_OACK_OPT_MAX];
};

int tftp_req_parse(char * hdr, struct tftp_req * req)
{
	char * cp;
	char * opt;
	int n;

	/* Read Request */
	req->fname = hdr;

	cp = req->fname + strlen(req->fname) + 1;

	if (strcmp(cp, "octet") == 0) {
		req->mode = TFTP_OCTET;
	} else if (strcmp(cp, "netascii") == 0) {
		req->mode = TFTP_NETASCII;
	} else {
		return -1;
	}

	opt = req->opt;
	req->opt_len = 0;
	cp += strlen(cp) + 1;
	if (strcmp(cp, "blksize") == 0) {
		cp += strlen(cp) + 1;
		req->blksize = strtoul(cp, NULL, 10);
		if (req->blksize > MAX_TFTP_SEGSIZE)
			req->blksize = MAX_TFTP_SEGSIZE;
		n = sprintf(opt, "blksize.%d", req->blksize);
		opt[7] = '\0';
		opt += n + 1;
		req->opt_len += n + 1;
	} else {
		/* default segment size */
		req->blksize = TFTP_SEGSIZE; 
	}

	return 0;
}

void __attribute__((noreturn)) tftpd_task(void * arg)
{
	uint8_t buf[MAX_TFTP_MSG];
	struct tftphdr * hdr = (struct tftphdr *)buf;
	char * msg = (char *)buf;
	struct sockaddr_in sin;
	struct udp_pcb * udp;
	struct tftp_req req;
	int state = TFTPD_IDLE;
	unsigned int addr_start = 0;
	unsigned int addr_end = 0;
	int block = 0;
	int opc;
	int len;
	int blksize = TFTP_SEGSIZE; 

	// DCC_LOG1(LOG_TRACE, "thread: %d", thinkos_thread_self());

	if ((udp = udp_alloc()) == NULL) {
		// DCC_LOG(LOG_WARNING, "udp_alloc() fail!");
		abort();
	}

	if (udp_bind(udp, INADDR_ANY, htons(IPPORT_TFTP)) < 0) {
		// DCC_LOG(LOG_WARNING, "udp_bind() fail!");
		abort();
	}

	for (;;) {
		if ((len = udp_recv(udp, buf, MAX_TFTP_MSG, &sin)) < 0) {
			if (len == -ECONNREFUSED) {
				// DCC_LOG(LOG_WARNING, "udp_rcv ICMP error: ECONNREFUSED");
			}
			if (len == -EFAULT) {
				// DCC_LOG(LOG_WARNING, "udp_rcv error: EFAULT");
			}
			if (len == -ENOTCONN) {
				// DCC_LOG(LOG_WARNING, "udp_rcv error: ENOTCONN");
			}
			continue;
		}


		opc = htons(hdr->th_opcode);
		if ((opc != TFTP_RRQ) && (opc != TFTP_WRQ)) {
			// DCC_LOG1(LOG_WARNING, "invalid opc: %d", opc);
			continue;
		}

		if (udp_connect(udp, sin.sin_addr.s_addr, sin.sin_port) < 0) {
			// DCC_LOG(LOG_WARNING, "udp_connect() error");
			continue;
		}

		// DCC_LOG2(LOG_TRACE, "Connected to: %I.%d", sin.sin_addr.s_addr,
		//		 ntohs(sin.sin_port));

		for (;;) {
			// DCC_LOG3(LOG_INFO, "%I.%d %d",
			//		 sin.sin_addr.s_addr, ntohs(sin.sin_port), len);

			// DCC_LOG2(LOG_INFO, "len=%d, opc=%s", len, tftp_opc[opc]);

			switch (opc) {
			case TFTP_RRQ:
				// DCC_LOG(LOG_TRACE, "read request: ...");

				tftp_req_parse((char *)&(hdr->th_stuff), &req);
				blksize = req.blksize;

				/* set the transfer info */
				addr_start = 0;
				addr_end = addr_start + (1024 * 1024);
				block = 0;

				// DCC_LOG2(LOG_TRACE, "start=0x%08x end=0x%08x",
				//		 addr_start, addr_end);

				if (req.mode == TFTP_NETASCII) {
					state = TFTPD_SEND_NETASCII;
				} else if (req.mode == TFTP_OCTET) {
					state = TFTPD_SEND_OCTET;
				} else {
					tftp_error(udp, &sin, TFTP_EUNDEF, NULL);
					break;
				}

				if (req.opt_len) {
					tftp_oack(udp, &sin, req.opt, req.opt_len);
					break;
				}

				if (req.mode == TFTP_NETASCII)
					goto send_netascii;

				if (req.mode == TFTP_OCTET)
					goto send_octet;

				break;

			case TFTP_WRQ:
				/* Write Request */
				// DCC_LOG(LOG_TRACE, "write request...");

				tftp_req_parse((char *)&(hdr->th_stuff), &req);
				blksize = req.blksize;

				/* set the transfer info */
				addr_start = 0;
				addr_end = addr_start + (1024 * 1024);
				block = 0;

				// DCC_LOG2(LOG_TRACE, "start=0x%08x end=0x%08x",
				//		 addr_start, addr_end);

				if ((req.mode == TFTP_NETASCII) || (req.mode == TFTP_OCTET)) {
					state = (req.mode == TFTP_NETASCII) ? 
						TFTPD_RECV_NETASCII : TFTPD_RECV_OCTET;

					if (req.opt_len) 
						tftp_oack(udp, &sin, req.opt, req.opt_len);
					else
						tftp_ack(udp, block, &sin);

					break;
				} 

				tftp_error(udp, &sin, TFTP_EUNDEF, NULL);
				break;

			case TFTP_ACK:
				block = htons(hdr->th_block);
				// DCC_LOG1(LOG_TRACE, "ACK: %d.", block);

				if (state == TFTPD_SEND_NETASCII) {
					unsigned int addr;
					int rem;
					int n;

send_netascii:
					addr = addr_start + (block * blksize);
					rem = addr_end - addr;
					if (rem < 0) {
						state = TFTPD_IDLE;
						// DCC_LOG1(LOG_TRACE, "eot: %d bytes sent.",
						//		 addr_end - addr_start);
						break;
					}

					n = (rem < blksize) ? rem : blksize;

					// DCC_LOG2(LOG_TRACE, "send netascii: addr=0x%08x n=%d",
					//		 addr, n);

					/* build the packet */
					len = tftp_ascii_read(addr, hdr->th_data, n);

					goto send_data;
				}

				if (state == TFTPD_SEND_OCTET) {
					unsigned int addr;
					int rem;
					int n;

send_octet:
					addr = addr_start + (block * blksize);
					rem = addr_end - addr;
					if (rem < 0) {
						state = TFTPD_IDLE;
						// DCC_LOG1(LOG_TRACE, "eot: %d bytes sent.",
						//		 addr_end - addr_start);
						break;
					}
					n = (rem < blksize) ? rem : blksize;

					// DCC_LOG2(LOG_TRACE, "send octet: addr=0x%08x n=%d", addr, n);

					/* build the packet */
					len = tftp_bin_read(addr, hdr->th_data, n);

					if (len < 0) {
						// DCC_LOG(LOG_WARNING, "target memory read error.");
						len = 0;
					}

send_data:
					hdr->th_opcode = htons(TFTP_DATA);
					hdr->th_block = htons(block + 1);

					// DCC_LOG2(LOG_TRACE, "block %d: %d bytes.", block + 1,  len);

					if (udp_sendto(udp, hdr, 
								   sizeof(struct tftphdr) + len, &sin) < 0) {
						// DCC_LOG(LOG_WARNING, "udp_sendto() fail");
						state = TFTPD_IDLE;
						break;
					}

					break;
				}

				// DCC_LOG(LOG_WARNING, "state invalid!");
				break;

			case TFTP_DATA:
				/* skip the header */
				len -= 4;
				// DCC_LOG2(LOG_TRACE, "block=%d len=%d",
				//		 htons(hdr->th_block), len);

				if (htons(hdr->th_block) != (block + 1)) {
					/* retransmission, just ack */
					// DCC_LOG2(LOG_WARNING, "retransmission, block=%d len=%d",
					//		 block, len);
					tftp_ack(udp, block, &sin);
					break;
				}

				if (state == TFTPD_RECV_OCTET) {
					unsigned int addr;
					int n;

					addr = addr_start + (block * blksize);

					block++;

					if (len != blksize) {
						// DCC_LOG(LOG_TRACE, "last packet...");
						state = TFTPD_IDLE;
						if (len == 0) {
							tftp_ack(udp, block, &sin);
							break;
						}
					} else {
						// DCC_LOG2(LOG_TRACE, "rcvd octet: addr=0x%08x n=%d",	addr, len);
						/* ACK the packet before writing to
						   speed up the transfer, errors are postponed... */
						tftp_ack(udp, block, &sin);
					}

					n = tftp_bin_write(addr, hdr->th_data, len);

					if (n < len) {
						if (n < 0) {
							// DCC_LOG(LOG_ERROR, "target_mem_write()!");
							sprintf(msg, "TARGET WRITE FAIL: %08x", addr);
						} else {
							// DCC_LOG2(LOG_WARNING, "short read: ret(%d) < len(%d)!", n, len);
							sprintf(msg, "TARGET SHORT WRITE: %08x", 
									addr + n);
						}
						tftp_error(udp, &sin, TFTP_EUNDEF, msg);
						state = TFTPD_RECV_ERROR;
					} else {
						if (n > len) {
							// DCC_LOG2(LOG_WARNING, "long read: ret(%d) < len(%d)!", n, len);
						}
						if (state == TFTPD_IDLE) {
							/* ack the last packet ... */
							tftp_ack(udp, block, &sin);
						}
					}
					break;
				}

				if (state == TFTPD_RECV_ERROR) {
//					tftp_error(udp, &sin, TFTP_EUNDEF, "TARGET WRITE FAIL.");
					state = TFTPD_IDLE;
					break;
				}

				if (state == TFTPD_RECV_NETASCII) {
					unsigned int addr;

					addr = addr_start + (block * blksize);

					block++;
					if (len != blksize) {
						state = TFTPD_IDLE;
						if (len == 0) {
							tftp_ack(udp, block, &sin);
							break;
						}
					} else {
						/* ACK the packet before writing to
						   speed up the transfer, errors are postponed... */
						tftp_ack(udp, block, &sin);
					}
					// DCC_LOG1(LOG_TRACE, "ASCII recv %d...", len);
					tftp_ascii_write(addr, hdr->th_data, len);

					break;
				}

				tftp_error(udp, &sin, TFTP_EUNDEF, NULL);
				break;

			case TFTP_ERROR:
				// DCC_LOG2(LOG_TRACE, "error: %d: %s.",
				//		 htons(hdr->th_code), hdr->th_data);
				break;

			}

			if (state == TFTPD_IDLE) {
				// DCC_LOG(LOG_TRACE, "[IDLE]");
				break;
			}

			if ((len = udp_recv_tmo(udp, buf, MAX_TFTP_MSG, &sin, 5000)) < 0) {
				if (len == -ETIMEDOUT) {
					// DCC_LOG(LOG_WARNING, "udp_recv_tmo() timeout!");
				} else {
					if (len == -ECONNREFUSED) {
						// DCC_LOG(LOG_WARNING, "udp_recv_tmo() lost peer!");
					} else {
						// DCC_LOG(LOG_WARNING, "udp_recv_tmo() failed!");
					}
				}
				/* break the inner loop */
				break;
			}

			opc = htons(hdr->th_opcode);
		}

		/* disconnect */
		// DCC_LOG(LOG_TRACE, "disconnecting.");
		udp_connect(udp, INADDR_ANY, 0);
	}
}

uint32_t tftp_stack[384 + (MAX_TFTP_SEGSIZE / 4)];

const struct thinkos_thread_inf tftpd_inf = {
	.stack_ptr = tftp_stack, 
	.stack_size = sizeof(tftp_stack), 
	.priority = 32,
	.thread_id = 9, 
	.paused = 0,
	.tag = "TFTPD"
};

int tftpd_start(void)
{
	return thinkos_thread_create_inf((void *)tftpd_task, NULL, &tftpd_inf);
}

