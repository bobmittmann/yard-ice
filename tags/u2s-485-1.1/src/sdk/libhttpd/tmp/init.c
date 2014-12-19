/* $Id: init.c,v 2.2 2006/04/11 14:39:35 bob Exp $ 
 *
 * File:	init.c
 * Module:  
 * Project:
 * Author:	Robinson Mittmann (bob@boreste.com, bob@methafora.com.br)
 * Target:
 * Comment:
 * Copyright(c) 2004-2006 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#ifdef __CONFIG__
#include "config.h"
#endif

#include <stdlib.h>
#include <arpa/inet.h>

#include <tcpip/httpd.h>
#include "httpdefs.h"

#ifdef HTTPD_DEBUG
#ifndef DEBUG
#define DEBUG
#endif
#endif
#include <debug.h>

struct httpd * httpd_init(struct tcp_pcb * __tp, const char * __root, 
						  int __uid, const void * __auth_data, int __auth_size)
{
	struct httpd * httpd;

	/* TODO: if PCB is NULL ?? */ 
	httpd = __tp->t_handle;

	/* register the listening tcp protocol control block (PCB) */
	if (tcp_listen(__tp, NULL, NULL) != 0) {
		return NULL;
	}
	
	if (__root != NULL)
		httpd->root = (char *)__root;
	else
		httpd->root = HTTPD_ROOT;
	
	/* clear the one-behind cache */
	httpd->auth_last = 0;
	if ((__auth_data != NULL) && (__auth_size > 4)) {
		/* authentication data */
		httpd->auth_data_buf = (unsigned char *)__auth_data;
		/* the size of authentication data */
		httpd->auth_data_size = __auth_size;
	} else {
		httpd->auth_data_buf = NULL;
		httpd->auth_data_size = 0;
	}

	httpd->uid = __uid;
	/* initialize the memory allocation bitmap */
	httpd->mem_bmp = (unsigned char) (0xfe << (HTTPD_MAX_CONNECT - 1));
	
	return httpd;
}

