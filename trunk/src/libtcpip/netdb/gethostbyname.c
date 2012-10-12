/* $Id: gethostbyname.c,v 2.0 2006/03/28 15:10:22 bob Exp $ 
 *
 * File:	gethostbyname.c
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

#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>

#ifdef NETDB_DEBUG
#ifndef DEBUG
#define DEBUG
#endif
#endif
#include <debug.h>

struct hostent * gethostbyname(const char *__name)
{
	return NULL;
}

