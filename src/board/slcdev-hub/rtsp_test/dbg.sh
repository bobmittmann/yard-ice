#!/bin/sh

JTAGTOOL_ADDR=192.168.10.50
BUILD_NAME=rtsp_test
TOOLS_DIR=../../../../tools

${TOOLS_DIR}/tftp_load.py -q -e -r -a 0x08010000 -h ${JTAGTOOL_ADDR} debug/${BUILD_NAME}.bin 

if [ $? == 0 ] ; then
	${TOOLS_DIR}/dcclog -h ${JTAGTOOL_ADDR} ../boot/debug/thinkos.elf debug/${BUILD_NAME}.elf
fi

