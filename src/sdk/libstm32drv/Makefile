#
# Copyright(c) 2006-2012 BORESTE (www.boreste.com). All Rights Reserved.
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

#
# File:   Makefile
# Author: Robinson Mittmann <bobmittmann@gmail.com>
# 

include ../../mk/config.mk

LIB_STATIC = stm32drv

SERIAL_CFILES = stm32f-serial.c stm32f-serial_dma.c \
				stm32f-usart1.c stm32f-usart2.c \
				stm32f-uart5.c \
				stm32f-usart6.c stm32f-uart7.c \
				stm32f-usart1_dma.c stm32f-usart2_dma.c stm32f-usart3_dma.c \
				stm32f-uart4_dma.c stm32f-uart5_dma.c stm32f-usart6_dma.c \
				stm32f-uart7_dma.c stm32f-uart8_dma.c \
				serial-file.c

CFILES = stm32f-ethif.c stm32f-dac.c stm32f-adc.c \
		 stm32f-bkp_sram.c stm32f-rtc.c \
		 $(addprefix serial/,$(SERIAL_CFILES))

override INCPATH += ./include ../libtcpip/include

include ../../mk/lib.mk


