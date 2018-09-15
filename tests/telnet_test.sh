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
#    File: telnet_test.sh 
# Comment: TELNET tester
#  Author: Robinson Mittmann <bobmittmann@gmail.com>
# 

PROG=`basename $0`
VER_MINOR=2
VER_MAJOR=0

HOST="192.168.10.50"
TESTFILE=telnet_test.txt

CNT=0
while [  $CNT -lt 100 ]; do
	echo $CNT
	printf "\rrelease\rtgt stm32f force\rreset\rconnect\r" | \
		nc -v -t ${HOST} 23 
	CNT=$((CNT + 1))
done

