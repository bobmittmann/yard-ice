/* 
 * File:	tcp_proto_getstat.c
 * Module:
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com, bobmittmann@gmail.com)
 * Target:	
 * Comment:
 * Copyright(c) 2009 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#include <tcpip/tcp.h>
#include <tcpip/stat.h>
#include <string.h>

void tcp_proto_getstat(struct proto_stat * __st, int __rst)
{
#if ENABLE_TCP_PROTO_STAT
	proto_stat_copy(__st, &__tcp__.stat, __rst);
#else
	memset(__st, 0, sizeof(struct proto_stat)); 
#endif
}

