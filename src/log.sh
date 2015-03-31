#!/bin/sh

JTAGTOOL_ADDR=192.168.10.51
BUILD_NAME=yard-ice

../tools/tftp_reset.py -q -h ${JTAGTOOL_ADDR} 

if [ $? == 0 ] ; then
	../tools/dcclog -h ${JTAGTOOL_ADDR} debug/${BUILD_NAME}.elf
fi

