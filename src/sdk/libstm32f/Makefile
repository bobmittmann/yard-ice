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

LIB_STATIC = stm32f

USART_CFILES = stm32f-usart.c stm32f-usart_io.c

USB_FS_CFILES = stm32f-usb_fs.c stm32f-usb_fs_dev.c

OTG_FS_CFILES = stm32f-otg_fs.c stm32f-otg_fs_dev.c

SFILES = stm32f-crt0.S

CFILES = stm32f-init.c stm32f-gpio.c stm32f-eth.c\
		 stm32f-spi.c stm32f-fsmc.c stm32f-mco.c\
		 $(addprefix usart/,$(USART_CFILES))\
		 $(addprefix usb_fs/,$(USB_FS_CFILES))\
		 $(addprefix otg_fs/,$(OTG_FS_CFILES))


include ../../mk/lib.mk

