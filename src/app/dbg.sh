#!/bin/sh

JTAGTOOL_ADDR=192.168.10.50
BUILD_NAME=yard-ice

../../../tools/tftp_load.py -q -e -r -a 0x08020000 -h ${JTAGTOOL_ADDR} debug/${BUILD_NAME}.bin 

if [ $? == 0 ] ; then
	../../../tools/dcclog -h ${JTAGTOOL_ADDR} ../boot/debug/thinkos.elf debug/${BUILD_NAME}.elf
fi

