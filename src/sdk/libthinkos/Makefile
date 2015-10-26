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

LIB_STATIC = thinkos

DMON_CFILES = thinkos_dmon.c usb_comm.c \
			  print_osinfo.c print_alloc.c print_thread.c \
			  print_context.c print_stack_usage.c soft_reset.c \
			  app_exec.c flash_erase.c flash_ymodem.c \
			  print_exception.c dmgets.c dmprintf.c dmscanf.c  

MISC_CFILES = thinkos_util.c thinkos_type.c obj_type_get.c \
			  alloc_bmp_get.c bmp_alloc.c \
			  memcpy.c memcpy32.c memset32.c active.c suspended.c \
			  thread_getnext.c thread_get.c thread_inf_set.c \
			  thread_isfaulty.c thread_ispaused.c exec.c\
			  thread_isalive.c pause_all.c resume_all.c xcpt_util.c

CONSOLE_CFILES = thinkos_console.c bypass.c 
CFILES = thinkos_core.c thinkos_svc.c thinkos_init.c \
		 thinkos_thread.c thinkos_mutex.c thinkos_cond.c \
		 thinkos_semaphore.c thinkos_event.c thinkos_irq.c \
		 thinkos_flag.c thinkos_except.c thinkos_debug.c \
		 thinkos_sleep.c thinkos_break.c thinkos_pause.c \
		 thinkos_exit.c thinkos_join.c thinkos_cancel.c \
		 thinkos_gate.c thinkos_ctl.c thinkos_idle.c thinkos_mpu.c \
		 $(addprefix misc/,$(MISC_CFILES)) \
		 $(addprefix dmon/,$(DMON_CFILES)) \
		 $(addprefix console/,$(CONSOLE_CFILES))

include ../../mk/lib.mk

