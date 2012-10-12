/* 
 * File:	udp_proto_getstat.c
 * Module:
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com, bobmittmann@gmail.com)
 * Target:	
 * Comment:
 * Copyright(c) 2009 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#include <tcpip/udp.h>
#include <tcpip/stat.h>
#include <string.h>

void udp_proto_getstat(struct proto_stat * __st, int __rst)
{
#if ENABLE_UDP_PROTO_STAT
	proto_stat_copy(__st, &__udp__.stat, __rst);
#else
	memset(__st, 0, sizeof(struct proto_stat)); 
#endif
}

