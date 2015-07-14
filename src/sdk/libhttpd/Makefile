# File:		Makefile
# Author:   Robinson Mittmann (bobmittmann@gmail.com)
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

include ../../mk/config.mk

LIB_STATIC = httpd

CFILES = 200.c 400.c 401.c 403.c 404.c 405.c 408.c 409.c 410.c \
		411.c 413.c 414.c 415.c 500.c 501.c 503.c 505.c \
		base64.c nvparse.c httpd.c get.c post.c http_hdr.c http_mime.c \
		http_resp.c http_recv.c http_send.c
		 

override INCPATH += ../include ./include

include ../../mk/lib.mk

