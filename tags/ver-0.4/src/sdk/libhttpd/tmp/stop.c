/* $Id: stop.c,v 2.1 2006/04/06 18:16:26 bob Exp $ 
 *
 * File:	stop.c
 * Module:  
 * Project:
 * Author:	Robinson Mittmann (bob@boreste.com)
 * Target:
 * Comment:
 * Copyright(c) 2004-2006 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#ifdef __CONFIG__
#include "config.h"
#endif

#include <stdlib.h>

#include <tcpip/httpd.h>
#include "httpdefs.h"

#ifdef HTTPD_DEBUG
#ifndef DEBUG
#define DEBUG
#endif
#endif
#include <debug.h>

int httpd_stop(struct tcp_pcb * tp)
{
/*	struct httpctl * http; */
	unsigned int bm;
	struct httpd * httpd;
	int i;

	httpd = (struct httpd *)tp->t_handle;

	/* Just close the tcp listener */
	tcp_close(tp);

	/* release the resources and the services */
	bm = httpd->mem_bmp;
	for (i = 0; (bm); bm >>= 1, i++) {
		if (bm & 1) {
			
/*			http = (struct httpctl *)httpd_mem_pool[i];
			if (http->tp != NULL)
				tcp_close(http->tp); */
			
			/* update the allocation bitmap */			
			httpd->mem_bmp &= ~(1 << i);
		}
	}

	return 0;
}

