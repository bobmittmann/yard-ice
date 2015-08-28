#!/bin/sh

JTAGTOOL_ADDR=192.168.10.50
BUILD_NAME=httpd_test
TOOLS_DIR=../../../tools

${TOOLS_DIR}/tftp_reset.py -q -h ${JTAGTOOL_ADDR} 

if [ $? = 0 ] ; then
	${TOOLS_DIR}/dcclog -h ${JTAGTOOL_ADDR} debug/${BUILD_NAME}.elf
fi

