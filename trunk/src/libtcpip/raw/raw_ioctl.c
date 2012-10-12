/* $Id: raw.c,v 2.1 2008/06/04 00:03:15 bob Exp $ 
 *
 * File:	raw_ioctl.c
 * Module:
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com, bob@methafora.com.br)
 * Target:	
 * Comment:
 * Copyright(c) 2004-2008 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#define __USE_SYS_RAW__
#include <sys/raw.h>

#include <stdlib.h>
#include <sys/ioctl.h>

int raw_ioctl(struct raw_pcb * __raw, int __cmd, void * __data)
{
	int * flag;

	if (ip_raw != __raw)
		return -1;

	if ((__cmd == FIONBIO) && ((flag = (int *)__data) != NULL)) {
		if (*flag)
			__raw->r_flags |= RF_NONBLOCK;
		else
			__raw->r_flags &= ~RF_NONBLOCK;
		return 0;
	}

	return -1;
}

