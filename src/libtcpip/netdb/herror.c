/* $Id: herror.c,v 2.0 2006/03/28 15:10:22 bob Exp $ 
 *
 * File:	herror.c
 * Module:
 * Project:		
 * Author:	Robinson Mittmann (bob@boreste.com, bob@methafora.com.br)
 * Target:	
 * Comment:
 * Copyright(c) 2004-2005 CNX Technologies. All Rights Reserved.
 *
 */

#ifdef __CONFIG__
#include "config.h"
#endif

#include <netdb.h>
#include <errno.h>

#ifdef NETDB_DEBUG
#ifndef DEBUG
#define DEBUG
#endif
#endif
#include <debug.h>

int *__h_errno_location(void)
{
	return &errno;
}

const char * hstrerror(int __err_num)
{
	return NULL;
}

void herror(const char *__str)
{

}

