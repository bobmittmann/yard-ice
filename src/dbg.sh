#!/bin/sh

JTAGTOOL_ADDR=192.168.10.51
TOOLS_DIR=../thinkos/tools
THINKOS=boot/debug/thinkos-0.22
THINKAPP=app/debug/yard-ice-0.32

${TOOLS_DIR}/tftp_load.py -q -i -e -a 0x08000000 -h ${JTAGTOOL_ADDR} ${THINKOS}.bin 
if [ $? == 0 ] ; then
	${TOOLS_DIR}/tftp_load.py -q -e -r -a 0x08020000 -h ${JTAGTOOL_ADDR} ${THINKAPP}.bin 
	if [ $? == 0 ] ; then
		${TOOLS_DIR}/dcclog -h ${JTAGTOOL_ADDR} ${THINKOS}.elf ${THINKAPP}.elf
	fi
fi


