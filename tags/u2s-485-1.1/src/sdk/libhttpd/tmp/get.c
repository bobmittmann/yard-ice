/* 
 * File:	http_get.c
 * Module:  
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com, bobmittmann@gmail.com)
 * Target:	
 * Comment: 
 * Copyright(c) 2005-2009 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#include <stdlib.h>
#include <string.h>
#include <tcpip/httpd.h>

#include <sys/file.h>
#include <sys/fs.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <mime.h>
#include <crc.h>
#include <ctype.h>
#include <sys/mman.h>

#include "httpdefs.h"

#ifdef HTTPD_DEBUG
#ifndef DEBUG
#define DEBUG
#endif
#endif
#include <debug.h>
#include <sys/dcclog.h>

uint32_t mk_ext_hash(char * ext)
{
	uint32_t hash;
	char * cp;
	int n;
	int c;

	hash = 0;
	cp = ext;

	for (n = 0; n < 4; n++) {
		if ((c = *cp) == '\0')
			break;
		hash += toupper(c) << (8 * n);
		cp++;
	}
	return hash;
}

int http_get(struct httpd * httpd, struct httpctl * ctl)
{
	int (* cgi)(struct tcp_pcb * tp);
	char buf[128];
//	int content_type;
//	int content_len;
//	char * opt;	
	char * ext;	
	struct stat sb;
	int type;
	int hash;
	int size;
	int ret;
	int fd;
	int n;

	strcpy(buf, httpd->root);
	strcat(buf, ctl->uri);

//	for (opt = buf; (*opt); opt++) {
 //		if (*opt == '?') {
//			*opt++ = '\0';
//			break;
//		}
//	}
	
	DBG(DBG_INFO, "path=%s", buf);

	if ((ret = stat(buf, &sb)) < 0) {
		DBG(DBG_ERROR, "404 File Not Found");
		httpd_404(ctl->tp);
		return ret;
	}

	if (S_ISDIR(sb.st_mode)) {
		DBG(DBG_INFO, "is a directory");
		strcat(buf, "/");
		strcat(buf, httpd_page_default);
		if ((ret = stat(buf, &sb)) < 0) {
			strcpy(buf, httpd->root);
			strcat(buf, ctl->uri);	
			return httpd_dirlist(httpd, ctl);
		}
	}

	if (!S_ISREG(sb.st_mode)) {
		DBG(DBG_ERROR, "403 Forbidden");
		httpd_403(ctl->tp);
		return -1;
	}

	if ((fd = open(buf, O_RDONLY)) < 0) {
		DBG(DBG_ERROR, "404 File Not Found");
		httpd_404(ctl->tp);
		return -1;
	}

	ret = 0;

	if ((ext = strrchr(buf, '.')) != NULL) {
		DBG(DBG_INFO, "extension=%s", ext);
		/* skip the '.' */
		ext++;
		hash = mk_ext_hash(ext);

		switch (hash) {
		case EXT_CGI:
			DBG(DBG_INFO, "cgi");
			goto dynamic_cgi;
		case EXT_TXT:
			DBG(DBG_INFO, "text");
			type = text_plain; 
			break;
		case EXT_JPEG:
		case EXT_JPG:
			DBG(DBG_INFO, "jpeg");
			type = image_jpeg; 
			break;
		case EXT_ICO:
			DBG(DBG_INFO, "ico");
		case EXT_GIF:
			DBG(DBG_INFO, "gif");
			type = image_gif; 
			break;
		case EXT_PNG:
			DBG(DBG_INFO, "png");
			type = image_png; 
			break;
		case EXT_JS:
			DBG(DBG_INFO, "js");
			type = application_x_javascript;
			break;
		case EXT_HTML:
		case EXT_HTM:
			DBG(DBG_INFO, "html");
		default:
			type = text_html; 
			break;
		}

		httpd_200(ctl->tp, type);

		while ((n = read(fd, buf, 128)) > 0) {
			if (tcp_send(ctl->tp, buf, n, 0) <= 0) {
				DBG(DBG_ERROR, "tcp_send()");
				ret = -1;
				break;
			}
		}

	} else {
dynamic_cgi:

		size = lseek(fd, 0, SEEK_END);
		DBG(DBG_TRACE, "size=%d", size);

		cgi = mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0);

		DBG(DBG_TRACE, "cgi=%p", cgi);

		ret = cgi(ctl->tp);
	}

	close(fd);

#if 0
	DBG(DBG_INFO, "file uid=%d", info.fi_uid);

	if (info.fi_uid < __httpd->uid) {
		unsigned int uid;
		
		DBG(DBG_INFO, "info.fi_uid=%d < httpd->uid=%d, auth required", 
			info.fi_uid, __httpd->uid);
		uid = httpd_auth(__http->httpd, __http->usr, __http->pwd);
		if (info.fi_uid < uid) {
			DBG(DBG_ERROR, "401 Unauthorized, uid=%d", uid);
			httpd_401_auth(__tp);
			return -1;
		}
	}

	content_type = __http->ctype;
	content_len = __http->ctlen;

	if (info.fi_mime != _code_) {
		/* detach the PCB */
		tcp_attach(__tp, NULL, __httpd);
		/* release the control structure */
		httpd_free(__httpd, __http);

		httpd_send(__tp, info.fi_mime, (char *)info.fi_data, info.fi_size);
		tcp_close(__tp);
		return 0;
	}

	/* dynamic cgi */
	DBG(DBG_INFO, "dynamic page");
	cgi_call = (httpd_cgi_t)info.fi_data;

	/* reuse the allocated buffer */
	cgi = (struct cgistate *)__http;
	/* XXX: */
	cgi->httpd = __httpd;
	cgi->callback = NULL;

	/* attach the PCB to the CGI handler */
	tcp_attach(__tp, (tcp_callback_t)httpd_cgi_handler, cgi);

	if (cgi_call(__tp, opt, content_type, content_len) < 0) {
		/* close the tcp pcb */
		tcp_close(__tp);
	} else {
		/* if the CGI is still detached, close-it */
		if (cgi->callback == NULL)
			tcp_close(__tp);		
	}
#endif



	return ret;
}

