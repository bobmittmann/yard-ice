# File:		Makefile
# Module: 
# Project:
# Author:	Robinson Mittmann (bob@boreste.com, bobmittmann@gmail.com)
# Target:
# Comment:  
# Copyright(c) 2006-2012 BORESTE (www.boreste.com). All Rights Reserved.

LIB_STATIC = tcpip

IFNET_CFILES = get_ifn_byipaddr.c get_ifn_byname.c ifn_enum.c ifn_getaddr.c \
			   ifn_get_first.c ifn_getname.c ifn_get_next.c ifn_getstat.c \
			   ifn_ipv4_get.c ifn_ipv4_set.c ifn_register.c ifn_unregister.c \
			   loopif.c

PCB_CFILES = in_pcb.c

ARP_CFILES = etharp.c

RAW_CFILES = raw.c raw_close.c raw_getstat.c \
			 raw_recvfrom.c raw_recvfrom_tmo.c raw_sendto.c raw_ioctl.c

UDP_CFILES = udp_bind.c udp.c udp_close.c udp_connect.c udp_enum.c udp_input.c \
			 udp_proto_getstat.c udp_recv.c udp_recv_tmo.c udp_release.c udp_send.c \
			 udp_sendto.c

TCP_CFILES = 

CFILES = arping.c icmp.c in_chksum.c ip.c ip_route.c net.c stat.c \
		 $(addprefix pcb/,$(PCB_CFILES)) $(addprefix ifnet/,$(IFNET_CFILES)) \
		 $(addprefix arp/,$(ARP_CFILES)) $(addprefix raw/,$(RAW_CFILES)) \
		 $(addprefix udp/,$(UDP_CFILES)) $(addprefix tcp/,$(TCP_CFILES))

override INCPATH += ./include

include ../mk/lib.mk


