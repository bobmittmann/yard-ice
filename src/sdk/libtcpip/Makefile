#
# Copyright(c) 2006-2012 BORESTE (www.boreste.com). All Rights Reserved.
# 
# This file is part of the libtcpip.
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 3.0 of the License, or (at your option) any later version.
# 
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
# 
# You can receive a copy of the GNU Lesser General Public License from 
# http://www.gnu.org/
#

#
# File:   Makefile
# Author: Robinson Mittmann <bobmittmann@gmail.com>
# 

include ../../mk/config.mk

LIB_STATIC = tcpip

MBUF_CFILES = mbuf.c mbuf_memavail.c mbuf_alloc.c mbuf_try_alloc.c\
			  mbuf_try_alloc_and_wait.c mbuf_free.c mbuf_check.c\
			  mbuf_size.c mbuf_avail.c mbuf_max.c mbuf_used.c mbuf_wait.c\
			  mbuf_list_alloc.c mbuf_list_free.c mbuf_list_read.c\
			  mbuf_list_write.c\
			  mbuf_queue_add.c mbuf_queue_free.c mbuf_queue_get.c\
			  mbuf_queue_init.c mbuf_queue_remove.c mbuf_queue_trim.c

IFNET_CFILES = get_ifn_byipaddr.c get_ifn_byname.c ifn_enum.c ifn_getaddr.c \
			   ifn_get_first.c ifn_getname.c ifn_get_next.c ifn_getstat.c \
			   ifn_ipv4_get.c ifn_ipv4_set.c ifn_register.c ifn_unregister.c \
			   ifn_getinfo.c ifn_input.c loopif.c 

PCB_CFILES = in_pcb.c pcb_getnext.c pcb_enum.c

ARP_CFILES = etharp.c arp.c

DHCP_CFILES = dhcp.c 

RAW_CFILES = raw.c raw_close.c raw_getstat.c \
			 raw_recvfrom.c raw_recvfrom_tmo.c raw_sendto.c raw_ioctl.c

UDP_CFILES = udp_bind.c udp.c udp_close.c udp_connect.c udp_enum.c \
			 udp_input.c udp_proto_getstat.c udp_recv.c udp_recv_tmo.c\
			 udp_release.c udp_send.c udp_sendto.c udp_bcast.c

TCP_CFILES = tcp_abort.c tcp_accept.c tcp_accept_tmo.c tcp_bind.c\
			 tcp.c tcp_close.c tcp_connect.c tcp_drop.c tcp_dump.c\
			 tcp_enum.c tcp_flags.c tcp_in.c tcp_keepalive.c tcp_listen.c\
			 tcp_out.c tcp_proto_getstat.c tcp_recv.c tcp_send.c\
			 tcp_shutdown.c tcp_tmr.c

CFILES = arping.c icmp.c in_chksum.c ip.c ip_route.c net.c pktbuf.c stat.c \
		 $(addprefix mbuf/,$(MBUF_CFILES))\
		 $(addprefix pcb/,$(PCB_CFILES)) $(addprefix ifnet/,$(IFNET_CFILES))\
		 $(addprefix arp/,$(ARP_CFILES)) $(addprefix raw/,$(RAW_CFILES))\
		 $(addprefix udp/,$(UDP_CFILES)) $(addprefix tcp/,$(TCP_CFILES))\
		 $(addprefix dhcp/,$(DHCP_CFILES))\
		 telnet/telopts.c

override INCPATH += ./include

include ../../mk/lib.mk


