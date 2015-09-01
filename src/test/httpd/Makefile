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


PROG = httpd_test

CFILES = httpd_test.c tcp_echo.c stdio.c www_root.c www_img.c www_lib.c \
		 www_cgi.c zarathustra.html.c treeview.html.c treeview.css.c tftp.c \
		 netcat.c tcp_qotd.c thinkos.html.c

SDK = ../../sdk
TOOLS = ../../../tools

LIBDIRS = $(SDK)/libstm32f $(SDK)/libcm3 $(SDK)/libutil \
		  $(SDK)/libthinkos $(SDK)/libc $(SDK)/libice-comm \
		  $(SDK)/libtcpip $(SDK)/libstm32drv $(SDK)/libtty $(SDK)/libcrc \
		  $(SDK)/libhttpd $(SDK)/libshell

LIBS = cm3 stm32f util thinkos c ice-comm crc tcpip stm32drv tty httpd shell

INCPATH = $(SDK)/include  
CDEFS = CONFIG_H
CFLAGS = -g -O1 -Wall -ffunction-sections -fdata-sections
LDFLAGS = -Wl,--gc-sections

JTAGTOOL_ADDR = 192.168.10.50
MACH = stm32f407
include ../../mk/stm32f4xx.mk

#JTAGTOOL_ADDR = 192.168.0.50
#MACH = stm32f207
#include ../../../mk/stm32f2xx.mk

%.c: %.png
	$(TOOLS)/bin2hex.py $< > $@ 

%.c: %.jpg
	$(TOOLS)/bin2hex.py $< > $@ 

%.c: %.js
	$(TOOLS)/bin2hex.py $< > $@ 

%.c: %.gz
	$(TOOLS)/bin2hex.py -H www.h $< > $@ 

#%.html.c: %.html.gz
#	$(TOOLS)/bin2hex.py -H www.h $< > $@ 

#%.css.c: %.css.gz
#	$(TOOLS)/bin2hex.py -H www.h $< > $@ 

%.html.c: %.html
	$(TOOLS)/bin2hex.py -H www.h $< > $@ 

%.html.gz: %.html
	gzip -c $< > $@ 

%.css.gz: %.css
	gzip -c $< > $@ 

