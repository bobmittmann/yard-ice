#!/bin/sh

JTAGTOOL_ADDR=192.168.10.50
BUILD_NAME=simrpc-test
TOOLS_DIR=../../../../tools

${TOOLS_DIR}/tftp_load.py -q -i -e -r  -a 0x08000000 -h ${JTAGTOOL_ADDR} \
		debug/${BUILD_NAME}.bin 

if [ $? = 0 ] ; then
	# Disable the halt debug mode by clearing C_DEBUGEN on DHCSR
	${TOOLS_DIR}/tftp_cmd.py -h ${JTAGTOOL_ADDR} \
		'disable poll' 'let dhcsr 0xa05f0000' 'enable poll'
fi

if [ $? = 0 ] ; then
	${TOOLS_DIR}/dcclog -h ${JTAGTOOL_ADDR} debug/${BUILD_NAME}.elf | tee dbg.log
fi

