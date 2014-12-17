/* $Id: auth.c,v 2.2 2006/04/11 14:39:35 bob Exp $ 
 *
 * File:	auth.c
 * Module:  
 * Project:
 * Author:	Robinson Mittmann (bob@boreste.com, bob@methafora.com.br)
 * Target:
 * Comment:
 * Copyright(c) 2004-2006 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <tcpip/httpd.h>
#include "httpdefs.h"

#ifdef HTTPD_DEBUG
#ifndef DEBUG
#define DEBUG
#endif
#endif
#include <debug.h>

unsigned int httpd_auth(struct httpd * __httpd,
						char * __usr, char * __pwd)
{
	char * cp;
	int i;
	int n;
	int size;
	int uid;
	
	if (__usr == NULL) {
		DBG(DBG_WARNING, "__usr==NULL");
		return (unsigned int)-1;
	}

	if (__pwd == NULL) {
		DBG(DBG_WARNING, "__pwd==NULL");
		return (unsigned int)-1;
	}
	
	if ((cp = (char *)__httpd->auth_data_buf) == NULL) {
		DBG(DBG_WARNING, "no authorization data");
		return (unsigned int)-1;
	}

	if ((size = __httpd->auth_data_size) < 4) {
		DBG(DBG_WARNING, "authorization data buffer is empty");
		return (unsigned int)-1;
	}

	/* remove the leading blanks */
	for (; *__usr == ' '; __usr++);
	for (; *__pwd == ' '; __pwd++);

	/* first try the cache */
	n = 0;
	if (__httpd->auth_last) {
		cp += __httpd->auth_last;
		for(i = 0; (__usr[i] != '\0') && (cp[n] != ':') && 
			(__usr[i] == cp[n]); i++, n++);
		
		if (((__usr[i] == '\0') || (__usr[i] == ' ')) && (cp[n] == ':')) {
			DBG(DBG_NOTICE, "cache hint");
			goto user_match;
		}
		DBG(DBG_NOTICE, "cache miss");
		n = 0;
		cp = (char *)__httpd->auth_data_buf;
	}

	do {
		/* save the beginig of the user name at the cache */
		__httpd->auth_last = n;		
		for(i = 0; (__usr[i] != '\0') && (cp[n] != ':') && 
			(__usr[i] == cp[n]); i++, n++);
		
		if (((__usr[i] == '\0') || (__usr[i] == ' ')) && (cp[n] == ':')) {
			goto user_match;
		}

		for (; (cp[n] != '\n') && (n < size); n++);
		for (; (cp[n] == '\n'); n++);

	} while (n < size);

	/* user not found */
	/* clear the cache */
	__httpd->auth_last = 0;
	return (unsigned int)-1;

user_match:
	DBG(DBG_NOTICE, "user match");	
	/* user match */
	n++;
	if (cp[n] == '*') {
		/* login denied */
		return (unsigned int)-1;
	}

	for(i = 0; (__pwd[i] != '\0') && (cp[n] != ':') && 
		(__pwd[i] == cp[n]); i++, n++);
	
	if (((__pwd[i] == '\0') || (__pwd[i] == ' ')) && (cp[n] == ':')) {
		/* password match */				
		n++;
		for (uid = cp[n++] - '0'; cp[n] != ':'; n++)
			uid = (uid * 10) + cp[n] - '0';

		DBG(DBG_NOTICE, "authenticated uid=%d", uid);	
		return uid;
	}

	DBG(DBG_NOTICE, "bad password");
	/* bad password */
	return (unsigned int)-1;
}

