/* $Id: mbuf.c,v 2.5 2008/05/28 22:32:21 bob Exp $ 
 *
 * File:	mbuf.c
 * Module:
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com, bob@methafora.com.br)
 * Target:	
 * Comment:
 * Copyright(c) 2005-2008 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#include "__mbuf.h"

int mbuf_max(void) {
	return __mbufs__.max;
};

