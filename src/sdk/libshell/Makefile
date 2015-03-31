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

LIB_STATIC = shell

CFILES = shell.c get_next.c exec.c lookup.c history.c parseline.c stripline.c cmd_set.c cmd_get.c \
		 cmd_unset.c cmd_echo.c cmd_close.c cmd_sleep.c cmd_reboot.c \
		 cmd_memxxd.c cmd_osinfo.c cmd_netstat.c cmd_ifconfig.c \
		 cmd_ping.c cmd_route.c cmd_arp.c cmd_ipcfg.c cmd_close.c \
		 cmd_clear.c cmd_cfgerase.c

INCPATH = ../include

include ../../mk/lib.mk

