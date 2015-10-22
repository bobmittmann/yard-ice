#!/bin/sh
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
#    File: backup.sh
# Comment: Backup
#  Author: Robinson Mittmann <bobmittmann@gmail.com>
# 

BKPDIR=`cd ..; pwd`

SRCDIR=`pwd`
BASENAME=`basename ${SRCDIR}`
BKPNAME=${BASENAME}-`date +%Y-%m-%d-%H`.tar.gz
BKPPATH=${BKPDIR}/${BKPNAME}
EXCLUDE="${BASENAME}/tmp* ${BASENAME}/*/.deps ${BASENAME}/*/.metadata"
EXCLUDE=${EXCLUDE}" ${BASENAME}/*/*.elf ${BASENAME}/*/*.exe" 
EXCLUDE=${EXCLUDE}" ${BASENAME}/*/*.o ${BASENAME}/*/*.a ${BASENAME}/*/*.lib" 
EXCLUDE=${EXCLUDE}" ${BASENAME}/*/*.lst ${BASENAME}/*/*.bak" 

echo 
echo Backup file: ${BKPPATH}
echo ------------
echo 

cd ..

for p in ${EXCLUDE} ; do
	TAR_ARGS="${TAR_ARGS} --exclude=$p"
done

tar -vzc --exclude-vcs --exclude="*~" ${TAR_ARGS} -f ${BKPPATH} ${BASENAME} 

echo 
echo ${BKPPATH}
echo 

