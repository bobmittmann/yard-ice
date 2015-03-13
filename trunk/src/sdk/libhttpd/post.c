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

#include "httpd-i.h"

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

#define HTTP_QUERY_KEY_MAX 16
#define HTTP_QUERY_VAL_MAX 16

int http_decode_uri_query(char * buf, int len, 
						  struct httpqry lst[], int max)  
{
	int n;
	int i;
	int j;
	int c;

	n = 0;
	lst[n].key = buf;
	for (i = 0, j = 0; i < len; i++) {
		c = buf[i];
		/* translate */
		switch (c) {
		case '+':
			buf[j++] = ' ';
			break;
		case '%':
			DCC_LOG4(LOG_TRACE, "%c%c, %x%x", buf[i + 1], buf[i + 2], 
				HEX2INT(buf[i + 1]), HEX2INT(buf[i + 2]));

			c = (HEX2INT(buf[i + 1]) << 4) + HEX2INT(buf[i + 2]);
			i += 2;
			buf[j++] = c;
			break;
		case '&':
		case ';':
			buf[j++] = '\0';
			if (n < HTTP_QUERY_KEY_MAX) {
				lst[n].key = &buf[j];
				lst[n].val = "\0";
			}
			break;
		case '=':
			buf[j++] = '\0';
			if (n < HTTP_QUERY_VAL_MAX)
				lst[n++].val = &buf[j];
			break;
		case '\0':
			return n;
		case '#':
			/* FIXME: implement fragment optional part */
			break;
		default:
			buf[j++] = c;
		}
	}
	buf[j] = '\0';

	return n;
}

char * http_query_lookup(struct httpctl * ctl, char * key)
{
	struct httpqry * lst = ctl->qrylst;
	int cnt = ctl->qrycnt;
	int i;

	for (i = 0; i < cnt; ++i) {
		if (strcmp(lst[i].key, key) == 0)
			return lst[i].val;
	}

	return "\0";
}

#define HTTP_FORM_CONTENT_MAX 128

int http_post(struct httpctl * ctl, const struct httpdobj * obj)
{
	int (* cgi)(struct httpctl * ctl);
	int content_type;
	int content_len;
	int len;
	int ret;

	content_type = ctl->ctype;
	content_len = ctl->ctlen;

	if ((content_type == HASH_APPLICATION_X_WWW_FORM_URLENCODED) && 
		(content_len > 0)) {
		char buf[HTTP_FORM_CONTENT_MAX];
		int n;

		DCC_LOG(LOG_TRACE, "application/x-www-form-urlencoded");

		len = content_len > HTTP_FORM_CONTENT_MAX ? 
			HTTP_FORM_CONTENT_MAX : content_len;

		if ((len = tcp_recv(ctl->tp, buf, len)) <= 0) {
			return len;
		}

		n = http_decode_uri_query(buf, len, ctl->qrylst, HTTPD_QUERY_LST_MAX);

		if (n < 0) {
			/* FIXME: return error code */
			DCC_LOG(LOG_ERROR, "http_decode_uri_query() failed!");
			return -1;
		}

		ctl->qrycnt = n;

	} else if ((content_type == HASH_MULTIPART_FORM_DATA) && 
			   (content_len > 0)) {

		DCC_LOG(LOG_TRACE, "multipart/form-data");

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
		return 0;
	} else {
		/* dynamic cgi */
		DCC_LOG1(LOG_TRACE, "content_type=%02x", content_type);
	}

	cgi = (int (*)(struct httpctl *))obj->ptr;
	ret = cgi(ctl);

	return ret;
}

