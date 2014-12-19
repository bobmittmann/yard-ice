/* 
 *
 * File:	dirlist.c
 * Module:  
 * Project:
 * Author:	Robinson Mittmann (bob@boreste.com, bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(c) 2004-2006 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <tcpip/httpd.h>
#include "httpdefs.h"

#ifdef HTTPD_DEBUG
#ifndef DEBUG
#define DEBUG
#endif
#endif
#include <debug.h>

static const char doctype_html[] = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 3.2 Final//EN\">\r\n";

static const char html_footer[] = "\r\n</pre><hr>"
	"<b>BORESTE</b> - Embedded Electronics - "
	"<i><a href=\"http://www.boreste.com\">www.boreste.com</a></i>"
	"</body></html>";

int httpd_dirlist(struct httpd * httpd, struct httpctl * ctl)
{
	char buf[128];
	struct dirent entry;
	struct dirent * ep;
	DIR * dir;
	int ret;
	int n;
	char * cp;

	/* remove trailing slashes */
	cp = ctl->uri + strlen(ctl->uri);
	do {
		cp--;
	} while (*cp == '/');
	cp++;
	*cp = '\0';

	strcpy(buf, httpd->root);
	strcat(buf, ctl->uri);

	DBG(DBG_INFO, "path=%s", buf);

	if ((dir = opendir(buf)) == NULL) {
		DBG(DBG_ERROR, "403 Forbidden");
		httpd_403(ctl->tp);
		return -1;
	}

	httpd_200_html(ctl->tp);

	tcp_send(ctl->tp, doctype_html, sizeof(doctype_html) - 1, 0);

	n = sprintf(buf, "<html><head><title>Indef of %s/</title></head>", 
				ctl->uri);

	tcp_send(ctl->tp, buf, n, 0);

	n = sprintf(buf, "<body bgcolor=\"white\" text=\"black\">"
				"<h1>Index of %s/</h1><hr>\r\n<pre>", ctl->uri);
	tcp_send(ctl->tp, buf, n, 0);


	while ((ret = readdir_r(dir, &entry, &ep)) == 0) {
		if (ep == NULL)
			break;
		if (ep->d_type == DT_DIR) {
			n = sprintf(buf,  "<a href=\"%s/%s\">[%s]</a><br>", 
						ctl->uri, ep->d_name, ep->d_name);
		} else {
			n = sprintf(buf,  "<a href=\"%s/%s\">%s</a><br>", 
						ctl->uri, ep->d_name, ep->d_name);
		}
		tcp_send(ctl->tp, buf, n, 0);
	}

	n = sprintf(buf,  "</pre>");
	tcp_send(ctl->tp, buf, n, 0);

	closedir(dir);

	return tcp_send(ctl->tp, html_footer, sizeof(html_footer) - 1, 0);
}

