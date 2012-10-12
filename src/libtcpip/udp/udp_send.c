/* $Id: udp.c,v 2.16 2008/06/04 00:03:15 bob Exp $ 
 *
 * File:	udp_send.c
 * Module:
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com, bob@methafora.com.br)
 * Target:	
 * Comment:
 * Copyright(c) 2004-2008 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#define __USE_SYS_UDP__
#include <sys/udp.h>

#include <stdlib.h>

int udp_send(struct udp_pcb * __up, void * __buf, int __len)
{
	return udp_sendto(__up, __buf, __len, NULL);
}

