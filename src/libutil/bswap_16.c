/* $Id: ntohl.c,v 2.0 2006/03/28 15:10:27 bob Exp $ 
 *
 * File:	ntohl.c
 * Module:
 * Project:
 * Author:	Robinson Mittmann (bob@cnxtech.com)
 * Target:
 * Comment:	
 * Copyright(c) 2004 CNX Technologies. All Rights Reserved.
 *
 */

#include <stdint.h>

uint16_t __bswap_16(uint16_t __val)
{
	return ((__val & 0xff) << 8) | ((__val & 0xff00) >> 8);
}

