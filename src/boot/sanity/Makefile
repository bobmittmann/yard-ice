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

PROJECTDIR = ../../../
THINKOSDIR = $(PROJECTDIR)/thinkos
SDKDIR = $(THINKOSDIR)/sdk
ARCHDIR = $(THINKOSDIR)/arch/stm32

PROG = sanity

CFILES = sanity.c timer.c

LIBDIRS = $(SDKDIR)/libthinkos-usr \
		  $(SDKDIR)/libtty $(SDKDIR)/libutil \
		  $(SDKDIR)/libc $(ARCHDIR)/libstm32f 

LIBS = m util c thinkos-usr tty stm32f

INCPATH = $(PROJECTDIR)/src/include

CDEFS = CONFIG_H 
CFLAGS = -Og

MACH = stm32f207xe
LDCODE := 0x08020000
LDDATA := 0x20002000
include $(ARCHDIR)/mk/stm32-app.mk

