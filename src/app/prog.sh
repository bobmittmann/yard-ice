#!/bin/sh

JTAGTOOL_ADDR=192.168.10.50
BUILD_NAME=yard-ice

../../thinkos/tools/tftp_load.py -q -i -e -a 0x08020000 -h ${JTAGTOOL_ADDR} release/${BUILD_NAME}.bin 

