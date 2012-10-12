/* $Id: services.h,v 2.0 2006/03/28 15:10:05 bob Exp $ 
 *
 * File:	netinet/services.h
 * Module:	
 * Project:	AT91X40DK	
 * Author:	Robinson Mittmann (bob@cnxtech.com)
 * Target:	arm7tdmi
 * Comment:	
 * Copyright(c) 2003,2004 CNX Technologies. All Rights Reserved.
 *
 */

#ifndef __NETINET_SERVICES_H__
#define __NETINET_SERVICES_H__

#define TCP_TCPMUX 1 
#define TCP_ECHO 7 
#define UDP_ECHO 7 
#define TCP_DISCARD 9 
#define UDP_DISCARD 9 
#define TCP_SYSTAT 11 
#define TCP_DAYTIME 13 
#define UDP_DAYTIME 13 
#define TCP_NETSTAT 15 
#define TCP_QOTD 17 
#define TCP_MSP 18 
#define UDP_MSP 18 
#define TCP_CHARGEN 19 
#define UDP_CHARGEN 19 
#define TCP_FTP 21 
#define TCP_TELNET 23 
#define TCP_SMTP 25 
#define TCP_TIME 37 
#define UDP_TIME 37 
#define UDP_RLP 39 
#define TCP_NAMESERVER 42 
#define TCP_WHOIS 43 
#define TCP_DOMAIN 53 
#define UDP_DOMAIN 53 
#define TCP_MTP 57 
#define TCP_BOOTPS 67 
#define UDP_BOOTPS 67 
#define TCP_BOOTPC 68 
#define UDP_BOOTPC 68 
#define UDP_TFTP 69 
#define TCP_GOPHER 70 
#define UDP_GOPHER 70 
#define TCP_RJE 77 
#define TCP_FINGER 79 
#define TCP_WWW 80 
#define UDP_WWW 80 
#define TCP_LINK 87 
#define TCP_KERBEROS 88 
#define UDP_KERBEROS 88 
#define TCP_SUPDUP 95 
#define TCP_HOSTNAMES 101 
#define TCP_ISO_TSAP 102 
#define TCP_CSNET_NS 105 
#define UDP_CSNET_NS 105 
#define TCP_RTELNET 107 
#define UDP_RTELNET 107 
#define TCP_POP2 109 
#define UDP_POP2 109 
#define TCP_POP3 110 
#define UDP_POP3 110 
#define TCP_SUNRPC 111 
#define UDP_SUNRPC 111 
#define TCP_AUTH 113 
#define TCP_SFTP 115 
#define TCP_UUCP_PATH 117 
#define TCP_NNTP 119 
#define TCP_NTP 123 
#define UDP_NTP 123 
#define TCP_NETBIOS_NS 137 
#define UDP_NETBIOS_NS 137 
#define TCP_NETBIOS_DGM 138 
#define UDP_NETBIOS_DGM 138 
#define TCP_NETBIOS_SSN 139 
#define UDP_NETBIOS_SSN 139 
#define TCP_IMAP2 143 
#define UDP_IMAP2 143 
#define UDP_SNMP 161 
#define UDP_SNMP_TRAP 162 
#define TCP_CMIP_MAN 163 
#define UDP_CMIP_MAN 163 
#define TCP_CMIP_AGENT 164 
#define UDP_CMIP_AGENT 164 
#define TCP_XDMCP 177 
#define UDP_XDMCP 177 
#define TCP_NEXTSTEP 178 
#define UDP_NEXTSTEP 178 
#define TCP_BGP 179 
#define UDP_BGP 179 
#define TCP_PROSPERO 191 
#define UDP_PROSPERO 191 
#define TCP_IRC 194 
#define UDP_IRC 194 
#define TCP_SMUX 199 
#define UDP_SMUX 199 
#define TCP_AT_RTMP 201 
#define UDP_AT_RTMP 201 
#define TCP_AT_NBP 202 
#define UDP_AT_NBP 202 
#define TCP_AT_ECHO 204 
#define UDP_AT_ECHO 204 
#define TCP_AT_ZIS 206 
#define UDP_AT_ZIS 206 
#define TCP_Z3950 210 
#define UDP_Z3950 210 
#define TCP_IPX 213 
#define UDP_IPX 213 
#define TCP_IMAP3 220 
#define UDP_IMAP3 220 
#define TCP_ULISTSERV 372 
#define UDP_ULISTSERV 372 

/*
 * UNIX specific services
 */

#define TCP_EXEC 512 
#define UDP_BIFF 512 
#define TCP_LOGIN 513 
#define UDP_WHO 513 
#define TCP_SHELL 514 
#define UDP_SYSLOG 514 
#define TCP_PRINTER 515 
#define UDP_TALK 517 
#define UDP_NTALK 518 
#define UDP_ROUTE 520 
#define UDP_TIMED 525 
#define TCP_TEMPO 526 
#define TCP_COURIER 530 
#define TCP_CONFERENCE 531 
#define TCP_NETNEWS 532 
#define UDP_NETWALL 533 
#define TCP_UUCP 540 
#define TCP_REMOTEFS 556 
#define TCP_KLOGIN 543 
#define TCP_KSHELL 544 
#define TCP_KERBEROS_ADM 749 

#endif

