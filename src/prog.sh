#!/bin/sh

JTAGTOOL_ADDR=192.168.10.50
TOOLS_DIR=../thinkos/tools
THINKOS=boot/release/thinkos-0.21
THINKAPP=app/release/yard-ice

${TOOLS_DIR}/tftp_load.py -q -i -e -a 0x08000000 -h ${JTAGTOOL_ADDR} ${THINKOS}.bin 
if [ $? == 0 ] ; then
	${TOOLS_DIR}/tftp_load.py -q -e -r -a 0x08020000 -h ${JTAGTOOL_ADDR} ${THINKAPP}.bin 
fi

