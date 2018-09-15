#!/bin/sh

TOOLS_DIR=../../tools
PYTHON=python
if [ -z "$JTAGTOOL_ADDR" ]; then
	JTAGTOOL_ADDR=192.168.0.51
fi
# Collect ".bin" files in the positional parameters
set -- `ls release/*.bin`
# Get the last one
for PROG_BIN; do true; done

${PYTHON} ${TOOLS_DIR}/tftp_load.py -q -i -e -r  -a 0x08020000 \
	-h ${JTAGTOOL_ADDR} ${PROG_BIN} 

