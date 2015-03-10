#!/bin/sh

if [ "$1" = "" ]; then
	LVL="5"
else
	LVL=$1
fi

JTAGTOOL_ADDR=192.168.10.50
BUILD_NAME=httpd_test

make D=$LVL jtagload && ../../../../tools/dcclog -h ${JTAGTOOL_ADDR} debug/${BUILD_NAME}.elf
