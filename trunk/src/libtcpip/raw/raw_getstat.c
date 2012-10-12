/* 
 * File:	raw_getstat.c
 * Module:
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com, bobmittmann@gmail.com)
 * Target:	
 * Comment:
 * Copyright(c) 2009 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#define __USE_SYS_RAW__
#include <sys/raw.h>

#include <string.h>

void raw_getstat(struct proto_stat * __st, int __rst)
{
#if ENABLE_RAW_STAT
	proto_stat_copy(__st, &raw_stat, __rst);
#else
	memset(__st, 0, sizeof(struct proto_stat));
#endif
}

