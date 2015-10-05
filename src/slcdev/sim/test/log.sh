#!/bin/sh

JTAGTOOL_ADDR=192.168.10.50
BUILD_NAME=slc-dev

../../../../tools/dcclog -h ${JTAGTOOL_ADDR} debug/${BUILD_NAME}.elf

