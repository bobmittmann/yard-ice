/* $Id: tnshell.h,v 2.2 2006/04/09 15:03:31 bob Exp $ 
 *
 * File:	tcpip/tnshell.h
 * Module:
 * Project:
 * Author:	Robinson Mittmann (bob@cnxtech.com, bob@methafora.com.br)
 * Target:
 * Comment: Basic Telnet Shell
 * Copyright(c) 2006 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#ifndef __TCPIP_TNSHELL_H__
#define __TCPIP_TNSHELL_H__

#include <tcpip/telnetd.h>
#include <sys/tty.h>
#include <sys/shell.h>

#ifdef __cplusplus
extern "C" {
#endif

int telnetd_shell_start(struct shell_def * shell, int backlog);

#ifdef __cplusplus
}
#endif

#endif /* __TCPIP_TNSHELL_H__ */

