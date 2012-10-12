/* $Id: udp.c,v 2.16 2008/06/04 00:03:15 bob Exp $ 
 *
 * File:	udp_release.c
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

int udp_release(struct udp_pcb * __inp)
{
	return pcb_release((struct pcb *)__inp, &__udp__.list);
}

