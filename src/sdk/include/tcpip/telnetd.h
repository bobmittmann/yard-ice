/* 
 * Copyright(c) 2004-2012 BORESTE (www.boreste.com). All Rights Reserved.
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
 * @file tcpip/telnetd.h
 * @brief 
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __TCPIP_TELNETD_H__
#define __TCPIP_TELNETD_H__

#include <tcpip/tcp.h>
#include <arpa/inet.h>

struct telnet_state;

/* Termios Like flags */

/* Canonical mode */
#define TELNET_ICANON 0x08
/* Binary mode */
#define TELNET_BINARY 0x10
/* Echo iput characteres */
#define TELNET_ECHO 0x20
/* Map NL to CR-NL on output */
#define TELNET_ONLCR 0x40
/* Map CR to NL on output */
#define TELNET_OCRNL 0x80

#ifdef __cplusplus
extern "C" {
#endif

/*
 * RAW interface telnet server
 */
struct telnet_state * telnetd_accept(struct tcp_pcb * mux, unsigned int flags);

void * telnet_attach(struct telnet_state * tn, void * data);

void * telnet_get_arg(struct telnet_state * tn);

int telnet_write(struct telnet_state * tn, const void * buf, int len);

int telnet_read(struct telnet_state * tn, void * buf, int len);

int telnet_close(struct telnet_state * tn);

/*
 * File IO telnet server
 */

/*! \brief Starts TELNET server
 *  Starts listening for TELNET connections
 *
 *	\param port \n
 *  tcp port number in host byte order
 *	\param backlog\n
 *  tcp listen backlog (the number of allowed pending connections)
 *	\return
 *  On success, a TCP control block. On fail, NULL is returned instead.
 */
struct tcp_pcb * telnetd_start(int port, int backlog);

/*! \brief Stops TELNET server
 *
 *	\param telnetd_tp \n
 *  TCP protocol control block of the TELNET server
 *	\return
 *  Returns zero, on success,  othervise a negative number.
 */
void telnetd_stop(struct tcp_pcb * telnetd_tp);

/*! \brief Accepts an incomming TELNET connection
 *
 *	\param telnetd_tp \n
 *  TCP protocol control block of the TELNET server
 *	\param binary\n
 *  enable binary mode
 *	\param echo\n
 *  enable echo
 *	\return
 *  Returns a file descriptor, on success,  othervise a negative number.
 */
#include <stdio.h>

FILE * telnetd_fopen(struct tcp_pcb * tnd_tp, unsigned int flags);

#ifdef __cplusplus
}
#endif

#endif /* __TCPIP_TELNETD_H__ */

