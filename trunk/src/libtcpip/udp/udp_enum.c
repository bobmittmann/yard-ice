/* $Id: udp.c,v 2.16 2008/06/04 00:03:15 bob Exp $ 
 *
 * File:	udp_enum.c
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

int udp_enum(int (* __callback)(struct udp_pcb *, void *), void * __parm)
{
	return pcb_enum((int (*)(struct pcb *, void *))__callback,
					  __parm, &__udp__.list);
}

