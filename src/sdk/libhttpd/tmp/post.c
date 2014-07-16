/* 
 * File:	post.c
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

/* application/x-www-form-urlencoded  */
#define HASH_APPLICATION_X_WWW_FORM_URLENCODED 0x58

/* multipart/form-data */
#define HASH_MULTIPART_FORM_DATA 0x92


void post_url_form_recv(struct tcp_pcb * tp, int event)
{
#if 0
	struct cgistate * cgi;
	char * cp;
	int n;
	int m;
	int size;
	char * name[HTTPD_MAX_FORM_VARS];
	char * value[HTTPD_MAX_FORM_VARS];
	int i;
	
	if (event <= TCP_ON_CLOSE) {
		/* handle last notification */
		DBG(DBG_TRACE, "%d.%d.%d.%d:%d releasing", 
				IP4_ADDR1(tp->t_faddr), IP4_ADDR2(tp->t_faddr), 
				IP4_ADDR3(tp->t_faddr), IP4_ADDR4(tp->t_faddr), 
				ntohs(tp->t_fport));
		/* release the memory */
		httpd_free(post->httpd, post);
		return;
	}

	if (event == TCP_ON_FIN_RCVD) {
		DBG(DBG_TRACE, "%d.%d.%d.%d:%d finishing", 
				IP4_ADDR1(tp->t_faddr), IP4_ADDR2(tp->t_faddr), 
				IP4_ADDR3(tp->t_faddr), IP4_ADDR4(tp->t_faddr), 
				ntohs(tp->t_fport));
		tcp_close(tp);
		return;
	}

	if (event == TCP_ON_DATA_SENT) {
		DBG(DBG_TRACE, "%d.%d.%d.%d:%d sent", 
				IP4_ADDR1(tp->t_faddr), IP4_ADDR2(tp->t_faddr), 
				IP4_ADDR3(tp->t_faddr), IP4_ADDR4(tp->t_faddr), 
				ntohs(tp->t_fport));
		return;
	}
	
	if (event != TCP_ON_DATA_RCVD)
		return;

	/* fill in the buffer */
	cp = post->buf + post->count;
	if ((n = tcp_recv(tp, cp, post->size - post->count)) < 0) {
		/* error receiving data */
		tcp_close(tp);
		return;
	}
	post->count += n;
	if ((n != 0) && (post->count != post->size))
		return;

	cp = post->buf;
	size = post->count;

	cp[size] = '\0';

	DBG(DBG_TRACE, cp);

	n = 0;
	i = 0;
	do {
		name[i] = &cp[n];
		for (; (n < size) && (cp[n] != '='); n++);
		cp[n] = '\0';
		n++;
		value[i] = &cp[n];
		m = n;
		for (; (n < size) && (cp[n] != '&'); n++) {
			if (cp[n] == '+')
				cp[m] = ' ';
			else {
				if (cp[n] == '%') {
					cp[m] = ((cp[n + 1] - '0') << 4) + cp[n + 2] - '0';
					n += 2;
				} else {
					cp[m] = cp[n];
				}
			}
			m++;
		}
		cp[m] = '\0';
		n++;
		i++;
		if (i == HTTPD_MAX_FORM_VARS)
			break;
	} while (n < size);

	/* reuse the allocated buffer */
	cgi = (struct cgistate *)post;
	/* XXX: */
	cgi->httpd = post->httpd;
	cgi->callback = NULL;

	/* attach the PCB to the CGI handler */
	tcp_attach(tp, (tcp_callback_t)httpd_cgi_handler, cgi);

	if (post->callback(tp, name, value, i) < 0) {
		/* close the tcp pcb */
		tcp_close(tp);
	} else {
		/* if the CGI is still detached, close-it */
		if (cgi->callback == NULL)
			tcp_close(tp);		
	}
#endif
}

#define HTTPMULTI_BOUNDARY 0
#define HTTPMULTI_BOUNDARY_CR 1
#define HTTPMULTI_HDR 2
#define HTTPMULTI_HDR_CR 3
#define HTTPMULTI_HDR_LF 4
#define HTTPMULTI_HDR_LASTCR 5
#define HTTPMULTI_DATA 6
#define HTTPMULTI_DATA_CR 7
#define HTTPMULTI_EOF 8
#define HTTPMULTI_CLOSING 9
#define HTTPMULTI_WAIT 10
#define HTTPMULTI_AGAIN 11
#define HTTPMULTI_AGAIN_CR 12
#define HTTPMULTI_AGAIN_LAST 13
/*
 * XXX: This implementation take into account only the first part of a 
 * multipart form.
 *
 */
void post_multipart_form_recv(struct tcp_pcb * tp, int event)
{
#if 0
	char * cp;
	int n;
	int c;
	
	if (event <= TCP_ON_CLOSE) {
		/* handle last notification */
		DBG(DBG_TRACE, "%d.%d.%d.%d:%d releasing", 
				IP4_ADDR1(tp->t_faddr), IP4_ADDR2(tp->t_faddr), 
				IP4_ADDR3(tp->t_faddr), IP4_ADDR4(tp->t_faddr), 
				ntohs(tp->t_fport));
		/* release the memory */
		httpd_free(multi->httpd, multi);
		return;
	}

	if (event == TCP_ON_FIN_RCVD) {
		DBG(DBG_TRACE, "%d.%d.%d.%d:%d finishing", 
				IP4_ADDR1(tp->t_faddr), IP4_ADDR2(tp->t_faddr), 
				IP4_ADDR3(tp->t_faddr), IP4_ADDR4(tp->t_faddr), 
				ntohs(tp->t_fport));
		tcp_close(tp);
		return;
	}

	if (event == TCP_ON_DATA_SENT) {
		DBG(DBG_TRACE, "%d.%d.%d.%d:%d sent", 
				IP4_ADDR1(tp->t_faddr), IP4_ADDR2(tp->t_faddr), 
				IP4_ADDR3(tp->t_faddr), IP4_ADDR4(tp->t_faddr), 
				ntohs(tp->t_fport));
		return;
	}
	
	if (event != TCP_ON_DATA_RCVD)
		return;

	if (multi->state >= HTTPMULTI_AGAIN) {
		DBG(DBG_TRACE, "again");
		n = multi->callback(tp, multi->buf, multi->size, multi->offs);
		if	(n == 0)
			return;
		if (n < 0) {
//			tcp_close(tp);
			multi->size = 0;
			multi->state = HTTPMULTI_CLOSING;
		} else {
			multi->offs += multi->size;
			if (multi->state >= HTTPMULTI_AGAIN_CR) {
				multi->buf[0] = '\r';
				multi->size = 1;
			} else {
				multi->size = 0;
			}	
			multi->state = HTTPMULTI_DATA;
		}
	}

	/* fill in the buffer */
	cp = &multi->buf[multi->size];
	while ((n = tcp_recv(tp, cp, 1)) == 1) {
		c = *cp;

		switch (multi->state) {
			
		case HTTPMULTI_BOUNDARY:
			if (c == '\r') {
				multi->state = HTTPMULTI_BOUNDARY_CR;
				DBG(DBG_TRACE, "Boundary: hash=%02x len=%d",
					multi->bdry_hash, multi->bdry_len);
			} else {
				multi->bdry_hash = CRC8(multi->bdry_hash, c);
				multi->bdry_len++;
			}
			break;

		case HTTPMULTI_BOUNDARY_CR:
			if (c != '\n') {
				/* TODO: Malformed request */
				DBG(DBG_WARNING, "Boundary parsing");
			}
			multi->state = HTTPMULTI_HDR;
			break;

		case HTTPMULTI_HDR:
			if (c == '\r')
				multi->state = HTTPMULTI_HDR_CR;
			break;

		case HTTPMULTI_HDR_CR:
			if (c == '\n')
				multi->state = HTTPMULTI_HDR_LF;
			else
				multi->state = HTTPMULTI_HDR;
			break;

		case HTTPMULTI_HDR_LF:
			if (c == '\r')
				multi->state = HTTPMULTI_HDR_LASTCR;
			else
				multi->state = HTTPMULTI_HDR;
			break;
			
		case HTTPMULTI_HDR_LASTCR:
			if (c == '\n') {
				multi->state = HTTPMULTI_DATA;
				multi->line_hash = 0;
				multi->line_len = 0;
				multi->size = 0;
				multi->offs = 0;
				/* notify the beginning of the file */
				multi->callback(tp, NULL, 0, 0);
				return;
			} else {
				/* TODO: Malformed request */
				DBG(DBG_WARNING, "Multi Header parsing");
				multi->state = HTTPMULTI_HDR;
			}
			break;

		case HTTPMULTI_DATA:
			if (c == '\r') {
				if (multi->size) {
					/* flushes the input buffer */
					DBG(DBG_TRACE, "CR flush");
					n = multi->callback(tp, multi->buf, 
										multi->size, multi->offs);
					if (n == 0) {
						multi->state = HTTPMULTI_AGAIN_CR;
						return;
					}
					if (n < 0) {
						cp = multi->buf;
						multi->size = 0;
						multi->state = HTTPMULTI_CLOSING;
						return;
					} 	
					multi->offs += multi->size;
					cp = multi->buf;
					*cp = '\r';
					multi->size = 1;
					return;
				} else {
					multi->size++;
				}
			} else {
				multi->size++;

				if (c == '\n') {
					multi->line_len = 0;
					multi->line_hash = 0;
				} else {
					multi->line_len++;
					multi->line_hash = CRC8(multi->line_hash, c);
					if (multi->line_len == multi->bdry_len) {
						if (multi->line_hash == multi->bdry_hash) {
							DBG(DBG_TRACE, "Boundary hash match");
							multi->state = HTTPMULTI_EOF;
							break;
						}
						multi->line_len = 0;
						multi->line_hash = 0;
					}
				}

				if (multi->size == HTTPD_MAX_FILECHUNK) {
					DBG(DBG_INFO, "flush %d bytes", multi->size);
					n = multi->callback(tp, multi->buf, 
										multi->size, multi->offs);
					if (n == 0) {
						multi->state = HTTPMULTI_AGAIN;
						return;
					}
					if (n < 0) {
						cp = multi->buf;
						multi->size = 0;
						multi->state = HTTPMULTI_CLOSING;
						return;
					} 
					multi->offs += multi->size;
					cp = multi->buf;
					multi->size = 0;
					return;
				}
			}
			cp++;
			break;

		case HTTPMULTI_EOF:
			DBG(DBG_TRACE, "End of file");
			multi->state = HTTPMULTI_WAIT;
			/* notify the end of the file */
			multi->callback(tp, NULL, -1, -1);
/*			tcp_close(tp); */
			return;
			
		case HTTPMULTI_CLOSING:
			multi->size++;

			if (c == '\n') {
				multi->line_len = 0;
				multi->line_hash = 0;
			} else {
				multi->line_len++;
				multi->line_hash = CRC8(multi->line_hash, c);
				if (multi->line_len == multi->bdry_len) {
					if (multi->line_hash == multi->bdry_hash) {
						DBG(DBG_TRACE, "Boundary hash match");
						multi->state = HTTPMULTI_WAIT;
						tcp_close(tp);
						break;
					}
					multi->line_len = 0;
					multi->line_hash = 0;
				}
			}

			if (multi->size == HTTPD_MAX_FILECHUNK) {
				cp = multi->buf;
				multi->size = 0;
				break;
			}
			cp++;
			break;
		}
	}

	if (n < 0) {
		DBG(DBG_TRACE, "Closing");
		tcp_close(tp);
		return;
	}
#endif
}

uint32_t mk_ext_hash(char * ext);

#define HEX2INT(C) (((C) <= '9') ? (C) - '0' : (C) - ('A' - 10))

int http_decode_url_attr(char * buf, int len, 
						 char * attr[], char * val[], int max)  
{
	int n;
	int i;
	int j;
	int c;

	n = 0;
	attr[n] = buf;
	for (i = 0, j = 0; i < len; i++) {
		c = buf[i];
		/* translate */
		switch (c) {
		case '+':
			buf[j++] = ' ';
			break;
		case '%':
			DBG(DBG_MSG, "%c%c, %x%x", buf[i + 1], buf[i + 2], 
				HEX2INT(buf[i + 1]), HEX2INT(buf[i + 2]));

			c = (HEX2INT(buf[i + 1]) << 4) + HEX2INT(buf[i + 2]);
			i += 2;
			buf[j++] = c;
			break;
		case '&':
			buf[j++] = '\0';
			if (n < 16) {
				DBG(DBG_TRACE, "%s='%s'", attr[n - 1], val[n - 1]);
				attr[n] = &buf[j];
			}
			break;
		case '=':
			buf[j++] = '\0';
			if (n < 16) {
				val[n++] = &buf[j];
			}
			break;
		default:
			buf[j++] = c;
		}
	}
	buf[j] = '\0';

	if (n) {
		DBG(DBG_TRACE, "%s='%s'", attr[n - 1], val[n - 1]);
	}

	return n;
}

int http_post(struct httpd * httpd, struct httpctl * ctl)
{
	char buf[128];
	char * attr[16];	
	char * val[16];	
	int content_type;
	int content_len;
	struct stat sb;
	int ret;
	int fd;
	int (* cgi)(struct tcp_pcb * tp);
	int size;
	char * ext;	
//	int type;
	int hash;
	int len;
	int n;

	strcpy(buf, httpd->root);
	strcat(buf, ctl->uri);

	DBG(DBG_INFO, "path=%s", buf);

	if ((ret = stat(buf, &sb)) < 0) {
		DBG(DBG_ERROR, "404 File Not Found");
		httpd_404(ctl->tp);
		return ret;
	}

	if (!S_ISREG(sb.st_mode)) {
		DBG(DBG_ERROR, "403 Forbidden");
		httpd_403(ctl->tp);
		return -1;
	}

	if ((ext = strrchr(buf, '.')) != NULL) {
		DBG(DBG_INFO, "extension=%s", ext);
		/* skip the '.' */
		ext++;
		hash = mk_ext_hash(ext);
	} else 
		hash = EXT_NONE;
		
	if ((hash != EXT_NONE) && (hash != EXT_CGI)) {
		DBG(DBG_ERROR, "403 Forbidden");
		return httpd_403(ctl->tp);
	}

	if ((fd = open(buf, O_RDONLY)) < 0) {
		DBG(DBG_ERROR, "404 File Not Found");
		httpd_404(ctl->tp);
		return fd;
	}

#if 0
	for (opt = buf; (*opt); opt++) {
 		if (*opt == '?') {
			*opt++ = '\0';
			break;
		}
	}

	if (!S_ISREG(st.st_mode) || !(st.st_mode & S_IXUSR)) {
		DBG(DBG_ERROR, "403 Forbidden");
		httpd_403(__tp);
		return -1;
	}

	if (st.st_uid < __httpd->uid) {
		DBG(DBG_TRACE, "Authorization required");
		if (st.st_uid < httpd_auth(__httpd, __http->usr, __http->pwd)) {
			httpd_401_auth(__tp);
			return -1;
		}
	}
	if (file_open(&fd, path, 0) < 0) {
		DBG(DBG_ERROR, "403 Forbidden");
		return httpd_403(__tp);
	}
#endif

	content_type = ctl->ctype;
	content_len = ctl->ctlen;

	size = lseek(fd, 0, SEEK_END);
	DBG(DBG_TRACE, "size=%d", size);

	if ((content_type == HASH_APPLICATION_X_WWW_FORM_URLENCODED) && 
		(content_len > 0)) {
		int (* form_cgi)(struct tcp_pcb * tp, char **, char **, int);

		DBG(DBG_TRACE, "application/x-www-form-urlencoded");

		len = content_len > 128 ? 128 : content_len;

		if ((len = tcp_recv(ctl->tp, buf, len)) <= 0) {
			close(fd);
			return ret;
		}

		n = http_decode_url_attr(buf, len, attr, val, 16);

		form_cgi = mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0);

		ret = form_cgi(ctl->tp, attr, val, n);

		close(fd);
		return 0;
	}
	
	if ((content_type == HASH_MULTIPART_FORM_DATA) && (content_len > 0)) {
		
		DBG(DBG_TRACE, "multipart/form-data");

#if 0
		multi = (struct postfilestate *)__http;
		multi->size = content_len;
		multi->state = HTTPMULTI_BOUNDARY;
		multi->bdry_len = 0;
		multi->bdry_hash = 0;
		multi->callback = (httpd_file_cgi_t)info.fi_data;
		
		/* attach the pcb to the form decode callback */
		tcp_attach(__tp, (tcp_callback_t)post_multipart_form_recv, multi);
#endif
		close(fd);
		return 0;
	}
	
	/* dynamic cgi */
	DBG(DBG_TRACE, "content_type=%02x", content_type);

	size = lseek(fd, 0, SEEK_END);
	DBG(DBG_TRACE, "size=%d", size);

	cgi = mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0);

	DBG(DBG_TRACE, "cgi=%p", cgi);

	ret = cgi(ctl->tp);

	close(fd);

	return ret;
}

