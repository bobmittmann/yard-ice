/*
 *
 * File:	httpdefs.h
 * Module:
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com, bobmittmann@gmail.com)
 * Target:
 * Comment: 
 * Copyright(c) 2004-2009 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#ifndef __HTTPDEFS_H__
#define __HTTPDEFS_H__

/* 
 * The HTTPD configuration options
 */

#ifndef HTTPD_ROOT
#define HTTPD_ROOT "/httpd"
#endif

#ifndef HTTPD_DFT_PAGE 
#define HTTPD_DFT_PAGE "index.html"
#endif

#ifndef HTTPD_MAX_USER
#define HTTPD_MAX_USER 16
#endif

#ifndef HTTPD_MAX_PASSWD
#define HTTPD_MAX_PASSWD 16
#endif

#ifndef HTTPD_MAX_FORM_VARS
#define HTTPD_MAX_FORM_VARS 16
#endif

#ifndef HTTPD_SERVER
#define HTTPD_SERVER "BORESTE HTTPD"
#endif

/* maximum length of the authentication information */
#define HTTPD_MAX_AUTH (HTTPD_MAX_USER + HTTPD_MAX_PASSWD)

#define MK_EXT(A, B, C, D) ((A) + ((B) << 8) + ((C) << 16) + ((D) << 24))

#define EXT_NONE    MK_EXT(0, 0, 0, 0)
#define EXT_HTML    MK_EXT('H', 'T', 'M', 'L')
#define EXT_HTM     MK_EXT('H', 'T', 'M', 0)
#define EXT_JPEG    MK_EXT('J', 'P', 'E', 'G')
#define EXT_JPG     MK_EXT('J', 'P', 'G', 0)
#define EXT_GIF     MK_EXT('G', 'I', 'F', 0)
#define EXT_PNG     MK_EXT('P', 'N', 'G', 0)
#define EXT_TXT     MK_EXT('T', 'X', 'T', 0)
#define EXT_MPEG    MK_EXT('M', 'P', 'E', 'G')
#define EXT_MPG     MK_EXT('M', 'P', 'G', 0)
#define EXT_MP3     MK_EXT('M', 'P', '3', 0)
#define EXT_OGG     MK_EXT('O', 'G', 'G', 0)
#define EXT_CSS     MK_EXT('C', 'S', 'S', 0)
#define EXT_ICO     MK_EXT('I', 'C', 'O', 0)
#define EXT_CGI     MK_EXT('C', 'G', 'I', 0)
#define EXT_JS      MK_EXT('J', 'S', '0', 0)

/* maximum length of the uri */
#define HTTPD_MAX_URI (HTTPD_MEM_BLK_SIZE - \
					   (sizeof(struct httpctl) + HTTPD_MAX_AUTH))

/* maximum length of the post data */
#define HTTPD_MAX_POST (HTTPD_MEM_BLK_SIZE - \
						(sizeof(struct poststate) + HTTPD_MAX_AUTH))

#define HTTPD_MAX_CGI (HTTPD_MEM_BLK_SIZE - sizeof(struct cgistate))

/* maximum length for file chunks in multipart forms */
#define HTTPD_MAX_FILECHUNK (HTTPD_MEM_BLK_SIZE - \
						(sizeof(struct postfilestate) + HTTPD_MAX_AUTH))

/* 
 * The HTTP message decoding state machine 'states'
 */

/* this connection is not in use */
#define HTTPCTL_FREE        0
/* waiting for a incomming request */
#define HTTPCTL_WAITING     1

/* proccessig the request line */
#define HTTPCTL_REQ         2
#define HTTPCTL_URI         3
#define HTTPCTL_VERSION     4
#define HTTPCTL_REQ_CR      5

/* proccessig the header fields */
#define HTTPCTL_HDR         6
#define HTTPCTL_HDR_LWS     7
#define HTTPCTL_HDR_VALUE   8
#define HTTPCTL_HDR_CR      9
#define HTTPCTL_HDR_LASTCR  10

#define HTTPCTL_HDR_AUTH    11

/* content-type header */
#define HTTPCTL_HDR_CTYPE   12

#define HTTPCTL_HDR_CTYPE_PARM 32

/* content-length header */
#define HTTPCTL_HDR_CTLEN   13

/* authentication header */
#define HTTPCTL_AUTH_LWS    14
#define HTTPCTL_AUTH_USR0   15
#define HTTPCTL_AUTH_USR1   16
#define HTTPCTL_AUTH_USR2   17
#define HTTPCTL_AUTH_USR3   18
#define HTTPCTL_AUTH_PWD0   19
#define HTTPCTL_AUTH_PWD1   20
#define HTTPCTL_AUTH_PWD2   21
#define HTTPCTL_AUTH_PWD3   22

/* 400 Bad Request ... */
#define HTTPCTL_400         23
#define HTTPCTL_400_CR      24

/* the URI was too long ... */
#define HTTPCTL_414         25
#define HTTPCTL_414_CR      26

/* 500 internal server error... */
#define HTTPCTL_500         27
#define HTTPCTL_500_CR      28

/* parsing the request */
#define HTTPCTL_REQUEST     29
#define HTTPCTL_TRY_REQUEST 30
/* waiting for the connection to close */
#define HTTPCTL_CLOSING     31


/*
 * Preformatted html macros 
 */

#define HTML_COPYRIGHT "&copy; Copyrigth 2004-2006, "\
	"CNX Tecnologia em Inform&aacute;tica LTDA"

#define HTML_COMPANY "<b>BORESTE</b> - Embedded Systems - "\
	"<i><a href=\"http://www.boreste.com\">www.boreste.com</a></i>"

#define HTML_MSG_FOOTER "<hr>" HTML_COPYRIGHT "<br>" HTML_COMPANY \
	"</body></html>\r\n"

#define HTTP_MSG(CODE, INFO, MSG) "HTTP/1.1 " #CODE " " INFO "\r\n"\
	"Server: " HTTPD_SERVER "\r\nContent-type: text/html\r\n\r\n"\
	"<html><head><title>" #CODE "</title></head>"\
	"<h2>" #CODE " - " INFO "</h2><p>" MSG "</p>" HTML_MSG_FOOTER

/*
 * Preformated Content-type header field
 */

extern const char http_type_binary[];
extern const char http_type_gif[];
extern const char http_type_png[];
extern const char http_type_jpeg[];
extern const char http_type_text[];
extern const char http_type_html[];
extern const char http_type_xml[];
extern const char http_type_mpeg[];
extern const char http_type_css[];
extern const char http_type_javascript[];

/*
 * mime content type lookup table 
 */
extern const char * const http_content_type[];


/* Page default */
extern const char httpd_page_default[];

/* the root directory */
extern const char httpd_root[];

/* 
 * Preformatted html chunks 
 */

extern const char html_header[];
//extern const char html_footer[];

/* Preformatted status codes rplies */
extern const char http_200[];
extern const char http_hdr_server[];

/* Preformatted http headers */
extern const char http_hdr_200[];
extern const char http_hdr_200_binary[];
extern const char http_hdr_200_gif[];
extern const char http_hdr_200_png[];
extern const char http_hdr_200_jpeg[];
extern const char http_hdr_200_text[];
extern const char http_hdr_200_html[];
extern const char http_hdr_200_xml[];
extern const char http_hdr_200_mpeg[];
extern const char http_hdr_200_css[];

#ifdef __cplusplus
extern "C" {
#endif

int http_get(struct httpd * httpd, struct httpctl * ctl);

int http_post(struct httpd * httpd, struct httpctl * ctl);

void * httpd_malloc(struct httpd * __httpd, size_t __size);

int httpd_free(struct httpd * __httpd, void * __p);

int is_httpd_mem(void * __p);

unsigned int httpd_auth(struct httpd * __httpd,
						char * __usr, char * __pwd);

#ifdef __cplusplus
}
#endif

#endif /* __HTTPDEFS_H__ */

