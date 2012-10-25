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


#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/reboot.h>
#include <sys/shell.h>
#include <sys/null.h>

#include <tcpip/ethif.h>
#include <tcpip/route.h>
#include <tcpip/pcb.h>
#include <tcpip/udp.h>

#include <arpa/tftp.h>

#include <debugger.h>
#include <command.h>
#include <tftpd.h>
#include <errno.h>

#ifdef TFTP_DEBUG
#ifndef DEBUG
#define DEBUG
#endif
#endif
#include <debug.h>

#include <sys/dcclog.h>


#define MAX_TFTP_MSG (TFTP_SEGSIZE  + sizeof(struct tftphdr))

const char * const tftp_opc[] = {
	"UNKNOWN", "RRQ", "WRQ", "DATA", "ACK", "ERROR"
};

const char * const tftp_err[] = {
	"EUNDEF", "ENOTFOUND", "EACCESS", "ENOSPACE", 
	"EBADOP", "EBADID", "EEXISTS", "ENOUSER"
};

enum tftp_st {
	TFTPD_IDLE = 0,
	TFTPD_RECV_NETASCII = 1,
	TFTPD_RECV_OCTET = 2,
	TFTPD_SEND_NETASCII = 3,
	TFTPD_SEND_OCTET = 4,
	TFTPD_RECV_ERROR = 5
};

#define TFTP_MSG_MAX 32

struct tftp_msg {
	struct tftphdr hdr;
	uint8_t payload[TFTP_MSG_MAX];
};

int tftp_error(struct udp_pcb * udp, struct sockaddr_in * sin, 
			   int errno, char * msg)
{
	int n;
	struct tftp_msg pkt;

	if (msg == NULL) {
		msg = (char *)tftp_err[errno];
	}

	n = sizeof(struct tftphdr) + strlen(msg) + 1;

	pkt.hdr.th_opcode = HTONS(TFTP_ERROR);
	pkt.hdr.th_code = htons(errno);
	strcpy((char *)pkt.hdr.th_msg, msg);

	return udp_sendto(udp, &pkt, n, sin);
}

int tftp_ack(struct udp_pcb * udp, int block, struct sockaddr_in * sin)
{
	struct tftphdr hdr;
	int ret;

	DCC_LOG1(LOG_INFO, "block: %d", block);

	hdr.th_opcode = HTONS(TFTP_ACK);
	hdr.th_block = htons(block);

	if ((ret = udp_sendto(udp, &hdr, sizeof(struct tftphdr), sin)) < 0) {
		DCC_LOG(LOG_WARNING, "udp_sendto() fail");
	}

	return ret;
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

	if ((fname[0] == '0') && (fname[1] == 'x')) {
		addr = strtoul(fname, &cp, 16);

		DCC_LOG1(LOG_INFO, "addr=0x%08x", addr);

		if (cp == NULL) {
			end = addr + (dbg->transf.size);
		} else {
			if ((*cp == ':') || (*cp == '-')) {
				cp++;
				end = strtoul(cp, NULL, 16);

				DCC_LOG1(LOG_INFO, "end=0x%08x", end);
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

					DCC_LOG1(LOG_INFO, "size=%d", size);

					end = addr + size;
				} else {
					end = addr + (dbg->transf.size);
				}
			}
		}

		DCC_LOG2(LOG_INFO, "region: %08x, %d", addr, end - addr);

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
			DCC_LOG(LOG_INFO, "memory not found!");
			return 0;
		}

		DCC_LOG2(LOG_INFO, "mem: %08x,%d", e->addr.base + e->addr.offs, 
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

//	f = stdout;
	f = null;

	i = 0;
	n = 1;
	ret = 0;
	while (i < len) {
		for (line = &buf[i]; i < len; i++) {
			if (buf[i] == '\r') {
				buf[i++] = '\0';
				if (buf[i] == '\n')
					i++;
				break;
			}
		}

		if (i == len)
			buf[i] = '\0';


		if ((ret = exec(f, line)) < 0) {
			DCC_LOG1(LOG_ERROR, "shell_exec(): %d", ret);
			break;
		}
		n++;
		DCC_LOG1(LOG_TRACE, "line %d", n);
	}

	if (ret < 0) {
		sprintf(buf, "ERROR %d on line %d: %s", -ret, n, target_strerror(ret));
		ret = tftp_error(udp, sin, TFTP_EACCESS, buf);
	} else {
		ret = tftp_ack(udp, block, sin);
	}

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

int tftp_daemon(struct debugger * dbg, uthread_id_t id)
{
	uint8_t buf[MAX_TFTP_MSG];
	struct tftphdr * hdr = (struct tftphdr *)buf;
	char * msg = (char *)buf;
	struct sockaddr_in sin;
	struct udp_pcb * udp;
	char * fname;
	char * mode;
	int state = TFTPD_IDLE;
	unsigned int addr_start = 0;
	unsigned int addr_end = 0;
	int block = 0;
	int opc;
	int len;

	DCC_LOG1(LOG_INFO, "thread: %d", id);

	if ((udp = udp_alloc()) == NULL) {
		DCC_LOG(LOG_WARNING, "udp_alloc() fail!");
		return -1;
	}

	if (udp_bind(udp, INADDR_ANY, HTONS(IPPORT_TFTP)) < 0) {
		DCC_LOG(LOG_WARNING, "udp_bind() fail!");
		return -1;
	}

	for (;;) {
		if ((len = udp_recv(udp, buf, MAX_TFTP_MSG, &sin)) < 0) {
			if (errno == ECONNREFUSED) {
				DCC_LOG(LOG_WARNING, "udp_rcv ICMP error: ECONNREFUSED");
			}
			if (errno == EFAULT) {
				DCC_LOG(LOG_WARNING, "udp_rcv error: EFAULT");
			}
			if (errno == ENOTCONN) {
				DCC_LOG(LOG_WARNING, "udp_rcv error: ENOTCONN");
			}
			continue;
		}

		opc = htons(hdr->th_opcode);
		if ((opc != TFTP_RRQ) && (opc != TFTP_WRQ)) {
			DCC_LOG1(LOG_WARNING, "invalid opc: %d", opc);
			continue;
		}

		if (udp_connect(udp, sin.sin_addr.s_addr, sin.sin_port) < 0) {
			DCC_LOG(LOG_WARNING, "udp_connect() error");
			continue;
		}

		DCC_LOG2(LOG_TRACE, "Connected to: %I.%d", sin.sin_addr.s_addr, 
				 ntohs(sin.sin_port));

		for (;;) {
			DCC_LOG3(LOG_INFO, "%I.%d %d", 
					 sin.sin_addr.s_addr, ntohs(sin.sin_port), len);

			DCC_LOG2(LOG_INFO, "len=%d, opc=%s", len, tftp_opc[opc]);

			switch (opc) {
			case TFTP_RRQ:
				/* Read Request */
				fname = (char *)&(hdr->th_stuff);
				mode = fname + strlen(fname) + 1;

				DCC_LOG(LOG_INFO, "read request: ...");

				if (tftp_decode_fname(dbg, fname) < 0) {
					tftp_error(udp, &sin, TFTP_ENOTFOUND, "BAD ADDR.");
					break;
				}

				/* set the transfer info */
				addr_start = dbg->transf.base;
				addr_end = addr_start + dbg->transf.size;
				block = 0;

				DCC_LOG2(LOG_INFO, "start=0x%08x end=0x%08x", 
						 addr_start, addr_end);

				if (strcmp(mode, "netascii") == 0) {
					state = TFTPD_SEND_NETASCII;
					goto send_netascii;
				} 

				if (strcmp(mode, "octet") == 0) {
					state = TFTPD_SEND_OCTET;
					goto send_octet;
				}

				tftp_error(udp, &sin, TFTP_EUNDEF, NULL);
				break;

			case TFTP_WRQ:
				/* Write Request */
				fname = (char *)&(hdr->th_stuff);
				mode = fname + strlen(fname) + 1;
				DCC_LOG(LOG_INFO, "write request...");

				if (tftp_decode_fname(dbg, fname) < 0) {
					tftp_error(udp, &sin, TFTP_ENOTFOUND, "BAD ADDR.");
					break;
				}

				/* set the transfer info */
				addr_start = dbg->transf.base;
				addr_end = addr_start + dbg->transf.size;
				block = 0;

				DCC_LOG2(LOG_INFO, "start=0x%08x end=0x%08x", 
						 addr_start, addr_end);

				if (strcmp(mode, "netascii") == 0) {
					state = TFTPD_RECV_NETASCII;
					tftp_ack(udp, block, &sin);
					break;
				} 

				if (strcmp(mode, "octet") == 0) {
					state = TFTPD_RECV_OCTET;
					tftp_ack(udp, block, &sin);
					break;
				}

				tftp_error(udp, &sin, TFTP_EUNDEF, NULL);
				break;

			case TFTP_ACK:
				block = htons(hdr->th_block);
				DCC_LOG1(LOG_INFO, "ACK: %d.", block);

				if (state == TFTPD_SEND_NETASCII) {
					unsigned int addr;
					int rem;
					int n;

send_netascii:
					addr = addr_start + (block * 256);
					rem = addr_end - addr;
					if (rem < 0) {
						state = TFTPD_IDLE;
						DCC_LOG1(LOG_INFO, "eot: %d bytes sent.", 
								 addr_end - addr_start);
						break;
					}

					n = (rem < 256) ? rem : 256;

					DCC_LOG2(LOG_INFO, "send netascii: addr=0x%08x n=%d", 
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
					addr = addr_start + (block * TFTP_SEGSIZE);
					rem = addr_end - addr;
					if (rem < 0) {
						state = TFTPD_IDLE;
						DCC_LOG1(LOG_INFO, "eot: %d bytes sent.", 
								 addr_end - addr_start);
						break;
					}
					n = (rem < TFTP_SEGSIZE) ? rem : TFTP_SEGSIZE;

					DCC_LOG2(LOG_INFO, "send octet: addr=0x%08x n=%d", addr, n);

					/* build the packet */
					len = target_mem_read(addr, &hdr->th_data, n);

					if (len < 0) {
						DCC_LOG(LOG_WARNING, "target memory read error.");
						len = 0;
					}

send_data:
					hdr->th_opcode = htons(TFTP_DATA);
					hdr->th_block = htons(block + 1);

					DCC_LOG2(LOG_INFO, "block %d: %d bytes.", block + 1,  len);

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
				DCC_LOG2(LOG_INFO, "block=%d len=%d", 
						 htons(hdr->th_block), len);

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

					addr = addr_start + (block * TFTP_SEGSIZE);

					block++;

					if (len != TFTP_SEGSIZE) {
						DCC_LOG(LOG_INFO, "last packet...");
						state = TFTPD_IDLE;
						if (len == 0) {
							tftp_ack(udp, block, &sin);
							break;
						}
					} else {
						DCC_LOG2(LOG_INFO, "rcvd octet: addr=0x%08x n=%d", 
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
						} else {
							DCC_LOG2(LOG_WARNING, "short read: "
									 "ret(%d) < len(%d)!", n, len);
							sprintf(msg, "TARGET SHORT WRITE: %08x", 
									addr + n);
						}
						tftp_error(udp, &sin, TFTP_EUNDEF, msg);
						state = TFTPD_RECV_ERROR;
					} else {
						if (n > len) {
							DCC_LOG2(LOG_WARNING, "long read: "
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
					if (len != TFTP_SEGSIZE) {
						state = TFTPD_IDLE;
						if (len == 0) {
							tftp_ack(udp, block, &sin);
							break;
						}
					}
					DCC_LOG1(LOG_TRACE, "ASCII recv %d...", len);
					tftp_recv_netascii(udp, &sin, block++, 
									   (char *)hdr->th_data, len);
					break;
				}

				tftp_error(udp, &sin, TFTP_EUNDEF, NULL);
				break;

			case TFTP_ERROR:
				DCC_LOG2(LOG_INFO, "error: %d: %s.", 
						 htons(hdr->th_code), hdr->th_data);
				break;

			}

			if (state == TFTPD_IDLE) {
				DCC_LOG(LOG_TRACE, "[IDLE]");
				break;
			}

			if ((len = udp_recv_tmo(udp, buf, MAX_TFTP_MSG, &sin, 5000)) < 0) {
				if (len == -ETIMEDOUT) {
					DCC_LOG(LOG_WARNING, "udp_recv_tmo() timeout!");
				} else {
					if (errno == ECONNREFUSED) {
						DCC_LOG(LOG_WARNING, "udp_recv_tmo() lost peer!");
					} else {
						DCC_LOG(LOG_WARNING, "udp_recv_tmo() fial!");
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

	return 0;
}

uint32_t tftp_stack[512];

int tftpd_start(void)
{
	int th;

	th = uthread_create(tftp_stack, sizeof(tftp_stack), 
				   (uthread_task_t)tftp_daemon, (void *)&debugger,
				   0, NULL); 

	printf("<%d> ", th);

	return 0;
}
