#!/bin/sh

JTAGTOOL_ADDR=192.168.10.50
TOOLS_DIR=../thinkos/tools
THINKOS=boot/debug/thinkos-0.20
THINKAPP=app/debug/yard-ice

${TOOLS_DIR}/tftp_reset.py -q -h ${JTAGTOOL_ADDR}

if [ $? == 0 ] ; then
	${TOOLS_DIR}/dcclog -h ${JTAGTOOL_ADDR} ${THINKOS}.elf ${THINKAPP}.elf
fi

