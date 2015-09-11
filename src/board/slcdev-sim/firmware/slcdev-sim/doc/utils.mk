#
# utils.mk 
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

THISDIR := $(dir $(lastword $(MAKEFILE_LIST)))

ifndef AUXDIR
  TOOLSDIR := $(abspath $(THISDIR))
  BASEDIR := $(abspath $(THISDIR)/..)
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
  ifneq (,$(findstring MINGW, $(MSYSTEM)))
#   $(info Windows MinGW/Msys Host)
   HOST := Msys
   DIRMODE := windows
  else 
   ifneq (,$(HOME))
#    $(info Windows Cygwin Host)
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
 FIXDEP := echo
 PYTHON := python
 TXT2TAGS = txt2tags
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
 FIXDEP := echo FIXDEP 
 PYTHON := python
 TXT2TAGS = $(PYTHON) $(TOOLSDIR)/txt2tags-2.6.py
else
ifeq ($(HOST),Msys)
 CAT := cat
 RMALL := rm -f
 RMDIR := rm -fr
 CP := cp
 MV := mv    
 MKDIR := mkdir
 ECHO := echo
 DEVNULL := /dev/null
 FIXDEP := echo
 PYTHON := '\Python27\python'
 TXT2TAGS = $(PYTHON) $(TOOLSDIR)/txt2tags-2.6.py
else
ifeq ($(HOST),Windows)
 CAT := type
 RMALL := del /F /Q 
 CP := copy 
 MV := ren
 MKDIR := mkdir
 ECHO := echo
 DEVNULL := NUL:
 FIXDEP := echo
 PYTHON := '\Python27\python'
 TXT2TAGS = $(PYTHON) $(TOOLSDIR)/txt2tags-2.6.py
else
 $(error Unsuported HOST)
endif
endif
endif
endif

#------------------------------------------------------------------------------ 
# Scripts
#------------------------------------------------------------------------------ 


#------------------------------------------------------------------------------ 
# Document transformations
#------------------------------------------------------------------------------ 


STYLE = $(TOOLSDIR)/tech.css

ifeq (Windows,$(HOST))
  STYLE := $(subst /,\,$(STYLE))
endif

.SUFFIXES:

%.html : %.t2t
	@$(TXT2TAGS) -t html --style=$(STYLE) --css-sugar --css-inside --toc -o $@ $<

