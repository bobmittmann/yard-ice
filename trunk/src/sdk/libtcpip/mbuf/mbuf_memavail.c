/* $Id: mbuf_memavail.c,v 1.1 2008/05/23 03:54:45 bob Exp $ 
 *
 * File:	mbuf_memavail.c
 * Module:
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com, bob@methafora.com.br)
 * Target:	
 * Comment:
 * Copyright(c) 2005-2009 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#include "__mbuf.h"

int mbuf_memavail(void)
{
	return (__mbufs__.max - __mbufs__.used) * MBUF_DATA_SIZE;
}

