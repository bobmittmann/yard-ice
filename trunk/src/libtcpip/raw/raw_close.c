/* $Id: raw.c,v 2.1 2008/06/04 00:03:15 bob Exp $ 
 *
 * File:	raw_close.c
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

int raw_close(struct raw_pcb * __raw)
{
	int ret;

	tcpip_net_lock();

	ret = raw_pcb_free(__raw);
	
	tcpip_net_unlock();

	return ret;
}

