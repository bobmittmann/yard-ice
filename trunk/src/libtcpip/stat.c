/* 
 * File:	tcpip/stat.c
 * Module:	
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com, bobmittmann@gmail.com)
 * Target:	
 * Comment:
 * Copyright(c) 2003-2009 BORESTE (www.boreste.com). All Rights Reserved.
 *
 *
 */

#define __USE_SYS_NET__
#include <sys/net.h>

#include <tcpip/stat.h>
#include <string.h>
#include <stdlib.h>

void proto_stat_copy(struct proto_stat * __dst, 
					 struct proto_stat * __src, int __rst)
{
	tcpip_net_lock();

	if (__dst != NULL) {
		memcpy(__dst, __src, sizeof(struct proto_stat)); 
	}

	if (__rst) {
		memset(__src, 0, sizeof(struct proto_stat)); 
	}

	tcpip_net_unlock();
}


