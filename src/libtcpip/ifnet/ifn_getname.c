/* $Id: ifnet.c,v 2.7 2008/06/04 00:03:15 bob Exp $ 
 *
 * File:	ifn_getname.c
 * Module:
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com, bob@methafora.com.br)
 * Target:	
 * Comment:
 * Copyright(c) 2004-2008 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#define __USE_SYS_IFNET__
#include <sys/ifnet.h>

#include <stdio.h>

#ifdef IFNET_DEBUG
#ifndef DEBUG
#define DEBUG
#endif
#endif

const char ifn_type_name[][4] = {
	"xxx",
	"eth",
	"lo",
	"sl",
	"ppp",
	"atm"
	""
};

int ifn_getname(struct ifnet * __if, char * __s)
{
	return sprintf(__s, "%s%d", ifn_type_name[__if->if_id >> 4], 
				   __if->if_id & 0x0f);
}

