#!/bin/sh

if [ "$1" = "" ]; then
	LVL="5"
else
	LVL=$1
fi

export JTAGTOOL_ADDR=192.168.10.50

make D=$LVL jtagload && ../../../tools/dcclog -h 192.168.10.50 debug/mjs-test.elf

