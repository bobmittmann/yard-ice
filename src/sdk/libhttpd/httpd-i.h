/*
 *
 * File:	httpd-i.h
 * Module:
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com, bobmittmann@gmail.com)
 * Target:
 * Comment: 
 * Copyright(c) 2004-2009 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#ifndef __HTTPD_I_H__
#define __HTTPD_I_H__

#ifdef CONFIG_H
#include "config.h"
#endif

#include <stdint.h>
#include <tcpip/tcp.h>
#include <errno.h>

#ifndef HTTPD_URI_MAX_LEN
#define HTTPD_URI_MAX_LEN 64
#endif

#ifndef HTTPD_SERVER_NAME 
#define HTTPD_SERVER_NAME "ThinkOS Web Server"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* __HTTPD_I_H__ */

