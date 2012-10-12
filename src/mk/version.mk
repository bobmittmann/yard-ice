#
# version.mk 
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

ifndef VERSION_MAJOR
$(error VERSION_MAJOR undefined!) 
endif

ifndef VERSION_MINOR
  VERSION_MINOR = 0
endif

ifndef VERSION_DATE
  VERSION_DATE = "2012-07-23"
endif

ifndef COMPANY
  COMPANY = ""
endif

ifndef PRODUCT
  PRODUCT = ""
endif

ifndef REVISION
  REVISION = ""
endif

ifndef ASSEMBLY
  ASSEMBLY = ""
endif

ifndef PROJECT
  PROJECT = $(PROG)
endif

$(VERSION_H):
	$(ACTION) "Creating: $@"
	@NOW=`date -u +"%F %T"`; \
	S=$$((`date -d "$$NOW" +%s` - `date -d $(VERSION_DATE) +%s`));\
	H=$$(($$S / (60 * 60))); \
	VERSION_BUILD=$${H}$(VERSION_BUILD_EXTRA); \
	export VERSION_BUILD; \
	echo "/*" > $@; \
	echo " * File:    $@" >> $@; \
	echo " * Project: uBoot" >> $@; \
    echo " * Author:  Robinson Mittmann (bobmittmann@gmail.com)" >> $@; \
	echo " * Comment: Automatically generated. DO NOT edit!" >> $@; \
    echo " * Copyright(c) 2012 Bob Mittmann. All Rights Reserved." >> $@; \
	echo " */" >> $@; \
	echo  >> $@; \
	echo "#ifndef __VERSION_H__" >> $@; \
	echo "#define __VERSION_H__" >> $@; \
	echo >> $@;\
	echo "#define VERSION_NAME \"$(PROJECT)\"" >> $@; \
	echo "#define VERSION_MAJOR $(VERSION_MAJOR)" >> $@; \
	echo "#define VERSION_MINOR $(VERSION_MINOR)" >> $@; \
	echo "#define VERSION_BUILD \"$$VERSION_BUILD\"" >> $@; \
	echo "#define VERSION_NUM \"$(VERSION_MAJOR).$(VERSION_MINOR).$$VERSION_BUILD\"" >> $@; \
	echo "#define VERSION_MACH \"$(MACH)\"" >> $@; \
	echo "#define VERSION_ARCH \"$(ARCH)\"" >> $@; \
	echo "#define VERSION_CPU \"$(CPU)\"" >> $@; \
	echo "#define VERSION_STR \"$(PROJECT)-$(VERSION_MAJOR).$(VERSION_MINOR).$$VERSION_BUILD\"" >> $@; \
	echo "#define VERSION_DATE \"$$NOW\"" >> $@; \
	echo >> $@; \
	if [ "$(COMPANY)" != "" ]; then \
		echo "#define COMPANY_ID $(COMPANY)" >> $@; \
	fi; \
	if [ "$(PRODUCT)" != "" ]; then \
		echo "#define PRODUCT_ID $(PRODUCT)" >> $@; \
	fi; \
	if [ "$(REVISION)" != "" ]; then \
		echo "#define PRODUCT_REV $(REVISION)" >> $@; \
	fi; \
	if [ "$(ASSEMBLY)" != "" ]; then \
		echo "#define ASSEMBLY '$(ASSEMBLY)'" >> $@; \
	fi; \
	echo >> $@; \
	echo "#endif /* __VERSION_H__ */" >> $@
	
$(PROG_TAG):
	$(ACTION) "Creating: $@"
	@QTAG="$(PROJECT)-$(VERSION_MAJOR)_$(VERSION_MINOR)-`date -d "$$NOW" +"%Y%m%d"`"; \
	echo $$TAG > $@;
	cat $@

