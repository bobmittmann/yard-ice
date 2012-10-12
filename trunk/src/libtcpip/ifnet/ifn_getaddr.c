/* $Id: ifnet.c,v 2.7 2008/06/04 00:03:15 bob Exp $ 
 *
 * File:	ifn_getaddr.c
 * Module:
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com, bobmittmann@gmail.com)
 * Target:	
 * Comment:
 * Copyright(c) 2004-2009 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#define __USE_SYS_IFNET__
#include <sys/ifnet.h>

int ifn_getaddr(struct ifnet * __if, uint8_t * __buf)
{
	return __if->if_op->op_getaddr(__if, __buf);
}
