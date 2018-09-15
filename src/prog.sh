#!/bin/sh

JTAGTOOL_ADDR=192.168.0.51
TOOLS_DIR=../thinkos/tools
THINKOS=boot/release/thinkos-0.21.bin
FPGARBF=fpga/release/yard_ice.rbf
THINKAPP=app/release/yard-ice-0.32.bin

${TOOLS_DIR}/tftp_load.py -q -i -e -a 0x08000000 -h ${JTAGTOOL_ADDR} ${THINKOS}
if [ $? == 0 ] ; then
	${TOOLS_DIR}/tftp_load.py -q -e -a 0x08010000 -h ${JTAGTOOL_ADDR} ${FPGARBF} 
	if [ $? == 0 ] ; then
		${TOOLS_DIR}/tftp_load.py -q -e -r -a 0x08020000 -h ${JTAGTOOL_ADDR} ${THINKAPP}
	fi
fi

