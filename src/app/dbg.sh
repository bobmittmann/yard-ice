#!/bin/sh

BUILD_NAME=yard-ice
JTAGTOOL_ADDR=192.168.10.50
TOOLS_DIR=../../thinkos/tools
THINKOS_ELF=../boot/debug/thinkos-0.20.elf
THINKAPP_ELF=debug/${BUILD_NAME}.elf

${TOOLS_DIR}/tftp_load.py -q -e -r -a 0x08020000 -h ${JTAGTOOL_ADDR} debug/${BUILD_NAME}.bin 

if [ $? == 0 ] ; then
	${TOOLS_DIR}/dcclog -h ${JTAGTOOL_ADDR} ${THINKOS_ELF} ${THINKAPP_ELF}
fi

