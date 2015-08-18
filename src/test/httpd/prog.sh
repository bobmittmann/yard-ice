#!/bin/sh

JTAGTOOL_ADDR=192.168.10.50
BUILD_NAME=httpd_test

../../../tools/tftp_load.py -q -i -e -r  -a 0x08000000 -h ${JTAGTOOL_ADDR} release/${BUILD_NAME}.bin 

