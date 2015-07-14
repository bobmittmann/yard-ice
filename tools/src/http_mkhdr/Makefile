# File: Makefile
# Module:   
# Project: 
# Author: Robinson Mittmann (bobmittmann@gmail.com)
# Target:
# Comment:  

PROJECT = mkhdr 

CFILES = mkhdr.c
OFILES = $(CFILES:.c=.o)

INCPATH = ./include ../include

SFLAGS = -Wall
CFLAGS = -Wall -O
LDFLAGS =

CC  = gcc
LD  = gcc
AS  = as
AR  = ar
OBJCOPY = objcopy
OBJDUMP = objdump
STRIP = strip

all: Makefile $(PROJECT)

clean: 
	@rm -fv http_*.* mime.* $(OFILES) mkhdr.exe

.PHONY: all clean

.SUFFIXES:

$(PROJECT): Makefile $(OFILES)
	$(LD) $(OPTIONS) $(LDFLAGS) $(addprefix -L,$(LIBPATH)) -o $@ $(OFILES) \
	$(addprefix -l,$(LIBS))

%.o : %.c Makefile
	$(CC) $(OPTIONS) $(CFLAGS) $(addprefix -I,$(INCPATH)) -o $@ -c $<

%.o : %.S Makefile
	$(AS) $(OPTIONS) $(SFLAGS) $(addprefix -I,$(INCPATH)) -o $@ -c $<


