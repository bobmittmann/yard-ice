#!/bin/sh

JTAGTOOL_ADDR=192.168.0.51
BUILD_NAME=thinkos-0.23
TOOLS_DIR=../../thinkos/tools

${TOOLS_DIR}/tftp_load.py -q -i -e -r  -a 0x08000000 -h ${JTAGTOOL_ADDR} \
		debug/${BUILD_NAME}.bin 

if [ $? = 0 ] ; then
	# Disable the halt debug mode by clearing C_DEBUGEN on DHCSR
	${TOOLS_DIR}/tftp_cmd.py -h ${JTAGTOOL_ADDR} 'disable debug'
fi

if [ $? = 0 ] ; then
	${TOOLS_DIR}/dcclog -h ${JTAGTOOL_ADDR} debug/${BUILD_NAME}.elf 
fi

