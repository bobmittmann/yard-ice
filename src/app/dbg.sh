#!/bin/sh

BUILD_NAME=yard-ice-0.32
JTAGTOOL_ADDR=192.168.10.51
TOOLS_DIR=../../thinkos/tools
THINKAPP_ELF=debug/${BUILD_NAME}.elf

${TOOLS_DIR}/tftp_load.py -q -e -r -a 0x08020000 -h ${JTAGTOOL_ADDR} debug/${BUILD_NAME}.bin 

if [ $? == 0 ] ; then
	${TOOLS_DIR}/dcclog -h ${JTAGTOOL_ADDR} ${THINKAPP_ELF}
fi

