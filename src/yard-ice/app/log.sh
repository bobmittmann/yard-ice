#!/bin/sh

JTAGTOOL_ADDR=192.168.10.50
BUILD_NAME=yard-ice

if [ $? == 0 ] ; then
	../../../tools/dcclog -h ${JTAGTOOL_ADDR} ../boot/debug/thinkos.elf debug/${BUILD_NAME}.elf
fi

