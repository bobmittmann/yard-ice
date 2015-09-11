#!/bin/sh

JTAGTOOL_ADDR=192.168.10.50
BUILD_NAME=tcp_test
TOOLS_DIR=../../../../tools

${TOOLS_DIR}/tftp_load.py -q -i -e -r -a 0x08010000 -h ${JTAGTOOL_ADDR} \
		release/${BUILD_NAME}.bin 

