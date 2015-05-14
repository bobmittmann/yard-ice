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

LIB_STATIC = stm32f

USART_CFILES = stm32f-usart.c stm32f-usart_init.c\
			   stm32f-usart_baudrate_set.c stm32f-usart_baudrate_get.c\
			   stm32f-usart_mode_set.c\
			   stm32f-usart_enable.c stm32f-usart_disable.c\
			   stm32f-usart_power_off.c stm32f-usart_flush.c\
			   stm32f-usart_getc.c stm32f-usart_putc.c\
			   stm32f-usart_read.c stm32f-usart_write.c\
			   stm32f-usart_canon_write.c\
			   stm32f-usart_fops.c stm32f-usart_fops_raw.c stm32f-usart_legacy.c

USB_FS_CFILES = stm32f-usb_fs.c stm32f-usb_fs_dev.c

OTG_FS_CFILES = stm32f-otg_fs.c stm32f-otg_fs_dev.c

OTG_HS_CFILES = stm32f-otg_hs.c stm32f-otg_hs_dev.c

ifeq ($(STM32), stm32l)

SFILES = stm32l-vec.S

CFILES = stm32l-init.c stm32l-flash.c stm32f-gpio.c stm32l-eeprom.c \
		 stm32f-dma.c \
		 $(addprefix usart/,$(USART_CFILES))

else

SFILES = stm32f-vec.S

CFILES = stm32f2x-init.c stm32f1x-init.c stm32f-gpio.c stm32f-eth.c\
		 stm32f-spi.c stm32f-fsmc.c  stm32f-exti.c stm32f-mco.c\
		 stm32f-dma.c stm32f-flash.c stm32f-rtc.c\
		 $(addprefix usart/,$(USART_CFILES))\
		 $(addprefix usb_fs/,$(USB_FS_CFILES))\
		 $(addprefix otg_fs/,$(OTG_FS_CFILES))\
		 $(addprefix otg_hs/,$(OTG_HS_CFILES))

endif




include ../../mk/lib.mk

