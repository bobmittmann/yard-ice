# File:		Makefile
# Module: 
# Target:
# Comment:  
# Copyright(C) 2012 Bob Mittmann. All Rights Reserved.
# 
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
# 


PROG = rtsp_test

CFILES = audio.c jitbuf.c rtcp.c rtp.c rtsp.c rtsp_hdr.c rtsp_test.c \
		 sdp.c sndbuf.c webserver.c

SDK = ../../../sdk

#LIBDIRS = $(SDK)/libcm3 $(SDK)/libice-comm \
#		  $(SDK)/libstm32f $(SDK)/libstm32drv $(SDK)/libutil \
#		  $(SDK)/libc $(SDK)/libconsole $(SDK)/libtcpip $(SDK)/libhttpd \
#		  $(SDK)/libtrace $(SDK)/libcrc

LIBS = gcc cm3 ice-comm stm32f util stm32drv c console tcpip httpd trace crc

INCPATH = $(SDK)/include ../include

CDEFS = CONFIG_H
CFLAGS = -g -O1 -Wall -ffunction-sections -fdata-sections
LDFLAGS = -Wl,--gc-sections 

RELEASE_DIR := Release
DEBUG_DIR := Debug
#LIBPATH = ../sdklibs/Debug
LIBPATH = ../sdklibs/Release

JTAGTOOL_ADDR= 192.168.10.50

MACH = stm32f407
THINKAPP = 1
LDSCRIPT = ../stm32f407.ld
LOAD_ADDR = 0x08010000
include ../../../mk/stm32f4xx.mk
