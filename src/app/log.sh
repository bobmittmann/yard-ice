#!/bin/sh

BUILD_NAME=yard-ice-0.32
JTAGTOOL_ADDR=192.168.0.51
TOOLS_DIR=../../thinkos/tools
THINKAPP_ELF=debug/${BUILD_NAME}.elf

${TOOLS_DIR}/dcclog -h ${JTAGTOOL_ADDR} ${THINKAPP_ELF}

