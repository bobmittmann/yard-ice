#!/bin/sh

JTAGTOOL_ADDR=192.168.10.50
BUILD_NAME=yard_ice-0.2

../../../tools/tftp_load.py -q -i -e -a 0x08060000 -h ${JTAGTOOL_ADDR} release/${BUILD_NAME}.rbf
