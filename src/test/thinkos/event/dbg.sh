#!/bin/sh

JTAGTOOL_ADDR=192.168.10.50
BUILD_NAME=ev_test

../../../../../tools/tftp_load.py -q -i -e -r  -a 0x08000000 -h ${JTAGTOOL_ADDR} debug/${BUILD_NAME}.bin 

if [ $? == 0 ] ; then
	../../../../../tools/dcclog -h ${JTAGTOOL_ADDR} debug/${BUILD_NAME}.elf
fi

