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
#include <sys/null.h>
#include <sys/shell.h>

#include <tcpip/ethif.h>
#include <tcpip/route.h>
#include <tcpip/udp.h>

#include <arpa/tftp.h>

#include <debugger.h>
#include <command.h>
#include <tftpd.h>
#include <errno.h>
#include <trace.h>

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

	DCC_LOG(LOG_TRACE, "1.");

	return udp_sendto(udp, &pkt, len, sin);
}

int tftp_ack(struct udp_pcb * udp, int block, struct sockaddr_in * sin)
{
	struct tftphdr hdr;
	int ret;

	DCC_LOG1(LOG_TRACE, "block: %d", block);

	DBG("TFTP: ACK, block=%d", block);

	hdr.th_opcode = htons(TFTP_ACK);
	hdr.th_block = htons(block);

	if ((ret = udp_sendto(udp, &hdr, sizeof(struct tftphdr), sin)) < 0) {
		DCC_LOG(LOG_WARNING, "udp_sendto() fail");
	}

	return ret;
}

int tftp_oack(struct udp_pcb * udp, struct sockaddr_in * sin,
			  char * opt, int len)
{
	struct tftp_pkt_oack pkt;

	if (len > TFTP_OACK_OPT_MAX) {
		DCC_LOG1(LOG_ERROR, "len(%d) > TFTP_OACK_OPT_MAX", len);
		return -1;
	}

	DBG("TFTP: OACK. len=%d", len);
	DCC_LOG(LOG_TRACE, "OACK....");

	pkt.th_opcode = htons(TFTP_OACK);
	memcpy(pkt.opt, opt, len);

	return udp_sendto(udp, &pkt, 2 + len, sin);
}

int tftp_decode_fname(struct debugger * dbg, char * fname)
{
	char * cp;
	unsigned int size;
	unsigned int addr;
	unsigned int end;

	/* 
	 * octet mode:
	 * if the file name starts with 0x convert it to the load address 
	 * as hexadecimal number
	 *
	 * File name format:
	 *
	 * Start only: 
	 *   0x[START]
	 *
	 *   The start address is specified, the size is defined by the 
	 *   debugger variable: transf.size
	 *
	 *   Ex: 
	 *   	0x0100000
	 *   	0x0
	 *
	 * Range: * 0x[START]:0x[END] or 0x[START]-0x[END]
	 *
	 *   Specify the initial (START) and final (END) addresses. 
	 *   The size is END - START. END must be great then START.
	 *  
	 *   Ex: 
	 *   	0x0100000:0x0100100
	 *   	0x0:0x0a0000
	 *
	 * Start and size: 0x[START].[SIZE] or 0x[START]_[SIZE] or [START]#[SIZE]
	 *
	 *   Specify the initial (START) address and data size. Size may be 
	 *   both a decimal or hexadecimal value.
	 *  
	 *   Ex: 
	 *   	0x0100000.0x100
	 *   	0x0100000_0x100
	 *   	0x0100000#0x100
	 *   	0x0200000_4096
	 *   	0x0200000_4k
	 *   	0x0200000_0x100000
	 *   	0x0200000_2M
	 */

	INF("FFTP: fname=\"%s\"", fname);

	if ((fname[0] == '0') && (fname[1] == 'x')) {
		
		addr = strtoul(fname, &cp, 16);

		DCC_LOG1(LOG_TRACE, "addr=0x%08x", addr);

		if (cp == NULL) {
			end = addr + (dbg->transf.size);
		} else {
			if ((*cp == ':') || (*cp == '-')) {
				cp++;
				end = strtoul(cp, NULL, 16);

				DCC_LOG1(LOG_TRACE, "end=0x%08x", end);
				if (end <= addr) {
/*					tftp_error(udp, &sin, TFTP_ENOTFOUND, 
							   "BAD ADDR."); */
					return -1;
				}
			} else {
				if ((*cp == '.') || (*cp == '_') || (*cp == '#')) {
					cp++;
					size = strtoul(cp, &cp, 0);
					if (cp != NULL) {
						if ((*cp == 'k') || (*cp == 'K')) {
							size *= 1024;
						} else {
							if ((*cp == 'm') || (*cp == 'M')) {
								size *= 1024 * 1024;
							}
						}
					}

					DCC_LOG1(LOG_TRACE, "size=%d", size);

					end = addr + size;
				} else {
					end = addr + (dbg->transf.size);
				}
			}
		}

		DCC_LOG2(LOG_TRACE, "region: %08x, %d", addr, end - addr);
		DBG("TFTP region: %08x, %d", addr, end - addr);

		/* set the address and size info in the debugger state */
		dbg->transf.base = addr;
		dbg->transf.size = end - addr;
	} else {
		ice_drv_t * ice = (ice_drv_t *)&dbg->ice;
		struct ice_mem_entry * e;
		char * cp;
		int c;

		for (cp = fname; (c = *cp); cp++) {
			if ((c == '.') || (c == ' ')) {
				*cp = '\0';
				break;
			}
		}

		if ((e = ice_mem_by_name(ice, dbg->mem, fname)) == NULL) {
			DCC_LOG(LOG_TRACE, "memory not found!");
			return 0;
		}

		DCC_LOG2(LOG_TRACE, "mem: %08x,%d", e->addr.base + e->addr.offs, 
				 e->blk.count * e->blk.size);

		dbg->transf.base = e->addr.base + e->addr.offs;
		dbg->transf.size = e->blk.count * e->blk.size;
	}

	return 0;
}

int tftp_recv_netascii(struct udp_pcb * udp, struct sockaddr_in * sin, 
					   int block, char * buf, int len)
{
	char * line;
	FILE * f;
	int ret;
	int n;
	int i;


	f = null_fopen(NULL);
	DCC_LOG1(LOG_TRACE, "1. f=%p", f);

	i = 0;
	n = 1;
	ret = 0;
	while (i < len) {
		for (line = &buf[i]; i < len; i++) {
			/* CR or [CR, LF] */
			if (buf[i] == '\r') {
				buf[i++] = '\0';
				if (buf[i] == '\n')
					i++;
				break;
			}
			/* LF or [LF, CR] */
			if (buf[i] == '\n') {
				buf[i++] = '\0';
				if (buf[i] == '\r')
					i++;
				break;
			}
		}

		if (i == len)
			buf[i] = '\0';


		if ((ret = exec(f, yard_ice_cmd_tab, line)) < 0) {
			DCC_LOG1(LOG_ERROR, "shell_exec(): %d", ret);
			break;
		}
		n++;
		DCC_LOG1(LOG_TRACE, "line %d", n);
	}

	if (ret < 0) {
		sprintf(buf, "ERROR %d on line %d: %s", -ret, n, target_strerror(ret));
		DCC_LOG1(LOG_TRACE, "2. f=%p", f);
		ret = tftp_error(udp, sin, TFTP_EACCESS, buf);
		DCC_LOG1(LOG_TRACE, "3. f=%p", f);
	} else {
		ret = tftp_ack(udp, block, sin);
	}

	DCC_LOG1(LOG_TRACE, "f=%p", f);
	fclose(f);

	return ret;
}

static const char hextab[] = { 
	'0', '1', '2', '3', '4', '5', '6', '7',
	'8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
};

int tftp_hex(unsigned int addr, unsigned char * buf, int size)
{
	unsigned char * s;
	unsigned char * cp;
	int n;
	int i;

	/* Read data at buffer's last half... */
	n = target_mem_read(addr, &buf[256], size);

	if (n < 0) {
		DCC_LOG(LOG_WARNING, "target memory read error.");
		n = 0;
	}

	s = &buf[256];
	cp = buf;
	/* convert to hexadecimal (in place) */
	for (i = 0; i < n; i++) {
		*cp++ = hextab[(s[i] >> 4) & 0xf];
		*cp++ = hextab[s[i] & 0xf];
	}

	return n * 2;
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

void __attribute__((noreturn)) tftp_daemon_task(struct debugger * dbg)
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

	DCC_LOG1(LOG_TRACE, "thread: %d", __os_thread_self());

	if ((udp = udp_alloc()) == NULL) {
		DCC_LOG(LOG_WARNING, "udp_alloc() fail!");
		abort();
	}

	if (udp_bind(udp, INADDR_ANY, htons(IPPORT_TFTP)) < 0) {
		DCC_LOG(LOG_WARNING, "udp_bind() fail!");
		abort();
	}

	for (;;) {
		if ((len = udp_recv(udp, buf, MAX_TFTP_MSG, &sin)) < 0) {
			if (len == -ECONNREFUSED) {
				DCC_LOG(LOG_WARNING, "udp_rcv ICMP error: ECONNREFUSED");
			}
			if (len == -EFAULT) {
				DCC_LOG(LOG_WARNING, "udp_rcv error: EFAULT");
			}
			if (len == -ENOTCONN) {
				DCC_LOG(LOG_WARNING, "udp_rcv error: ENOTCONN");
			}
			continue;
		}


		opc = htons(hdr->th_opcode);
		if ((opc != TFTP_RRQ) && (opc != TFTP_WRQ)) {
			DCC_LOG1(LOG_WARNING, "invalid opc: %d", opc);
			WARN("TFTP: invalid opc: %d", opc);
			continue;
		}

		if (udp_connect(udp, sin.sin_addr.s_addr, sin.sin_port) < 0) {
			DCC_LOG(LOG_WARNING, "udp_connect() error");
			WARN("TFTP: UDP connect failed!");
			continue;
		}

		DCC_LOG2(LOG_TRACE, "Connected to: %I.%d", sin.sin_addr.s_addr, 
				 ntohs(sin.sin_port));

//		INF("Connected to: %08x.%d", sin.sin_addr.s_addr, 
//			ntohs(sin.sin_port));

		for (;;) {
			DCC_LOG3(LOG_INFO, "%I.%d %d", 
					 sin.sin_addr.s_addr, ntohs(sin.sin_port), len);

			DCC_LOG2(LOG_INFO, "len=%d, opc=%s", len, tftp_opc[opc]);

			switch (opc) {
			case TFTP_RRQ:
				DCC_LOG(LOG_TRACE, "read request: ...");

				tftp_req_parse((char *)&(hdr->th_stuff), &req);
				blksize = req.blksize;

				INF("TFTP: RRQ '%s' '%s' blksize=%d", req.fname, 
					   tftp_mode[req.mode], req.blksize);

				if (tftp_decode_fname(dbg, req.fname) < 0) {
					ERR("TFTP: bad address.");
					tftp_error(udp, &sin, TFTP_ENOTFOUND, "BAD ADDR.");
					break;
				}

				/* set the transfer info */
				addr_start = dbg->transf.base;
				addr_end = addr_start + dbg->transf.size;
				block = 0;

				DCC_LOG2(LOG_TRACE, "start=0x%08x end=0x%08x", 
						 addr_start, addr_end);

				DBG("TFTP: start=0x%08x end=0x%08x", 
						 addr_start, addr_end);

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
				DCC_LOG(LOG_TRACE, "write request...");

				tftp_req_parse((char *)&(hdr->th_stuff), &req);
				blksize = req.blksize;

				INF("WRQ '%s' '%s' blksize=%d", req.fname, 
					   tftp_mode[req.mode], req.blksize);

				if (tftp_decode_fname(dbg, req.fname) < 0) {
					tftp_error(udp, &sin, TFTP_ENOTFOUND, "BAD ADDR.");
					break;
				}

				/* set the transfer info */
				addr_start = dbg->transf.base;
				addr_end = addr_start + dbg->transf.size;
				block = 0;

				DCC_LOG2(LOG_TRACE, "start=0x%08x end=0x%08x", 
						 addr_start, addr_end);

				DBG("TFTP: start=0x%08x end=0x%08x", 
						 addr_start, addr_end);

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
				DCC_LOG1(LOG_TRACE, "ACK: %d.", block);

				if (state == TFTPD_SEND_NETASCII) {
					unsigned int addr;
					int rem;
					int n;

send_netascii:
					addr = addr_start + (block * 256);
					rem = addr_end - addr;
					if (rem < 0) {
						state = TFTPD_IDLE;
						DCC_LOG1(LOG_TRACE, "eot: %d bytes sent.", 
								 addr_end - addr_start);
						break;
					}

					n = (rem < 256) ? rem : 256;

					DCC_LOG2(LOG_TRACE, "send netascii: addr=0x%08x n=%d", 
							 addr, n);

					/* build the packet */
					len = tftp_hex(addr, hdr->th_data, n);

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
						DCC_LOG1(LOG_TRACE, "eot: %d bytes sent.", 
								 addr_end - addr_start);
						break;
					}
					n = (rem < blksize) ? rem : blksize;

					DCC_LOG2(LOG_TRACE, "send octet: addr=0x%08x n=%d", 
							 addr, n);

					/* build the packet */
					len = target_mem_read(addr, hdr->th_data, n);

					if (len < 0) {
						DCC_LOG(LOG_WARNING, "target memory read error.");
						len = 0;
					}

send_data:
					hdr->th_opcode = htons(TFTP_DATA);
					hdr->th_block = htons(block + 1);

					DCC_LOG2(LOG_TRACE, "block %d: %d bytes.", 
							 block + 1,  len);

					if (udp_sendto(udp, hdr, 
								   sizeof(struct tftphdr) + len, &sin) < 0) {
						DCC_LOG(LOG_WARNING, "udp_sendto() fail");
						state = TFTPD_IDLE;
						break;
					}

					break;
				}

				DCC_LOG(LOG_WARNING, "state invalid!");
				break;

			case TFTP_DATA:
				/* skip the header */
				len -= 4;
				DCC_LOG2(LOG_TRACE, "block=%d len=%d", 
						 htons(hdr->th_block), len);

				DBG("TFTP: DATA block=%d len=%d", htons(hdr->th_block), len);

				if (htons(hdr->th_block) != (block + 1)) {
					/* retransmission, just ack */
					DCC_LOG2(LOG_WARNING, "retransmission, block=%d len=%d", 
							 block, len);
					tftp_ack(udp, block, &sin);
					break;
				}

				if (state == TFTPD_RECV_OCTET) {
					unsigned int addr;
					int n;

					addr = addr_start + (block * blksize);

					block++;

					if (len != blksize) {
						DCC_LOG(LOG_TRACE, "last packet...");
						state = TFTPD_IDLE;
						if (len == 0) {
							tftp_ack(udp, block, &sin);
							break;
						}
					} else {
						DCC_LOG2(LOG_TRACE, "rcvd octet: addr=0x%08x n=%d", 
								 addr, len);
						/* ACK the packet before writing to
						   speed up the transfer, errors are postponed... */
						tftp_ack(udp, block, &sin);
					}

					n = target_mem_write(addr, hdr->th_data, len);

					if (n < len) {
						if (n < 0) {
							DCC_LOG(LOG_ERROR, "target_mem_write()!");
							sprintf(msg, "TARGET WRITE FAIL: %08x", addr);
							WARN("memory write failed at "
								   "addr=0x%08x, code %d", addr, n);
						} else {
							DCC_LOG2(LOG_WARNING, "short writ: "
									 "ret(%d) < len(%d)!", n, len);
							sprintf(msg, "TARGET SHORT WRITE: %08x", 
									addr + n);
						}
						tftp_error(udp, &sin, TFTP_EUNDEF, msg);
						state = TFTPD_RECV_ERROR;
					} else {
						if (n > len) {
							DCC_LOG2(LOG_WARNING, "long write: "
									 "ret(%d) < len(%d)!", n, len);
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
					block++;
					if (len != blksize) {
						state = TFTPD_IDLE;
						if (len == 0) {
							tftp_ack(udp, block, &sin);
							break;
						}
					}
					DCC_LOG1(LOG_TRACE, "ASCII recv %d...", len);
					tftp_recv_netascii(udp, &sin, block, 
									   (char *)hdr->th_data, len);
					break;
				}

				tftp_error(udp, &sin, TFTP_EUNDEF, NULL);
				break;

			case TFTP_ERROR:
				DCC_LOG2(LOG_TRACE, "error: %d: %s.", 
						 htons(hdr->th_code), hdr->th_data);
				break;

			}

			if (state == TFTPD_IDLE) {
				DCC_LOG(LOG_TRACE, "[IDLE]");
				break;
			}

			DBG("TFTP: UDP receive ...");

			if ((len = udp_recv_tmo(udp, buf, MAX_TFTP_MSG, &sin, 5000)) < 0) {
				if (len == -ETIMEDOUT) {
					DCC_LOG(LOG_WARNING, "udp_recv_tmo() timeout!");
					WARN("TFTP: UDP receive timeout!");
				} else {
					if (len == -ECONNREFUSED) {
						DCC_LOG(LOG_WARNING, "udp_recv_tmo() lost peer!");
						WARN("TFTP: UDP peer lost!");
					} else {
						DCC_LOG(LOG_WARNING, "udp_recv_tmo() failed!");
						WARN("TFTP: UDP receive failed!");
					}
				}
				/* break the inner loop */
				break;
			}

			opc = htons(hdr->th_opcode);
		}

		/* disconnect */
		DCC_LOG(LOG_TRACE, "disconnecting.");
		udp_connect(udp, INADDR_ANY, 0);
	}
}

uint32_t tftpd_stack[384 + (MAX_TFTP_SEGSIZE / 4)];

const struct thinkos_thread_inf tftpd_inf = {
	.stack_ptr = tftpd_stack, 
	.stack_size = sizeof(tftpd_stack),
	.priority = __OS_PRIORITY_HIGHEST,
	.thread_id = 2,
	.paused = false,
	.tag = "TFTPD"
};

int tftpd_start(void)
{
	int th;

	th = thinkos_thread_create_inf((void *)tftp_daemon_task, 
								   (void *)&debugger, &tftpd_inf);
	(void)th;

	INF("TFTPD started th=%d", th);

	return 0;
}

