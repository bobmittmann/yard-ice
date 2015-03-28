#
# Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
# 
# This file is part of the YARD-ICE.
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
# File:   Makefile
# Author: Robinson Mittmann <bobmittmann@gmail.com>
# 

include ../../mk/config.mk

LIB_STATIC = bacnet

STACK_CFILES = abort.c alarm_ack.c apdu.c arf.c awf.c \
			   bacaddr.c bacapp.c bacdcode.c bacdevobjpropref.c \
			   bacerror.c bacint.c bacprop.c bacpropstates.c \
			   bacreal.c bacstr.c bactext.c bigend.c \
			   cov.c crc.c datalink.c datetime.c dcc.c debug.c \
			   event.c fifo.c filename.c get_alarm_sum.c getevent.c \
			   iam.c ihave.c indtext.c key.c keylist.c lso.c memcopy.c \
			   npdu.c proplist.c ptransfer.c \
			   rd.c readrange.c reject.c ringbuf.c rp.c rpm.c \
			   sbuf.c timestamp.c timesync.c tsm.c version.c whohas.c \
			   whois.c wp.c wpm.c address.c 
#			   bvlc.c 

#mstp.c mstptext.c bip.c 

HANDLER_CFILES = h_alarm_ack.c h_arf.c h_arf_a.c h_awf.c \
				 h_ccov.c h_cov.c h_dcc.c h_gas_a.c h_get_alarm_sum.c \
				 h_getevent.c h_iam.c h_ihave.c h_lso.c h_npdu.c h_pt.c \
				 h_pt_a.c h_rd.c h_routed_npdu.c h_rp.c h_rp_a.c h_rpm.c \
				 h_rpm_a.c h_rr.c h_rr_a.c h_ts.c h_ucov.c h_upt.c \
				 h_whohas.c h_whois.c h_wp.c h_wpm.c noserv.c objects.c \
				 s_ack_alarm.c s_arfs.c s_awfs.c s_cevent.c s_cov.c \
				 s_dcc.c s_iam.c s_ihave.c s_lso.c s_ptransfer.c s_rd.c \
				 s_readrange.c s_router.c s_rp.c s_rpm.c s_ts.c s_uevent.c \
				 s_upt.c s_whohas.c s_whois.c s_wp.c s_wpm.c txbuf.c
#dlenv.c 

OBJECT_CFILES = lo.c lsp.c ai.c ao.c av.c bi.c bo.c \
				bv.c csv.c ms-input.c mso.c msv.c nc.c \
				gw_device.c 
#				lc.c 
#trendlog.c
#bacfile.c 
#device.c 
#device-client.c 

CFILES = $(addprefix bacnet-stack/src/,$(STACK_CFILES))\
		 $(addprefix bacnet-stack/demo/handler/,$(HANDLER_CFILES))\
		 $(addprefix bacnet-stack/demo/object/,$(OBJECT_CFILES))\
		 timer.c bacnet_dl.c bacnet_ptp.c bacnet_crc.c bacnet_mstp.c

INCPATH = ../include 

override INCPATH += bacnet-stack/include bacnet-stack/demo/object
override CFLAGS += -Wno-unused
 
include ../../mk/lib.mk

