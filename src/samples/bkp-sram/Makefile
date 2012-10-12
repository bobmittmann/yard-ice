# File:		Makefile
# Module: 
# Project:
# Author:	Robinson Mittmann (bob@boreste.com, bobmittmann@gmail.com)
# Target:
# Comment:  
# Copyright(c) 2006-2009 BORESTE (www.boreste.com). All Rights Reserved.

MACH = stm32f207

VERSION_MAJOR = 0

PROG = bkp-sram

CFILES = bkp-sram.c 

LIBDIRS = ../../stm32f ../../libcm3 ../../libutil ../../libc

LIBS = stm32f cm3 util c

CFLAGS = -O1
CDEFS = "CM3_SYSTICK_CLK_HZ=15000000"

JTAGTOOL_ADDR = 192.168.1.30

include ../../mk/cortex-m3.mk

