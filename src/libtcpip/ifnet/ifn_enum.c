/* $Id: ifnet.c,v 2.7 2008/06/04 00:03:15 bob Exp $ 
 *
 * File:	ifn_enum.c
 * Module:
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com, bob@methafora.com.br)
 * Target:	
 * Comment:
 * Copyright(c) 2004-2009 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#define __USE_SYS_IFNET__
#include <sys/ifnet.h>

int ifn_enum(int (* __callback)(struct ifnet *, void *), void * __parm)
{
	int i;
	int n;

	n = 0;
	for (i = 0; i < ifnet_max; i++) {
		if (__ifnet__[i].if_id != 0) {
			if (__callback(&__ifnet__[i], __parm))
				break;
		}
	}

	return n;
}

