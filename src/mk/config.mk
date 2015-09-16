#
# config.mk 
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

CONFIG_MK := $(lastword $(MAKEFILE_LIST))

THISDIR := $(dir $(CONFIG_MK))
SCRPTDIR := $(abspath $(THISDIR))
BASEDIR := $(abspath $(THISDIR)/..)
TOOLSDIR := $(abspath $(THISDIR)/../../tools)
LDDIR := $(abspath $(THISDIR)/../ld)
SRCDIR := $(abspath $(dir $(firstword $(MAKEFILE_LIST))))

#------------------------------------------------------------------------------ 
# default output directories
#------------------------------------------------------------------------------ 
RELEASE_DIR := release
DEBUG_DIR := debug

#------------------------------------------------------------------------------ 
# parameters 
#------------------------------------------------------------------------------ 
ifdef V
  verbose := $(V)
else
  verbose := 0
endif

ifdef D
  dbg_level := $(D)
else
  dbg_level := 0
endif

#------------------------------------------------------------------------------ 
# Host OS detection
#------------------------------------------------------------------------------ 

ifeq (, $(OS))
 OS := $(shell uname -o) 
 ifneq (,$(findstring Linux, $(OS)))
# $(info Linux Host)
  HOST := Linux
  DIRMODE := unix
 else
  $(error Unsuported OS)
 endif
else
 ifneq (,$(findstring Windows, $(OS)))
  ifeq ($(PROGRAMFILES\(X86\))$(ProgramFiles\(x86\)),)
#   $(info Windows 32bits)
   WIN := Win32
  else
#   $(info Windows 64bits)
   WIN := Win64
  endif
  ifneq (,$(findstring MINGW32, $(MSYSTEM)))
#   $(info Windows MinGW/Msys Host)
   ifneq (, $(MSYSCON))
     $(info MSYSCON = '$(MSYSCON)')
   endif
   ifneq (, $(MAKE_MODE))
     $(info MAKE_MODE = '$(MAKE_MODE)')
   endif
   HOST := Msys
   DIRMODE := windows
  else 
   ifeq (+++, $(firstword $(subst /,+++ ,$(BASEDIR))))
#    $(info Windows Cygwin Host, $(shell cygpath -W))
    HOST := Cygwin
    DIRMODE := unix
   else
#    $(info Windows Native Host)
    HOST := Windows
    DIRMODE := windows
   endif
  endif
 endif
endif

#------------------------------------------------------------------------------ 
# Other macros
#------------------------------------------------------------------------------ 

ifeq ($(verbose),0) 
  Q := @
  ACTION := @echo
#  ECHO := @\#
else
  Q :=
 ifeq ($(HOST),Windows)
  ACTION := @rem
 else
  ACTION := @\#
 endif
#  ECHO := @echo
endif

#------------------------------------------------------------------------------ 
# OS specific helper macros
#------------------------------------------------------------------------------ 

ifeq ($(HOST),Linux)
 CAT := cat
 RMALL := rm -f 
 RMDIR := rm -fr
 CP := cp
 MV := mv    
 MKDIR := mkdir -p
 ECHO := echo
 DEVNULL := /dev/null
else
ifeq ($(HOST),Cygwin)
 CAT := cat
 RMALL := rm -f 
 RMDIR := rm -fr
 CP := cp
 MV := mv    
 MKDIR := mkdir -p
 ECHO := echo
 DEVNULL := /dev/null
else
ifeq ($(HOST),Msys)
 CAT := cat
 RMALL := rm -f
 RMDIR := rm -fr
 CP := cp
 MV := mv    
 MKDIR := mkdir -p
 ECHO := echo
 DEVNULL := /dev/null
else
ifeq ($(HOST),Windows)
 CAT := type
 RMALL := del /F /Q 
 CP := copy 
 MV := ren
 MKDIR := mkdir
 ECHO := echo
 DEVNULL := NUL:
else
 $(error Unsuported HOST)
endif
endif
endif
endif


ifdef O
  OUTDIR := $(abspath $(O))
else
  ifneq ($(dbg_level),0)
    OUTDIR := $(abspath $(DEBUG_DIR))
  else
    OUTDIR := $(abspath $(RELEASE_DIR))
  endif
endif

# ----------------------------------------------------------------------------
# Function: windrv
#   Return: windows driver letter
# ----------------------------------------------------------------------------
windrv = $(patsubst //j/%,j:/%,$(patsubst //i/%,i:/%,$(patsubst //h/%,h:/%,$(patsubst //h/%,h:/%,$(patsubst //g/%,g:/%,$(patsubst //f/%,f:/%,$(patsubst //e/%,e:/%,$(patsubst //d/%,d:/%,$(patsubst //c/%,c:/%,$(addprefix /,$1))))))))))

ifeq ($(HOST),Windows)
  OUTDIR := $(subst /,\,$(OUTDIR))
  SRCDIR := $(subst /,\,$(SRCDIR))
else
ifeq ($(HOST),Msys)
#  OUTDIR := $(call windrv,$(OUTDIR))
#  SRCDIR := $(call windrv,$(SRCDIR))
endif
endif

export OUTDIR Q ACTION

