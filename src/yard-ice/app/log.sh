#!/bin/sh

JTAGTOOL_ADDR=192.168.10.50
BUILD_NAME=yard-ice
TOOLS_DIR=../../../tools

${TOOLS_DIR}/tftp_reset.py -q -h ${JTAGTOOL_ADDR} 

if [ $? == 0 ] ; then
	../../../tools/dcclog -h ${JTAGTOOL_ADDR} ../boot/debug/thinkos.elf debug/${BUILD_NAME}.elf
fi
