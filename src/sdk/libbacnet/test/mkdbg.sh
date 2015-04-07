#!/bin/sh

if [ "$1" = "" ]; then
	LVL="5"
else
	LVL=$1
fi

JTAGTOOL_ADDR=192.168.10.50
BUILD_NAME=bacnet_test

make JTAGTOOL_ADDR=${JTAGTOOL_ADDR} D=$LVL jtagload 

if [ $? = 0 ] ; then
	../../../../tools/dcclog -h ${JTAGTOOL_ADDR} debug/${BUILD_NAME}.elf

fi
