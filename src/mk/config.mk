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

#
#
#

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
ifndef $(RELEASE_DIR) 
  RELEASE_DIR := release
endif

ifndef $(DEBUG_DIR) 
  DEBUG_DIR := debug
endif

#------------------------------------------------------------------------------ 
# parameters
# <V> verbose, can be either 0 or 1
# <D> debug level: (0 == release, no debug)
# <O> output directory
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

ifdef O
  out_dir := $(O)
else
  # if the output directory is not set select the appropriate one according
  # to debug level.
  ifneq ($(dbg_level),0)
    out_dir := $(DEBUG_DIR)
  else
    out_dir := $(RELEASE_DIR)
  endif
endif

#------------------------------------------------------------------------------ 
# Host OS detection
#------------------------------------------------------------------------------ 

ifeq (, $(OS))
  OS := $(shell uname -o) 
  ifneq (,$(findstring Linux, $(OS)))
#   $(info Linux Host)
    HOST := Linux
    DIRMODE := unix
  else
    $(error Unsuported OS)
  endif
else
  # check if we are in a windows environment
  ifneq (,$(findstring Windows, $(OS)))
    ifeq ($(PROGRAMFILES\(X86\))$(ProgramFiles\(x86\)),)
#     $(info Windows 32bits)
      WIN := Win32
    else
#     $(info Windows 64bits)
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
#       $(info Windows Cygwin Host, $(shell cygpath -W))
        HOST := Cygwin
        DIRMODE := unix
      else
       $(info Windows Native Host)
        HOST := Windows
        DIRMODE := windows
        SHELL := $(ComSpec) # Force the shell to the Windows Command Prompt
        export SHELL
      endif
    endif
  else
    ifndef HOST 
      $(error Unable to detect the host OS type)
    endif
  endif
endif

#------------------------------------------------------------------------------ 
# Other macros
#------------------------------------------------------------------------------ 

ifeq ($(verbose),0) 
  Q := @
  ACTION := @echo
else
  Q :=
  ifeq ($(HOST),Windows)
    ACTION := @rem
  else
    ACTION := @\#
  endif
endif

#------------------------------------------------------------------------------ 
# OS specific helper macros
#------------------------------------------------------------------------------ 

ifeq ($(HOST),Linux)
  CAT := cat
  RMALL := rm -f 
  RMDIR := rm -fr
  MKDIR := mkdir -p
  CP := cp
  MV := mv    
  ECHO := echo
  DEVNULL := /dev/null
else
ifeq ($(HOST),Cygwin)
  CAT := cat
  RMALL := rm -f 
  RMDIR := rm -fr
  MKDIR := mkdir -p
  CP := cp
  MV := mv    
  ECHO := echo
  DEVNULL := /dev/null
else
ifeq ($(HOST),Msys)
  CAT := cat
  RMALL := rm -f
  RMDIR := rm -fr
  MKDIR := mkdir -p
  CP := cp
  MV := mv    
  ECHO := echo
  DEVNULL := /dev/null
else
ifeq ($(HOST),Windows)
  CAT := type
  RMALL := del /F /Q 
  RMDIR := rmdir
  MKDIR := mkdir
  CP := copy 
  MV := ren
  ECHO := echo
  DEVNULL := NUL:
else
  $(error Unsuported HOST)
endif
endif
endif
endif

# ----------------------------------------------------------------------------
# Function: windrv
#   Return: windows driver letter
# This macro converts an absolute path from UNIX into windows style, ex:
# "/c/directory/file" will be turn into: "c:/directory/file".
# It works by adding the prefix '+' into the path and searching for a 
# matching patters: "+/c/", "+/d/" ... "+/z/", then replacing the pattern
# by driver letter followed by colon and a slash: "c:/", "d:/" .. "z:/"
# ----------------------------------------------------------------------------
windrv = $(patsubst +/z/%,z:/%,$(patsubst +/y/%,y:/%,$(patsubst +/w/%,w:/%,$(patsubst +/v/%,v:/%,$(patsubst +/u/%,u:/%,$(patsubst +/t/%,t:/%,$(patsubst +/s/%,s:/%,$(patsubst +/r/%,r:/%,$(patsubst +/q/%,q:/%,$(patsubst +/p/%,p:/%,$(patsubst +/o/%,o:/%,$(patsubst +/n/%,n:/%,$(patsubst +/m/%,m:/%,$(patsubst +/l/%,l:/%,$(patsubst +/k/%,k:/%,$(patsubst +/j/%,j:/%,$(patsubst +/i/%,i:/%,$(patsubst +/h/%,h:/%,$(patsubst +/h/%,h:/%,$(patsubst +/g/%,g:/%,$(patsubst +/f/%,f:/%,$(patsubst +/e/%,e:/%,$(patsubst +/d/%,d:/%,$(patsubst +/c/%,c:/%,$(addprefix +,$1)))))))))))))))))))))))))

# ----------------------------------------------------------------------------
# Function: lc
#   Return: text in lower case
# ----------------------------------------------------------------------------
lc = $(subst A,a,$(subst B,b,$(subst C,c,$(subst D,d,$(subst E,e,$(subst F,f,$(subst G,g,$(subst H,h,$(subst I,i,$(subst J,j,$(subst K,k,$(subst L,l,$(subst M,m,$(subst N,n,$(subst O,o,$(subst P,p,$(subst Q,q,$(subst R,r,$(subst S,s,$(subst T,t,$(subst U,u,$(subst V,v,$(subst W,w,$(subst X,x,$(subst Y,y,$(subst Z,z,$1))))))))))))))))))))))))))

# ----------------------------------------------------------------------------
# Function: uc
#   Return: text in upper case
# ----------------------------------------------------------------------------
uc = $(subst a,A,$(subst b,B,$(subst c,C,$(subst d,D,$(subst e,E,$(subst f,F,$(subst g,G,$(subst h,H,$(subst i,I,$(subst j,J,$(subst k,K,$(subst l,L,$(subst m,M,$(subst n,N,$(subst o,O,$(subst p,P,$(subst q,Q,$(subst r,R,$(subst s,S,$(subst t,T,$(subst u,U,$(subst v,V,$(subst w,W,$(subst x,X,$(subst y,Y,$(subst z,Z,$1))))))))))))))))))))))))))

OUTDIR := $(abspath $(out_dir))

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

