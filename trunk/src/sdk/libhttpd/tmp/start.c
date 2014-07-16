/* $Id: start.c,v 2.3 2006/10/18 20:02:09 bob Exp $ 
 *
 * File:	start.c
 * Module:  
 * Project:
 * Author:	Robinson Mittmann (bob@boreste.com)
 * Target:
 * Comment:
 * Copyright(c) 2004-2006 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#ifdef __CONFIG__
#include "config.h"
#endif

#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/file.h>
#include <sys/fs.h>

#include <tcpip/httpd.h>
#include "httpdefs.h"

#ifdef HTTPD_DEBUG
#ifndef DEBUG
#define DEBUG
#endif
#endif
#include <debug.h>

void httpd_listen_callback(struct tcp_pcb * tp, int event, 
						  struct httpd * httpd);

struct httpd httpd_80 = {
	/* auth data ptr */
	NULL,
	/* auth data size */
	0,
	/* auth cache */
	0,
	/* server uid */
	HTTPD_UID,
	/* mem bitmap */
	(unsigned char) (0xfe << (HTTPD_MAX_CONNECT - 1)),
};

const struct tcp_pcb httpd80_tp = TCP_LISTEN_TEMPLATE(
	INADDR_ANY, 
	80, 
	(void *)&httpd_80, 
	(tcp_callback_t)httpd_listen_callback, 
	0);

static int getuid(char * usr, char * cp, int size)
{
	int n;
	int uid;
	int i;

	DBG(DBG_TRACE, "usr='%s' size=%d", usr, size);	

	n = 0;
	do {
		DBG(DBG_MSG, "%c%c%c%c",  cp[n], cp[n+1], cp[n+2], cp[n+3]);	

		for(i = 0; (usr[i] != '\0') && (cp[n] != ':') && 
			(usr[i] == cp[n]); i++, n++);
		
		if (((usr[i] == '\0') || (usr[i] == ' ')) && (cp[n] == ':')) {

			DBG(DBG_MSG, "match...");	

			n++;

			/* skip password */
			for(; (n < size) && (cp[n] != ':'); n++);
			n++;

			for (uid = cp[n++] - '0'; cp[n] != ':'; n++)
				uid = (uid * 10) + cp[n] - '0';

			DBG(DBG_TRACE, "uid=%d", uid);	
			return uid;
		} else {
			DBG(DBG_MSG, "skip..");	
		}

		for (; (cp[n] != '\n') && (n < size); n++);
		for (; (cp[n] == '\n') && (n < size); n++);

	} while (n < size);

	return -1;
}

struct tcp_pcb * httpd_start(void)
{
	struct tcp_pcb * tp = (struct tcp_pcb *)&httpd80_tp;
	int uid;
	char * buf;
	int size;
	struct file_info info;

	if (file_lookup("/etc/passwd", &info) >= 0) {
		/* authentication data */
		buf = info.fi_data;
		/* the size of authentication data */
		size = info.fi_size;

		if ((uid = getuid("httpd", buf, size)) < 0) {
			/* assign the default user id  */
			uid = HTTPD_UID;

			DBG(DBG_TRACE, "using default uid=%d", uid);
		} else {
			DBG(DBG_TRACE, "uid=%d", uid);
		}
		
	} else {
		DBG(DBG_WARNING, "file_lookup()!");

		buf = NULL;
		size = 0;
		/* assign the default user id  */
		uid = HTTPD_UID;
	}

	httpd_init(tp, HTTPD_ROOT, uid, buf, size);

	return tp;
}

