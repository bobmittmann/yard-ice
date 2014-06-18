#!/bin/sh

if [ "$1" = "" ]; then
	LVL="5"
else
	LVL=$1
fi

#make -j 6 D=$LVL && make D=$LVL jtagload && make D=$LVL logview
make D=$LVL jtagload && ../../../tools/dcclog -h 192.168.10.50 debug/slcdev-hub.elf

